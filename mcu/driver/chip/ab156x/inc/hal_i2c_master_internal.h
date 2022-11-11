/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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

#ifndef _HAL_I2C_MASTER_INTERNAL_H_
#define _HAL_I2C_MASTER_INTERNAL_H_

#include "hal_platform.h"
#include "hal_log.h"
#if defined(HAL_I2C_MASTER_MODULE_ENABLED)

#include "hal_pdma_internal.h"
//#define I2C_ENABLE_DEBUG_LOG

//#define I2C_SW_FIFO
#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
#define HAL_I2C_SW_FIFO_LEN 10
#endif

#define I2C_FIFO_DEPTH   (8)

#define I2C_SOURCE_CLOCK_RATE_26M         (26000)     /* 26Mhz*/
#define I2C_SOURCE_CLOCK_BUS_CLOCK        (0)

#define I2C_CONTROL_REGISTER_DMA_BIT    (8)

#define I2C_COMPLETE_STATUS_BIT     (0)
#define I2C_ACKERR_STATUS_BIT       (8)
#define I2C_HS_NACKERR_STATUS_BIT   (16)

#define I2C_IO_PUSH_PULL            (0x0)
#define I2C_IO_OPEN_DRAIN           (0x3)

#define HAL_I2C_TX_DMA_CHANNEL      PDMA_I2C0_TX, PDMA_I2C1_TX, PDMA_I2C2_TX
#define HAL_I2C_RX_DMA_CHANNEL      PDMA_I2C0_RX, PDMA_I2C1_RX, PDMA_I2C2_RX

#define HAL_I2C_IRQ_TABLE           I2C0_IRQn, I2C1_IRQn, I2C2_IRQn, I2C_AO_IRQn

#define HAL_I2C_PDN_TABLE           HAL_CLOCK_CG_I2C0, HAL_CLOCK_CG_I2C1, HAL_CLOCK_CG_I2C2, HAL_CLOCK_CG_I2C_AO
#define HAL_I2C_SOURCE_CLOCK_TABLE  I2C_SOURCE_CLOCK_RATE_26M, I2C_SOURCE_CLOCK_RATE_26M, I2C_SOURCE_CLOCK_RATE_26M, I2C_SOURCE_CLOCK_RATE_26M

#define I2C_DMA_FINISH_CALLBACK_COUNT  (2)

#define I2C_ENABLE_POLLING_TIMEOUT
#define I2C_POLLING_TIMEOUT_VALUE   (20000) /* 20ms */
#define I2C_INTERRUPT_TIMEOUT_VALUE (20000) /* 200us */

#define I2C_WAIT_TRANSACTION_COMPLETE

#define I2C_TX_DMA_RUNNING_FLAG (0x01)
#define I2C_TX_DMA_FINISH_FLAG  (0x02)
#define I2C_RX_DMA_RUNNING_FLAG (0x04)
#define I2C_RX_DMA_FINISH_FLAG  (0x08)

#define MIN(A,B) ( (A) > (B) ? (B) : (A) )
#define MAX(A,B) ( (A) > (B) ? (A) : (B) )

//#define log_hal_msgid_error(_message,arg_cnt,...)
//#define log_hal_msgid_warning(_message,arg_cnt,...)
//#define log_hal_msgid_info(_message,arg_cnt,...)
//#define log_hal_msgid_debug(_message,arg_cnt,...)

//#define log_hal_error(_message,...)
//#define log_hal_warning(_message,...)
//#define log_hal_info(_message,...)
//#define log_hal_debug(_message,...)
//#define log_hal_dump(_message, _data, _len, ...)

/** @brief This enum define the I2C transaction speed.  */
typedef enum {
    I2C_FREQUENCY_50K  = 50,          /**<  Transmit data with 50kbps. */
    I2C_FREQUENCY_100K = 100,          /**<  Transmit data with 100kbps. */
    I2C_FREQUENCY_200K = 200,          /**<  Transmit data with 200kbps. */
    I2C_FREQUENCY_300K = 300,          /**<  Transmit data with 300kbps. */
    I2C_FREQUENCY_400K = 383,          /**<  Transmit data with 400kbps. */
    I2C_FREQUENCY_1M   = 1000,         /**<  Transmit data with 1mbps. */
#ifdef    HAL_I2C_MASTER_FEATURE_HIGH_SPEED
    I2C_FREQUENCY_2M   = 2000,         /**<  Transmit data with 2mbps. */
    I2C_FREQUENCY_3M   = 3250,         /**<  Transmit data with 3.25mbps. */
#endif
    I2C_FREQUENCY_MAX                  /**<  Max transaction speed, don't use this value. */
} i2c_frequency_t;

