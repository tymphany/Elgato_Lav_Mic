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
#ifdef AIR_LE_AUDIO_ENABLE

#include "app_le_audio_usb.h"
#include "app_le_audio_utillity.h"
#include "app_preproc_activity.h"
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
#include "app_le_audio_ucst.h"
#include "app_le_audio_aird.h"
#include "app_le_audio_ucst_utillity.h"
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
#include "app_le_audio_bcst.h"
#endif
#include "app_le_audio.h"

#include "bt_le_audio_source.h"

#include "ui_shell_manager.h"
#include "apps_events_usb_event.h"
#include "FreeRTOS.h"
#include "timers.h"

#include "bt_le_audio_msglog.h"
#include "usbaudio_drv.h"

/**************************************************************************************************
* Define
**************************************************************************************************/
#define APP_LE_AUDIO_USB_LOG_SLIM

#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
#define APP_LE_AUDIO_DONGLE_SIMULATE_MEDIA_STATE 1
#else
#define APP_LE_AUDIO_DONGLE_SIMULATE_MEDIA_STATE 0
#endif

/* USB releated */
#define APP_LE_AUDIO_USB_PORT_NUM_0 0
#define APP_LE_AUDIO_USB_PORT_NUM_1 1

#define APP_LE_AUDIO_VOL_VALUE_MIN      0
#define APP_LE_AUDIO_VOL_VALUE_MAX      100
#define APP_LE_AUDIO_VOL_VALUE_DEFAULT  80

#define APP_LE_AUDIO_USB_TIMEOUT    (3000)  /* unit: ms */

typedef enum {
    APP_LE_AUDIO_USB_EVENT_ENABLE,
    APP_LE_AUDIO_USB_EVENT_DISABLE,
    APP_LE_AUDIO_USB_EVENT_SAMPLE_SIZE_OBTAIN, /* Obtain a valid value when the corresponding steram port is not initialized. */
    APP_LE_AUDIO_USB_EVENT_SAMPLE_SIZE_CHANGE, /* Changed for an enabled USB port */
    APP_LE_AUDIO_USB_EVENT_SAMPLE_RATE_OBTAIN,
    APP_LE_AUDIO_USB_EVENT_SAMPLE_RATE_CHANGE,
    APP_LE_AUDIO_USB_EVENT_CHANNEL_OBTAIN,
    APP_LE_AUDIO_USB_EVENT_CHANNEL_CHANGE,

    APP_LE_AUDIO_USB_EVENT_MAX = 0xFF
} app_le_audio_usb_event_enum;

/**************************************************************************************************
* Structure
**************************************************************************************************/
typedef struct {
    uint8_t is_streaming;                   /**< USB audio streaming state */
    uint8_t mute;                           /**< USB audio mute state: MUTE or UNMUTE */
    uint8_t vol_left;                       /**< USB audio volume L, value: 0 ~ 100 */
    uint8_t vol_right;                      /**< USB audio volume R, value: 0 ~ 100 */
} app_le_audio_usb_info_t;


/**************************************************************************************************
* Variable
**************************************************************************************************/
const uint32_t sample_rate_table[] = {16000,16000,32000,44100,48000,96000};
const uint32_t sample_size_table[] =
{
    AFE_PCM_FORMAT_S16_LE, //default
    AFE_PCM_FORMAT_S8,
    AFE_PCM_FORMAT_S16_LE,
    AFE_PCM_FORMAT_S24_LE,
    AFE_PCM_FORMAT_S32_LE
};

app_le_audio_usb_info_t g_lea_usb_info[APP_LE_AUDIO_USB_PORT_MAX];
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
static TimerHandle_t g_lea_usb_timer = NULL;
#endif
/**************************************************************************************************
* Prototype
**************************************************************************************************/

/**************************************************************************************************
* Functions
**************************************************************************************************/
static uint8_t app_le_audio_map_vol_level(uint8_t volume)
{
    float local_level_f = 0;
    float vcp_level_f = volume;

    if(volume == 0){
        return 0;
    }

    local_level_f = (vcp_level_f * 15) / 100 + 0.5f;
    return (uint8_t)local_level_f;
}

