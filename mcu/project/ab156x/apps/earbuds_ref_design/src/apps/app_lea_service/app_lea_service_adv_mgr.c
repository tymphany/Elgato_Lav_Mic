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

#if defined(AIR_LE_AUDIO_ENABLE) || defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)

#include "app_lea_service_adv_mgr.h"

#include "app_lea_service.h"
#include "app_lea_service_conn_mgr.h"
#include "app_lea_service_event.h"
#include "app_lea_service_sync_info.h"
#include "app_lea_service_target_addr.h"

#ifdef AIR_LE_AUDIO_ENABLE
#include "ble_ascs_def.h"
#include "ble_bass_def.h"
#include "ble_cas_def.h"
#include "ble_csis.h"
#include "ble_pacs.h"
#include "bt_gap_le_audio.h"
#endif
#ifdef AIR_BLE_ULTRA_LOW_LATENCY_ENABLE
#include "bt_ull_le_service.h"
#endif

#include "bt_hci.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_le.h"
#include "bt_sink_srv_le_cap_audio_manager.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#include "apps_aws_sync_event.h"
#endif

#include "app_bt_state_service.h"
#include "apps_debug.h"
#include "apps_events_bt_event.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_config_features_dynamic_setting.h"
#include "multi_ble_adv_manager.h"

#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#include "bt_customer_config.h"
#include "bt_device_manager.h"
#include "ui_shell_manager.h"
#include "FreeRTOS.h"

#include "app_bt_conn_manager.h"
#include "bt_sink_srv_ami.h"

#define LOG_TAG     "[LEA][ADV]"



typedef enum {
    APP_LEA_ADV_ERROR_OK                       = 0,
    APP_LEA_ADV_ERROR_LC3_DISABLE,
    APP_LEA_ADV_ERROR_MP_TEST_MODE,
    APP_LEA_ADV_ERROR_MAX_LINK,
    APP_LEA_ADV_ERROR_MAX_SUPPORT_LINK,
    APP_LEA_ADV_ERROR_ADV_MODE,
    APP_LEA_ADV_ERROR_NO_TARGET_ADDR,
    APP_LEA_ADV_ERROR_WIRED_AUDIO,
    APP_LEA_ADV_ERROR_WIRELESS_MIC_DISALLOW,
    APP_LEA_ADV_ERROR_APP_CONN_DISALLOW,
    APP_LEA_ADV_ERROR_UPDATE_STOP,
} app_lea_adv_error_code_t;

typedef struct {
    uint8_t     mode;
    uint32_t    timeout;
} app_lea_adv_param_t;

#define APP_LE_AUDIO_RSI_LENGTH             6

static bool         app_lea_adv_enabled = FALSE;
static uint32_t     app_lea_adv_timeout = APP_LE_AUDIO_ADV_TIME;   /* 0 - always */

static uint16_t     app_lea_adv_interval_min = APP_LE_AUDIO_ADV_INTERVAL_MIN_S;
static uint16_t     app_lea_adv_interval_max = APP_LE_AUDIO_ADV_INTERVAL_MAX_S;

typedef enum {
    APP_LEA_ADV_TIMEOUT_ACTION_STOP                 = 0,
    APP_LEA_ADV_TIMEOUT_ACTION_START,
} app_lea_adv_timeout_action_t;

static uint8_t      app_lea_adv_mode = APP_LEA_ADV_MODE_NONE;
extern uint8_t      app_lea_adv_target_addr_num;
extern bt_addr_t    app_lea_adv_target_addr[];

#if defined(AIR_LE_AUDIO_ENABLE) && defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
bool                app_lea_adv_add_ull_data        = FALSE;
#endif



/**================================================================================*/
/**                                   Internal API                                 */
/**================================================================================*/
extern bt_status_t bt_gatts_service_set_le_audio_device_name(const uint8_t *device_name, uint16_t length);
static void app_lea_adv_mgr_do_stop_advertising(void);

#ifdef AIR_LE_AUDIO_DIRECT_ADV
static uint8_t app_lea_adv_mgr_convert_addr_type(bt_addr_type_t addr_type)
{
    uint8_t direct_adv_addr_type = 0xFF;       // 0xFF is invalid value
    if (addr_type == BT_ADDR_PUBLIC || addr_type == BT_ADDR_PUBLIC_IDENTITY) {
        direct_adv_addr_type = 0;
    } else if (addr_type == BT_ADDR_RANDOM || addr_type == BT_ADDR_RANDOM_IDENTITY) {
        direct_adv_addr_type = 1;
    }
    return direct_adv_addr_type;
}
#endif

static bool app_lea_adv_mgr_is_esco_ongoing(void)
{
    bool ret = FALSE;
    bt_sink_srv_device_state_t state_list[3] = {0};
    bt_sink_srv_get_device_state(NULL, state_list, 3);
    for (int i = 0; i < 3; i++) {
        if (state_list[i].sco_state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED) {
            ret = TRUE;
            break;
        }
    }
    return ret;
}

