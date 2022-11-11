#ifndef __LSM6DSO_H__
#define __LSM6DSO_H__

#include "hal.h"
#include "hal_platform.h"
#include "lsm6ds3_reg.h"
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_common.h"
#include "lsm6dso_reg.h"


int      lsm6dso_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config);
int      lsm6dso_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config);
int      lsm6dso_deinit(bsp_multi_axis_handle_t handle);
int      lsm6dso_read_format_data(bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len);
uint32_t lsm6dso_get_frame_cnt();
int      lsm6dso_self_test(bsp_multi_axis_enable_t sensor_en);
void     lsm6dso_eint_irq_handle(bsp_multi_axis_handle_t handle);
#endif
