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


#include "race_cmd_feature.h"
#if defined (AIR_LE_AUDIO_ENABLE) && defined (MTK_RACE_CMD_ENABLE)
#include "FreeRTOS.h"
#include "task.h"
#include "syslog.h"
#include "hal.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_lpcomm_aws.h"
#include "race_cmd_bluetooth.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_conn.h"
#include "race_lpcomm_msg_struct.h"
#include "race_bt.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"
#include "race_fota_util.h"
#include "race_fota.h"
#include "race_cmd_fcd_cmd.h"
#include "race_cmd_le_audio.h"
#if defined(AIR_LE_AUDIO_BIS_ENABLE) && !defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
#include "bt_sink_srv.h"
#include "bt_hci.h"
#include "bt_gap_le.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_le.h"
#include "bt_device_manager_le_config.h"
#include "bt_device_manager_config.h"
#include "bt_device_manager.h"
#include "bt_device_manager_power.h"
#include "bt_connection_manager_internal.h"
#include "bt_hci.h"
#include "bt_connection_manager.h"
#include "ble_bass.h"
#include "ble_bap.h"
////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern void ble_bap_config_adv_report(bt_addr_t addr, ble_bap_broadcast_audio_announcements_ind_t *ea_data, ble_bap_basic_audio_announcements_ind_t *pa_data);
//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
uint8_t g_leaudio_channel_id;

static bt_addr_t g_leaudio_current_bms_addr = {0};
static bt_addr_t g_leaudio_retry_bms_addr = {0};

static uint8_t g_leaudio_pa_sync_handle = 0;
static bool g_leaudio_flag_retry_scan = false;

static bt_sink_srv_cap_event_base_broadcast_audio_announcements_t curr_ea_data = {0};
static bt_sink_srv_cap_event_base_basic_audio_announcements_t curr_pa_data = {0};

//////////////////////////////////////////////////////////////////////////////
// STATIC FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
static bt_status_t race_le_audio_scan(uint8_t scan_mode)
{
    bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
    scan_param.duration = (RACE_LE_AUDIO_SCAN_MODE_ALL == scan_mode ? 0 : DEFAULT_SCAN_TIMEOUT);
    scan_param.audio_channel_allocation = ble_pacs_get_audio_location(AUDIO_DIRECTION_SINK);
    scan_param.bms_address = NULL;
    scan_param.duration = DEFAULT_SCAN_TIMEOUT;
    scan_param.sync_policy = BT_SINK_SRV_CAP_STREAM_SYNC_POLICY_SELECT_BIS;
    ble_bap_config_bis_sync_state(BT_BASS_BIS_SYNC_NO_PREFERENCE);
    return bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
}

static bool race_le_audio_play_bis(uint8_t subgroup_idx)
{
    bt_sink_srv_cap_stream_bmr_scan_info_t *scan_info = bt_sink_srv_cap_stream_get_bmr_scan_info();
    bt_le_audio_location_t audio_location = ble_pacs_get_audio_location(AUDIO_DIRECTION_SINK);
    bool status = false;

    if (le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET) {
        uint8_t bis_indices[2] = {BIS_INDEX_INVALID, BIS_INDEX_INVALID};
        /*Checking 1 stereo BIS*/
        audio_location = AUDIO_LOCATION_FRONT_LEFT | AUDIO_LOCATION_FRONT_RIGHT;
        bis_indices[0] = bt_sink_srv_cap_stream_get_bis_index_by_audio_location(audio_location, subgroup_idx);
        if (BIS_INDEX_INVALID == bis_indices[0]) {
            /*Checking 2 mono BIS*/
            bis_indices[0] = bt_sink_srv_cap_stream_get_bis_index_by_audio_location(AUDIO_LOCATION_FRONT_LEFT, subgroup_idx);
            bis_indices[1] = bt_sink_srv_cap_stream_get_bis_index_by_audio_location(AUDIO_LOCATION_FRONT_RIGHT, subgroup_idx);
            if (BIS_INDEX_INVALID != bis_indices[0] && BIS_INDEX_INVALID != bis_indices[1]) {
                status = bt_sink_srv_cap_stream_start_broadcast_reception(scan_info->sync_handle, 1, 2, bis_indices);
            }
        } else {
            status = bt_sink_srv_cap_stream_start_broadcast_reception(scan_info->sync_handle, 1, 1, bis_indices);
        }
    } else {
        uint8_t bis_index = bt_sink_srv_cap_stream_get_bis_index_by_audio_location(audio_location, subgroup_idx);
        status = bt_sink_srv_cap_stream_start_broadcast_reception(scan_info->sync_handle, 1, 1, &bis_index);
    }
    return status;
}

