#ifndef __LSM6DSO_H__
#define __LSM6DSO_H__

#include "hal.h"
#include "hal_platform.h"
#include "lsm6dsox_reg.h"
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_common.h"

typedef union {
    int16_t i16bit[3];
    uint8_t u8bit[6];
} axis3bit16_t;



typedef struct {
    float       temperature;
    uint32_t    pre_sys_tick;
    uint32_t    system_tick;
    uint32_t    timestamp_us;
    uint32_t    packet_len;
    bsp_multi_axis_callback_t user_callback;
    void        *user_data;
    sw_fifo_t    fmt_data_fifo;
    lsm6dsox_odr_xl_t    accr_odr;
    lsm6dsox_odr_g_t     gyro_odr;
    lsm6dsox_bdr_xl_t    fifo_odr;
} lsm6dsox_privte_info_t;


int      lsm6dsox_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config);
int      lsm6dsox_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config);
int      lsm6dsox_deinit(bsp_multi_axis_handle_t handle);
int      lsm6dsox_read_format_data(bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len);
uint32_t lsm6dsox_get_frame_cnt();
int      lsm6dsox_self_test(bsp_multi_axis_enable_t sensor_en);
void     lsm6dsox_eint_irq_handle(bsp_multi_axis_handle_t handle);
#endif