#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
static void app_le_audio_usb_handle_timeout(TimerHandle_t timer_handle, void *user_data)
{
    app_le_audio_usb_port_mask_t streaming_port;

    g_lea_usb_timer = NULL;
    streaming_port = app_le_audio_usb_get_streaming_port();
    LE_AUDIO_MSGLOG_I("[APP][USB] handle_timeout, port:%x", 1, streaming_port);

#ifdef AIR_LE_AUDIO_DO_NOT_STOP_CALL_MODE_WHEN_CALL_EXIST
    if (app_le_audio_usb_hid_call_existing()) {
    //if (BLE_TBS_STATE_IDLE > (bt_le_audio_source_call_get_state(ble_tbs_get_gtbs_service_idx(), 1))) {
        //LE_AUDIO_MSGLOG_I("[APP][USB] handle_timeout, call", 0);
        return;
    }
#endif

    if (APP_LE_AUDIO_UCST_CREATE_CIS_ALWAYS_BIDIRECTIONAL != app_le_audio_ucst_get_create_cis_mode() &&
        APP_LE_AUDIO_MODE_UCST == app_le_audio_get_current_mode()) {
        app_le_audio_ucst_stop(true);
    }
}


bool app_le_audio_usb_check_timer_exist(void)
{
    if (g_lea_usb_timer) {
        return true;
    }
    return false;
}


static void app_le_audio_usb_start_timer(void)
{
    bt_status_t ret = BT_STATUS_SUCCESS;

    if (!g_lea_usb_timer) {
        ret = app_le_audio_timer_start(&g_lea_usb_timer,
                                       APP_LE_AUDIO_USB_TIMEOUT,
                                       app_le_audio_usb_handle_timeout,
                                       NULL);

        if (BT_STATUS_SUCCESS != ret) {
            app_le_audio_timer_stop(g_lea_usb_timer);
            g_lea_usb_timer = NULL;
        }
    }
}

void app_le_audio_usb_stop_timer(void)
{
    if (g_lea_usb_timer) {
        app_le_audio_timer_stop(g_lea_usb_timer);
        g_lea_usb_timer = NULL;
    }
}
#endif

#if 0//Module slim
app_le_audio_usb_info_t *app_le_audio_usb_get_info(app_le_audio_usb_port_t usb_port)
{
    if (APP_LE_AUDIO_USB_PORT_MAX <= usb_port) {
        return NULL;
    }

    return &g_lea_usb_info[usb_port];
}
#endif

app_le_audio_usb_port_t app_le_audio_usb_get_port(app_usb_audio_port_t port_type, uint8_t port_num)
{
    if((port_type == APP_USB_AUDIO_SPK_PORT)&&(port_num <= APP_LE_AUDIO_USB_PORT_NUM_1)){
        return (APP_LE_AUDIO_USB_PORT_SPK_0 + (port_num - APP_LE_AUDIO_USB_PORT_NUM_0));
    }
    else if((port_type == APP_USB_AUDIO_MIC_PORT)&&(port_num == APP_LE_AUDIO_USB_PORT_NUM_0)){
        return APP_LE_AUDIO_USB_PORT_MIC_0;
    }

    //LE_AUDIO_MSGLOG_I("[APP][USB] get_port, unknown port_type:%x port_num:%x", 2, port_type, port_num);
    return APP_LE_AUDIO_USB_PORT_MAX;
}

app_le_audio_usb_info_t *app_le_audio_usb_get_info_by_port(app_usb_audio_port_t port_type, uint8_t port_num)
{
    app_le_audio_usb_port_t usb_port = app_le_audio_usb_get_port(port_type, port_num);

    if (APP_LE_AUDIO_USB_PORT_MAX <= usb_port) {
        return NULL;
    }

    return &g_lea_usb_info[usb_port];
}


bool app_le_audio_usb_convert_port(uint32_t *usb_port, app_le_audio_usb_port_t port)
{
    if (!usb_port || APP_LE_AUDIO_USB_PORT_MAX <= port) {
        return FALSE;
    }

    *usb_port = (APP_LE_AUDIO_USB_PORT_SPK_1 == port ? 1 : 0);
    return TRUE;
}


