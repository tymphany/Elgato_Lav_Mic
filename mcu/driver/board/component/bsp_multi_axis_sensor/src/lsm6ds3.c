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
#include "lsm6ds3.h"
#include "lsm6ds3_reg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include <math.h>
#include <string.h>
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"
#include "airo_platform_bus.h"
#define FREE_COUNT_32K_TO_MS(count)  ((((count) * 1000.0f) / 32768))

#define     LSM6DS3_I2C_PORT    AXIS_SENSOR_IIC_PORT
#define     LSM6DS3_SLV_ADDR    0x6A

extern uint32_t lsm6ds3_get_frame_cnt();

ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static  uint8_t  g_rx_sensor_raw_data[4096] = {0};
static lsm6ds3_privte_info_t                              g_priv_info = {0};
uint8_t *gp_rx_sensor_raw_data;

////////////////////////////Airoha Platform Function Start ////////////////////////////////////////

static void    platform_bus_read_completed_callback(void *user_data)
{
    uint32_t uhandle = (uint32_t)user_data;
    uint32_t frame_cnt = 0;

    if (g_priv_info.user_callback != NULL) {
        frame_cnt = lsm6ds3_get_frame_cnt();
        g_priv_info.user_callback(uhandle, frame_cnt, g_priv_info.user_data);
    }
}

static void platform_delay(uint32_t ms)
{
    hal_gpt_delay_ms(ms);
}

static int  platform_init(void *handle, platform_bus_init_config_t *config)
{
    return platform_bus_init((uint32_t)handle, config);
}

static int  platform_deinit(void *handle)
{
    return platform_bus_deinit((uint32_t)handle);
}



static int  platform_write(void *handle, uint8_t reg, uint8_t *data, uint16_t size)
{
    platform_transfer_config_t config;

    config.dev_addr = LSM6DS3_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    return platform_bus_write((uint32_t)handle, &config);
}

static int  platform_read(void *handle, uint8_t reg, uint8_t *data, uint16_t size)
{
    platform_transfer_config_t config;

    config.dev_addr = LSM6DS3_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    return platform_bus_read((uint32_t)handle, &config);
}

static int  platform_read_with_dma(void *handle, uint8_t reg, uint8_t *data, uint16_t size)
{
    platform_transfer_dma_config_t    config;

    config.dev_addr = LSM6DS3_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    config.call_back = platform_bus_read_completed_callback;
    config.user_data = handle;

    return platform_bus_read_dma((uint32_t)handle, &config);
}

////////////////////////////////Airoha Platform Function End /////////////////////////////





///////////////////////////////ST LSM6DS3 Sensor Driver Start////////////////////////////
#define     BOOT_TIME               20  //ms
#define     WAIT_TIME_A             200 //ms
#define     WAIT_TIME_G_01          800 //ms
#define     WAIT_TIME_G_02          60  //ms
#define     LSM6DS3_PARTERN_LEN     12
#define     LSM6DS3_WAIT_TIMEOUT    10000  //10ms
#define     WAIT_TIME_G_01          800 //ms
#define     WAIT_TIME_G_02          60 //ms

#define     MIN_ST_LIMIT_mg         90.0f
#define     MAX_ST_LIMIT_mg       1700.0f
#define     MIN_ST_LIMIT_mdps   150000.0f
#define     MAX_ST_LIMIT_mdps   700000.0f
#define     ST_PASS     1U
#define     ST_FAIL     0U


static  stmdev_ctx_t     dev_ctx = {
    .write_reg  = platform_write,
    .read_reg   = platform_read,
    .read_reg_with_dma = platform_read_with_dma,
    .handle     = NULL
};


static sensor_lsm6ds3_xl lsm6ds3_xl_config = {
    PROPERTY_DISABLE,
    LSM6DS3_XL_ODR_52Hz,
    0,
    LSM6DS3_2g,
    0,
    0,
};


