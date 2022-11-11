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
#include "hal_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "icm42607.h"
#include "airo_platform_bus.h"
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"
#include "bsp_multi_axis_sensor_common.h"

//#define     ICM42607_EXTEND
#define     ICM42607_DEVICE_ID  0x68
#define     ICM42607_SLV_ADDR   0x68
#define     BOOT_TIME           100


static icm42607_privte_info_t  g_priv_info;

#ifdef ICM42607_EXTEND
static void    platform_bus_read_completed_callback(void *user_data)
{
}
#endif
static void platform_delay(uint32_t ms)
{
    hal_gpt_delay_ms(ms);
}

static int  platform_init(uint32_t handle, platform_bus_init_config_t *config)
{
    return platform_bus_init((uint32_t)handle, config);
}

static int  platform_deinit(uint32_t handle)
{
    return platform_bus_deinit((uint32_t)handle);
}

static int  platform_write(uint32_t handle, uint8_t reg, uint8_t *data, uint32_t size)
{
    platform_transfer_config_t config;

    config.dev_addr = ICM42607_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    return platform_bus_write((uint32_t)handle, &config);
}

static int  platform_read(uint32_t handle, uint8_t reg, uint8_t *data, uint32_t size)
{
    platform_transfer_config_t config;

    config.dev_addr = ICM42607_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    return platform_bus_read((uint32_t)handle, &config);
}

#ifdef ICM42607_EXTEND
static int  platform_read_with_dma(uint32_t handle, uint8_t reg, uint8_t *data, uint32_t size)
{
    platform_transfer_dma_config_t    config;

    config.dev_addr = ICM42607_SLV_ADDR;
    config.reg_addr = reg;
    config.buff     = data;
    config.size     = size;
    config.call_back = platform_bus_read_completed_callback;
    config.user_data = (void *)handle;

    return platform_bus_read_dma((uint32_t)handle, &config);
}
#endif

static  int icm42607_write_register(uint32_t handle, uint8_t reg, uint8_t bit_ofs, uint8_t mask, uint8_t val)
{
    uint8_t temp = 0;
    int     ret = 0;


    ret = platform_read( handle, reg, &temp, 1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_write_register() fail: platform_read error(%d)\r\n", 1, ret);
        return -1;
    }
    temp &= ~(mask << bit_ofs);
    temp |= ((val & mask) << bit_ofs);

    ret = platform_write( handle, reg, &temp, 1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_write_register() fail: platform_read error(%d)\r\n", 1, ret);
    }
    return ret;
}

static int icm42607_write_bank_reg(uint32_t handle, uint8_t bank, uint8_t reg, uint8_t val)
{
    uint8_t bank_sel = 0, reg_sel = 0, value = 0;
    int     ret = 0;

    if(bank == 0 || bank > 3) {
        return -1;
    }
    reg_sel = reg;
    value   = val;
    switch(bank){
        case 1: bank_sel = 0x00; break;
        case 2: bank_sel = 0x28; break;
        case 3: bank_sel = 0x50; break;
    }
    ret = platform_write(handle, ICM42607_BLK_SEL_W, &bank_sel, 1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_write_bank_reg() fail: write %x error(%d)\r\n", 2, ICM42607_BLK_SEL_W, ret);
        return -1;
    }
    ret = platform_write(handle, ICM42607_MADDR_W,   &reg_sel,  1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_write_bank_reg() fail: write %x error(%d)\r\n", 2, ICM42607_MADDR_W, ret);
        return -1;
    }
    ret = platform_write(handle, ICM42607_M_W,       &value,    1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_write_bank_reg() fail: write %x error(%d)\r\n", 2, ICM42607_M_W, ret);
        return -1;
    }

    platform_delay(1);
    value = 0;
    ret = platform_write(handle, ICM42607_BLK_SEL_W, &value,    1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_write_bank_reg() fail: write %x error(%d)\r\n", 2, ICM42607_BLK_SEL_W, ret);
        return -1;
    }
    return 0;
}


