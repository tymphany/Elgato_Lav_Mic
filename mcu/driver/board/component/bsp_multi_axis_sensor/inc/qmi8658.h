#ifndef _QMI858_H_
#define _QMI858_H_
#include "hal.h"
#include "hal_platform.h"
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_common.h"


typedef struct {
    
    float       temperature;
    uint32_t    pre_sys_tick;
    uint32_t    system_tick;
    uint32_t    timestamp_us;
    uint32_t    packet_len;
    bsp_multi_axis_callback_t user_callback;
    void        *user_data;
} qmi8658_privte_info_t;

int      qmi8658_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config);
int      qmi8658_deinit(bsp_multi_axis_handle_t handle);
int      qmi8658_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config);
uint32_t qmi8658_get_frame_cnt();
int      qmi8658_read_format_data(bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len);
void     qmi8658_eint_irq_handle(bsp_multi_axis_handle_t handle);







#endif