typedef enum {
    I2C_TRANSACTION_PUSH_PULL_SEND_POLLING = 0,
    I2C_TRANSACTION_PUSH_PULL_SEND_DMA     = 1,
    I2C_TRANSACTION_PUSH_PULL_RECEIVE_POLLING = 2,
    I2C_TRANSACTION_PUSH_PULL_RECEIVE_DMA = 3,
    I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_POLLING = 4,
    I2C_TRANSACTION_PUSH_PULL_SEND_TO_RECEIVE_DMA = 5,

    I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING = 6,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA     = 7,
    I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING = 8,
    I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA = 9,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING = 10,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA = 11,

    I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND     = 12,
    I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND = 13,
    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND = 14
} i2c_transaction_type_t;

typedef enum {
    I2C_TRANSACTION_UNKNOWN_ERROR = -4,
    I2C_TRANSACTION_ACK_ERROR = -3,           /**<  an ACK error happened during transaction. */
    I2C_TRANSACTION_NACK_ERROR = -2,          /**<  an NACK error happened during transaction.*/
    I2C_TRANSACTION_TIMEOUT_ERROR = -1,       /**<  an timeout error happened during transaction.*/
    I2C_TRANSACTION_SUCCESS = 0               /**<  the transaction complete wihtout any error. */
} i2c_transaction_status_t;

typedef struct {
    uint32_t send_size;
    uint32_t receive_size;
    uint32_t transfer_length;
    uint32_t transfer_length_aux;
    uint32_t transaction_length;
} i2c_transaction_size_t;

typedef struct {
    i2c_transaction_type_t transaction_type;        /* transaction type */
    bool dma_finish;
    uint8_t i2c_port;
} i2c_dma_callback_parameter_t;

typedef struct {
    uint16_t clock_div;
    uint16_t timing;
    uint32_t io;
    uint32_t control2;
} i2c_register_backup_t;

#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
typedef struct {
    hal_i2c_port_t i2c_port;
    hal_i2c_config_t i2c_config;
    hal_i2c_callback_t i2c_callback;
    void *user_data;
    hal_i2c_send_to_receive_config_ex_t i2c_send_to_receive_config_ex;
} hal_i2c_sw_fifo_node_t, hal_i2c_sw_fifo_base;

typedef struct {
    hal_i2c_sw_fifo_base   *i2c_sw_fifo_start;
    uint32_t i2c_sw_fifo_r_index;
    uint32_t i2c_sw_fifo_w_index;
    uint32_t i2c_sw_fifo_len;
} hal_i2c_sw_fifo_t;
#endif


typedef struct {
    /*indicatte i2c hw status*/
    uint8_t      i2c_op_state;
    /*indicatte whether oporater is preempted*/
    bool        *i2c_is_preempte;
    /*save last transfer status*/
    i2c_transaction_status_t    *i2c_intr_status;
    /* indicate rx data */
    uint8_t     *i2c_rx_buff;
    /* indicate rx transfer size*/
    uint32_t    i2c_rx_size;
    /*indicate how many rx operater in currently*/
    uint32_t    i2c_rx_cnt;
    /*indicate how many tx operater in currently*/
    uint32_t    i2c_tx_cnt;
} i2c_transfer_info_t;


#define I2C_BUSY  0x1
#define I2C_IDLE  0x0
#define I2C_RX 0x2
#define I2C_TX 0x4

#define I2C_CHECK_AND_SET_BUSY(i2c_port,busy_status)  \
do{ \
    uint32_t saved_mask; \
    hal_nvic_save_and_set_interrupt_mask(&saved_mask); \
    if(s_i2c_master_status[i2c_port] == I2C_BUSY){ \
        busy_status = HAL_I2C_STATUS_ERROR_BUSY; \
    } else { \
        s_i2c_master_status[i2c_port] = I2C_BUSY;  \
        busy_status = HAL_I2C_STATUS_OK; \
    } \
    hal_nvic_restore_interrupt_mask(saved_mask); \
}while(0)

#define I2C_SET_IDLE(i2c_port)   \
do{  \
    s_i2c_master_status[i2c_port] = I2C_IDLE;  \
}while(0)

#ifdef I2C_ENABLE_DEBUG_LOG
#define I2C_DEBUG_FLOW_ARRAY_COUNT  (200)
#define I2C_DEBUG_LOG(i2c_port,i2c_api_type) log_hal_error("[D_I2C%d][type= %d]\r\n",i2c_port,i2c_api_type)
#define I2C_DEBUG_RECORD_FLOW(i2c_port,i2c_api_type) i2c_debug_record_flow(i2c_port,i2c_api_type)
#else
#define I2C_DEBUG_LOG(i2c_port,i2c_api_type)
#define I2C_DEBUG_RECORD_FLOW(i2c_port,i2c_api_type)
#endif

