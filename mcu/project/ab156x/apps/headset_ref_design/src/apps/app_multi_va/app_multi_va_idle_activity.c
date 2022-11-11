/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 * File: app_multi_va_idle_activity.c
 *
 * Description: This file provides utility function to handle BT event for multi va app.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for Homescreen APP.
 *
 */

#include "app_multi_va_idle_activity.h"
#include "multi_ble_adv_manager.h"
#include "multi_va_manager.h"
#include "multi_va_event_id.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_events_bt_event.h"
#include "bt_sink_srv.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "apps_config_features_dynamic_setting.h"
#include "apps_aws_sync_event.h"
#include "apps_debug.h"
#include "bt_app_common.h"
#include "apps_events_interaction_event.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "app_rho_idle_activity.h"
#include "bt_aws_mce_srv.h"
#endif

#define LOG_TAG     "[MULTI_VA_ACT]"    /* Log tag. */

#define ADV_NORMAL_MODE     (0)     /* This defines the variable of s_low_power_mode, means not in low power mode */
#define LOW_POWER_MODE      (1)     /* This defines the variable of s_low_power_mode, means in low power mode */
static uint8_t s_low_power_mode = ADV_NORMAL_MODE;  /* The variable records if current system mode is low power mode. */

/**
 * @brief      This function is the implementation of get_ble_adv_data_func_t, the content is the adv for smart phone UT APP.
 * @param[out] adv_info, the implemented function should fill the contains of the parameter adv_info.
 * @return     The bits of the return value means adv_param, adv_data or scan_rsp need generated by multi_ble_adv_manager.
 */
static uint32_t get_default_ble_adv_data_func(multi_ble_adv_info_t *adv_info)
{
    if (adv_info) {
        bt_app_common_generate_default_adv_data(adv_info->adv_param,
                                                adv_info->adv_data,
                                                adv_info->scan_rsp, NULL, 0);
    }
    return 0;
}

/*****************************************************************************
 ********************************handle functions*****************************
 *****************************************************************************/
/**
 * @brief      This function processing function of EVENT_GROUP_UI_SHELL_SYSTEM events.
 * @param[in]  self, the context pointer of the activity.
 * @param[in]  event_id, the current event ID to be handled.
 * @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
 * @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
 * @return     If return true, the current event cannot be handle by the next activity.
 */
static bool app_multi_va_proc_ui_shell_group(ui_shell_activity_t *self,
                                             uint32_t event_id,
                                             void *extra_data,
                                             size_t data_len)
{
    bool ret = true;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            /* Read system mode is low power mode or not. */
            uint32_t tsize = sizeof(s_low_power_mode);
            nvkey_status_t status = nvkey_read_data(NVID_APP_SYSTEM_MODE_SETTING, &s_low_power_mode, &tsize);
            if (status != NVKEY_STATUS_OK) {
                s_low_power_mode = ADV_NORMAL_MODE;
            }
            break;
        }
        default:
            break;
    }

    return ret;
}

/**
 * @brief      This function processing function of EVENT_GROUP_UI_SHELL_BT events.
 * @param[in]  self, the context pointer of the activity.
 * @param[in]  event_id, the current event ID to be handled.
 * @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
 * @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
 * @return     If return true, the current event cannot be handle by the next activity.
 */
static bool app_multi_va_proc_bt_group(ui_shell_activity_t *self,
                                       uint32_t event_id,
                                       void *extra_data,
                                       size_t data_len)
{
    bool ret = false;

    /* The multi_ble_adv want to receive some events. Ex. BT_GAP_LE_DISCONNECT_IND event means BLE disconnected. */
    multi_ble_adv_manager_bt_event_proc(event_id, extra_data, data_len);
    return ret;
}

/**
 * @brief      This function processing function of EVENT_GROUP_UI_SHELL_APP_INTERACTION events.
 * @param[in]  self, the context pointer of the activity.
 * @param[in]  event_id, the current event ID to be handled.
 * @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
 * @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
 * @return     If return true, the current event cannot be handle by the next activity.
 */
static bool app_multi_va_proc_interaction_group(ui_shell_activity_t *self,
                                                uint32_t event_id,
                                                void *extra_data,
                                                size_t data_len)
{
    bool ret = false;
    /* The multi_ble_adv want to receive some events. Ex. APPS_EVENTS_INTERACTION_RHO_END event means RHO is done. */
    multi_ble_adv_manager_interaction_proc(event_id, extra_data, data_len);
    return ret;
}

