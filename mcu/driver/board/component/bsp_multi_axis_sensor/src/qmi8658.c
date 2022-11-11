#include "qmi8658.h"
#include "qmi8658_reg.h"
#include "hal.h"
#include "hal_platform.h"
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"
#include "bsp_multi_axis_sensor_common.h"
#include "airo_platform_bus.h"
#include <string.h>
#include <stdlib.h>

static qmi8658_privte_info_t qmi8658_info;
static uint8_t  sensor_temp_buffer[4096] = {0};

#define     FREE_COUNT_32K_TO_MS(count)  ((((count) * 1000.0f) / 32768))

int    qmi8658_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config)
{
    qmi8658_privte_info_t *pinfo = &qmi8658_info;

    memset(&qmi8658_info, 0, sizeof(qmi8658_info));
    pinfo->user_callback = config->call_back;
    pinfo->user_data     = config->user_data;
    return qmi8658_sensor_init( handle, config);
}

int    qmi8658_deinit(bsp_multi_axis_handle_t handle)
{
    return qmi8658_sensor_deinit(handle);
}

int    qmi8658_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config)
{
    int ret = 0;
    qmi8658_privte_info_t *pinfo = &qmi8658_info;

    pinfo->packet_len    = config->frame_threshold;
    ret = qmi8658_sensor_config(handle, config);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &pinfo->pre_sys_tick);
    return ret;
}

uint32_t qmi8658_get_frame_cnt()
{
    return qmi8658_sensor_available_byte() / 12;
}



int     qmi8658_read_format_data(bsp_multi_axis_data_t *user_buff, uint32_t user_buff_len)
{
    uint32_t            i = 0, avail_frame;
    uint32_t            rd_byte;
    qmi8658_privte_info_t *pinfo = &qmi8658_info;
    uint32_t            delt_tm = 0;
    uint32_t            temp = 0;
    
    if (user_buff == NULL) {
        return 0;
    }
    avail_frame = qmi8658_get_frame_cnt();
    hal_gpt_get_duration_count(pinfo->pre_sys_tick, pinfo->system_tick, &temp);
    delt_tm     = temp / avail_frame;
    temp        = user_buff_len > avail_frame? avail_frame : user_buff_len;
    rd_byte     = temp * 12;
    
    qmi8658_sensor_read_fifo_data(sensor_temp_buffer, rd_byte);
    qmi8658_sensor_format(sensor_temp_buffer, rd_byte, user_buff, user_buff_len);

    for (i = 0; i < avail_frame; i++) {
        temp = pinfo->pre_sys_tick + delt_tm * (i + 1);
        user_buff[i].head.system_time_ms = FREE_COUNT_32K_TO_MS(temp);
    }
    pinfo->pre_sys_tick = temp;    
    return avail_frame;
}

void    qmi8658_eint_irq_handle(bsp_multi_axis_handle_t handle)
{
    qmi8658_privte_info_t *pinfo = &qmi8658_info;
    uint32_t frame_len = qmi8658_get_frame_cnt();
    
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &pinfo->system_tick);

    if (frame_len == 0) {
        uint8_t eint_num;
        eint_num = BSP_SENSOR_GET_EINT_NUM(handle);
        hal_eint_unmask(eint_num);
    } else if (pinfo->user_callback != NULL) {
        pinfo->user_callback(handle, frame_len, pinfo->user_data);
    }
    return;
}