#ifdef ICM42607_EXTEND
static int icm42607_read_bank_reg(uint32_t handle, uint8_t bank, uint8_t reg, uint8_t *value)
{
    uint8_t bank_sel = 0, reg_sel = 0, uc_temp;
    int     ret = 0;

    if(bank == 0 || bank > 3) {
        return -1;
    }
    reg_sel = reg;
    switch(bank){
        case 1: bank_sel = 0x00; break;
        case 2: bank_sel = 0x28; break;
        case 3: bank_sel = 0x50; break;
    }
    ret = platform_write(handle, ICM42607_BLK_SEL_R, &bank_sel, 1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_read_bank_reg() fail: read %x error(%d)\r\n", 2, ICM42607_BLK_SEL_R, ret);
        return -1;
    }
    ret = platform_write(handle, ICM42607_MADDR_R,   &reg_sel,  1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_read_bank_reg() fail: read %x error(%d)\r\n", 2, ICM42607_MADDR_R, ret);
        return -1;
    }

    platform_delay(1);
    ret = platform_read(handle, ICM42607_M_R, value, 1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_read_bank_reg() fail: read %x error(%d)\r\n", 2, ICM42607_M_R, ret);
        return -1;
    }
    platform_delay(1);
    uc_temp = 0;
    ret = platform_write(handle, ICM42607_BLK_SEL_R, &uc_temp,    1);
    if(ret != 0){
        log_sensor_error("[icm42607] icm42607_write_bank_reg() fail: write %x error(%d)\r\n", 2, ICM42607_BLK_SEL_R, ret);
        return -1;
    }
    return 0;
}
#endif


static void icm42607_config_accel(bsp_multi_axis_handle_t handle, bsp_multi_axis_accel_range_t range, bsp_multi_axis_output_data_rate_t odr)
{
    uint8_t temp = 0;

    switch(range){
        case BSP_MULTI_AXIS_ACCEL_RANGE_2G:     range = ICM42607_XL_RANGE_2G;  temp = 2; break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_4G:     range = ICM42607_XL_RANGE_4G;  temp = 4; break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_8G:     range = ICM42607_XL_RANGE_8G;  temp = 8; break;
        case BSP_MULTI_AXIS_ACCEL_RANGE_16G:    range = ICM42607_XL_RANGE_16G; temp = 16; break;
    }

    switch(odr){
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_25HZ:   odr = ICM42607_XL_ODR_25HZ;  break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_50HZ:   odr = ICM42607_XL_ODR_50HZ;  break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_100HZ:  odr = ICM42607_XL_ODR_100HZ; break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_200HZ:  odr = ICM42607_XL_ODR_200HZ; break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_400HZ:  odr = ICM42607_XL_ODR_400HZ; break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_800HZ:  odr = ICM42607_XL_ODR_800HZ; break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_1600HZ: odr = ICM42607_XL_ODR_1K6HZ; break;
        default:                                     odr = ICM42607_XL_ODR_25HZ; break;
    }
    g_priv_info.xl_range = temp;
    temp = (range << 5) | (odr << 0);
    platform_write(handle, ICM42607_ACCL_CFG0, &temp , 1);
}


static void icm42607_config_gyro(bsp_multi_axis_handle_t handle, bsp_multi_axis_gyro_range_t range, bsp_multi_axis_output_data_rate_t odr)
{
    uint32_t temp = 0;
    uint8_t  uc_temp = 0;

    switch(range){
        case BSP_MULTI_AXIS_GYRO_RANGE_250_DEG_SEC:   range = ICM42607_GY_RANGE_250;  temp = 250; break;
        case BSP_MULTI_AXIS_GYRO_RANGE_500_DEG_SEC:   range = ICM42607_GY_RANGE_500;  temp = 500; break;
        case BSP_MULTI_AXIS_GYRO_RANGE_1000_DEG_SEC:  range = ICM42607_GY_RANGE_1000; temp = 1000;break;
        case BSP_MULTI_AXIS_GYRO_RANGE_2000_DEG_SEC:  range = ICM42607_GY_RANGE_2000; temp = 2000;break;
        default:                                      range = ICM42607_GY_RANGE_250;  temp = 250; break;
    }
    if(range == BSP_MULTI_AXIS_GYRO_RANGE_125_DEG_SEC) {
        range = BSP_MULTI_AXIS_GYRO_RANGE_250_DEG_SEC;
    }
    switch(odr){
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_25HZ:   odr = ICM42607_GY_ODR_25HZ;  break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_50HZ:   odr = ICM42607_GY_ODR_50HZ;  break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_100HZ:  odr = ICM42607_GY_ODR_100HZ; break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_200HZ:  odr = ICM42607_GY_ODR_200HZ; break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_400HZ:  odr = ICM42607_GY_ODR_400HZ; break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_800HZ:  odr = ICM42607_GY_ODR_800HZ; break;
        case BSP_MULTI_AXIS_OUTPUT_DATA_RATE_1600HZ: odr = ICM42607_GY_ODR_1K6HZ; break;
        default:                                     odr = ICM42607_GY_ODR_25HZ;  break;
    }
    g_priv_info.gy_range = temp;
    uc_temp = (range << 5) | (odr << 0);
    platform_write(handle, ICM42607_GYRO_CFG0, &uc_temp , 1);
}


