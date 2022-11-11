/*
 ******************************************************************************
 * @file    multi_read_fifo_simple.c
 * @author  Sensors Software Solution Team
 * @brief   This file show how to get data from sensor FIFO.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/*
 * This example was developed using the following STMicroelectronics
 * evaluation boards:
 *
 * - STEVAL_MKI109V3 + STEVAL-MKI196V1
 * - NUCLEO_F411RE + X_NUCLEO_IKS01A3
 * - DISCOVERY_SPC584B + STEVAL-MKI196V1
 *
 * Used interfaces:
 *
 * STEVAL_MKI109V3    - Host side:   USB (Virtual COM)
 *                    - Sensor side: SPI(Default) / I2C(supported)
 *
 * NUCLEO_STM32F411RE - Host side: UART(COM) to USB bridge
 *                    - I2C(Default) / SPI(supported)
 *
 * DISCOVERY_SPC584B  - Host side: UART(COM) to USB bridge
 *                    - Sensor side: I2C(Default) / SPI(supported)
 *
 * If you need to run this example on a different hardware platform a
 * modification of the functions: `platform_write`, `platform_read`,
 * `tx_com` and 'platform_init' is required.
 *
 */

/* STMicroelectronics evaluation boards definition
 *
 * Please uncomment ONLY the evaluation boards in use.
 * If a different hardware is used please comment all
 * following target board and redefine yours.
 */

//#define STEVAL_MKI109V3  /* little endian */
//#define NUCLEO_F411RE    /* little endian */
//#define SPC584B_DIS      /* big endian */

/* ATTENTION: By default the driver is little endian. If you need switch
 *            to big endian please see "Endianness definitions" in the
 *            header file of the driver (_reg.h).
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "hal.h"
#include "hal_log.h"

#include "hal_platform.h"
#include "lsm6dsox.h"
#include "lsm6dsox_reg.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include <math.h>
#include <string.h>
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"
#include "airo_platform_bus.h"

/* Private macro -------------------------------------------------------------*/
#define     BOOT_TIME               10
#define     MIN_ST_LIMIT_mg         50.0f
#define     MAX_ST_LIMIT_mg         1700.0f
#define     MIN_ST_LIMIT_mdps       150000.0f
#define     MAX_ST_LIMIT_mdps       700000.0f


#define     LSM6dSOX_BUFF_SZ         4096
#define     LSM6dSOX_SLV_ADDR        0x6B
#define     LSM6DSOX_PARTERN_LEN     12
#define     LSM6DSOX_WAIT_TIMEOUT    100000 /* us*/

#define     FREE_COUNT_32K_TO_MS(count)  ((((count) * 1000.0f) / 32768))


/* Private function declare ---------------------------------------------------------*/
extern uint32_t lsm6dsox_get_frame_cnt();
static int  platform_write(void *handle, uint8_t reg, uint8_t *data, uint16_t size);
static int  platform_read(void *handle, uint8_t reg, uint8_t *data, uint16_t size);
static int  platform_read_with_dma(void *handle, uint8_t reg, uint8_t *data, uint16_t size);

/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_acceleration;
static axis3bit16_t data_raw_angular_rate;


static   uint8_t                                         *s_rx_sensor_raw_data;
static   lsm6dsox_privte_info_t                           g_priv_info = {0};
static   uint8_t                                         *s_ptr_rx_sensor_raw_data = NULL;
static   float                                            g_lsm6dsox_odr_period_in_us = 0.0;
static   float                                            g_lsm6dsox_odr_in_hz = 0.0;
static   lsm6dsox_fs_g_t                                  s_st_gy_fs = LSM6DSOX_2000dps;
static   lsm6dsox_fs_xl_t                                 s_st_xl_fs = LSM6DSOX_2g;


static  stmdev_ctx_t     dev_ctx = {
    .write_reg  = platform_write,
    .read_reg   = platform_read,
    .read_reg_with_dma = platform_read_with_dma,
    .handle     = NULL
};


////////////////////////////Airoha Platform Function Start ////////////////////////////////////////