#define I2C_STOP(i2c_port) i2c_master_register[i2c_port]->START = 0x0
#define I2C_START(i2c_port) i2c_master_register[i2c_port]->START = 0x1
#define I2C_MASK_INTERRUPT(i2c_port) i2c_master_register[i2c_port]->INTR_MASK = 0x0
#define I2C_UNMASK_INTERRUPT(i2c_port) i2c_master_register[i2c_port]->INTR_MASK = 0x010101
#define I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port) (i2c_master_register[i2c_port]->INTR_STAT)
#define I2C_IF_RECEIVE(i2c_port) ((i2c_master_register[i2c_port]->SLAVE_ADDR) & 0x1)
#define I2C_IF_DMA_ENABLE(i2c_port) (((i2c_master_register[i2c_port]->CONTROL) >> I2C_CONTROL_REGISTER_DMA_BIT) & 0x1)
#define I2C_GET_SLAVE_ADDRESS(i2c_port) ((i2c_master_register[i2c_port]->SLAVE_ADDR) >> 1)
#define I2C_CLEAR_FIFO(i2c_port) (i2c_master_register[i2c_port]->FIFO_ADDR_CLR = 0x1)
#define I2C_CLEAR_INTERRUPT_STATUS(i2c_port) (i2c_master_register[i2c_port]->INTR_STAT = 0x010101)
#define I2C_QUERY_RUNNING_STATUS(i2c_port)  (i2c_master_register[i2c_port]->START & 0x1)
#define I2C_RESET(i2c_port) (i2c_master_register[i2c_port]->SOFTRESET = 0x1)



#define I2C_GET_SEND_ADDRESS(slave_address)  ((slave_address<<1) & 0xFE)
#define I2C_GET_RECEIVE_ADDRESS(slave_address)  ((slave_address<<1) | 0x01)


#define I2C_IF_DMA_FINISH(i2c_port) (true == s_i2c_dma_parameter[i2c_port].dma_finish)

#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
void        i2c_sw_fifo_init(uint32_t i2c_port);
uint32_t    i2c_push_sw_fifo(uint32_t i2c_port, hal_i2c_sw_fifo_node_t *tansfer_config, uint32_t transfer_cnt);
uint32_t    i2c_pop_sw_fifo(uint32_t i2c_port, hal_i2c_sw_fifo_node_t *transfer_config, uint32_t transfer_cnt);
int32_t     i2c_get_sw_fifo_remain_transfer_cnt(uint32_t i2c_port);
int32_t     i2c_get_sw_fifo_available_transfer_cnt(uint32_t i2c_port);
#endif

void    i2c_set_frequency(uint32_t i2c_port, uint32_t transfer_frequency);
void    i2c_set_io_config(uint32_t i2c_port, uint32_t io_config);
void    i2c_config_hardware(uint32_t i2c_port, i2c_transaction_type_t transaction_type, uint8_t slave_address, i2c_transaction_size_t *config_size);
void    i2c_write_fifo(uint32_t i2c_port, const uint8_t *data, uint32_t size);
void    i2c_read_fifo(uint32_t i2c_port, uint8_t *buffer, uint32_t size);
void    i2c_master_wait_transaction_finish(hal_i2c_port_t i2c_port, i2c_transaction_type_t transaction_type, volatile uint32_t *flag);
void    i2c_get_transaction_status_special(hal_i2c_port_t i2c_port, volatile uint32_t  *i2c_need_query_flag);
void    i2c_master_get_polling_status(hal_i2c_port_t i2c_port);
void    i2c_debug_record_flow(hal_i2c_port_t i2c_port, uint32_t type);
void    i2c_master_dump_register(hal_i2c_port_t i2c_port);

i2c_transaction_status_t    i2c_get_transaction_status(uint32_t i2c_port, volatile uint32_t  *flag);
i2c_transaction_status_t    i2c_get_irq_status(uint32_t i2c_port);
pdma_status_t               i2c_dma_init(uint32_t i2c_port, bool is_tx, uint32_t size);
pdma_status_t               i2c_dma_deinit(uint32_t i2c_port, bool is_tx);


#endif /* #if defined(HAL_I2C_MASTER_MODULE_ENABLED) */
#endif /* #ifndef _HAL_I2C_MASTER_INTERNAL_H_ */