static uint32_t app_lea_adv_mgr_get_adv_data_internal(multi_ble_adv_info_t *adv_data)
{
    /* SCAN RSP */
    if (NULL != adv_data->scan_rsp) {
        adv_data->scan_rsp->data_length = 0;
    }

    /* ADV DATA */
    if ((NULL != adv_data->adv_data) && (NULL != adv_data->adv_data->data)) {
        uint8_t len = 0;
        uint8_t rsi[APP_LE_AUDIO_RSI_LENGTH] = {0};

#ifdef AIR_BLE_ULTRA_LOW_LATENCY_ENABLE
        uint8_t ull_uuid[BT_ULL_LE_MAX_UUID_LENGTH] = {0};
        const bt_ull_le_uuid_t *bt_ull_le_uuid = bt_ull_le_srv_get_uuid();
        memcpy(ull_uuid, bt_ull_le_uuid, BT_ULL_LE_MAX_UUID_LENGTH);
#endif

        adv_data->adv_data->data[len] = 2;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_FLAG;
#ifdef AIR_LE_AUDIO_DUALMODE_ENABLE
        /* if LE Audio supported for dualmode, only show BR/EDR not supported in partner */
        adv_data->adv_data->data[len + 2] = BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;
    #ifdef MTK_AWS_MCE_ENABLE
        /* BR/EDR is only supported on Agent */
        bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
        if (BT_AWS_MCE_ROLE_AGENT != role) {
            adv_data->adv_data->data[len + 2] |= BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED;
        }
    #endif
#else
        adv_data->adv_data->data[len + 2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;
#endif
        len += 3;

#ifdef AIR_LE_AUDIO_ENABLE
        uint8_t announcement_type = ANNOUNCEMENT_TYPE_GENERAL;
        if (app_lea_adv_mode != APP_LEA_ADV_MODE_GENERAL && app_lea_adv_target_addr_num > 0) {
            announcement_type = ANNOUNCEMENT_TYPE_TARGETED;
        }
        uint16_t sink_conent = 0;
        uint16_t source_conent = 0;
        ble_pacs_get_available_audio_contexts(&sink_conent, &source_conent);

        /* adv_data: RSI */
        adv_data->adv_data->data[len] = 7;
        adv_data->adv_data->data[len + 1] = 0x2E;
        ble_csis_get_rsi(rsi);
        memcpy(&adv_data->adv_data->data[len + 2], rsi, sizeof(rsi));
        len += 8;

        /* adv_data: AD_TYPE_SERVICE_DATA (BAP)*/
        adv_data->adv_data->data[len] = 9;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_SERVICE_DATA;
        adv_data->adv_data->data[len + 2] = (BT_GATT_UUID16_ASCS_SERVICE & 0x00FF);
        adv_data->adv_data->data[len + 3] = ((BT_GATT_UUID16_ASCS_SERVICE & 0xFF00) >> 8);
        adv_data->adv_data->data[len + 4] = announcement_type;
        memcpy(&adv_data->adv_data->data[len + 5], &sink_conent, 2);
        memcpy(&adv_data->adv_data->data[len + 7], &source_conent, 2);
        adv_data->adv_data->data[len + 9] = 0x00; /* Length of the Metadata field = 0 */
        len += 10;

        /* adv_data: TX_POWER (BAP)*/
        adv_data->adv_data->data[len] = 2;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_TX_POWER;
        adv_data->adv_data->data[len + 2] = 0x7F;
        len += 3;

#ifndef AIR_LE_AUDIO_DUALMODE_ENABLE
        /* adv_data: AD_TYPE_APPEARANCE (TMAP) */
        adv_data->adv_data->data[len] = 3;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_APPEARANCE;
        /* value: 2 bytes */
        adv_data->adv_data->data[len + 2] = 0x41;
        adv_data->adv_data->data[len + 3] = 0x09;
        len += 4;
#endif

        /* adv_data: AD_TYPE_SERVICE_DATA (BASS)*/
        adv_data->adv_data->data[len] = 3;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_SERVICE_DATA;
        /* BASS UUID: 2 bytes */
        adv_data->adv_data->data[len + 2] = (BT_SIG_UUID16_BASS & 0x00FF);
        adv_data->adv_data->data[len + 3] = ((BT_SIG_UUID16_BASS & 0xFF00) >> 8);
        len += 4;

        /* adv_data: AD_TYPE_SERVICE_DATA (CAS)*/
        adv_data->adv_data->data[len] = 4;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_SERVICE_DATA;
        /* CAS UUID: 2 bytes */
        adv_data->adv_data->data[len + 2] = (BT_SIG_UUID16_CAS & 0x00FF);
        adv_data->adv_data->data[len + 3] = ((BT_SIG_UUID16_CAS & 0xFF00) >> 8);
        adv_data->adv_data->data[len + 4] = ANNOUNCEMENT_TYPE_GENERAL;
        len += 5;

        /* Dual mode: Agent H_<local_addr>, Partner LEA-H_<local_addr>
         * Agent LEA-H_<local_addr>, Partner LEA-H_<local_addr>
         */
        uint16_t device_name_len = 0;
        char device_name[BT_GAP_LE_MAX_DEVICE_NAME_LENGTH] = {0};
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
#ifdef AIR_LE_AUDIO_DUALMODE_ENABLE
        if (BT_AWS_MCE_ROLE_AGENT == role) {
            snprintf(device_name, BT_GAP_LE_MAX_DEVICE_NAME_LENGTH, "H_%.2X%.2X%.2X%.2X%.2X%.2X",
                 (*local_addr)[5], (*local_addr)[4], (*local_addr)[3],
                 (*local_addr)[2], (*local_addr)[1], (*local_addr)[0]);
        } else {
            snprintf(device_name, BT_GAP_LE_MAX_DEVICE_NAME_LENGTH, "LEA-H_%.2X%.2X%.2X%.2X%.2X%.2X",
                 (*local_addr)[5], (*local_addr)[4], (*local_addr)[3],
                 (*local_addr)[2], (*local_addr)[1], (*local_addr)[0]);
        }
#else
        snprintf(device_name, BT_GAP_LE_MAX_DEVICE_NAME_LENGTH, "LEA-H_%.2X%.2X%.2X%.2X%.2X%.2X",
             (*local_addr)[5], (*local_addr)[4], (*local_addr)[3],
             (*local_addr)[2], (*local_addr)[1], (*local_addr)[0]);
#endif
        device_name_len = strlen((char *)device_name);
        /* scan_rsp: AD_TYPE_NAME_COMPLETE*/
        adv_data->adv_data->data[len] = device_name_len + 1;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
        memcpy(&adv_data->adv_data->data[len + 2], device_name, device_name_len);
        /*set GASTT GAP service device name*/
        bt_gatts_service_set_le_audio_device_name((const uint8_t *)device_name, device_name_len);
        len += 2 + device_name_len;
#endif /* AIR_LE_AUDIO_ENABLE */

#ifdef AIR_BLE_ULTRA_LOW_LATENCY_ENABLE
#ifdef AIR_LE_AUDIO_ENABLE        // LE AUDIO + LE ULL co-exist
        if (app_lea_adv_add_ull_data) {
            adv_data->adv_data->data[len] = BT_ULL_LE_MAX_UUID_LENGTH + 1;
            adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_128_BIT_UUID_DATA;
            memcpy(&adv_data->adv_data->data[len + 2], ull_uuid, sizeof(ull_uuid));
            len += BT_ULL_LE_MAX_UUID_LENGTH + 2;
        }
#else                             // Only LE ULL
        /* adv_data: RSI */
        adv_data->adv_data->data[len] = APP_LE_AUDIO_RSI_LENGTH + 1;
        adv_data->adv_data->data[len + 1] = 0x2E;
        bt_ull_le_srv_get_rsi(rsi);
        memcpy(&adv_data->adv_data->data[len + 2], rsi, sizeof(rsi));
        len += APP_LE_AUDIO_RSI_LENGTH + 2;

        /* service data */
        adv_data->adv_data->data[len] = BT_ULL_LE_MAX_UUID_LENGTH + 1;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_128_BIT_UUID_DATA;
        memcpy(&adv_data->adv_data->data[len + 2], ull_uuid, sizeof(ull_uuid));
        len += BT_ULL_LE_MAX_UUID_LENGTH + 2;
#endif
#endif

        adv_data->adv_data->data_length = len;
    }

    if (NULL != adv_data->adv_param) {
        adv_data->adv_param->advertising_event_properties = BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE;
        /* Interval should be no larger than 100ms when discoverable */
        adv_data->adv_param->primary_advertising_interval_min = app_lea_adv_interval_min;
        adv_data->adv_param->primary_advertising_interval_max = app_lea_adv_interval_max;
        adv_data->adv_param->primary_advertising_channel_map = 0x07;
#ifdef AIR_LE_AUDIO_DUALMODE_ENABLE
        adv_data->adv_param->own_address_type = BT_ADDR_LE_PUBLIC;
#else
        adv_data->adv_param->own_address_type = BT_ADDR_RANDOM;
#endif
        adv_data->adv_param->advertising_filter_policy = 0;
        adv_data->adv_param->advertising_tx_power = 0x7F;
        adv_data->adv_param->primary_advertising_phy = BT_HCI_LE_ADV_PHY_1M;
        adv_data->adv_param->secondary_advertising_phy = BT_HCI_LE_ADV_PHY_1M;
    }
    return 0;
}

static uint32_t app_lea_adv_mgr_get_adv_data(multi_ble_adv_info_t *adv_data)
{
    uint32_t ret = app_lea_adv_mgr_get_adv_data_internal(adv_data);

    if (app_lea_adv_mode == APP_LEA_ADV_MODE_DIRECT && adv_data->adv_param != NULL) {
#ifdef AIR_LE_AUDIO_DIRECT_ADV
        if (app_lea_adv_target_addr_num != 1) {
            APPS_LOG_MSGID_E(LOG_TAG" get_adv_data, error DIRECT mode but target_num=%d", 1, app_lea_adv_target_addr_num);
        } else {
            uint8_t direct_adv_addr_type = app_lea_adv_mgr_convert_addr_type(app_lea_adv_target_addr[0].type);
            adv_data->adv_param->advertising_event_properties |= BT_HCI_ADV_EVT_PROPERTIES_MASK_DIRECTED;
            adv_data->adv_param->advertising_event_properties &= (~BT_HCI_ADV_EVT_PROPERTIES_MASK_SCANNABLE);
            adv_data->adv_param->peer_address.type = direct_adv_addr_type;
            memcpy(adv_data->adv_param->peer_address.addr, app_lea_adv_target_addr[0].addr, sizeof(bt_bd_addr_t));
            adv_data->adv_param->own_address_type = BT_ADDR_RANDOM_IDENTITY;

            if (adv_data->adv_data != NULL) {
                adv_data->adv_data->data_length = 0;
            }
            if (adv_data->scan_rsp != NULL) {
                adv_data->scan_rsp->data_length = 0;
            }
        }
#endif
    } else if ((app_lea_adv_mode == APP_LEA_ADV_MODE_TARGET
            || app_lea_adv_mode == APP_LEA_ADV_MODE_TARGET_ALL) && adv_data->adv_param != NULL) {
#ifdef AIR_LE_AUDIO_WHITELIST_ADV
        adv_data->adv_param->advertising_filter_policy = BT_HCI_ADV_FILTER_ACCEPT_SCAN_CONNECT_IN_WHITE_LIST;
#endif
    }
    return ret;
}

static void app_lea_adv_mgr_update_multi_adv(void)
{
    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_lea_adv_mgr_get_adv_data);
    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_lea_adv_mgr_get_adv_data, 1);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_NOT_RHO);
}