static bt_status_t race_le_audio_stop_bis(bool is_stop_pa)
{
    bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();
    bt_status_t status = BT_STATUS_FAIL;

    if (is_stop_pa) {
        if (ble_bap_is_syncing_to_pa()) {
            if (g_leaudio_pa_sync_handle) {
                ble_bap_stop_syncing_to_periodic_advertising(g_leaudio_pa_sync_handle);
                g_leaudio_pa_sync_handle = 0;
                status = BT_STATUS_SUCCESS;
            }
        }
    }

    if (bt_sink_srv_cap_stream_is_broadcast_streaming()) {
        if (big_info->big_handle) {
            bt_sink_srv_cap_am_audio_stop(CAP_AM_BROADCAST_MUSIC_MODE);
            status = BT_STATUS_SUCCESS;
        }
    }

    return status;
}

static void race_le_audio_reset_bis_params(void)
{
    memset(&g_leaudio_current_bms_addr, 0, sizeof(bt_addr_t));
    memset(&g_leaudio_retry_bms_addr, 0, sizeof(bt_addr_t));

    if (NULL != curr_ea_data.data) {
        le_audio_free(curr_ea_data.data);
        curr_ea_data.data = NULL;
    }

    memset(&curr_ea_data, 0, sizeof(bt_sink_srv_cap_event_base_broadcast_audio_announcements_t));
    memset(&curr_pa_data, 0, sizeof(bt_sink_srv_cap_event_base_basic_audio_announcements_t));
}

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
void *race_le_audio_scan_broadcast_source_handler(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        race_le_audio_scan_mode scan_mode;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_LEAUDIO_SCAN_BROADCAST_SOURCE,
                                 (uint16_t)sizeof(race_le_audio_status_t),
                                 channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("[RACE_LEA] scan_broadcast_source_handler, mode:%d", 1, pCmd->scan_mode);

    if (pEvt) {
        bt_status_t status = BT_STATUS_FAIL;

        if (RACE_LE_AUDIO_SCAN_MODE_STOP != pCmd->scan_mode) {
            if (!bt_sink_srv_cap_stream_is_scanning_broadcast_source()) {
                memset(&g_leaudio_current_bms_addr, 0, sizeof(bt_addr_t));
                memset(&g_leaudio_retry_bms_addr, 0, sizeof(bt_addr_t));
                if (ble_bap_is_syncing_to_pa()) {
                    /* Stop BIS & PA to retry scan */
                    g_leaudio_flag_retry_scan = true;
                    status = race_le_audio_stop_bis(true);
                    RACE_LOG_MSGID_I("[RACE_LEA] Retry Scan", 0);
                } else {
                    /* Basic scan flow */
                    status = race_le_audio_scan(pCmd->scan_mode);
                }
            }
        } else {
            status = ble_bap_stop_scanning_broadcast_source();
        }

        if (BT_STATUS_SUCCESS == status) {
            ret = RACE_ERRCODE_SUCCESS;
        }
        pEvt->status = ret;
    }

    return pEvt;
}

