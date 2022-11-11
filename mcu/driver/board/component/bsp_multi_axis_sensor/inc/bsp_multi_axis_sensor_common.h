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

#ifndef __BSP_MULTI_AXIS_SENSOR_COMMON_H__
#define __BSP_MULTI_AXIS_SENSOR_COMMON_H__
#include <stdint.h>

#define        BSP_SENSOR_MAGIC_ID            0x47

#define        BSP_SENSOR_MAGICID_OFS        24
#define        BSP_SENSOR_BUS_TYPE_OFS        20
#define        BSP_SENSOR_BUS_PORT_OFS        16
#define        BSP_SENSOR_VENDOR_OFS        12
#define        BSP_SENSOR_USER_ID_OFS        8
#define        BSP_SENSOR_EINT_NUM_OFS        0

#define        BSP_SENSOR_GET_MAGICID(handle)        ((handle >> BSP_SENSOR_MAGICID_OFS)  & 0xFF)
#define        BSP_SENSOR_GET_BUS_TYPE(handle)        ((handle >> BSP_SENSOR_BUS_TYPE_OFS) & 0xF)
#define        BSP_SENSOR_GET_BUS_PORT(handle)        ((handle >> BSP_SENSOR_BUS_PORT_OFS) & 0xF)
#define        BSP_SENSOR_GET_VENDOR(handle)        ((handle >> BSP_SENSOR_VENDOR_OFS)   & 0xF)
#define        BSP_SENSOR_GET_USER_ID(handle)        ((handle >> BSP_SENSOR_USER_ID_OFS)  & 0xF)
#define        BSP_SENSOR_GET_EINT_NUM(handle)        ((handle >> BSP_SENSOR_EINT_NUM_OFS)  & 0xFF)

#define        BSP_SENSOR_SET_MAGICID(handle, type)        (handle |= ((type   & 0xFF) << BSP_SENSOR_MAGICID_OFS))
#define        BSP_SENSOR_SET_BUS_TYPE(handle, type)        (handle |= ((type   & 0xF) << BSP_SENSOR_BUS_TYPE_OFS))
#define        BSP_SENSOR_SET_BUS_PORT(handle, port)        (handle |= ((port   & 0xF) << BSP_SENSOR_BUS_PORT_OFS))
#define        BSP_SENSOR_SET_VENDOR(handle, vendor)        (handle |= ((vendor & 0xF) << BSP_SENSOR_VENDOR_OFS))
#define        BSP_SENSOR_SET_USER_ID(handle, user)        (handle |= ((user   & 0xF) << BSP_SENSOR_USER_ID_OFS))
#define        BSP_SENSOR_SET_EINT_NUM(handle, user)        (handle |= ((user   & 0xFF) << BSP_SENSOR_EINT_NUM_OFS))

#if 1
#define log_sensor_debug(_message, cnt, ...)         LOG_MSGID_D(bsp_multi_sensor, _message, cnt, ##__VA_ARGS__)
#define log_sensor_info(_message, cnt, ...)          LOG_MSGID_I(bsp_multi_sensor, _message, cnt, ##__VA_ARGS__)
#define log_sensor_warn(_message, cnt, ...)          LOG_MSGID_W(bsp_multi_sensor, _message, cnt, ##__VA_ARGS__)
#define log_sensor_error(_message,cnt, ...)          LOG_MSGID_E(bsp_multi_sensor, _message, cnt, ##__VA_ARGS__)

#define log_axis_sensor_debug(_message, cnt, ...)    LOG_MSGID_D(bsp_multi_sensor, _message, cnt, ##__VA_ARGS__)
#define log_axis_sensor_info(_message, cnt, ...)     LOG_MSGID_I(bsp_multi_sensor, _message, cnt, ##__VA_ARGS__)
#define log_axis_sensor_warn(_message, cnt, ...)     LOG_MSGID_W(bsp_multi_sensor, _message, cnt, ##__VA_ARGS__)
#define log_axis_sensor_error(_message,cnt, ...)     LOG_MSGID_E(bsp_multi_sensor, _message, cnt, ##__VA_ARGS__)

