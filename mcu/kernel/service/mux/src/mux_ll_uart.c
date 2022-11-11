 /* Copyright Statement:
 *
 * (C) 2022  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include "mux.h"
#include "mux_port.h"
#include "mux_port_device.h"

#include "mux_ll_uart.h"
#include "mux_ll_uart_real_time.h"
#include "mux_ll_uart_wakeup.h"
#include "mux_ll_uart_sync.h"

#include "hal_resource_assignment.h"
#include "hal_pdma_internal.h"
#include "hal_uart_internal.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif/*HAL_SLEEP_MANAGER_ENABLED*/

#include "ept_gpio_var.h"

/* create mux debug module */
log_create_module(MUX_LL_PORT, PRINT_LEVEL_WARNING);

#ifdef MTK_CPU_NUMBER_0

ATTR_ZIDATA_IN_FAST_MEM  static volatile mux_port_setting_t g_mux_port_hw_setting;

#endif //MTK_CPU_NUMBER_0

extern uint32_t g_latch_phase_clock_diff;

mux_ll_user_context_t g_mux_ll_user_context[MAX_MUX_LL_USER_COUNT];

mux_ll_port_config_t g_port_config;

ATTR_SHARE_ZIDATA mux_ll_config_t mux_ll_config;
mux_ll_config_t *g_mux_ll_config = NULL;

#define MUX_LL_CONFIG_SHMEM_ADDRESS_VALUE *(volatile uint32_t *)HW_SYSRAM_PRIVATE_MEMORY_LLMUX_VAR_PORT_START
#define MUX_LL_CONFIG_SHMEM_POINTER (mux_ll_config_t *)(MUX_LL_CONFIG_SHMEM_ADDRESS_VALUE)

/* define mux handle magic symbol */
#define handle_to_port(handle) ((handle) & 0xFF)
#define handle_to_user_id(handle) (((handle) >> 8) & 0xFF)
#define user_id_to_handle(user_id, port) ((MUX_LL_HANDLE_MAGIC_NUMBER << 16) | (user_id << 8) | (port))

#ifdef MTK_CPU_NUMBER_0

ATTR_TEXT_IN_FAST_MEM static __FORCE_INLINE__ uint32_t mux_tx_vfifo(mux_ringbuffer_t *tx_vfifo, mux_ringbuffer_t *tx_ufifo, mux_ll_user_config_t *user_config, uint32_t *min_send_len)
{
    uint8_t head_buf[MUX_PACKAGE_HEAD_LEN];
    mux_ll_header_t *pheader = (mux_ll_header_t *)head_buf;
    uint32_t user_buffer_data_len = 0;
    uint32_t vfifo_free_len;
    uint32_t tmp_pkt_len;
    uint32_t real_pkt_len;
    uint32_t all_send_len = 0;
    uint32_t current_seq = 0;
    uint32_t tx_timestamp = 0;
#ifdef MUX_LL_UART_HEADER_CHECK_ENABLE
    uint8_t xor_check_sum = 0;
#endif
    uint16_t data_crc = 0;
    mux_user_tag_t *tag = &user_config->utag;
    mux_ll_user_context_t *user_context = &g_mux_ll_user_context[tag->uid];
    uint32_t local_duration = 0;

#if defined(MUX_LL_UART_TIMESTAMP_ENABLE)
    static uint32_t last_local_timestamp = 0;
    tx_timestamp = user_context->tx_timestamp;
#endif

    user_buffer_data_len = mux_ringbuffer_data_length(tx_ufifo);
    vfifo_free_len = mux_ringbuffer_free_space(tx_vfifo);

    if (user_buffer_data_len == 0) {
        RB_LOG_D("[mux_tx_vfifo] no data in ringbuffer[0x%x],uid=%d", 2, (uint32_t)tx_ufifo, tag->uid);
        return 0;
    }
    if (vfifo_free_len <= MUX_PACKAGE_HEAD_TAIL_LEN) {
        RB_LOG_I("[mux_tx_vfifo] uid=%d, vfifo has no enough free space!!! vd_len=%u", 2, tag->uid, vfifo_free_len);
        return 0;
    }
    tmp_pkt_len = RB_MIN(tag->tx_pkt_len, user_buffer_data_len);
    tmp_pkt_len = RB_MIN(tmp_pkt_len, vfifo_free_len - MUX_PACKAGE_HEAD_TAIL_LEN);
#if 0
    RB_LOG_I("[mux_tx_vfifo] tmp_pkt_len=%d user_data_len=%d tx_pkt_len=%d vfifo_free_len=%d tx_ufifo->flags=%x", 5, tmp_pkt_len, user_buffer_data_len, tag->tx_pkt_len, vfifo_free_len, tx_ufifo->flags);
    RB_LOG_I("[mux_tx_vfifo] MUX_ID=0x%x, scid=%d dcid=%d uid=%d", 4, MUX_ID(user_config->scid, user_config->dcid, tag->uid), user_config->scid, user_config->dcid, tag->uid);
#endif

    if (tx_ufifo->flags & MUX_RB_FLAG_DATA_4TO3) {//data 4->3, (tmp_pkt_len / 4 ) * 3
        /* tmp_pkt_len may be not aligned to 4 bytes. so, force to align to 4 bytes here. */
        tmp_pkt_len &= 0xFFFFFFFC;
        real_pkt_len = tmp_pkt_len >> 2;
        real_pkt_len = real_pkt_len + (real_pkt_len << 1);
    } else {
        real_pkt_len = tmp_pkt_len;
    }
    if (real_pkt_len == 0) {
        RB_LOG_W("[mux_tx_vfifo] uid=%d, data or vfifo not enough!!! ud_len=%d vd_len=%d", 3, tag->uid, user_buffer_data_len, mux_ringbuffer_data_length(tx_vfifo));
        return 0;
    }

    pheader->head = MUX_LL_UART_PKT_HEAD;
    pheader->id = MUX_ID(user_config->scid, user_config->dcid, tag->uid);
    pheader->length = real_pkt_len;

#ifdef MUX_LL_UART_TIMESTAMP_ENABLE
    pheader->timestamp = tx_timestamp;
    local_duration = mux_get_tick_elapse_with_end(last_local_timestamp, user_context->tx_local_timestamp);
#endif

#ifdef MUX_LL_UART_SEQ_ENABLE
    current_seq = user_context->sc_pkt_seq;
    user_context->current_seq = current_seq;
    pheader->seq = user_context->sc_pkt_seq++;
    RB_LOG_D("[mux_tx_vfifo] uid=%x seq=%d tag=0x%x", 3, tx_ufifo->uid, current_seq, (uint32_t)tag);
#endif

#ifdef MUX_LL_UART_HEADER_CHECK_ENABLE
    foreach_array(uint8_t, item, head_buf, MUX_PACKAGE_HEAD_LEN - 1,
        xor_check_sum ^= item;
    );
    pheader->xor_check_sum = xor_check_sum;
    RB_LOG_D("[mux_tx_vfifo] uid=%x check sum=0x%x", 2, tx_ufifo->uid, head_buf[5]);
    RB_LOG_D("%02X %02X %02X %02X %02X %02X ", 6, \
    head_buf[0], head_buf[1], head_buf[2], head_buf[3], head_buf[4], head_buf[5]);
#endif

#ifdef RB_WATER_MARK_ENABLE
    uint32_t total_data_len = mux_ringbuffer_data_length(tx_vfifo);
    if (total_data_len > tx_vfifo->water_mark) {
      tx_vfifo->water_mark = total_data_len;
    }

    RB_LOG_I("[mux_tx_vfifo] uid[%x] seq=%d prio=%d rs_len=%d pkt_len=%d, ud_len=%d vd_len=%d uwm=%d vwm=%d timestamp=%d:%d LTD:%d", 12,\
        tx_ufifo->uid, current_seq, tag->tx_priority, real_pkt_len, tag->tx_pkt_len, user_buffer_data_len, total_data_len, \
        tx_ufifo->water_mark, tx_vfifo->water_mark, tx_timestamp, user_context->tx_local_timestamp, local_duration);
#else
    RB_LOG_I("[mux_tx_vfifo] uid[%x] seq=%d prio=%d rs_len=%d pkt_len=%d, ud_len=%d vd_len=%d timestamp=%d:%d LDT:%d", 10,\
        tx_ufifo->uid, current_seq, tag->tx_priority, real_pkt_len, tag->tx_pkt_len, user_buffer_data_len, mux_ringbuffer_data_length(tx_vfifo), tx_timestamp, user_context->tx_timestamp, local_duration);
#endif

#ifdef MUX_LL_UART_TIMESTAMP_ENABLE
    last_local_timestamp = user_context->tx_local_timestamp;
#endif

    //TODO: do not push data if free space size less that one package size, query remaining space int vfifo
    all_send_len = mux_ringbuffer_write_st(tx_vfifo, head_buf, MUX_PACKAGE_HEAD_LEN);
    all_send_len += mux_ringbuffer_write_buffer_st(tx_ufifo, tx_vfifo, tmp_pkt_len, &data_crc);
#ifdef MUX_LL_UART_CRC_CHECK_ENABLE
    all_send_len += mux_ringbuffer_write_st(tx_vfifo, (uint8_t*)&data_crc, MUX_DATA_CHECK_LEN);
#endif
    mux_ringbuffer_write_move_hw_tail_st(tx_vfifo, real_pkt_len + MUX_PACKAGE_HEAD_TAIL_LEN);
#ifdef LLUART_REAL_TIME_MEASUREMENT
    lluart_busy_time_measurement_action(LLUART_ACTION_TX_LIMTER);
#endif

    *min_send_len = real_pkt_len;
    return all_send_len;
}