static  bool    icm42607_is_data_ready(bsp_multi_axis_handle_t handle)
{
    uint8_t st_int_drdy = 0;
    uint8_t st_int = 0;

    platform_read(handle, ICM42607_INT_STATUS_DRDY, &st_int_drdy, 1);
    platform_read(handle, ICM42607_INT_STATUS, &st_int, 1);
    if( (st_int_drdy& 0x1) || (st_int & 0x4)){
        return true;
    } else {
        return false;
    }
}



#ifdef ICM42607_EXTEND
void    icm4607_dump(bsp_multi_axis_handle_t handle)
{
    int i = 0;
    uint8_t data;

    printf("------------- BANK 0 -------------");
    for(i = 0; i < 0x3F; i++){
        platform_read(handle, i, &data, 1);
        printf("read sensor addr %02x -> Value %02x", i, data);
    }
    printf("------------- BANK 1 -------------");
    for( i = 0; i < 0x34; i++) {
        icm42607_read_bank_reg(handle,1, i, &data);
        printf("read sensor addr %02x -> Value %02x", i, data);
    }
    printf("------------- BANK 2 -------------");
    i = 0x06;
    icm42607_read_bank_reg(handle,2, i, &data);
    printf("read sensor addr %02x -> Value %02x", i, data);
}

#endif


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int icm42607_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config)
{
    uint8_t who_am_i = 0;
    uint8_t u8temp = 0;
    uint32_t  pre_tick, cur_tick, dur_tick;

    g_priv_info.user_callback = config->call_back;
    g_priv_info.user_data     = config->user_data;
    g_priv_info.accr_odr = 0;
    platform_init((uint32_t)handle, (platform_bus_init_config_t *)&config->bus_config); /* init platform bus */
    platform_delay(BOOT_TIME); /* Wait sensor boot time */

    /*Get devive ID*/
    platform_read((uint32_t)handle, ICM42607_WHO_AM_I, &who_am_i, 1);
    if (who_am_i != ICM42607_DEVICE_ID) {
        log_sensor_error("[sensor] icm42607_init fail: ID not match(expect:0x%x,real:0x%x)\r\n", 2, ICM42607_DEVICE_ID, who_am_i);
        return BSP_MULTI_AXIS_GET_CHIP_ID_ERROR;
    }
    log_sensor_info("[sensor] icm42607_init: Detect Succ: ID %x\r\n", 1, who_am_i);

    /* reset sensor & waiting bootup */
    u8temp = 1<<4;
    platform_write( handle, ICM42607_SIG_PATH_RST, &u8temp, 1);
    hal_gpt_get_free_run_count( HAL_GPT_CLOCK_SOURCE_1M, &pre_tick);
    while(1) {
        hal_gpt_get_free_run_count( HAL_GPT_CLOCK_SOURCE_1M, &cur_tick);
        platform_read( handle, ICM42607_INT_STATUS, &u8temp, 1);
        if(u8temp & 0x10){
            break;
        }
        hal_gpt_get_duration_count(pre_tick,cur_tick, &dur_tick);
        if(dur_tick > 10000){
            log_sensor_error("[sensor] icm42607_init: reset fail\r\n", 0);
            return -1;
        }
    }
    platform_delay(BOOT_TIME);

    /* enable mclock */
    icm42607_write_register(handle, ICM42607_PWR_MGMT0, 4, 0x1, 0x1);

    /* otp reload*/
    icm42607_write_bank_reg(handle, 1, 0x2B, 0x04); /* enable copy otp */
    platform_delay(1);
    icm42607_write_bank_reg(handle, 2, 0x06, 0x00); /* power up OTP */
    platform_delay(1);
    icm42607_write_bank_reg(handle, 2, 0x06, 0x08); /* trigger OTP copy */
    platform_delay(5);

    /* enable mclock again*/
    icm42607_write_register(handle, ICM42607_PWR_MGMT0, 4, 0x1, 0x1);
    platform_delay(15);

    /* config INT1 */
    u8temp = (0 << 2) | (1 << 1) | (0x1);   /*bit2(0:pulse, 1:latch)  bit1(0:opendrain, 1:pushpull) bit0(0:active low, 1:active high) */
    icm42607_write_register(handle, ICM42607_INT_CFG, 0, 0x7, u8temp);
    u8temp = (1 << 2);   /* fifo threshold interrupt map to INT1 */
    icm42607_write_register(handle, ICM42607_INT_SOURCE0, 0, 0xff, u8temp);
    return 0;
}