void *race_le_audio_select_broadcast_source_handler(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        bt_bd_addr_t addr;
        uint8_t addr_type;
        uint8_t advertising_sid;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_LEAUDIO_SELECT_BROADCAST_SOURCE,
                                 (uint16_t)sizeof(race_le_audio_status_t),
                                 channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("[RACE_LEA] select_broadcast_source_handler, addr:%02X:%02X:%02X:%02X:%02X:%02X", 6,
                     pCmd->addr[5], pCmd->addr[4], pCmd->addr[3], pCmd->addr[2], pCmd->addr[1], pCmd->addr[0]);

    if (pEvt) {
        bt_status_t status = BT_STATUS_FAIL;

        if (memcmp(g_leaudio_current_bms_addr.addr, pCmd->addr, sizeof(bt_bd_addr_t))) {
            if (bt_sink_srv_cap_stream_is_scanning_broadcast_source()) {
                /* Basic select flow */
                bt_addr_t addr;
                addr.type = pCmd->addr_type;
                memcpy(addr.addr, pCmd->addr, sizeof(bt_bd_addr_t));
                status = bt_sink_srv_cap_stream_broadcast_sync_periodic_advretising(addr, pCmd->advertising_sid);
            } else {
                /* Retry scan & select */
                g_leaudio_flag_retry_scan = true;
                g_leaudio_retry_bms_addr.type = pCmd->addr_type;
                memcpy(g_leaudio_retry_bms_addr.addr, pCmd->addr, sizeof(bt_bd_addr_t));
                if (ble_bap_is_syncing_to_pa()) {
                    status = race_le_audio_stop_bis(true);
                } else {
                    status = race_le_audio_scan(RACE_LE_AUDIO_SCAN_MODE_ALL);
                }
                RACE_LOG_MSGID_I("[RACE_LEA] Retry Scan & Select", 0);
            }
        }

        if (BT_STATUS_SUCCESS == status) {
            ret = RACE_ERRCODE_SUCCESS;
        }
        pEvt->status = ret;
    }

    return pEvt;
}

void *race_le_audio_play_bis_handler(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t subgroup_idx;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_LEAUDIO_PLAY_BIS,
                                 (uint16_t)sizeof(race_le_audio_status_t),
                                 channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("[RACE_LEA] play_bis_handler, subgroup_idx:%d", 1, pCmd->subgroup_idx);

    if (pEvt) {
        bt_status_t status = BT_STATUS_FAIL;

        if (ble_bap_is_syncing_to_pa()) {
            if (bt_sink_srv_cap_stream_is_broadcast_streaming()) {
                /* To Do: Select different subgroup idx */
            } else {
                /* Basic play flow */
                if (race_le_audio_play_bis(pCmd->subgroup_idx)) {
                    status = BT_STATUS_SUCCESS;
                }
            }
        }

        if (BT_STATUS_SUCCESS == status) {
            ret = RACE_ERRCODE_SUCCESS;
        }
        pEvt->status = ret;
    }

    return pEvt;
}

void *race_le_audio_stop_bis_handler(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
    } PACKED RSP;

    //CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_LEAUDIO_STOP_BIS,
                                 (uint16_t)sizeof(race_le_audio_status_t),
                                 channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("[RACE_LEA] stop_bis_handler", 0);

    if (pEvt) {
        if (BT_STATUS_SUCCESS == race_le_audio_stop_bis(false)) {
            ret = RACE_ERRCODE_SUCCESS;
        }
        pEvt->status = ret;
    }

    return pEvt;
}

void *race_le_audio_bis_reset_handler(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
    } PACKED RSP;

    //CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_LEAUDIO_BIS_RESET,
                                 (uint16_t)sizeof(race_le_audio_status_t),
                                 channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("[RACE_LEA] bis_reset_handler", 0);

    if (pEvt) {
        race_le_audio_reset_bis_params();
        g_leaudio_flag_retry_scan = false;
        race_le_audio_stop_bis(true);
        bt_sink_srv_cap_stream_reset_broadcast_state();
        ret = RACE_ERRCODE_SUCCESS;
        pEvt->status = ret;
    }

    return pEvt;
}

void *race_le_audio_get_bis_state_handler(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
        uint8_t state_type;
    } PACKED RSP;

    //CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_LEAUDIO_GET_BIS_STATE,
                                 (uint16_t)sizeof(race_le_audio_bis_state_t),
                                 channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("[RACE_LEA] get_bis_state_handler", 0);

    if (pEvt) {
        pEvt->state_type = 0;
        if (bt_sink_srv_cap_stream_is_scanning_broadcast_source()) {
            pEvt->state_type = 1;
        } else {
            if (ble_bap_is_syncing_to_pa()) {
                race_le_audio_notify_ea(0, &curr_ea_data);
                race_le_audio_notify_pa(0, &curr_pa_data);
                pEvt->state_type = 2;
                if (bt_sink_srv_cap_stream_is_broadcast_streaming()) {
                    race_le_audio_notify_big_sync_ind(0);
                    pEvt->state_type = 3;
                }
            }
        }

        ret = RACE_ERRCODE_SUCCESS;
        pEvt->status = ret;
    }

    return pEvt;
}