static sensor_lsm6ds3_gy lsm6ds3_gy_config = {
    PROPERTY_DISABLE,
    LSM6DS3_GY_ODR_52Hz,
    0,
    LSM6DS3_2000dps,
    0,
    0,
};

static float    g_lsm6ds3_odr_in_hz = 0;
static float    g_lsm6ds3_odr_period_in_us = 0;
int    lsm6ds3_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config)
{
    uint8_t whoamI, rst;
    lsm6ds3_int1_route_t int_1_reg;
    uint32_t tick_pre, tick_cur, tick_dur;

    memset(&g_priv_info, 0, sizeof(g_priv_info));
    g_priv_info.user_callback = config->call_back;
    g_priv_info.user_data     = config->user_data;
    dev_ctx.handle            = (void *)handle;

    if (BSP_SENSOR_GET_BUS_TYPE(handle) == BSP_MULTI_AXIS_BUS_TYPE_SPI) {
        gp_rx_sensor_raw_data = &g_rx_sensor_raw_data[1];
    } else {
        gp_rx_sensor_raw_data = g_rx_sensor_raw_data;
    }

    platform_init((void *) handle, (platform_bus_init_config_t *)&config->bus_config); /* init platform bus */
    platform_delay(BOOT_TIME); /* Wait sensor boot time */
    /* Check device ID */
    lsm6ds3_device_id_get(&dev_ctx, &whoamI);
    if (whoamI != LSM6DS3_ID) {
        log_sensor_error("[sensor] lsm6ds3tr-c init fail: ID not match(expect:0x%x,real:0x%x)\r\n", 2, LSM6DS3_ID, whoamI);
        return -1;
    }

    // log_sensor_info("[sensor] lsm6ds3tr-c reset and wait bootup\r\n", 0);
    /* reset sensor & waiting bootup */
    lsm6ds3_reset_set(&dev_ctx, PROPERTY_ENABLE);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_pre);
    do {
        lsm6ds3_reset_get(&dev_ctx, &rst);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_cur);
        hal_gpt_get_duration_count(tick_pre, tick_cur, &tick_dur);
        if (tick_dur > LSM6DS3_WAIT_TIMEOUT) {
            log_sensor_error("[sensor] lsm6ds3tr-c init fail: wait reset state timeout\r\n", 0);
            return -2;
        }
    } while (rst);

    /* Enable Block Data Update */
    lsm6ds3_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

    /* config INT1 for notify host */
    lsm6ds3_pin_int1_route_get(&dev_ctx, &int_1_reg);
    int_1_reg.int1_fth = PROPERTY_ENABLE;
    lsm6ds3_pin_int1_route_set(&dev_ctx, &int_1_reg);
    lsm6ds3_fifo_xl_batch_set(&dev_ctx, LSM6DS3_FIFO_XL_NO_DEC);
    lsm6ds3_fifo_gy_batch_set(&dev_ctx, LSM6DS3_FIFO_GY_NO_DEC);

    return 0;
}


int    lsm6ds3_deinit(bsp_multi_axis_handle_t handle)
{
    lsm6ds3_fifo_data_rate_set(&dev_ctx, LSM6DS3_FIFO_DISABLE);
    lsm6ds3_xl_data_rate_set(&dev_ctx,  LSM6DS3_XL_ODR_OFF);
    lsm6ds3_gy_data_rate_set(&dev_ctx,  LSM6DS3_GY_ODR_OFF);
    platform_deinit((void *)handle);
    return 0;
}