ATTR_TEXT_IN_FAST_MEM static __FORCE_INLINE__ void mux_tx_limiter(void)
{
    mux_ll_port_config_t * p_port_config = &g_port_config;
    mux_user_fifo_node_t * high_prio_head = p_port_config->user_fifo_list_head[MUX_LL_UART_PRIORITY_HIGH];
    mux_user_fifo_node_t * low_prio_head = p_port_config->user_fifo_list_head[MUX_LL_UART_PRIORITY_LOW];
    mux_user_fifo_node_t * uart_latch_head = p_port_config->user_fifo_list_head[MUX_LL_UART_PRIORITY_ABOVE_HIGH];

    mux_user_fifo_node_t * node;
    uint32_t all_send_len = 0;
    uint32_t min_send_len = 0;
    uint8_t latch_uid = 0;
    mux_user_tag_t *tag;

    mux_ringbuffer_t *tx_vfifo = &p_port_config->txbuf;

#ifdef AIR_LL_MUX_WAKEUP_ENABLE
    mux_ll_uart_wakeup_sync();
#endif

#ifdef LLUART_REAL_TIME_MEASUREMENT
        uart_busy_rate_timer_start();
       // cpu_mips_measurement_timer_start();
#endif

    foreach_node(node, uart_latch_head) {
        tag = &node->p_user_config->utag;
        latch_uid = tag->uid;
        if (mux_ringbuffer_data_length(&node->p_user_config->txbuf) > 0) {
#ifdef MUX_LL_UART_TIMESTAMP_ENABLE
            g_mux_ll_user_context[tag->uid].tx_timestamp = DCHS_bt_get_native_clock_us();
            g_mux_ll_user_context[tag->uid].tx_local_timestamp = mux_get_tick_count();
#endif
            RB_LOG_D("[mux_tx_limiter] Pri[%d] uid=%d tx_pkt_len=%d", 3, tag->tx_priority, tag->uid, tag->tx_pkt_len);
            all_send_len += mux_tx_vfifo(tx_vfifo, &node->p_user_config->txbuf, node->p_user_config, &min_send_len);
        }
    }

    //If someone requests latch uart, only highest fifo can send data through uart vfifo
    if (!DCHS_IS_UART_TX_LOCKED()) {

        // polling high priority user fifo
        foreach_node(node, high_prio_head) {
            tag = &node->p_user_config->utag;
            if (mux_ringbuffer_data_length(&node->p_user_config->txbuf) > 0) {
                RB_LOG_D("[mux_tx_limiter] Pri[%d] uid=%d tx_pkt_len=%d", 3, tag->tx_priority, tag->uid, tag->tx_pkt_len);
#ifdef MUX_LL_UART_TIMESTAMP_ENABLE
                g_mux_ll_user_context[tag->uid].tx_timestamp = DCHS_bt_get_native_clock_us();
                g_mux_ll_user_context[tag->uid].tx_local_timestamp = mux_get_tick_count();
#endif
                all_send_len += mux_tx_vfifo(tx_vfifo, &node->p_user_config->txbuf, node->p_user_config, &min_send_len);
            }
        }

        //no data in high priority fifo
        if (all_send_len == 0) {
            foreach_node(node, low_prio_head) {
                tag = &node->p_user_config->utag;
                if (mux_ringbuffer_data_length(&node->p_user_config->txbuf) > 0) {
                    RB_LOG_D("[mux_tx_limiter] Pri[%d] uid=%d tx_pkt_len=%d", 3, tag->tx_priority, tag->uid, tag->tx_pkt_len);
#ifdef MUX_LL_UART_TIMESTAMP_ENABLE
                    g_mux_ll_user_context[tag->uid].tx_timestamp = DCHS_bt_get_native_clock_us();
                    g_mux_ll_user_context[tag->uid].tx_local_timestamp = mux_get_tick_count();
#endif
                    all_send_len += mux_tx_vfifo(tx_vfifo, &node->p_user_config->txbuf, node->p_user_config, &min_send_len);
                }
            }
        }
    } else {
        if ((MUX_GET_TX_FLAGS() & 0xFF00) != (latch_uid << 8)) {
            RB_LOG_W("[mux_tx_limiter] UART is in latch mode, can not send user data, tx_flags=0x%x", 1, MUX_GET_TX_FLAGS());
        }
    }
    if (all_send_len == 0) {
        if (vdma_disable_interrupt(VDMA_UART1TX) != VDMA_OK) {
            assert(0);
            return;
        }
    } else {
        //enable tx dma irq
        if (vdma_enable_interrupt(VDMA_UART1TX) != VDMA_OK) {
            assert(0);
            return;
        }
    }
}

ATTR_TEXT_IN_FAST_MEM uint32_t peek_buffer_data_and_parse_first_package (
    mux_ringbuffer_t *rb, uint32_t *p_consume_len, uint32_t total_rx_left_data_size, uint8_t *duid, uint8_t *dcid)
{
    uint32_t pkt_len = 0;
    uint32_t drop_len = 0;
    uint8_t seq = 0;
    uint8_t id;
    uint8_t header[MUX_PACKAGE_HEAD_LEN];
    // uint8_t scid;
    uint8_t head;
    mux_ll_header_t *p_header = (mux_ll_header_t*)&header;
    static uint32_t last_r_addr = 0;
    if (last_r_addr == 0) {
        last_r_addr = (uint32_t)RB_START_ADDRESS(rb);
    }
#if defined(MUX_LL_UART_SEQ_ENABLE) || defined(MUX_LL_UART_TIMESTAMP_ENABLE)
    mux_ll_user_context_t *user_context;
#endif
#ifdef MUX_LL_UART_HEADER_CHECK_ENABLE
    uint32_t xor_check_sum = 0;
    uint8_t *pdata;
#endif
    static uint32_t fetch_pkt_count = 0;
    fetch_pkt_count++;
    *duid = 0xff;
    *dcid = 0xff;
    *p_consume_len = 0;
    head = RB_FETCH_BYTE_DATA(rb, 0);

    /*max pkt len is 320 bytes, it cost at most 320 bytes time in while loop.*/
    if (head != MUX_LL_UART_PKT_HEAD) {
        RB_LOG_E("[peek first pkt] Header error start addr=0x%x val=0x%x fetch_pkt_count=%d vd_len=%d:0x%x", 5, \
            (uint32_t)RB_FETCH_BYTE_ADDRESS(rb, 0), head, fetch_pkt_count, total_rx_left_data_size, *(volatile uint32_t*)0x40090238);
        do {
            drop_len++;
            head = RB_FETCH_BYTE_DATA(rb, drop_len);
            if (drop_len >= total_rx_left_data_size) {
                break;
            }
        } while (head != MUX_LL_UART_PKT_HEAD);

        *p_consume_len = drop_len;
        RB_LOG_E("[peek first pkt] Header error end address=0x%x val=0x%x drop_len=%d", 3, \
            (uint32_t)RB_FETCH_BYTE_ADDRESS(rb, drop_len), head, drop_len);
        // hexdump(RB_FETCH_BYTE_ADDRESS(rb, 0), drop_len);
        return 0;
    }

    header[0] = head;
    foreach_index(idx, 1, MUX_PACKAGE_HEAD_LEN) {
        header[idx] = RB_FETCH_BYTE_DATA(rb, idx);
    }
#ifdef MUX_LL_UART_HEADER_CHECK_ENABLE
    pdata = header;
    foreach_array(uint8_t, item, pdata, MUX_PACKAGE_HEAD_LEN - 1,
        xor_check_sum ^= item;
    );

    if (xor_check_sum != p_header->xor_check_sum) {
        *p_consume_len = MUX_PACKAGE_HEAD_LEN;
        RB_LOG_E("[peek first pkt] error!!! addr=0x%x:0x%x last_addr=0x%x Header check sum error 0x%x, drop header", 4, \
            RB_FETCH_BYTE_ADDRESS(rb, 0), *(volatile uint32_t*)0x40090234, last_r_addr, xor_check_sum);
        foreach_index(idx, 0, MUX_PACKAGE_HEAD_LEN + 16) {
           RB_LOG_E("pdata[%d] = 0x%x", 2, idx, RB_FETCH_BYTE_DATA(rb, idx));
        }
        //assert(0);
        return 0;
    }
#endif

    id = p_header->id;
    *dcid = MUX_GET_DEST_CORE_ID(id);
    *duid = MUX_GET_DEST_USER_ID(id);
    if ((*dcid > 1) || ((*duid >= MAX_MUX_LL_USER_COUNT))) {
        RB_LOG_E("[peek first pkt] ID[0x%x] error,dcid=%d duid=%d", 3, id, dcid, duid);
        *p_consume_len = 1;
        return 0;
    }
    pkt_len = p_header->length;
#if defined(MUX_LL_UART_SEQ_ENABLE) || defined(MUX_LL_UART_TIMESTAMP_ENABLE)
    user_context = &g_mux_ll_user_context[*duid];
#endif
#if defined(MUX_LL_UART_TIMESTAMP_ENABLE)
    user_context->rx_timestamp = p_header->timestamp;
#endif
#ifdef MUX_LL_UART_SEQ_ENABLE
    seq = p_header->seq;
#endif

    RB_LOG_I("[peek first pkt] uid[%02x] seq=%d pkt_len=%d+%d vd_len=%d addr=0x%x", 6, \
        *duid, seq, pkt_len, MUX_PACKAGE_HEAD_TAIL_LEN,total_rx_left_data_size, (uint32_t)RB_FETCH_BYTE_ADDRESS(rb, 0));
    //TODO: check hw rptr if match with sw rptr
    /*
    if (*(volatile uint32_t*)0x40090234 != ((uint32_t)RB_FETCH_BYTE_ADDRESS(rb, 0) & 0x0FFFFFFF)) {
        assert(0);
    }
    if (pkt_len > 320) {
        mux_ringbuffer_hexdump(rb, true);
        assert(0);
    }
    */
    if (total_rx_left_data_size < (pkt_len + MUX_PACKAGE_HEAD_TAIL_LEN)) {
        return 0;
    }
#ifdef MUX_LL_UART_SEQ_ENABLE
    user_context = &g_mux_ll_user_context[*duid];
    /* For Tx reset, Rx not reset */
    if ((seq == 0) && (user_context->dc_pkt_seq != 0)) {
        user_context->dc_pkt_seq = 0;
        RB_LOG_W("[peek first pkt] TX reset but RX not reset", 0);
    }
    /* For Rx reset, Tx not reset */
    if ((user_context->dc_pkt_seq == 0) && (seq != 0)) {
        user_context->dc_pkt_seq = seq;
        RB_LOG_W("[peek first pkt] RX reset but TX not reset", 0);
    }
    if (seq != user_context->dc_pkt_seq) {
        RB_LOG_E("[peek first pkt] recv_seq=%d, expect_seq=%d", 2, seq, user_context->dc_pkt_seq);
        hexdump(header, MUX_PACKAGE_HEAD_LEN);
        assert(0 && "seq error");
    }
    user_context->current_seq = seq;
    user_context->dc_pkt_seq++;
#endif
    last_r_addr = (uint32_t)RB_FETCH_BYTE_ADDRESS(rb, 0);
    return pkt_len; // peek real len
}


