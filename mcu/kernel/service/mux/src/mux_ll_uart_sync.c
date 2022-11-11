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
#include "mux_ll_uart_sync.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif/*HAL_SLEEP_MANAGER_ENABLED*/


#define DEVICE_SYNC_TIMEOUT 1000
#define SW1_IRQn (SW_IRQn + 1)
mux_handle_t device_sync_handle;

uint32_t device_sync_timer_handle = 0xFFFFFFFF;
uint32_t device_sync_start_count = 0;

volatile dchs_mode_t g_device_mode = DCHS_MODE_ERROR;
volatile static uint32_t flag_self = 0, flag_other = 0;


ATTR_TEXT_IN_FAST_MEM bool mux_ll_uart_send_sync_data(uint16_t race_id, uint16_t tag)
{
    uint16_t buf[4] = {0x5D05, 0x0004, race_id, tag};

    // RB_LOG_I("[device sync] device_sync_handle=0x%x", 1, device_sync_handle);

    mux_buffer_t buffer = {(uint8_t*)buf, sizeof(buf)};
    mux_status_t status;
    uint32_t send_done_len = 0;
    status = mux_tx(device_sync_handle, &buffer, 1, &send_done_len);
    if (status != MUX_STATUS_OK) {
        assert(0);
    }
    if (buffer.buf_size != send_done_len) {
        RB_LOG_I("[device sync] tx buffer full", 0);
    }
    // mux_hexdump_ll(device_sync_handle, __func__, true, true);
    return true;

}

static void mux_ll_uart_set_device_mode(dchs_mode_t device_mode)
{
    mux_ll_config.device_mode = device_mode;
    g_device_mode = device_mode;
}

void dchs_device_sync_gpt_callback(void *user_data)
{
    if (flag_self == 1 ) {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_unlock_sleep(MUX_LL_UART_SLEEP_LOCK_HANDLE);
#endif
        return ;
    }
#ifdef AIR_DCHS_MODE_MASTER_ENABLE
    mux_ll_uart_send_sync_data(DCHS_RACE_DETECT_SLAVE, 0x1);
    LOG_MSGID_I(common, "[device sync] timer send DCHS_RACE_DETECT_SLAVE", 0);
#else
    mux_ll_uart_send_sync_data(DCHS_RACE_DETECT_MASTER, 0x1);
    LOG_MSGID_I(common, "[device sync] timer send DCHS_RACE_DETECT_MASTER", 0);
#endif

    hal_gpt_sw_start_timer_ms(device_sync_timer_handle, DEVICE_SYNC_TIMEOUT, dchs_device_sync_gpt_callback, NULL);
}

ATTR_TEXT_IN_FAST_MEM void dchs_device_sync_cb(hal_nvic_irq_t irq_number)
{
    *((volatile uint32_t*)0x40060018) |= 0x1000000;//xon
    // *((volatile uint32_t*)0x40060018) |= 0x10000;//xoff
    dchs_device_sync_gpt_callback(NULL);
}

dchs_mode_t dchs_get_device_mode(void)
{
    uint32_t start_count = mux_get_tick_count();
    uint32_t timeout;

    do {
        timeout = mux_get_tick_elapse(start_count);
    } while ((device_sync_timer_handle == 0xFFFFFFFF) && (timeout < 3000000));//3s timeout

    if (device_sync_timer_handle == 0xFFFFFFFF) {
        //device sync sw irq may be blocked!!
        assert(0);
    }
#if 0
    start_count = mux_get_tick_count();
    do {
        timeout = mux_get_tick_elapse(start_count);
    } while ((g_device_mode == DCHS_MODE_ERROR) && (timeout < 4000000));//4s timeout

    if (g_device_mode == DCHS_MODE_ERROR) {
        //device sync timer may be blocked!!
        assert(0);
    }
#else
    start_count = mux_get_tick_count();
    while (g_device_mode == DCHS_MODE_ERROR) {
        timeout = mux_get_tick_elapse(start_count);
        if (timeout > 100000) {
            RB_LOG_W("[device sync] waiting for sync", 0);
            start_count = mux_get_tick_count();
        }
    }
#endif
    return g_device_mode;
}