int lsm6ds3_set_gy_full_scale(bsp_multi_axis_gyro_range_t range)
{
    lsm6ds3_fs_g_t st_strct = LSM6DS3_2000dps;
    // log_sensor_info("[sensor] lsm6ds3tr-c lsm6ds3_set_gy_full_scale\r\n", 0);

    switch (range) {
        case BSP_MULTI_AXIS_GYRO_RANGE_125_DEG_SEC:
            st_strct = LSM6DS3_125dps;
            break;
        case BSP_MULTI_AXIS_GYRO_RANGE_250_DEG_SEC:
            st_strct = LSM6DS3_250dps;
            break;
        case BSP_MULTI_AXIS_GYRO_RANGE_500_DEG_SEC:
            st_strct = LSM6DS3_500dps;
            break;
        case BSP_MULTI_AXIS_GYRO_RANGE_1000_DEG_SEC:
            st_strct = LSM6DS3_1000dps;
            break;
        case BSP_MULTI_AXIS_GYRO_RANGE_2000_DEG_SEC:
            st_strct = LSM6DS3_2000dps;
            break;
    }
    lsm6ds3_gy_config.enable = PROPERTY_ENABLE;
    lsm6ds3_gy_config.fs = st_strct;
    lsm6ds3_gy_full_scale_set(&dev_ctx, st_strct);
    return 0;
}



int lsm6ds3_set_xl_full_scale(bsp_multi_axis_accel_range_t range)
{
    lsm6ds3_xl_fs_t st_strct = LSM6DS3_2g;
    // log_sensor_info("[sensor] lsm6ds3tr-c lsm6ds3_set_xl_full_scale\r\n", 0);

    switch (range) {
        case BSP_MULTI_AXIS_ACCEL_RANGE_2G:
            st_strct = LSM6DS3_2g;
            break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_4G:
            st_strct = LSM6DS3_4g;
            break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_8G:
            st_strct = LSM6DS3_8g;
            break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_16G:
            st_strct = LSM6DS3_16g;
            break;
    }
    lsm6ds3_xl_config.enable = PROPERTY_ENABLE;
    lsm6ds3_xl_config.fs = st_strct;
    lsm6ds3_xl_full_scale_set(&dev_ctx, st_strct);
    return 0;
}