static void    platform_bus_read_completed_callback(void *user_data)
{
    uint32_t uhandle = (uint32_t)user_data;
    uint32_t frame_cnt = 0;

    if (g_priv_info.user_callback != NULL) {
        frame_cnt = lsm6dsox_get_frame_cnt();
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

    config.dev_addr = LSM6dSOX_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    return platform_bus_write((uint32_t)dev_ctx.handle, &config);
}

static int  platform_read(void *handle, uint8_t reg, uint8_t *data, uint16_t size)
{
    platform_transfer_config_t config;

    config.dev_addr = LSM6dSOX_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    return platform_bus_read((uint32_t)dev_ctx.handle, &config);
}

static int  platform_read_with_dma(void *handle, uint8_t reg, uint8_t *data, uint16_t size)
{
    platform_transfer_dma_config_t    config;

    config.dev_addr = LSM6dSOX_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    config.call_back = platform_bus_read_completed_callback;
    config.user_data = handle;

    return platform_bus_read_dma((uint32_t)dev_ctx.handle, &config);
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////

int lsm6dsox_set_gy_full_scale(bsp_multi_axis_gyro_range_t range)
{
    lsm6dsox_fs_g_t st_strct = LSM6DSOX_2000dps;
    // log_sensor_info("[sensor] lsm6dsoxxtr-c lsm6dsoxx_set_gy_full_scale\r\n", 0);

    switch (range) {
        case BSP_MULTI_AXIS_GYRO_RANGE_125_DEG_SEC:
            st_strct = LSM6DSOX_125dps;
            break;
        case BSP_MULTI_AXIS_GYRO_RANGE_250_DEG_SEC:
            st_strct = LSM6DSOX_250dps;
            break;
        case BSP_MULTI_AXIS_GYRO_RANGE_500_DEG_SEC:
            st_strct = LSM6DSOX_500dps;
            break;
        case BSP_MULTI_AXIS_GYRO_RANGE_1000_DEG_SEC:
            st_strct = LSM6DSOX_1000dps;
            break;
        case BSP_MULTI_AXIS_GYRO_RANGE_2000_DEG_SEC:
            st_strct = LSM6DSOX_2000dps;
            break;
    }
    lsm6dsox_gy_full_scale_set(&dev_ctx, st_strct);
    return st_strct;
}


int lsm6dsox_set_xl_full_scale(bsp_multi_axis_accel_range_t range)
{
    lsm6dsox_fs_xl_t st_strct = LSM6DSOX_2g;
    // log_sensor_info("[sensor] lsm6dsoxxtr-c lsm6dsoxx_set_xl_full_scale\r\n", 0);

    switch (range) {
        case BSP_MULTI_AXIS_ACCEL_RANGE_2G:
            st_strct = LSM6DSOX_2g;
            break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_4G:
            st_strct = LSM6DSOX_4g;
            break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_8G:
            st_strct = LSM6DSOX_8g;
            break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_16G:
            st_strct = LSM6DSOX_16g;
            break;
    }
    lsm6dsox_xl_full_scale_set(&dev_ctx, st_strct);
    return st_strct;
}




int lsm6dsox_set_data_output_rate(bsp_multi_axis_output_data_rate_t output_data_rate, uint32_t ready_to_read_len, uint8_t enable)
{
    lsm6dsox_odr_xl_t    xl_odr_config;
    lsm6dsox_odr_g_t     gy_odr_config;
    lsm6dsox_bdr_xl_t    ff_odr_config;
    //uint8_t             u8temp = 0;
    // log_sensor_info("[sensor] lsm6dsoxxtr-c lsm6dsoxx_set_data_output_rate\r\n", 0);

    switch (output_data_rate) {
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_25HZ:  {
            xl_odr_config = LSM6DSOX_XL_ODR_26Hz;
            gy_odr_config = LSM6DSOX_GY_ODR_26Hz;
            ff_odr_config = LSM6DSOX_XL_BATCHED_AT_26Hz;
            g_lsm6dsox_odr_in_hz = 26.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_50HZ:  {
            xl_odr_config = LSM6DSOX_XL_ODR_52Hz;
            gy_odr_config = LSM6DSOX_GY_ODR_52Hz;
            ff_odr_config = LSM6DSOX_XL_BATCHED_AT_52Hz;
            g_lsm6dsox_odr_in_hz = 52.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_100HZ:  {
            xl_odr_config = LSM6DSOX_XL_ODR_104Hz;
            gy_odr_config = LSM6DSOX_GY_ODR_104Hz;
            ff_odr_config = LSM6DSOX_XL_BATCHED_AT_104Hz;
            g_lsm6dsox_odr_in_hz = 104.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_200HZ:  {
            xl_odr_config = LSM6DSOX_XL_ODR_208Hz;
            gy_odr_config = LSM6DSOX_GY_ODR_208Hz;
            ff_odr_config = LSM6DSOX_XL_BATCHED_AT_208Hz;
            g_lsm6dsox_odr_in_hz = 208.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_400HZ:  {
            xl_odr_config = LSM6DSOX_XL_ODR_417Hz;
            gy_odr_config = LSM6DSOX_GY_ODR_417Hz;
            ff_odr_config = LSM6DSOX_XL_BATCHED_AT_417Hz;
            g_lsm6dsox_odr_in_hz = 417.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_800HZ:  {
            xl_odr_config = LSM6DSOX_XL_ODR_833Hz;
            gy_odr_config = LSM6DSOX_GY_ODR_833Hz;
            ff_odr_config = LSM6DSOX_XL_BATCHED_AT_833Hz;
            g_lsm6dsox_odr_in_hz = 833.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_1600HZ:  {
            xl_odr_config = LSM6DSOX_XL_ODR_1667Hz;
            gy_odr_config = LSM6DSOX_GY_ODR_1667Hz;
            ff_odr_config = LSM6DSOX_XL_BATCHED_AT_1667Hz;
            g_lsm6dsox_odr_in_hz = 1667.0f;
        }
        break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_3200HZ:  {
            xl_odr_config = LSM6DSOX_XL_ODR_3333Hz;
            gy_odr_config = LSM6DSOX_GY_ODR_3333Hz;
            ff_odr_config = LSM6DSOX_XL_BATCHED_AT_3333Hz;
            g_lsm6dsox_odr_in_hz = 3333.0f;
        }
        break;
        default:
            return -1;
    }

    ff_odr_config = ff_odr_config;
    lsm6dsox_fifo_watermark_set(&dev_ctx, ready_to_read_len * 2);

    /* Set FIFO batch XL/Gyro ODR to 12.5Hz */
    if (enable & BSP_MULTI_AXIS_ACCEL_ENABLE) {
        lsm6dsox_fifo_xl_batch_set(&dev_ctx, xl_odr_config);
    }
    if (enable  & BSP_MULTI_AXIS_GYRO_ENABLE ) {
        lsm6dsox_fifo_gy_batch_set(&dev_ctx, gy_odr_config);
    }
    /* Set FIFO mode to Stream mode (aka Continuous Mode) */
    lsm6dsox_fifo_mode_set(&dev_ctx, LSM6DSOX_STREAM_MODE);

    /* set xl & gyro data output rate */
    if (enable & BSP_MULTI_AXIS_ACCEL_ENABLE) {
        lsm6dsox_xl_data_rate_set(&dev_ctx, xl_odr_config);
    }
    if (enable  & BSP_MULTI_AXIS_GYRO_ENABLE ) {
        lsm6dsox_gy_data_rate_set(&dev_ctx, gy_odr_config);
    }
    /* Set High Resolution Timestamp (25 us tick) */
    //lsm6dsox_timestamp_res_set(&dev_ctx, LSM6DS3_LSB_25us);
    /* Enable time stamp */
    lsm6dsox_timestamp_set(&dev_ctx, PROPERTY_ENABLE);

    g_lsm6dsox_odr_period_in_us = 1000000.0f / g_lsm6dsox_odr_in_hz;
    return 0;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////


int     lsm6dsox_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config)
{
    uint8_t whoamI, rst;
    lsm6dsox_pin_int1_route_t int1_route;
    uint32_t tick_pre, tick_cur, tick_dur;

    memset(&g_priv_info, 0, sizeof(g_priv_info));
    g_priv_info.user_callback = config->call_back;
    g_priv_info.user_data     = config->user_data;
    dev_ctx.handle            = (void *)handle;

    platform_init((void *) handle, (platform_bus_init_config_t *)&config->bus_config); /* init platform bus */
    platform_delay(BOOT_TIME); /* Wait sensor boot time */
    lsm6dsox_device_id_get(&dev_ctx, &whoamI);
    if (whoamI != LSM6DSOX_ID) {
        log_sensor_error("[sensor] lsm6dsox init fail: ID not match(expect:0x%x,real:0x%x)\r\n", 2, LSM6DSOX_ID, whoamI);
        return BSP_MULTI_AXIS_GET_CHIP_ID_ERROR;
    }
    /* Restore default configuration */
    lsm6dsox_reset_set(&dev_ctx, PROPERTY_ENABLE);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_pre);
    do {
        lsm6dsox_reset_get(&dev_ctx, &rst);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_cur);
        hal_gpt_get_duration_count(tick_pre, tick_cur, &tick_dur);
        if (tick_dur > LSM6DSOX_WAIT_TIMEOUT) {
            log_sensor_error("[sensor] lsm6dsox init fail: wait reset state timeout\r\n", 0);
            return BSP_MULTI_AXIS_TIME_OUT_ERROR;
        }
    } while (rst);
    /* Disable I3C interface */
    lsm6dsox_i3c_disable_set(&dev_ctx, LSM6DSOX_I3C_DISABLE);
    /* Enable Block Data Update */
    lsm6dsox_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
    lsm6dsox_fifo_mode_set(&dev_ctx, LSM6DSOX_STREAM_MODE);


    /* Enable drdy 75 us pulse: uncomment if interrupt must be pulsed */
    lsm6dsox_data_ready_mode_set(&dev_ctx, LSM6DSOX_DRDY_PULSED);
    /* Uncomment if interrupt generation on Free Fall INT1 pin */
    lsm6dsox_pin_int1_route_get(&dev_ctx, &int1_route);
    int1_route.fifo_th = PROPERTY_ENABLE;    
    lsm6dsox_pin_int1_route_set(&dev_ctx, int1_route);

    /* Uncomment if interrupt generation on Free Fall INT2 pin */
    //lsm6dsox_pin_int2_route_get(&dev_ctx, &int2_route);
    //int2_route.reg.int2_ctrl.int2_fifo_th = PROPERTY_ENABLE;
    //lsm6dsox_pin_int2_route_set(&dev_ctx, &int2_route);

    /*init timestamp and enable it */
    lsm6dsox_timestamp_rst(&dev_ctx);
    lsm6dsox_timestamp_set(&dev_ctx, 1);

    s_rx_sensor_raw_data = pvPortMallocNC(LSM6dSOX_BUFF_SZ);
    if(s_rx_sensor_raw_data == NULL){
        log_sensor_error("[sensor] lsm6dsox init fail: malloc mem error\r\n", 0);
        return BSP_MULTI_AXIS_NULL_POINT_ERROR;
    }
    if (BSP_SENSOR_GET_BUS_TYPE(handle) == BSP_MULTI_AXIS_BUS_TYPE_SPI) {
        s_ptr_rx_sensor_raw_data = &s_rx_sensor_raw_data[1];
    } else {
        s_ptr_rx_sensor_raw_data = s_rx_sensor_raw_data;
    }

    return BSP_MULTI_AXIS_OK;
}


int     lsm6dsox_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config)
{
    lsm6dsox_privte_info_t *pinfo;

    pinfo = &g_priv_info;
    if (config == NULL || config->enable == 0) {
        log_sensor_error("[sensor] lsm6dsox config fail: invalid para\r\n", 0);
        return BSP_MULTI_AXIS_INVALID_PARA;
    }
    if (config->enable & BSP_MULTI_AXIS_ACCEL_ENABLE) {
        s_st_xl_fs = lsm6dsox_set_xl_full_scale(config->accel_range);
    }
    if (config->enable & BSP_MULTI_AXIS_GYRO_ENABLE) {
        s_st_gy_fs = lsm6dsox_set_gy_full_scale(config->gyro_range);
    }
    lsm6dsox_set_data_output_rate(config->sensor_odr, config->frame_threshold, config->enable);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &pinfo->pre_sys_tick);
    return BSP_MULTI_AXIS_OK;
}




int    lsm6dsox_deinit(bsp_multi_axis_handle_t handle)
{
    uint32_t tick_pre, tick_cur, tick_dur;
    uint8_t rst;

    /* Restore default configuration */
    lsm6dsox_reset_set(&dev_ctx, PROPERTY_ENABLE);
    vPortFreeNC(s_rx_sensor_raw_data);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_pre);
    do {
        lsm6dsox_reset_get(&dev_ctx, &rst);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_cur);
        hal_gpt_get_duration_count(tick_pre, tick_cur, &tick_dur);
        if (tick_dur > LSM6DSOX_WAIT_TIMEOUT) {
            log_sensor_error("[sensor] lsm6dsox deinit fail: wait reset state timeout\r\n", 0);
            return BSP_MULTI_AXIS_TIME_OUT_ERROR;
        }
    } while (rst);
    platform_deinit((void *)handle);
    return BSP_MULTI_AXIS_OK;
}


int     lsm6dsox_read_format_data(bsp_multi_axis_data_t *user_buff, uint32_t frame_len)
{
    uint16_t num = 0;
    int16_t  temperature;
    lsm6dsox_fifo_tag_t reg_tag;
    uint32_t xl_cnt = 0, gy_cnt = 0, i =0;
    lsm6dsox_privte_info_t *pinfo;
    axis3bit16_t dummy;
    uint32_t    tm_delt,tick_dur;

    pinfo = &g_priv_info;

    /* get fifo length(tag + 6byte)*/
    lsm6dsox_fifo_data_level_get(&dev_ctx, &num);
    /* get sensor temperature */
    lsm6dsox_temperature_raw_get(&dev_ctx, &temperature);
    pinfo->temperature = lsm6dsox_from_lsb_to_celsius(temperature);
    hal_gpt_get_duration_count(pinfo->pre_sys_tick, pinfo->system_tick, &tick_dur);
    tm_delt = tick_dur / (num >> 1);
    //printf("[bsp][sensor] lsm6dsox_read_format_data: raw time(pre:%d, cur:%d, dlt:%d)", pinfo->pre_sys_tick, pinfo->system_tick, tm_delt);
    while (num--) {
        if(i < frame_len) {
            user_buff[i].head.temperature    = pinfo->temperature;
            user_buff[i].head.sensor_time    = (pinfo->timestamp_us + g_lsm6dsox_odr_period_in_us * i) / 1000;
            user_buff[i].head.system_time_ms = (float)(FREE_COUNT_32K_TO_MS(pinfo->pre_sys_tick + tm_delt * (i + 1)));
            i++;
        }
        /* Read FIFO tag */
        lsm6dsox_fifo_sensor_tag_get(&dev_ctx, &reg_tag);
        switch (reg_tag) {
          case LSM6DSOX_XL_NC_TAG: {
                memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
                lsm6dsox_fifo_out_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
                /* if xl data size bigger than buffer, then continue */
                if(xl_cnt >= frame_len){
                    continue;
                }
                user_buff[xl_cnt].axis_sensor_data_t.accel_data.x =
                    (double) lsm6dsox_acce_fs_to_mg(s_st_xl_fs, data_raw_acceleration.i16bit[0]) / (double) 1000.0;
                user_buff[xl_cnt].axis_sensor_data_t.accel_data.y =
                    (double) lsm6dsox_acce_fs_to_mg(s_st_xl_fs, data_raw_acceleration.i16bit[1]) / (double) 1000.0;
                user_buff[xl_cnt].axis_sensor_data_t.accel_data.z =
                    (double) lsm6dsox_acce_fs_to_mg(s_st_xl_fs, data_raw_acceleration.i16bit[2]) / (double) 1000.0;
                xl_cnt++;
            } break;

          case LSM6DSOX_GYRO_NC_TAG: {
                memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
                lsm6dsox_fifo_out_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);
                /* if gy data size bigger than buffer, then continue */
                if(gy_cnt >= frame_len){
                    continue;
                }
                user_buff[gy_cnt].axis_sensor_data_t.gyro_data.x = (double) lsm6dsox_gyro_fs_to_mdps(s_st_gy_fs, data_raw_angular_rate.i16bit[0]) / (double) 1000.0;
                user_buff[gy_cnt].axis_sensor_data_t.gyro_data.y = (double) lsm6dsox_gyro_fs_to_mdps(s_st_gy_fs, data_raw_angular_rate.i16bit[1]) / (double) 1000.0;
                user_buff[gy_cnt].axis_sensor_data_t.gyro_data.z = (double) lsm6dsox_gyro_fs_to_mdps(s_st_gy_fs, data_raw_angular_rate.i16bit[2]) / (double) 1000.0;
                gy_cnt++;
            }break;

          default:
            /* Flush unused samples */
            memset(dummy.u8bit, 0x00, 3 * sizeof(int16_t));
            lsm6dsox_fifo_out_raw_get(&dev_ctx, dummy.u8bit);
            break;
        }
        if(xl_cnt >= frame_len && gy_cnt >= frame_len) {
            break;
        }
    }
    pinfo->pre_sys_tick = pinfo->system_tick;

    return ((xl_cnt > gy_cnt)? gy_cnt : xl_cnt);
}





uint32_t lsm6dsox_get_frame_cnt()
{
    uint16_t num;

    lsm6dsox_fifo_data_level_get(&dev_ctx, &num);
    return num>>1;
}


void lsm6dsox_eint_irq_handle(bsp_multi_axis_handle_t handle)
{
    uint32_t frame_cnt = 0, u32temp = 0;
    uint8_t  eint_num;
    lsm6dsox_privte_info_t *pinfo;

    //printf("[bsp][sensor] lsm6dsox_eint_irq_handle");
    pinfo = &g_priv_info;
    hal_gpt_get_free_run_count( HAL_GPT_CLOCK_SOURCE_32K, &pinfo->system_tick);
    /* get sensor timestamp */
    lsm6dsox_timestamp_raw_get(&dev_ctx, &u32temp);
    pinfo->timestamp_us = u32temp * 25;


    //printf("[bsp][sensor] lsm6dsox_eint_irq_handle: timestamp %d", pinfo->timestamp_us);

    eint_num  = BSP_SENSOR_GET_EINT_NUM(handle);
    frame_cnt = lsm6dsox_get_frame_cnt();
    if ( frame_cnt != 0 && g_priv_info.user_callback != NULL) {
        g_priv_info.user_callback(handle, frame_cnt, g_priv_info.user_data);
    } else {
        hal_eint_unmask(eint_num);
    }
    
}



/* Main Example --------------------------------------------------------------*/



int     lsm6dsox_self_test(bsp_multi_axis_enable_t sensor_en)
{
  int16_t data_raw[3];
  float val_st_off[3];
  float val_st_on[3];
  float test_val[3];
  uint8_t st_result;
  uint8_t whoamI;
  uint8_t drdy;
  uint8_t rst;
  uint8_t i;
  uint8_t j;
  uint32_t tick_pre, tick_cur, tick_dur;
  uint32_t handle;

  handle = (uint32_t) (dev_ctx.handle);
  //platform_init(dev_ctx.handle, (platform_bus_init_config_t *)&config->bus_config); /* init platform bus */
  platform_delay(BOOT_TIME); /* Wait sensor boot time */

  if (BSP_SENSOR_GET_BUS_TYPE(handle) == BSP_MULTI_AXIS_BUS_TYPE_SPI) {
      s_ptr_rx_sensor_raw_data = &s_rx_sensor_raw_data[1];
  } else {
      s_ptr_rx_sensor_raw_data =  s_rx_sensor_raw_data;
  }

  /* Check device ID */
  lsm6dsox_device_id_get(&dev_ctx, &whoamI);
  if (whoamI != LSM6DSOX_ID) {
      log_sensor_error("[sensor] lsm6dsox self test fail: ID not match(expect:0x%x,real:0x%x)\r\n", 2, LSM6DSOX_ID, whoamI);
      return BSP_MULTI_AXIS_GET_CHIP_ID_ERROR;
  }

  /* Restore default configuration */
  lsm6dsox_reset_set(&dev_ctx, PROPERTY_ENABLE);
  hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_pre);
  do {
      lsm6dsox_reset_get(&dev_ctx, &rst);
      hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_cur);
      hal_gpt_get_duration_count(tick_pre, tick_cur, &tick_dur);
      if (tick_dur > LSM6DSOX_WAIT_TIMEOUT) {
          log_sensor_error("[sensor] lsm6dsox init fail: wait reset state timeout\r\n", 0);
          return BSP_MULTI_AXIS_TIME_OUT_ERROR;
      }
  } while (rst);

  /* Disable I3C interface */
  lsm6dsox_i3c_disable_set(&dev_ctx, LSM6DSOX_I3C_DISABLE);
  /* Enable Block Data Update */
  lsm6dsox_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /*
   * Accelerometer Self Test
   */
  /* Set Output Data Rate */
  lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_XL_ODR_52Hz);
  /* Set full scale */
  lsm6dsox_xl_full_scale_set(&dev_ctx, LSM6DSOX_4g);
  /* Wait stable output */
  platform_delay(100);

  /* Check if new value available */
  do {
    lsm6dsox_xl_flag_data_ready_get(&dev_ctx, &drdy);
  } while (!drdy);

  /* Read dummy data and discard it */
  lsm6dsox_acceleration_raw_get(&dev_ctx, data_raw);
  /* Read 5 sample and get the average vale for each axis */
  memset(val_st_off, 0x00, 3 * sizeof(float));

  for (i = 0; i < 5; i++) {
    /* Check if new value available */
    do {
      lsm6dsox_xl_flag_data_ready_get(&dev_ctx, &drdy);
    } while (!drdy);

    /* Read data and accumulate the mg value */
    lsm6dsox_acceleration_raw_get(&dev_ctx, data_raw);

    for (j = 0; j < 3; j++) {
      val_st_off[j] += lsm6dsox_from_fs4_to_mg(data_raw[j]);
    }
  }

  /* Calculate the mg average values */
  for (i = 0; i < 3; i++) {
    val_st_off[i] /= 5.0f;
  }

  /* Enable Self Test positive (or negative) */
  lsm6dsox_xl_self_test_set(&dev_ctx, LSM6DSOX_XL_ST_NEGATIVE);
  //lsm6dsox_xl_self_test_set(&dev_ctx, LSM6DSOX_XL_ST_POSITIVE);
  /* Wait stable output */
  platform_delay(100);

  /* Check if new value available */
  do {
    lsm6dsox_xl_flag_data_ready_get(&dev_ctx, &drdy);
  } while (!drdy);

  /* Read dummy data and discard it */
  lsm6dsox_acceleration_raw_get(&dev_ctx, data_raw);
  /* Read 5 sample and get the average vale for each axis */
  memset(val_st_on, 0x00, 3 * sizeof(float));

  for (i = 0; i < 5; i++) {
    /* Check if new value available */
    do {
      lsm6dsox_xl_flag_data_ready_get(&dev_ctx, &drdy);
    } while (!drdy);

    /* Read data and accumulate the mg value */
    lsm6dsox_acceleration_raw_get(&dev_ctx, data_raw);

    for (j = 0; j < 3; j++) {
      val_st_on[j] += lsm6dsox_from_fs4_to_mg(data_raw[j]);
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
  st_result = 1;

  for (i = 0; i < 3; i++) {
    if (( MIN_ST_LIMIT_mg > test_val[i] ) ||
        ( test_val[i] > MAX_ST_LIMIT_mg)) {
      st_result = 0;
    }
  }

  /* Disable Self Test */
  lsm6dsox_xl_self_test_set(&dev_ctx, LSM6DSOX_XL_ST_DISABLE);
  /* Disable sensor. */
  lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_XL_ODR_OFF);
  /*
   * Gyroscope Self Test
   */
  /* Set Output Data Rate */
  lsm6dsox_gy_data_rate_set(&dev_ctx, LSM6DSOX_GY_ODR_208Hz);
  /* Set full scale */
  lsm6dsox_gy_full_scale_set(&dev_ctx, LSM6DSOX_2000dps);
  /* Wait stable output */
  platform_delay(100);

  /* Check if new value available */
  do {
    lsm6dsox_gy_flag_data_ready_get(&dev_ctx, &drdy);
  } while (!drdy);

  /* Read dummy data and discard it */
  lsm6dsox_angular_rate_raw_get(&dev_ctx, data_raw);
  /* Read 5 sample and get the average vale for each axis */
  memset(val_st_off, 0x00, 3 * sizeof(float));

  for (i = 0; i < 5; i++) {
    /* Check if new value available */
    do {
      lsm6dsox_gy_flag_data_ready_get(&dev_ctx, &drdy);
    } while (!drdy);

    /* Read data and accumulate the mg value */
    lsm6dsox_angular_rate_raw_get(&dev_ctx, data_raw);

    for (j = 0; j < 3; j++) {
      val_st_off[j] += lsm6dsox_from_fs2000_to_mdps(data_raw[j]);
    }
  }

  /* Calculate the mg average values */
  for (i = 0; i < 3; i++) {
    val_st_off[i] /= 5.0f;
  }

  /* Enable Self Test positive (or negative) */
  lsm6dsox_gy_self_test_set(&dev_ctx, LSM6DSOX_GY_ST_POSITIVE);
  //lsm6dsox_gy_self_test_set(&dev_ctx, LIS2DH12_GY_ST_NEGATIVE);
  /* Wait stable output */
  platform_delay(100);
  /* Read 5 sample and get the average vale for each axis */
  memset(val_st_on, 0x00, 3 * sizeof(float));

  for (i = 0; i < 5; i++) {
    /* Check if new value available */
    do {
      lsm6dsox_gy_flag_data_ready_get(&dev_ctx, &drdy);
    } while (!drdy);

    /* Read data and accumulate the mg value */
    lsm6dsox_angular_rate_raw_get(&dev_ctx, data_raw);

    for (j = 0; j < 3; j++) {
      val_st_on[j] += lsm6dsox_from_fs2000_to_mdps(data_raw[j]);
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
    if (( MIN_ST_LIMIT_mdps > test_val[i] ) ||
        ( test_val[i] > MAX_ST_LIMIT_mdps)) {
      st_result = 0;
    }
  }

  /* Disable Self Test */
  lsm6dsox_gy_self_test_set(&dev_ctx, LSM6DSOX_GY_ST_DISABLE);
  /* Disable sensor. */
  lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_GY_ODR_OFF);

  if (st_result == 1) {
    log_sensor_info("lsm6dsox self test - PASS\r\n", 0);
    return 0;
  }

  else {
    log_sensor_error("lsm6dsox self test - FAIL\r\n", 0);
    return -1;
  }
  return 0;
}