bool app_le_audio_usb_is_port_ready(app_le_audio_usb_port_t port)
{
    uint32_t usb_port = 0;
    bool ret = FALSE;

    if (!app_le_audio_usb_convert_port(&usb_port, port)) {
        return FALSE;
    }

    if ((APP_LE_AUDIO_USB_PORT_SPK_0 == port ||
         APP_LE_AUDIO_USB_PORT_SPK_1 == port) &&
        g_lea_usb_info[port].is_streaming &&
        USB_Audio_Get_RX_Sample_Rate(usb_port) &&
        USB_Audio_Get_RX_Sample_Size(usb_port) &&
        USB_Audio_Get_RX_Channel(usb_port)) {
        ret = TRUE;
    }else if (APP_LE_AUDIO_USB_PORT_MIC_0 == port &&
        g_lea_usb_info[port].is_streaming &&
        USB_Audio_Get_TX_Sample_Rate(usb_port) &&
        USB_Audio_Get_TX_Sample_Size(usb_port) &&
        USB_Audio_Get_TX_Channel(usb_port)) {
        ret = TRUE;
    }

    LE_AUDIO_MSGLOG_I("[APP][USB] is_port_ready port:%x ret:%d", 2, port, ret);
    return ret;
}

uint32_t app_le_audio_usb_convert_sample_size(uint8_t sample_size)
{
    if (sample_size >= (sizeof(sample_size_table) / sizeof(uint32_t))) {
        sample_size = 0;
    }
    return sample_size_table[sample_size];
}


uint32_t app_le_audio_usb_convert_sample_rate(uint8_t sample_rate)
{
    if (sample_rate >= (sizeof(sample_rate_table) / sizeof(uint32_t))) {
        sample_rate = 0;
    }
    return sample_rate_table[sample_rate];
}


void app_le_audio_usb_handle_usb_event(app_le_audio_usb_event_enum event, app_usb_audio_port_t port_type, uint8_t port_num)
{
    app_le_audio_mode_t mode = app_le_audio_get_current_mode();
    app_le_audio_usb_port_t port = app_le_audio_usb_get_port(port_type, port_num);
    app_le_audio_stream_port_t stream_port = APP_LE_AUDIO_CONVERT_USB_PORT(port);

    LE_AUDIO_MSGLOG_I("[APP][USB] handle_usb_event event:%d port: %x_%x", 3, event, port_type, port_num);

    if (APP_LE_AUDIO_USB_EVENT_SAMPLE_SIZE_OBTAIN == event ||
        APP_LE_AUDIO_USB_EVENT_SAMPLE_RATE_OBTAIN == event ||
        APP_LE_AUDIO_USB_EVENT_CHANNEL_OBTAIN == event ||
        APP_LE_AUDIO_USB_EVENT_ENABLE == event) {
        if (app_le_audio_usb_is_port_ready(port)) {
            /* All USB info is obtained. Try to start */
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
            app_le_audio_ucst_ctrl_t *ucst_ctrl = app_le_audio_ucst_get_ctrl();

            if (APP_LE_AUDIO_MODE_UCST == mode) {
                if (APP_LE_AUDIO_UCST_PAUSE_STREAM_ALL <= app_le_audio_ucst_get_pause_stream_flag()) {
                    return;
                }

                if (APP_LE_AUDIO_USB_PORT_MIC_0 == port ||
                    APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE != ucst_ctrl->curr_target) {
                    /* MIC port only controls the call mode. */
                    app_le_audio_usb_stop_timer();
                    app_le_audio_ucst_start();
                }

                if ((APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == ucst_ctrl->curr_target ||
                     APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == ucst_ctrl->curr_target) &&
                    APP_LE_AUDIO_UCST_TARGET_NONE == ucst_ctrl->next_target) {
                    if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(stream_port)) {
                        app_le_audio_start_audio_transmitter(stream_port);
                    }
                }
            }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
            if (APP_LE_AUDIO_MODE_BCST == mode) {
                if (APP_LE_AUDIO_USB_PORT_MIC_0 == port) {
                    return;
                }

                if (app_le_audio_bcst_gat_curr_state() == APP_LE_AUDIO_BCST_STATE_IDLE) {
                    app_le_audio_bcst_start();
                    return;
                }

                if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(stream_port)) {
                    app_le_audio_start_audio_transmitter(stream_port);
                }
            }
#endif
        }
    } else if (APP_LE_AUDIO_USB_EVENT_SAMPLE_SIZE_CHANGE == event ||
              APP_LE_AUDIO_USB_EVENT_SAMPLE_RATE_CHANGE == event ||
              APP_LE_AUDIO_USB_EVENT_CHANNEL_CHANGE == event) {
        /* USB info changes. Restart */
        // TODO: no need to restart. Just stop and restart
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
        if (APP_LE_AUDIO_MODE_UCST == mode) {
            app_le_audio_ucst_stop(true);
        }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
        if (APP_LE_AUDIO_MODE_BCST == mode) {
            if (port_type != APP_USB_AUDIO_MIC_PORT) {
                app_le_audio_bcst_stop(true);
            }
        }
#endif
    }
}