int icm42607_deinit(bsp_multi_axis_handle_t handle)
{
    /* off sensor gyro & accel */
    icm42607_write_register(handle, ICM42607_PWR_MGMT0, 0, 0xF, 0x0);
    platform_delay(15);
    /* off sensor gyro & accel */
    icm42607_write_register(handle, ICM42607_PWR_MGMT0, 4, 0x1, 0x0);
    platform_deinit(handle);
    g_priv_info.accr_odr = 0;
    return 0;
}



int icm42607_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config)
{
    uint8_t pwr_mgm = 0;
    uint32_t u32temp = 0;
    uint8_t  u8temp = 0;
    uint8_t  item_len = 0;


    if(config->enable & BSP_MULTI_AXIS_ACCEL_ENABLE)
    {
        icm42607_config_accel(handle, config->accel_range, config->sensor_odr);
        pwr_mgm |= 0x3;
        item_len += 6;
    }
    if(config->enable & BSP_MULTI_AXIS_GYRO_ENABLE)
    {
        icm42607_config_gyro(handle, config->gyro_range, config->sensor_odr);
        pwr_mgm |= (0x3 << 2);
        item_len += 6;
    }
    /* config fifo & watermark */
    item_len += 4; /* Head(1B) + Temperature(1B) + Timestamp(2B)*/
    u32temp = config->frame_threshold * 16;
    icm42607_write_register(handle, ICM42607_FIFO_CFG2, 0, 0xff, (u32temp & 0xFF));
    icm42607_write_register(handle, ICM42607_FIFO_CFG3, 0, 0xff, ((u32temp>>8) & 0xF));

    /* enable gyro & accel & timestamp to fifo*/
    u8temp = (1 << 2) | (1 << 1) | 0x1;
    icm42607_write_bank_reg(handle, 1, 0x01, u8temp);

    /* reset & flush fifo*/
    icm42607_write_register(handle, ICM42607_FIFO_CFG1,    0, 0xff, 0x01); /* by pass*/
    icm42607_write_register(handle, ICM42607_FIFO_CFG1,    0, 0xff, 0x00); /* not by pass*/
    icm42607_write_register(handle, ICM42607_SIG_PATH_RST, 2, 0x01, 0x01);
    platform_delay(1);

    u8temp = (1 << 6); /*bit6(0:count by byte, 1: by record), bit5(0:count is little, 1: count is big), bit4(0:data is little, 1:data is big) */
    icm42607_write_register(handle, ICM42607_INTF_CFG0, 0, 0xff, u8temp); /* fifo data odr */
    icm42607_write_register(handle, ICM42607_INTF_CFG0, 0, 0xff, 1);

    /* config temperature */
    icm42607_write_register(handle, ICM42607_TEMP_CFG0, 4, 0x7, 1);

    /* config timestamp */
    u8temp =  (1 << 3) | (0x1); /*15us/LSB, */
    icm42607_write_bank_reg(handle, 1, 0x00, u8temp);

    /* enable gyro& accel & timestamp to fifo */
    u8temp = (1 << 5) | (1 << 1) | 0x1;
    icm42607_write_bank_reg(handle, 1, 0x01, u8temp);

    /* enable gyro & accel power */
    icm42607_write_register(handle, ICM42607_PWR_MGMT0, 0, 0xff, pwr_mgm);
#ifdef ICM42607_EXTEND
    icm4607_dump(handle);
#endif
    platform_delay(100);
    icm42607_write_register(handle, ICM42607_FIFO_CFG1,    0, 0xff, 0x01); /* by pass*/
    icm42607_write_register(handle, ICM42607_FIFO_CFG1,    0, 0xff, 0x00); /* not by pass*/
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(g_priv_info.pre_sys_tick));
    g_priv_info.accr_odr = 0;
    return 0;
}


int icm42607_get_frame_cnt(bsp_multi_axis_handle_t handle)
{
    uint8_t  data[2];
    uint16_t frame_cnt = 0;
    int res = 0;

    res = platform_read(handle, ICM42607_FIFO_COUNTH, data, 2);
    if(res < 0){
        log_sensor_error("[sensor] icm42607_get_frame_cnt: read err\r\n",0);
        return 0;
    }
    frame_cnt = (data[1] << 8) | data[0];
    return (frame_cnt/16);
}


static icm42607_fifo_packet_t  s_fifo_data;

//static uint32_t  sensor_tm_pre = 0, sensor_sample_cnt = 0;