bool mux_pop_one_pkt_from_rx_fifo(mux_ll_user_config_t *user_config, uint8_t *read_buf, uint32_t read_len, uint32_t *real_read_len)
{
    uint8_t *src_addr;
    uint32_t src_len;
    uint32_t per_cpu_irq_mask;
    buffer_fifo_t *buffer_fifo = &user_config->buffer_fifo;

    src_addr = buffer_fifo->p_data[buffer_fifo->rptr];
    src_len = buffer_fifo->data_size[buffer_fifo->rptr];
    if (read_len < src_len) {
        RB_LOG_E("POP FIFO: uid:%u, pop buffer fail, drop_count:%u, wptr:%u rptr:%u\r\n", 4, user_config->utag.uid, buffer_fifo->drop_count, buffer_fifo->wptr, buffer_fifo->rptr);
        return false;
    }

    memcpy(read_buf, src_addr, src_len);
    *real_read_len = src_len;
    port_mux_free(buffer_fifo->p_data[buffer_fifo->rptr]);
    buffer_fifo->p_data[buffer_fifo->rptr] = 0;
    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
    buffer_fifo->rptr++;
    buffer_fifo->rptr %= MAX_MUX_LL_USER_RX_DATA_CNT;
    if (buffer_fifo->wptr == buffer_fifo->rptr) {
        buffer_fifo->is_empty = true;
    }
    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);
    RB_LOG_I("POP FIFO: *real_read_len:%u, >rptr:%u\r\n", 2, *real_read_len, buffer_fifo->rptr);
    return true;
}