bt_status_t race_le_audio_notify_ea(uint8_t status, bt_sink_srv_cap_event_base_broadcast_audio_announcements_t *msg)
{
    if (NULL == msg) {
        return BT_STATUS_FAIL;
    }

    race_status_t ret = RACE_STATUS_OK;
    typedef struct {
        race_le_audio_ea_report_t info;
    } PACKED NOTI;

    uint8_t channel_id = g_leaudio_channel_id;
    uint16_t length = sizeof(race_le_audio_ea_report_t) + msg->data_length - 1;

    NOTI *noti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                  RACE_LEAUDIO_EA_REPORT,
                                  length,
                                  channel_id);

    if (noti) {
        noti->info.scan_status = status;
        noti->info.addr_type = msg->addr.type;
        memcpy(noti->info.addr, msg->addr.addr, sizeof(bt_bd_addr_t));
        noti->info.advertising_sid = msg->advertising_sid;
        noti->info.rssi = msg->rssi;
        noti->info.data_length = msg->data_length;
        memcpy(&noti->info.data[0], &msg->data[0], msg->data_length);
        //memcpy(&(noti->info), msg, length);

        RACE_LOG_MSGID_I("[RACE_LEA] notify_ea_report, channel_id:%x scan_status:%d", 2, channel_id, noti->info.scan_status);

        if (0 == noti->info.scan_status) {

            RACE_LOG_MSGID_I("[RACE_LEA] notify_ea_report, addr:%02X:%02X:%02X:%02X:%02X:%02X", 6,
                             noti->info.addr[5], noti->info.addr[4], noti->info.addr[3], noti->info.addr[2], noti->info.addr[1], noti->info.addr[0]);

            if (!memcmp(&g_leaudio_retry_bms_addr, &msg->addr, sizeof(bt_addr_t))) {
                status = bt_sink_srv_cap_stream_broadcast_sync_periodic_advretising(g_leaudio_retry_bms_addr, msg->advertising_sid);
            }
        }

        ret = race_flush_packet((uint8_t *)noti, channel_id);
    }
    return (RACE_STATUS_OK == ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL);
}

bt_status_t race_le_audio_notify_pa(uint8_t status, bt_sink_srv_cap_event_base_basic_audio_announcements_t *msg)
{
    race_status_t ret = RACE_STATUS_OK;
    typedef struct {
        race_le_audio_pa_report_t info;
    } PACKED NOTI;

    uint8_t channel_id = g_leaudio_channel_id, offset = 0, length = 2, subgroup_idx;

    /*calculate data length*/
    if (NULL != msg) {
        for (subgroup_idx = 0; subgroup_idx < msg->level_1->num_subgroups; subgroup_idx++) {
            /*subgroup_idx(1) + codec_id(5) + codec_specific_configuration_length(1) + codec_specific_configuration(variable) +
            metadata_length(1) + metadata(variable)*/
            length += (1 + AUDIO_CODEC_ID_SIZE + 1 + msg->level_1->level_2[subgroup_idx].codec_specific_configuration_length
                       + 1 + msg->level_1->level_2[subgroup_idx].metadata_length);
        }
    }

    NOTI *noti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                  RACE_LEAUDIO_PA_REPORT,
                                  length,
                                  channel_id);

    if (noti) {
        noti->info.sync_status = status;
        if (NULL != msg) {
            noti->info.num_subgroup = msg->level_1->num_subgroups;
            for (subgroup_idx = 0; subgroup_idx < msg->level_1->num_subgroups; subgroup_idx++) {
                noti->info.subgroup_data[offset] = subgroup_idx;
                offset++;

                memcpy(&noti->info.subgroup_data[offset], &msg->level_1->level_2[subgroup_idx].codec_id[0], AUDIO_CODEC_ID_SIZE);
                offset += AUDIO_CODEC_ID_SIZE;

                noti->info.subgroup_data[offset] = msg->level_1->level_2[subgroup_idx].codec_specific_configuration_length;
                offset++;

                memcpy(&noti->info.subgroup_data[offset], &msg->level_1->level_2[subgroup_idx].codec_specific_configuration[0],
                       msg->level_1->level_2[subgroup_idx].codec_specific_configuration_length);
                offset += msg->level_1->level_2[subgroup_idx].codec_specific_configuration_length;

                noti->info.subgroup_data[offset] = msg->level_1->level_2[subgroup_idx].metadata_length;
                offset++;

                memcpy(&noti->info.subgroup_data[offset], &msg->level_1->level_2[subgroup_idx].metadata,
                       msg->level_1->level_2[subgroup_idx].metadata_length);
                offset += msg->level_1->level_2[subgroup_idx].metadata_length;
            }
        }

        RACE_LOG_MSGID_I("[RACE_LEA] notify_pa_report, channel_id:%x data_length:%x sync_status:%d num_subgroup:%d", 4, channel_id, length, noti->info.sync_status, noti->info.num_subgroup);

        ret = race_flush_packet((uint8_t *)noti, channel_id);
    }
    return (RACE_STATUS_OK == ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL);
}