static void app_lea_adv_mgr_do_start_advertising(uint8_t mode, uint32_t timeout)
{
    bool visible = app_bt_service_is_visible();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    APPS_LOG_MSGID_I(LOG_TAG" do_start_advertising, [%02X] start adv_mode=%d->%d timeout=%d visible=%d",
                     5, role, app_lea_adv_mode, mode, timeout, visible);

#if (APP_LE_AUDIO_ADV_CONN_MODE == APP_LE_AUDIO_MODE_DEFAULT)
#if defined(AIR_LE_AUDIO_DUALMODE_ENABLE) || defined(AIR_WIRELESS_MIC_ENABLE)
    if (mode == APP_LEA_ADV_MODE_TARGET_ALL) {
        mode = APP_LEA_ADV_MODE_GENERAL;
        timeout = 0;
        APPS_LOG_MSGID_W(LOG_TAG" do_start_advertising, [%02X] Target_All -> General", 1, role);
    }
#endif

    if (app_lea_adv_mode == APP_LEA_ADV_MODE_GENERAL && visible
        && mode != APP_LEA_ADV_MODE_TARGET && mode != APP_LEA_ADV_MODE_DIRECT) {
        if (mode == APP_LEA_ADV_MODE_GENERAL && timeout == APP_LE_AUDIO_ADV_TIME
            && app_lea_adv_timeout == APP_LE_AUDIO_SWITCH_LEA_ADV_TIME) {
            app_lea_adv_timeout = timeout;
            uint8_t adv_timeout_action = APP_LEA_ADV_TIMEOUT_ACTION_STOP;
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER);
            if (app_lea_adv_timeout > 0) {
                ui_shell_send_event(FALSE, EVENT_PRIORITY_MIDDLE,
                                    EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER,
                                    (void *)(int)adv_timeout_action, 0, NULL, app_lea_adv_timeout);
            }
            APPS_LOG_MSGID_W(LOG_TAG" start_advertising, switch mode fail from GENERAL Mode timeout=%d",
                             1, timeout);
        } else {
            APPS_LOG_MSGID_W(LOG_TAG" start_advertising, switch mode fail from GENERAL Mode", 0);
        }
        return;
    }