static bool app_le_audio_usb_handle_sample_rate_event(app_events_usb_port_t *p_rate, bool update_sample_rate, bool update_size_channel)
{
    app_le_audio_usb_info_t *p_usb_info = NULL;
    app_le_audio_usb_port_t usb_port;
    app_le_audio_stream_port_t stream_port;
    uint32_t sample_rate = 0, rate = 0;
    uint8_t sample_size = 0;
    uint8_t channel = 0;

    /* Ignore value of 0 when it is reported for usb port disable. */
    if (NULL == p_rate) {
        return false;
    }
    uint8_t interface_id = apps_event_usb_get_interface_id_from_port_info(p_rate);
    const apps_usb_interface_enable_app_task_recorder_t *app_record = app_preproc_activity_get_usb_interface_info(interface_id);
    if (app_record == NULL || app_record->sample_rate == 0) {
        return false;
    }

    usb_port = app_le_audio_usb_get_port(p_rate->port_type, p_rate->port_num);
    stream_port = APP_LE_AUDIO_CONVERT_USB_PORT(usb_port);

    p_usb_info = app_le_audio_usb_get_info_by_port(p_rate->port_type, p_rate->port_num);
    if (NULL == p_usb_info) {
        return false;
    }
    if (update_size_channel) {
        sample_size = app_le_audio_get_usb_sample_size_in_use(stream_port);
        LE_AUDIO_MSGLOG_I("[APP][USB] [%d]SAMPLE_SIZE_%x_%x, usb size:%x stream size:%x", 5, interface_id, p_rate->port_type, p_rate->port_num, app_record->sample_size, sample_size);
        if (!sample_size) {
            app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_SAMPLE_SIZE_OBTAIN, p_rate->port_type, p_rate->port_num);
        } else if (sample_size != app_record->sample_size) {
            app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_SAMPLE_SIZE_CHANGE, p_rate->port_type, p_rate->port_num);
        }

        channel = app_le_audio_get_usb_channel_in_use(stream_port);
        LE_AUDIO_MSGLOG_I("[APP][USB] [%d]CHANNEL_%x_%x, usb channel:%x stream channel:%x", 5, interface_id, p_rate->port_type, p_rate->port_num, app_record->channel, channel);
        if (!channel) {
            app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_CHANNEL_OBTAIN, p_rate->port_type, p_rate->port_num);
        } else if (channel != app_record->channel) {
            app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_CHANNEL_CHANGE, p_rate->port_type, p_rate->port_num);
        }
    }

    if (update_sample_rate) {
        sample_rate = app_le_audio_get_usb_sample_rate_in_use(stream_port);
        rate = app_record->sample_rate;
        LE_AUDIO_MSGLOG_I("[APP][USB] [%d]SAMPLE_RATE_%x_%x, usb rate:%x stream rate:%x", 5, interface_id, p_rate->port_type, p_rate->port_num, rate, sample_rate);
        if (!sample_rate) {
            app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_SAMPLE_RATE_OBTAIN, p_rate->port_type, p_rate->port_num);
        } else if (sample_rate != rate) {
            app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_SAMPLE_RATE_CHANGE, p_rate->port_type, p_rate->port_num);
        }
    }

    return true;
}