#else
#define log_sensor_info(_message, cnt, ...)      printf(_message, ##__VA_ARGS__);
#define log_sensor_warn(_message, cnt, ...)      printf(_message, ##__VA_ARGS__);
#define log_sensor_error(_message, cnt, ...)     printf(_message, ##__VA_ARGS__);
#define log_axis_sensor_debug(_message, cnt, ...)     printf(_message, ##__VA_ARGS__);
#define log_axis_sensor_info(_message, cnt, ...)      printf(_message, ##__VA_ARGS__);
#define log_axis_sensor_warn(_message, cnt, ...)      printf(_message, ##__VA_ARGS__);
#define log_axis_sensor_error(_message,cnt, ...)      printf(_message, ##__VA_ARGS__);
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////

#define MIN(A,B) ( (A) > (B) ? (B) : (A) )
#define MAX(A,B) ( (A) > (B) ? (A) : (B) )

/*!
 * @brief Structure containing gyro xyz data
 */
typedef struct {
    int16_t x;              /**<gyro X  data*/
    int16_t y;              /**<gyro Y  data*/
    int16_t z;              /**<gyro Z  data*/
} sensor_gyro_t;

/*!
 * @brief Structure containing gyro xyz data
 */
typedef struct {
    float x;                /**<gyro X  data*/
    float y;                /**<gyro Y  data*/
    float z;                /**<gyro Z  data*/
} sensor_gyro_float_t;

/*!
 * @brief Structure containing accel xyz data
 */
typedef struct {
    int16_t x;              /**<accel X  data*/
    int16_t y;              /**<accel Y  data*/
    int16_t z;              /**<accel Z  data*/
} sensor_accel_t;

/*!
 * @brief Structure containing accel xyz data
 */
typedef struct {
    float x;                /**<accel X  data*/
    float y;                /**<accel Y  data*/
    float z;                /**<accel Z  data*/
} sensor_accel_float_t;

typedef struct {
    int16_t x;              /**< mag X raw data*/
    int16_t y;              /**< mag Y raw data*/
    int16_t z;              /**< mag Z raw data*/
    uint16_t r;             /**< mag R raw data*/
} sensor_mag_xyzr_t;

typedef struct {
    float system_time_ms;
    uint32_t sensor_time;
    float temperature;
    sensor_mag_xyzr_t mag_xyz;
    sensor_gyro_t gyro_xyz;
    sensor_accel_t accel_xyz;
} bsp_axis_sensor_raw_data_t;

typedef bsp_axis_sensor_raw_data_t sw_fifo_data_t;
typedef struct {
    uint32_t write_ptr;
    uint32_t read_ptr;
    uint32_t fifo_size;
    uint32_t fifo_data_size;
    sw_fifo_data_t *fifo_buffer;
} sw_fifo_t;

typedef struct {
    uint32_t write_ptr;
    uint32_t read_ptr;
    uint32_t fifo_size;
    uint32_t fifo_data_size;
    sw_fifo_data_t *fifo_buffer;
} bsp_axis_sensor_fifo_t;

void     sw_fifo_init(sw_fifo_t *fifo, uint32_t fifo_size, uint32_t fifo_data_size);
uint32_t sw_fifo_push(sw_fifo_t *fifo, sw_fifo_data_t *fifo_data, uint32_t data_cnt);
uint32_t sw_fifo_pop(sw_fifo_t *fifo, sw_fifo_data_t *fifo_data, uint32_t data_cnt);
int32_t  sw_fifo_get_remain_frame_cnt(sw_fifo_t *fifo);
int32_t  sw_fifo_get_available_frame_cnt(sw_fifo_t *fifo);

#endif // __BSP_MULTI_AXIS_SENSOR_COMMON_H__

