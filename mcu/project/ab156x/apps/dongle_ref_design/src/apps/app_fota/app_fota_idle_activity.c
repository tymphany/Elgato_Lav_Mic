/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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

/**
 * File: app_fota_idle_activity.c
 *
 * Description: This file is the activity to handle fota state and request reboot.
 *
 * Note: See doc/AB158X_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#include "app_fota_idle_activity.h"

#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "bt_device_manager.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "race_app_race_event_hdl.h"
#include "race_app_bt_event_hdl.h"
#include "race_app_aws_event_hdl.h"
#endif
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"
#endif
#include "ui_shell_manager.h"

#ifdef AIR_LE_AUDIO_ENABLE
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
#include "app_le_audio_ucst.h"
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
#include "app_le_audio_bcst.h"
#endif
#endif

#ifdef MTK_PORT_SERVICE_BT_ENABLE
#include "ble_air_internal.h"
#endif

#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
#include "bt_ull_service.h"
#endif
#include "bt_sink_srv.h"

#define LOG_TAG                 "[FOTA_APP]"

/**
 *  @brief This structure defines the context of app_fota_idle_activity.
 */
typedef struct {
    app_fota_state_t    state;                  /* Record the current fota state. */
    bool                ota_ongoing;            /* Record OTA ongoing flag. */
} app_fota_context_t;

static app_fota_context_t s_app_fota_context = {0};

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
static bool app_fota_check_ull_chat_status(void)
{
    bt_ull_streaming_info_t info = {0};
    bt_status_t ret = BT_STATUS_FAIL;
    bt_ull_streaming_t streaming = {
        .streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER,
        .port = 0,
    };
    ret = bt_ull_get_streaming_info(streaming, &info);
    if (BT_STATUS_SUCCESS == ret) {
        return info.is_playing;
    }
    return FALSE;

}

static bool app_fota_check_ull_streaming_status(void)
{
    bt_ull_streaming_info_t info = {0};
    bt_status_t ret = BT_STATUS_FAIL;
    bt_ull_streaming_t streaming = {
        .streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER,
        .port = 1,
    };
    ret = bt_ull_get_streaming_info(streaming, &info);
    if (BT_STATUS_SUCCESS == ret) {
        return info.is_playing;
    }
    return FALSE;
}

#endif

/**
* @brief      This function is weak symbol implement, used in Race FOTA module for RACE_FOTA_ADAPTIVE_MODE.
*             Customer could configure "is_busy" variable according to own scene.
*/
void race_get_device_busy_status(bool *is_busy)
{
    bool is_bis_streaming = FALSE;
    bool is_cis_streaming = FALSE;
    bool is_ull_streaming = FALSE;
    bt_sink_srv_state_t bt_sink_state = bt_sink_srv_get_state();
#ifdef AIR_LE_AUDIO_ENABLE
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
    is_cis_streaming = app_le_audio_ucst_is_streaming();
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
    is_bis_streaming = app_le_audio_bcst_is_streaming();
#endif
#endif
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    if (app_fota_check_ull_chat_status() || app_fota_check_ull_streaming_status()) {
        is_ull_streaming = TRUE;
    }
#endif
    APPS_LOG_MSGID_I(LOG_TAG" race_get_device_busy_status, state=%d [LEA][BIS]is_cis_streaming=%d, is_bis_streaming=%d, is_ull_streaming=%d",
                     4, bt_sink_state, is_cis_streaming, is_bis_streaming, is_ull_streaming);
    if (is_busy != NULL) {
        *is_busy = (bt_sink_state >= BT_SINK_SRV_STATE_STREAMING || is_cis_streaming || is_bis_streaming || is_ull_streaming);
    }
}

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
static app_power_saving_target_mode_t app_fota_get_power_saving_target_mode(void)
{
    if (app_fota_get_ota_ongoing()) {
        return APP_POWER_SAVING_TARGET_MODE_NORMAL;
    } else {
        return APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
    }
}
#endif

#ifdef MTK_RACE_CMD_ENABLE
/**
* @brief      This function shows and updates the state of fota.
* @param[in]  self, the context pointer of the activity, can't be NULL.
* @param[in]  state, current state of fota.
*/
static void app_fota_idle_update_state(app_fota_state_t state)
{
    APPS_LOG_MSGID_I(LOG_TAG" update_state, state=%d->%d", 2, s_app_fota_context.state, state);
    if (s_app_fota_context.state != state) {
        s_app_fota_context.state = state;
        switch (state) {
            case APP_FOTA_STATE_IDLE: {
                APPS_LOG_MSGID_I(LOG_TAG" FOTA IN IDLE", 0);
                app_fota_set_ota_ongoing(FALSE);
                break;
            }
            case  APP_FOTA_STATE_START: {
                APPS_LOG_MSGID_I(LOG_TAG" FOTA START", 0);
                app_fota_set_ota_ongoing(TRUE);
                break;
            }
            case APP_FOTA_STATE_CANCELLING: {
                APPS_LOG_MSGID_I(LOG_TAG" FOTA CANCELLING", 0);
                app_fota_set_ota_ongoing(TRUE);
                break;
            }
            case APP_FOTA_STATE_CANCELLED: {
                APPS_LOG_MSGID_I(LOG_TAG" FOTA CANCELLED", 0);
                app_fota_set_ota_ongoing(FALSE);
                break;
            }
        }
    }
}
#endif