static bool app_le_audio_usb_handle_play_event(app_events_usb_port_t *p_port)
{
    app_le_audio_usb_port_t port = APP_LE_AUDIO_USB_PORT_MAX;
    uint8_t interface_id;

    if (NULL == p_port) {
        return false;
    }

    LE_AUDIO_MSGLOG_I("[APP][USB] PLAY_%x_%x", 2, p_port->port_type, p_port->port_num);

    port = app_le_audio_usb_get_port(p_port->port_type, p_port->port_num);
    interface_id = apps_event_usb_get_interface_id_from_port_info(p_port);
    if (APP_LE_AUDIO_USB_PORT_MAX <= port ||
        g_lea_usb_info[port].is_streaming || interface_id >= APPS_USB_EVENTS_INTERFACE_MAX) {
        return true;
    }

#if APP_LE_AUDIO_DONGLE_SIMULATE_MEDIA_STATE
    // check if SPK from stop to play, if yes, send MediaState=playing
    if ((APP_LE_AUDIO_USB_PORT_SPK_0 == port) || (APP_LE_AUDIO_USB_PORT_SPK_1 == port)) {
#if AIR_MS_TEAMS_ENABLE
        LE_AUDIO_MSGLOG_I("[APP][USB] PLAY, spk:%d %d call:%d", 3, g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_0].is_streaming, g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_1].is_streaming, app_le_audio_usb_hid_call_existing());
        if (!(g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_0].is_streaming || g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_1].is_streaming)) {
            if (!app_le_audio_usb_hid_call_existing()) {
                bt_le_audio_source_action_param_t le_param;
                le_param.service_index = ble_mcs_get_gmcs_service_idx();
                bt_le_audio_source_send_action(BT_LE_AUDIO_SOURCE_ACTION_MEDIA_PLAY, &le_param);
            }
        }
#else
        LE_AUDIO_MSGLOG_I("[APP][USB] PLAY, spk:%d %d call:%d", 3, g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_0].is_streaming, g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_1].is_streaming, false);
        if (!(g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_0].is_streaming || g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_1].is_streaming)) {
            bt_le_audio_source_action_param_t le_param;
            le_param.service_index = ble_mcs_get_gmcs_service_idx();
            bt_le_audio_source_send_action(BT_LE_AUDIO_SOURCE_ACTION_MEDIA_PLAY, &le_param);
        }
#endif
    }
#endif
    app_le_audio_usb_handle_sample_rate_event(p_port, false, true);

    g_lea_usb_info[port].is_streaming = true;

    app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_ENABLE, p_port->port_type, p_port->port_num);

    app_le_audio_usb_handle_sample_rate_event(p_port, true, false);

    return true;
}

static bool app_le_audio_usb_handle_stop_event(app_events_usb_port_t *p_port)
{
    app_le_audio_usb_port_t port;

    if (NULL == p_port) {
        return false;
    }

    LE_AUDIO_MSGLOG_I("[APP][USB] STOP_%x_%x", 2, p_port->port_type, p_port->port_num);

    if (APP_LE_AUDIO_USB_PORT_MAX == (port = app_le_audio_usb_get_port(p_port->port_type, p_port->port_num))) {
        return false;
    }

#if APP_LE_AUDIO_DONGLE_SIMULATE_MEDIA_STATE
    // check if SPK from play to stop, if yes, send MediaState=paused
    if ((APP_LE_AUDIO_USB_PORT_SPK_0 == port) || (APP_LE_AUDIO_USB_PORT_SPK_1 == port)) {
#if AIR_MS_TEAMS_ENABLE
        LE_AUDIO_MSGLOG_I("[APP][USB] STOP, spk:%d %d call:%d", 3, g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_0].is_streaming, g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_1].is_streaming, app_le_audio_usb_hid_call_existing());
        if (g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_0].is_streaming ^ g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_1].is_streaming) {
            if (!app_le_audio_usb_hid_call_existing()) {
                bt_le_audio_source_action_param_t le_param;
                le_param.service_index = ble_mcs_get_gmcs_service_idx();
                bt_le_audio_source_send_action(BT_LE_AUDIO_SOURCE_ACTION_MEDIA_PAUSE, &le_param);
            }
        }
#else
        LE_AUDIO_MSGLOG_I("[APP][USB] STOP, spk:%d %d call:%d", 3, g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_0].is_streaming, g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_1].is_streaming, false);
        if (g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_0].is_streaming ^ g_lea_usb_info[APP_LE_AUDIO_USB_PORT_SPK_1].is_streaming) {
            bt_le_audio_source_action_param_t le_param;
            le_param.service_index = ble_mcs_get_gmcs_service_idx();
            bt_le_audio_source_send_action(BT_LE_AUDIO_SOURCE_ACTION_MEDIA_PAUSE, &le_param);
        }