bool mux_push_one_pkt_to_rx_fifo(mux_ll_user_config_t *user_config, mux_ringbuffer_t *rb, uint32_t new_pkt_len)
{
    uint8_t *p_malloc_buff_addr;
    buffer_fifo_t *buffer_fifo = &user_config->buffer_fifo;
    if ((buffer_fifo->wptr == buffer_fifo->rptr) && (buffer_fifo->is_empty == false)) {
        //push fail
        buffer_fifo->drop_count ++;
        RB_LOG_E("PUSH FIFO: uid:%u, pop buffer fail, drop_count:%u, wptr:%u rptr:%u\r\n", 4, user_config->utag.uid, buffer_fifo->drop_count, buffer_fifo->wptr, buffer_fifo->rptr);
        return false;
    } else {
        p_malloc_buff_addr = port_mux_malloc(new_pkt_len);
        if (p_malloc_buff_addr == NULL) {
            assert(0 && "malloc fail");
        }
        buffer_fifo->p_data[buffer_fifo->wptr] = p_malloc_buff_addr;
        buffer_fifo->data_size[buffer_fifo->wptr] = new_pkt_len;
        buffer_fifo->wptr++;
        buffer_fifo->wptr %= MAX_MUX_LL_USER_RX_DATA_CNT;
        buffer_fifo->is_empty = false;

        mux_ringbuffer_read(rb, p_malloc_buff_addr, new_pkt_len);
        RB_LOG_I("PUSH FIFO: new_pkt_len:%u, wptr:%u\r\n", 2, new_pkt_len, buffer_fifo->wptr);
        return true;
    }
}
#ifdef MUX_LL_UART_CRC_CHECK_ENABLE
mux_ringbuffer_t vfifo_rxrb_tmp;
#endif
ATTR_TEXT_IN_FAST_MEM static __FORCE_INLINE__ void mux_rx_deliver(void)
{
    uint32_t total_rx_left_data_size;
    uint32_t write_done_len = 0;
    uint8_t duid,dcid;

    mux_ringbuffer_t *rx_ufifo = NULL;
    mux_ringbuffer_t *vfifo_rxrb = (mux_ringbuffer_t *)&g_port_config.rxbuf;
    mux_ll_user_config_t *user_config;
    mux_ll_user_context_t *user_context;
    mux_handle_t handle;
    mux_protocol_t *mux_protocol;
    mux_buffer_t buffers[2];
    static uint32_t last_local_end_count = 0;
    uint32_t consume_len;
    uint32_t new_pkt_len;
    uint32_t full_pkt_len;
    uint32_t buffer_count;
    uint32_t end_count = 0;
    uint32_t local_end_count = 0;
    uint32_t rx_timestamp = 0;
    uint32_t duration = 0;
    uint32_t local_duration = 0;
    uint32_t port = 0;
    uint16_t data_crc = 0;
    uint8_t flags;

#ifdef MUX_LL_UART_CRC_CHECK_ENABLE
    uint16_t recved_data_crc = 0;
#endif
    while(1) {
#ifdef AIR_LL_MUX_WAKEUP_ENABLE
    mux_ll_uart_reset_lock_sleep_timer();
#endif
        total_rx_left_data_size = mux_ringbuffer_data_length(vfifo_rxrb);
#ifdef RB_WATER_MARK_ENABLE
        if (total_rx_left_data_size > vfifo_rxrb->water_mark) {
            vfifo_rxrb->water_mark = total_rx_left_data_size;
        }
#endif
        RB_LOG_D("[mux_rx_deliver] MUX_EVENT_READY_TO_READ  %d bytes data in HW FIFO=0x%x", 2, total_rx_left_data_size, (uint32_t)vfifo_rxrb);
        if (total_rx_left_data_size <= MUX_PACKAGE_HEAD_LEN )
            break;
        new_pkt_len = peek_buffer_data_and_parse_first_package(vfifo_rxrb, &consume_len, total_rx_left_data_size, &duid, &dcid);
        if (consume_len != 0) {
            mux_ringbuffer_read_move_ht_st(vfifo_rxrb, consume_len);
            mux_ringbuffer_read_move_hw_tail_ht_st(vfifo_rxrb, consume_len);//drop header
            break;
        }
        full_pkt_len = new_pkt_len + MUX_PACKAGE_HEAD_TAIL_LEN;
        if (new_pkt_len == 0) {
            //TODO:: need remind a risk: current Rx buffer data size > Rx threshold , UART IRQ will can not clear!!!!
            // RB_LOG_W("[mux_rx_deliver] no pkt, no csum, vf_data_len=%d", 1, total_rx_left_data_size);
            break;
        }

        /* move HW FIFO SW write pointer, so, user can know how many data in HW FIFO by SW pointer. */
        /* FIXME: If user do not deal with all total_rx_left_data_size data at the following procedure,
           the data leng in vfifo_rxrb must be wrong, data length will larger than real data length.
        */
        mux_ringbuffer_write_move_ht_st(vfifo_rxrb, full_pkt_len);
        if (duid >= MAX_MUX_LL_USER_COUNT){
            RB_LOG_E("[mux_rx_deliver] duid=%d out of range", 1, duid);
            assert(0 && "duid is out of range");
        }
#ifdef MUX_LL_UART_CRC_CHECK_ENABLE
        memcpy(&vfifo_rxrb_tmp, vfifo_rxrb, sizeof(mux_ringbuffer_t));
#endif
        user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[duid];
        //TODO: check rx_ufifo if vaild??
        rx_ufifo = &user_config->rxbuf;

#ifdef MUX_LL_UART_CRC_CHECK_ENABLE
        recved_data_crc = *(uint8_t*)RB_FETCH_BYTE_ADDRESS(vfifo_rxrb, new_pkt_len + MUX_PACKAGE_HEAD_LEN);
        recved_data_crc |= (*(uint8_t*)RB_FETCH_BYTE_ADDRESS(vfifo_rxrb, new_pkt_len + MUX_PACKAGE_HEAD_LEN + 1)) << 8;
#endif
        mux_ringbuffer_read_move_ht_st(vfifo_rxrb, MUX_PACKAGE_HEAD_LEN);//drop header
        // mux_ringbuffer_hexdump(rx_ufifo, false);
        //user fifo has not enough free sapce, just drop or assert!!!
        if(new_pkt_len > mux_ringbuffer_free_space(rx_ufifo)){
            mux_ringbuffer_hexdump(rx_ufifo, false);
            assert(0 && "user rx buffer is almost full, please check uid to find the user. 4:DL, 5:UL");
            // mux_ringbuffer_read_move_ht_st(vfifo_rxrb, new_pkt_len);
            // mux_ringbuffer_read_move_hw_tail_ht_st(vfifo_rxrb, new_pkt_len);
        } else {
            // receive new_pkt_len bytes data in vfifo, move SW wptr
            // mux_ringbuffer_write_move_ht_st(vfifo_rxrb, new_pkt_len);
            write_done_len = mux_ringbuffer_write_buffer_st(vfifo_rxrb, rx_ufifo, new_pkt_len, &data_crc);
            if (rx_ufifo->flags & MUX_RB_FLAG_DATA_3TO4) {
                uint32_t tmp_len = write_done_len >> 2;
                tmp_len = tmp_len + (tmp_len << 1);
                if (new_pkt_len != tmp_len) {
                // if (new_pkt_len != mux_ringbuffer_write_try(rx_ufifo, pbuf, new_pkt_len)) {
                    //user fifo have not enough free space to contain the received packet, drop the received packet
                    RB_LOG_E("[mux_rx_deliver] 3->4 copy Error!!!,write_done_len:%d,new_pkt_len:%d", 2, write_done_len, new_pkt_len);
                    mux_ringbuffer_hexdump(rx_ufifo, false);
                    mux_ringbuffer_read_move_ht_st(vfifo_rxrb, new_pkt_len);
                    mux_ringbuffer_read_move_hw_tail_ht_st(vfifo_rxrb, new_pkt_len);
                    assert(0 && "user rx buffer is almost full, please check uid to find the user. 4:DL, 5:UL");
                }
            } else {
                if (new_pkt_len != write_done_len) {
                // if (new_pkt_len != mux_ringbuffer_write_try(rx_ufifo, pbuf, new_pkt_len)) {
                    //user fifo have not enough free space to contain the received packet, drop the received packet
                    RB_LOG_E("[mux_rx_deliver] copy Error!!!,write_done_len:%d,new_pkt_len:%d", 2, write_done_len, new_pkt_len);
                    mux_ringbuffer_read_move_ht_st(vfifo_rxrb, new_pkt_len);
                    mux_ringbuffer_read_move_hw_tail_ht_st(vfifo_rxrb, new_pkt_len);
                    assert(0 && "user rx buffer is almost full, please check uid to find the user. 4:DL, 5:UL");
                }
            }
#ifdef MUX_LL_UART_CRC_CHECK_ENABLE
            if (recved_data_crc != data_crc) {
                RB_LOG_E("[mux_rx_deliver] recved_data_crc:%x,data_crc:%x", 2, recved_data_crc, data_crc);
                mux_ringbuffer_hexdump(&vfifo_rxrb_tmp, false);
                mux_ringbuffer_hexdump(vfifo_rxrb, false);
                mux_ringbuffer_hexdump(rx_ufifo, false);
                assert(0 && "data crc check fail");
            }
#endif
        }
#ifdef MUX_LL_UART_CRC_CHECK_ENABLE
        // move software read pointer
        mux_ringbuffer_read_move_ht_st(vfifo_rxrb, MUX_DATA_CHECK_LEN);//drop data check value
#endif
        // move hardware read pointer
        mux_ringbuffer_read_move_hw_tail_ht_st(vfifo_rxrb, full_pkt_len);
        user_context = &g_mux_ll_user_context[duid];
        flags = user_config->utag.flags;
        if (!(flags & MUX_LL_UART_ATTR_USER_RX_POLLING)) {
            //CM33 should notify DSP0 when one packet data receive done
            if (dcid == HAL_CORE_DSP0) {
                hal_ccni_status_t ret;
                uint32_t msg_array[2];

                /*  cm33 send ccni to dsp  */
                ret = hal_ccni_set_event(CCNI_CM4_TO_DSP0_LL_UART, (hal_ccni_message_t*)msg_array);
                if (HAL_CCNI_STATUS_OK != ret) {
                    RB_LOG_W("cm33 ccni send error. return = %d", 1, ret);
                } else {
                    RB_LOG_D("cm33 ccni send ok", 0);
                }
            } else {
                if (user_config->callback != NULL) {
                    port = (uint32_t)vfifo_rxrb->port_idx + MUX_LL_UART_0;
                    if (flags & MUX_LL_UART_ATTR_USER_TRX_PROTOCOL) {
                        mux_protocol = &g_port_config.protocol_cpu;
                        if (RB_IS_DATA_SEGMENTED(rx_ufifo)) {
                            buffers[0].p_buf = (uint8_t *)RB_CONTIGUOUS_DATA_START_ADDR(rx_ufifo);
                            buffers[0].buf_size = rx_ufifo->capacity - RB_PHY_RPTR_HEAD(rx_ufifo);
                            buffers[1].p_buf = (uint8_t *)MUX_RB_SWITCH_START_MEMVIEW(rx_ufifo);
                            buffers[1].buf_size = RB_TOTAL_DATA_SIZE(rx_ufifo) - buffers[0].buf_size;
                            buffer_count = 2;
                            RB_LOG_D("[mux_rx_deliver] 2 RX buffer! hw_rptr=%d, total_size=%d\r\n", 2, (int)RB_PHY_SW_RPTR_HEAD(rx_ufifo), (int)RB_TOTAL_DATA_SIZE(rx_ufifo));
                        } else {
                            buffers[0].p_buf = (uint8_t *)RB_CONTIGUOUS_DATA_START_ADDR(rx_ufifo);
                            buffers[0].buf_size = RB_TOTAL_DATA_SIZE(rx_ufifo);
                            buffers[1].p_buf = NULL;
                            buffers[1].buf_size = 0;
                            buffer_count = 1;
                        }
                        consume_len = 0;
                        new_pkt_len = 0;

                        if (mux_protocol->rx_protocol_callback) {
                            mux_protocol->rx_protocol_callback(&handle, buffers, buffer_count, &consume_len, &new_pkt_len, mux_protocol->user_data);
                        }
                        if (consume_len != 0) {
                            mux_ringbuffer_read_move_ht_st(rx_ufifo, consume_len);//drop data
                        }
                        if (new_pkt_len == 0) {
                            RB_LOG_W("MCU continue while loop uid=%u handle:0x%x, new:%u, cmus:%u, left:%u", 5, rx_ufifo->uid, handle, new_pkt_len, consume_len, RB_TOTAL_DATA_SIZE(rx_ufifo));
                            continue;
                        }

                        if ((consume_len + new_pkt_len) > RB_TOTAL_DATA_SIZE(rx_ufifo)) {
                            RB_LOG_W("The packet not receive done, uid=%u handle:0x%x new:%u cmus:%u left:%u", 5, rx_ufifo->uid, handle, new_pkt_len, consume_len, RB_TOTAL_DATA_SIZE(rx_ufifo));
                            continue;
                        }

                        if ((handle_to_port(handle) != port) && (new_pkt_len != 0)) {
                            RB_LOG_E("Mux error: port dismatch with handle, port:%u handle:0x%x consume_len:%u new_pkt_len:%u rx_receive:%u", 5,
                                 (unsigned int)port, (unsigned int)handle, (unsigned int)consume_len, (unsigned int)new_pkt_len, RB_TOTAL_DATA_SIZE(rx_ufifo));
                            mux_ringbuffer_read_move_ht_st(rx_ufifo, RB_TOTAL_DATA_SIZE(rx_ufifo));
                            continue;
                        }
                        if (mux_push_one_pkt_to_rx_fifo(user_config, rx_ufifo, new_pkt_len)) {
                            user_config->callback(handle, MUX_EVENT_READY_TO_READ, new_pkt_len, user_config->user_data);
                        } else {
                            mux_ringbuffer_read_move_ht_st(rx_ufifo, new_pkt_len);
                            RB_LOG_E("Mux error: port push rx buffer fail, uid=%u  new_pkt_len:%u", 2, rx_ufifo->uid, new_pkt_len);
                        }
                    } else {
                        //set need_rx_callback as flase every time before call ready to read callback
                        user_config->callback(user_id_to_handle(duid, port), MUX_EVENT_READY_TO_READ, write_done_len, user_config->user_data);
                    }
                }
            }
        }
    #ifdef MUX_LL_UART_TIMESTAMP_ENABLE
        end_count = DCHS_bt_get_native_clock_us();
        local_end_count = mux_get_tick_count();
        rx_timestamp = user_context->rx_timestamp;
        duration = DCHS_bt_get_native_clock_us_duration(rx_timestamp, end_count);
        local_duration = mux_get_tick_elapse_with_end(last_local_end_count, local_end_count);
        last_local_end_count = local_end_count;
    #endif

    #ifdef RB_WATER_MARK_ENABLE
        RB_LOG_I("[mux_rx_deliver] uid[%02x] %4dB rx done, seq=%3d uwm=%d vwm=%d vd_len=%d LTD:%4d RX_TS:%d END_TS:%d:%d TD:%6d=%d-%d", 13, \
        duid, write_done_len, user_context->current_seq, rx_ufifo->water_mark, vfifo_rxrb->water_mark, mux_ringbuffer_data_length(vfifo_rxrb), local_duration, rx_timestamp, end_count, local_end_count, (int)duration - (int)g_latch_phase_clock_diff, (int)duration, (int)g_latch_phase_clock_diff);
    #else
        RB_LOG_I("[mux_rx_deliver] uid[%02x] %4dB rx done, seq=%3d vd_len=%d LTD:%4d RX_TS:%d END_TS:%d:%d TD:%6d=%d-%d", 10, duid, write_done_len, user_context->current_seq,\
        mux_ringbuffer_data_length(vfifo_rxrb), local_duration, rx_timestamp, end_count, local_end_count, (int)duration - (int)g_latch_phase_clock_diff, (int)duration, (int)g_latch_phase_clock_diff);
    #endif
    }
}

ATTR_TEXT_IN_FAST_MEM void mux_tx_limiter_sw_irq_isr(hal_nvic_irq_t irq_number)
{
#ifdef LLUART_REAL_TIME_MEASUREMENT
    cpu_mips_measurement_start();
    mux_tx_limiter();
    cpu_mips_measurement_end(true);
#else
    mux_tx_limiter();
#endif
}

ATTR_TEXT_IN_FAST_MEM  uint32_t mux_uart_rb_get_rptr(struct mux_ringbuffer *rb)
{
    // RB_LOG_I("[mux_uart_rb_get_rptr]rb->port_idx=%d isRX=%d", 2, (uint32_t)rb->port_idx, RB_IS_RXBUF(rb));

    return uart_get_hw_rptr((uint32_t)rb->port_idx, RB_IS_RXBUF(rb));
}

ATTR_TEXT_IN_FAST_MEM  uint32_t mux_uart_rb_get_wptr(struct mux_ringbuffer *rb)
{
    // RB_LOG_I("[mux_uart_rb_get_wptr]rb->port_idx=%d isRX=%d", 2, (uint32_t)rb->port_idx, RB_IS_RXBUF(rb));
    return uart_get_hw_wptr((uint32_t)rb->port_idx, RB_IS_RXBUF(rb));
}