ATTR_TEXT_IN_FAST_MEM void mux_ll_uart_sync_callback(uint16_t race_id, uint8_t *buff, uint32_t len)
{
    switch(race_id) {
#ifdef AIR_DCHS_MODE_MASTER_ENABLE
    case DCHS_RACE_DETECT_MASTER: /*CMD from Slave, send response to Slave.*/
        if (flag_other == 1) {
            LOG_MSGID_I(common, "[device sync] detect event from slave, seems slave abnormal reset, so master should reset to sync, my mode = %d, flag_self:%d flag_other:%d", 3, g_device_mode, flag_self, flag_other);
            hal_wdt_software_reset();
        } else {
            mux_ll_uart_send_sync_data(DCHS_RACE_DETECT_RESPONSE_FROM_MASTER, 0x2);
            flag_other = 1;
            if (flag_other == 1 && flag_self == 1) {
                mux_ll_uart_set_device_mode(DCHS_MODE_RIGHT);
            }
            LOG_MSGID_I(common, "[device sync] detect event from slave, my mode = %d, flag_self:%d flag_other:%d", 3, g_device_mode, flag_self, flag_other);
        }
        break;
    case DCHS_RACE_DETECT_SLAVE: /*TX RX connect, FT single mode*/
        flag_self = 1;
        LOG_MSGID_I(common, "[device sync] recv DCHS_RACE_DETECT_SLAVE, single mode,  flag_self:%d flag_other:%d", 2, flag_self, flag_other);
        mux_ll_uart_set_device_mode(DCHS_MODE_SINGLE);
        break;
    case DCHS_RACE_DETECT_RESPONSE_FROM_SLAVE:
        flag_self = 1;
        if (flag_other == 1 && flag_self == 1) {
            mux_ll_uart_set_device_mode(DCHS_MODE_RIGHT);
        }
        LOG_MSGID_I(common, "[device sync] recv DCHS_RACE_DETECT_RESPONSE_FROM_SLAVE, flag_self:%d flag_other:%d", 2, flag_self, flag_other);
        break;
#endif

#ifdef AIR_DCHS_MODE_SLAVE_ENABLE
    case DCHS_RACE_DETECT_SLAVE: /*CMD from Master, send response to Master.*/
        if (flag_other == 1) {
            LOG_MSGID_I(common, "[device sync] detect event from master, seems Master abnormal reset, so slave should reset to sync, my mode = %d, flag_self:%d flag_other:%d", 3, g_device_mode, flag_self, flag_other);
            hal_wdt_software_reset();
        } else {
            mux_ll_uart_send_sync_data(DCHS_RACE_DETECT_RESPONSE_FROM_SLAVE, 0x2);
            flag_other = 1;
            if (flag_other == 1 && flag_self == 1) {
                mux_ll_uart_set_device_mode(DCHS_MODE_LEFT);
            }
            LOG_MSGID_I(common, "[device sync] detect event from master, my mode = %d, flag_self:%d flag_other:%d", 3, g_device_mode, flag_self, flag_other);
        }
        break;
    case DCHS_RACE_DETECT_MASTER: /*TX RX connect, FT single mode*/
        flag_self = 1;
        LOG_MSGID_I(common, "[device sync] recv DCHS_RACE_DETECT_MASTER, single mode,  flag_self:%d flag_other:%d", 2, flag_self, flag_other);
        mux_ll_uart_set_device_mode(DCHS_MODE_SINGLE);
        break;
    case DCHS_RACE_DETECT_RESPONSE_FROM_MASTER:
        flag_self = 1;
        if (flag_other == 1 && flag_self == 1) {
            mux_ll_uart_set_device_mode(DCHS_MODE_LEFT);
        }
        LOG_MSGID_I(common, "[device sync] recv DCHS_RACE_DETECT_RESPONSE_FROM_MASTER, flag_self:%d flag_other:%d", 2, flag_self, flag_other);
        break;
#endif
    default:
        RB_LOG_E("[device sync] Error race id:0x%x!!", 1, race_id);
        break;
    }
}

ATTR_TEXT_IN_TCM void dchs_uart_sync_race_callback(mux_handle_t handle, mux_event_t event, uint32_t data_len, void *user_data)
{
    switch (event) {
    case MUX_EVENT_READY_TO_READ:
        mux_ll_uart_race_decode(mux_query_ll_user_buffer(handle, true), mux_ll_uart_sync_callback);
        break;
    case MUX_EVENT_READY_TO_WRITE:
        RB_LOG_I("[device sync] [race_callback] MUX_EVENT_READY_TO_WRITE", 0);
        break;
    default:
        RB_LOG_I("[device sync] [race_callback] event %d no task", 1, event);
    }
}

void mux_ll_uart_sync_init(void)
{
    mux_status_t status;

    status = mux_open(MUX_LL_UART_PORT, "SYNC", &device_sync_handle, dchs_uart_sync_race_callback, NULL);
    if(MUX_STATUS_OK != status) {
        RB_LOG_I("[device sync] mux_open fail, status=%d handle=0x%x", 2, status, device_sync_handle);
        assert(0);
    } else {
        RB_LOG_I("[device sync] mux_open ok handle=0x%x", 1, device_sync_handle);
    }

    NVIC_SetPriority(SW1_IRQn, DEFAULT_IRQ_PRIORITY + 1);

    hal_nvic_register_isr_handler(SW1_IRQn, dchs_device_sync_cb);
    hal_nvic_enable_irq(SW1_IRQn);
    hal_nvic_set_pending_irq(SW1_IRQn);
    hal_gpt_sw_get_timer(&device_sync_timer_handle);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(MUX_LL_UART_SLEEP_LOCK_HANDLE);
#endif
}