#endif
    }
#endif

    g_lea_usb_info[port].is_streaming = false;

#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
    if (APP_LE_AUDIO_MODE_UCST == app_le_audio_get_current_mode()) {
        app_le_audio_ucst_target_t curr_target = app_le_audio_ucst_get_curr_target();
        app_le_audio_usb_port_mask_t streaming_port = app_le_audio_usb_get_streaming_port();

        LE_AUDIO_MSGLOG_I("[APP][USB][U] STOP, curr_target:%x port%x streaming_port:%x", 3, curr_target, port, streaming_port);

        if ((APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == curr_target) ||
            (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == curr_target)) {

            if ((!streaming_port) || (APP_LE_AUDIO_USB_PORT_MIC_0 == port)) {
                app_le_audio_usb_start_timer();
            }
            app_le_audio_stop_audio_transmitter(port);
        }
#ifdef AIR_SILENCE_DETECTION_ENABLE
        else if (APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE == curr_target) {
            app_le_audio_silence_detection_handle_event(APP_LE_AUDIO_SILENCE_DETECTION_EVENT_PORT_DISABLED, (void *)port);
        }
#endif
    }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
    if (APP_LE_AUDIO_MODE_BCST == app_le_audio_get_current_mode()) {
        if (APP_LE_AUDIO_USB_PORT_MIC_0 == port) {
            return true;
        }
        app_le_audio_stop_audio_transmitter(port);
    }
#endif
    return true;
}

static bool app_le_audio_usb_handle_volume_event(app_events_usb_volume_t *p_vol)
{
    app_le_audio_usb_info_t *p_usb_info = NULL;
    app_le_audio_usb_port_t port;

    if (NULL == p_vol) {
        //LE_AUDIO_MSGLOG_I("[APP][USB] volume_event, invalid data", 0);
        return true;
    }

    if (APP_LE_AUDIO_USB_PORT_MAX == (port = app_le_audio_usb_get_port(p_vol->port_type, p_vol->port_num))) {
        return true;
    }

    p_usb_info = &g_lea_usb_info[port];
    LE_AUDIO_MSGLOG_I("[APP][USB] VOLUME_%x_%x, vol:%x %x->%x %x", 6, p_vol->port_type, p_vol->port_num,
                      p_usb_info->vol_left, p_usb_info->vol_right,
                      p_vol->left_volume, p_vol->right_volume);
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
    if ((p_usb_info->vol_left != p_vol->left_volume) &&
        ((APP_LE_AUDIO_VOL_VALUE_MAX == p_vol->left_volume) || (APP_LE_AUDIO_VOL_VALUE_MIN == p_vol->left_volume))) {
        app_le_audio_aird_notify_volume_change(p_vol->port_type,
                                               p_vol->port_num,
                                               p_vol->left_volume);

    } else if ((p_usb_info->vol_right != p_vol->right_volume) &&
        ((APP_LE_AUDIO_VOL_VALUE_MAX == p_vol->right_volume) || (APP_LE_AUDIO_VOL_VALUE_MIN == p_vol->right_volume))) {
        app_le_audio_aird_notify_volume_change(p_vol->port_type,
                                               p_vol->port_num,
                                               p_vol->right_volume);
    }
#endif

    p_usb_info->vol_left = (p_vol->left_volume > APP_LE_AUDIO_VOL_VALUE_MAX)?APP_LE_AUDIO_VOL_VALUE_MAX:p_vol->left_volume;
    p_usb_info->vol_right = (p_vol->right_volume > APP_LE_AUDIO_VOL_VALUE_MAX)?APP_LE_AUDIO_VOL_VALUE_MAX:p_vol->left_volume;


#if 0
    if (p_usb_info->vol_left != p_usb_info->vol_right) {
        if (p_usb_info->vol_left > p_usb_info->vol_right) {
            p_usb_info->vol_right = p_usb_info->vol_left;
        } else {
            p_usb_info->vol_left = p_usb_info->vol_right;
        }
    }
#endif
    app_le_audio_set_audio_transmitter_volume_level(port, app_le_audio_map_vol_level(p_usb_info->vol_left), app_le_audio_map_vol_level(p_usb_info->vol_right));

    return true;
}