bt_status_t race_le_audio_notify_pa_sync_ind(uint8_t status, ble_bap_periodic_adv_sync_established_notify_t *msg)
{
    race_status_t ret = RACE_STATUS_OK;
    typedef struct {
        race_le_audio_pa_report_t info;
    } PACKED NOTI;

    uint8_t channel_id = g_leaudio_channel_id;

    NOTI *noti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                  RACE_LEAUDIO_PA_REPORT,
                                  sizeof(race_le_audio_pa_report_t),
                                  channel_id);

    if (noti) {
        noti->info.sync_status = status;
        noti->info.num_subgroup = 0;
        if ((0 == noti->info.sync_status) && (NULL != msg)) {
            g_leaudio_pa_sync_handle = msg->sync_handle;
            memcpy(&g_leaudio_current_bms_addr, &msg->advertiser_addr, sizeof(bt_addr_t));
            memset(&g_leaudio_retry_bms_addr, 0, sizeof(bt_addr_t));
            ble_bap_config_adv_report(g_leaudio_current_bms_addr, &curr_ea_data, &curr_pa_data);
        }

        RACE_LOG_MSGID_I("[RACE_LEA] notify_pa_sync_ind, channel_id:%x status:%d", 2, channel_id, noti->info.sync_status);
        ret = race_flush_packet((uint8_t *)noti, channel_id);
    }
    return (RACE_STATUS_OK == ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL);
}

bt_status_t race_le_audio_notify_big_sync_ind(uint8_t status)
{
    race_status_t ret = RACE_STATUS_OK;
    typedef struct {
        race_le_audio_status_t info;
    } PACKED NOTI;

    uint8_t channel_id = g_leaudio_channel_id;

    NOTI *noti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                  RACE_LEAUDIO_BIG_SYNC_IND,
                                  sizeof(race_le_audio_status_t),
                                  channel_id);

    if (noti) {
        noti->info.status = status;
        RACE_LOG_MSGID_I("[RACE_LEA] notify_big_sync_ind, channel_id:%x status:%d", 2, channel_id, noti->info.status);
        ret = race_flush_packet((uint8_t *)noti, channel_id);
    }
    return (RACE_STATUS_OK == ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL);
}

bt_status_t race_le_audio_notify_pa_terminated_ind(uint8_t status)
{
    race_status_t ret = RACE_STATUS_OK;
    typedef struct {
        race_le_audio_pa_report_t info;
    } PACKED NOTI;

    uint8_t channel_id = g_leaudio_channel_id;

    NOTI *noti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                  RACE_LEAUDIO_PA_REPORT,
                                  sizeof(race_le_audio_pa_report_t),
                                  channel_id);

    if (noti) {
        noti->info.sync_status = status;
        noti->info.num_subgroup = 0;
        RACE_LOG_MSGID_I("[RACE_LEA] notify_pa_terminated_ind, channel_id:%x retry_scan:%d", 2, channel_id, g_leaudio_flag_retry_scan);
        if (g_leaudio_flag_retry_scan) {
            if (!bt_sink_srv_cap_stream_is_broadcast_streaming()) {
                g_leaudio_flag_retry_scan = false;
                race_le_audio_scan(RACE_LE_AUDIO_SCAN_MODE_ALL);
            }
        } else {
            race_le_audio_reset_bis_params();
        }
        ret = race_flush_packet((uint8_t *)noti, channel_id);
    }
    return (RACE_STATUS_OK == ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL);
}