#endif
#ifdef APP_CONN_MGR_RECONNECT_CONTROL
    bool reconnecting = app_bt_conn_mgr_is_reconnecting();
    if (app_lea_adv_mode == APP_LEA_ADV_MODE_TARGET && reconnecting
        && mode != APP_LEA_ADV_MODE_TARGET) {
        APPS_LOG_MSGID_W(LOG_TAG" start_advertising, switch mode fail from TARGET Mode", 0);
        return;
    }
#endif

    app_lea_adv_error_code_t error_code = APP_LEA_ADV_ERROR_OK;
    uint8_t adv_timeout_action = APP_LEA_ADV_TIMEOUT_ACTION_STOP;

    if (mode == APP_LEA_ADV_MODE_GENERAL) {
        app_lea_clear_target_addr();
        // General mode will switch via listen EDR visible change
        if (timeout == APP_LE_AUDIO_SWITCH_LEA_ADV_TIME) {
            adv_timeout_action = APP_LEA_ADV_TIMEOUT_ACTION_START;
        }
    } else if (mode == APP_LEA_ADV_MODE_TARGET) {
        // app_bt_conn_mgr_reconnect_complete->start->remove EVENT_ID_LE_AUDIO_ADV_TIMER when TARGET Mode (Power on reconnect) complete or timeout
        adv_timeout_action = APP_LEA_ADV_TIMEOUT_ACTION_START;
    } else if (mode == APP_LEA_ADV_MODE_TARGET_ALL) {
        app_lea_clear_target_addr();
        uint8_t empty_addr[BT_BD_ADDR_LEN] = {0};
        app_lea_bond_info_t *bond_info = app_lea_conn_mgr_get_bond_info();
        for (int i = 0; i < APP_LEA_MAX_TARGET_NUM; i++) {
            uint8_t addr_type = bond_info[i].addr_type;
            const uint8_t *addr = bond_info[i].bond_addr;
            if (bond_info[i].used && !APP_LEA_IS_EMPTY_ADDR(addr, empty_addr)) {
                bool is_connected = app_lea_conn_mgr_is_connected(addr);
                APPS_LOG_MSGID_I(LOG_TAG" do_start_advertising, bond_info[%d] type=%d addr=%02X:%02X:%02X:%02X:%02X:XX is_connected=%d",
                                 8, i, bond_info[i].addr_type, addr[5], addr[4], addr[3], addr[2], addr[1], is_connected);
                if (!is_connected) {
                    app_lea_update_target_addr(APP_LEA_TARGET_ADD_ADDR, addr_type, addr);
                }
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" do_start_advertising, bond_info[%d] empty", 1, i);
            }
        }

        if (app_lea_adv_target_addr_num == 0) {
            APPS_LOG_MSGID_E(LOG_TAG" do_start_advertising, bond_info is empty", 0);
            app_lea_adv_mgr_do_stop_advertising();
            return;
        }
    }

    uint8_t cur_conn_num = app_lea_conn_mgr_get_conn_num();
    uint8_t support_max_conn_num = app_lea_conn_mgr_get_support_max_conn_num();
    if (apps_config_features_is_mp_test_mode()) {
        error_code = APP_LEA_ADV_ERROR_MP_TEST_MODE;
    } else if (cur_conn_num == APP_LEA_MAX_CONN_NUM) {
        error_code = APP_LEA_ADV_ERROR_MAX_LINK;
    } else if (cur_conn_num == support_max_conn_num) {
        error_code = APP_LEA_ADV_ERROR_MAX_SUPPORT_LINK;
    } else if (mode == APP_LEA_ADV_MODE_NONE) {
        error_code = APP_LEA_ADV_ERROR_ADV_MODE;
    } else if (mode > APP_LEA_ADV_MODE_GENERAL && app_lea_adv_target_addr_num == 0) {
        error_code = APP_LEA_ADV_ERROR_NO_TARGET_ADDR;
    } else if (!app_bt_conn_manager_allow_le_adv()) {
        error_code = APP_LEA_ADV_ERROR_APP_CONN_DISALLOW;
    }
#ifndef MTK_AWS_MCE_ENABLE  // Headset project
    extern bool app_le_audio_is_wired_audio(void);
    if (app_le_audio_is_wired_audio()) {
        error_code = APP_LEA_ADV_ERROR_WIRED_AUDIO;
    }
