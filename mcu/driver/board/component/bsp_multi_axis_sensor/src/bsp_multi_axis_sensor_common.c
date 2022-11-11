/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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
#include <stdlib.h>
#include <string.h>
#include "bsp_multi_axis_sensor_common.h"
#include "hal.h"

#ifndef SENSOR_DEBUG_INF
#define SENSOR_DEBUG_INF(fmt,...) 
#endif


uint8_t  sensor_temp_buffer[4096] = {0};

void sw_fifo_init(sw_fifo_t *fifo, uint32_t fifo_size, uint32_t fifo_data_size)
{
    fifo->fifo_size = fifo_size;
    fifo->fifo_data_size = fifo_data_size;
    fifo->fifo_buffer = (sw_fifo_data_t *)malloc(fifo_size * fifo_data_size);
    if (NULL == fifo->fifo_buffer) {
        SENSOR_DEBUG_INF("sensor_sw_fifo_init failed\r\n");
    }

    fifo->read_ptr = 0;
    fifo->write_ptr = 0;
}

uint32_t sw_fifo_push(sw_fifo_t *fifo, sw_fifo_data_t *fifo_data, uint32_t fifo_data_count)
{
    uint32_t temp_cnt0 = 0;
    uint32_t temp_cnt1 = 0;
    uint32_t temp_cnt2 = 0;
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    sw_fifo_data_t *sw_fifo_p =  fifo->fifo_buffer + fifo->write_ptr;

    /* Check if fifo is full */
    if ((fifo->write_ptr + 1) % fifo->fifo_size == (fifo->read_ptr % fifo->fifo_size)) {
        SENSOR_DEBUG_INF("fifo is full,please pop data\r\n");
        hal_nvic_restore_interrupt_mask(saved_mask);
        return 0;
    }

    /* read index > write index */
    if (fifo->read_ptr > fifo->write_ptr) {
        /* Get min of frame cnt remained in fifo and the frame cnt of user wanted to push in fifo */
        temp_cnt0 = MIN(fifo->read_ptr - fifo->write_ptr - 1, fifo_data_count);

        /* push frame in fifo */
        memcpy(sw_fifo_p, fifo_data, temp_cnt0 * fifo->fifo_data_size);

        fifo->write_ptr += temp_cnt0;

    } else if (fifo->read_ptr == 0) {
        /* Get min of frame cnt remained in fifo and the frame cnt of user wanted to push in fifo */
        temp_cnt0 = MIN(fifo->fifo_size - fifo->write_ptr - 1, fifo_data_count);

        /* push frame in fifo */
        memcpy(sw_fifo_p, fifo_data, temp_cnt0 * fifo->fifo_data_size);

        fifo->write_ptr += temp_cnt0;
    } else {
        temp_cnt1 = MIN(fifo->fifo_size - fifo->write_ptr, fifo_data_count);
        memcpy(sw_fifo_p, fifo_data, temp_cnt1 * fifo->fifo_data_size);
        if (temp_cnt1 < fifo_data_count) {
            temp_cnt2 = MIN(fifo->read_ptr, fifo_data_count - temp_cnt1);
            memcpy(sw_fifo_p, &fifo_data[temp_cnt2] , temp_cnt2 * fifo->fifo_data_size);
            fifo->write_ptr += temp_cnt0;
            temp_cnt0 =  temp_cnt1 + temp_cnt2;
        } else {
            fifo->write_ptr = (fifo->write_ptr + temp_cnt1) % fifo->fifo_size ;
            temp_cnt0 =  temp_cnt1;
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
    return temp_cnt0;
}

uint32_t  sw_fifo_pop(sw_fifo_t *fifo, sw_fifo_data_t *fifo_data, uint32_t fifo_data_count)
{
    uint32_t temp_cnt0 = 0;
    uint32_t temp_cnt1 = 0;
    uint32_t temp_cnt2 = 0;
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    sw_fifo_data_t *sw_fifo_p =  fifo->fifo_buffer + fifo->read_ptr;

    if (fifo->read_ptr == fifo->write_ptr) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return 0;
    } else if (fifo->write_ptr > fifo->read_ptr) {
        temp_cnt0 = MIN(fifo->write_ptr - fifo->read_ptr, fifo_data_count);

        memcpy(fifo_data, sw_fifo_p , temp_cnt0 * fifo->fifo_data_size);

        fifo->read_ptr += temp_cnt0;
    } else {
        temp_cnt1 = MIN(fifo->fifo_size - fifo->read_ptr, fifo_data_count);
        memcpy(fifo_data, sw_fifo_p, temp_cnt1 * fifo->fifo_data_size);
        if (temp_cnt1 < fifo_data_count) {
            temp_cnt2 = MIN(fifo->write_ptr, fifo_data_count - temp_cnt1);
            memcpy(fifo_data, fifo->fifo_buffer , temp_cnt2 * fifo->fifo_data_size);
            fifo->read_ptr += temp_cnt0;
            temp_cnt0 =  temp_cnt1 + temp_cnt2;
        } else {
            fifo->read_ptr = (fifo->read_ptr + temp_cnt1) % fifo->fifo_size ;
            temp_cnt0 =  temp_cnt1;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    return temp_cnt0;
}

int32_t sw_fifo_get_remain_frame_cnt(sw_fifo_t *fifo)
{
    int32_t cnt = fifo->read_ptr - fifo->write_ptr - 1;
    if (cnt < 0) {
        cnt += fifo->fifo_size;
    }

    return cnt;

}

int32_t sw_fifo_get_available_frame_cnt(sw_fifo_t *fifo)
{
    int32_t cnt = fifo->write_ptr - fifo->read_ptr ;
    if (cnt < 0) {
        cnt += fifo->fifo_size ;
    }

    return cnt;
}