int icm42607_read_format_data(bsp_multi_axis_handle_t handle, bsp_multi_axis_data_t *user_buff, uint32_t frame_len)
{
    uint16_t avail_frame_cnt = 0, i =0;
    int res = 0;
    icm42607_fifo_packet_t  *pdata;
    uint32_t     uw_pkt_len = 0;
    int16_t      xl_fs_sel, gy_fs_sel;
    int16_t      x,y,z;
    uint32_t     tick_dur = 0;
    uint32_t     delt = 0;

    pdata  = &s_fifo_data;
    xl_fs_sel = g_priv_info.xl_range;
    gy_fs_sel = g_priv_info.gy_range;

    /* get fifo length(tag + 6byte)*/
    avail_frame_cnt = icm42607_get_frame_cnt(handle);
    frame_len  = (frame_len > avail_frame_cnt)?avail_frame_cnt : frame_len;
    uw_pkt_len = (uint32_t) sizeof(icm42607_fifo_packet_t);
    hal_gpt_get_duration_count(g_priv_info.pre_sys_tick, g_priv_info.sys_tick, &tick_dur);
    if(avail_frame_cnt != 0) {
        delt = tick_dur / avail_frame_cnt;
    }
    for( i = 0; i < frame_len; i++){
        res = platform_read(handle, ICM42607_FIFO_DATA, (uint8_t *)&s_fifo_data, uw_pkt_len);
        if(res < 0){
            log_sensor_error("[sensor] icm42607_get_frame_cnt: read err\r\n",0);
            return 0;
        }
        if(pdata->header & ICM42607_FIFO_HEADER_FIFO_EMPTY_FLG_MASK) {
            break;
        }
        if(pdata->header & ICM42607_FIFO_HEADER_TIME_AVAILABLE_MASK) {
            user_buff[i].head.sensor_time = (((pdata->time_stamp[1] << 8) | pdata->time_stamp[0]) * 16) / 1000;
        } else {
            user_buff[i].head.sensor_time = 0;
        }

        /* for icm42607 timestamp */
        user_buff[i].head.system_time_ms = (float)(((g_priv_info.pre_sys_tick + (i + 1) * delt) / 32768.0f) * 1000);
        log_axis_sensor_info("[bsp][sensor] system time stamp ->%d", 1, (uint32_t) user_buff[i].head.system_time_ms);

        user_buff[i].head.temperature    = pdata->temper / 2 + 25;
        x = (pdata->accl_x[1] << 8) | pdata->accl_x[0];
        y = (pdata->accl_y[1] << 8) | pdata->accl_y[0];
        z = (pdata->accl_z[1] << 8) | pdata->accl_z[0];

        //printf("--->accl(xl_fs_sel %d)  x:%d, y:%d, z:%d", xl_fs_sel,x, y, z);
        user_buff[i].axis_sensor_data_t.accel_data.x =  (double)( x * xl_fs_sel) / 32768.0;
        user_buff[i].axis_sensor_data_t.accel_data.y =  (double)( y * xl_fs_sel) / 32768.0;
        user_buff[i].axis_sensor_data_t.accel_data.z =  (double)( z * xl_fs_sel) / 32768.0;

        x = (pdata->gyro_x[1] << 8) | pdata->gyro_x[0];
        y = (pdata->gyro_y[1] << 8) | pdata->gyro_y[0];
        z = (pdata->gyro_z[1] << 8) | pdata->gyro_z[0];

        //printf("--->gyro(xl_fs_sel %d)  x:%d, y:%d, z:%d", gy_fs_sel,x, y, z);
        user_buff[i].axis_sensor_data_t.gyro_data.x  =  (double)(x * gy_fs_sel) / 32768.0;
        user_buff[i].axis_sensor_data_t.gyro_data.y  =  (double)(y * gy_fs_sel) / 32768.0;
        user_buff[i].axis_sensor_data_t.gyro_data.z  =  (double)(z * gy_fs_sel) / 32768.0;
    }
    g_priv_info.pre_sys_tick = g_priv_info.sys_tick;
    return i;
}



void icm42607_eint_irq_handle(bsp_multi_axis_handle_t handle)
{
    uint32_t frame_cnt;
    uint8_t  eint_num;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(g_priv_info.sys_tick));
    frame_cnt = icm42607_get_frame_cnt(handle);
    if (frame_cnt != 0) {
        if(g_priv_info.user_callback != NULL && icm42607_is_data_ready(handle)) {
            g_priv_info.user_callback(handle, frame_cnt, g_priv_info.user_data);
        } else {
            //log_sensor_error("[sensor] icm42607_eint_irq_handle: not ready\r\n",0);
        }
    } else {
        //log_sensor_error("[sensor] icm42607_eint_irq_handle: frame cnt = 0\r\n",0);
        eint_num  = BSP_SENSOR_GET_EINT_NUM(handle);
        hal_eint_unmask(eint_num);
    }
}