#if 0
static bool app_le_audio_usb_handle_sample_size_event(app_events_usb_sample_size_t *p_size)
{
    app_le_audio_usb_info_t *p_usb_info = NULL;
    app_le_audio_usb_port_t usb_port;
    app_le_audio_stream_port_t stream_port;
    uint8_t sample_size = 0;

    /* Ignore value of 0 when it is reported for usb port disable. */
    if (NULL == p_size || !p_size->size) {
        return false;
    }

    usb_port = app_le_audio_usb_get_port(p_size->port_type, p_size->port_num);
    stream_port = APP_LE_AUDIO_CONVERT_USB_PORT(usb_port);
    sample_size = app_le_audio_get_usb_sample_size_in_use(stream_port);

    LE_AUDIO_MSGLOG_I("[APP][USB] SAMPLE_SIZE_%x_%x, usb size:%x stream size:%x", 4, p_size->port_type, p_size->port_num, p_size->size, sample_size);

    p_usb_info = app_le_audio_usb_get_info_by_port(p_size->port_type, p_size->port_num);
    if (NULL == p_usb_info) {
        return false;
    }

    if (!sample_size) {
        app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_SAMPLE_SIZE_OBTAIN, p_size->port_type, p_size->port_num);
    } else if (sample_size != p_size->size) {
        app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_SAMPLE_SIZE_CHANGE, p_size->port_type, p_size->port_num);
    }

    return true;
}


static bool app_le_audio_usb_handle_channel_event(app_events_usb_channel_t *p_channel)
{
    app_le_audio_usb_info_t *p_usb_info = NULL;
    app_le_audio_usb_port_t usb_port;
    app_le_audio_stream_port_t stream_port;
    uint8_t channel = 0;

    /* Ignore value of 0 when it is reported for usb port disable. */
    if (NULL == p_channel || !p_channel->channel) {
        return false;
    }

    usb_port = app_le_audio_usb_get_port(p_channel->port_type, p_channel->port_num);
    stream_port = APP_LE_AUDIO_CONVERT_USB_PORT(usb_port);
    channel = app_le_audio_get_usb_channel_in_use(stream_port);

    LE_AUDIO_MSGLOG_I("[APP][USB] CHANNEL_%x_%x, usb channel:%x stream channel:%x", 4, p_channel->port_type, p_channel->port_num, p_channel->channel, channel);

    p_usb_info = app_le_audio_usb_get_info_by_port(p_channel->port_type, p_channel->port_num);
    if (NULL == p_usb_info) {
        return false;
    }

    if (!channel) {
        app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_CHANNEL_OBTAIN, p_channel->port_type, p_channel->port_num);
    } else if (channel != p_channel->channel) {
        app_le_audio_usb_handle_usb_event(APP_LE_AUDIO_USB_EVENT_CHANNEL_CHANGE, p_channel->port_type, p_channel->port_num);
    }

    return true;
}
#endif

static bool app_le_audio_usb_handle_mute_event(app_events_usb_mute_t *p_mute)
{
    app_le_audio_usb_info_t *p_usb_info;
    app_le_audio_usb_port_t port;

    if (NULL == p_mute) {
        //LE_AUDIO_MSGLOG_I("[APP][USB] mute_event, invalid data", 0);
        return true;
    }

    if (APP_LE_AUDIO_USB_PORT_MAX == (port = app_le_audio_usb_get_port(p_mute->port_type, p_mute->port_num))) {
        return true;
    }

    p_usb_info = &g_lea_usb_info[port];

#ifndef APP_LE_AUDIO_USB_LOG_SLIM
    if (APP_USB_AUDIO_SPK_PORT == p_mute->port_type) {
        LE_AUDIO_MSGLOG_I("[APP][USB] MUTE_SPK_%x, mute:%x->%x", 3, p_mute->port_num,
                          p_usb_info->mute, p_mute->is_mute);
    } else if (APP_USB_AUDIO_MIC_PORT == p_mute->port_type) {
        LE_AUDIO_MSGLOG_I("[APP][USB] MUTE_MIC_%x, mute:%x->%x", 3, p_mute->port_num,
                          p_usb_info->mute, p_mute->is_mute);
    }
#else
    LE_AUDIO_MSGLOG_I("[APP][USB] MUTE_%x_%x, mute:%x->%x", 4, p_mute->port_type,p_mute->port_num,
                          p_usb_info->mute, p_mute->is_mute);
#endif

    if (p_mute->is_mute != p_usb_info->mute) {
        p_usb_info->mute = p_mute->is_mute;
    }

    if (p_mute->is_mute) {
        app_le_audio_mute_audio_transmitter(port);
    } else {
        app_le_audio_unmute_audio_transmitter(port);
    }

    return true;
}