static bool app_fota_idle_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(LOG_TAG" create", 0);
            memset(&s_app_fota_context, 0, sizeof(app_fota_context_t));
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            app_power_save_utils_register_get_mode_callback(app_fota_get_power_saving_target_mode);
#endif
            break;
        };
    }
    return TRUE;
}

/**
* @brief      This function process the events of race fota.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return TRUE, the current event cannot be handle by the next activity.
*/
#ifdef MTK_RACE_CMD_ENABLE
static bool app_fota_idle_proc_fota_group(struct _ui_shell_activity *self,
                                          uint32_t event_id,
                                          void *extra_data,
                                          size_t data_len)
{
    bool ret = FALSE;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG" FOTA [%02X] event_id=%d", 2, role, event_id);
#endif
    switch (event_id) {
        case RACE_EVENT_TYPE_FOTA_START: {
            app_fota_idle_update_state(APP_FOTA_STATE_START);
            ble_air_link_performace_optimization();
#ifdef MTK_AWS_MCE_ENABLE
            if (role == BT_AWS_MCE_ROLE_AGENT)
#endif
            {
                apps_config_set_vp(VP_INDEX_SUCCESSED, TRUE, 200, VOICE_PROMPT_PRIO_MEDIUM, FALSE, NULL);
                apps_config_set_foreground_led_pattern(LED_INDEX_FOTA_START, 30, FALSE);
            }
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            app_power_save_utils_notify_mode_changed(false, app_fota_get_power_saving_target_mode);
#endif
            break;
        }
        case RACE_EVENT_TYPE_FOTA_CANCELLING: {
            app_fota_idle_update_state(APP_FOTA_STATE_CANCELLING);
            break;
        }
        case RACE_EVENT_TYPE_FOTA_CANCEL: {
            app_fota_idle_update_state(APP_FOTA_STATE_CANCELLED);
            ble_air_link_performace_optimization_revert();
#ifdef MTK_AWS_MCE_ENABLE
            if (role == BT_AWS_MCE_ROLE_AGENT)
#endif
            {
                apps_config_set_foreground_led_pattern(LED_INDEX_FOTA_CANCELLED, 30, FALSE);
            }
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            app_power_save_utils_notify_mode_changed(false, app_fota_get_power_saving_target_mode);
#endif
            break;
        }
        case RACE_EVENT_TYPE_FOTA_TRANSFER_COMPLETE: {
            app_fota_idle_update_state(APP_FOTA_STATE_IDLE);
            break;
        }
        case RACE_EVENT_TYPE_FOTA_NEED_REBOOT: {
            APPS_LOG_MSGID_I(LOG_TAG" need reboot", 0);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGHEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_REQUEST_REBOOT, NULL, 0,
                                NULL, 0);
            break;
        }
    }
    return ret;
}
#endif

bool app_fota_idle_activity_proc(struct _ui_shell_activity *self,
                                 uint32_t event_group,
                                 uint32_t event_id,
                                 void *extra_data,
                                 size_t data_len)
{
    bool ret = FALSE;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            /* UI Shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = app_fota_idle_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef MTK_RACE_CMD_ENABLE
        case EVENT_GROUP_UI_SHELL_FOTA: {
            /* FOTA events sent by race_cmd_fota. */
            ret = app_fota_idle_proc_fota_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* BT sink events, notify race cmd. */
            APPS_LOG_MSGID_I(LOG_TAG", event_id : %x", 1, event_id);
#ifdef MTK_RACE_CMD_ENABLE
            race_app_bt_sink_event_handler(event_id, extra_data);
#endif
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            /* BT connection manager events, notify race cmd. */
#ifdef MTK_RACE_CMD_ENABLE
            race_app_bt_cm_event_handler(event_id, extra_data);
#ifdef RACE_AWS_ENABLE
            race_app_aws_cm_event_handler(event_id, extra_data);
#endif
#endif
            break;
        }
        default: {
            break;
        }
    }
    return ret;
}

void app_fota_set_ota_ongoing(bool ongoing)
{
    APPS_LOG_MSGID_I(LOG_TAG" set_ota_ongoing, %d->%d", 2, s_app_fota_context.ota_ongoing, ongoing);
    if (s_app_fota_context.ota_ongoing != ongoing) {
        s_app_fota_context.ota_ongoing = ongoing;
    }
}

bool app_fota_get_ota_ongoing(void)
{
    APPS_LOG_MSGID_I(LOG_TAG" get_ota_ongoing, %d", 1, s_app_fota_context.ota_ongoing);
    return s_app_fota_context.ota_ongoing;
}