ATTR_TEXT_IN_FAST_MEM  void mux_uart_rb_set_tx_wptr(struct mux_ringbuffer *rb, uint32_t move_bytes)
{
#if 0
    static uint32_t send_len = 0;
    static uint32_t drop_head_len = 0;
    send_len += move_bytes;
    drop_head_len += (move_bytes - MUX_PACKAGE_HEAD_TAIL_LEN);
#endif
    uart_set_sw_move_byte((uint32_t)rb->port_idx, 0, move_bytes);
#if 0
    RB_LOG_I("[tx_wptr]%d send_len=%d drop_head_len=%d move_bytes=%d addr=0x%x:0x%x", 6, \
        (uint32_t)rb->port_idx, send_len, drop_head_len, move_bytes, RB_WRITE_BYTE_ADDRESS(rb, 0), *(volatile uint32_t*)0x40090130);
#endif

}

ATTR_TEXT_IN_FAST_MEM  void mux_uart_rb_set_rx_rptr(struct mux_ringbuffer *rb, uint32_t move_bytes)
{
#if 0
    static uint32_t read_len = 0;
    static uint32_t drop_head_len = 0;
    read_len += move_bytes;
    drop_head_len += (move_bytes - MUX_PACKAGE_HEAD_TAIL_LEN);
#endif
    uart_set_sw_move_byte((uint32_t)rb->port_idx, 1, move_bytes);
#if 0
    RB_LOG_I("[rx_rptr]%d read_len=%d drop_head_len=%d move_bytes=%d addr=0x%x:0x%x", 6, \
        (uint32_t)rb->port_idx, read_len, drop_head_len, move_bytes, RB_FETCH_BYTE_ADDRESS(rb, 0), *(volatile uint32_t*)0x40090234);
#endif
}

ATTR_TEXT_IN_FAST_MEM  bool mux_uart_rb_full(struct mux_ringbuffer *rb)
{
    return uart_get_buf_full_status((uint32_t)rb->port_idx, RB_IS_RXBUF(rb));
}

ATTR_TEXT_IN_FAST_MEM  bool mux_uart_rb_empty(struct mux_ringbuffer *rb)
{
    typedef bool (*uart_query_trx_empty_t)(hal_uart_port_t uart_port);
    uart_query_trx_empty_t uart_query_trx_empty[2] = {uart_query_tx_empty, uart_query_rx_empty};

    return uart_query_trx_empty[RB_TRXBUF_IDX(rb)]((uint32_t)rb->port_idx);
}

ATTR_TEXT_IN_FAST_MEM  uint32_t mux_uart_rb_data_size(struct mux_ringbuffer *rb)
{
    vdma_channel_t vdma_channel[2] = {VDMA_UART1TX, VDMA_UART1RX};
    uint32_t data_size = 0;

    vdma_get_available_receive_bytes(vdma_channel[RB_TRXBUF_IDX(rb)], &data_size);

    return data_size;
}

ATTR_TEXT_IN_FAST_MEM  uint32_t mux_uart_rb_free_space(struct mux_ringbuffer *rb)
{
    vdma_channel_t vdma_channel[2] = {VDMA_UART1TX, VDMA_UART1RX};
    uint32_t free_space = 0;

    vdma_get_available_send_space(vdma_channel[RB_TRXBUF_IDX(rb)], &free_space);

    return free_space;
}

mux_ringbuffer_ops_t mux_uart_ops = {
    .rb_get_rptr = mux_uart_rb_get_rptr,
    .rb_get_wptr = mux_uart_rb_get_wptr,
    .rb_set_tx_wptr = mux_uart_rb_set_tx_wptr,
    .rb_set_rx_rptr = mux_uart_rb_set_rx_rptr,
    .rb_full = mux_uart_rb_full,
    .rb_empty = mux_uart_rb_empty,
    .rb_data_size = mux_uart_rb_data_size,
    .rb_free_space = mux_uart_rb_free_space,
};

#ifdef MTK_CPU_NUMBER_0

static void mux_init_ll_uart_buffer(uint32_t port_index)
{
    uint32_t i;
    mux_ll_user_config_t *p_user_config;
    mux_ll_port_config_t *p_port_config = &g_port_config;
    mux_ll_uart_user_configure_t *ll_uart_config;
    mux_user_fifo_node_t *node;
    mux_user_fifo_node_t **fifo_list_head;
    mux_user_fifo_node_t *new_fifo_node;
    mux_user_tag_t *utag;
    uint32_t txflags;
    uint32_t rxflags;
    uint32_t rl_tx_pkt_len = 0;
    uint32_t max_tx_pkt_len = 0;
    uint32_t min_tx_pkt_len = 0xFFFFFFFF;

    #define RB_SET_UID_PID(p_user_config, _uid, pid)\
    do {\
        p_user_config->txbuf.uid = 0x80 | (_uid);\
        p_user_config->rxbuf.uid = _uid;\
        p_user_config->txbuf.port_idx = pid;\
        p_user_config->rxbuf.port_idx = pid;\
    } while(0)
    memset(p_port_config, 0, sizeof(mux_ll_port_config_t));
    memset(g_mux_ll_config, 0, sizeof(mux_ll_user_config_t));

    p_port_config->is_bypass_tx = false;

    g_mux_ll_config->user_count = MAX_MUX_LL_USER_COUNT;
    g_port_config.ll_uart_hw_info[0] = (mux_ll_uart_hw_info_t*)0x40090130;//TX
    g_port_config.ll_uart_hw_info[1] = (mux_ll_uart_hw_info_t*)0x40090230;//RX
    for (i = 0; i < MAX_MUX_LL_USER_COUNT; i++) {
        txflags = 0;
        rxflags = 0;
        p_user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[i];
        ll_uart_config = &g_mux_ll_uart_user_configure_table[i];
        memset(&p_user_config->buffer_fifo, 0, sizeof(buffer_fifo_t));
        p_user_config->buffer_fifo.is_empty = true;

        utag = &p_user_config->utag;
        p_user_config->scid = ll_uart_config->scid;
        p_user_config->dcid = ll_uart_config->dcid;
        utag->uid = i;

        utag->tx_priority = ll_uart_config->tx_priority;
        utag->tx_pkt_len = ll_uart_config->tx_pkt_len;

#ifdef MUX_LL_UART_SEQ_ENABLE
        g_mux_ll_user_context[i].sc_pkt_seq = 0;
        g_mux_ll_user_context[i].dc_pkt_seq = 0;
#endif

        if (strlen(ll_uart_config->name) >= MUX_LL_UART_MAX_USER_NAME) {
            RB_LOG_E("[mux_init_ll_uart_buffer] user name length out of range, max user name length is %d", 1, MUX_LL_UART_MAX_USER_NAME - 1);
            assert(0);
        }

        g_mux_ll_config->device_mode = DCHS_MODE_ERROR;

        strncpy(p_user_config->user_name, ll_uart_config->name, strlen(ll_uart_config->name));
        p_user_config->user_name[strlen(ll_uart_config->name)] = '\0';

        RB_SET_UID_PID(p_user_config, i, port_index);

        if (ll_uart_config->flags & MUX_LL_UART_ATTR_PACKAGE_TYPE_DECODE) {
            txflags = MUX_RB_FLAG_DATA_4TOH3 | MUX_RB_FLAG_TX_BUF;
            rxflags = MUX_RB_FLAG_DATA_3TOH4 | MUX_RB_FLAG_RX_BUF;
            rl_tx_pkt_len = utag->tx_pkt_len >> 2;
            rl_tx_pkt_len = (rl_tx_pkt_len << 1) + rl_tx_pkt_len;
        } else {
            rl_tx_pkt_len = utag->tx_pkt_len;
        }

        max_tx_pkt_len = RB_MAX(max_tx_pkt_len, rl_tx_pkt_len + MUX_PACKAGE_HEAD_TAIL_LEN);
        min_tx_pkt_len = RB_MIN(min_tx_pkt_len, rl_tx_pkt_len + MUX_PACKAGE_HEAD_TAIL_LEN);

        utag->flags = ll_uart_config->flags;
        if (ll_uart_config->scid == HAL_CORE_DSP0) {
            txflags |= MUX_RB_FLAG_SHARED_BUFFER;
        }
        if (ll_uart_config->dcid == HAL_CORE_DSP0) {
            rxflags |= MUX_RB_FLAG_SHARED_BUFFER;
        }

        mux_ringbuffer_init((mux_ringbuffer_t *)&p_user_config->txbuf, ll_uart_config->tx_buf_addr, ll_uart_config->tx_buf_len, txflags);
        mux_ringbuffer_init((mux_ringbuffer_t *)&p_user_config->rxbuf, ll_uart_config->rx_buf_addr, ll_uart_config->rx_buf_len, rxflags);
        //add user fifo to list from tail according to priority
        fifo_list_head = &p_port_config->user_fifo_list_head[ll_uart_config->tx_priority];//priority 0 is reserved now
        new_fifo_node = (mux_user_fifo_node_t*)pvPortMalloc(sizeof(mux_user_fifo_node_t));
        if (new_fifo_node == NULL) {
            assert(0);
        }

        if (*fifo_list_head == NULL) {
            // RB_LOG_I("[mux_init_ll_uart_buffer] first new_node=0x%x, usr_count=%d!!", 2, (uint32_t)new_fifo_node, i);
            *fifo_list_head = new_fifo_node;
        } else {
            last_node(node, *fifo_list_head);
            node->next = new_fifo_node;
            // RB_LOG_I("[mux_init_ll_uart_buffer] not first node=0x%x new_node=0x%x, usr_count=%d!!", 3, (uint32_t)node, (uint32_t)new_fifo_node, i);
        }

        new_fifo_node->p_user_config = p_user_config;
        new_fifo_node->next = NULL;

        // RB_LOG_I("[mux_init_ll_uart_buffer] node=0x%08x", 1, (uint32_t)new_fifo_node);
    }
  /*mux_user_tag_t *tag;
    foreach_node(node, *fifo_list_head) {
        tag = &node->p_user_config->utag;
        RB_LOG_I("[mux_init_ll_uart_buffer] node=0x%x uid=%d priority=%d tx_pkt=%d", 4,\
            (uint32_t)node, tag->uid, tag->tx_priority, tag->tx_pkt_len);
    }*/
    g_mux_ll_config->tx_pkt_head_tail_len = MUX_PACKAGE_HEAD_TAIL_LEN;
    g_mux_ll_config->uart_tx_threshold = min_tx_pkt_len - 1;
    g_mux_ll_config->uart_rx_threshold = max_tx_pkt_len - 1;
}