app_le_audio_usb_port_mask_t app_le_audio_usb_get_streaming_port(void)
{
    uint8_t state = 0, i;

    i = APP_LE_AUDIO_USB_PORT_MAX;
    while (i > 0) {
        i --;
        if (g_lea_usb_info[i].is_streaming) {
            state |= (1 << i);
        }
    }

    return state;
}

void app_le_audio_usb_init(void)
{
    memset(&g_lea_usb_info, 0, sizeof(app_le_audio_usb_info_t)*APP_LE_AUDIO_USB_PORT_MAX);
}

bool app_le_audio_idle_usb_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    uint32_t usb_data = (uint32_t)extra_data;
    LE_AUDIO_MSGLOG_I("[APP][USB] usb_event_proc, event_id:%x usb_data:%x", 2, event_id,usb_data);

    switch (event_id) {
        case APPS_EVENTS_USB_AUDIO_UNPLUG:
        case APPS_EVENTS_USB_AUDIO_PLAY:
        case APPS_EVENTS_USB_AUDIO_STOP: {
            app_events_usb_port_t *usb_port = (app_events_usb_port_t *) & (usb_data);
            if (usb_port) {
#if (defined(AIR_USB_AUDIO_ENABLE) && ! defined(APPS_USB_AUDIO_SUPPORT))|| defined (AIR_USB_AUDIO_1_MIC_ENABLE)
                uint8_t interface_id = apps_event_usb_get_interface_id_from_port_info(usb_port);
                if (interface_id < APPS_USB_EVENTS_INTERFACE_MAX) {
                    const apps_usb_interface_enable_app_task_recorder_t *app_recorder = app_preproc_activity_get_usb_interface_info(interface_id);
                    if (app_recorder) {
                        if (app_recorder->enabled) {
                            app_le_audio_usb_handle_play_event((app_events_usb_port_t *) & (usb_data));
                        } else {
                            app_le_audio_usb_handle_stop_event((app_events_usb_port_t *) & (usb_data));
                        }
                    }
                }
#endif
            }
            break;
        }
        case APPS_EVENTS_USB_AUDIO_VOLUME: {
            app_le_audio_usb_handle_volume_event((app_events_usb_volume_t *)extra_data);
            break;
        }
        case APPS_EVENTS_USB_AUDIO_MUTE: {
            app_le_audio_usb_handle_mute_event((app_events_usb_mute_t *) & (usb_data));
            break;
        }
        case APPS_EVENTS_USB_AUDIO_SAMPLE_RATE: {
            app_events_usb_sample_rate_t *p_sample_rate = (app_events_usb_sample_rate_t *) & (usb_data);
            if (p_sample_rate) {
#if (defined(AIR_USB_AUDIO_ENABLE) && ! defined(APPS_USB_AUDIO_SUPPORT))|| defined (AIR_USB_AUDIO_1_MIC_ENABLE)
                uint8_t interface_id = apps_event_usb_get_interface_id_from_port_info(&p_sample_rate->port);
                if (interface_id < APPS_USB_EVENTS_INTERFACE_MAX) {
                    app_le_audio_usb_handle_sample_rate_event(&p_sample_rate->port, true, true);
                }
#endif
            }
            break;
        }
/*
        case APPS_EVENTS_USB_AUDIO_SAMPLE_SIZE: {
            app_le_audio_usb_handle_sample_size_event((app_events_usb_sample_size_t *) & (usb_data));
            break;
        }
        case APPS_EVENTS_USB_AUDIO_CHANNEL: {
            app_le_audio_usb_handle_channel_event((app_events_usb_channel_t *) & (usb_data));
            break;
        }
*/

        default:
            break;
    }

    /* Always return ture to let the event continure to be processed by the following registers. */
    return true;
}

#endif  /* AIR_LE_AUDIO_ENABLE */