/**
 * @brief      This function processing function of EVENT_GROUP_UI_SHELL_MULTI_VA events.
 * @param[in]  self, the context pointer of the activity.
 * @param[in]  event_id, the current event ID to be handled.
 * @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
 * @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
 * @return     If return true, the current event cannot be handle by the next activity.
 */
static bool app_multi_va_proc_multi_va_group(ui_shell_activity_t *self,
                                             uint32_t event_id,
                                             void *extra_data,
                                             size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case MULTI_VA_EVENT_SET_VA:
            /* Notify the multi_voice_assistant_manager when the VA is configured by Smart phone  */
            multi_voice_assistant_manager_va_config_changed();
            ret = true;
            break;
        default:
            break;
    }

    if (!ret) {
        /* The multi_ble_adv want to receive some events. Ex. MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL to change the adv interval. */
        multi_ble_adv_manager_multi_va_proc(event_id, extra_data, data_len);
    }
    return ret;
}

/**
 * @brief      This function processing function of EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER events.
 * @param[in]  self, the context pointer of the activity.
 * @param[in]  event_id, the current event ID to be handled.
 * @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
 * @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
 * @return     If return true, the current event cannot be handle by the next activity.
 */
static bool app_multi_va_proc_bt_cm_group(ui_shell_activity_t *self,
                                          uint32_t event_id,
                                          void *extra_data,
                                          size_t data_len)
{
    bool ret = false;

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role;
    role = bt_device_manager_aws_local_info_get_role();
#endif

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == remote_update) {
                break;
            }

            /* When BT EDR any non-AWS profile connected. */
            if (!(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                && ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service
#ifdef MTK_AWS_MCE_ENABLE
                && (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
               ) {
                APPS_LOG_MSGID_I(LOG_TAG"start ble adv caused by profile connected", 0);
                /* Add adv when BT EDR profile connected. */
                if (!multi_voice_assistant_manager_enable_adv(true)) {
                }
                /* Add the default adv. */
                multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, get_default_ble_adv_data_func, 1);
                multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
            }

            /* Remove adv when BT EDR is disconnected. */
            if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state) {
                /* Confirm all smart phones are disconneted to avoid mistakes in multi link case. */
                if (0 == bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0)
#ifdef MTK_AWS_MCE_ENABLE
                    && (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
                   ) {
                    APPS_LOG_MSGID_I(LOG_TAG"SP disconnected, remove adv", 0);
                    if (!multi_voice_assistant_manager_enable_adv(false)) {
                    }
                    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, get_default_ble_adv_data_func);
                    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
                }
            }

#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
            /* When AWS connected. */
            if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                && BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service) {
                if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
                    /* Agent send local random BLE address to partner. */
                    multi_ble_adv_manager_sync_ble_addr();
#ifdef MULTI_VA_SUPPORT_COMPETITION
                    APPS_LOG_MSGID_I(LOG_TAG"partner attached, need sync va to partner", 0);
                    /* Agent send VA type to partner. */
                    multi_va_manager_send_va_type_to_partner();
#endif
                } else {
                    /* The AWS link type is LINK_NORMAL for partner means has connected to smart phone. */
                    if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                        APPS_LOG_MSGID_I(LOG_TAG"Partner add adv when AWS connected NORMAL", 0);
                        if (!multi_voice_assistant_manager_enable_adv(true)) {
                        }
                        multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, get_default_ble_adv_data_func, 1);
                    }
                    /* Since the partner only recored the added adv but not start adv, it's not neccesary to notify. */
                }
            } else if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service
                       && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                /* When AWS disconnected. */
                if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
#ifdef MULTI_VA_SUPPORT_COMPETITION
                    multi_va_manager_on_partner_detached();
#endif
                } else {
                    if (!multi_voice_assistant_manager_enable_adv(false)) {
                    }
                    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, get_default_ble_adv_data_func);
                    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
                    APPS_LOG_MSGID_I(LOG_TAG"Partner remove default ble adv caused by AWS disconencted", 0);
                }
            }
#endif
            break;
        }
        default:
            break;
    }
    return ret;
}

/**
 * @brief      This function processing function of EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER events.
 * @param[in]  self, the context pointer of the activity.
 * @param[in]  event_id, the current event ID to be handled.
 * @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
 * @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
 * @return     If return true, the current event cannot be handle by the next activity.
 */
