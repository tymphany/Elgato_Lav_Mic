/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
#ifndef __LSM6DS3_H__
#define __LSM6DS3_H__
#include "hal.h"
#include "hal_platform.h"
#include "lsm6ds3_reg.h"
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_common.h"


#define    MIN_ODR(x, y)         (x < y ? x : y)
#define    MAX_ODR(x, y)         (x > y ? x : y)
#define    LSM6DS3_ODR_LSB_TO_HZ(_odr)  (_odr ? (13 << (_odr - 1)) : 0)


typedef union {
    int16_t i16bit[3];
    uint8_t u8bit[6];
} axis3bit16_t;

typedef struct {
    uint8_t enable;
    lsm6ds3_odr_xl_t odr;
    uint16_t odr_hz_val;
    lsm6ds3_xl_fs_t fs;
    uint8_t decimation;
    uint8_t samples_num_in_pattern;
} sensor_lsm6ds3_xl;

typedef struct {
    uint8_t enable;
    lsm6ds3_odr_g_t odr;
    uint16_t odr_hz_val;
    lsm6ds3_fs_g_t fs;
    uint8_t decimation;
    uint8_t samples_num_in_pattern;
} sensor_lsm6ds3_gy;

typedef union {
    uint8_t byte[4];
    uint32_t val;
} timestamp_t;


typedef struct {
    float       temperature;
    uint32_t    pre_sys_tick;
    uint32_t    system_tick;
    uint32_t    timestamp_us;
    uint32_t    packet_len;
    bsp_multi_axis_callback_t user_callback;
    void        *user_data;
    sw_fifo_t    fmt_data_fifo;
    lsm6ds3_odr_xl_t    accr_odr;
    lsm6ds3_odr_g_t     gyro_odr;
    lsm6ds3_odr_fifo_t  fifo_odr;
} lsm6ds3_privte_info_t;


int      lsm6ds3_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config);
int      lsm6ds3_deinit(bsp_multi_axis_handle_t handle);
int      lsm6ds3_set_gy_full_scale(bsp_multi_axis_gyro_range_t range);
int      lsm6ds3_set_xl_full_scale(bsp_multi_axis_accel_range_t range);
int      lsm6ds3_set_data_output_rate(bsp_multi_axis_output_data_rate_t output_data_rate, uint32_t ready_to_read_len);
int      lsm6ds3_format_rawdata(uint8_t *raw_buff, uint32_t raw_buff_len, bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len);
int      lsm6ds3_self_test(bsp_multi_axis_enable_t sensor_en);
void     lsm6ds3_eint_irq_handle();
int      lsm6ds3_read_format_data(bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len);
uint32_t lsm6ds3_get_frame_cnt();
#endif