#endif
#ifdef AIR_WIRELESS_MIC_ENABLE  // Headset project
    extern bool app_le_ull_adv_disable_by_cmd(void);
    if (app_le_ull_adv_disable_by_cmd()) {
        error_code = APP_LEA_ADV_ERROR_WIRELESS_MIC_DISALLOW;
        APPS_LOG_MSGID_I(LOG_TAG" APP_LEA_ADV_ERROR_WIRELESS_MIC_DISALLOW", 0);
    }
#endif
    if (error_code != APP_LEA_ADV_ERROR_OK) {
        APPS_LOG_MSGID_E(LOG_TAG" do_start_advertising, error_code=%d conn_num=%d %d %d",
                         4, error_code, cur_conn_num, APP_LEA_MAX_CONN_NUM, support_max_conn_num);
        return;
    }

    app_lea_adv_mode = mode;

    /* LE-Audio ADV is long interval when the AWS link is disconnected. */
    if (bt_sink_srv_get_state() >= BT_SINK_SRV_STATE_STREAMING
        || app_lea_adv_mgr_is_esco_ongoing()
        || bt_cm_get_connecting_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0) > 0
#ifdef MTK_AWS_MCE_ENABLE
        || BT_AWS_MCE_SRV_LINK_NONE == bt_aws_mce_srv_get_link_type()
#endif
       ) {
        app_lea_adv_interval_min = APP_LE_AUDIO_ADV_INTERVAL_MIN_L;
        app_lea_adv_interval_max = APP_LE_AUDIO_ADV_INTERVAL_MAX_L;
    } else {
        app_lea_adv_interval_min = APP_LE_AUDIO_ADV_INTERVAL_MIN_S;
        app_lea_adv_interval_max = APP_LE_AUDIO_ADV_INTERVAL_MAX_S;
    }

    app_lea_adv_mgr_update_multi_adv();

    if (app_lea_adv_mode == APP_LEA_ADV_MODE_TARGET_ALL || app_lea_adv_mode == APP_LEA_ADV_MODE_DIRECT) {
        app_lea_adv_timeout = 0;        // Must always start ADV for target_all and direct mode
    } else {
        app_lea_adv_timeout = timeout;
    }

    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER);
    if (app_lea_adv_timeout > 0) {
        ui_shell_send_event(FALSE, EVENT_PRIORITY_MIDDLE,
                            EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER,
                            (void *)(int)adv_timeout_action, 0, NULL, app_lea_adv_timeout);
    }

    app_lea_adv_enabled = TRUE;
    APPS_LOG_MSGID_I(LOG_TAG" do_start_advertising, success adv_mode=%d timeout=%d interval=0x%04X 0x%04X",
                     4, app_lea_adv_mode, app_lea_adv_timeout,
                     app_lea_adv_interval_min, app_lea_adv_interval_max);

#if defined(MTK_AWS_MCE_ENABLE) && defined(AIR_LE_AUDIO_BOTH_SYNC_INFO)
    app_lea_sync_info_send();
#endif
}

static void app_lea_adv_mgr_do_stop_advertising(void)
{
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER);

    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_lea_adv_mgr_get_adv_data);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_NOT_RHO);

    app_lea_adv_enabled = FALSE;
    app_lea_adv_timeout = 0;
    app_lea_adv_mode = APP_LEA_ADV_MODE_NONE;

    app_lea_clear_target_addr();
    APPS_LOG_MSGID_I(LOG_TAG" do_stop_advertising", 0);

#if defined(MTK_AWS_MCE_ENABLE) && defined(AIR_LE_AUDIO_BOTH_SYNC_INFO)
    app_lea_sync_info_send();
#endif
}

static void app_lea_adv_mgr_do_update_advertising_param(void)
{
    app_lea_adv_error_code_t error_code = APP_LEA_ADV_ERROR_OK;
    uint8_t cur_conn_num = app_lea_conn_mgr_get_conn_num();
    uint8_t support_max_conn_num = app_lea_conn_mgr_get_support_max_conn_num();

    if (apps_config_features_is_mp_test_mode()) {
        error_code = APP_LEA_ADV_ERROR_MP_TEST_MODE;
    } else if (!app_lea_adv_enabled) {
        error_code = APP_LEA_ADV_ERROR_UPDATE_STOP;
    } else if (cur_conn_num == APP_LEA_MAX_CONN_NUM) {
        error_code = APP_LEA_ADV_ERROR_MAX_LINK;
    } else if (cur_conn_num == support_max_conn_num) {
        error_code = APP_LEA_ADV_ERROR_MAX_SUPPORT_LINK;
    } else if (app_lea_adv_mode == APP_LEA_ADV_MODE_NONE) {
        error_code = APP_LEA_ADV_ERROR_ADV_MODE;
    } else if (app_lea_adv_mode > APP_LEA_ADV_MODE_DIRECT && app_lea_adv_target_addr_num == 0) {
        error_code = APP_LEA_ADV_ERROR_NO_TARGET_ADDR;
    } else if (!app_bt_conn_manager_allow_le_adv()) {
        error_code = APP_LEA_ADV_ERROR_APP_CONN_DISALLOW;
    }

    if (error_code != APP_LEA_ADV_ERROR_OK) {
        APPS_LOG_MSGID_E(LOG_TAG" update_advertising_param, error_code=%d conn_num=%d %d %d",
                         4, error_code, cur_conn_num, APP_LEA_MAX_CONN_NUM, support_max_conn_num);
        return;
    }

    app_lea_adv_mgr_update_multi_adv();

    app_lea_adv_enabled = TRUE;

    APPS_LOG_MSGID_I(LOG_TAG" update_advertising_param, adv_mode=%d timeout=%d interval=0x%04X 0x%04X",
                     4, app_lea_adv_mode, app_lea_adv_timeout,
                     app_lea_adv_interval_min, app_lea_adv_interval_max);
}