bt_status_t race_le_audio_notify_big_terminated_ind(uint8_t status)
{
    race_status_t ret = RACE_STATUS_OK;
    typedef struct {
        race_le_audio_status_t info;
    } PACKED NOTI;

    uint8_t channel_id = g_leaudio_channel_id;

    NOTI *noti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                                  RACE_LEAUDIO_BIG_TERMINATED_IND,
                                  sizeof(race_le_audio_status_t),
                                  channel_id);

    if (noti) {
        noti->info.status = status;
        RACE_LOG_MSGID_I("[RACE_LEA] notify_big_terminated_ind, channel_id:%x status:%d retry_scan:%d", 3, channel_id, noti->info.status, g_leaudio_flag_retry_scan);

        if (g_leaudio_flag_retry_scan) {
            g_leaudio_flag_retry_scan = false;
            race_le_audio_scan(RACE_LE_AUDIO_SCAN_MODE_ALL);
        }

        ret = race_flush_packet((uint8_t *)noti, channel_id);
    }
    return (RACE_STATUS_OK == ret ? BT_STATUS_SUCCESS : BT_STATUS_FAIL);
}

#elif defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
#include "app_le_audio.h"
#include "app_le_audio_utillity.h"

void *race_le_audio_set_dongle_stream_mode_handler(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t mode;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_LEAUDIO_SET_DONGLE_STREAM_MODE,
                                 (uint16_t)sizeof(race_le_audio_status_t),
                                 channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("channel_id = %x \r\n", 1, channel_id);
    RACE_LOG_MSGID_I("race_le_audio_set_dongle_stream_mode_handler, set mode: %d", 1, pCmd->mode);

    if (pEvt) {
        if (pCmd->mode == 1) {
            app_le_audio_start_unicast();
            ret = RACE_ERRCODE_SUCCESS;
        } else if (pCmd->mode == 2) {
            app_le_audio_start_broadcast();
            ret = RACE_ERRCODE_SUCCESS;
        }
        pEvt->status = ret;
    }

    return pEvt;
}

void *race_le_audio_get_dongle_stream_mode_handler(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
        uint8_t mode;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_LEAUDIO_GET_DONGLE_STREAM_MODE,
                                 (uint16_t)sizeof(race_le_audio_dongle_mode_t),
                                 channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("channel_id = %x \r\n", 1, channel_id);
    RACE_LOG_MSGID_I("race_le_audio_get_dongle_stream_mode_handler", 0);

    if (pEvt) {
        pEvt->mode = app_le_audio_get_current_mode();
        ret = RACE_ERRCODE_SUCCESS;
        pEvt->status = ret;
    }

    return pEvt;
}
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

void *RACE_CmdHandler_LE_AUDIO(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void *ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_le_audio, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
                     pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    if (pRaceHeaderCmd->hdr.type == RACE_TYPE_COMMAND ||
        pRaceHeaderCmd->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP) {
#if defined(AIR_LE_AUDIO_BIS_ENABLE) && !defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        g_leaudio_channel_id = channel_id;
#endif
        switch (pRaceHeaderCmd->hdr.id) {
#if defined(AIR_LE_AUDIO_BIS_ENABLE) && !defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
            case RACE_LEAUDIO_SCAN_BROADCAST_SOURCE : {
                ptr = race_le_audio_scan_broadcast_source_handler(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_LEAUDIO_SELECT_BROADCAST_SOURCE : {
                ptr = race_le_audio_select_broadcast_source_handler(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_LEAUDIO_PLAY_BIS: {
                ptr = race_le_audio_play_bis_handler(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_LEAUDIO_STOP_BIS: {
                ptr = race_le_audio_stop_bis_handler(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_LEAUDIO_BIS_RESET: {
                ptr = race_le_audio_bis_reset_handler(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_LEAUDIO_GET_BIS_STATE: {
                ptr = race_le_audio_get_bis_state_handler(pRaceHeaderCmd, channel_id);
            }
            break;

#elif defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
            case RACE_LEAUDIO_SET_DONGLE_STREAM_MODE: {
                ptr = race_le_audio_set_dongle_stream_mode_handler(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_LEAUDIO_GET_DONGLE_STREAM_MODE: {
                ptr = race_le_audio_get_dongle_stream_mode_handler(pRaceHeaderCmd, channel_id);
                break;
            }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
            default: {
                break;
            }
        }
    }

    return ptr;
}


#endif /* AIR_LE_AUDIO_ENABLE && MTK_RACE_CMD_ENABLE */