#endif


ATTR_TEXT_IN_FAST_MEM  void user_uart_dma_callback(hal_uart_callback_event_t status, void *user_data)
{
    switch (status) {
    case HAL_UART_EVENT_READY_TO_WRITE:
        RB_LOG_D("[udcb]: MUX_EVENT_READY_TO_WRITE", 0);
        MUX_SET_TX_FLAGS(0xFF, MUX_TX_TRIGGER_OWNER_RDYW);
#ifdef LLUART_REAL_TIME_MEASUREMENT
        lluart_busy_time_measurement_action(LLUART_ACTION_DMA_EMPTY_IRQ);
#endif
        hal_nvic_set_pending_irq(SW_IRQn);
        break;
    case HAL_UART_EVENT_READY_TO_READ:
        RB_LOG_D("[udcb]: HAL_UART_EVENT_READY_TO_READ", 0);
#ifdef LLUART_REAL_TIME_MEASUREMENT
        cpu_mips_measurement_start();
        mux_rx_deliver();
        cpu_mips_measurement_end(false);
#else
        mux_rx_deliver();
#endif
        break;
    case HAL_UART_EVENT_TRANSMISSION_DONE:
        RB_LOG_I("[udcb]: HAL_UART_EVENT_TRANSMISSION_DONE", 0);
        break;
    case HAL_UART_EVENT_WAKEUP_SLEEP:
        RB_LOG_W("[udcb]: HAL_UART_EVENT_WAKEUP_SLEEP", 0);
#ifdef AIR_LL_MUX_WAKEUP_ENABLE
        g_mux_ll_user_context[MUX_WAKEUP_INDEX].dc_pkt_seq++;
        mux_ll_uart_parse_wakeup_cmd();
#endif
        break;
#ifdef HAL_UART_FEATURE_FLOWCONTROL_CALLBACK
    case HAL_UART_EVENT_SW_FLOW_CTRL:
        RB_LOG_I("[udcb]: HAL_UART_EVENT_SW_FLOW_CTRL", 0);
        break;
#endif
    case HAL_UART_EVENT_TRANSACTION_ERROR:
        RB_LOG_E("[udcb]: HAL_UART_EVENT_TRANSACTION_ERROR", 0);
        break;
    default:
        RB_LOG_E("[udcb]: unsupport event:%d", 1, status);
    }
}

ATTR_TEXT_IN_FAST_MEM void temp_uart_enable_tx_hw_fifo_empty_irq(hal_uart_port_t UART_PORT)
{
    extern UART_REGISTER_T *const g_uart_regbase[];
    UART_REGISTER_T *uartx;
    uartx = g_uart_regbase[UART_PORT];
    // uart_unmask_send_interrupt(uartx);
    uart_mask_send_interrupt(uartx);
    // uart_mask_receive_interrupt(uartx);//timeout mask
}

ATTR_TEXT_IN_FAST_MEM void temp_uart_enable_rx_dma_irq(hal_uart_port_t UART_PORT)
{
    vdma_status_t status;
    extern vdma_channel_t  uart_port_to_dma_map[2][3];
    extern vdma_status_t vdma_enable_interrupt(vdma_channel_t channel);
    #define  uart_port_to_dma_channel(uart_port,is_rx)   (uart_port_to_dma_map[is_rx][uart_port])

    vdma_channel_t channel;
    channel = uart_port_to_dma_channel(UART_PORT, 1);
    status  = vdma_enable_interrupt(channel);
    if (status != VDMA_OK) {
        assert(0);
    }

}

mux_status_t mux_ll_uart_normal_init(uint8_t port_index, mux_ll_port_config_t *p_setting)
{
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;
    hal_uart_status_t sta;

    IRQn_Type uart_dma_irq[] = {UART_DMA0_IRQn, UART_DMA1_IRQn, UART_DMA2_IRQn};
    IRQn_Type uart_irq[] = {UART0_IRQn, UART1_IRQn, UART2_IRQn};

    uart_config.baudrate     = p_setting->p_user_setting->dev_setting.uart.uart_config.baudrate;
    uart_config.parity       = p_setting->p_user_setting->dev_setting.uart.uart_config.parity;
    uart_config.stop_bit     = p_setting->p_user_setting->dev_setting.uart.uart_config.stop_bit;
    uart_config.word_length  = p_setting->p_user_setting->dev_setting.uart.uart_config.word_length;

    dma_config.send_vfifo_buffer      = (uint8_t *)(uint32_t *)p_setting->tx_buf_addr;
    dma_config.send_vfifo_buffer_size = p_setting->tx_buf_size;
    dma_config.receive_vfifo_buffer   = (uint8_t *)(uint32_t *)p_setting->rx_buf_addr;
    dma_config.receive_vfifo_buffer_size = p_setting->rx_buf_size;
    dma_config.send_vfifo_threshold_size = g_mux_ll_config->uart_tx_threshold;
    dma_config.receive_vfifo_threshold_size   = g_mux_ll_config->uart_rx_threshold;
    dma_config.receive_vfifo_alert_size       = 12;

    hal_uart_deinit(port_index);

    if (HAL_UART_STATUS_OK != (sta = hal_uart_init(port_index, &uart_config))) {
        RB_LOG_E("[mux_ll_uart_normal_init] hal_uart_init fail, sta = %d", 1, sta);
        return MUX_STATUS_ERROR_INIT_FAIL;
    }

    if (HAL_UART_STATUS_OK != (sta = hal_uart_set_dma(port_index, &dma_config))) {
        RB_LOG_E("[mux_ll_uart_normal_init] hal_uart_set_dma fail, sta = %d", 1, sta);
        return MUX_STATUS_ERROR_INIT_FAIL;
    }

    if (p_setting->p_user_setting->dev_setting.uart.flowcontrol_type == MUX_UART_NONE_FLOWCONTROL) {
        hal_uart_disable_flowcontrol(port_index);
    } else if (p_setting->p_user_setting->dev_setting.uart.flowcontrol_type == MUX_UART_SW_FLOWCONTROL) {
        hal_uart_set_software_flowcontrol(port_index, 0x11, 0x13, 0x77);
    } else if (p_setting->p_user_setting->dev_setting.uart.flowcontrol_type == MUX_UART_HW_FLOWCONTROL) {
        hal_uart_set_hardware_flowcontrol(port_index);
    } else {
        assert(0);
    }

    if (HAL_UART_STATUS_OK != (sta = hal_uart_register_callback(port_index, user_uart_dma_callback, (void *)(intptr_t)port_index))) {
        RB_LOG_E("[mux_ll_uart_normal_init] hal_uart_register_callback fail, sta = %d", 1, sta);
        return MUX_STATUS_ERROR_INIT_FAIL;
    }

    temp_uart_enable_tx_hw_fifo_empty_irq(port_index);
    temp_uart_enable_rx_dma_irq(port_index);

    NVIC_SetPriority(SW_IRQn, DEFAULT_IRQ_PRIORITY + 1);
    NVIC_SetPriority(uart_dma_irq[port_index], DEFAULT_IRQ_PRIORITY + 2);
    NVIC_SetPriority(uart_irq[port_index], DEFAULT_IRQ_PRIORITY + 2);

    hal_nvic_register_isr_handler(SW_IRQn, mux_tx_limiter_sw_irq_isr);
    hal_nvic_enable_irq(SW_IRQn);

    return MUX_STATUS_OK;
}

mux_status_t mux_init_ll(mux_port_t port, mux_port_setting_t *setting, mux_protocol_t *protocol_callback)
{
    uint32_t port_index;
    mux_status_t sta;
    mux_ll_port_config_t *mux_ll_port_config = &g_port_config;

    port_index = MUX_LL_PORT_2_UART_PORT(port);

    g_mux_ll_config = &mux_ll_config;

    MUX_LL_CONFIG_SHMEM_ADDRESS_VALUE = (uint32_t)hal_memview_mcu_to_dsp0((uint32_t)g_mux_ll_config);

    mux_init_ll_uart_buffer(port_index);

    RB_LOG_I("[mux_init_ll] mux_init port=%d for low latency uart, sizeof(mux_ll_config_t)=%d g_mux_ll_config=0x%x", 3, port, sizeof(mux_ll_config_t), (uint32_t)g_mux_ll_config);

    memcpy((void *)&g_mux_port_hw_setting, (void *)setting, sizeof(mux_port_setting_t));

    mux_ll_port_config->p_user_setting = (mux_port_setting_t *)&g_mux_port_hw_setting;

    mux_ll_port_config->tx_buf_size = setting->tx_buffer_size;
    if (setting->tx_buffer == 0) {
        mux_ll_port_config->tx_buf_addr = (uint32_t)pvPortMallocNC(mux_ll_port_config->tx_buf_size);
    } else {
        mux_ll_port_config->tx_buf_addr = setting->tx_buffer;
    }
    mux_ll_port_config->rx_buf_size = setting->rx_buffer_size;
    if (setting->rx_buffer == 0) {
        mux_ll_port_config->rx_buf_addr = (uint32_t)pvPortMallocNC(mux_ll_port_config->rx_buf_size);
    } else {
        mux_ll_port_config->rx_buf_addr = setting->rx_buffer;
    }
    /*This is used to check if the shared memory, overwritten by someone else.*/
    g_mux_ll_config->config_head = 0x5A5A5A5A;

    if (is_mux_ll_port(port)) {
        RB_LOG_I("[mux_init_ll] port=%d, tx_buffer=0x%x tx_buffer_size=%d rx_buffer=0x%x rx_buffer_size=%d", 5, port, setting->tx_buffer, setting->tx_buffer_size, setting->rx_buffer, setting->rx_buffer_size);
        mux_ringbuffer_init(&mux_ll_port_config->txbuf, (uint8_t*)mux_ll_port_config->tx_buf_addr, mux_ll_port_config->tx_buf_size, MUX_RB_FLAG_TX_BUF);
        mux_ll_port_config->txbuf.uid = 0xFF;

        mux_ringbuffer_init(&mux_ll_port_config->rxbuf, (uint8_t*)mux_ll_port_config->rx_buf_addr, mux_ll_port_config->rx_buf_size, MUX_RB_FLAG_RX_BUF);
        mux_ll_port_config->rxbuf.uid = 0x7F;
    } else {
        RB_LOG_I("[mux_init_ll] port=%d not ll port, setting->tx_buffer=0x%x setting->rx_buffer=0x%x", 3, port, setting->tx_buffer, setting->rx_buffer);
    }
    mux_ll_port_config->txbuf.port_idx = port_index;
    mux_ll_port_config->rxbuf.port_idx = port_index;

    mux_ll_port_config->txbuf.ops = &mux_uart_ops;
    mux_ll_port_config->rxbuf.ops = &mux_uart_ops;

    if (protocol_callback == NULL) {
        mux_ll_port_config->protocol_cpu.tx_protocol_callback = NULL;
        mux_ll_port_config->protocol_cpu.rx_protocol_callback = NULL;
        mux_ll_port_config->protocol_cpu.user_data = NULL;
    } else {
        mux_ll_port_config->protocol_cpu.tx_protocol_callback = protocol_callback->tx_protocol_callback;
        mux_ll_port_config->protocol_cpu.rx_protocol_callback = protocol_callback->rx_protocol_callback;
        mux_ll_port_config->protocol_cpu.user_data = protocol_callback->user_data;
    }
    /* logging device post initialization */
    if (MUX_STATUS_OK != (sta = mux_ll_uart_normal_init(port_index, mux_ll_port_config))) {
        RB_LOG_E("[mux_init_ll] port=%d sta=0x%x", 2, port, sta);
        return MUX_STATUS_ERROR_INIT_FAIL;
    }
#ifdef AIR_LL_MUX_WAKEUP_ENABLE
    mux_ll_uart_wakeup_init();
#endif
    mux_ll_uart_sync_init();
    mux_ll_uart_latch_init();
    return MUX_STATUS_OK;
}

