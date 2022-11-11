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
#include "lsm6dsl.h"
#include "lsm6dsl_reg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include <math.h>
#include <string.h>
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"
#include "airo_platform_bus.h"

#define     FREE_COUNT_32K_TO_MS(count)  ((uint32_t)(((uint64_t)count * 1000) / 32768))

#define     LSM6DS3_I2C_PORT            AXIS_SENSOR_IIC_PORT
#define     LSM6DS3_SLV_ADDR            0x6A

extern uint32_t lsm6ds3_get_frame_cnt();

ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static  uint8_t  g_rx_sensor_raw_data[4096] = {0};
static lsm6dsl_privte_info_t                              g_priv_info_sl = {0};
uint8_t *gp_rx_sensor_raw_data;

////////////////////////////Airoha Platform Function Start ////////////////////////////////////////

static void    platform_bus_read_completed_callback(void *user_data)
{
    uint32_t uhandle = (uint32_t)user_data;
    uint32_t frame_cnt = 0;

    if (g_priv_info_sl.user_callback != NULL) {
        frame_cnt = lsm6ds3_get_frame_cnt();
        g_priv_info_sl.user_callback(uhandle, frame_cnt, g_priv_info_sl.user_data);
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

///////////////////////////////ST LSM6DS3 Sensor Driver Start////////////////////////////
void    lsm6dsl_get_gy_xl_odr(bsp_multi_axis_output_data_rate_t odr, lsm6dsl_odr_xl_t *podr_xl, lsm6dsl_odr_g_t *podr_gy, lsm6dsl_odr_fifo_t *podr_ff)
{
    lsm6dsl_odr_xl_t odr_xl = LSM6DSL_XL_ODR_26Hz;
    lsm6dsl_odr_g_t  odr_gy = LSM6DSL_GY_ODR_26Hz;
    lsm6dsl_odr_fifo_t odr_ff = LSM6DSL_FIFO_26Hz;

    switch(odr) {
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_25HZ:{
                odr_xl = LSM6DSL_XL_ODR_26Hz;
                odr_gy = LSM6DSL_GY_ODR_26Hz;
                odr_ff = LSM6DSL_FIFO_26Hz;
            };break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_50HZ:{
                odr_xl = LSM6DSL_XL_ODR_52Hz;
                odr_gy = LSM6DSL_GY_ODR_52Hz;
                odr_ff = LSM6DSL_FIFO_52Hz;
            }break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_100HZ:{
                odr_xl = LSM6DSL_XL_ODR_104Hz;
                odr_gy = LSM6DSL_GY_ODR_104Hz;
                odr_ff = LSM6DSL_FIFO_104Hz;
            }break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_200HZ:{
                odr_xl = LSM6DSL_XL_ODR_208Hz;
                odr_gy = LSM6DSL_GY_ODR_208Hz;
                odr_ff = LSM6DSL_FIFO_208Hz;
            }break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_400HZ:{
                odr_xl = LSM6DSL_XL_ODR_416Hz;
                odr_gy = LSM6DSL_GY_ODR_416Hz;
                odr_ff = LSM6DSL_FIFO_416Hz;
            }break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_800HZ:{
                odr_xl = LSM6DSL_XL_ODR_833Hz;
                odr_gy = LSM6DSL_GY_ODR_833Hz;
                odr_ff = LSM6DSL_FIFO_833Hz;
            }break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_1600HZ:{
                odr_xl = LSM6DSL_XL_ODR_1k66Hz;
                odr_gy = LSM6DSL_GY_ODR_1k66Hz;
                odr_ff = LSM6DSL_FIFO_1k66Hz;
            }break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_3200HZ:{
                odr_xl = LSM6DSL_XL_ODR_3k33Hz;
                odr_gy = LSM6DSL_GY_ODR_3k33Hz;
                odr_ff = LSM6DSL_FIFO_3k33Hz;
            }break;
    }
    if (podr_xl != NULL) {
        *podr_xl = odr_xl;
    }
    if(podr_gy != NULL) {
        *podr_gy = odr_gy;
    }
    if(podr_ff != NULL) {
        *podr_ff = odr_ff;
    }
}


static  stmdev_ctx_t     dev_ctx = {
    .write_reg          = platform_write,
    .read_reg           = platform_read,
    .read_reg_with_dma  = platform_read_with_dma,
    .handle             = NULL
};


int    lsm6dsl_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config)
{
    uint8_t whoamI, rst;
    uint32_t tick_pre, tick_cur, tick_dur;

    memset(&g_priv_info_sl, 0, sizeof(g_priv_info_sl));
    g_priv_info_sl.user_callback = config->call_back;
    g_priv_info_sl.user_data     = config->user_data;
    dev_ctx.handle            = (void *)handle;

    if (BSP_SENSOR_GET_BUS_TYPE(handle) == BSP_MULTI_AXIS_BUS_TYPE_SPI) {
        gp_rx_sensor_raw_data = &g_rx_sensor_raw_data[1];
    } else {
        gp_rx_sensor_raw_data = g_rx_sensor_raw_data;
    }

    platform_init((void *) handle, (platform_bus_init_config_t *)&config->bus_config); /* init platform bus */
    platform_delay(BOOT_TIME);

    /* Check device ID */
    whoamI = 0;
    lsm6dsl_device_id_get(&dev_ctx, &whoamI);
    if ( whoamI != LSM6DSL_ID ) {
        log_sensor_error("[sensor] lsm6dsl init fail: ID not match(expect:0x%x,real:0x%x)\r\n", 2, LSM6DSL_ID, whoamI);
        return -1;
    }
    /* Restore default configuration */
    lsm6dsl_reset_set(&dev_ctx, PROPERTY_ENABLE);
    do {
      lsm6dsl_reset_get(&dev_ctx, &rst);
    } while (rst);


    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_pre);
    do {
        lsm6dsl_reset_get(&dev_ctx, &rst);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_cur);
        hal_gpt_get_duration_count(tick_pre, tick_cur, &tick_dur);
        if (tick_dur > LSM6DS3_WAIT_TIMEOUT) {
            log_sensor_error("[sensor] lsm6dsl init fail: wait reset state timeout\r\n", 0);
            return -2;
        }
    } while (rst);

    /* Enable Block Data Update */
    lsm6dsl_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
}