static bool app_multi_va_proc_bt_dm_group(ui_shell_activity_t *self,
                                          uint32_t event_id,
                                          void *extra_data,
                                          size_t data_len)
{
    bool ret = false;
    bt_device_manager_power_event_t evt;
    bt_device_manager_power_status_t status;
    bt_event_get_bt_dm_event_and_status(event_id, &evt, &status);
    switch (evt) {
        case BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status
                || BT_DEVICE_MANAGER_POWER_STATUS_AIR_PAIRING_COMPLETE == status
                || BT_DEVICE_MANAGER_POWER_STATUS_ROLE_RECOVERY == status) {
                if (apps_config_features_is_mp_test_mode()) {
                    /* In MP test mode, the tool want to scan the BLE adv and connect it. */
                    APPS_LOG_MSGID_I(LOG_TAG"add default ble adv when mp_test_mode and BT ON", 0);
                    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_DEFAULT, get_default_ble_adv_data_func, 1);
                    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_DEFAULT);
                }
#if defined(AIR_AMA_ENABLE) && defined(AIR_AMA_ADV_ENABLE_BEFORE_EDR_CONNECT_ENABLE)
                /**
                 * @brief Add the AMA ADV after power on.
                 */
                if (multi_voice_assistant_manager_get_ama_adv_function() != NULL) {
                    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_VA, multi_voice_assistant_manager_get_ama_adv_function(), 1);
                    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_VA);
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG"AMA ADV callback is NULL", 0);
                }
#endif /* AIR_AMA_ENABLE && AIR_AMA_ADV_ENABLE_BEFORE_EDR_CONNECT_ENABLE */
                APPS_LOG_MSGID_I(LOG_TAG"start ble adv caused by BT ON low_power_mode:%d", 1, s_low_power_mode);
                /* Start BLE adv when BT is enabled. */
                if (LOW_POWER_MODE != s_low_power_mode) {
                    multi_ble_adv_manager_start_ble_adv();
                }
            }
            break;
        }
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_RESET_TYPE_NORMAL == status
                || BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_START == status
                || BT_DEVICE_MANAGER_POWER_STATUS_ROLE_RECOVERY == status) {
                APPS_LOG_MSGID_I(LOG_TAG"stop ble adv caused by BT OFF", 0);
                /* Disable BLE adv when BT is disable. */
                multi_ble_adv_manager_stop_ble_adv();
            }
            break;
        }
        default:
            break;
    }

    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
/**
 * @brief      This function processing function of EVENT_GROUP_UI_SHELL_AWS_DATA events.
 * @param[in]  self, the context pointer of the activity.
 * @param[in]  event_id, the current event ID to be handled.
 * @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
 * @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
 * @return     If return true, the current event cannot be handle by the next activity.
 */
static bool app_multi_va_proc_aws_group(ui_shell_activity_t *self,
                                        uint32_t unused_id,
                                        void *extra_data,
                                        size_t data_len)
{
    bool ret = false;

    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t event_id;
        void *aws_extra_data;
        uint32_t extra_len;
        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event_id, &aws_extra_data, &extra_len);

        switch (event_group) {
            case EVENT_GROUP_UI_SHELL_MULTI_VA:
                switch (event_id) {
#ifdef MULTI_VA_SUPPORT_COMPETITION
                    case MULTI_VA_EVENT_SYNC_VA_TYPE_TO_PARTNER: {
                        multi_va_manager_receive_va_change_from_agent(aws_extra_data);
                        ret = true;
                    }
                    break;
                    case MULTI_VA_EVENT_SYNC_REBOOT: {
                        /* When VA is switched, need reboot agent and partner at the same time. */
                        multi_va_manager_receive_reboot_from_agent();
                        ret = true;
                    }
                    break;
#endif
                    default:
                        break;
                }
                if (!ret) {
                    /* The multi_ble_adv want to receive some events. Ex. MULTI_VA_EVENT_SYNC_BLE_ADDRESS to receive the BLE random address. */
                    ret = multi_ble_adv_manager_aws_data_proc(unused_id, extra_data, data_len);
                }
                break;
            default:
                break;
        }
    }
    return ret;
}
#endif

bool app_multi_va_idle_activity_proc(ui_shell_activity_t *self,
                                     uint32_t event_group,
                                     uint32_t event_id,
                                     void *extra_data,
                                     size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = app_multi_va_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT:
            ret = app_multi_va_proc_bt_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = app_multi_va_proc_bt_cm_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER: {
            ret = app_multi_va_proc_bt_dm_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_LE_SERVICE:
            ret = multi_ble_adv_manager_le_service_proc(event_id, extra_data, data_len);
            break;
#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = app_multi_va_proc_aws_group(self, event_id, extra_data, data_len);
            break;
#endif
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = app_multi_va_proc_interaction_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_MULTI_VA:
            ret = app_multi_va_proc_multi_va_group(self, event_id, extra_data, data_len);
            break;
        default:
            break;
    }
    return ret;
}