mux_status_t mux_deinit_ll(mux_port_t port)
{
    if (HAL_UART_STATUS_OK != hal_uart_deinit(MUX_LL_PORT_2_UART_PORT(port))) {
        return MUX_STATUS_ERROR_DEINIT_FAIL;
    } else {
        return MUX_STATUS_OK;
    }
}

#endif

mux_status_t mux_open_ll(mux_port_t port, const char *user_name, mux_handle_t *p_handle, mux_callback_t callback, void *user_data)
{
    uint32_t per_cpu_irq_mask;
    uint32_t uid;

    mux_ll_user_config_t *mux_ll_user_config = NULL;

    if (g_mux_ll_config == NULL) {
        return MUX_STATUS_ERROR_PARAMETER;
    }

    for (uid = 0; uid < g_mux_ll_config->user_count; uid++) {
        mux_ll_user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[uid];
        if (!strcmp(mux_ll_user_config->user_name, user_name)) {
            break;
        }
    }

    if (uid == g_mux_ll_config->user_count) {
        LOG_E(MUX_LL_PORT, "[mux_open_ll] port=%d, [%s] user not found", port, user_name);
        return MUX_STATUS_ERROR;
    }

    *p_handle = user_id_to_handle(uid, port);
    RB_LOG_I("[mux_open_ll] port=%d handle=0x%x", 2, port, *p_handle);

    port_mux_cross_local_enter_critical(&per_cpu_irq_mask);
    mux_ll_user_config->callback = callback;
    mux_ll_user_config->user_data = user_data;
    port_mux_cross_local_exit_critical(per_cpu_irq_mask);

    return MUX_STATUS_OK;
}


mux_status_t mux_close_ll(mux_handle_t handle)
{
    uint32_t uid;
    uint32_t per_cpu_irq_mask;
    mux_ll_user_config_t *mux_ll_user_config = NULL;

    uid = handle_to_user_id(handle);
    mux_ll_user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[uid];

    port_mux_cross_local_enter_critical(&per_cpu_irq_mask);
    mux_ll_user_config->callback = NULL;
    mux_ll_user_config->user_data = NULL;
    port_mux_cross_local_exit_critical(per_cpu_irq_mask);

    return MUX_STATUS_OK;
}

void mux_reset_user_fifo(mux_handle_t handle)
{
    uint32_t uid;
    mux_ll_user_config_t *user_config;

    uid = handle_to_user_id(handle);

    user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[uid];
    mux_ringbuffer_reset((mux_ringbuffer_t *)&user_config->rxbuf);
}

ATTR_TEXT_IN_FAST_MEM mux_status_t mux_rx_ll(mux_handle_t handle, mux_buffer_t *buffer, uint32_t *receive_done_data_len)
{
    uint32_t uid;
    mux_ll_user_config_t *user_config;

    uid = handle_to_user_id(handle);

    if (g_mux_ll_config == NULL) {
        return MUX_STATUS_ERROR;
    }

    user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[uid];
    if (user_config->utag.flags & MUX_LL_UART_ATTR_USER_TRX_PROTOCOL) {
        if (user_config->buffer_fifo.is_empty == true) {
            *receive_done_data_len = 0;
            return MUX_STATUS_USER_RX_QUEUE_EMPTY;
        }
        if (mux_pop_one_pkt_from_rx_fifo(user_config, buffer->p_buf, buffer->buf_size, receive_done_data_len)) {
            return MUX_STATUS_OK;
        } else {
            return MUX_STATUS_USER_RX_BUF_SIZE_NOT_ENOUGH;
        }
    } else {
        *receive_done_data_len = mux_ringbuffer_read((mux_ringbuffer_t *)&user_config->rxbuf, buffer->p_buf, buffer->buf_size);
    }

    LOG_MSGID_I(common, "[mux_rx_ll] [%02x] expect size=%d real size=%d, ud_len=%d vd_len=%d", 5, user_config->rxbuf.uid, buffer->buf_size, *receive_done_data_len,\
        mux_ringbuffer_data_length(&user_config->rxbuf), mux_ringbuffer_data_length(&g_port_config.rxbuf));
    return MUX_STATUS_OK;
}


ATTR_TEXT_IN_FAST_MEM mux_status_t mux_tx_ll(mux_handle_t handle, const mux_buffer_t buffers[], uint32_t buffers_counter, uint32_t *send_done_data_len)
{
    mux_ringbuffer_t *txbuf;
    uint32_t send_len = 0;
    mux_ll_user_config_t *user_config;
    mux_protocol_t *mux_protocol;
    mux_buffer_t head_buf_info, tail_buf_info;
    uint32_t head_buf[(TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN + 4) / 4], tail_buf[(TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN + 4) / 4]; // 68 BYTE
    uint32_t total_size;
    uint32_t payload_size = 0;
    uint32_t old_head;
    uint32_t tmp_old_head;
    uint32_t new_head;
    uint32_t uid;
    uint32_t i;

#ifdef MUX_LL_UART_RB_DUMP_ENABLE
    uint32_t expected_total_size = 0;
#endif
    const mux_buffer_t *pbuf;
    uid = handle_to_user_id(handle);

    if (g_mux_ll_config == NULL) {
        return MUX_STATUS_ERROR;
    }

    user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[uid];

    txbuf = &user_config->txbuf;

    if (g_port_config.is_bypass_tx) {
        /* Calculate total size of payload */
        for (i = 0; i < buffers_counter; i++) {
            payload_size += buffers[i].buf_size;
        }
        RB_LOG_W("[mux_tx_ll] bypass, uid=%02x send_len=%d ud_len=%d vd_len=%d", 4, txbuf->uid, payload_size, mux_ringbuffer_data_length(txbuf), mux_ringbuffer_data_length(&g_port_config.txbuf));
        *send_done_data_len = payload_size;
        return MUX_STATUS_OK;
    }

    if (user_config->utag.flags & MUX_LL_UART_ATTR_USER_TRX_PROTOCOL) {
        /* Calculate total size of payload */
        for (i = 0; i < buffers_counter; i++) {
            payload_size += buffers[i].buf_size;
        }
        head_buf_info.p_buf = NULL;
        tail_buf_info.p_buf = NULL;
        mux_protocol = &g_port_config.protocol_cpu;
        if (mux_protocol->tx_protocol_callback != NULL) {
            head_buf[TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN / 4] = 0xdeadbeef; //for memory crash check
            head_buf_info.p_buf = (uint8_t *)head_buf;
            head_buf_info.buf_size = TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN;
            tail_buf[TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN / 4] = 0xdeadbeef; //for memory crash check
            tail_buf_info.p_buf = (uint8_t *)tail_buf;
            tail_buf_info.buf_size = TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN;
            mux_protocol->tx_protocol_callback(handle, buffers, buffers_counter, &head_buf_info, &tail_buf_info, mux_protocol->user_data);
            if ((head_buf[TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN / 4] != 0xdeadbeef) || (tail_buf[TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN / 4] != 0xdeadbeef)) {
                //memory crash check!!! Rx and Tx buffer max len is TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN and TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN
                //But the callback write too many memory
                return MUX_STATUS_USER_ERROR_OF_RX_TX_PROTOCOL;
            }
            if ((head_buf_info.buf_size > TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN) || (tail_buf_info.buf_size > TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN)) {
                //memory crash check!!! Rx and Tx buffer max len is TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN and TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN
                //But the callback write too many memory
                return MUX_STATUS_USER_ERROR_OF_RX_TX_PROTOCOL;
            }
            total_size = head_buf_info.buf_size + payload_size + tail_buf_info.buf_size;
        } else {
            total_size = payload_size;
            // assert(0);
        }

        if (mux_ringbuffer_write_move_head(txbuf, &old_head, &new_head, total_size) != total_size) {
            //txbuf do not have enough free space
            assert(0);
        }
        send_len = payload_size;
        tmp_old_head = old_head;
        if (head_buf_info.p_buf != NULL) {
            mux_ringbuffer_write_move_data(txbuf, head_buf_info.p_buf, head_buf_info.buf_size, tmp_old_head);
            tmp_old_head += head_buf_info.buf_size;
        }
        for (i = 0; i < buffers_counter; i++) {
            pbuf = &buffers[i];
            mux_ringbuffer_write_move_data(txbuf, pbuf->p_buf, pbuf->buf_size, tmp_old_head);
            tmp_old_head += pbuf->buf_size;
        }
        if (tail_buf_info.p_buf != NULL) {
            mux_ringbuffer_write_move_data(txbuf, tail_buf_info.p_buf, tail_buf_info.buf_size, tmp_old_head);
            tmp_old_head += tail_buf_info.buf_size;
        }

        mux_ringbuffer_write_move_tail(txbuf, old_head, new_head);
        // mux_ringbuffer_hexdump(txbuf, true);
    } else {
        for (i = 0; i < buffers_counter; i++) {
            pbuf = &buffers[i];
            send_len += mux_ringbuffer_write(txbuf, pbuf->p_buf, pbuf->buf_size);
#ifdef MUX_LL_UART_RB_DUMP_ENABLE
        expected_total_size += pbuf->buf_size;
#endif
    }
#ifdef MUX_LL_UART_RB_DUMP_ENABLE
        if (send_len != expected_total_size) {
            mux_ringbuffer_hexdump(txbuf, false);
        }
#endif
    }
    *send_done_data_len = send_len;
    LOG_MSGID_I(common, "[mux_tx_ll] uid=%02x send_len=%d ud_len=%d vd_len=%d", 4, txbuf->uid, send_len, mux_ringbuffer_data_length(txbuf), mux_ringbuffer_data_length(&g_port_config.txbuf));

    //TODO: Finding vFIFO empty, pull SW IRQ trigger to send MUX TX
    MUX_SET_TX_FLAGS(uid, MUX_TX_TRIGGER_OWNER_MCU_TX);
    hal_nvic_set_pending_irq(SW_IRQn);

    return MUX_STATUS_OK;
}