#if 0
/* Main Example --------------------------------------------------------------*/
void lsm6dsox_fifo(void)
{
    uint32_t handle = 0x47014012;
    platform_bus_init_config_t config;
    axis3bit16_t data_raw_acceleration;
    axis3bit16_t data_raw_angular_rate;
    float acceleration_mg[3];
    float angular_rate_mdps[3];
    uint8_t whoamI, rst;
    uint8_t tx_buffer[1000];
    int i = 0;

  dev_ctx.handle = handle;
  config.i2c_config.frequency = HAL_I2C_FREQUENCY_400K;

  printf("dec_ctx:%x, handle:%x",&dev_ctx, dev_ctx.handle);

  /* Init test platform */
  platform_init((void *) handle, (platform_bus_init_config_t *)&config); /* init platform bus */
  /* Wait sensor boot time */
  platform_delay(BOOT_TIME);
  /* Check device ID */
  lsm6dsox_device_id_get(&dev_ctx, &whoamI);

  if (whoamI != LSM6DSOX_ID)
    while (1);

  /* Restore default configuration */
  lsm6dsox_reset_set(&dev_ctx, PROPERTY_ENABLE);

  do {
    lsm6dsox_reset_get(&dev_ctx, &rst);
  } while (rst);

  /* Disable I3C interface */
  lsm6dsox_i3c_disable_set(&dev_ctx, LSM6DSOX_I3C_DISABLE);
  /* Enable Block Data Update */
  lsm6dsox_block_data_update_set(&dev_ctx, PROPERTY_ENABLE);
  /* Set full scale */
  lsm6dsox_xl_full_scale_set(&dev_ctx, LSM6DSOX_2g);
  lsm6dsox_gy_full_scale_set(&dev_ctx, LSM6DSOX_2000dps);
  /* Set FIFO watermark (number of unread sensor data TAG + 6 bytes
   * stored in FIFO) to 10 samples
   */
  lsm6dsox_fifo_watermark_set(&dev_ctx, 10);
  /* Set FIFO batch XL/Gyro ODR to 12.5Hz */
  lsm6dsox_fifo_xl_batch_set(&dev_ctx, LSM6DSOX_XL_BATCHED_AT_12Hz5);
  lsm6dsox_fifo_gy_batch_set(&dev_ctx, LSM6DSOX_GY_BATCHED_AT_12Hz5);
  /* Set FIFO mode to Stream mode (aka Continuous Mode) */
  lsm6dsox_fifo_mode_set(&dev_ctx, LSM6DSOX_STREAM_MODE);
  /* Enable drdy 75 ¦Ìs pulse: uncomment if interrupt must be pulsed */
  //lsm6dsox_data_ready_mode_set(&dev_ctx, LSM6DSOX_DRDY_PULSED);
  /* Uncomment if interrupt generation on Free Fall INT1 pin */
  //lsm6dsox_pin_int1_route_get(&dev_ctx, &int1_route);
  //int1_route.reg.int1_ctrl.int1_fifo_th = PROPERTY_ENABLE;
  //lsm6dsox_pin_int1_route_set(&dev_ctx, &int1_route);
  /* Uncomment if interrupt generation on Free Fall INT2 pin */
  //lsm6dsox_pin_int2_route_get(&dev_ctx, &int2_route);
  //int2_route.reg.int2_ctrl.int2_fifo_th = PROPERTY_ENABLE;
  //lsm6dsox_pin_int2_route_set(&dev_ctx, &int2_route);
  /* Set Output Data Rate */
  lsm6dsox_xl_data_rate_set(&dev_ctx, LSM6DSOX_XL_ODR_12Hz5);
  lsm6dsox_gy_data_rate_set(&dev_ctx, LSM6DSOX_GY_ODR_12Hz5);


  platform_transfer_config_t  transfer;
  uint8_t  rxbuff[8];

  transfer.buff = rxbuff;
  transfer.size = 1;
  transfer.dev_addr = 0x6A;

  for(i = 0;i < 0x63; i++) {
      transfer.reg_addr = i;
      platform_bus_read(handle, &transfer);
      printf("Reg addr %-02x,  value is %-02x",i, rxbuff[0]);
  }

  /* Wait samples. */
  while (1) {
    uint16_t num = 0;
    uint8_t wmflag = 0;
    lsm6dsox_fifo_tag_t reg_tag;
    axis3bit16_t dummy;
    /* Read watermark flag */
    lsm6dsox_fifo_wtm_flag_get(&dev_ctx, &wmflag);

    if (wmflag > 0) {
      /* Read number of samples in FIFO */
      lsm6dsox_fifo_data_level_get(&dev_ctx, &num);

      while (num--) {
        /* Read FIFO tag */
        lsm6dsox_fifo_sensor_tag_get(&dev_ctx, &reg_tag);

        switch (reg_tag) {
          case LSM6DSOX_XL_NC_TAG:
            memset(data_raw_acceleration.u8bit, 0x00, 3 * sizeof(int16_t));
            lsm6dsox_fifo_out_raw_get(&dev_ctx, data_raw_acceleration.u8bit);
            acceleration_mg[0] =
              lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[0]);
            acceleration_mg[1] =
              lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[1]);
            acceleration_mg[2] =
              lsm6dsox_from_fs2_to_mg(data_raw_acceleration.i16bit[2]);
            printf("Acceleration [mg]:%4.2f\t%4.2f\t%4.2f\r\n",
                    acceleration_mg[0], acceleration_mg[1], acceleration_mg[2]);
            break;

          case LSM6DSOX_GYRO_NC_TAG:
            memset(data_raw_angular_rate.u8bit, 0x00, 3 * sizeof(int16_t));
            lsm6dsox_fifo_out_raw_get(&dev_ctx, data_raw_angular_rate.u8bit);
            angular_rate_mdps[0] =
              lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[0]);
            angular_rate_mdps[1] =
              lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[1]);
            angular_rate_mdps[2] =
              lsm6dsox_from_fs2000_to_mdps(data_raw_angular_rate.i16bit[2]);
            printf("Angular rate [mdps]:%4.2f\t%4.2f\t%4.2f\r\n",
                    angular_rate_mdps[0], angular_rate_mdps[1], angular_rate_mdps[2]);
            break;

          default:
            /* Flush unused samples */
            memset(dummy.u8bit, 0x00, 3 * sizeof(int16_t));
            lsm6dsox_fifo_out_raw_get(&dev_ctx, dummy.u8bit);
            break;
        }
      }
    }
  }
}
#endif