#define     LSM6DSL_PATTERN_LEN     12


int    lsm6dsl_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config)
{
    lsm6dsl_odr_xl_t odr_xl;
    lsm6dsl_odr_g_t  odr_gy;
    lsm6dsl_odr_fifo_t odr_ff;
    lsm6dsl_int1_route_t int1_route = 0;
    lsm6dsl_int2_route_t int2_route = 0;
    
    lsm6dsl_get_gy_xl_odr(config->sensor_odr, &odr_xl, &odr_gy, &odr_ff);
    
    lsm6dsl_fifo_mode_set(&dev_ctx, LSM6DSL_BYPASS_MODE);
    lsm6dsl_fifo_watermark_set(&dev_ctx, config->frame_threshold * LSM6DSL_PATTERN_LEN);

    /* config accel  range & odr */
    if(config->enable & BSP_MULTI_AXIS_ACCEL_ENABLE) {
        lsm6dsl_fs_xl_t xl_range = BSP_MULTI_AXIS_ACCEL_RANGE_2G;

        switch(config->accel_range){
            case BSP_MULTI_AXIS_ACCEL_RANGE_2G:  xl_range = LSM6DSL_2g;break;
            case BSP_MULTI_AXIS_ACCEL_RANGE_4G:  xl_range = LSM6DSL_4g;break;
            case BSP_MULTI_AXIS_ACCEL_RANGE_8G:  xl_range = LSM6DSL_8g;break;
            case BSP_MULTI_AXIS_ACCEL_RANGE_16G: xl_range = LSM6DSL_16g;break;
        }
        lsm6dsl_xl_full_scale_set(&dev_ctx, xl_range);
        lsm6dsl_xl_data_rate_set( &dev_ctx, odr_xl);
        g_priv_info_sl.xl_range = xl_range;
    } else {
        lsm6dsl_xl_data_rate_set( &dev_ctx, LSM6DSL_XL_ODR_OFF);
    }

    /* config gyro range & odr */
    if (config->enable & BSP_MULTI_AXIS_GYRO_ENABLE) {
        lsm6dsl_fs_g_t gy_range = BSP_MULTI_AXIS_GYRO_RANGE_125_DEG_SEC;

        switch(config->accel_range){
            case BSP_MULTI_AXIS_GYRO_RANGE_2000_DEG_SEC:    gy_range = LSM6DSL_2000dps;break;
            case BSP_MULTI_AXIS_GYRO_RANGE_1000_DEG_SEC:    gy_range = LSM6DSL_1000dps;break;
            case BSP_MULTI_AXIS_GYRO_RANGE_500_DEG_SEC:     gy_range = LSM6DSL_500dps; break;
            case BSP_MULTI_AXIS_GYRO_RANGE_250_DEG_SEC:     gy_range = LSM6DSL_250dps; break;
            case BSP_MULTI_AXIS_GYRO_RANGE_125_DEG_SEC:     gy_range = LSM6DSL_125dps; break;
        }
        lsm6dsl_gy_full_scale_set(&dev_ctx, gy_range);
        lsm6dsl_gy_data_rate_set( &dev_ctx, odr_gy);
        g_priv_info_sl.gy_range = gy_range;
    } else {
        lsm6dsl_gy_data_rate_set( &dev_ctx, LSM6DSL_GY_ODR_OFF);
    }

    /* Configure filtering chain(No aux interface) */
    /* Accelerometer - analog filter */
    lsm6dsl_xl_filter_analog_set(&dev_ctx, LSM6DSL_XL_ANA_BW_400Hz);
    /* Accelerometer - LPF1 + LPF2 path */
    lsm6dsl_xl_lp2_bandwidth_set(&dev_ctx, LSM6DSL_XL_LOW_NOISE_LP_ODR_DIV_100);
    /* Gyroscope - filtering chain */
    lsm6dsl_gy_band_pass_set(&dev_ctx, LSM6DSL_HP_260mHz_LP1_STRONG);

    /* rounte interrupt signal to INT1 or INT2*/
    int1_route.int1_fth = PROPERTY_ENABLE;
    int2_route.int2_fth = PROPERTY_ENABLE;
    lsm6dsl_data_ready_mode_set(&dev_ctx, LSM6DSL_DRDY_PULSED);
    lsm6dsl_pin_mode_set(&dev_ctx, LSM6DSL_PUSH_PULL);
    lsm6dsl_pin_int1_route_set(&dev_ctx,  int1_route);
    lsm6dsl_pin_int2_route_set(&dev_ctx,  int2_route);
    /* Set FIFO sensor decimator */
    lsm6dsl_fifo_xl_batch_set(&dev_ctx, LSM6DSL_FIFO_XL_NO_DEC);
    lsm6dsl_fifo_gy_batch_set(&dev_ctx, LSM6DSL_FIFO_GY_NO_DEC);
    /* config fifo odr */
    lsm6dsl_fifo_data_rate_set(&dev_ctx,  odr_ff);

    lsm6dsl_fifo_mode_set(&dev_ctx, LSM6DSL_FIFO_MODE);
}