/**================================================================================*/
/**                                 APP Event Handler                              */
/**================================================================================*/
static void app_lea_adv_mgr_interaction_event_group(uint32_t event_id, void *extra_data, size_t data_len)
{
#if (APP_LE_AUDIO_ADV_CONN_MODE == APP_LE_AUDIO_MODE_DEFAULT)
    if (event_id == APPS_EVENTS_INTERACTION_BT_VISIBLE_NOTIFY) {
        bool visible = (bool)extra_data;
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

        APPS_LOG_MSGID_I(LOG_TAG" BT_VISIBLE_NOTIFY event, [%02X] visible=%d adv_mode=%d",
                         3, role, visible, app_lea_adv_mode);
        if (visible) {
            if (app_lea_adv_mode == APP_LEA_ADV_MODE_TARGET) {
                // Do nothing, wait target_mode (power_on reconnect) finish or timeout
            } else {
                app_lea_adv_mgr_do_start_advertising(APP_LEA_ADV_MODE_GENERAL, APP_LE_AUDIO_ADV_TIME);
            }
        } else if (app_lea_adv_mode == APP_LEA_ADV_MODE_GENERAL) {
            app_lea_adv_mgr_do_start_advertising(APP_LEA_ADV_MODE_TARGET_ALL, 0);
        }
    }
#endif
}

static void app_lea_adv_mgr_bt_sink_event_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
            if (param == NULL) {
                break;
            }

            if (param->previous != BT_SINK_SRV_STATE_STREAMING && param->current == BT_SINK_SRV_STATE_STREAMING) {
                APPS_LOG_MSGID_I(LOG_TAG" Music Start", 0);
                app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_L, APP_LE_AUDIO_ADV_INTERVAL_MAX_L);
            } else if (param->previous == BT_SINK_SRV_STATE_STREAMING && param->current != BT_SINK_SRV_STATE_STREAMING) {
                APPS_LOG_MSGID_I(LOG_TAG" Music Stop", 0);
                app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_S, APP_LE_AUDIO_ADV_INTERVAL_MAX_S);
            }
            break;
        }

        case BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE: {
            bt_sink_srv_sco_state_update_t *esco_state = (bt_sink_srv_sco_state_update_t *)extra_data;
            if (esco_state == NULL) {
                break;
            }

            APPS_LOG_MSGID_I(LOG_TAG" eSCO state = %d", 1, esco_state->state);
            if (esco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED) {
                app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_L, APP_LE_AUDIO_ADV_INTERVAL_MAX_L);
            } else if (esco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED) {
                app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_S, APP_LE_AUDIO_ADV_INTERVAL_MAX_S);;
            }
            break;
        }
        default:
            break;
    }
}

static void app_lea_adv_mgr_bt_dm_event_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_device_manager_power_event_t evt;
    bt_device_manager_power_status_t status;
    bt_event_get_bt_dm_event_and_status(event_id, &evt, &status);
    switch (evt) {
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_RESET_TYPE_NORMAL == status) {
                //APPS_LOG_MSGID_I(LOG_TAG" BT_DM POWER OFF", 0);
                app_lea_clear_target_addr();
            }
            break;
        }
    }
}

static void app_lea_adv_mgr_lea_event_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_LE_AUDIO_ADV_TIMER: {
            uint8_t action = (uint8_t)(int)extra_data;
            bool visible = app_bt_service_is_visible();
            APPS_LOG_MSGID_I(LOG_TAG" LE Audio event, ADV timeout action=%d visible=%d",
                             2, action, visible);
            if (action == APP_LEA_ADV_TIMEOUT_ACTION_START) {
                if (visible) {
                    app_lea_adv_mgr_do_start_advertising(APP_LEA_ADV_MODE_GENERAL, APP_LE_AUDIO_ADV_TIME);
                } else {
                    app_lea_adv_mgr_do_start_advertising(APP_LEA_ADV_MODE_TARGET_ALL, 0);
                }
            } else {
                app_lea_adv_mgr_do_stop_advertising();
            }
            break;
        }
        case EVENT_ID_LE_AUDIO_START_ADV: {
            app_lea_adv_param_t *adv_param = (app_lea_adv_param_t *)extra_data;
            app_lea_adv_mgr_do_start_advertising(adv_param->mode, adv_param->timeout);
            break;
        }
        case EVENT_ID_LE_AUDIO_STOP_ADV: {
            app_lea_adv_mgr_do_stop_advertising();
            break;
        }
        case EVENT_ID_LE_AUDIO_RESTART_ADV: {
            // For low power mode testing
            APPS_LOG_MSGID_E(LOG_TAG" LE Audio event, restart ADV for testing on Low power mode", 0);
            app_lea_adv_mgr_do_start_advertising(APP_LEA_ADV_MODE_GENERAL, 0);
            multi_ble_adv_manager_start_ble_adv();
            break;
        }
#if defined(AIR_LE_AUDIO_ENABLE) && defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
        case EVENT_ID_LE_AUDIO_CONTROL_ADV_DATA: {
            uint32_t adv_param = (uint32_t)extra_data;
            uint8_t adv_type = (uint8_t)((adv_param & 0xFF00) >> 8);
            uint8_t enable = (uint8_t)(adv_param & 0xFF);
            APPS_LOG_MSGID_I(LOG_TAG" control_adv_data, adv_type=%d enable=%d app_lea_adv_enabled=%d",
                             3, adv_type, enable, app_lea_adv_enabled);

            if (adv_type == APP_LEA_SERVICE_ADV_DATA_ULL) {
                // If already advertising, will add/remove "LEA/ULL ADV Data" via update ADV parameter
                // If not advertising, only disable ADV data, user need to call <app_lea_service_start_advertising> to restart ADV
                if (enable) {
                    app_lea_adv_add_ull_data = TRUE;
                    app_lea_adv_mgr_do_update_advertising_param();
                } else {
                    app_lea_adv_add_ull_data = FALSE;
                    app_lea_adv_mgr_do_update_advertising_param();
                }
            }
            break;
        }