int lsm6ds3_set_data_output_rate(bsp_multi_axis_output_data_rate_t output_data_rate, uint32_t ready_to_read_len)
{
    lsm6ds3_odr_xl_t    xl_odr_config;
    lsm6ds3_odr_g_t     gy_odr_config;
    lsm6ds3_odr_fifo_t  ff_odr_config;
    uint8_t             u8temp = 0;
    // log_sensor_info("[sensor] lsm6ds3tr-c lsm6ds3_set_data_output_rate\r\n", 0);

    switch (output_data_rate) {
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_25HZ:  {
            xl_odr_config = LSM6DS3_XL_ODR_26Hz;
            gy_odr_config = LSM6DS3_GY_ODR_26Hz;
            ff_odr_config = LSM6DS3_FIFO_26Hz;
            g_lsm6ds3_odr_in_hz = 26.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_50HZ:  {
            xl_odr_config = LSM6DS3_XL_ODR_52Hz;
            gy_odr_config = LSM6DS3_GY_ODR_52Hz;
            ff_odr_config = LSM6DS3_FIFO_52Hz;
            g_lsm6ds3_odr_in_hz = 52.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_100HZ:  {
            xl_odr_config = LSM6DS3_XL_ODR_104Hz;
            gy_odr_config = LSM6DS3_GY_ODR_104Hz;
            ff_odr_config = LSM6DS3_FIFO_104Hz;
            g_lsm6ds3_odr_in_hz = 104.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_200HZ:  {
            xl_odr_config = LSM6DS3_XL_ODR_208Hz;
            gy_odr_config = LSM6DS3_GY_ODR_208Hz;
            ff_odr_config = LSM6DS3_FIFO_208Hz;
            g_lsm6ds3_odr_in_hz = 208.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_400HZ:  {
            xl_odr_config = LSM6DS3_XL_ODR_416Hz;
            gy_odr_config = LSM6DS3_GY_ODR_416Hz;
            ff_odr_config = LSM6DS3_FIFO_416Hz;
            g_lsm6ds3_odr_in_hz = 416.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_800HZ:  {
            xl_odr_config = LSM6DS3_XL_ODR_833Hz;
            gy_odr_config = LSM6DS3_GY_ODR_833Hz;
            ff_odr_config = LSM6DS3_FIFO_833Hz;
            g_lsm6ds3_odr_in_hz = 833.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_1600HZ:  {
            xl_odr_config = LSM6DS3_XL_ODR_1k66Hz;
            gy_odr_config = LSM6DS3_GY_ODR_1k66Hz;
            ff_odr_config = LSM6DS3_FIFO_1k66Hz;
            g_lsm6ds3_odr_in_hz = 1660.0f;
        }
        break;
        default:
            return -1;
    }
    lsm6ds3_fifo_watermark_set(&dev_ctx, ready_to_read_len * LSM6DS3_PARTERN_LEN);
    /* Set FIFO mode to Stream mode */
    lsm6ds3_fifo_mode_set(&dev_ctx, LSM6DS3_STREAM_MODE);
    /* set fifo & xl & gyro data output rate */
    lsm6ds3_fifo_data_rate_set(&dev_ctx, ff_odr_config);
    lsm6ds3_xl_data_rate_set(&dev_ctx,  xl_odr_config);
    lsm6ds3_gy_data_rate_set(&dev_ctx,  gy_odr_config);

    /* Set High Resolution Timestamp (25 us tick) */
    lsm6ds3_timestamp_res_set(&dev_ctx, LSM6DS3_LSB_25us);
    /* Enable time stamp */
    lsm6ds3_read_reg(&dev_ctx, LSM6DS3_CTRL10_C, &u8temp, 1);
    u8temp |= (1 << 5);
    lsm6ds3_write_reg(&dev_ctx, LSM6DS3_CTRL10_C, &u8temp, 1);
    g_lsm6ds3_odr_period_in_us = 1000000.0f / g_lsm6ds3_odr_in_hz;

    return 0;
}



int  lsm6ds3_format_rawdata(uint8_t *raw_buff, uint32_t raw_buff_len, bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len)
{
    axis3bit16_t   *raw_data_union = NULL;
    uint32_t        num_pattern, i = 0;
    float           real_data_float[3];
    lsm6ds3_privte_info_t *pinfo;
    uint32_t        tick_dur = 0;
    uint32_t        delt = 0;

    if (raw_buff == NULL || raw_buff_len == 0 || user_buff == NULL || user_buff_len == 0) {
        log_sensor_error("[sensor] lsm6ds3_format_rawdata fail: parameter invalid\r\n", 0);
        return -1;
    }
    if (lsm6ds3_gy_config.enable == PROPERTY_DISABLE && lsm6ds3_xl_config.enable == PROPERTY_DISABLE) {
        log_sensor_error("[sensor] lsm6ds3_format_rawdata fail: accel & gyro disablled\r\n", 0);
        return -2;
    }
    pinfo = &g_priv_info;
    raw_data_union = (axis3bit16_t *) raw_buff;
    /* get frame length */
    num_pattern = raw_buff_len / LSM6DS3_PARTERN_LEN;
    num_pattern = (num_pattern > user_buff_len) ? user_buff_len : num_pattern;
    hal_gpt_get_duration_count(pinfo->pre_sys_tick, pinfo->system_tick, &tick_dur);
    if (num_pattern != 0) {
        delt = tick_dur / num_pattern;
    }

    while (num_pattern-- > 0) {
        if (i >= user_buff_len) {
            break;
        }
        /* Get tempeature and timestamp */
        user_buff[i].head.temperature = pinfo->temperature;
        user_buff[i].head.sensor_time = (pinfo->timestamp_us + g_lsm6ds3_odr_period_in_us * i) / 1000;
        user_buff[i].head.system_time_ms = FREE_COUNT_32K_TO_MS(pinfo->pre_sys_tick + (i + 1) * delt);
        /* covert gyro */
        if (lsm6ds3_gy_config.enable == PROPERTY_ENABLE) {
            real_data_float[0] = lsm6ds3_gyro_fs_to_mdps(lsm6ds3_gy_config.fs, raw_data_union->i16bit[0]) / 1000.0f;
            real_data_float[1] = lsm6ds3_gyro_fs_to_mdps(lsm6ds3_gy_config.fs, raw_data_union->i16bit[1]) / 1000.0f;
            real_data_float[2] = lsm6ds3_gyro_fs_to_mdps(lsm6ds3_gy_config.fs, raw_data_union->i16bit[2]) / 1000.0f;
            user_buff[i].axis_sensor_data_t.gyro_data.x  = real_data_float[0];
            user_buff[i].axis_sensor_data_t.gyro_data.y  = real_data_float[1];
            user_buff[i].axis_sensor_data_t.gyro_data.z  = real_data_float[2];
            raw_data_union++;
        }
        /* covert accel */
        if (lsm6ds3_xl_config.enable == PROPERTY_ENABLE) {
            real_data_float[0] = lsm6ds3_acce_fs_to_mg(lsm6ds3_xl_config.fs, raw_data_union->i16bit[0]) / 1000.0f;
            real_data_float[1] = lsm6ds3_acce_fs_to_mg(lsm6ds3_xl_config.fs, raw_data_union->i16bit[1]) / 1000.0f;
            real_data_float[2] = lsm6ds3_acce_fs_to_mg(lsm6ds3_xl_config.fs, raw_data_union->i16bit[2]) / 1000.0f;
            user_buff[i].axis_sensor_data_t.accel_data.x = real_data_float[0];
            user_buff[i].axis_sensor_data_t.accel_data.y = real_data_float[1];
            user_buff[i].axis_sensor_data_t.accel_data.z = real_data_float[2];
            raw_data_union++;
        }
        /* for next packet */
        i++;
    }
    return i;
}



int  lsm6ds3_self_test(bsp_multi_axis_enable_t sensor_en)
{
    int16_t data_raw[3];
    float val_st_off[3], val_st_on[3], test_val[3];
    uint8_t st_result = 0, i = 0, j = 0;
    uint8_t whoamI, drdy;

    /* Init test platform */
    //platform_init();
    /* Wait sensor boot time */
    platform_delay(BOOT_TIME);

    /* Check device ID */
    lsm6ds3_device_id_get(&dev_ctx, &whoamI);
    if (whoamI != LSM6DS3_ID) {
        log_sensor_error("[sensor] lsm6ds3tr-c self test fail: ID not match(expect:0x%x,real:0x%x)\r\n", 2, LSM6DS3_ID, whoamI);
        return -1;
    }
    /* Enable Block Data Update */
    lsm6ds3_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);

    /*
    * Accelerometer Self Test
    */
    if (sensor_en & BSP_MULTI_AXIS_ACCEL_ENABLE) {
        /* Set Output Data Rate */
        lsm6ds3_xl_data_rate_set(&dev_ctx, LSM6DS3_XL_ODR_52Hz);
        /* Set full scale */
        lsm6ds3_xl_full_scale_set(&dev_ctx, LSM6DS3_4g);
        /* Wait stable output */
        platform_delay(WAIT_TIME_A);

        /* Check if new value available */
        do {
            lsm6ds3_xl_flag_data_ready_get(&dev_ctx, &drdy);
        } while (!drdy);

        /* Read dummy data and discard it */
        lsm6ds3_acceleration_raw_get(&dev_ctx, data_raw);
        /* Read 5 sample and get the average vale for each axis */
        memset(val_st_off, 0x00, 3 * sizeof(float));

        for (i = 0; i < 5; i++) {
            /* Check if new value available */
            do {
                lsm6ds3_xl_flag_data_ready_get(&dev_ctx, &drdy);
            } while (!drdy);

            /* Read data and accumulate the mg value */
            lsm6ds3_acceleration_raw_get(&dev_ctx, data_raw);
            for (j = 0; j < 3; j++) {
                val_st_off[j] += lsm6ds3_from_fs4g_to_mg(data_raw[j]);
            }
        }

        /* Calculate the mg average values */
        for (i = 0; i < 3; i++) {
            val_st_off[i] /= 5.0f;
        }

        /* Enable Self Test positive (or negative) */
        lsm6ds3_xl_self_test_set(&dev_ctx, LSM6DS3_XL_ST_NEGATIVE);
        //lsm6ds3_xl_self_test_set(&dev_ctx, LSM6DS3_XL_ST_POSITIVE);
        /* Wait stable output */
        platform_delay(WAIT_TIME_A);

        /* Check if new value available */
        do {
            lsm6ds3_xl_flag_data_ready_get(&dev_ctx, &drdy);
        } while (!drdy);

        /* Read dummy data and discard it */
        lsm6ds3_acceleration_raw_get(&dev_ctx, data_raw);
        /* Read 5 sample and get the average vale for each axis */
        memset(val_st_on, 0x00, 3 * sizeof(float));

        for (i = 0; i < 5; i++) {
            /* Check if new value available */
            do {
                lsm6ds3_xl_flag_data_ready_get(&dev_ctx, &drdy);
            } while (!drdy);

            /* Read data and accumulate the mg value */
            lsm6ds3_acceleration_raw_get(&dev_ctx, data_raw);

            for (j = 0; j < 3; j++) {
                val_st_on[j] += lsm6ds3_from_fs4g_to_mg(data_raw[j]);
            }
        }

        /* Calculate the mg average values */
        for (i = 0; i < 3; i++) {
            val_st_on[i] /= 5.0f;
        }

        /* Calculate the mg values for self test */
        for (i = 0; i < 3; i++) {
            test_val[i] = fabs((val_st_on[i] - val_st_off[i]));
        }

        /* Check self test limit */
        st_result = ST_PASS;
        for (i = 0; i < 3; i++) {
            if ((MIN_ST_LIMIT_mg > test_val[i]) ||
                (test_val[i] > MAX_ST_LIMIT_mg)) {
                st_result = ST_FAIL;
            }
        }

        /* Disable Self Test */
        lsm6ds3_xl_self_test_set(&dev_ctx, LSM6DS3_XL_ST_DISABLE);
        /* Disable sensor. */
        lsm6ds3_xl_data_rate_set(&dev_ctx, LSM6DS3_XL_ODR_OFF);
    }
    /*
    * Gyroscope Self Test
    */
    if (sensor_en & BSP_MULTI_AXIS_GYRO_ENABLE) {
        /* Set Output Data Rate */
        lsm6ds3_gy_data_rate_set(&dev_ctx, LSM6DS3_GY_ODR_208Hz);
        /* Set full scale */
        lsm6ds3_gy_full_scale_set(&dev_ctx, LSM6DS3_2000dps);
        /* Wait stable output */
        platform_delay(WAIT_TIME_G_01);

        /* Check if new value available */
        do {
            lsm6ds3_gy_flag_data_ready_get(&dev_ctx, &drdy);
        } while (!drdy);

        /* Read dummy data and discard it */
        lsm6ds3_angular_rate_raw_get(&dev_ctx, data_raw);
        /* Read 5 sample and get the average vale for each axis */
        memset(val_st_off, 0x00, 3 * sizeof(float));

        for (i = 0; i < 5; i++) {
            /* Check if new value available */
            do {
                lsm6ds3_gy_flag_data_ready_get(&dev_ctx, &drdy);
            } while (!drdy);

            /* Read data and accumulate the mg value */
            lsm6ds3_angular_rate_raw_get(&dev_ctx, data_raw);

            for (j = 0; j < 3; j++) {
                val_st_off[j] += lsm6ds3_from_fs2000dps_to_mdps(data_raw[j]);
            }
        }

        /* Calculate the mg average values */
        for (i = 0; i < 3; i++) {
            val_st_off[i] /= 5.0f;
        }

        /* Enable Self Test positive (or negative) */
        lsm6ds3_gy_self_test_set(&dev_ctx, LSM6DS3_GY_ST_POSITIVE);
        //lsm6ds3_gy_self_test_set(&dev_ctx, LIS2DH12_GY_ST_NEGATIVE);
        /* Wait stable output */
        platform_delay(WAIT_TIME_G_02);
        /* Read 5 sample and get the average vale for each axis */
        memset(val_st_on, 0x00, 3 * sizeof(float));

        for (i = 0; i < 5; i++) {
            /* Check if new value available */
            do {
                lsm6ds3_gy_flag_data_ready_get(&dev_ctx, &drdy);
            } while (!drdy);

            /* Read data and accumulate the mg value */
            lsm6ds3_angular_rate_raw_get(&dev_ctx, data_raw);

            for (j = 0; j < 3; j++) {
                val_st_on[j] += lsm6ds3_from_fs2000dps_to_mdps(data_raw[j]);
            }
        }

        /* Calculate the mg average values */
        for (i = 0; i < 3; i++) {
            val_st_on[i] /= 5.0f;
        }

        /* Calculate the mg values for self test */
        for (i = 0; i < 3; i++) {
            test_val[i] = fabs((val_st_on[i] - val_st_off[i]));
        }

        /* Check self test limit */
        for (i = 0; i < 3; i++) {
            if ((MIN_ST_LIMIT_mdps > test_val[i]) || (test_val[i] > MAX_ST_LIMIT_mdps)) {
                st_result = ST_FAIL;
            }
        }

        /* Disable Self Test */
        lsm6ds3_gy_self_test_set(&dev_ctx, LSM6DS3_GY_ST_DISABLE);
        /* Disable sensor. */
        lsm6ds3_gy_data_rate_set(&dev_ctx, LSM6DS3_GY_ODR_OFF);
    }
    if (st_result == ST_PASS) {
        log_sensor_error("[sensor] lsm6ds3tr-c self test -> PASS\r\n", 0);
        return 0;
    } else {
        log_sensor_error("[sensor] lsm6ds3tr-c self test -> FAIL\r\n", 0);
        return -1;
    }
}

void lsm6ds3_eint_irq_handle()
{
    uint16_t        fifo_data_len = 0;
    int16_t         i16temp = 0;
    timestamp_t     timestamp;
    lsm6ds3_privte_info_t *pinfo;
    int             status;
    pinfo = &g_priv_info;

    /* Read number of word in FIFO */
    lsm6ds3_fifo_data_level_get(&dev_ctx, &fifo_data_len);
    if (fifo_data_len == 0) {
        log_sensor_error("[sensor] fifo data len = 0 \r\n", 0);
        return;
    }
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &pinfo->system_tick);
    pinfo->packet_len = fifo_data_len / LSM6DS3_PARTERN_LEN;
    /* Get timestamp */
    timestamp.val = 0;
    lsm6ds3_read_reg(&dev_ctx, LSM6DS3_TIMESTAMP0_REG, (uint8_t *)timestamp.byte, 3);
    pinfo->timestamp_us = timestamp.val * 25;
    /* Get temperature */
    lsm6ds3_temperature_raw_get(&dev_ctx, &i16temp);
    pinfo->temperature = lsm6ds3_from_lsb_to_celsius(i16temp);
    status = dev_ctx.read_reg_with_dma(dev_ctx.handle, LSM6DS3_FIFO_DATA_OUT_L, g_rx_sensor_raw_data, fifo_data_len);
    if (status != 0) {
        log_sensor_error("[sensor] lsm6ds3_eint_irq_handle fail: read reg with dma fail %d\r\n", 1, status);
    }
}

int lsm6ds3_read_format_data(bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len)
{
    lsm6ds3_privte_info_t *pinfo;
    int  result;

    pinfo = &g_priv_info;
    result = lsm6ds3_format_rawdata(gp_rx_sensor_raw_data, pinfo->packet_len * LSM6DS3_PARTERN_LEN, user_buff, user_buff_len);
    pinfo->packet_len = 0;
    return result;
}


uint32_t lsm6ds3_get_frame_cnt()
{
    return g_priv_info.packet_len;
}

////////////////////////////////////ST LSM6DS3 Sensor Driver End ////////////////////////////////