uint32_t lsm6dsl_get_frame_cnt()
{
    uint16_t dlen = 0;
    
    lsm6dsl_fifo_data_level_get(&dev_ctx, &dlen);
    return (uint32_t) dlen;
}

static int16_t data_raw_acceleration[3];
static int16_t data_raw_angular_rate[3];
static int16_t data_raw_temperature;
static float acceleration_mg[3];
static float angular_rate_mdps[3];
static float temperature_degC;

int     lsm6dsl_read_format_data(bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len)
{
    uint16_t num = 0;
    uint16_t num_pattern = 0;
    uint8_t waterm = 0;
    uint32_t i = 0;


    lsm6dsl_temperature_raw_get(&dev_ctx, &data_raw_temperature);
    temperature_degC = lsm6dsl_from_lsb_to_celsius(
                         data_raw_temperature );

    /* Read number of word in FIFO */
    lsm6dsl_fifo_data_level_get(&dev_ctx, &num);
    num_pattern = num / LSM6DSL_PATTERN_LEN;


    while (num_pattern-- > 0) {
        user_buff[i].head.temperature    = temperature_degC;
        user_buff[i].head.system_time_ms = ;
        user_buff[i].head.sensor_time    = ;
        /* Following the sensors ODR configuration, FIFO pattern is composed
        * by this sequence of samples: GYRO, XL
        */
        lsm6dsl_fifo_raw_data_get(&dev_ctx,
                                data_raw_angular_rate.u8bit,
                                3 * sizeof(int16_t));

        user_buff[i].axis_sensor_data.gyro_data.x = lsm6dsl_from_range_to_mdps(g_priv_info_sl.gy_range, data_raw_angular_rate.i16bit[0]);      
        user_buff[i].axis_sensor_data.gyro_data.y = lsm6dsl_from_range_to_mdps(g_priv_info_sl.gy_range, data_raw_angular_rate.i16bit[1]);      
        user_buff[i].axis_sensor_data.gyro_data.z = lsm6dsl_from_range_to_mdps(g_priv_info_sl.gy_range, data_raw_angular_rate.i16bit[2]);

        lsm6dsl_fifo_raw_data_get(&dev_ctx,
                                data_raw_acceleration.u8bit,
                                3 * sizeof(int16_t));
        user_buff[i].axis_sensor_data.accel_data.x = lsm6dsl_from_range_to_mg(g_priv_info_sl.xl_range, data_raw_angular_rate.i16bit[0]);      
        user_buff[i].axis_sensor_data.accel_data.y = lsm6dsl_from_range_to_mg(g_priv_info_sl.xl_range, data_raw_angular_rate.i16bit[1]);      
        user_buff[i].axis_sensor_data.accel_data.z = lsm6dsl_from_range_to_mg(g_priv_info_sl.xl_range, data_raw_angular_rate.i16bit[2]);
        if(i > user_buff_len) {
            break;
        }
        i++;
    }
}