#endif
    }
}

#ifdef MTK_AWS_MCE_ENABLE
static void app_lea_adv_mgr_proc_aws_data(void *extra_data, size_t data_len)
{
    uint32_t aws_event_group;
    uint32_t aws_event_id;
    void *p_extra_data = NULL;
    uint32_t extra_data_len = 0;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind == NULL || aws_data_ind->module_id != BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        return;
    }

    apps_aws_sync_event_decode_extra(aws_data_ind, &aws_event_group, &aws_event_id,
                                     &p_extra_data, &extra_data_len);
    if (aws_event_group == EVENT_GROUP_UI_SHELL_LE_AUDIO) {
        APPS_LOG_MSGID_I(LOG_TAG" AWS_DATA event, [%02X] event_id=%d", 2, role, aws_event_id);
        if (aws_event_id == EVENT_ID_LE_AUDIO_START_ADV) {
            app_lea_adv_param_t *adv_param = (app_lea_adv_param_t *)p_extra_data;
            app_lea_adv_mgr_do_start_advertising(adv_param->mode, adv_param->timeout);
        } else if (aws_event_id == EVENT_ID_LE_AUDIO_STOP_ADV) {
            app_lea_adv_mgr_do_stop_advertising();
        }
    }
}
#endif

static void app_lea_adv_mgr_proc_bt_cm_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == remote_update) {
                break;
            }

#ifdef MTK_AWS_MCE_ENABLE
            if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service) &&
                (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                /* LE-Audio ADV resume to the short interval when the AWS link connected. */
                app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_S, APP_LE_AUDIO_ADV_INTERVAL_MAX_S);
            } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                       && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                /* LE-Audio ADV resume to the long interval when the AWS link disconnected. */
                app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_L, APP_LE_AUDIO_ADV_INTERVAL_MAX_L);
            }
#endif

            if (BT_CM_ACL_LINK_CONNECTED > remote_update->pre_acl_state
                && BT_CM_ACL_LINK_CONNECTED <= remote_update->acl_state
                && 0 != memcmp(remote_update->address, bt_device_manager_get_local_address(), sizeof(bt_bd_addr_t))) {
#if (APP_LE_AUDIO_ADV_CONN_MODE == APP_LE_AUDIO_MODE_DEFAULT)
                /* LE-Audio ADV resume to the short interval when the EDR link connected. */
                app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_S, APP_LE_AUDIO_ADV_INTERVAL_MAX_S);
#endif
            }
            break;
        }

        default:
            break;
    }
}

static bool app_lea_adv_mgr_proc_ble_ull_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
#if defined(AIR_LE_AUDIO_ENABLE) && defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
        // Need modify if-option if ULL2 could support multi-link independently
        case BT_ULL_EVENT_LE_STREAMING_START_IND: {
            APPS_LOG_MSGID_I(LOG_TAG" BLE ULL2 Start", 0);
            app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_L, APP_LE_AUDIO_ADV_INTERVAL_MAX_L);
            break;
        }
        case BT_ULL_EVENT_LE_STREAMING_STOP_IND: {
            APPS_LOG_MSGID_I(LOG_TAG" BLE ULL2 Stop", 0);
            app_lea_adv_mgr_update_adv_interval(APP_LE_AUDIO_ADV_INTERVAL_MIN_S, APP_LE_AUDIO_ADV_INTERVAL_MAX_S);
            break;
        }
#endif
        default:
            break;
    }
    return FALSE;
}



/**================================================================================*/
/**                                     Public API                                 */
/**================================================================================*/
void app_lea_adv_mgr_start_advertising(uint8_t mode, bool sync, uint32_t timeout)
{
    APPS_LOG_MSGID_I(LOG_TAG" start_advertising, mode=%d sync=%d timeout=%d", 3, mode, sync, timeout);

    app_lea_adv_param_t *adv_param = (app_lea_adv_param_t *)pvPortMalloc(sizeof(app_lea_adv_param_t));
    if (adv_param == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG" start_advertising, malloc error", 0);
        return;
    } else if (mode == APP_LEA_ADV_MODE_NONE || mode == APP_LEA_ADV_MODE_MIX) {
        // Reserved for future
        if (adv_param != NULL) {
            vPortFree(adv_param);
        }
        return;
    }
#ifndef AIR_LE_AUDIO_DIRECT_ADV
    if (mode == APP_LEA_ADV_MODE_DIRECT) {
        if (adv_param != NULL) {
            vPortFree(adv_param);
        }
        APPS_LOG_MSGID_E(LOG_TAG" start_advertising, disable DIRECT ADV mode", 0);
        return;
    }
#endif

    adv_param->mode = mode;
    adv_param->timeout = timeout;
    ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                        EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_START_ADV,
                        (void *)adv_param, sizeof(app_lea_adv_param_t), NULL, 0);

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_srv_link_type_t aws_link_type = bt_aws_mce_srv_get_link_type();
    if (sync && aws_link_type != BT_AWS_MCE_SRV_LINK_NONE) {
        bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                                               EVENT_ID_LE_AUDIO_START_ADV,
                                                               adv_param,
                                                               sizeof(app_lea_adv_param_t));
        if (bt_status != BT_STATUS_SUCCESS) {
            APPS_LOG_MSGID_E(LOG_TAG" start_advertising, error AWS bt_status=0x%08X", 1, bt_status);
        }
    }
#endif
}

