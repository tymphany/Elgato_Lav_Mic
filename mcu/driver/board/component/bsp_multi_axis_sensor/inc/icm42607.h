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

#include "hal.h"
#include "hal_platform.h"
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"

#ifndef _ICM42607_H_
#define _ICM42607_H_


enum {
    ICM42607_MCLK_RDY      = 0x00,
    ICM42607_DEVICE_CONFIG = 0x01,
    ICM42607_SIG_PATH_RST  = 0x02,
    ICM42607_DRIV_CFG1     = 0x03,
    ICM42607_DRIV_CFG2     = 0x04,
    ICM42607_DRIV_CFG3     = 0x05,
    ICM42607_INT_CFG       = 0x06,

    ICM42607_TEMP_DATA1    = 0x09,
    ICM42607_TEMP_DATA0    = 0x0A,
    ICM42607_ACCL_DATA_X1  = 0x0B,
    ICM42607_ACCL_DATA_X0  = 0x0C,
    ICM42607_ACCL_DATA_Y1  = 0x0D,
    ICM42607_ACCL_DATA_Y0  = 0x0E,
    ICM42607_ACCL_DATA_Z1  = 0x0F,
    ICM42607_ACCL_DATA_Z0  = 0x10,
    ICM42607_GYRO_DATA_X1  = 0x11,
    ICM42607_GYRO_DATA_X0  = 0x12,
    ICM42607_GYRO_DATA_Y1  = 0x13,
    ICM42607_GYRO_DATA_Y0  = 0x14,
    ICM42607_GYRO_DATA_Z1  = 0x15,
    ICM42607_GYRO_DATA_Z0  = 0x16,
    ICM42607_TMST_FSYNCH   = 0x17,
    ICM42607_TMST_FSYNCL   = 0x18,

    ICM42607_APEX_DATA4    = 0x1D,
    ICM42607_APEX_DATA5    = 0x1E,
    ICM42607_PWR_MGMT0     = 0x1F,
    ICM42607_GYRO_CFG0     = 0x20,
    ICM42607_ACCL_CFG0     = 0x21,
    ICM42607_TEMP_CFG0     = 0x22,
    ICM42607_GYRO_CFG1     = 0x23,
    ICM42607_ACCL_CFG1     = 0x24,
    ICM42607_APEX_CFG0     = 0x25,
    ICM42607_APEX_CFG1     = 0x26,
    ICM42607_WOM_CFG       = 0x27,
    ICM42607_FIFO_CFG1     = 0x28,
    ICM42607_FIFO_CFG2     = 0x29,
    ICM42607_FIFO_CFG3     = 0x2A,
    ICM42607_INT_SOURCE0   = 0x2B,
    ICM42607_INT_SOURCE1   = 0x2C,
    ICM42607_INT_SOURCE3   = 0x2D,
    ICM42607_INT_SOURCE4   = 0x2E,
    ICM42607_FIFO_LOST_PKT0 = 0x2F,
    ICM42607_FIFO_LOST_PKT1 = 0x30,
    ICM42607_APEX_DATA0    = 0x31,
    ICM42607_APEX_DATA1    = 0x32,
    ICM42607_APEX_DATA2    = 0x33,
    ICM42607_APEX_DATA3    = 0x34,
    ICM42607_INTF_CFG0     = 0x35,
    ICM42607_INTF_CFG1     = 0x36,
    
    ICM42607_INT_STATUS_DRDY= 0x39,
    ICM42607_INT_STATUS     = 0x3A,
    ICM42607_INT_STATUS2    = 0x3B,
    ICM42607_INT_STATUS3    = 0x3C,
    ICM42607_FIFO_COUNTH    = 0x3D,
    ICM42607_FIFO_COUNTL    = 0x3E,
    ICM42607_FIFO_DATA      = 0x3F,

    ICM42607_WHO_AM_I       = 0x75,

    ICM42607_BLK_SEL_W      = 0x79,
    ICM42607_MADDR_W        = 0x7A,
    ICM42607_M_W            = 0x7B,
    ICM42607_BLK_SEL_R      = 0x7C,
    ICM42607_MADDR_R        = 0x7D,
    ICM42607_M_R            = 0x7E,
};



typedef enum {
    ICM42607_GY_RANGE_2000 = 0,
    ICM42607_GY_RANGE_1000,
    ICM42607_GY_RANGE_500,
    ICM42607_GY_RANGE_250
}icm42607_gy_rang_t;

typedef enum {
    ICM42607_XL_RANGE_16G = 0,
    ICM42607_XL_RANGE_8G,
    ICM42607_XL_RANGE_4G,
    ICM42607_XL_RANGE_2G
}icm42607_xl_rang_t;


typedef enum {
    ICM42607_GY_ODR_1K6HZ = 5,
    ICM42607_GY_ODR_800HZ,
    ICM42607_GY_ODR_400HZ,
    ICM42607_GY_ODR_200HZ,
    ICM42607_GY_ODR_100HZ,
    ICM42607_GY_ODR_50HZ,
    ICM42607_GY_ODR_25HZ,
    ICM42607_GY_ODR_12HZ
}icm42607_odr_g_t;



typedef enum {
    ICM42607_XL_ODR_1K6HZ = 5,
    ICM42607_XL_ODR_800HZ,
    ICM42607_XL_ODR_400HZ,
    ICM42607_XL_ODR_200HZ,
    ICM42607_XL_ODR_100HZ,
    ICM42607_XL_ODR_50HZ,
    ICM42607_XL_ODR_25HZ,
    ICM42607_XL_ODR_12P5HZ,
    ICM42607_XL_ODR_6P25HZ,
    ICM42607_XL_ODR_3P125HZ,
    ICM42607_XL_ODR_1P562HZ,    
}icm42607_odr_xl_t;

typedef struct {
    uint8_t     header;
    uint8_t     accl_x[2];
    uint8_t     accl_y[2];
    uint8_t     accl_z[2];
    uint8_t     gyro_x[2];
    uint8_t     gyro_y[2];
    uint8_t     gyro_z[2];
    int8_t        temper;
    uint8_t     time_stamp[2];
}icm42607_fifo_packet_t;



typedef struct {
    uint32_t    packet_len;
    bsp_multi_axis_callback_t user_callback;
    void        *user_data;
    icm42607_odr_xl_t   accr_odr;
    icm42607_odr_g_t    gyro_odr;
    uint32_t            xl_range;
    uint32_t            gy_range;
    uint32_t            sys_tick;
    uint32_t            pre_sys_tick;
    float               sys_tm_ms;
    int                 odr_delt;
} icm42607_privte_info_t;

#define     ICM42607_FIFO_HEADER_TIME_AVAILABLE_MASK    (0x3 << 2)
#define     ICM42607_FIFO_HEADER_GYRO_AVAILABLE_MASK    (1 << 5)
#define     ICM42607_FIFO_HEADER_ACCL_AVAILABLE_MASK    (1 << 6)
#define     ICM42607_FIFO_HEADER_FIFO_EMPTY_FLG_MASK    (1 << 7)


int icm42607_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config);
int icm42607_deinit(bsp_multi_axis_handle_t handle);
int icm42607_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config);
int icm42607_get_frame_cnt(bsp_multi_axis_handle_t handle);
int icm42607_read_format_data(bsp_multi_axis_handle_t handle, bsp_multi_axis_data_t *user_buff, uint32_t frame_len);
void icm42607_eint_irq_handle(bsp_multi_axis_handle_t handle);


#endif