ATTR_TEXT_IN_FAST_MEM void mux_ll_uart_tx_event_from_dsp_handler(hal_ccni_event_t event, void * msg)
{
    hal_ccni_status_t status;

    RB_LOG_D("MCU receive ccni tx event ", 0);
    status = hal_ccni_clear_event(event);  // clear the event.
    if (status != HAL_CCNI_STATUS_OK) {
        RB_LOG_E("MCU CCNI clear event: 0x%x something wrong, return is %d", 2, event, status);
    }

    status = hal_ccni_unmask_event(event); // unmask the event.
    if (status != HAL_CCNI_STATUS_OK) {
        RB_LOG_E("MCU CCNI unmask event: 0x%x something wrong, return is %d", 2, event, status);
    }
    if (!DCHS_IS_UART_TX_LOCKED()) {
        MUX_SET_TX_FLAGS(0xFE, MUX_TX_TRIGGER_OWNER_DSP_TX);
        hal_nvic_set_pending_irq(SW_IRQn);
    }
}



mux_status_t mux_control_ll(mux_handle_t handle, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    uint32_t uid;
    uint32_t port_idx;
    uint32_t rl_tx_pkt_len;
    mux_ll_user_config_t *user_config;
    uid = handle_to_user_id(handle);
    vdma_channel_t uart_tx_ch[3] = {VDMA_UART0TX, VDMA_UART1TX, VDMA_UART2TX};
    vdma_channel_t uart_rx_ch[3] = {VDMA_UART0RX, VDMA_UART1RX, VDMA_UART2RX};

    user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[uid];
    switch(command) {
    case MUX_CMD_GET_LL_USER_RX_BUFFER_DATA_SIZE:
        para->mux_ll_user_rx_data_len = mux_ringbuffer_data_length(&user_config->rxbuf);
        break;
    case MUX_CMD_GET_LL_USER_TX_BUFFER_FREE_SIZE:
        para->mux_ll_user_tx_free_len = mux_ringbuffer_free_space(&user_config->txbuf);
        break;
    case MUX_CMD_SET_LL_USER_TX_PKT_LEN:
        if (user_config->txbuf.flags & MUX_RB_FLAG_DATA_4TO3) {
            rl_tx_pkt_len = para->mux_ll_user_tx_pkt_len >> 2;
            rl_tx_pkt_len = (rl_tx_pkt_len << 1) + rl_tx_pkt_len;
        } else {
            rl_tx_pkt_len = para->mux_ll_user_tx_pkt_len;
        }

        g_mux_ll_config->uart_tx_threshold = RB_MIN(rl_tx_pkt_len + g_mux_ll_config->tx_pkt_head_tail_len, g_mux_ll_config->uart_tx_threshold) - 1;
        g_mux_ll_config->uart_rx_threshold = RB_MAX(rl_tx_pkt_len + g_mux_ll_config->tx_pkt_head_tail_len, g_mux_ll_config->uart_rx_threshold) - 1;
        user_config->utag.tx_pkt_len = para->mux_ll_user_tx_pkt_len;
        port_idx = MUX_LL_PORT_2_UART_PORT(handle_to_port(handle));
        vdma_set_threshold(uart_tx_ch[port_idx], g_mux_ll_config->uart_tx_threshold);
        vdma_set_threshold(uart_rx_ch[port_idx], g_mux_ll_config->uart_rx_threshold);
        break;
    default:
        break;
    }
    return MUX_STATUS_OK;
}

mux_status_t mux_query_ll_user_handle(mux_port_t port, const char *user_name, mux_handle_t *p_handle)
{
    uint32_t i;
    uint32_t ll_user_count;
    mux_ll_user_config_t *mux_ll_user_config = NULL;

    *p_handle = 0xdeadbeef;

    if (user_name == NULL) {
        return MUX_STATUS_ERROR_PARAMETER;
    }

    ll_user_count = sizeof(g_mux_ll_config->user_config) / sizeof(g_mux_ll_config->user_config[0]);

    for (i = 0; i < ll_user_count; i++) {
        mux_ll_user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[i];
        if (!strcmp(mux_ll_user_config->user_name, user_name) && (HAL_CORE_MCU == mux_ll_user_config->scid)) {
            *p_handle = user_id_to_handle(i, port);
            return MUX_STATUS_OK;
        }
    }

    return MUX_STATUS_ERROR;
}

mux_ringbuffer_t * mux_query_ll_user_buffer(mux_handle_t handle, bool is_rx)
{
    uint32_t uid;
    mux_ringbuffer_t *rb = NULL;

    if (is_mux_ll_handle(handle)) {
        uid = handle_to_user_id(handle);
        if (is_rx) {
            rb = &g_mux_ll_config->user_config[uid].rxbuf;
        } else {
            rb = &g_mux_ll_config->user_config[uid].txbuf;
        }

    }
    return rb;
}

void mux_clear_ll_user_buffer(mux_handle_t handle, bool is_rx)
{
    mux_ringbuffer_t *rb;

    LOG_MSGID_I(common, "[mux_clear_ll_user_buffer] handle=0x%x is_rx=%d", 2, handle, is_rx);

    rb = mux_query_ll_user_buffer(handle, is_rx);
    if (rb) {
        mux_ringbuffer_reset(rb);
    } else {
        RB_LOG_E("Handle error:0x%x", 1, handle);
    }
}

ATTR_TEXT_IN_FAST_MEM mux_status_t mux_peek_ll(mux_handle_t handle, mux_buffer_t *buffer, uint32_t *receive_done_data_len)
{
    uint32_t uid;
    mux_ll_user_config_t *user_config;

    uid = handle_to_user_id(handle);

    if (g_mux_ll_config == NULL) {
        return MUX_STATUS_ERROR;
    }

    user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[uid];

    *receive_done_data_len = mux_ringbuffer_peek((mux_ringbuffer_t *)&user_config->rxbuf, buffer->p_buf, buffer->buf_size);

    RB_LOG_D("[mux_peek_ll] [%02x] rxbuf[%s] expect size=%d real size=%d", 3, uid, buffer->buf_size, *receive_done_data_len);
    return MUX_STATUS_OK;
}

void mux_hexdump_ll(mux_handle_t handle, const char* funcname, bool is_dump_data, bool is_txbuff)
{
    uint32_t uid;
    mux_ll_user_config_t *user_config;
    mux_ringbuffer_t *rb;

    if (!is_mux_ll_handle(handle)) {
        return;
    }
    uid = handle_to_user_id(handle);
    user_config = (mux_ll_user_config_t *)&g_mux_ll_config->user_config[uid];

    rb = is_txbuff ? &user_config->txbuf : &user_config->rxbuf;
    mux_ringbuffer_dump(rb, funcname, is_dump_data);
}

void dchs_device_ready_to_off_callback(void)
{
    uint32_t start_count;
    uint32_t start_count_bkp;
    uint32_t timeout;

    start_count = mux_get_tick_count();
    start_count_bkp = start_count;
    LOG_MSGID_I(common, "[dchs_device_ready_to_off_callback]", 0);
    g_port_config.is_bypass_tx = true;

    do {
        timeout = mux_get_tick_elapse(start_count);
        LOG_MSGID_I(common, "[dchs_device_ready_to_off_callback] wait uart tx fifo empty", 0);
    } while ((mux_ringbuffer_empty(&g_port_config.txbuf) != true) && (timeout < 4000000));//4s timeout

    if (timeout >= 4000000) {
        //uart tx may be blocked!!
        assert(0);
    }
    hal_gpio_set_high_impedance(HAL_UART1_TXD_PIN);
    do {
        timeout = mux_get_tick_elapse(start_count);
        LOG_MSGID_I(common, "[dchs_device_ready_to_off_callback] wait uart rx fifo empty", 0);
    } while ((mux_ringbuffer_empty(&g_port_config.rxbuf) != true) && (timeout < 4000000));//4s timeout

    if (timeout >= 4000000) {
        //uart tx may be blocked!!
        assert(0);
    }
    hal_gpio_set_high_impedance(HAL_UART1_RXD_PIN);
    LOG_MSGID_I(common, "[dchs_device_ready_to_off_callback]cost %uus", 1, mux_get_tick_elapse(start_count_bkp));

}