void app_lea_adv_mgr_stop_advertising(bool sync)
{
    APPS_LOG_MSGID_I(LOG_TAG" stop_advertising, sync=%d", 1, sync);
    ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                        EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_STOP_ADV,
                        NULL, 0, NULL, 0);

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_srv_link_type_t aws_link_type = bt_aws_mce_srv_get_link_type();
    if (sync && aws_link_type != BT_AWS_MCE_SRV_LINK_NONE) {
        bt_status_t bt_status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                                         EVENT_ID_LE_AUDIO_STOP_ADV);
        if (bt_status != BT_STATUS_SUCCESS) {
            APPS_LOG_MSGID_E(LOG_TAG" stop_advertising, error AWS bt_status=0x%08X", 1, bt_status);
        }
    }
#endif
}

bool app_lea_adv_mgr_update_adv_interval(uint16_t interval_min, uint16_t interval_max)
{
    uint8_t sink_state = bt_sink_srv_get_state();
    bool esco_ongoing = app_lea_adv_mgr_is_esco_ongoing();
    uint32_t connecting_num = bt_cm_get_connecting_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0);
    bool turn_down = (interval_min < app_lea_adv_interval_min && interval_max < app_lea_adv_interval_max);
    APPS_LOG_MSGID_I(LOG_TAG" update_adv_interval, adv_enabled=%d sink_state=%d esco_ongoing=%d connecting_num=%d min=0x%04X->0x%04X max=0x%04X->0x%04X",
                     8, app_lea_adv_enabled, sink_state, esco_ongoing, connecting_num,
                     app_lea_adv_interval_min, interval_min,
                     app_lea_adv_interval_max, interval_max);

    if (!app_lea_adv_enabled) {
        //APPS_LOG_MSGID_E(LOG_TAG" update_adv_interval, not advertising", 0);
        return FALSE;
    } else if (app_lea_adv_interval_min == interval_min && app_lea_adv_interval_max == interval_max) {
        //APPS_LOG_MSGID_E(LOG_TAG" update_adv_interval, same parameter", 0);
        return FALSE;
    } else if ((interval_min < app_lea_adv_interval_min && interval_max > app_lea_adv_interval_max)
               || (interval_min > app_lea_adv_interval_min && interval_max < app_lea_adv_interval_max)) {
        //APPS_LOG_MSGID_E(LOG_TAG" update_adv_interval, not invalid parameter", 0);
        return FALSE;
    } else if (turn_down) {
        if (sink_state >= BT_SINK_SRV_STATE_STREAMING || esco_ongoing) {
            //APPS_LOG_MSGID_E(LOG_TAG" update_adv_interval, not turn_down when Audio ongoing", 0);
            return FALSE;
        }
#ifdef MTK_AWS_MCE_ENABLE
        if (BT_AWS_MCE_SRV_LINK_NONE == bt_aws_mce_srv_get_link_type()) {
            //APPS_LOG_MSGID_E(LOG_TAG" update_adv_interval, not turn_down when AWS not connected", 0);
            return FALSE;
        }
#endif
        if (connecting_num > 0) {
            //APPS_LOG_MSGID_E(LOG_TAG" update_adv_interval, not turn_down when BT EDR connecting", 0);
            return FALSE;
        }
    }

    app_lea_adv_interval_min = interval_min;
    app_lea_adv_interval_max = interval_max;
    app_lea_adv_mgr_do_update_advertising_param();
//    APPS_LOG_MSGID_I(LOG_TAG" update_adv_interval, success min=%d max=%d",
//                     2, app_lea_adv_interval_min, app_lea_adv_interval_max);
    return TRUE;
}

bool app_lea_adv_mgr_control_adv_data(uint8_t adv_type, bool enable)
{
#if defined(AIR_LE_AUDIO_ENABLE) && defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
    if (adv_type == APP_LEA_SERVICE_ADV_DATA_LEA || adv_type > APP_LEA_SERVICE_ADV_DATA_ULL) {
        // ToDo, LEA ADV Data add/remove
        return FALSE;
    }

    uint32_t adv_param = ((adv_type << 8) | enable);
    ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                        EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_CONTROL_ADV_DATA,
                        (void *)adv_param, 0, NULL, 0);
#endif
    return TRUE;
}

void app_lea_adv_mgr_get_adv_info(uint8_t *mode, bt_addr_t *target_addr, uint32_t *timeout)
{
    if (mode != NULL) {
        *mode = app_lea_adv_mode;
    }
    if (target_addr != NULL) {
        memcpy(target_addr, &app_lea_adv_target_addr[0], sizeof(bt_addr_t) * APP_LEA_MAX_TARGET_NUM);
    }
    if (timeout != NULL) {
        *timeout = app_lea_adv_timeout;
    }
}

bool app_lea_adv_mgr_update_target_addr(app_le_audio_update_target_mode_t mode,
                                        bt_addr_type_t addr_type,
                                        const uint8_t *addr)
{
    return app_lea_update_target_addr(mode, addr_type, addr);
}

void app_lea_adv_mgr_init(void)
{
#if defined(AIR_LE_AUDIO_ENABLE) && defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
    app_lea_adv_add_ull_data = TRUE;
#endif
}

void app_lea_adv_mgr_proc_ui_shell_event(uint32_t event_group,
                                         uint32_t event_id,
                                         void *extra_data,
                                         size_t data_len)
{
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            app_lea_adv_mgr_interaction_event_group(event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_SINK:
            app_lea_adv_mgr_bt_sink_event_group(event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER:
            app_lea_adv_mgr_bt_dm_event_group(event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_LE_AUDIO:
            app_lea_adv_mgr_lea_event_group(event_id, extra_data, data_len);
            break;
#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            app_lea_adv_mgr_proc_aws_data(extra_data, data_len);
            break;
#endif
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            app_lea_adv_mgr_proc_bt_cm_group(event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_BT_ULTRA_LOW_LATENCY:
            app_lea_adv_mgr_proc_ble_ull_group(event_id, extra_data, data_len);
            break;
    }
}

#endif  /* AIR_LE_AUDIO_ENABLE */

