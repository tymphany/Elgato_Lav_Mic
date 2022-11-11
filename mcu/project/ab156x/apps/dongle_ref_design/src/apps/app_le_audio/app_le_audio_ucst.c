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

#include "app_le_audio.h"
#include "app_le_audio_usb.h"
#include "app_le_audio_ucst.h"
#include "app_le_audio_air.h"
#include "app_le_audio_aird.h"
#include "usb_hid_srv.h"
#include "app_le_audio_ucst_utillity.h"
#include "app_le_audio_nvkey_struct.h"
#include "app_dongle_connection_common.h"

#include "bt_gattc_discovery.h"
#include "bt_device_manager_le.h"

#include "bt_le_audio_source.h"
#include "ble_bap_client.h"
#include "ble_csip.h"
#include "ble_bap.h"
#include "ble_tmap.h"

#include "apps_events_event_group.h"
#include "ui_shell_manager.h"
#include "hal_usb.h"
#include "app_dongle_le_race.h"
#include "bt_le_audio_msglog.h"
#ifdef AIR_SILENCE_DETECTION_ENABLE
#include "bt_sink_srv_ami.h"
#include "app_le_audio_utillity.h"
#include "app_le_audio_ucst_utillity.h"
#endif
#ifdef MTK_BLE_GAP_SRV_ENABLE
#include "bt_gap_le_service.h"
#endif

#ifdef AIR_LE_AUDIO_BA_ENABLE
#include "app_le_audio_ba.h"
#endif
/**************************************************************************************************
* Define
**************************************************************************************************/
/* ASE releated */
#define APP_LE_AUDIO_UCST_ASE_ID_INVALID    0

#define APP_LE_AUDIO_UCST_ASE_OPCODE_NONE   0

#define APP_LE_AUDIO_UCST_ASE_STATE_INVALID         0xFF

#define APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN    2       /* ASE control point header(2B) = | Opcode (1B) | ASE num (1B) | */
#define APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN      2       /* ASE metadata:(2B) = | ASE ID (1B) | metadata len (1B) | */

#define APP_LE_AUDIO_UCST_MAX_MODE_NUM      2   /* call mode & media mode */

/* CIS releated */
#define APP_LE_AUDIO_CIG_ID_1           0x01    /* unidirectional (c->s) */
#define APP_LE_AUDIO_CIG_ID_2           0x02    /* bidirectional */

#define APP_LE_AUDIO_CIS_ID_1_MEDIA     0x01    /* Left */
#define APP_LE_AUDIO_CIS_ID_2_MEDIA     0x02    /* Right */
#define APP_LE_AUDIO_CIS_ID_3_CALL      0x03    /* Left */
#define APP_LE_AUDIO_CIS_ID_4_CALL      0x04    /* Right */

#define APP_LE_AUDIO_CONN_INTERVAL_STREAMING    0x30
#define APP_LE_AUDIO_CONN_INTERVAL_CONFIG       0x08

/* Metadata: Streaming Audio Contexts */
#define APP_LE_AUDIO_METADATA_STREAMING_AUDIO_CONTEXTS_LEN      0x03
#define APP_LE_AUDIO_METADATA_STREAMING_AUDIO_CONTEXTS_TYPE     0x02
#define APP_LE_AUDIO_CONTEXTS_TYPE_GAME                         0x0004

#define APP_LE_AUDIO_UCST_VCP_DEFAULT_VOLUME 0xFF

#define APP_LE_AUDIO_UCST_IS_CALL_MODE  (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target)

#define APP_LE_AUDIO_PREPARE_VCMD_MODE_DISCONN  0x01
#define APP_LE_AUDIO_PREPARE_VCMD_MODE_CONN     0x02

/**************************************************************************************************
* Structure
**************************************************************************************************/
BT_PACKED(
typedef struct {
    bt_handle_t handle;     /* LE Connection Handle */
    uint8_t mode;           /* bit0: Disconnect bit1:Connection */
    uint8_t enable;         /* 0: Disable, 1:Enable */
}) app_le_audio_prepare_vcmd_t;

/**************************************************************************************************
* Variable
**************************************************************************************************/
static app_dongle_le_race_event_callback_t g_lea_ucst_callback = NULL;

static bt_addr_t g_lea_ucst_waiting_conn_addr = {0};
static uint8_t g_lea_ucst_waiting_conn_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
static bool g_lea_ucst_waiting_conn_flag = false;
static bool g_lea_ucst_waiting_disconn_flag = false;
static bt_handle_t g_lea_ucst_prepare_vcmd_conn_handle[APP_LE_AUDIO_UCST_LINK_MAX_NUM];
static bt_handle_t g_lea_ucst_prepare_vcmd_disconn_handle[APP_LE_AUDIO_UCST_LINK_MAX_NUM];
static ble_bap_config_codec_param_t g_lea_cofig_code_param = {
    .ase_id = APP_LE_AUDIO_UCST_ASE_ID_INVALID,
    .target_latency = 1,
    .target_phy = 2,
    .codec_id.coding_format = 6,
    .codec_id.company_id = 0,
    .codec_id.vendor_specific_codec_id = 0,
    .codec_specific_configuration_length = 16,
    .codec_specific_configuration.sampling_freq_len = CODEC_CONFIGURATION_LEN_SAMPLING_FREQUENCY,
    .codec_specific_configuration.sampling_freq_type = CODEC_CONFIGURATION_TYPE_SAMPLING_FREQUENCY,
    .codec_specific_configuration.sampling_freq_value = CODEC_CONFIGURATION_SAMPLING_FREQ_48KHZ,
    .codec_specific_configuration.frame_duration_len = CODEC_CONFIGURATION_LEN_FRAME_DURATIONS,
    .codec_specific_configuration.frame_duration_type = CODEC_CONFIGURATION_TYPE_FRAME_DURATIONS,
    .codec_specific_configuration.frame_duration_value = SDU_INTERVAL_10_MS,
    .codec_specific_configuration.octets_per_codec_frame_len = CODEC_CONFIGURATION_LEN_OCTETS_PER_CODEC_FRAME,
    .codec_specific_configuration.octets_per_codec_frame_type = CODEC_CONFIGURATION_TYPE_OCTETS_PER_CODEC_FRAME,
    .codec_specific_configuration.octets_per_codec_frame_value = 155,
    .codec_specific_configuration.audio_channel_alloaction_len = CODEC_CONFIGURATION_LEN_AUDIO_CHANNEL_ALLOCATION,
    .codec_specific_configuration.audio_channel_alloaction_type = CODEC_CONFIGURATION_TYPE_AUDIO_CHANNEL_ALLOCATION,
    .codec_specific_configuration.audio_channel_alloaction_value = AUDIO_LOCATION_FRONT_LEFT,
};

extern app_le_audio_ctrl_t g_lea_ctrl;

extern app_le_audio_ucst_bonded_list_t g_lea_ucst_bonded_list;

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
extern bt_lea_sink_info_nvkey_t g_lea_ucst_sirk_info[APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM];
#endif

extern uint8_t g_lea_ucst_active_group;

extern app_le_audio_ucst_set_white_list_t g_lea_ucst_set_white_list;

extern app_le_audio_ucst_ctrl_t g_lea_ucst_ctrl;

extern bt_key_t g_le_audio_sirk;

extern app_le_audio_ucst_link_info_t g_lea_ucst_link_info[APP_LE_AUDIO_UCST_LINK_MAX_NUM];

extern app_le_audio_ucst_cis_info_t g_lea_ucst_cis_info[APP_LE_AUDIO_UCST_CIS_MAX_NUM];

extern app_le_audio_ucst_group_info_t g_lea_ucst_group_info[APP_LE_AUDIO_UCST_GROUP_ID_MAX];

extern app_le_audio_qos_params_t g_lea_ucst_qos_params_spk_0;

extern app_le_audio_qos_params_t g_lea_ucst_qos_params_spk_1;

extern app_le_audio_qos_params_t g_lea_ucst_qos_params_mic_0;

extern app_le_audio_ucst_cig_params_test_t *g_lea_ucst_cig_params_test;

extern uint8_t g_lea_ucst_test_mode_flag;

extern uint8_t g_lea_ucst_ccid_list_size;

extern uint8_t g_lea_ucst_ccid_list[APP_LE_AUDIO_UCST_MAX_CCID_LIST_SIZE];

extern uint8_t g_lea_ucst_pts_set_size;

#ifdef AIR_MS_TEAMS_ENABLE
bool g_lea_ucst_pts_remote_call_service = false;
extern ble_tbs_call_index_t g_curr_call_idx;
#endif

/**************************************************************************************************
* Prototype
**************************************************************************************************/
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
extern void app_le_audio_ucst_write_sirk_list_to_nvkey(void);
#endif

extern void app_le_audio_usb_hid_handle_busy_call(bool busy);

extern void bt_app_common_at_cmd_print_report(char *string);

extern bt_status_t ble_tbs_switch_device_completed(void);

extern bt_status_t ble_vcp_set_default_volume(uint8_t volume);

extern bt_status_t bt_gap_le_remove_cig(const bt_gap_le_remove_cig_t *params);

#if defined(AIR_MS_TEAMS_ENABLE) && defined(AIR_LE_AUDIO_UNICAST_ENABLE)
extern bool app_le_audio_usb_hid_handle_call_active(void);
extern bool app_le_audio_usb_hid_handle_call_end(void);
extern bool app_le_audio_usb_hid_handle_call_hold(void);
extern bool app_le_audio_usb_hid_handle_call_unhold(void);
#endif

/**************************************************************************************************
* Static Functions
**************************************************************************************************/
static void app_le_audio_ucst_reset_release(void)
{
    uint32_t i;

    for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
        g_lea_ucst_link_info[i].release = false;
    }

    g_lea_ucst_ctrl.release = false;
}

static void app_le_audio_ucst_send_prepare_vcmd(bt_handle_t handle, uint8_t mode, uint8_t enable)
{
    app_le_audio_prepare_vcmd_t param = {
        .handle = handle,
        .mode = mode,
        .enable = enable,
    };

    bt_hci_send_vendor_cmd(0xFDCE, (uint8_t *)&param, sizeof(app_le_audio_prepare_vcmd_t));

    uint8_t i = 0;
    if (APP_LE_AUDIO_PREPARE_VCMD_MODE_DISCONN == mode) {
        for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
            if (enable) {
                if (BT_HANDLE_INVALID == g_lea_ucst_prepare_vcmd_disconn_handle[i]) {
                    g_lea_ucst_prepare_vcmd_disconn_handle[i] = handle;
                    break;
                }
            } else {
                if (handle == g_lea_ucst_prepare_vcmd_disconn_handle[i]) {
                    g_lea_ucst_prepare_vcmd_disconn_handle[i] = BT_HANDLE_INVALID;
                    break;
                }
            }
        }
    } else if (APP_LE_AUDIO_PREPARE_VCMD_MODE_CONN == mode) {
        for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
            if (enable) {
                if (BT_HANDLE_INVALID == g_lea_ucst_prepare_vcmd_conn_handle[i]) {
                    g_lea_ucst_prepare_vcmd_conn_handle[i] = handle;
                    break;
                }
            } else {
                if (handle == g_lea_ucst_prepare_vcmd_conn_handle[i]) {
                    g_lea_ucst_prepare_vcmd_conn_handle[i] = BT_HANDLE_INVALID;
                    break;
                }
            }
        }
    }
}

static uint8_t app_le_audio_ucst_get_ase_idx(app_le_audio_ucst_link_info_t *p_info, uint8_t ase_id)
{
    uint8_t i;

    if ((NULL == p_info) ||
        (APP_LE_AUDIO_UCST_ASE_ID_INVALID == ase_id)) {
        LE_AUDIO_MSGLOG_I("[APP] get_ase_idx error, ase_id", 1, ase_id);
        return APP_LE_AUDIO_UCST_ASE_MAX_NUM;
    }

    for (i = 0; i < APP_LE_AUDIO_UCST_ASE_MAX_NUM; i++) {
        if (ase_id == p_info->ase[i].id) {
            /*LE_AUDIO_MSGLOG_I("[APP] get_ase_idx, id[%x]:%x direction:%x ase_state:%x", 4, i,
                                p_info->ase[i].id,
                                p_info->ase[i].direction,
                                p_info->ase[i].curr_state);*/
            break;
        }
    }

    if (APP_LE_AUDIO_UCST_ASE_MAX_NUM == i) {
        LE_AUDIO_MSGLOG_I("[APP] get_ase_idx not found, ase_id", 1, ase_id);
    }

    return i;
}

uint8_t app_le_audio_ucst_get_max_link_num(void)
{
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    return APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM;
#else
    return APP_LE_AUDIO_UCST_LINK_MAX_NUM;
#endif
}

uint8_t app_le_audio_ucst_get_link_num_ex(void)
{
    uint8_t i = 0, count = 0;

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        if ((BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) &&
            (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL != g_lea_ucst_link_info[i].next_state)) {
            count++;
        }
    }

    return count;
}

uint8_t app_le_audio_ucst_get_group_link_num_ex(uint8_t group_id)
{
    uint8_t link_idx, i, num = 0;

    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= group_id) {
        return 0;
    }

    if (0 == g_lea_ucst_group_info[group_id].size) {
        return 0;
    }

    for (i = 0; i < APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM; i++) {
        if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID != (link_idx = g_lea_ucst_group_info[group_id].link_idx[i])) {
            if (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL != g_lea_ucst_link_info[link_idx].next_state) {
                num++;
            }
        }
    }

    return num;
}

static uint16_t app_le_audio_ucst_get_metadata_len(void)
{
    /* MUST sync with API: app_le_audio_ucst_set_metadata() */

    if (g_lea_ucst_ccid_list_size) {
        /* metadata_len: (num of metadata + context_type metadata len + CCID_list metadata len) */
        return (2 + METADATA_LTV_LEN_STREAMING_AUDIO_CONTEXTS + (1 + g_lea_ucst_ccid_list_size));
    }

    /* metadata: (num of metadata + context_type metadata len) */
    return (1 + METADATA_LTV_LEN_STREAMING_AUDIO_CONTEXTS);
}

static bool app_le_audio_ucst_set_ase(app_le_audio_ucst_link_info_t *p_info, uint8_t ase_id, uint8_t ase_state, bt_le_audio_direction_t direction)
{
    uint8_t i;

    if (NULL == p_info) {
        LE_AUDIO_MSGLOG_I("[APP][U] set_ase, p_info null", 0);
        return false;
    }

    for (i = 0; i < APP_LE_AUDIO_UCST_ASE_MAX_NUM; i++) {
        if (0 == p_info->ase[i].id) {
            p_info->ase[i].id = ase_id;
            p_info->ase[i].curr_state = ase_state;
            p_info->ase[i].direction = direction;
            LE_AUDIO_MSGLOG_I("[APP][U] set_ase, id[%x]:%x direction:%x ase_state:%x", 4, i,
                              p_info->ase[i].id,
                              p_info->ase[i].direction,
                              p_info->ase[i].curr_state);
            return true;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] set_ase, no space!", 0);
    return false;
}

static uint16_t app_le_audio_ucst_set_metadata(uint8_t *p_buf, uint16_t context_type)
{
    /* MUST sync with API: app_le_audio_ucst_get_metadata_len() */

    ble_bap_ltv_structure_t *p_ltv = NULL;
    uint16_t i = 0;

    if (NULL == p_buf) {
        return 0;
    }

    /* LTV_0: AUDIO_CONTEXTS */
    p_ltv = (ble_bap_ltv_structure_t *)&p_buf[0];
    /* lenght: */
    p_ltv->length = METADATA_LTV_LEN_STREAMING_AUDIO_CONTEXTS;
    /* type: */
    p_ltv->type = METADATA_LTV_TYPE_STREAMING_AUDIO_CONTEXTS;
    /* value: */
    p_ltv->value[0] = (context_type & 0xFF);
    p_ltv->value[1] = ((context_type & 0xFF00) >> 8);
    i += (METADATA_LTV_LEN_STREAMING_AUDIO_CONTEXTS + 1);


    /* LTV_1: CCID_LIST */
    if (g_lea_ucst_ccid_list_size) {
        p_ltv = (ble_bap_ltv_structure_t *)&p_buf[i];
        /* lenght: */
        p_ltv->length = (1 + g_lea_ucst_ccid_list_size);
        /* type: */
        p_ltv->type = METADATA_LTV_TYPE_CCID_LIST;
        /* value: */
        p_ltv->value[0] = g_lea_ucst_ccid_list[0];
        if (APP_LE_AUDIO_UCST_MAX_CCID_LIST_SIZE == g_lea_ucst_ccid_list_size) {
            p_ltv->value[1] = g_lea_ucst_ccid_list[1];
        }
        i += (2 + g_lea_ucst_ccid_list_size);
    }

    return i;
}

void app_le_audio_ucst_set_mic_channel(void)
{
    uint32_t channel = 0;
    uint8_t i, tmp;

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= g_lea_ucst_ctrl.curr_group) {
        return;
    }
#endif

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) {
            switch (g_lea_ucst_link_info[i].source_ase_num) {
                case 1: {
                    if (AUDIO_LOCATION_FRONT_LEFT == g_lea_ucst_link_info[i].source_location) {
                        channel |= APP_LE_AUDIO_TRANSMITTER_CHANNEL_L;
                    } else if (AUDIO_LOCATION_FRONT_RIGHT == g_lea_ucst_link_info[i].source_location) {
                        channel |= APP_LE_AUDIO_TRANSMITTER_CHANNEL_R;
                    }
                    break;
                }
                case 2: {
                    channel = APP_LE_AUDIO_TRANSMITTER_CHANNEL_DUAL;
                    if (g_lea_ucst_link_info[i].source_location_num == g_lea_ucst_link_info[i].source_ase_num) {
                        app_le_audio_set_audio_transmitter_mic_channel(channel);
                        return;
                    }
                    break;
                }
                default: {
                    channel |= APP_LE_AUDIO_TRANSMITTER_CHANNEL_L;
                    break;
                }
            }
        }
    }

    if ((0 == channel) || (APP_LE_AUDIO_TRANSMITTER_CHANNEL_DUAL == channel
#ifdef APP_LE_AUDIO_UCST_UPLINK_MIX_ENABLE
        && !app_le_audio_ucst_get_uplink_mix_status()
#endif
        )) {
        LE_AUDIO_MSGLOG_I("[APP][U] set_mic_channel, use L (channel:%x)", 1, channel);
        channel = APP_LE_AUDIO_TRANSMITTER_CHANNEL_L;
    }

    app_le_audio_set_audio_transmitter_mic_channel(channel);
}

static bt_status_t app_le_audio_ucst_add_white_list(bt_addr_t *addr)
{
    bt_addr_t device;
    bt_status_t ret;

    memcpy(&device, addr, sizeof(bt_addr_t));
    ret = bt_gap_le_set_white_list(BT_GAP_LE_ADD_TO_WHITE_LIST, &device);

    LE_AUDIO_MSGLOG_I("[APP][U] add_white_list, ret:%x", 1, ret);

    return ret;
}

static bt_status_t app_le_audio_ucst_remove_white_list(bt_addr_t *addr)
{
    bt_addr_t device;
    bt_status_t ret;

    memcpy(&device, addr, sizeof(bt_addr_t));
    ret = bt_gap_le_set_white_list(BT_GAP_LE_REMOVE_FROM_WHITE_LIST, &device);

    LE_AUDIO_MSGLOG_I("[APP][U] rm_white_list, ret:%x", 1, ret);

    return ret;
}

static bt_status_t app_le_audio_ucst_update_connection_parameter(bt_handle_t handle, uint16_t interval, uint16_t ce_len)
{
    bt_hci_cmd_le_connection_update_t param;
    bt_status_t ret;

    param.connection_handle = handle;
    param.conn_interval_min = interval;
    param.conn_interval_max = interval;
    param.conn_latency = 0x0000;
    param.supervision_timeout = 0x01F4;
    param.minimum_ce_length = ce_len;
    param.maximum_ce_length = ce_len;

    ret = bt_gap_le_update_connection_parameter((const bt_hci_cmd_le_connection_update_t *)(&param));

    LE_AUDIO_MSGLOG_I("[APP][U] update_connection_param, handle:%x ret:%x", 2, handle, ret);
    return ret;
}

static void app_le_audio_ucst_update_active_device_connection_interval(uint16_t interval)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t i, tmp;

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= g_lea_ucst_ctrl.curr_group) {
            break;
        }
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        p_info = &g_lea_ucst_link_info[i];

        if (BT_HANDLE_INVALID != p_info->handle) {
            if (((interval > p_info->curr_interval) &&
                 (interval != p_info->next_interval)) ||
                ((interval == p_info->curr_interval) &&
                 (interval > p_info->next_interval) &&
                 (0 != p_info->next_interval))) {
                LE_AUDIO_MSGLOG_I("[APP] conn_interval, handle:%x interval:%x->%x set:30", 3, p_info->handle,
                                  p_info->curr_interval, p_info->next_interval);
                p_info->next_interval = interval;
                if (BT_STATUS_SUCCESS != app_le_audio_ucst_update_connection_parameter(p_info->handle, interval, 0x0002)) {
                    p_info->next_interval = 0;
                }
            }
        }
    }
}

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
void app_le_audio_ucst_connect_group_device(uint8_t group_id)
{
    g_lea_ucst_waiting_conn_group = group_id;
    if (g_lea_ucst_group_info[group_id].bond_num == g_lea_ucst_group_info[group_id].size) {
        uint8_t i;
        bool in_white_list = false, scan_cs = false, non_connected = false;
        for (i = 0; i < g_lea_ucst_bonded_list.num; i++) {
#if 0   /* for test */
            LE_AUDIO_MSGLOG_I("[APP] connect_group_device, [%x] group:%x link_idx:%x in_white_list:%x", 4,
                              i,
                              g_lea_ucst_bonded_list.device[i].group_id,
                              g_lea_ucst_bonded_list.device[i].link_idx,
                              g_lea_ucst_bonded_list.device[i].in_white_list);
#endif

            if (group_id == g_lea_ucst_bonded_list.device[i].group_id) {
                if (g_lea_ucst_bonded_list.device[i].in_white_list) {
                    in_white_list = true;
                } else {
                    scan_cs = true;
                }
                if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == g_lea_ucst_bonded_list.device[i].link_idx) {
                    non_connected = true;
                }
            }
        }

        if (in_white_list && non_connected) {
            app_le_audio_ucst_connect_bonded_device(scan_cs);
        }

    } else {
        uint8_t i = 0;
        for (i = 0; i < APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM; i++) {
            if (g_lea_ucst_sirk_info[i].group_id == group_id) {
                app_le_audio_ucst_set_sirk(&(g_lea_ucst_sirk_info[i].sirk), false);
                app_le_audio_ucst_connect_coordinated_set(false);
                break;
            }
        }
    }
}
#endif

static bt_status_t app_le_audio_ucst_disconnect(bt_handle_t handle)
{
    bt_status_t ret;
    bt_hci_cmd_disconnect_t param;

    if (BT_HANDLE_INVALID == handle) {
        LE_AUDIO_MSGLOG_I("[APP][U] disconnect, invalid handle", 0);
        return BT_STATUS_FAIL;
    }

    param.connection_handle = handle;
    param.reason = BT_HCI_STATUS_CONNECTION_TERMINATED_BY_LOCAL_HOST;
    ret = bt_gap_le_disconnect(&param);
    LE_AUDIO_MSGLOG_I("[APP][U] disconnect, handle:%x ret:%x", 2, param.connection_handle, ret);

    return ret;
}

static bt_status_t app_le_audio_ucst_exchange_mtu(bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_status_t ret;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return BT_STATUS_CONNECTION_NOT_FOUND;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_EXCHANGE_MTU == p_info->next_state) {
        return BT_STATUS_SUCCESS;
    }

    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_EXCHANGE_MTU;

    BT_GATTC_NEW_EXCHANGE_MTU_REQ(req, 512);
    ret = bt_gattc_exchange_mtu(handle, &req);

    if (BT_STATUS_SUCCESS != ret) {
        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
        LE_AUDIO_MSGLOG_I("[APP][U] exchange_mtu failed, handle:%x ret:%x", 2, handle, ret);
    }

    return ret;
}

bt_status_t app_le_audio_ucst_config_codec(bt_handle_t handle)
{
    ble_bap_ascs_config_codec_operation_t *buf = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_status_t ret;
    uint8_t idx = 0, ase_num = 0, sink_ase_num = 0, source_ase_num = 0;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return BT_STATUS_FAIL;
    }

    /* get the num of SINK ASE to be configured */
    if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
        sink_ase_num = p_info->sink_ase_num;
        p_info->cis_num = sink_ase_num;
    } else {
        p_info->cis_num = p_info->sink_location_num;
        sink_ase_num = (p_info->sink_location_num * 2); /* call mode and media mode */
        if (p_info->sink_ase_num < sink_ase_num) {
            sink_ase_num = p_info->sink_ase_num;
        }
    }

    /* get the num of SOURCE ASE to be configured */
    if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
        source_ase_num = p_info->source_ase_num;
    } else {
        source_ase_num = p_info->source_location_num;
    }

    if (p_info->cis_num < source_ase_num) {
        p_info->cis_num = source_ase_num;
    }

    ase_num = (sink_ase_num + source_ase_num);

    LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, group_size:%x ase_num:(%x,%x) location_num:(%x,%x) cis_num:%x ase_num:%x", 7,
                      p_info->group_size,
                      p_info->sink_ase_num,
                      p_info->source_ase_num,
                      p_info->sink_location_num,
                      p_info->source_location_num,
                      p_info->cis_num,
                      ase_num);

    if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_config_codec_param_t)))) {
        LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, malloc failed", 0);
        return BT_STATUS_FAIL;
    }

    memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_config_codec_param_t));
    LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, ase_num(total:%x sink:%x source:%x)", 3, ase_num, sink_ase_num, source_ase_num);

    buf->opcode = ASE_OPCODE_CONFIG_CODEC;
    buf->num_of_ase = ase_num;

    uint8_t i, location_idx;

    /* SINK ASE */
    location_idx = 0;
    for (i = 0; i < sink_ase_num; i++) {
        memcpy(&buf->param[idx], &g_lea_cofig_code_param, sizeof(ble_bap_config_codec_param_t));
        buf->param[idx].ase_id = p_info->ase[i].id;

        if (sink_ase_num <= p_info->cis_num) {
            if (0 == p_info->source_ase_num) {
                /* media mode */
                LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, sink 1 media", 0);
                buf->param[idx].codec_specific_configuration.sampling_freq_value = g_lea_ucst_qos_params_spk_1.sampling_freq;
                buf->param[idx].codec_specific_configuration.frame_duration_value = g_lea_ucst_qos_params_spk_1.sdu_interval;
                buf->param[idx].codec_specific_configuration.octets_per_codec_frame_value = g_lea_ucst_qos_params_spk_1.sdu_size;
            } else {
                /* call mode */
                LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, sink 2 call", 0);
                buf->param[idx].codec_specific_configuration.sampling_freq_value = g_lea_ucst_qos_params_spk_0.sampling_freq;
                buf->param[idx].codec_specific_configuration.frame_duration_value = g_lea_ucst_qos_params_spk_0.sdu_interval;
                buf->param[idx].codec_specific_configuration.octets_per_codec_frame_value = g_lea_ucst_qos_params_spk_0.sdu_size;
            }
        } else {
            /* config SINK ASE for media and call */
            /* earbuds: media(ase[0]), call(ase[1]) */
            /* headset: media(ase[0], ase[1]), call(ase[2], ase[3]) */
            if (idx < p_info->cis_num) {
                /* media mode */
                LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, sink 3 media", 0);
                buf->param[idx].codec_specific_configuration.sampling_freq_value = g_lea_ucst_qos_params_spk_1.sampling_freq;
                buf->param[idx].codec_specific_configuration.frame_duration_value = g_lea_ucst_qos_params_spk_1.sdu_interval;
                buf->param[idx].codec_specific_configuration.octets_per_codec_frame_value = g_lea_ucst_qos_params_spk_1.sdu_size;
            } else {
                /* call mode */
                LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, sink 4 call", 0);
                buf->param[idx].codec_specific_configuration.sampling_freq_value = g_lea_ucst_qos_params_spk_0.sampling_freq;
                buf->param[idx].codec_specific_configuration.frame_duration_value = g_lea_ucst_qos_params_spk_0.sdu_interval;
                buf->param[idx].codec_specific_configuration.octets_per_codec_frame_value = g_lea_ucst_qos_params_spk_0.sdu_size;
            }
            if (idx == p_info->cis_num) {
                location_idx = 0;
            }
        }
        if (0 == p_info->sink_location_num) {
            buf->param[idx].codec_specific_configuration.audio_channel_alloaction_value = AUDIO_LOCATION_NONE;
        } else if (1 == p_info->sink_location_num) {
            buf->param[idx].codec_specific_configuration.audio_channel_alloaction_value = p_info->sink_location;
        } else {
            buf->param[idx].codec_specific_configuration.audio_channel_alloaction_value = app_le_audio_ucst_get_location(location_idx, p_info->sink_location);
            location_idx++;
        }
        LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, ase_id[%x]:%x sink[%x] location:%x", 4,
                          idx,
                          buf->param[idx].ase_id,
                          i,
                          buf->param[idx].codec_specific_configuration.audio_channel_alloaction_value);
        idx++;
    }

    /* SOURCE ASE */
    location_idx = 0;
    for (i = 0; i < source_ase_num; i++) {
        memcpy(&buf->param[idx], &g_lea_cofig_code_param, sizeof(ble_bap_config_codec_param_t));
        buf->param[idx].ase_id = p_info->ase[p_info->source_ase_idx + i].id;
        buf->param[idx].codec_specific_configuration.sampling_freq_value = g_lea_ucst_qos_params_mic_0.sampling_freq;
        buf->param[idx].codec_specific_configuration.frame_duration_value = g_lea_ucst_qos_params_mic_0.sdu_interval;
        buf->param[idx].codec_specific_configuration.octets_per_codec_frame_value = g_lea_ucst_qos_params_mic_0.sdu_size;
        if (0 == p_info->source_location_num) {
            buf->param[idx].codec_specific_configuration.audio_channel_alloaction_value = AUDIO_LOCATION_NONE;
        } else if (1 == p_info->source_location_num) {
            buf->param[idx].codec_specific_configuration.audio_channel_alloaction_value = p_info->source_location;
        } else {
            buf->param[idx].codec_specific_configuration.audio_channel_alloaction_value = app_le_audio_ucst_get_location(location_idx, p_info->source_location);
            location_idx++;
        }
        LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, ase_id[%x]:%x source[%x] location:%x", 4,
                          idx,
                          buf->param[idx].ase_id,
                          i,
                          buf->param[idx].codec_specific_configuration.audio_channel_alloaction_value);
        idx++;
    }

    p_info->wait_event.wait_ase_event = buf->num_of_ase;
    p_info->wait_event.wait_ase_cp_event = 1;

    ret = ble_bap_ascs_config_codec(handle, buf);

    LE_AUDIO_MSGLOG_I("[APP][ASE] config_codec, handle:%x ret:%x w_ase:%x", 3, handle, ret, p_info->wait_event.wait_ase_event);

    vPortFree(buf);

#if 0
    memcpy(&g_lea_ucst_ctrl.qos_params_spk_0, &g_lea_ucst_qos_params_spk_0, sizeof(app_le_audio_qos_params_t));
    memcpy(&g_lea_ucst_ctrl.qos_params_spk_1, &g_lea_ucst_qos_params_spk_1, sizeof(app_le_audio_qos_params_t));
    memcpy(&g_lea_ucst_ctrl.qos_params_mic_0, &g_lea_ucst_qos_params_mic_0, sizeof(app_le_audio_qos_params_t));
#endif

    return ret;
}

static bt_status_t app_le_audio_ucst_config_qos(bt_handle_t handle)
{
    ble_bap_ascs_config_qos_operation_t *buf = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_status_t ret;
    uint8_t ase_num, idx = 0, i;
    uint8_t sink_ase_num = 0, source_ase_num = 0;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, link not exist (hdl:%x)", 1, handle);
        return BT_STATUS_FAIL;
    }

    /* get the num of SINK ASE to be configured */
    if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
        sink_ase_num = p_info->sink_ase_num;
    } else {
        sink_ase_num = p_info->sink_location_num;
    }

    ase_num = sink_ase_num;

    if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
        /* Call mode */

        /* get the num of SOURCE ASE to be configured */
        if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
            source_ase_num = p_info->source_ase_num;
        } else {
            source_ase_num = p_info->source_location_num;
        }

        ase_num += source_ase_num;

        if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_config_qos_param_t)))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, malloc failed (call)", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_config_qos_param_t));

        LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, ase_num(total:%x sink:%x source:%x) (call)", 3, ase_num, sink_ase_num, source_ase_num);

        buf->opcode = ASE_OPCODE_CONFIG_QOS;
        buf->num_of_ase = ase_num;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            buf->param[idx].cig_id = APP_LE_AUDIO_CIG_ID_2;
            if (sink_ase_num == p_info->sink_ase_num) {
                buf->param[idx].ase_id = p_info->ase[i].id;
            } else {
                if (p_info->sink_ase_num >= (sink_ase_num * APP_LE_AUDIO_UCST_MAX_MODE_NUM)) {
                    buf->param[idx].ase_id = p_info->ase[sink_ase_num + i].id;
                } else {
                    LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, failed, sink_ase_num:%x %x (call)", 2, sink_ase_num, p_info->sink_ase_num);
                    return BT_STATUS_FAIL;
                }
            }
            if ((1 == g_lea_ucst_ctrl.cis_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->sink_location)) {
                buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_3_CALL;
            } else {
                if (AUDIO_LOCATION_FRONT_LEFT == p_info->sink_location) {
                    buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_3_CALL;
                } else if (AUDIO_LOCATION_FRONT_RIGHT == p_info->sink_location) {
                    buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_4_CALL;
                } else {
                    buf->param[idx].cis_id = (APP_LE_AUDIO_CIS_ID_3_CALL+i);
                }
            }
            buf->param[idx].sdu_interval = app_le_audio_ucst_get_sdu_interval(false);
            buf->param[idx].framing = 0;
            buf->param[idx].phy = 2;
            buf->param[idx].maximum_sdu_size = g_lea_ucst_qos_params_spk_0.sdu_size;
            buf->param[idx].retransmission_number = g_lea_ucst_qos_params_spk_0.rtn;
            buf->param[idx].transport_latency = g_lea_ucst_qos_params_spk_0.latency;
            buf->param[idx].presentation_delay = 0x009C40;
            LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, ase_id[%x]:%x sink[%x] cig_id:%x cis_id:%x", 5,
                              idx,
                              buf->param[idx].ase_id,
                              i,
                              buf->param[idx].cig_id,
                              buf->param[idx].cis_id);
            idx++;
        }

        /* SORUCE ASE */
        for (i = 0; i < source_ase_num; i++) {
            buf->param[idx].cig_id = APP_LE_AUDIO_CIG_ID_2;
            buf->param[idx].ase_id = p_info->ase[p_info->source_ase_idx + i].id;
            if ((1 == g_lea_ucst_ctrl.cis_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->source_location)) {
                buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_3_CALL;
            } else {
                if (AUDIO_LOCATION_FRONT_LEFT == p_info->source_location) {
                    buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_3_CALL;
                } else if (AUDIO_LOCATION_FRONT_RIGHT == p_info->source_location) {
                    buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_4_CALL;
                } else {
                    buf->param[idx].cis_id = (APP_LE_AUDIO_CIS_ID_3_CALL+i);
                }
            }
            buf->param[idx].sdu_interval = app_le_audio_ucst_get_sdu_interval(true);
            buf->param[idx].framing = 0;
            buf->param[idx].phy = 2;
            buf->param[idx].maximum_sdu_size = g_lea_ucst_qos_params_mic_0.sdu_size;
            buf->param[idx].retransmission_number = g_lea_ucst_qos_params_mic_0.rtn;
            buf->param[idx].transport_latency = g_lea_ucst_qos_params_mic_0.latency;
            buf->param[idx].presentation_delay = 0x009C40;
            LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, ase_id[%x]:%x source[%x] cig_id:%x cis_id:%x", 5,
                              idx,
                              buf->param[idx].ase_id,
                              i,
                              buf->param[idx].cig_id,
                              buf->param[idx].cis_id);
            idx++;
        }
    } else {
        /* Media mode */

        if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_config_qos_param_t)))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, malloc failed", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_config_qos_param_t));

        LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, ase_num(total:%x sink:%x)", 2, ase_num, sink_ase_num);

        buf->opcode = ASE_OPCODE_CONFIG_QOS;
        buf->num_of_ase = ase_num;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            buf->param[idx].cig_id = APP_LE_AUDIO_CIG_ID_1;
            buf->param[idx].ase_id = p_info->ase[i].id;
            if ((1 == g_lea_ucst_ctrl.cis_num) && (AUDIO_LOCATION_FRONT_LEFT == p_info->sink_location)) {
                buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_1_MEDIA;
            } else {
                if (AUDIO_LOCATION_FRONT_LEFT == p_info->sink_location) {
                    buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_1_MEDIA;
                } else if (AUDIO_LOCATION_FRONT_RIGHT == p_info->sink_location) {
                    buf->param[idx].cis_id = APP_LE_AUDIO_CIS_ID_2_MEDIA;
                } else {
                    buf->param[idx].cis_id = (APP_LE_AUDIO_CIS_ID_1_MEDIA+i);
                }
            }

            buf->param[idx].sdu_interval = app_le_audio_ucst_get_sdu_interval(false);
            buf->param[idx].framing = 0;
            buf->param[idx].phy = 2;
            buf->param[idx].maximum_sdu_size = g_lea_ucst_qos_params_spk_1.sdu_size;
            buf->param[idx].retransmission_number = g_lea_ucst_qos_params_spk_1.rtn;
            buf->param[idx].transport_latency = g_lea_ucst_qos_params_spk_1.latency;
            buf->param[idx].presentation_delay = 0x009C40;
            LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, ase_id[%x]:%x sink[%x] cig_id:%x cis_id:%x", 5,
                              idx,
                              buf->param[idx].ase_id,
                              i,
                              buf->param[idx].cig_id,
                              buf->param[idx].cis_id);
            idx++;
        }
    }

    p_info->wait_event.wait_ase_event = buf->num_of_ase;
    p_info->wait_event.wait_ase_cp_event = 1;

    ret = ble_bap_ascs_config_qos(handle, buf);

    LE_AUDIO_MSGLOG_I("[APP][ASE] config_qos, handle:%x ret:%x w_ase:%x", 3, handle, ret, p_info->wait_event.wait_ase_event);

    vPortFree(buf);

    return ret;
}

static bt_status_t app_le_audio_ucst_enable_ase(bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t *buf = NULL;
    bt_status_t ret;
    uint16_t context_type, metadata_len, idx = 0;
    uint8_t ase_num, i;
    uint8_t sink_ase_num = 0;
    uint16_t total_len = 0;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return BT_STATUS_FAIL;
    }

    /* get context type */
    if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
        context_type = (g_lea_ucst_qos_params_spk_0.sampling_freq == g_lea_ucst_qos_params_mic_0.sampling_freq) ? AUDIO_CONTENT_TYPE_CONVERSATIONAL : AUDIO_CONTENT_TYPE_GAME;
    } else {
        context_type = AUDIO_CONTENT_TYPE_MEDIA;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASE] sampling_freq(to_air:0x%x from_air:0x%x) context_type:0x%x", 3, g_lea_ucst_qos_params_spk_0.sampling_freq, g_lea_ucst_qos_params_mic_0.sampling_freq, context_type);

    /* get metadata len for all ASEs */
    metadata_len = app_le_audio_ucst_get_metadata_len();

    /* get the num of SINK ASE to be configured */
    if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
        sink_ase_num = p_info->sink_ase_num;
    } else {
        sink_ase_num = p_info->sink_location_num;
    }

    ase_num = sink_ase_num;

    if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
        uint8_t source_ase_num = 0;

        /* Call mode */

        /* get the num of SOURCE ASE to be configured */
        if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
            source_ase_num = p_info->source_ase_num;
        } else {
            source_ase_num = p_info->source_location_num;
        }

        ase_num += source_ase_num;

        /* total_len: | opcode(1B) | ase_num(1B) | ASE_0: ase_id(1B)+metadata_len(1B)+Metadata(Varies) | ASE_1....| */
        /* To do: when earch ASEs has different metadata */

        total_len = APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len);
        LE_AUDIO_MSGLOG_I("[APP][ASE] enable, len metadata:%x ase_len:%x total:%x", 3, metadata_len, (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len), total_len);

        if (NULL == (buf = pvPortMalloc(total_len))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] enable, malloc failed (call)", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, total_len);
        LE_AUDIO_MSGLOG_I("[APP][ASE] enable, ase_num(total:%x sink:%x source:%x) (call)", 3, ase_num, sink_ase_num, source_ase_num);

        /* Opcode: Enable(0x03) */
        buf[0] = ASE_OPCODE_ENABLE;
        /* Number_of_ASEs */
        buf[1] = ase_num;
        idx = APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            /* ASE_ID[i] */
            if (sink_ase_num == p_info->sink_ase_num) {
                buf[idx] = p_info->ase[i].id;
            } else {
                if (p_info->sink_ase_num >= (sink_ase_num * APP_LE_AUDIO_UCST_MAX_MODE_NUM)) {
                    buf[idx] = p_info->ase[sink_ase_num + i].id;
                } else {
                    LE_AUDIO_MSGLOG_I("[APP][ASE] enable, failed, sink_ase_num:%x %x (call)", 2, sink_ase_num, p_info->sink_ase_num);
                    return BT_STATUS_FAIL;
                }
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] enable, ase_id:%x sink[%x]", 2, buf[idx], i);

            /* Metadata_Length[i] */
            buf[idx + 1] = metadata_len;
            idx += APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN;

            /* Metadata[i] */
            if (g_lea_ucst_test_mode_flag) {
                context_type = p_info->sink_available_contexts;
            }
            idx += app_le_audio_ucst_set_metadata(&buf[idx], context_type);

        }

        /* SORUCE ASE */
        for (i = 0; i < source_ase_num; i++) {
            /* ASE_ID[i] */
            buf[idx] = p_info->ase[p_info->source_ase_idx + i].id;
            LE_AUDIO_MSGLOG_I("[APP][ASE] enable, ase_id:%x source[%x]", 2, buf[idx], i);

            /* Metadata_Length[i] */
            buf[idx + 1] = metadata_len;
            idx += APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN;

            /* Metadata[i] */
            if (g_lea_ucst_test_mode_flag) {
                context_type = p_info->source_available_contexts;
            }
            idx += app_le_audio_ucst_set_metadata(&buf[idx], context_type);

        }
    } else {

        /* Media mode */

        /* total_len: | opcode(1B) | ase_num(1B) | ASE_0: ase_id(1B)+metadata_len(1B)+Metadata(Varies) | ASE_1....| */
        /* To do: when earch ASEs has different metadata */

        total_len = APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len);
        LE_AUDIO_MSGLOG_I("[APP][ASE] enable, len metadata:%x ase_len:%x total:%x", 3, metadata_len, (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len), total_len);

        if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len)))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] enable, malloc failed", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, (APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len)));
        LE_AUDIO_MSGLOG_I("[APP][ASE] enable, ase_num(total:%x sink:%x)", 2, ase_num, sink_ase_num);

        /* Opcode: Enable(0x03) */
        buf[0] = ASE_OPCODE_ENABLE;
        /* Number_of_ASEs */
        buf[1] = ase_num;
        idx = APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            /* ASE_ID[i] */
            buf[idx] = p_info->ase[i].id;
            LE_AUDIO_MSGLOG_I("[APP][ASE] enable, ase_id:%x sink[%x]", 2, buf[idx], i);

            /* Metadata_Length[i] */
            buf[idx + 1] = metadata_len;
            idx += APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN;

            /* Metadata[i] */
            if (g_lea_ucst_test_mode_flag) {
                context_type = p_info->sink_available_contexts;
            }
            idx += app_le_audio_ucst_set_metadata(&buf[idx], context_type);

        }
    }

    p_info->wait_event.wait_ase_event = ase_num;
    p_info->wait_event.wait_ase_cp_event = 1;

    app_le_audio_ucst_send_prepare_vcmd(handle, APP_LE_AUDIO_PREPARE_VCMD_MODE_CONN, 1);
    ret = ble_bap_ascs_enable_ase_ex(handle, buf, total_len);

    LE_AUDIO_MSGLOG_I("[APP][ASE] enable, handle:%x ret:%x w_ase:%x", 3, p_info->handle, ret, p_info->wait_event.wait_ase_event);

    vPortFree(buf);


    return ret;
}

bt_status_t app_le_audio_ucst_update_metadata(bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t *buf = NULL;
    bt_status_t ret;
    uint16_t context_type, metadata_len, idx = 0;
    uint8_t ase_num, i;
    uint8_t sink_ase_num = 0;
    uint16_t total_len = 0;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return BT_STATUS_FAIL;
    }

    /* get context type */
    if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
        context_type = (g_lea_ucst_qos_params_spk_0.sampling_freq == g_lea_ucst_qos_params_mic_0.sampling_freq) ? AUDIO_CONTENT_TYPE_CONVERSATIONAL : AUDIO_CONTENT_TYPE_GAME;
    } else {
        context_type = AUDIO_CONTENT_TYPE_MEDIA;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASE] sampling_freq(to_air:0x%x from_air:0x%x) context_type:0x%x", 3, g_lea_ucst_qos_params_spk_0.sampling_freq, g_lea_ucst_qos_params_mic_0.sampling_freq, context_type);

    /* get metadata len for all ASEs */
    metadata_len = app_le_audio_ucst_get_metadata_len();

    /* get the num of SINK ASE to be configured */
    if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
        sink_ase_num = p_info->sink_ase_num;
    } else {
        sink_ase_num = p_info->sink_location_num;
    }

    ase_num = sink_ase_num;

    if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
        uint8_t source_ase_num = 0;

        /* Call mode */

        /* get the num of SOURCE ASE to be configured */
        if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
            source_ase_num = p_info->source_ase_num;
        } else {
            source_ase_num = p_info->source_location_num;
        }

        ase_num += source_ase_num;

        /* total_len: | opcode(1B) | ase_num(1B) | ASE_0: ase_id(1B)+metadata_len(1B)+Metadata(Varies) | ASE_1....| */
        /* To do: when earch ASEs has different metadata */

        total_len = APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len);
        LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, len metadata:%x ase_len:%x total:%x", 3, metadata_len, (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len), total_len);

        if (NULL == (buf = pvPortMalloc(total_len))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, malloc failed (call)", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, total_len);
        LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, ase_num(total:%x sink:%x source:%x) (call)", 3, ase_num, sink_ase_num, source_ase_num);

        /* Opcode: Update_metadat(0x07) */
        buf[0] = ASE_OPCODE_UPDATE_METADATA;
        /* Number_of_ASEs */
        buf[1] = ase_num;
        idx = APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            /* ASE_ID[i] */
            if (sink_ase_num == p_info->sink_ase_num) {
                buf[idx] = p_info->ase[i].id;
            } else {
                if (p_info->sink_ase_num >= (sink_ase_num * APP_LE_AUDIO_UCST_MAX_MODE_NUM)) {
                    buf[idx] = p_info->ase[sink_ase_num + i].id;
                } else {
                    LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, failed, sink_ase_num:%x %x (call)", 2, sink_ase_num, p_info->sink_ase_num);
                    return BT_STATUS_FAIL;
                }
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, ase_id:%x sink[%x]", 2, buf[idx], i);

            /* Metadata_Length[i] */
            buf[idx + 1] = metadata_len;
            idx += APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN;

            /* Metadata[i] */
            if (g_lea_ucst_test_mode_flag) {
                context_type = p_info->sink_available_contexts;
            }
            idx += app_le_audio_ucst_set_metadata(&buf[idx], context_type);

        }

        /* SORUCE ASE */
        for (i = 0; i < source_ase_num; i++) {
            /* ASE_ID[i] */
            buf[idx] = p_info->ase[p_info->source_ase_idx + i].id;
            LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, ase_id:%x source[%x]", 2, buf[idx], i);

            /* Metadata_Length[i] */
            buf[idx + 1] = metadata_len;
            idx += APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN;

            /* Metadata[i] */
            if (g_lea_ucst_test_mode_flag) {
                context_type = p_info->source_available_contexts;
            }
            idx += app_le_audio_ucst_set_metadata(&buf[idx], context_type);

        }
    } else {

        /* Media mode */

        /* total_len: | opcode(1B) | ase_num(1B) | ASE_0: ase_id(1B)+metadata_len(1B)+Metadata(Varies) | ASE_1....| */
        /* To do: when earch ASEs has different metadata */

        total_len = APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len);
        LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, len metadata:%x ase_len:%x total:%x", 3, metadata_len, (APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN + metadata_len), total_len);

        if (NULL == (buf = pvPortMalloc(total_len))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata call, malloc failed", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, total_len);
        LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, ase_num(total:%x sink:%x)", 2, ase_num, sink_ase_num);

        /* Opcode: Update_metadat(0x07) */
        buf[0] = ASE_OPCODE_UPDATE_METADATA;
        /* Number_of_ASEs */
        buf[1] = ase_num;
        idx = APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            /* ASE_ID[i] */
            buf[idx] = p_info->ase[i].id;
            LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, ase_id:%x sink[%x]", 2, buf[idx], i);

            /* Metadata_Length[i] */
            buf[idx + 1] = metadata_len;
            idx += APP_LE_AUDIO_UCST_ASE_METADATA_HDR_LEN;

            /* Metadata[i] */
            if (g_lea_ucst_test_mode_flag) {
                context_type = p_info->sink_available_contexts;
            }
            idx += app_le_audio_ucst_set_metadata(&buf[idx], context_type);

        }
    }

    p_info->wait_event.wait_ase_event = ase_num;
    p_info->wait_event.wait_ase_cp_event = 1;

    ret = ble_bap_ascs_update_metadata_ex(handle, buf, total_len);

    LE_AUDIO_MSGLOG_I("[APP][ASE] update_metadata, handle:%x ret:%x w_ase:%x", 3, p_info->handle, ret, p_info->wait_event.wait_ase_event);
    vPortFree(buf);

    return ret;
}

bt_status_t app_le_audio_ucst_set_receiver_start_ready(bt_handle_t handle)
{
    ble_bap_ascs_receiver_start_ready_operation_t *buf = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_status_t ret;
    uint8_t idx = 0, i;
    uint8_t source_ase_num = 0;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return BT_STATUS_FAIL;
    }

    /* get the num of SOURCE ASE to be configured */
    if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
        source_ase_num = p_info->source_ase_num;
    } else {
        source_ase_num = p_info->source_location_num;
    }

    if (0 == source_ase_num) {
        return BT_STATUS_SUCCESS;
    }

    if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + source_ase_num * sizeof(ble_bap_receiver_start_ready_param_t)))) {
        LE_AUDIO_MSGLOG_I("[APP][ASE] set_receiver_start_ready, malloc failed", 0);
        return BT_STATUS_FAIL;
    }

    memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + source_ase_num * sizeof(ble_bap_receiver_start_ready_param_t));
    LE_AUDIO_MSGLOG_I("[APP][ASE] set_receiver_start_ready, ase_num(total:%x)", 1, source_ase_num);

    buf->opcode = ASE_OPCODE_RECEIVER_START_READY;
    buf->num_of_ase = source_ase_num;

    /* SORUCE ASE */
    for (i = 0; i < source_ase_num; i++) {
        buf->param[idx].ase_id = p_info->ase[p_info->source_ase_idx + i].id;
        LE_AUDIO_MSGLOG_I("[APP][ASE] set_receiver_start_ready, ase_id[%d]:%x source[%x]", 3, idx, buf->param[idx].ase_id, i);
        idx++;
    }

    p_info->wait_event.wait_ase_event = buf->num_of_ase;
    p_info->wait_event.wait_ase_cp_event = 1;

    ret = ble_bap_ascs_receiver_start_ready(handle, buf);

    LE_AUDIO_MSGLOG_I("[APP][ASE] set_receiver_start_ready, handle:%x ret:%x w_ase:%x", 3, p_info->handle, ret, p_info->wait_event.wait_ase_event);

    vPortFree(buf);

    app_le_audio_ucst_set_mic_channel();

    return ret;
}

bt_status_t app_le_audio_ucst_disable_ase(bt_handle_t handle)
{
    ble_bap_ascs_disable_operation_t *buf = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_status_t ret;
    uint8_t ase_num, idx = 0, i;
    uint8_t sink_ase_num = 0, source_ase_num = 0;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return BT_STATUS_FAIL;
    }

    /* get the num of SINK ASE to be configured */
    if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
        sink_ase_num = p_info->sink_ase_num;
    } else {
        sink_ase_num = p_info->sink_location_num;
    }

    ase_num = sink_ase_num;

    if ((APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target) ||
        (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
        /* Call mode */

        /* get the num of SOURCE ASE to be configured */
        if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
            source_ase_num = p_info->source_ase_num;
        } else {
            source_ase_num = p_info->source_location_num;
        }

        ase_num += source_ase_num;

        if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_disable_param_t)))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] disable, malloc failed (call)", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_disable_param_t));
        LE_AUDIO_MSGLOG_I("[APP][ASE] disable, ase_num(total:%x sink:%x source:%x) (call)", 3, ase_num, sink_ase_num, source_ase_num);

        buf->opcode = ASE_OPCODE_DISABLE;
        buf->num_of_ase = ase_num;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            if (sink_ase_num == p_info->sink_ase_num) {
                buf->param[idx].ase_id = p_info->ase[i].id;
            } else {
                if (p_info->sink_ase_num >= (sink_ase_num * APP_LE_AUDIO_UCST_MAX_MODE_NUM)) {
                    buf->param[idx].ase_id = p_info->ase[sink_ase_num + i].id;
                } else {
                    LE_AUDIO_MSGLOG_I("[APP][ASE] disable, failed, sink_ase_num:%x %x (call)", 2, sink_ase_num, p_info->sink_ase_num);
                    return BT_STATUS_FAIL;
                }
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] disable, ase_id[%d]:%x sink[%x]", 3, idx, buf->param[idx].ase_id, i);
            idx++;
        }

        /* SORUCE ASE */
        for (i = 0; i < source_ase_num; i++) {
            buf->param[idx].ase_id = p_info->ase[p_info->source_ase_idx + i].id;
            LE_AUDIO_MSGLOG_I("[APP][ASE] disable, ase_id[%d]:%x source[%x]", 3, idx, buf->param[idx].ase_id, i);
            idx++;
        }
    } else {
        /* Media mode */

        if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_disable_param_t)))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] disable, malloc failed", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_disable_param_t));
        LE_AUDIO_MSGLOG_I("[APP][ASE] disable, ase_num(total:%x sink:%x)", 2, ase_num, sink_ase_num);

        buf->opcode = ASE_OPCODE_DISABLE;
        buf->num_of_ase = ase_num;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            buf->param[idx].ase_id = p_info->ase[i].id;
            LE_AUDIO_MSGLOG_I("[APP][ASE] disable, ase_id[%d]:%x sink[%x]", 3, idx, buf->param[idx].ase_id, i);
            idx++;
        }
    }

    p_info->wait_event.wait_ase_event = buf->num_of_ase;
    p_info->wait_event.wait_ase_cp_event = 1;

    app_le_audio_ucst_send_prepare_vcmd(handle, APP_LE_AUDIO_PREPARE_VCMD_MODE_DISCONN, 1);
    ret = ble_bap_ascs_disable_ase(handle, buf);
    LE_AUDIO_MSGLOG_I("[APP][ASE] disable, handle:%x ret:%x w_ase:%x", 3, p_info->handle, ret, p_info->wait_event.wait_ase_event);
    vPortFree(buf);

    return ret;
}

bt_status_t app_le_audio_ucst_release_ase(bt_handle_t handle)
{
    ble_bap_ascs_release_operation_t *buf = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_status_t ret;
    uint8_t ase_num, idx = 0, i;
    uint8_t sink_ase_num = 0, source_ase_num = 0;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return BT_STATUS_FAIL;
    }

    /* get the num of SINK ASE to be configured */
    if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
        sink_ase_num = p_info->sink_ase_num;
    } else {
        sink_ase_num = p_info->sink_location_num;
    }

    ase_num = sink_ase_num;

    if ((APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target) ||
        (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
        /* Call mode */

        /* get the num of SOURCE ASE to be configured */
        if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
            source_ase_num = p_info->source_ase_num;
        } else {
            source_ase_num = p_info->source_location_num;
        }

        ase_num += source_ase_num;

        if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_disable_param_t)))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] release, malloc failed (call)", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_disable_param_t));
        LE_AUDIO_MSGLOG_I("[APP][ASE] release, ase_num(total:%x sink:%x source:%x) (call)", 3, ase_num, sink_ase_num, source_ase_num);

        buf->opcode = ASE_OPCODE_RELEASE;
        buf->num_of_ase = ase_num;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            if (sink_ase_num == p_info->sink_ase_num) {
                buf->param[idx].ase_id = p_info->ase[i].id;
            } else {
                if (p_info->sink_ase_num >= (sink_ase_num * APP_LE_AUDIO_UCST_MAX_MODE_NUM)) {
                    buf->param[idx].ase_id = p_info->ase[sink_ase_num + i].id;
                } else {
                    LE_AUDIO_MSGLOG_I("[APP][ASE] release, failed, sink_ase_num:%x %x (call)", 2, sink_ase_num, p_info->sink_ase_num);
                    return BT_STATUS_FAIL;
                }
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] release, ase_id[%d]:%x sink[%x]", 3, idx, buf->param[idx].ase_id, i);
            idx++;
        }

        /* SORUCE ASE */
        for (i = 0; i < source_ase_num; i++) {
            buf->param[idx].ase_id = p_info->ase[p_info->source_ase_idx + i].id;
            LE_AUDIO_MSGLOG_I("[APP][ASE] release, ase_id[%d]:%x source[%x]", 3, idx, buf->param[idx].ase_id, i);
            idx++;
        }
    } else {
        /* Media mode */

        if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_disable_param_t)))) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] release, malloc failed", 0);
            return BT_STATUS_FAIL;
        }

        memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + ase_num * sizeof(ble_bap_disable_param_t));
        LE_AUDIO_MSGLOG_I("[APP][ASE] release, ase_num(total:%x sink:%x)", 2, ase_num, sink_ase_num);

        buf->opcode = ASE_OPCODE_RELEASE;
        buf->num_of_ase = ase_num;

        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            buf->param[idx].ase_id = p_info->ase[i].id;
            LE_AUDIO_MSGLOG_I("[APP][ASE] release, ase_id[%d]:%x sink[%x]", 3, idx, buf->param[idx].ase_id, i);
            idx++;
        }
    }

    p_info->wait_event.wait_ase_event = buf->num_of_ase;
    p_info->wait_event.wait_ase_cp_event = 1;

    ret = ble_bap_ascs_release_ase(handle, buf);
    LE_AUDIO_MSGLOG_I("[APP][ASE] release, handle:%x ret:%x w_ase:%x", 3, p_info->handle, ret, p_info->wait_event.wait_ase_event);
    vPortFree(buf);

    return ret;
}

bt_status_t app_le_audio_ucst_set_receiver_stop_ready(bt_handle_t handle)
{
    ble_bap_ascs_receiver_stop_ready_operation_t *buf = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_status_t ret;
    uint8_t idx = 0, i;
    uint8_t source_ase_num = 0;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return BT_STATUS_FAIL;
    }

    /* get the num of SOURCE ASE to be configured */
    if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
        source_ase_num = p_info->source_ase_num;
    } else {
        source_ase_num = p_info->source_location_num;
    }

    if (NULL == (buf = pvPortMalloc(APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + source_ase_num * sizeof(ble_bap_receiver_stop_ready_param_t)))) {
        LE_AUDIO_MSGLOG_I("[APP][ASE] set_receiver_stop_ready, malloc failed", 0);
        return BT_STATUS_FAIL;
    }

    memset(buf, 0, APP_LE_AUDIO_UCST_ASE_CTRL_POINT_HDR_LEN + sizeof(ble_bap_receiver_stop_ready_param_t));
    LE_AUDIO_MSGLOG_I("[APP][ASE] set_receiver_stop_ready, ase_num(total:%x)", 1, source_ase_num);

    buf->opcode = ASE_OPCODE_RECEIVER_STOP_READY;
    buf->num_of_ase = source_ase_num;

    /* SORUCE ASE */
    for (i = 0; i < source_ase_num; i++) {
        buf->param[idx].ase_id = p_info->ase[p_info->source_ase_idx + i].id;
        LE_AUDIO_MSGLOG_I("[APP][ASE] set_receiver_stop_ready, ase_id[%d]:%x source[%x]", 3, idx, buf->param[idx].ase_id, i);
        idx++;
    }

    p_info->wait_event.wait_ase_event = buf->num_of_ase;
    p_info->wait_event.wait_ase_cp_event = 1;

    ret = ble_bap_ascs_receiver_stop_ready(handle, buf);

    LE_AUDIO_MSGLOG_I("[APP][ASE] set_receiver_stop_ready, handle:%x ret:%x w_ase:%x", 3, p_info->handle, ret, p_info->wait_event.wait_ase_event);

    vPortFree(buf);

    return ret;
}

static bt_status_t app_le_audio_ucst_test_mode_set_cig_param(void)
{
    bt_gap_le_cis_params_t *p_cis = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_status_t ret;
    uint8_t cis_num = 0, group_size = 0;
    uint8_t i, j;
    uint8_t sink_ase_num = 0, source_ase_num = 0;

    if (0 == app_le_audio_ucst_get_link_num_ex()) {
        LE_AUDIO_MSGLOG_I("[APP][U] test_mode_set_cig_param, no connection!", 0);
        return BT_STATUS_FAIL;
    }

    for (j = 0; j < APP_LE_AUDIO_UCST_LINK_MAX_NUM; j++) {
        p_info = &g_lea_ucst_link_info[j];
        if ((BT_HANDLE_INVALID != p_info->handle) && (0 != p_info->cis_num)) {
            group_size = p_info->group_size;
            cis_num += p_info->cis_num;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param, group_size:%x cis_num:%x", 2,
                      group_size, cis_num);

    /* Use set CIG parameters command */
    if (NULL == (p_cis = (bt_gap_le_cis_params_t *)pvPortMalloc(sizeof(bt_gap_le_cis_params_t) * cis_num))) {
        LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param, malloc failed!", 0);
        return BT_STATUS_OUT_OF_MEMORY;
    }

    memset(p_cis, 0, (sizeof(bt_gap_le_cis_params_t)*cis_num));

    for (i = 0; i < cis_num; i++) {
        for (j = 0; j < APP_LE_AUDIO_UCST_LINK_MAX_NUM; j++) {
            p_info = &g_lea_ucst_link_info[j];
            if (BT_HANDLE_INVALID != p_info->handle) {
                if (!p_info->cis_num) {
                    continue;
                }

                LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param, i:%x j:%x sink(location:%x num:%x) source(location:%x num:%x)", 6, i, j, p_info->sink_location, p_info->sink_location_num, p_info->source_location, p_info->source_location_num);

                if (0 == (i % 2)) {
                    if (((0 != p_info->sink_location_num) && (AUDIO_LOCATION_FRONT_LEFT == p_info->sink_location)) ||
                        ((0 != p_info->source_location_num) && (AUDIO_LOCATION_FRONT_LEFT == p_info->source_location))) {
                        break;
                    }
                } else {
                    if (((0 != p_info->sink_location_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->sink_location)) ||
                        ((0 != p_info->source_location_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->source_location))) {
                        break;
                    }
                }
            }
        }
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == j) {
            break;
        }

        sink_ase_num = 0;
        source_ase_num = 0;

        if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
            sink_ase_num = p_info->sink_ase_num;
        } else {
            sink_ase_num = p_info->sink_location_num;
        }
        if ((0 == p_info->source_location_num) || (p_info->source_location_num >= p_info->source_ase_num)) {
            source_ase_num = p_info->source_ase_num;
        } else {
            source_ase_num = p_info->source_location_num;
        }

        LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param, sink_ase_num:%x source_ase_num:%x", 2,
                          sink_ase_num, source_ase_num);

        if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
            p_cis[i].cis_id = ((0 == i) ? APP_LE_AUDIO_CIS_ID_3_CALL : APP_LE_AUDIO_CIS_ID_4_CALL);
            p_cis[i].rtn_m_to_s = g_lea_ucst_qos_params_spk_0.rtn;
            p_cis[i].rtn_s_to_m = g_lea_ucst_qos_params_mic_0.rtn;
            if (0 == sink_ase_num) {
                p_cis[i].max_sdu_m_to_s = 0x0000;
                LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param 1, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
            } else {
                p_cis[i].max_sdu_m_to_s = g_lea_ucst_qos_params_spk_0.sdu_size;
                LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param 2, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
            }

            if ((0 == source_ase_num) ||
                ((1 == group_size) && (1 == source_ase_num) && (0 != (i % 2)))) {
                p_cis[i].max_sdu_s_to_m = 0x0000;
                LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param 3, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
            } else {
                p_cis[i].max_sdu_s_to_m = g_lea_ucst_qos_params_mic_0.sdu_size;
                LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param 4, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
            }

        } else {
            p_cis[i].cis_id = ((0 == i) ? APP_LE_AUDIO_CIS_ID_1_MEDIA : APP_LE_AUDIO_CIS_ID_2_MEDIA);
            LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param 5, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
            p_cis[i].rtn_m_to_s = g_lea_ucst_qos_params_spk_1.rtn;
            p_cis[i].rtn_s_to_m = g_lea_ucst_qos_params_spk_1.rtn;
            p_cis[i].max_sdu_m_to_s = g_lea_ucst_qos_params_spk_1.sdu_size;
            p_cis[i].max_sdu_s_to_m = 0x0000;
        }

        p_cis[i].phy_m_to_s = 0x02;
        p_cis[i].phy_s_to_m = 0x02;
    }

    bt_gap_le_set_cig_params_t param = {
        .cig_id = APP_LE_AUDIO_CIG_ID_1,
        .sca = 0x00,
        .packing = BT_GAP_LE_CIG_PACKING_INTERLEAVED,
        .framing = BT_GAP_LE_CIG_FRAMING_UNFRAMED,
        .cis_list = p_cis,
        .cis_count = cis_num,
    };

    if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
        param.cig_id = APP_LE_AUDIO_CIG_ID_2;
        param.sdu_interval_m_to_s = app_le_audio_ucst_get_sdu_interval(false);
        param.sdu_interval_s_to_m = app_le_audio_ucst_get_sdu_interval(true);
        param.max_transport_latency_m_to_s = g_lea_ucst_qos_params_spk_0.latency;
        param.max_transport_latency_s_to_m = g_lea_ucst_qos_params_mic_0.latency;

    } else {
        param.sdu_interval_m_to_s = app_le_audio_ucst_get_sdu_interval(false);
        param.sdu_interval_s_to_m = app_le_audio_ucst_get_sdu_interval(false);
        param.max_transport_latency_m_to_s = g_lea_ucst_qos_params_spk_1.latency;
        param.max_transport_latency_s_to_m = g_lea_ucst_qos_params_spk_1.latency;
    }

    g_lea_ucst_ctrl.cig_id = param.cig_id;

    ret = bt_gap_le_set_cig_parameters(&param);
    LE_AUDIO_MSGLOG_I("[APP] test_mode_set_cig_param, ret:%x cig_id:%x cis_num:%x", 3, ret, g_lea_ucst_ctrl.cig_id, cis_num);

    vPortFree(p_cis);

    return ret;
}

static bt_status_t app_le_audio_ucst_set_cig_parameters(void)
{
    bt_status_t ret;
    uint8_t cis_num = 0, group_size = 0;
    uint8_t i, tmp;
    uint8_t sink_ase_num = 0, source_ase_num = 0;

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= g_lea_ucst_ctrl.curr_group) {
        LE_AUDIO_MSGLOG_I("[APP][U] set_cig_parameters, no active group!", 0);
        return BT_STATUS_FAIL;
    }
#endif

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    if (0 == app_le_audio_ucst_get_group_link_num_ex(g_lea_ucst_ctrl.curr_group))
#else
    if (0 == app_le_audio_ucst_get_link_num_ex())
#endif
    {
        LE_AUDIO_MSGLOG_I("[APP][U] set_cig_parameters, no connection!", 0);
        return BT_STATUS_FAIL;
    }

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        if ((BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) && (0 != g_lea_ucst_link_info[i].cis_num)) {
            group_size = g_lea_ucst_link_info[i].group_size;
            cis_num = g_lea_ucst_link_info[i].cis_num;
            if ((0 == g_lea_ucst_link_info[i].sink_location_num) || (g_lea_ucst_link_info[i].sink_location_num >= g_lea_ucst_link_info[i].sink_ase_num)) {
                sink_ase_num = g_lea_ucst_link_info[i].sink_ase_num;
            } else {
                sink_ase_num = g_lea_ucst_link_info[i].sink_location_num;
            }
            if ((0 == g_lea_ucst_link_info[i].source_location_num) || (g_lea_ucst_link_info[i].source_location_num >= g_lea_ucst_link_info[i].source_ase_num)) {
                source_ase_num = g_lea_ucst_link_info[i].source_ase_num;
            } else {
                source_ase_num = g_lea_ucst_link_info[i].source_location_num;
            }
            break;
        }
    }

    if (tmp == app_le_audio_ucst_get_max_link_num()) {
        LE_AUDIO_MSGLOG_I("[APP][U] set_cig_parameters ERROR, check link!", 0);
        return BT_STATUS_FAIL;
    }

    cis_num *= group_size;
    LE_AUDIO_MSGLOG_I("[APP][U] set_cig_parameters, cis_num:%x group_size:%x sink_ase_num:%x source_ase_num:%x", 4, cis_num, group_size, sink_ase_num, source_ase_num);

    if (NULL != g_lea_ucst_cig_params_test) {

        /* Use set CIG parameters test command */
        bt_gap_le_cis_params_test_t *p_cis = NULL;

        if (NULL == (p_cis = (bt_gap_le_cis_params_test_t *)pvPortMalloc(sizeof(bt_gap_le_cis_params_test_t) * cis_num))) {
            LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters_test, malloc failed!", 0);
            return BT_STATUS_OUT_OF_MEMORY;
        }

        memset(p_cis, 0, (sizeof(bt_gap_le_cis_params_test_t)*cis_num));

        for (i = 0; i < cis_num; i++) {
            if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
                /* Call mode */
                p_cis[i].cis_id = ((0 == i) ? APP_LE_AUDIO_CIS_ID_3_CALL : APP_LE_AUDIO_CIS_ID_4_CALL);
                p_cis[i].max_sdu_m_to_s = g_lea_ucst_qos_params_spk_0.sdu_size;
                p_cis[i].max_pdu_m_to_s = g_lea_ucst_qos_params_spk_0.sdu_size;

                if ((0 == source_ase_num) ||
                    ((1 == group_size) && (1 == source_ase_num) && (0 != (i % 2)))) {
                    LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters_test 1, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
                    p_cis[i].max_sdu_s_to_m = 0x0000;
                    p_cis[i].max_pdu_s_to_m = 0x0000;
                    p_cis[i].bn_s_to_m = 0;

                } else {
                    LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters_test 2, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
                    p_cis[i].max_sdu_s_to_m = g_lea_ucst_qos_params_mic_0.sdu_size;
                    p_cis[i].max_pdu_s_to_m = g_lea_ucst_qos_params_mic_0.sdu_size;
                    p_cis[i].bn_s_to_m = g_lea_ucst_cig_params_test->bn;
                }

            } else {
                /* Media mode */
                p_cis[i].cis_id = ((0 == i) ? APP_LE_AUDIO_CIS_ID_1_MEDIA : APP_LE_AUDIO_CIS_ID_2_MEDIA);
                LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters_test 3, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
                p_cis[i].max_sdu_m_to_s = g_lea_ucst_qos_params_spk_1.sdu_size;
                p_cis[i].max_sdu_s_to_m = 0x0000;
                p_cis[i].max_pdu_m_to_s = g_lea_ucst_qos_params_spk_1.sdu_size;
                p_cis[i].max_pdu_s_to_m = 0x0000;
                p_cis[i].bn_s_to_m = 0;
            }

            p_cis[i].nse = g_lea_ucst_cig_params_test->nse;
            p_cis[i].phy_m_to_s = 0x02;
            p_cis[i].phy_s_to_m = 0x02;
            p_cis[i].bn_m_to_s = g_lea_ucst_cig_params_test->bn;
        }

        bt_gap_le_set_cig_params_test_t param = {
            .cig_id = APP_LE_AUDIO_CIG_ID_1,
            .ft_m_to_s = g_lea_ucst_cig_params_test->ft,
            .ft_s_to_m = g_lea_ucst_cig_params_test->ft,
            .iso_interval = g_lea_ucst_cig_params_test->iso_interval,
            .sca = 0x00,
            .packing = BT_GAP_LE_CIG_PACKING_INTERLEAVED,
            .framing = BT_GAP_LE_CIG_FRAMING_UNFRAMED,
            .cis_list = p_cis,
            .cis_count = cis_num,
        };

        if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
            param.cig_id = APP_LE_AUDIO_CIG_ID_2;
            param.sdu_interval_m_to_s = app_le_audio_ucst_get_sdu_interval(false);
            param.sdu_interval_s_to_m = app_le_audio_ucst_get_sdu_interval(true);

        } else {
            param.sdu_interval_m_to_s = app_le_audio_ucst_get_sdu_interval(false);
            param.sdu_interval_s_to_m = app_le_audio_ucst_get_sdu_interval(false);
        }

        g_lea_ucst_ctrl.cig_id = param.cig_id;
        ret = bt_gap_le_set_cig_parameters_test(&param);
        LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters_test, ret:%x cig_id:%x cis_num:%x", 3, ret, g_lea_ucst_ctrl.cig_id, cis_num);

        vPortFree(p_cis);

    } else {

        /* Use set CIG parameters command */
        bt_gap_le_cis_params_t *p_cis = NULL;

        if (NULL == (p_cis = (bt_gap_le_cis_params_t *)pvPortMalloc(sizeof(bt_gap_le_cis_params_t) * cis_num))) {
            LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters, malloc failed!", 0);
            return BT_STATUS_OUT_OF_MEMORY;
        }

        memset(p_cis, 0, (sizeof(bt_gap_le_cis_params_t)*cis_num));

        for (i = 0; i < cis_num; i++) {
            if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
                p_cis[i].cis_id = ((0 == i) ? APP_LE_AUDIO_CIS_ID_3_CALL : APP_LE_AUDIO_CIS_ID_4_CALL);
                p_cis[i].rtn_m_to_s = g_lea_ucst_qos_params_spk_0.rtn;
                p_cis[i].rtn_s_to_m = g_lea_ucst_qos_params_mic_0.rtn;
                if (0 == sink_ase_num) {
                    p_cis[i].max_sdu_m_to_s = 0x0000;
                    LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters 1, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
                } else {
                    p_cis[i].max_sdu_m_to_s = g_lea_ucst_qos_params_spk_0.sdu_size;
                    LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters 2, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
                }

                if ((0 == source_ase_num) ||
                    ((1 == group_size) && (1 == source_ase_num) && (0 != (i % 2)))) {
                    LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters 3, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
                    p_cis[i].max_sdu_s_to_m = 0x0000;
                } else {
                    LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters 4, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
                    p_cis[i].max_sdu_s_to_m = g_lea_ucst_qos_params_mic_0.sdu_size;
                }

            } else {
                p_cis[i].cis_id = ((0 == i) ? APP_LE_AUDIO_CIS_ID_1_MEDIA : APP_LE_AUDIO_CIS_ID_2_MEDIA);
                LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters 5, cis_id[%x]:%x", 2, i, p_cis[i].cis_id);
                p_cis[i].rtn_m_to_s = g_lea_ucst_qos_params_spk_1.rtn;
                p_cis[i].rtn_s_to_m = g_lea_ucst_qos_params_spk_1.rtn;
                p_cis[i].max_sdu_m_to_s = g_lea_ucst_qos_params_spk_1.sdu_size;
                p_cis[i].max_sdu_s_to_m = 0x0000;
            }

            p_cis[i].phy_m_to_s = 0x02;
            p_cis[i].phy_s_to_m = 0x02;
        }

        bt_gap_le_set_cig_params_t param = {
            .cig_id = APP_LE_AUDIO_CIG_ID_1,
            .sca = 0x00,
            .packing = BT_GAP_LE_CIG_PACKING_INTERLEAVED,
            .framing = BT_GAP_LE_CIG_FRAMING_UNFRAMED,
            .cis_list = p_cis,
            .cis_count = cis_num,
        };

        if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
            param.cig_id = APP_LE_AUDIO_CIG_ID_2;
            param.sdu_interval_m_to_s = app_le_audio_ucst_get_sdu_interval(false);
            param.sdu_interval_s_to_m = app_le_audio_ucst_get_sdu_interval(true);
            param.max_transport_latency_m_to_s = g_lea_ucst_qos_params_spk_0.latency;
            param.max_transport_latency_s_to_m = g_lea_ucst_qos_params_mic_0.latency;

        } else {
            param.sdu_interval_m_to_s = app_le_audio_ucst_get_sdu_interval(false);
            param.sdu_interval_s_to_m = app_le_audio_ucst_get_sdu_interval(false);
            param.max_transport_latency_m_to_s = g_lea_ucst_qos_params_spk_1.latency;
            param.max_transport_latency_s_to_m = g_lea_ucst_qos_params_spk_1.latency;
        }

        g_lea_ucst_ctrl.cig_id = param.cig_id;

        ret = bt_gap_le_set_cig_parameters(&param);
        LE_AUDIO_MSGLOG_I("[APP] set_cig_parameters, ret:%x cig_id:%x cis_num:%x", 3, ret, g_lea_ucst_ctrl.cig_id, cis_num);

        vPortFree(p_cis);
    }

    return ret;
}

static bt_status_t app_le_audio_ucst_create_cis(void)
{
    app_le_audio_ucst_link_info_t *p_info;
    bt_status_t ret;
    uint8_t i, tmp;
    bt_gap_le_cis_set_t hdl_list[APP_LE_AUDIO_UCST_CIS_MAX_NUM] = {{0}, {0}};
    bt_gap_le_create_cis_t param = {
        .cis_count = 0x00,
        .cis_list = hdl_list,
    };

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= g_lea_ucst_ctrl.curr_group) {
        LE_AUDIO_MSGLOG_I("[APP][U] create_cis, no active group!", 0);
        return BT_STATUS_FAIL;
    }
#endif

    LE_AUDIO_MSGLOG_I("[APP][U] create_cis, cig:%x total_cis_num:%x", 2, g_lea_ucst_ctrl.cig_id, g_lea_ucst_ctrl.cis_num);

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        p_info = &g_lea_ucst_link_info[i];

        if ((BT_HANDLE_INVALID != p_info->handle) &&
            (APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS == p_info->next_state)) {

            LE_AUDIO_MSGLOG_I("[APP][U] create_cis, handle:%x state:%x->%x cis_num:%x location:(%x %x)", 6,
                              p_info->handle,
                              p_info->curr_state, p_info->next_state,
                              p_info->cis_num,
                              p_info->sink_location,
                              p_info->source_location);

            if (!p_info->sink_location) {
                LE_AUDIO_MSGLOG_E("[APP][U] ERROR! sink_location shall not be empty! handle:%x", 1, p_info->handle);
            }

            if (APP_LE_AUDIO_UCST_CIS_MAX_NUM == p_info->cis_num) {
                /* create 2 CIS in 1 LE link */
                LE_AUDIO_MSGLOG_I("[APP][U] create_cis, 0", 0);
                hdl_list[0].acl_connection_handle = p_info->handle;
                hdl_list[0].cis_connection_handle = g_lea_ucst_cis_info[0].cis_handle; /* Left */
                g_lea_ucst_cis_info[0].acl_handle = p_info->handle;
                g_lea_ucst_cis_info[0].cis_status = APP_LE_AUDIO_UCST_CIS_CREATING;

                hdl_list[1].acl_connection_handle = p_info->handle;
                hdl_list[1].cis_connection_handle = g_lea_ucst_cis_info[1].cis_handle; /* Right */
                g_lea_ucst_cis_info[1].acl_handle = p_info->handle;
                g_lea_ucst_cis_info[1].cis_status = APP_LE_AUDIO_UCST_CIS_CREATING;
                param.cis_count = 2;
                break;
            }

            LE_AUDIO_MSGLOG_I("[APP][U] create_cis, cis_status:%x %x", 2,
                              g_lea_ucst_cis_info[0].cis_status,
                              g_lea_ucst_cis_info[1].cis_status);

            if ((1 == g_lea_ucst_ctrl.cis_num) &&
                ((AUDIO_LOCATION_FRONT_RIGHT== p_info->sink_location) ||
                 (AUDIO_LOCATION_FRONT_RIGHT == p_info->source_location))) {
                g_lea_ucst_cis_info[0].cis_status = APP_LE_AUDIO_UCST_CIS_CREATING;
                g_lea_ucst_cis_info[0].acl_handle = p_info->handle;
                hdl_list[param.cis_count].acl_connection_handle = p_info->handle;
                hdl_list[param.cis_count].cis_connection_handle = g_lea_ucst_cis_info[0].cis_handle;
                LE_AUDIO_MSGLOG_I("[APP][U] create_cis, 3", 0);
                param.cis_count++;
            } else {
                /* create 1 CIS in 1 LE link */
                if ((APP_LE_AUDIO_UCST_CIS_IDLE == g_lea_ucst_cis_info[0].cis_status) &&
                    (((0 == p_info->sink_location_num) && (0 == p_info->source_location_num)) ||
                    (AUDIO_LOCATION_FRONT_LEFT == p_info->sink_location) ||
                    (AUDIO_LOCATION_FRONT_LEFT == p_info->source_location))) {
                    g_lea_ucst_cis_info[0].cis_status = APP_LE_AUDIO_UCST_CIS_CREATING;
                    g_lea_ucst_cis_info[0].acl_handle = p_info->handle;
                    hdl_list[param.cis_count].acl_connection_handle = p_info->handle;
                    hdl_list[param.cis_count].cis_connection_handle = g_lea_ucst_cis_info[0].cis_handle;
                    LE_AUDIO_MSGLOG_I("[APP][U] create_cis, 1", 0);
                    param.cis_count++;

                } else if ((APP_LE_AUDIO_UCST_CIS_IDLE == g_lea_ucst_cis_info[1].cis_status) &&
                    ((AUDIO_LOCATION_FRONT_RIGHT== p_info->sink_location) ||
                    (AUDIO_LOCATION_FRONT_RIGHT == p_info->source_location))) {
                    g_lea_ucst_cis_info[1].cis_status = APP_LE_AUDIO_UCST_CIS_CREATING;
                    g_lea_ucst_cis_info[1].acl_handle = p_info->handle;
                    hdl_list[param.cis_count].acl_connection_handle = p_info->handle;
                    hdl_list[param.cis_count].cis_connection_handle = g_lea_ucst_cis_info[1].cis_handle;
                    LE_AUDIO_MSGLOG_I("[APP][U] create_cis, 2", 0);
                    param.cis_count++;
                }
            }
        }
    }

    LE_AUDIO_MSGLOG_I("[APP] create_cis, cis_count:%x [0](acl:%x cis:%x) [1](acl:%x cis:%x)", 5,
                      param.cis_count,
                      hdl_list[0].acl_connection_handle,
                      hdl_list[0].cis_connection_handle,
                      hdl_list[1].acl_connection_handle,
                      hdl_list[1].cis_connection_handle);

    ret = bt_gap_le_create_cis(&param);
    LE_AUDIO_MSGLOG_I("[APP] create_cis, ret:%x", 1, ret);

    return ret;
}

static bt_status_t app_le_audio_ucst_disconnect_cis(bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t cis_idx, i;
    bool pending_action = false;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        LE_AUDIO_MSGLOG_I("[APP][U] disconnect_cis, link not exist (hdl:%x)", 1, handle);
        return BT_STATUS_FAIL;
    }

    i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
    cis_idx = APP_LE_AUDIO_UCST_CIS_MAX_NUM;

    LE_AUDIO_MSGLOG_I("[APP][U] disconnect_cis, handle:%x state:%x->%x cis_num:%x", 4,
                      p_info->handle,
                      p_info->curr_state, p_info->next_state,
                      p_info->cis_num);

    while (i > 0) {
        i--;
        if (BT_HANDLE_INVALID != g_lea_ucst_cis_info[i].cis_handle) {
            LE_AUDIO_MSGLOG_I("[APP] disconnect_cis, check handle:%x cis_handle[%x]:%x cis_status:%x", 4,
                              g_lea_ucst_cis_info[i].acl_handle,
                              i,
                              g_lea_ucst_cis_info[i].cis_handle,
                              g_lea_ucst_cis_info[i].cis_status);
            if (p_info->handle == g_lea_ucst_cis_info[i].acl_handle) {
                g_lea_ucst_cis_info[i].cis_status = APP_LE_AUDIO_UCST_CIS_DISCONNECTING;
                cis_idx = i;

            } else if (APP_LE_AUDIO_UCST_CIS_DISCONNECTING == g_lea_ucst_cis_info[i].cis_status) {
                pending_action = true;
            }
        }
    }

    if (APP_LE_AUDIO_UCST_CIS_MAX_NUM == cis_idx) {
        LE_AUDIO_MSGLOG_I("[APP][U] disconnect_cis, cis not found!", 0);
        return BT_STATUS_FAIL;
    }

    if (pending_action) {
        LE_AUDIO_MSGLOG_I("[APP][U] disconnect_cis, pending_action:%x", 1, pending_action);
        return BT_STATUS_SUCCESS;
    }

    return app_le_audio_ucst_disconnect(g_lea_ucst_cis_info[cis_idx].cis_handle);
}

static bt_status_t app_le_audio_ucst_remove_cig(void)
{
    bt_status_t ret;
    bt_gap_le_remove_cig_t param = {
        .cig_id = g_lea_ucst_ctrl.cig_id
    };

    ret = bt_gap_le_remove_cig(&param);

    LE_AUDIO_MSGLOG_I("[APP][U] remove_cig, ret:%x cig_id:%x", 2, ret, g_lea_ucst_ctrl.cig_id);

    return ret;
}

bool app_le_audio_ucst_check_pause_stream(void)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t i, tmp;
    app_le_audio_ucst_lock_stream_t lock_stream = APP_LE_AUDIO_UCST_LCOK_STREAM_NONE;

    if  (APP_LE_AUDIO_UCST_PAUSE_STREAM_ALL <= g_lea_ucst_ctrl.pause_stream) {
        return true;
    }

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= g_lea_ucst_ctrl.curr_group) {
            LE_AUDIO_MSGLOG_I("[APP][U] check_pause_stream, curr_group(%x) not exist", 1, g_lea_ucst_ctrl.curr_group);
            break;
        }
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        p_info = &g_lea_ucst_link_info[i];
        if (BT_HANDLE_INVALID != p_info->handle) {
            LE_AUDIO_MSGLOG_I("[APP][U] check_pause_stream, handle:%x lock:%x", 2,
                              p_info->handle,
                              p_info->lock_stream);

            if (lock_stream < p_info->lock_stream) {
                lock_stream = p_info->lock_stream;
            }
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] check_pause_stream, p:%x lock_stream:%x", 2,
                      g_lea_ucst_ctrl.pause_stream,
                      lock_stream);

    if (APP_LE_AUDIO_UCST_LCOK_STREAM_ALL <= lock_stream) {
        return true;
    }

    if ((APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
        (APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == g_lea_ucst_ctrl.next_target)) {
        if ((APP_LE_AUDIO_UCST_PAUSE_STREAM_UNIDIRECTIONAL <= g_lea_ucst_ctrl.pause_stream) ||
            (APP_LE_AUDIO_UCST_LCOK_STREAM_UNIDIRECTIONAL <= lock_stream)) {
            return true;
        }
    }

    return false;
}

static bool app_le_audio_ucst_check_close_audio_stream(void)
{
    uint8_t i, tmp;

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) {
            if (!((APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC > g_lea_ucst_link_info[i].curr_state) ||
                  (((APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC == g_lea_ucst_link_info[i].curr_state) ||
                    (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS == g_lea_ucst_link_info[i].curr_state)) &&
                   (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == g_lea_ucst_link_info[i].next_state) &&
                   (0 == g_lea_ucst_link_info[i].wait_event.wait_ase_event)))) {
                LE_AUDIO_MSGLOG_I("[APP][ASE] close_audio_stream, check handle:%x state:%x->%x w_ase:%x r:%x", 5,
                                  g_lea_ucst_link_info[i].handle,
                                  g_lea_ucst_link_info[i].curr_state, g_lea_ucst_link_info[i].next_state,
                                  g_lea_ucst_link_info[i].wait_event.wait_ase_event,
                                  g_lea_ucst_link_info[i].ase_releasing);
                return false;
            }
        }
    }
    if (0 != app_le_audio_ucst_get_cis_num()) {
        return false;
    }
    /* stop audio stream */
    if ((APP_LE_AUDIO_UCST_STREAM_STATE_IDLE < g_lea_ucst_ctrl.curr_stream_state) &&
        (APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING >= g_lea_ucst_ctrl.curr_stream_state) &&
        (APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM != g_lea_ucst_ctrl.next_stream_state)) {
        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
        if (BT_STATUS_SUCCESS == app_le_audio_close_audio_transmitter()) {
            return true;
        }
        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
    }
    return false;
}

static void app_le_audio_ucst_check_set_ase(uint8_t link_idx)
{
    if (((APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC == g_lea_ucst_link_info[link_idx].curr_state) ||
         (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS == g_lea_ucst_link_info[link_idx].curr_state)) &&
        (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == g_lea_ucst_link_info[link_idx].next_state) &&
        (0 == g_lea_ucst_link_info[link_idx].wait_event.wait_ase_event)) {

        uint8_t ase_idx = APP_LE_AUDIO_UCST_ASE_IDX_0;

        LE_AUDIO_MSGLOG_I("[APP][U] check_set_ase", 0);

        if (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) {
            ase_idx = g_lea_ucst_link_info[link_idx].source_ase_idx;
        }

        if (ASE_STATE_QOS_CONFIGURED == g_lea_ucst_link_info[link_idx].ase[ase_idx].curr_state) {
            g_lea_ucst_link_info[link_idx].curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
            g_lea_ucst_link_info[link_idx].next_state = APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE;
            if (BT_STATUS_SUCCESS != app_le_audio_ucst_enable_ase(g_lea_ucst_link_info[link_idx].handle)) {
                g_lea_ucst_link_info[link_idx].next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            }

        } else if (ASE_STATE_CODEC_CONFIGURED == g_lea_ucst_link_info[link_idx].ase[ase_idx].curr_state) {
            g_lea_ucst_link_info[link_idx].curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC;
            g_lea_ucst_link_info[link_idx].next_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
            if (BT_STATUS_SUCCESS != app_le_audio_ucst_config_qos(g_lea_ucst_link_info[link_idx].handle)) {
                g_lea_ucst_link_info[link_idx].next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            }
        }
    }
}

bool app_le_audio_ucst_check_delete_group_device(void)
{
    app_le_audio_ucst_bonded_device_t *p_device = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    bt_addr_t addr = {0};
    uint8_t j, link_idx;
    bool rm_white_list = false;

    for (j = 0; j < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; j++) {
        p_device = &g_lea_ucst_bonded_list.device[j];

        if (!p_device->deleting) {
            continue;
        }

        link_idx = p_device->link_idx;

        if (p_device->in_white_list) {
            p_device->in_white_list = false;
            rm_white_list = true;
            memcpy((uint8_t *)&addr, (uint8_t *)&(p_device->addr), sizeof(bt_addr_t));
        }

        /* handle bonded list */
        app_le_audio_ucst_reset_bonded_list(j);
        app_le_audio_ucst_refresh_bonded_list(j);
        g_lea_ucst_bonded_list.num--;

        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM > link_idx) {
            p_info = &g_lea_ucst_link_info[link_idx];
            if (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL != p_info->next_state) {
                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL;
                if (BT_STATUS_SUCCESS == app_le_audio_ucst_disconnect(p_info->handle)) {
                    g_lea_ucst_waiting_disconn_flag = true;
                } else {
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                }
            }
        }

        if (rm_white_list) {
            /* rm from white list */
            LE_AUDIO_MSGLOG_I("[APP] check_del_group, set_white_list.state:%x", 1, g_lea_ucst_set_white_list.state);
            g_lea_ucst_set_white_list.state = APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING;
            app_le_audio_ucst_remove_white_list(&addr);
        }

        if ((APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING == g_lea_ucst_set_white_list.state) ||
            (true == g_lea_ucst_waiting_disconn_flag)) {
            //app_le_audio_ucst_write_bonded_list_to_nvkey();
            return true;
        }
    }

    return false;
}

bt_status_t app_le_audio_ucst_check_adv_data(bt_gap_le_ext_advertising_report_ind_t *ind)
{
    bt_bd_addr_t empty_addr = {0};
    uint16_t length = 0, index = 0;

    if (!(ind->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_CONNECTABLE)) {
        /* ignore non-connectable adv */
        //LE_AUDIO_MSGLOG_I("[APP][U] check_adv_data, ignore non-connectable adv", 0);
        return BT_STATUS_FAIL;
    }

    /* check address */
    if ((0 == memcmp(ind->address.addr, empty_addr, sizeof(bt_bd_addr_t))) ||
        ((ind->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED) &&
         (0 == memcmp(ind->direct_address.addr, empty_addr, sizeof(bt_bd_addr_t))))) {
        //LE_AUDIO_MSGLOG_I("[APP][U] check_adv_data, event_type:%x invalid addr!", 1, ind->event_type);
        return BT_STATUS_FAIL;
    }

    if (app_le_audio_ucst_is_connected_device(&ind->address)) {
        /* ignore connected device */
        //LE_AUDIO_MSGLOG_I("[APP][U] check_adv_data, ignore connected device", 0);
        return BT_STATUS_FAIL;
    }

    if (ind->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED) {
        if (!app_le_audio_ucst_is_bonded_device(&ind->address)) {
            /* ignore unknown device */
            //LE_AUDIO_MSGLOG_I("[APP][U] check_adv_data, ignore unknown device", 0);
            return BT_STATUS_FAIL;
        }

        return BT_STATUS_SUCCESS;
    }

    if (!app_le_audio_ucst_is_lea_adv(ind)) {
        return BT_STATUS_FAIL;
    }

    while (index < ind->data_length) {
        length = ind->data[index];

        if (0 == length) {
            break;
        }

        if ((length >= 7) && ((ind->data[index + 1] == 0xF0) || (ind->data[index + 1] == 0x2E))) {
            if (BT_STATUS_SUCCESS != ble_csip_verify_rsi(&ind->data[index + 2])) {
                LE_AUDIO_MSGLOG_I("[APP] csip_verify_rsi fail 1", 0);
                return BT_STATUS_FAIL;
            }

            app_le_audio_ucst_print_lea_adv_addr(&ind->address);
            return BT_STATUS_SUCCESS;
        }

        if ((length >= 10) && ((ind->data[index + 1] == 0xFF) && (ind->data[index + 2] == 0x94) && (ind->data[index + 5] == 0x2E))) {
            if (BT_STATUS_SUCCESS != ble_csip_verify_rsi(&ind->data[index + 6])) {
                LE_AUDIO_MSGLOG_I("[APP] csip_verify_rsi fail 2", 0);
                return BT_STATUS_FAIL;
            }
            app_le_audio_ucst_print_lea_adv_addr(&ind->address);
            return BT_STATUS_SUCCESS;
        }
        index = index + length + 1;
    }

    return BT_STATUS_FAIL;
}

void app_le_audio_ucst_delete_group_from_bonded_list(bt_addr_t *addr)
{
    app_le_audio_ucst_bonded_device_t *p_device = NULL;
    uint8_t i = 0;

    /* search device */
    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
        p_device = &g_lea_ucst_bonded_list.device[i];
        if (0 == memcmp(&(p_device->addr), addr, sizeof(bt_addr_t))) {
            break;
        }
    }

    if (APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM == i) {
        LE_AUDIO_MSGLOG_I("[APP][U] del_group, device not found", 0);
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] del_group, [%x] link_idx:%x white_list:%x group:%x(size:%x)", 5, i,
                      p_device->link_idx,
                      p_device->in_white_list,
                      p_device->group_id,
                      p_device->group_size);

    bt_device_manager_le_remove_bonded_device(&(p_device->addr));

    if (1 < p_device->group_size) {
        app_le_audio_ucst_bonded_device_t *p_device_mate = NULL;
        uint8_t j = 0;

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        for (j = 0; j < APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM; j ++) {
            if (p_device->group_id == g_lea_ucst_sirk_info[j].group_id) {
                g_lea_ucst_sirk_info[j].group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
                memset((uint8_t *)&(g_lea_ucst_sirk_info[j].sirk), 0, sizeof(bt_key_t));
                app_le_audio_ucst_write_sirk_list_to_nvkey();
                break;
            }
        }
#endif
        if (APP_LE_AUDIO_UCST_CONN_COORDINATED_SET_BY_SIRK <= g_lea_ucst_ctrl.curr_conn) {
            app_le_audio_ucst_group_info_t *p_group = NULL;
            if (NULL != (p_group = app_le_audio_ucst_get_group_info(p_device->group_id))) {
                if (0 == memcmp((bt_key_t *)&g_le_audio_sirk, (bt_key_t *)&(p_group->sirk), sizeof(bt_key_t))) {
                    app_le_audio_ucst_stop_scan_all();
                }
            }
        }

        /* search group mate */
        for (j = 0; j < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; j++) {
            if (i == j) {
                continue;
            }

            p_device_mate = &g_lea_ucst_bonded_list.device[j];

            if (p_device->group_id != p_device_mate->group_id) {
                continue;
            }

            LE_AUDIO_MSGLOG_I("[APP][U] del_group mate, [%x] link_idx:%x white_list:%x group:%x(size:%x)", 5, j,
                              p_device_mate->link_idx,
                              p_device_mate->in_white_list,
                              p_device_mate->group_id,
                              p_device_mate->group_size);

            p_device_mate->deleting = true;
            p_device_mate->group_id |= APP_LE_AUDIO_UCST_BONDED_RECORD_INVALID;

            bt_device_manager_le_remove_bonded_device(&(p_device_mate->addr));
        }
    }

    uint8_t link_idx;
    bool rm_white_list = false;

    link_idx = p_device->link_idx;

    /* reset group info */
    app_le_audio_ucst_reset_group_info(p_device->group_id);

    if (p_device->in_white_list) {
        p_device->in_white_list = false;
        rm_white_list = true;
    }

    /* handle bonded list */
    LE_AUDIO_MSGLOG_I("[APP] del_group, i:%x", 1, i);
    app_le_audio_ucst_reset_bonded_list(i);
    app_le_audio_ucst_refresh_bonded_list(i);
    g_lea_ucst_bonded_list.num--;

    app_le_audio_ucst_write_bonded_list_to_nvkey();

    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM > link_idx) {
        app_le_audio_ucst_link_info_t *p_info = NULL;

        p_info = &g_lea_ucst_link_info[link_idx];
        if (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL != p_info->next_state) {
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL;

            if (BT_STATUS_SUCCESS == app_le_audio_ucst_disconnect(p_info->handle)) {
                g_lea_ucst_waiting_disconn_flag = true;
            } else {
                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            }
        }
    }

    if (rm_white_list) {
        /* rm from white list */
        LE_AUDIO_MSGLOG_I("[APP] del_group, set_white_list.state:%x", 1, g_lea_ucst_set_white_list.state);
        g_lea_ucst_set_white_list.state = APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING;
        app_le_audio_ucst_remove_white_list(addr);
    }

    LE_AUDIO_MSGLOG_I("[APP] del_group, set_white_list.state:%x disconn:%x", 2,
                      g_lea_ucst_set_white_list.state,
                      g_lea_ucst_waiting_disconn_flag);

    if ((1 == p_device->group_size) ||
        (APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING == g_lea_ucst_set_white_list.state) ||
        (true == g_lea_ucst_waiting_disconn_flag)) {
        return;
    }

    app_le_audio_ucst_check_delete_group_device();
}

static void app_le_audio_ucst_delete_oldest_group_from_bonded_list(void)
{
    app_le_audio_ucst_bonded_device_t *p_device = NULL;
    uint8_t i;

    /* free oldest bonded group with no device connected, and also remove device from white list */
    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {

        p_device = &g_lea_ucst_bonded_list.device[i];

        LE_AUDIO_MSGLOG_I("[APP][U] del_oldest_group, [%x] link_idx:%x white_list:%x group:%x(size:%x)", 5, i,
                          p_device->link_idx,
                          p_device->in_white_list,
                          p_device->group_id,
                          p_device->group_size);

        if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= p_device->group_id) {
            LE_AUDIO_MSGLOG_I("[APP] del_oldest_group, [%x] invalid group:%x", 2, i, p_device->group_id);
            continue;
        }

        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM > p_device->link_idx) {
            continue;
        }

        if (0 != app_le_audio_ucst_get_group_link_num(p_device->group_id)) {
            /* any device in the group is connected, don't remove */
            continue;
        }

        bt_device_manager_le_remove_bonded_device(&(p_device->addr));

        if (1 < p_device->group_size) {
            app_le_audio_ucst_bonded_device_t *p_device_mate = NULL;
            uint8_t j = 0;

            /* search group mate */
            for (j = 0; j < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; j++) {
                if (i == j) {
                    continue;
                }

                p_device_mate = &g_lea_ucst_bonded_list.device[j];

                if (p_device->group_id != p_device_mate->group_id) {
                    continue;
                }

                LE_AUDIO_MSGLOG_I("[APP][U] del_oldest_group mate, [%x] link_idx:%x white_list:%x group:%x(size:%x)", 5, j,
                                  p_device_mate->link_idx,
                                  p_device_mate->in_white_list,
                                  p_device_mate->group_id,
                                  p_device_mate->group_size);

                p_device_mate->deleting = true;
                p_device_mate->group_id |= APP_LE_AUDIO_UCST_BONDED_RECORD_INVALID;

                bt_device_manager_le_remove_bonded_device(&(p_device_mate->addr));
            }
        }

        bt_addr_t addr = {0};
        bool rm_white_list = false;

        /* reset group info */
        app_le_audio_ucst_reset_group_info(p_device->group_id);

        if (p_device->in_white_list) {
            p_device->in_white_list = false;
            rm_white_list = true;
            memcpy((uint8_t *)&addr, (uint8_t *)&(p_device->addr), sizeof(bt_addr_t));
        }

        /* handle bonded list */
        LE_AUDIO_MSGLOG_I("[APP] del_oldest_group, i:%x", 1, i);
        app_le_audio_ucst_reset_bonded_list(i);
        app_le_audio_ucst_refresh_bonded_list(i);
        g_lea_ucst_bonded_list.num--;

        app_le_audio_ucst_write_bonded_list_to_nvkey();

        if (rm_white_list) {
            /* rm from white list */
            LE_AUDIO_MSGLOG_I("[APP] del_oldest_group, set_white_list.state:%x", 1, g_lea_ucst_set_white_list.state);
            g_lea_ucst_set_white_list.state = APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING;
            app_le_audio_ucst_remove_white_list(&addr);
        }

        LE_AUDIO_MSGLOG_I("[APP] del_oldest_group, set_white_list.state:%x disconn:%x", 2,
                          g_lea_ucst_set_white_list.state,
                          g_lea_ucst_waiting_disconn_flag);

        if ((1 == p_device->group_size) ||
            (APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING == g_lea_ucst_set_white_list.state) ||
            (true == g_lea_ucst_waiting_disconn_flag)) {
            return;
        }

        app_le_audio_ucst_check_delete_group_device();
    }
}


static void app_le_audio_ucst_disable_ase_when_setup_iso_data_path(void)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t i, tmp;

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        p_info = &g_lea_ucst_link_info[i];

        if (BT_HANDLE_INVALID != p_info->handle) {

            if ((((APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH == p_info->curr_state) ||
                  (APP_LE_AUDIO_UCST_LINK_STATE_STREAMING == p_info->curr_state)) &&
                 (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == p_info->next_state)) ||
                (APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS == p_info->curr_state)) {

                LE_AUDIO_MSGLOG_I("[APP] disable_ase_when_setup_iso_data_path, handle:%x state:%x->%x", 3,
                                  p_info->handle,
                                  p_info->curr_state,
                                  p_info->next_state);

                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE;
                if (BT_STATUS_SUCCESS != app_le_audio_ucst_disable_ase(p_info->handle)) {
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                }
            }
        }
    }
}

static bool app_le_audio_ucst_disable_ase_when_state_match_streaming(bool check_release)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t i, tmp;
    uint8_t wait_ase_num = 0;

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        p_info = &g_lea_ucst_link_info[i];

        if (BT_HANDLE_INVALID != p_info->handle) {

            LE_AUDIO_MSGLOG_I("[APP][U] disable_ase_when_state_match_streaming, handle:%x state:%x->%x w_ase:%x check_release:%x", 5,
                              p_info->handle,
                              p_info->curr_state,
                              p_info->next_state,
                              p_info->wait_event.wait_ase_event,
                              check_release);

            if ((APP_LE_AUDIO_UCST_LINK_STATE_STREAMING == p_info->curr_state) &&
                (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == p_info->next_state)) {

                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE;
                wait_ase_num++;

                if (BT_STATUS_SUCCESS != app_le_audio_ucst_disable_ase(p_info->handle)) {
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                    wait_ase_num--;
                }
            } else if (check_release && p_info->ase_releasing) {
                wait_ase_num++;
            }
        }
    }

    if (0 != wait_ase_num) {
        return true;
    }

    return false;
}

static void app_le_audio_ucst_disable_ase_when_state_match(app_le_audio_ucst_link_state_t target_state)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t i, tmp;

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        p_info = &g_lea_ucst_link_info[i];

        if ((BT_HANDLE_INVALID != p_info->handle) &&
            (target_state == p_info->curr_state) &&
            (APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE != p_info->next_state)) {

            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE;

            LE_AUDIO_MSGLOG_I("[APP] disable_ase_when_state_match, handle:%x state:%x->%x target_state:%x", 4,
                              p_info->handle,
                              p_info->curr_state,
                              p_info->next_state,
                              target_state);

            if (BT_STATUS_SUCCESS != app_le_audio_ucst_disable_ase(p_info->handle)) {
                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            }
        }
    }
}

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
static void app_le_audio_ucst_disconnect_other_group_device(uint8_t keep_group_id, uint8_t target_device_location)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    app_le_audio_ucst_link_state_t next_state;
    bt_status_t ret;
    uint8_t i;

    for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
        p_info = &g_lea_ucst_link_info[i];

        LE_AUDIO_MSGLOG_I("[APP] disconnect_other_group_device, handle:%x group:%x(%x) sink_location:%x(%x)", 5,
                          p_info->handle,
                          p_info->group_id,
                          keep_group_id,
                          p_info->sink_location,
                          target_device_location);

        if ((BT_HANDLE_INVALID != p_info->handle) &&
            (APP_LE_AUDIO_UCST_GROUP_ID_MAX > p_info->group_id) &&
            (keep_group_id != p_info->group_id) &&
            ((target_device_location == p_info->sink_location) || (AUDIO_LOCATION_NONE == p_info->sink_location))) {
            /* if p_info->sink_location == AUDIO_LOCATION_NONE, service discovery not complete */
            next_state = g_lea_ucst_link_info[i].next_state;
            g_lea_ucst_link_info[i].next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL;
            if (BT_STATUS_SUCCESS == (ret = app_le_audio_ucst_disconnect(p_info->handle))) {
                g_lea_ucst_waiting_conn_flag = true;
            } else {
                g_lea_ucst_waiting_conn_flag = false;
                g_lea_ucst_link_info[i].next_state = next_state;
            }

            LE_AUDIO_MSGLOG_I("[APP] disconnect_other_group_device, ret:%x handle:%x group:%x(%x) sink_location:%x(%x)", 6,
                              ret,
                              p_info->handle,
                              p_info->group_id,
                              keep_group_id,
                              p_info->sink_location,
                              target_device_location);
            break;
        }
    }
}
#endif

static void app_le_audio_ucst_handle_ase_codec_configured(ble_bap_ase_notify_t *p_notify)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(p_notify->handle))) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASE] CODEC_CONFIGURED, handle:%x state:%x->%x ase_id:%x w_ase:%x r:%x", 6,
                      p_info->handle,
                      p_info->curr_state, p_info->next_state,
                      p_notify->ase_id,
                      p_info->wait_event.wait_ase_event,
                      p_info->ase_releasing);

    /* update ase state */
    uint8_t ase_idx;
    if (APP_LE_AUDIO_UCST_ASE_MAX_NUM == (ase_idx = app_le_audio_ucst_get_ase_idx(p_info, p_notify->ase_id))) {
        return;
    }
    p_info->ase[ase_idx].curr_state = ASE_STATE_CODEC_CONFIGURED;

    if (p_info->ase_releasing) {
        if (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC == p_info->next_state) {
            /* handle ASE release */
            if (p_info->wait_event.wait_ase_event > 0) {
                p_info->wait_event.wait_ase_event--;

                if (0 != p_info->wait_event.wait_ase_event) {
                    return;
                }

                p_info->ase_releasing = false;
                p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC;
                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                app_le_audio_ucst_check_close_audio_stream();
            }
        }
        return;
    }

    switch (p_info->curr_state) {
        case APP_LE_AUDIO_UCST_LINK_STATE_READ_ASE: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC == p_info->next_state) {

                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    char conn_string[60] = {0};

                    snprintf((char *)conn_string, 50, "Device config complete! handle:0x%04x\r\n", p_info->handle);
                    bt_app_common_at_cmd_print_report(conn_string);

                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC;
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;

                    LE_AUDIO_MSGLOG_I("[APP][U] Device config complete! handle:%x target:%x->%x stream_state:%x->%x p:%x r:%x", 7,
                                      p_info->handle,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    if (((APP_LE_AUDIO_CONN_INTERVAL_STREAMING != p_info->curr_interval) &&
                         (APP_LE_AUDIO_CONN_INTERVAL_STREAMING != p_info->next_interval)) ||
                        ((APP_LE_AUDIO_CONN_INTERVAL_STREAMING == p_info->curr_interval) &&
                         (APP_LE_AUDIO_CONN_INTERVAL_STREAMING != p_info->next_interval) &&
                         (0 != p_info->next_interval))) {
                        LE_AUDIO_MSGLOG_I("[APP][U] update_conn_interval, handle:%x interval:%x->%x set:30", 3, p_info->handle,
                                          p_info->curr_interval, p_info->next_interval);
                        p_info->next_interval = APP_LE_AUDIO_CONN_INTERVAL_STREAMING;
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_update_connection_parameter(p_info->handle, APP_LE_AUDIO_CONN_INTERVAL_STREAMING, 0x0002)) {
                            p_info->next_interval = 0;
                        }
                    }

                    if (app_le_audio_ucst_check_pause_stream() || (g_lea_ucst_ctrl.release) ||
#ifdef AIR_SILENCE_DETECTION_ENABLE
                        (APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL == app_le_audio_silence_detection_get_silence_detection_mode()) ||
#endif
                        (((APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
                          (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) &&
                         (APP_LE_AUDIO_UCST_TARGET_NONE == g_lea_ucst_ctrl.next_target))) {
                        break;
                    }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                    LE_AUDIO_MSGLOG_I("[APP] codec_config, group:%x active:%x->%x latest:%x", 4,
                                      p_info->group_id,
                                      g_lea_ucst_ctrl.curr_group,
                                      g_lea_ucst_ctrl.next_group,
                                      g_lea_ucst_ctrl.latest_group);
                    if (!(((APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.curr_group) &&
                           (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.next_group) &&
                           (p_info->group_id == g_lea_ucst_ctrl.curr_group)) ||
                          ((APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.curr_group) &&
                           (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.latest_group) &&
                           (p_info->group_id == g_lea_ucst_ctrl.latest_group)))) {
                        break;
                    }
#endif
                    if ((APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state) &&
                        (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {
                        /* check usb state and start stream */
                        g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                        g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                        app_le_audio_ucst_start();
                        break;
                    }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                    if ((p_info->group_id != g_lea_ucst_ctrl.curr_group) ||
                        (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= g_lea_ucst_ctrl.curr_group)) {
                        break;
                    }
#endif

                    if ((APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
                        (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                        if (((APP_LE_AUDIO_UCST_STREAM_STATE_SET_CIG_PARAMETER == g_lea_ucst_ctrl.curr_stream_state) ||
                             (APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING == g_lea_ucst_ctrl.curr_stream_state)) &&
                            (APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM != g_lea_ucst_ctrl.next_stream_state)) {
                            /* continue to config ase */
                            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
                            if (BT_STATUS_SUCCESS != app_le_audio_ucst_config_qos(p_info->handle)) {
                                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                            }
                            /* check group mate link (for after ase release) */
                            uint8_t i, tmp;
                            for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                                if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
                                    continue;
                                }
#else
                                i = tmp;
#endif
                                if ((p_info->handle != g_lea_ucst_link_info[i].handle) &&
                                    (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle)) {
                                    app_le_audio_ucst_check_set_ase(i);
                                }
                            }
                            break;
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
    }

}

static void app_le_audio_ucst_handle_ase_qos_configured(ble_bap_ase_notify_t *p_notify)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(p_notify->handle))) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASE] QOS_CONFIGURED, handle:%x state:%x->%x ase_id:%x w_ase:%x r:%x", 6,
                      p_info->handle,
                      p_info->curr_state, p_info->next_state,
                      p_notify->ase_id,
                      p_info->wait_event.wait_ase_event,
                      p_info->ase_releasing);

    /* update ase state */
    uint8_t ase_idx;
    if (APP_LE_AUDIO_UCST_ASE_MAX_NUM == (ase_idx = app_le_audio_ucst_get_ase_idx(p_info, p_notify->ase_id))) {
        return;
    }
    p_info->ase[ase_idx].curr_state = ASE_STATE_QOS_CONFIGURED;

    switch (p_info->curr_state) {
        case APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS == p_info->next_state) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;

                    LE_AUDIO_MSGLOG_I("[APP] qos_config, group:%x active:%x->%x latest:%x", 4,
                                      p_info->group_id,
                                      g_lea_ucst_ctrl.curr_group,
                                      g_lea_ucst_ctrl.next_group,
                                      g_lea_ucst_ctrl.latest_group);

                    if ((!app_le_audio_ucst_is_active_group(p_info->group_id)) ||
                        app_le_audio_ucst_check_pause_stream() ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                        app_le_audio_ucst_check_close_audio_stream();
                        break;
                    }

                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE;
                    if (BT_STATUS_SUCCESS != app_le_audio_ucst_enable_ase(p_info->handle)) {
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                    }
                }
            }
            break;
        }
        case APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE: {
            /* handle switch bis or stop stream */
            if (APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE == p_info->next_state) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    LE_AUDIO_MSGLOG_I("[APP] QOS_CONFIGURED, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    LE_AUDIO_MSGLOG_I("[APP] qos_config, group:%x active:%x->%x latest:%x", 4,
                                      p_info->group_id,
                                      g_lea_ucst_ctrl.curr_group,
                                      g_lea_ucst_ctrl.next_group,
                                      g_lea_ucst_ctrl.latest_group);

                    if ((APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY;
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_set_receiver_stop_ready(p_info->handle)) {
                            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                        }

                    } else {
                        p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                        app_le_audio_ucst_check_close_audio_stream();
                    }
                }
                break;
            }

            if (APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY == p_info->next_state) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    LE_AUDIO_MSGLOG_I("[APP] QOS_CONFIGURED, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    LE_AUDIO_MSGLOG_I("[APP] qos_config, group:%x active:%x->%x latest:%x", 4,
                                      p_info->group_id,
                                      g_lea_ucst_ctrl.curr_group,
                                      g_lea_ucst_ctrl.next_group,
                                      g_lea_ucst_ctrl.latest_group);

                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                    app_le_audio_ucst_check_close_audio_stream();
                }
                break;
            }
            break;
        }
        case APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS:
        case APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH:
        case APP_LE_AUDIO_UCST_LINK_STATE_STREAMING: {
            /* handle disable stream (sink ase) */
            if (APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE == p_info->next_state) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    LE_AUDIO_MSGLOG_I("[APP] QOS_CONFIGURED, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    LE_AUDIO_MSGLOG_I("[APP] qos_config, group:%x active:%x->%x latest:%x", 4,
                                      p_info->group_id,
                                      g_lea_ucst_ctrl.curr_group,
                                      g_lea_ucst_ctrl.next_group,
                                      g_lea_ucst_ctrl.latest_group);

                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE;
                    if ((APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY;
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_set_receiver_stop_ready(p_info->handle)) {
                            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                        }
                    } else {
                        p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY;
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_CIS;
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_disconnect_cis(p_info->handle)) {
                            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                        }
                    }
                }
            }
            break;
        }
        case APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE: {
            /* handle disable stream (call mode) */
            if (APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY == p_info->next_state) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    LE_AUDIO_MSGLOG_I("[APP] QOS_CONFIGURED, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    LE_AUDIO_MSGLOG_I("[APP] qos_config, group:%x active:%x->%x latest:%x", 4,
                                      p_info->group_id,
                                      g_lea_ucst_ctrl.curr_group,
                                      g_lea_ucst_ctrl.next_group,
                                      g_lea_ucst_ctrl.latest_group);

                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY;
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_CIS;
                    if (BT_STATUS_SUCCESS != app_le_audio_ucst_disconnect_cis(p_info->handle)) {
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

static void app_le_audio_ucst_handle_ase_enabling(ble_bap_ase_notify_t *p_notify)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(p_notify->handle))) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASE] ENABLING, handle:%x state:%x->%x ase_id:%x w_ase:%x r:%x", 6,
                      p_info->handle,
                      p_info->curr_state, p_info->next_state,
                      p_notify->ase_id,
                      p_info->wait_event.wait_ase_event,
                      p_info->ase_releasing);

    /* update ase state */
    uint8_t ase_idx;
    if (APP_LE_AUDIO_UCST_ASE_MAX_NUM == (ase_idx = app_le_audio_ucst_get_ase_idx(p_info, p_notify->ase_id))) {
        return;
    }
    p_info->ase[ase_idx].curr_state = ASE_STATE_ENABLING;

    switch (p_info->curr_state) {
        case APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE == p_info->next_state) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE;
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;

                    uint8_t i, tmp;
                    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
                            continue;
                        }
#else
                        i = tmp;
#endif
                        p_info = &g_lea_ucst_link_info[i];
                        if ((BT_HANDLE_INVALID != p_info->handle) && (p_info->handle != p_notify->handle)) {
                            LE_AUDIO_MSGLOG_I("[APP] ENABLING check link, handle:%x state:%x->%x w_ase:%x", 4, p_info->handle,
                                              p_info->curr_state, p_info->next_state,
                                              p_info->wait_event.wait_ase_event);

                            if ((APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE == p_info->next_state) ||
                                (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS == p_info->next_state)) {
                                return;
                            }
                        }
                    }

                    /* start audio stream */
                    LE_AUDIO_MSGLOG_I("[APP] ENABLING, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    if (app_le_audio_ucst_check_pause_stream() ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                        app_le_audio_ucst_disable_ase_when_state_match(APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE);
                        break;
                    }

                    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
                            continue;
                        }
#else
                        i = tmp;
#endif
                        p_info = &g_lea_ucst_link_info[i];
                        if ((BT_HANDLE_INVALID != p_info->handle) &&
                            (APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE == p_info->curr_state) &&
                            (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == p_info->next_state) &&
                            (false == p_info->ase_releasing)) {
                            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS;
                            LE_AUDIO_MSGLOG_I("[APP] ENABLING, check handle:%x state:%x->%x", 3, p_info->handle,
                                              p_info->curr_state, p_info->next_state);
                        }
                    }
                    app_le_audio_ucst_create_cis();
                }
            }
            break;
        }
        default:
            break;
    }
}

static void app_le_audio_ucst_handle_ase_streaming(ble_bap_ase_notify_t *p_notify)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(p_notify->handle))) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASE] STREAMING, handle:%x state:%x->%x ase_id:%x w_ase:%x r:%x", 6,
                      p_info->handle,
                      p_info->curr_state, p_info->next_state,
                      p_notify->ase_id,
                      p_info->wait_event.wait_ase_event,
                      p_info->ase_releasing);
    /* update ase state */
    uint8_t ase_idx;
    if (APP_LE_AUDIO_UCST_ASE_MAX_NUM == (ase_idx = app_le_audio_ucst_get_ase_idx(p_info, p_notify->ase_id))) {
        return;
    }
    p_info->ase[ase_idx].curr_state = ASE_STATE_STREAMING;

    switch (p_info->curr_state) {
        case APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS: {
            if (AUDIO_DIRECTION_SINK == p_notify->direction) {
                bt_gap_le_iso_data_path_id_t iso_data_path_id = 0x01;
                uint8_t cis_idx = 0;
                bt_status_t ret = BT_STATUS_FAIL;

                if (APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE == p_info->next_state) {
                    break;
                }

                if ((1 == g_lea_ucst_ctrl.cis_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->sink_location)) {
                    iso_data_path_id = 0x02;
                } else if (((1 == p_info->sink_location_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->sink_location)) ||
                    ((2 == p_info->sink_location_num) && (0 != (ase_idx % 2)))) {
                    iso_data_path_id = 0x02;
                    cis_idx = 1;
                }

                LE_AUDIO_MSGLOG_I("[APP] STREAMING, handle:%x sink_location:%x(num:%x) iso_data_path_id:%x cis_idx:%x", 5,
                                  p_info->handle,
                                  p_info->sink_location,
                                  p_info->sink_location_num,
                                  iso_data_path_id,
                                  cis_idx);

                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH;
                g_lea_ucst_cis_info[cis_idx].cis_status = APP_LE_AUDIO_UCST_CIS_SETUP_DATA_PATH;
                ret = app_le_audio_setup_iso_data_path(g_lea_ucst_cis_info[cis_idx].cis_handle, BT_GAP_LE_ISO_DATA_PATH_DIRECTION_OUTPUT, iso_data_path_id);
                if (BT_STATUS_SUCCESS != ret &&
                    BT_STATUS_PENDING != ret) {
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                    g_lea_ucst_cis_info[cis_idx].cis_status = APP_LE_AUDIO_UCST_CIS_CREATED;
                }
            }
            break;
        }
        case APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH: {
            if ((APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_START_READY == p_info->next_state) &&
                (AUDIO_DIRECTION_SOURCE == p_notify->direction)) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    LE_AUDIO_MSGLOG_I("[APP] STREAMING, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_STREAMING;
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                    g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING;
                    g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;

                    if (app_le_audio_ucst_check_pause_stream() ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                        app_le_audio_ucst_disable_ase_when_setup_iso_data_path();
                        return;
                    }

                }
            }
            break;
        }
        default:
            break;
    }
}

static void app_le_audio_ucst_handle_ase_disabling(ble_bap_ase_notify_t *p_notify)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(p_notify->handle))) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASE] DISABLING, handle:%x state:%x->%x ase_id:%x w_ase:%x r:%x", 6,
                      p_info->handle,
                      p_info->curr_state, p_info->next_state,
                      p_notify->ase_id,
                      p_info->wait_event.wait_ase_event,
                      p_info->ase_releasing);
    /* update ase state */
    uint8_t ase_idx;
    if (APP_LE_AUDIO_UCST_ASE_MAX_NUM == (ase_idx = app_le_audio_ucst_get_ase_idx(p_info, p_notify->ase_id))) {
        return;
    }
    p_info->ase[ase_idx].curr_state = ASE_STATE_DISABLING;

    switch (p_info->curr_state) {
        case APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE: {
            /* handle switch bis or stop stream */
            if (APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE == p_info->next_state) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    LE_AUDIO_MSGLOG_I("[APP] DISABLING, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    if ((APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY;
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_set_receiver_stop_ready(p_info->handle)) {
                            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                        }

                    } else {
                        p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                        app_le_audio_ucst_check_close_audio_stream();
                    }

                }
            }
            break;
        }
        case APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS:
        case APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH:
        case APP_LE_AUDIO_UCST_LINK_STATE_STREAMING: {
            /* handle disable stream (source ase) */
            if (APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE == p_info->next_state) {
                if (p_info->wait_event.wait_ase_event > 0) {
                    p_info->wait_event.wait_ase_event--;

                    if ((0 != p_info->wait_event.wait_ase_event) || (0 != p_info->wait_event.wait_ase_cp_event)) {
                        break;
                    }

                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE;
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;

                    LE_AUDIO_MSGLOG_I("[APP] DISABLING, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

                    if ((APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) ||
                        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY;
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_set_receiver_stop_ready(p_info->handle)) {
                            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                        }
                    } else {
                        p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY;
                        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_CIS;
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_disconnect_cis(p_info->handle)) {
                            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
    }

}

static void app_le_audio_ucst_handle_ase_releasing(ble_bap_ase_notify_t *p_notify)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(p_notify->handle))) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASE] RELEASING, handle:%x state:%x->%x ase_id:%x w_ase:%x r:%x", 6,
                      p_info->handle,
                      p_info->curr_state, p_info->next_state,
                      p_notify->ase_id,
                      p_info->wait_event.wait_ase_event,
                      p_info->ase_releasing);

    /* update ase state */
    uint8_t ase_idx;
    if (APP_LE_AUDIO_UCST_ASE_MAX_NUM == (ase_idx = app_le_audio_ucst_get_ase_idx(p_info, p_notify->ase_id))) {
        return;
    }
    p_info->ase[ase_idx].curr_state = ASE_STATE_RELEASING;

    if (!p_info->ase_releasing) {
        p_info->ase_releasing = true;
        p_info->release = true;
        if ((APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC <= p_info->curr_state) ||
            (APP_LE_AUDIO_UCST_LINK_STATE_STREAMING >= p_info->curr_state)) {

            if ((APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) ||
                (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
                p_info->wait_event.wait_ase_event = 2;
            } else {
                p_info->wait_event.wait_ase_event = 1;
            }
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC;
        }
    }
    g_lea_ucst_ctrl.release = true;
}

static void app_le_audio_ucst_handle_ase_control_point_notify(ble_ascs_control_point_notify_t *p_notify)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(p_notify->handle))) {
        return;
    }

    switch (p_notify->opcode) {
        case ASE_OPCODE_CONFIG_CODEC: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC != p_info->next_state) {
                return;
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, CONFIG_CODEC handle:%x", 1, p_info->handle);
            break;
        }
        case ASE_OPCODE_CONFIG_QOS: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS != p_info->next_state) {
                return;
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, CONFIG_QOS handle:%x", 1, p_info->handle);
            break;
        }
        case ASE_OPCODE_ENABLE: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE != p_info->next_state) {
                return;
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, ENABLE handle:%x", 1, p_info->handle);
            uint8_t j;
            for (j = 0 ; j < p_notify->number_of_ases; j++) {
                if (RESPONSE_CODE_SUCCESS != p_notify->response[j].response_code) {
                    app_le_audio_ucst_send_prepare_vcmd(p_info->handle, APP_LE_AUDIO_PREPARE_VCMD_MODE_CONN, 0);
                }
                if (RESPONSE_CODE_INSUFFICIENT_RESOURCES == p_notify->response[j].response_code) {
                    LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, ENABLE ase_id:%d rsp:INSUFFICIENT_RESOURCES reason:0x%x", 2, p_notify->response[j].ase_id, p_notify->response[j].reason);
                    /* To do: change to qos */
                    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                    p_info->wait_event.wait_ase_cp_event = 0;
                    p_info->wait_event.wait_ase_event = 0;
                    app_le_audio_ucst_stop(false);

                    if (APP_LE_AUDIO_UCST_STREAM_STATE_STOP_STREAMING != g_lea_ucst_ctrl.next_stream_state) {
                        app_le_audio_ucst_check_close_audio_stream();
                    }
                    break;
                }
            }
            break;
        }
        case ASE_OPCODE_RECEIVER_START_READY: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_START_READY != p_info->next_state) {
                return;
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, RECEIVER_START_READY handle:%x", 1, p_info->handle);
            break;
        }
        case ASE_OPCODE_DISABLE: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE != p_info->next_state) {
                return;
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, DISABLE handle:%x", 1, p_info->handle);
            break;
        }
        case ASE_OPCODE_RECEIVER_STOP_READY: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY != p_info->next_state) {
                return;
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, RECEIVER_STOP_READY handle:%x", 1, p_info->handle);
            break;
        }
        case ASE_OPCODE_UPDATE_METADATA: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_UPDATE_ASE_METADATA != p_info->next_state) {
                return;
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, UPDATE_METADATA handle:%x", 1, p_info->handle);
            break;
        }
        case ASE_OPCODE_RELEASE: {
            if (APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE != p_info->next_state) {
                return;
            }
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY, RELEASE handle:%x", 1, p_info->handle);
            break;
        }

        default: {
            return;
        }

    }

    uint8_t i;
    for (i = 0 ; i < p_notify->number_of_ases; i++) {
        if (RESPONSE_CODE_SUCCESS != p_notify->response[i].response_code) {
            LE_AUDIO_MSGLOG_I("[APP][ASE] CONTROL_POINT_NOTIFY ERROR!!, ase_id:%d rsp:0x%x reason:0x%x", 3, p_notify->response[i].ase_id, p_notify->response[i].response_code, p_notify->response[i].reason);
        }
    }

    if (p_info->wait_event.wait_ase_cp_event > 0) {
        p_info->wait_event.wait_ase_cp_event--;
    }
}

static void app_le_audio_ucst_handle_bap_discover_service_complete(ble_bap_discover_service_complete_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(cnf->handle))) {
        LE_AUDIO_MSGLOG_I("[APP][U] handle_bap_discover_service_complete, link not exist (hdl:%x)", 1, cnf->handle);
        return;
    }

    p_info->sink_ase_num = cnf->number_of_sink_ases;
    p_info->source_ase_num = cnf->number_of_source_ases;
    p_info->source_ase_idx = p_info->sink_ase_num;

    LE_AUDIO_MSGLOG_I("[APP][U] handle_bap_discover_service_complete, handle:%x state:%x->%x ase_num(sink:%x source:%x)", 5, p_info->handle,
                      p_info->curr_state, p_info->next_state,
                      p_info->sink_ase_num, p_info->source_ase_num);

    bt_status_t ret;

    /* enter next step */
    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_AUDIO_CONTEXTS;
    if (BT_STATUS_SUCCESS != (ret = ble_bap_pacs_read_available_audio_contexts_req(p_info->handle))) {
        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
        LE_AUDIO_MSGLOG_I("[APP] read_available_audio_contexts_req failed, handle:%x ret:%x", 2, p_info->handle, ret);
    }
}

static void app_le_audio_ucst_handle_bap_pacs_discovery_all(app_le_audio_ucst_link_info_t *p_info)
{
    bt_status_t ret;

    if (NULL == p_info) {
        return;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_PAC == p_info->next_state) {
        if (BT_STATUS_SUCCESS == (ret = ble_bap_pacs_read_sink_pac_req(p_info->handle))) {
            return;
        }
        LE_AUDIO_MSGLOG_I("[APP] read_sink_pac_req failed, handle:%x ret:%x", 2, p_info->handle, ret);
        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_LOCATION;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_LOCATION == p_info->next_state) {
        if (BT_STATUS_SUCCESS == (ret = ble_bap_pacs_read_sink_location_req(p_info->handle))) {
            return;
        }
        LE_AUDIO_MSGLOG_I("[APP] read_sink_location_req failed, handle:%x ret:%x", 2, p_info->handle, ret);
        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_PAC;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_PAC == p_info->next_state) {
        if (BT_STATUS_SUCCESS == (ret = ble_bap_pacs_read_source_pac_req(p_info->handle))) {
            return;
        }
        LE_AUDIO_MSGLOG_I("[APP] read_source_pac_req failed, handle:%x ret:%x", 2, p_info->handle, ret);
        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_LOCATION;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_LOCATION == p_info->next_state) {
        if (BT_STATUS_SUCCESS == (ret = ble_bap_pacs_read_source_location_req(p_info->handle))) {
            return;
        }
        LE_AUDIO_MSGLOG_I("[APP] read_source_location_req failed, handle:%x ret:%x", 2, p_info->handle, ret);
        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_ASE;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_ASE == p_info->next_state) {
        p_info->read_ase_num = 1;
        if (BT_STATUS_SUCCESS == (ret = ble_bap_ascs_read_ase_value_req(p_info->handle, p_info->read_ase_num))) {
            return;
        }
        LE_AUDIO_MSGLOG_I("[APP] read_ase_value_req failed, handle:%x ret:%x", 2, p_info->handle, ret);
        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
        p_info->read_ase_num = 0;
    }
}

static void app_le_audio_ucst_handle_read_available_audio_contexts_cnf(ble_bap_read_available_audio_contexts_cnf_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(cnf->handle))) {
        LE_AUDIO_MSGLOG_I("[APP][PACS] AVAILABLE_AUDIO_CONTEXTS, link not exist (hdl:%x)", 1, cnf->handle);
        return;
    }

    p_info->sink_available_contexts = cnf->sink_contexts;
    p_info->source_available_contexts = cnf->source_contexts;

    LE_AUDIO_MSGLOG_I("[APP][PACS] AVAILABLE_AUDIO_CONTEXTS, handle:%x sink:0x%x source:0x%x", 3, p_info->handle,
                      p_info->sink_available_contexts,
                      p_info->source_available_contexts);

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_AUDIO_CONTEXTS != p_info->next_state) {
        LE_AUDIO_MSGLOG_I("[APP] AVAILABLE_AUDIO_CONTEXTS state NOT match, handle:%x state:%x->%x", 2, p_info->handle, p_info->curr_state, p_info->next_state);
        return;
    }

    /* enter next step */
    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_AUDIO_CONTEXTS;
    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_PAC;
    app_le_audio_ucst_handle_bap_pacs_discovery_all(p_info);
}

static void app_le_audio_ucst_handle_read_sink_pac_cnf(ble_bap_read_sink_pac_cnf_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(cnf->handle))) {
        LE_AUDIO_MSGLOG_I("[APP][PACS] SINK_PAC, link not exist (hdl:%x)", 1, cnf->handle);
        return;
    }

    p_info->sink_pac.is_supported = true;
    p_info->sink_pac.num_of_record = cnf->num_of_record;
    p_info->sink_pac.pac_record_length = cnf->pac_record_length;

    LE_AUDIO_MSGLOG_I("[APP][PACS] SINK_PAC, handle:%x pac_num:%x pac_len:%d", 3, p_info->handle, p_info->sink_pac.num_of_record, p_info->sink_pac.pac_record_length);

#if 0
    if (NULL == (p_info->sink_pac.pac_record = (uint8_t *)pvPortMalloc(cnf->pac_record_length))) {
        LE_AUDIO_MSGLOG_I("[APP][PACS] SINK_PAC, out of memory", 0);
        return;
    }

    memcpy(p_info->sink_pac.pac_record, cnf->pac_record, cnf->pac_record_length);
#endif

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_PAC != p_info->next_state) {
        LE_AUDIO_MSGLOG_I("[APP] SINK_PAC state NOT match, handle:%x state:%x->%x", 2, p_info->handle, p_info->curr_state, p_info->next_state);
        return;
    }

    /* enter next step */
    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_PAC;
    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_LOCATION;
    app_le_audio_ucst_handle_bap_pacs_discovery_all(p_info);
}

static void app_le_audio_ucst_handle_read_sink_location_cnf(ble_bap_read_sink_location_cnf_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(cnf->handle))) {
        LE_AUDIO_MSGLOG_I("[APP][PACS] SINK_LOCATION, link not exist (hdl:%x)", 1, cnf->handle);
        return;
    }

    p_info->sink_location = cnf->location;
    p_info->sink_location_num = app_le_audio_ucst_get_location_count(p_info->sink_location);

    LE_AUDIO_MSGLOG_I("[APP][PACS] SINK_LOCATION, handle:%x location:0x%x(num:%x)", 3, p_info->handle, p_info->sink_location, p_info->sink_location_num);

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_LOCATION != p_info->next_state) {
        LE_AUDIO_MSGLOG_I("[APP] SINK_LOCATION state NOT match, handle:%x state:%x->%x", 2, p_info->handle, p_info->curr_state, p_info->next_state);
        return;
    }

    /* enter next step */
    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_LOCATION;
    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_PAC;
    app_le_audio_ucst_handle_bap_pacs_discovery_all(p_info);
}

static void app_le_audio_ucst_handle_read_source_pac_cnf(ble_bap_read_source_pac_cnf_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(cnf->handle))) {
        LE_AUDIO_MSGLOG_I("[APP][PACS] SOURCE_PAC, link not exist (hdl:%x)", 1, cnf->handle);
        return;
    }

    p_info->source_pac.num_of_record = cnf->num_of_record;
    p_info->source_pac.pac_record_length = cnf->pac_record_length;

    LE_AUDIO_MSGLOG_I("[APP][PACS] SOURCE_PAC, handle:%x pac_num:%x pac_len:%d", 3, p_info->handle, p_info->source_pac.num_of_record, p_info->source_pac.pac_record_length);

#if 0
    if (NULL == (p_info->source_pac.pac_record = (uint8_t *)pvPortMalloc(cnf->pac_record_length))) {
        LE_AUDIO_MSGLOG_I("[APP][PACS] SOURCE_PAC, out of memory", 0);
        return;
    }

    memcpy(p_info->source_pac.pac_record, cnf->pac_record, cnf->pac_record_length);
#endif

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_PAC != p_info->next_state) {
        LE_AUDIO_MSGLOG_I("[APP] SOURCE_PAC state NOT match, handle:%x state:%x->%x", 2, p_info->handle, p_info->curr_state, p_info->next_state);
        return;
    }

    /* enter next step */
    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_PAC;
    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_LOCATION;
    app_le_audio_ucst_handle_bap_pacs_discovery_all(p_info);
}

static void app_le_audio_ucst_handle_read_source_location_cnf(ble_bap_read_source_location_cnf_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(cnf->handle))) {
        LE_AUDIO_MSGLOG_I("[APP][PACS] SOURCE_LOCATION, link not exist (hdl:%x)", 1, cnf->handle);
        return;
    }

    p_info->source_location = cnf->location;
    p_info->source_location_num = app_le_audio_ucst_get_location_count(p_info->source_location);

    LE_AUDIO_MSGLOG_I("[APP][PACS] SOURCE_LOCATION, handle:%x location:0x%x(num:%x)", 3, p_info->handle, p_info->source_location, p_info->source_location_num);

    if (!((APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING == g_lea_ucst_ctrl.curr_stream_state) &&
          (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state))) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (p_info->group_id == g_lea_ucst_ctrl.curr_group)
#endif
        {
            app_le_audio_ucst_set_mic_channel();
        }
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_LOCATION != p_info->next_state) {
        LE_AUDIO_MSGLOG_I("[APP] SOURCE_LOCATION state NOT match, handle:%x state:%x->%x", 2, p_info->handle, p_info->curr_state, p_info->next_state);
        return;
    }

    /* enter next step */
    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_LOCATION;
    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_ASE;
    app_le_audio_ucst_handle_bap_pacs_discovery_all(p_info);
}

static void app_le_audio_ucst_handle_read_ase_cnf(ble_ascs_read_ase_cnf_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(cnf->handle))) {
        LE_AUDIO_MSGLOG_I("[APP][ASCS] READ_ASE_CNF, link not exist (hdl:%x)", 1, cnf->handle);
        return;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_READ_ASE != p_info->next_state) {
        LE_AUDIO_MSGLOG_I("[APP][ASCS] READ_ASE_CNF state NOT match, handle:%x state:%x->%x", 2, p_info->handle, p_info->curr_state, p_info->next_state);
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][ASCS] READ_ASE_CNF, ase_id:%x ase_state:%x", 2, cnf->ase_id, cnf->ase_state);

    if (p_info->read_ase_num <= p_info->sink_ase_num) {
        app_le_audio_ucst_set_ase(p_info, cnf->ase_id, cnf->ase_state, AUDIO_DIRECTION_SINK);
    } else {
        app_le_audio_ucst_set_ase(p_info, cnf->ase_id, cnf->ase_state, AUDIO_DIRECTION_SOURCE);
    }

    if (p_info->read_ase_num < (p_info->sink_ase_num + p_info->source_ase_num)) {
        p_info->read_ase_num++;
        ble_bap_ascs_read_ase_value_req(p_info->handle, p_info->read_ase_num);

    } else {
        bt_status_t ret;

        /* enter next step */
        p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_READ_ASE;
#ifdef AIR_LE_AUDIO_BA_ENABLE
        if ((APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode)
            || (APP_LE_AUDIO_MODE_ASIT == g_lea_ctrl.next_mode)) {
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            app_le_audio_ba_read_broadcast_receive_state(cnf->handle);
            return;
        }
#endif
        p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC;
        if (g_lea_ucst_test_mode_flag & APP_LE_AUDIO_UCST_TEST_MODE_PAUSE_BEFORE_CONFIG_ASE) {
            bt_app_common_at_cmd_print_report("[TEST MODE] Service discovery complete!\r\n");
            return;
        }
        if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_config_codec(p_info->handle))) {
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            LE_AUDIO_MSGLOG_I("[APP] config_codec failed, handle:%x ret:%x", 2, p_info->handle, ret);
        }
    }
}

static void app_le_audio_ucst_handle_bap_evt(ble_bap_event_t event, void *msg)
{
    if (NULL == msg) {
        return;
    }

    switch (event) {
        case BLE_BAP_ASCS_ASE_NOTIFY: {
            ble_bap_ase_notify_t *p_notify = (ble_bap_ase_notify_t *)msg;
            switch (p_notify->ase_state) {
                case ASE_STATE_CODEC_CONFIGURED: {
                    app_le_audio_ucst_handle_ase_codec_configured(p_notify);
                    break;
                }
                case ASE_STATE_QOS_CONFIGURED: {
                    app_le_audio_ucst_handle_ase_qos_configured(p_notify);
                    break;
                }
                case ASE_STATE_ENABLING: {
                    app_le_audio_ucst_handle_ase_enabling(p_notify);
                    break;
                }
                case ASE_STATE_STREAMING: {
                    app_le_audio_ucst_handle_ase_streaming(p_notify);
                    break;
                }
                case ASE_STATE_DISABLING: {
                    app_le_audio_ucst_handle_ase_disabling(p_notify);
                    break;
                }
                case ASE_STATE_RELEASING: {
                    app_le_audio_ucst_handle_ase_releasing(p_notify);
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case BLE_BAP_ASCS_ASE_CONTROL_POINT_NOTIFY: {
            app_le_audio_ucst_handle_ase_control_point_notify((ble_ascs_control_point_notify_t *)msg);
            break;
        }
        case BLE_BAP_ASCS_DISCOVER_SERVICE_COMPLETE_NOTIFY: {
            app_le_audio_ucst_handle_bap_discover_service_complete((ble_bap_discover_service_complete_t *)msg);
            break;
        }
        case BLE_BAP_PACS_READ_AVAILABLE_AUDIO_CONTEXTS_CNF: {
            app_le_audio_ucst_handle_read_available_audio_contexts_cnf((ble_bap_read_available_audio_contexts_cnf_t *)msg);
            break;
        }
        case BLE_BAP_PACS_READ_SINK_PAC_CNF: {
            app_le_audio_ucst_handle_read_sink_pac_cnf((ble_bap_read_sink_pac_cnf_t *)msg);
            break;
        }
        case BLE_BAP_PACS_READ_SINK_LOCATION_CNF: {
            app_le_audio_ucst_handle_read_sink_location_cnf((ble_bap_read_sink_location_cnf_t *)msg);
            break;
        }
        case BLE_BAP_PACS_READ_SOURCE_PAC_CNF: {
            app_le_audio_ucst_handle_read_source_pac_cnf((ble_bap_read_source_pac_cnf_t *)msg);
            break;
        }
        case BLE_BAP_PACS_READ_SOURCE_LOCATION_CNF: {
            app_le_audio_ucst_handle_read_source_location_cnf((ble_bap_read_source_location_cnf_t *)msg);
            break;
        }
        case BLE_BAP_ASCS_READ_ASE_CNF: {
            app_le_audio_ucst_handle_read_ase_cnf((ble_ascs_read_ase_cnf_t *)msg);
            break;
        }
        default:
            break;
    }
#ifdef AIR_LE_AUDIO_BA_ENABLE
    app_le_audio_ba_handle_bap_client_evt(event, msg);
#endif
}

static void app_le_audio_ucst_handle_csip_discover_cs_cnf(ble_csip_discover_coordinated_set_cnf_t *cnf)
{
    char conn_string[60] = {0};
    uint8_t link_num;

    if (app_le_audio_ucst_is_connected_device(&cnf->address)) {
        return;
    }

    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == (link_num = app_le_audio_ucst_get_link_num_ex())) {
        LE_AUDIO_MSGLOG_I("[APP][CSIP] DISCOVER_COORDINATED_SET_CNF, link full", 0);
        g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
        app_le_audio_ucst_stop_scan_all();
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][CSIP] DISCOVER_COORDINATED_SET_CNF, addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,
                      cnf->address.type,
                      cnf->address.addr[5],
                      cnf->address.addr[4],
                      cnf->address.addr[3],
                      cnf->address.addr[2],
                      cnf->address.addr[1],
                      cnf->address.addr[0]);

    snprintf((char *)conn_string, 60, "Coordinated set found! type:%d, %02x:%02x:%02x:%02x:%02x:%02x\r\n",
             cnf->address.type,
             cnf->address.addr[5],
             cnf->address.addr[4],
             cnf->address.addr[3],
             cnf->address.addr[2],
             cnf->address.addr[1],
             cnf->address.addr[0]);
    bt_app_common_at_cmd_print_report(conn_string);

    LE_AUDIO_MSGLOG_I("[APP] DISCOVER_COORDINATED_SET_CNF, curr_conn:%x link_num:%x", 2, g_lea_ucst_ctrl.curr_conn, link_num);

    switch (g_lea_ucst_ctrl.curr_conn) {
        case APP_LE_AUDIO_UCST_CONN_COORDINATED_SET_BY_SIRK: {
            app_le_audio_ucst_connect_device(&cnf->address);
            break;
        }
        default:
            break;
    }
}

static void app_le_audio_ucst_handle_csip_read_cs_size_cnf(ble_csip_read_set_size_cnf_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    char conn_string[60] = {0};
    uint8_t link_num;
    uint8_t link_idx;
    uint8_t bond_idx = APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID;

    if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == (link_idx = app_le_audio_ucst_get_link_idx(cnf->handle))) {
        return;
    }

    /* link_info */
    p_info = &g_lea_ucst_link_info[link_idx];

    if (APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID == p_info->bond_idx) {
        /* new device */

        p_info->group_size = cnf->size;

        if (g_lea_ucst_pts_set_size) {
            p_info->group_size = g_lea_ucst_pts_set_size; /* some PTS cases which need 2 or more PTS dongles may have no CSIS */
        }

        /* group_info */
        if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > p_info->group_id) {
            g_lea_ucst_group_info[p_info->group_id].size = cnf->size;
        }

        if (APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID != (bond_idx = app_le_audio_ucst_get_empty_bonded_list_idx())) {
            /* link_info */
            p_info->bond_idx = bond_idx;

            /* bonded_list */
            g_lea_ucst_bonded_list.num++;
            memcpy(&(g_lea_ucst_bonded_list.device[bond_idx].addr), &(p_info->addr), sizeof(bt_addr_t));
            g_lea_ucst_bonded_list.device[bond_idx].group_id = p_info->group_id;
            g_lea_ucst_bonded_list.device[bond_idx].group_size = p_info->group_size;
            g_lea_ucst_bonded_list.device[bond_idx].link_idx = link_idx;
            g_lea_ucst_bonded_list.device[bond_idx].in_white_list = true;
            app_le_audio_ucst_write_bonded_list_to_nvkey();
        }
    }

    snprintf((char *)conn_string, 60, "Coordinated set size[%d], handle:%x\r\n", cnf->size, cnf->handle);
    bt_app_common_at_cmd_print_report(conn_string);

    link_num = app_le_audio_ucst_get_link_num_ex();

    LE_AUDIO_MSGLOG_I("[APP][CSIP] READ_CS_SIZE_CNF, handle:%x conn:%x link_num:%x group:%x(size:%x) bond_idx:%x", 6,
                      cnf->handle,
                      g_lea_ucst_ctrl.curr_conn,
                      link_num,
                      p_info->group_id,
                      p_info->group_size,
                      p_info->bond_idx);

    uint8_t active_group = app_le_audio_ucst_get_active_group();
    bool scan_cs = true, scan_bond = false, is_active_device = false;

    if (p_info->group_id == active_group) {
        is_active_device = true;
    }

    if (!app_le_audio_ucst_is_group_device_all_connected(p_info->group_id)) {
        /* check connect CS device */
        uint8_t i;
        /* check CS in bonded list */
        for (i = 0; i < g_lea_ucst_bonded_list.num; i++) {

#if 1   /* for test */
            LE_AUDIO_MSGLOG_I("[APP][CSIP] READ_CS_SIZE_CNF, bond_idx:%x->%x group:%x->%x in_white_list:%x", 5,
                              p_info->bond_idx,
                              i,
                              p_info->group_id,
                              g_lea_ucst_bonded_list.device[i].group_id,
                              g_lea_ucst_bonded_list.device[i].in_white_list);
#endif
            if ((i != p_info->bond_idx) &&
                (p_info->group_id == g_lea_ucst_bonded_list.device[i].group_id) &&
                (g_lea_ucst_bonded_list.device[i].in_white_list)) {
                scan_cs = false;
                scan_bond = true;
                break;
            }
        }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if ((!is_active_device) &&
            (APP_LE_AUDIO_UCST_GROUP_ID_INVALID != active_group) &&
            (APP_LE_AUDIO_UCST_LINK_MAX_NUM == link_num)) {
            scan_cs = false;
            scan_bond = false;
        }

        if ((APP_LE_AUDIO_UCST_LINK_MAX_NUM == link_num) &&
            ((true == scan_cs) || (true == scan_bond))) {
            g_lea_ucst_waiting_conn_group = p_info->group_id;

            LE_AUDIO_MSGLOG_I("[APP][CSIP] READ_CS_SIZE_CNF, link full scan, own group(%x) mate", 1, g_lea_ucst_waiting_conn_group);
        }
    } else {
        scan_cs = false;
        /* check connect bonded device */
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == link_num) {
            /* link full */
            if ((!is_active_device) &&
                (APP_LE_AUDIO_UCST_GROUP_ID_INVALID != active_group) &&
                (!app_le_audio_ucst_is_group_device_all_connected(active_group))) {
                scan_bond = true;
                g_lea_ucst_waiting_conn_group = active_group;
                LE_AUDIO_MSGLOG_I("[APP][CSIP] READ_CS_SIZE_CNF, link full scan, other group(%x) mate", 1, g_lea_ucst_waiting_conn_group);
            }
        } else if (!app_le_audio_ucst_is_bonded_device_all_connected()) {
            scan_bond = true;
        }
#endif
    }

    LE_AUDIO_MSGLOG_I("[APP][CSIP] READ_CS_SIZE_CNF, active_group:%x is_active_device:%x scan_cs:%x scan_bond:%x", 4,
                      active_group,
                      is_active_device,
                      scan_cs,
                      scan_bond);

    if ((APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) ||
        ((false == scan_bond) && (false == scan_cs))
#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        || (APP_LE_AUDIO_UCST_LINK_MAX_NUM == link_num) || (p_info->group_size == link_num)
#endif
        )
    {
        g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
        ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                            APP_LE_AUDIO_EVENT_STOP_SCAN_NEW_DEVICE, NULL, 0,
                            NULL, 0);

        if (APP_LE_AUDIO_MODE_BCST != g_lea_ctrl.next_mode) {
            if (g_lea_ucst_callback) {
                app_dongle_le_race_connect_ind_t evt;
                evt.ret = BT_STATUS_SUCCESS;
                memcpy(&(evt.peer_addr), &(p_info->addr), sizeof(bt_addr_t));
                evt.group_id = p_info->group_id;
                evt.group_size = p_info->group_size;
                g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_CONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
            }
        }
        return;
    }

    /* search next cs device */
    LE_AUDIO_MSGLOG_I("[APP] READ_CS_SIZE_CNF, scan:%x %x", 2, scan_cs, scan_bond);

    if (scan_cs && scan_bond) {
        app_le_audio_ucst_connect_bonded_device(true);
    } else if (scan_cs) {
        app_le_audio_ucst_connect_coordinated_set(false);
    } else if (scan_bond) {
        app_le_audio_ucst_connect_bonded_device(false);
    }

    if (g_lea_ucst_callback) {
        app_dongle_le_race_connect_ind_t evt;

        evt.ret = BT_STATUS_SUCCESS;
        memcpy(&(evt.peer_addr), &(p_info->addr), sizeof(bt_addr_t));
        evt.group_id = p_info->group_id;
        evt.group_size = p_info->group_size;
        g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_CONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
    }
}

static void app_le_audio_ucst_handle_csip_sirk_cnf(ble_csip_read_sirk_cnf_t *cnf)
{
    uint8_t link_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;

    if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == (link_idx = (app_le_audio_ucst_get_link_idx(cnf->handle)))) {
        return;
    }

    //LE_AUDIO_MSGLOG_I("[APP][CSIP] READ_SIRK_CNF, handle:%x", 1, cnf->handle);

    if (0 != memcmp((uint8_t *)&g_le_audio_sirk, &(cnf->sirk), sizeof(bt_key_t))) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        /* for scan cs */
        app_le_audio_ucst_set_sirk(&(cnf->sirk), false);
#else
        app_le_audio_ucst_set_sirk(&(cnf->sirk), true);
#endif
    }

    app_le_audio_ucst_link_info_t *p_info = NULL;

    p_info = &g_lea_ucst_link_info[link_idx];

    /* check group_info */
    if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID != p_info->group_id) {
        /* bonded device */

        memcpy((uint8_t *) & (g_lea_ucst_group_info[p_info->group_id].sirk), &(cnf->sirk), sizeof(bt_key_t));

    } else {
        /* new device */

        uint8_t group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;

        if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID != (group_id = app_le_audio_ucst_get_group_id_by_sirk(&(cnf->sirk)))) {
            /* group exist */
            app_le_audio_ucst_add_link_to_group(group_id, link_idx);
            g_lea_ucst_group_info[group_id].bond_num++;

        } else {
            group_id = app_le_audio_ucst_set_new_group_info(&(cnf->sirk), link_idx);
        }

        p_info->group_id = group_id;
#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.curr_group) {
            g_lea_ucst_ctrl.curr_group = group_id;
        }
#endif
        g_lea_ucst_ctrl.latest_group = group_id;

    }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    uint8_t i = 0;
    bool update_sirk = true;

    for (i = 0; i < APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM; i++) {
        if (g_lea_ucst_sirk_info[i].group_id == p_info->group_id) {
            update_sirk = false;
            break;
        }
    }
    if (update_sirk) {
        for (i = 0; i < APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM; i++) {
            if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_sirk_info[i].group_id) {
                g_lea_ucst_sirk_info[i].group_id = p_info->group_id;
                memcpy((uint8_t *)&(g_lea_ucst_sirk_info[i].sirk), &(cnf->sirk), sizeof(bt_key_t));
                app_le_audio_ucst_write_sirk_list_to_nvkey();
                break;
            }
        }
    }
#endif

    LE_AUDIO_MSGLOG_I("[APP][CSIP] READ_SIRK_CNF, handle:%x group:%x", 2,
                      p_info->handle,
                      p_info->group_id);

}

static void app_le_audio_ucst_handle_csip_evt(ble_csip_event_t event, void *msg)
{
    if (NULL == msg) {
        return;
    }

    switch (event) {
        case BLE_CSIP_DISCOVER_COORDINATED_SET_CNF: {
            app_le_audio_ucst_handle_csip_discover_cs_cnf((ble_csip_discover_coordinated_set_cnf_t *)msg);
            break;
        }
        case BLE_CSIP_READ_COORDINATED_SET_SIZE_CNF: {
            app_le_audio_ucst_handle_csip_read_cs_size_cnf((ble_csip_read_set_size_cnf_t *)msg);
            break;
        }
        case BLE_CSIP_READ_SIRK_CNF: {
            app_le_audio_ucst_handle_csip_sirk_cnf((ble_csip_read_sirk_cnf_t *)msg);
            break;
        }
        case BLE_CSIP_READ_SET_MEMBER_LOCK_CNF: {
            ble_csip_read_set_lock_cnf_t *cnf = (ble_csip_read_set_lock_cnf_t *)msg;
            LE_AUDIO_MSGLOG_I("[APP][CSIP] BLE_CSIP_READ_SET_MEMBER_LOCK_CNF, handle:%x set_lock:%x", 2, cnf->handle, cnf->lock);
            break;
        }
        case BLE_CSIP_NOTIFY_SET_MEMBER_LOCK_NOTIFY: {
            ble_csip_notify_set_lock_ind_t *ind = (ble_csip_notify_set_lock_ind_t *)msg;
            LE_AUDIO_MSGLOG_I("[APP][CSIP] BLE_CSIP_NOTIFY_SET_MEMBER_LOCK_NOTIFY, handle:%x set_lock:%x", 2, ind->handle, ind->lock);
            break;
        }
        case BLE_CSIP_READ_SET_MEMBER_RANK_CNF: {
            ble_csip_read_rank_cnf_t *cnf = (ble_csip_read_rank_cnf_t *)msg;
            LE_AUDIO_MSGLOG_I("[APP][CSIP] BLE_CSIP_READ_SET_MEMBER_RANK_CNF, handle:%x set_rank:%x", 2, cnf->handle, cnf->rank);
            break;
        }
        case BLE_CSIP_DISCOVER_SERVICE_COMPLETE_NOTIFY: {
            ble_csip_discover_service_complete_t *ind = (ble_csip_discover_service_complete_t *)msg;

            LE_AUDIO_MSGLOG_I("[APP][CSIP] DISCOVER_SERVICE_COMPLETE, handle:%x ret:%x", 2, ind->handle, ind->status);

            if (BT_STATUS_SUCCESS == ind->status) {
                break;
            }

            app_le_audio_ucst_link_info_t *p_info = NULL;
            ble_csip_read_set_size_cnf_t cnf;
            uint8_t link_idx;

            if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == (link_idx = app_le_audio_ucst_get_link_idx(ind->handle))) {
                LE_AUDIO_MSGLOG_I("[APP][CSIP] DISCOVER_SERVICE_COMPLETE, link not exist (hdl:%x)", 1, ind->handle);
                break;
            }

            p_info = &g_lea_ucst_link_info[link_idx];

            /* check group_info */
            if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == p_info->group_id) {
                /* new device */

                if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= (p_info->group_id = app_le_audio_ucst_set_new_group_info(NULL, link_idx))) {
                    LE_AUDIO_MSGLOG_I("[APP][CSIP] DISCOVER_SERVICE_COMPLETE, ERROR! handle:%x check group:%x)", 2,
                                      p_info->handle,
                                      p_info->group_id);
                    p_info->group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
                    break;
                }

#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.curr_group) {
                    g_lea_ucst_ctrl.curr_group = p_info->group_id;
                }
#endif
                g_lea_ucst_ctrl.latest_group = p_info->group_id;

            }

            LE_AUDIO_MSGLOG_I("[APP][CSIP] DISCOVER_SERVICE_COMPLETE, handle:%x group:%x", 2,
                              p_info->handle,
                              p_info->group_id);


            cnf.handle = p_info->handle;
            cnf.size = 1;
            if (g_lea_ucst_pts_set_size) {
                /* some PTS cases which need 2 or more PTS dongles may have no CSIS */
               cnf.size = g_lea_ucst_pts_set_size;
            }
            app_le_audio_ucst_handle_csip_read_cs_size_cnf(&cnf);
            break;
        }
        default:
            break;
    }
}

static void app_le_audio_ucst_handle_tmap_evt(ble_tmap_event_t event, void *msg)
{
    if (NULL == msg) {
        return;
    }

    switch (event) {
        case BLE_TMAP_READ_ROLE_CNF: {
            ble_tmap_read_role_cnf_t *cnf = (ble_tmap_read_role_cnf_t *)msg;
            LE_AUDIO_MSGLOG_I("[APP] TMAP handle:%x, role:%x", 2, cnf->handle, cnf->role);
            break;
        }
        default:
            break;
    }
}

static uint16_t app_le_audio_ucst_handle_source_evt(bt_le_audio_source_event_t event, void *msg)
{
    switch (event) {
        /* Call Event: call state */
        case BT_LE_AUDIO_SOURCE_EVENT_CALL_INCOMING: {
            ble_tbs_event_incoming_call_t *param = (ble_tbs_event_incoming_call_t *)msg;

            bt_app_common_at_cmd_print_report("call state: INCOMING\r\n");
            LE_AUDIO_MSGLOG_I("[APP] CALL INCOMING, call_idx:%x", 1, param->call_idx);
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_CALL_DIALING: {
            bt_app_common_at_cmd_print_report("call state: DIALING\r\n");
            LE_AUDIO_MSGLOG_I("[APP] CALL DIALING", 0);
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_CALL_ALERTING: {
            bt_app_common_at_cmd_print_report("call state: ALERTIN\r\n");
            LE_AUDIO_MSGLOG_I("[APP] CALL ALERTING", 0);
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_CALL_ACTIVE: {
            bt_app_common_at_cmd_print_report("call state: ACTIVE\r\n");
            LE_AUDIO_MSGLOG_I("[APP] CALL ACTIVE", 0);
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_CALL_HELD: {
            bt_app_common_at_cmd_print_report("call state: HELD\r\n");
            LE_AUDIO_MSGLOG_I("[APP] CALL HELD", 0);
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_CALL_ENDED: {
            bt_app_common_at_cmd_print_report("call state: END\r\n");
            LE_AUDIO_MSGLOG_I("[APP] CALL ENDED", 0);
            break;
        }
        /* Call Event: call control */
        case BT_LE_AUDIO_SOURCE_EVENT_CALL_ACCEPT: {
            ble_tbs_event_parameter_t *param = (ble_tbs_event_parameter_t *)msg;
            bt_le_audio_call_state_t call_state;

            bt_app_common_at_cmd_print_report("call action: ACCEPT\r\n");

            call_state =  bt_le_audio_source_call_get_state(param->service_idx, param->call_idx);
            LE_AUDIO_MSGLOG_I("[APP] CALL ACCEPT, call_idx:%x state:%x", 2, param->call_idx, call_state);

            if (BLE_TBS_STATE_INCOMING == call_state) {
#ifdef AIR_MS_TEAMS_ENABLE
                usb_hid_srv_send_action(USB_HID_SRV_ACTION_ACCEPT_CALL, NULL);

                if (true == g_lea_ucst_pts_remote_call_service) {
                    g_curr_call_idx = param->call_idx;
                    app_le_audio_usb_hid_handle_call_active();
                }
#endif
            }
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_CALL_TERMINATE: {
            ble_tbs_event_parameter_t *param = (ble_tbs_event_parameter_t *)msg;
            bt_le_audio_call_state_t call_state;

            bt_app_common_at_cmd_print_report("call action: TERMINATE\r\n");
            call_state =  bt_le_audio_source_call_get_state(param->service_idx, param->call_idx);

            LE_AUDIO_MSGLOG_I("[APP] CALL TERMINATE, call_idx:%x state:%x", 2, param->call_idx, call_state);
#ifdef AIR_MS_TEAMS_ENABLE
            if (BLE_TBS_STATE_INCOMING == call_state) {
                usb_hid_srv_send_action(USB_HID_SRV_ACTION_REJECT_CALL, NULL);
            }
            else if (BLE_TBS_STATE_IDLE > call_state) {
                usb_hid_srv_send_action(USB_HID_SRV_ACTION_TERMINATE_CALL, NULL);
            }

            if (true == g_lea_ucst_pts_remote_call_service) {
                g_curr_call_idx = param->call_idx;
                app_le_audio_usb_hid_handle_call_end();
            }
#endif
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_HOLD_CALL: {
            ble_tbs_event_parameter_t *param = (ble_tbs_event_parameter_t *)msg;
            bt_le_audio_call_state_t call_state;

            bt_app_common_at_cmd_print_report("call action: HOLD\r\n");

            call_state =  bt_le_audio_source_call_get_state(param->service_idx, param->call_idx);
            LE_AUDIO_MSGLOG_I("[APP] CALL HOLD, call_idx:%x state:%x", 2, param->call_idx, call_state);

#ifdef AIR_MS_TEAMS_ENABLE
            usb_hid_srv_send_action(USB_HID_SRV_ACTION_HOLD_CALL, NULL);

            if (true == g_lea_ucst_pts_remote_call_service) {
                g_curr_call_idx = param->call_idx;
                app_le_audio_usb_hid_handle_call_hold();
            }
#endif
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_UNHOLD_CALL: {
            ble_tbs_event_parameter_t *param = (ble_tbs_event_parameter_t *)msg;
            bt_le_audio_call_state_t call_state;

            bt_app_common_at_cmd_print_report("call action: UNHOLD\r\n");

            call_state =  bt_le_audio_source_call_get_state(param->service_idx, param->call_idx);
            LE_AUDIO_MSGLOG_I("[APP] CALL UNHOLD, call_idx:%x state:%x", 2, param->call_idx, call_state);

#ifdef AIR_MS_TEAMS_ENABLE
            usb_hid_srv_send_action(USB_HID_SRV_ACTION_UNHOLD_CALL, NULL);

            if (true == g_lea_ucst_pts_remote_call_service) {
                g_curr_call_idx = param->call_idx;
                app_le_audio_usb_hid_handle_call_unhold();
            }
#endif
            break;
        }
        /* Volume Event */
        case BT_LE_AUDIO_SOURCE_EVENT_VOLUME_STATE: {
            bt_le_audio_source_event_volume_t *ind = (bt_le_audio_source_event_volume_t *)msg;
            char conn_string[50] = {0};
            LE_AUDIO_MSGLOG_I("[APP] VOLUME STATE, volume:%02x mute:%x", 2, ind->volume, ind->mute);
            snprintf((char *)conn_string, 50, "Volume state:0x%02x, mute:0x%02x", ind->volume, ind->mute);
            bt_app_common_at_cmd_print_report(conn_string);
            break;
        }
        /* Media Event */
        case BT_LE_AUDIO_SOURCE_EVENT_MEDIA_PLAYING: {
            bt_app_common_at_cmd_print_report("media state: PLAYING\r\n");
            LE_AUDIO_MSGLOG_I("[APP] MEDIA PLAYING", 0);
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_MEDIA_PAUSED: {
            bt_app_common_at_cmd_print_report("media state: PAUSED\r\n");
            LE_AUDIO_MSGLOG_I("[APP] MEDIA PAUSED", 0);
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_MEDIA_PREVIOUS_TRACK: {
            bt_app_common_at_cmd_print_report("media action: PREVIOUS\r\n");
            LE_AUDIO_MSGLOG_I("[APP] MEDIA PREVIOUS TRACK", 0);
            break;
        }
        case BT_LE_AUDIO_SOURCE_EVENT_MEDIA_NEXT_TRACK: {
            bt_app_common_at_cmd_print_report("media action: NEXT\r\n");
            LE_AUDIO_MSGLOG_I("[APP] MEDIA NEXT TRACK", 0);
            break;
        }
        default:
            break;
    }
    return 0;
}

/**************************************************************************************************
* Public Functions
**************************************************************************************************/
void app_le_audio_ucst_handle_scan_cnf(bt_status_t ret)
{
    LE_AUDIO_MSGLOG_I("[APP][U] LE_SET_SCAN_CNF, ret:%x scan:%x->%x conn:%x", 4, ret,
                      g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.next_scan,
                      g_lea_ucst_ctrl.curr_conn);

    if (BT_STATUS_SUCCESS != ret) {
        /* handle error case */
        switch (g_lea_ucst_ctrl.curr_scan) {
            case APP_LE_AUDIO_UCST_SCAN_ENABLING:
            case APP_LE_AUDIO_UCST_SCAN_CS_ENABLING: {
                g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
                bt_app_common_at_cmd_print_report("Stop scanning.\r\n");

                if (APP_LE_AUDIO_UCST_CONN_LEA_DEVICE == g_lea_ucst_ctrl.curr_conn) {
                    g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
                    if (g_lea_ucst_callback) {
                        app_dongle_le_race_scan_cnf_t cnf;
                        cnf.ret = ret;
                        cnf.start_scan = true;
                        g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_SCAN_CNF, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &cnf);
                    }
                }
                break;
            }
            case APP_LE_AUDIO_UCST_SCAN_DISABLING: {
                g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_ENABLED;
                if (APP_LE_AUDIO_UCST_CONN_LEA_DEVICE == g_lea_ucst_ctrl.curr_conn) {
                    if (g_lea_ucst_callback) {
                        app_dongle_le_race_scan_cnf_t cnf;
                        cnf.ret = ret;
                        cnf.start_scan = false;
                        g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_SCAN_CNF, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &cnf);
                    }
                }
                break;
            }
            case APP_LE_AUDIO_UCST_SCAN_CS_DISABLING: {
                g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_CS_ENABLED;
                break;
            }
            default:
                break;
        }
        g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
        return;
    }

    if (g_lea_ucst_ctrl.next_scan == g_lea_ucst_ctrl.curr_scan) {
        g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
    }

    /* update curr_scan */
    switch (g_lea_ucst_ctrl.curr_scan) {
        case APP_LE_AUDIO_UCST_SCAN_ENABLING: {
            g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_ENABLED;
            bt_app_common_at_cmd_print_report("Start scanning:\r\n");
            if (APP_LE_AUDIO_UCST_CONN_LEA_DEVICE == g_lea_ucst_ctrl.curr_conn) {
                if (g_lea_ucst_callback) {
                    app_dongle_le_race_scan_cnf_t cnf;
                    cnf.ret = ret;
                    cnf.start_scan = true;
                    g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_SCAN_CNF, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &cnf);
                }
            }
            if ((APP_LE_AUDIO_UCST_SCAN_CS_ENABLING == g_lea_ucst_ctrl.next_scan) ||
                (APP_LE_AUDIO_UCST_SCAN_DISABLING == g_lea_ucst_ctrl.next_scan)) {
                if (BT_STATUS_SUCCESS != app_le_audio_ucst_stop_scan()) {
                    g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
                }
                return;
            }
            break;
        }
        case APP_LE_AUDIO_UCST_SCAN_CS_ENABLING: {
            g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_CS_ENABLED;
            bt_app_common_at_cmd_print_report("Start scanning:\r\n");
            if ((APP_LE_AUDIO_UCST_SCAN_ENABLING == g_lea_ucst_ctrl.next_scan) ||
                (APP_LE_AUDIO_UCST_SCAN_CS_DISABLING == g_lea_ucst_ctrl.next_scan)) {
                if (BT_STATUS_SUCCESS != app_le_audio_ucst_stop_scan_cs()) {
                    g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
                }
                return;
            }
            break;
        }
        case APP_LE_AUDIO_UCST_SCAN_DISABLING:
        case APP_LE_AUDIO_UCST_SCAN_CS_DISABLING: {
            app_le_audio_ucst_scan_t curr_scan = g_lea_ucst_ctrl.curr_scan;
            g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
            bt_app_common_at_cmd_print_report("Stop scanning.\r\n");
            if ((APP_LE_AUDIO_UCST_SCAN_DISABLING == curr_scan) &&
                (APP_LE_AUDIO_UCST_CONN_LEA_DEVICE == g_lea_ucst_ctrl.curr_conn)) {
                if (APP_LE_AUDIO_UCST_SCAN_NONE == g_lea_ucst_ctrl.next_scan) {
                    g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
                }
                if (g_lea_ucst_callback) {
                    app_dongle_le_race_scan_cnf_t cnf;
                    cnf.ret = ret;
                    cnf.start_scan = false;
                    g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_SCAN_CNF, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &cnf);
                }
            }
            if ((APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) ||
                (APP_LE_AUDIO_MODE_DISABLE == g_lea_ctrl.next_mode)) {
                /* [Switch streaming mode] UCST -> BCST */
                if (0 == app_le_audio_ucst_get_link_num_ex()) {
                    g_lea_ctrl.next_mode = APP_LE_AUDIO_MODE_NONE;

                    if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) {
                        g_lea_ctrl.curr_mode = APP_LE_AUDIO_MODE_BCST;
                    app_le_audio_start_broadcast();
                    } else {
                        g_lea_ctrl.curr_mode = APP_LE_AUDIO_MODE_NONE;
                        app_dongle_cm_lea_mode_t lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;
                        app_dongle_cm_notify_event(APP_DONGLE_CM_SOURCE_LEA, APP_DONGLE_CM_EVENT_SOURCE_END, BT_STATUS_SUCCESS, &lea_mode);
                    }
                }
                return;
            }
            if (APP_LE_AUDIO_UCST_SCAN_ENABLING == g_lea_ucst_ctrl.next_scan) {
                if (APP_LE_AUDIO_UCST_CONN_BONDED_DEVICE == g_lea_ucst_ctrl.curr_conn) {
                    if (BT_STATUS_SUCCESS != app_le_audio_ucst_start_scan(true)) {
                        g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
                    }
                } else {
                    if (BT_STATUS_SUCCESS != app_le_audio_ucst_start_scan(false)) {
                        g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
                    }
                }
                return;
            }
            if (APP_LE_AUDIO_UCST_SCAN_CS_ENABLING == g_lea_ucst_ctrl.next_scan) {
                if (BT_STATUS_SUCCESS != app_le_audio_ucst_start_scan_cs()) {
                    g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
                }
                return;
            }
            break;
        }
        default:
            break;
    }
}

void app_le_audio_ucst_handle_adv_report_ind(bt_status_t ret, bt_gap_le_ext_advertising_report_ind_t *ind)
{
    bt_bd_addr_t empty_addr = {0};
    uint8_t link_num;

    if (APP_LE_AUDIO_UCST_CONN_NONE == g_lea_ucst_ctrl.curr_conn) {
        //LE_AUDIO_MSGLOG_I("[APP][U] handle_adv_report, curr_conn:%x", 1, g_lea_ucst_ctrl.curr_conn);
        return;
    }

    if (BT_STATUS_SUCCESS != ret) {
        return;
    }

    /* check current link num */
    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == (link_num = app_le_audio_ucst_get_link_num_ex())) {
#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        LE_AUDIO_MSGLOG_I("[APP][U] handle_adv_report, link full", 0);
        g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
        app_le_audio_ucst_stop_scan();
        return;
#endif
    }


    if (!(ind->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_CONNECTABLE)) {
        /* ignore non-connectable adv */
        return;
    }

    /* check address */
    if ((0 == memcmp(ind->address.addr, empty_addr, sizeof(bt_bd_addr_t))) ||
        ((ind->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED) &&
         (0 == memcmp(ind->direct_address.addr, empty_addr, sizeof(bt_bd_addr_t))))) {
        LE_AUDIO_MSGLOG_I("[APP][U] handle_adv_report, event_type:%x invalid addr!", 1, ind->event_type);
        return;
    }

    if (app_le_audio_ucst_is_connected_device(&ind->address)) {
        /* ignore connected device */
        return;
    }

    /* upper layer scan device */
    if (APP_LE_AUDIO_UCST_CONN_LEA_DEVICE == g_lea_ucst_ctrl.curr_conn) {
        if (ind->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED) {
            if (!app_le_audio_ucst_is_bonded_device(&ind->address)) {
                /* ignore unknown device */
                return;
            }
        } else if (!app_le_audio_ucst_is_lea_adv(ind)) {
            /* ignore not le audio device */
            return;
        }

        app_le_audio_ucst_print_lea_adv_addr(&ind->address);
        return;
    }

    /* connect bonded device */
    if ((APP_LE_AUDIO_UCST_CONN_BONDED_DEVICE == g_lea_ucst_ctrl.curr_conn) ||
        (APP_LE_AUDIO_UCST_CONN_CS_AND_BONDED_DEVICE == g_lea_ucst_ctrl.curr_conn)) {
        if (app_le_audio_ucst_is_bonded_device(&ind->address)) {
            app_le_audio_ucst_print_lea_adv_addr(&ind->address);
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
            if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == link_num) {
                if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_waiting_conn_group) {
                    LE_AUDIO_MSGLOG_I("[APP][CSIP] handle_adv_report, link full scan 1, found group:%x", 1, g_lea_ucst_waiting_conn_group);
                    memcpy(&g_lea_ucst_waiting_conn_addr, &ind->address, sizeof(bt_addr_t));
                    app_le_audio_ucst_stop_scan();
                    app_le_audio_ucst_disconnect_other_group_device(g_lea_ucst_waiting_conn_group, AUDIO_LOCATION_FRONT_LEFT);
                }
            } else
#endif
            {
                app_le_audio_ucst_connect_device(&ind->address);
            }
            return;
        }
    }

    /* connect cs device */
    if ((APP_LE_AUDIO_UCST_CONN_COORDINATED_SET_BY_SIRK == g_lea_ucst_ctrl.curr_conn) ||
        (APP_LE_AUDIO_UCST_CONN_CS_AND_BONDED_DEVICE == g_lea_ucst_ctrl.curr_conn)) {
        if (app_le_audio_ucst_is_lea_adv(ind)) {
            uint16_t length = 0, index = 0;

            while (index < ind->data_length) {
                length = ind->data[index];

                if (0 == length) {
                    break;
                }

                if ((length >= 7) && ((ind->data[index + 1] == 0xF0) || (ind->data[index + 1] == 0x2E))) {
                    if (BT_STATUS_SUCCESS == ble_csip_verify_rsi(&ind->data[index + 2])) {
                        app_le_audio_ucst_print_lea_adv_addr(&ind->address);
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == link_num) {
                            if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_waiting_conn_group) {
                                LE_AUDIO_MSGLOG_I("[APP][CSIP] handle_adv_report, link full scan 2, found group:%x", 1, g_lea_ucst_waiting_conn_group);
                                memcpy(&g_lea_ucst_waiting_conn_addr, &ind->address, sizeof(bt_addr_t));
                                app_le_audio_ucst_stop_scan();
                                app_le_audio_ucst_disconnect_other_group_device(g_lea_ucst_waiting_conn_group, AUDIO_LOCATION_FRONT_LEFT);
                            }
                        } else
#endif
                        {
                            app_le_audio_ucst_connect_device(&ind->address);
                        }
                    } else {
                        LE_AUDIO_MSGLOG_I("[APP] csip_verify_rsi fail 3", 0);
                    }
                    return;
                }

                if ((length >= 10) && ((ind->data[index + 1] == 0xFF) && (ind->data[index + 2] == 0x94) && (ind->data[index + 5] == 0x2E))) {
                    if (BT_STATUS_SUCCESS == ble_csip_verify_rsi(&ind->data[index + 6])) {
                        app_le_audio_ucst_print_lea_adv_addr(&ind->address);
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == link_num) {
                            if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_waiting_conn_group) {
                                LE_AUDIO_MSGLOG_I("[APP][CSIP] handle_adv_report, link full scan 3, found group:%x", 1, g_lea_ucst_waiting_conn_group);
                                memcpy(&g_lea_ucst_waiting_conn_addr, &ind->address, sizeof(bt_addr_t));
                                app_le_audio_ucst_stop_scan();
                                app_le_audio_ucst_disconnect_other_group_device(g_lea_ucst_waiting_conn_group, AUDIO_LOCATION_FRONT_LEFT);
                            }
                        } else
#endif
                        {
                            app_le_audio_ucst_connect_device(&ind->address);
                        }
                    } else {
                        LE_AUDIO_MSGLOG_I("[APP] csip_verify_rsi fail 4", 0);
                    }
                    return;
                }
                index = index + length + 1;
            }
        }
    }
}

void app_le_audio_ucst_handle_set_white_list_cnf(bt_status_t ret)
{
    uint8_t i;

    LE_AUDIO_MSGLOG_I("[APP][U] LE_SET_WHITE_LIST_CNF, ret:%x", 1, ret);

    if (APP_LE_AUDIO_UCST_SET_WHITE_LIST_ADD_ON_GOING == g_lea_ucst_set_white_list.state) {
        app_le_audio_ucst_add_bonded_device_to_white_list();
    }
    if (APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING == g_lea_ucst_set_white_list.state) {
        g_lea_ucst_set_white_list.state = APP_LE_AUDIO_UCST_SET_WHITE_LIST_COMPLETE;
        if (g_lea_ucst_waiting_disconn_flag) {
            LE_AUDIO_MSGLOG_I("[APP][U] LE_SET_WHITE_LIST_CNF, wait disconn", 0);
            return;
        }
        /* check remove next white list */
        if (app_le_audio_ucst_check_delete_group_device()) {
            return;
        }

        for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
            if ((BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) &&
                (g_lea_ucst_link_info[i].add_white_list)) {
                g_lea_ucst_link_info[i].add_white_list = false;
                LE_AUDIO_MSGLOG_I("[APP] set_white_list_cnf, set_white_list.state:%x", 1, g_lea_ucst_set_white_list.state);
                app_le_audio_ucst_add_white_list(&(g_lea_ucst_link_info[i].addr));
                break;
            }
        }
    }
}

void app_le_audio_ucst_handle_connect_ind(bt_status_t ret, bt_gap_le_connection_ind_t *ind)
{
    app_le_audio_ucst_link_info_t *link_info = NULL;
    uint8_t link_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
    app_dongle_cm_lea_mode_t lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;

    if (APP_LE_AUDIO_MODE_UCST != g_lea_ctrl.curr_mode) {
        return;
    }

    if (NULL != (link_info = app_le_audio_ucst_find_connecting_link_info_by_peer_addr(&(ind->peer_addr), &link_idx))) {
        app_le_audio_timer_stop(link_info->le_connection_timer_handle);
        link_info->le_connection_timer_handle = NULL;
    }

    //g_lea_ucst_ctrl.is_creating_connection = false;

    if ((BT_STATUS_SUCCESS != ret) || (BT_HANDLE_INVALID == ind->connection_handle)) {
        LE_AUDIO_MSGLOG_I("[APP][U] LE_CONNECT_IND failed, handle:%x ret:%x curr_conn:%x", 3, ind->connection_handle, ret, g_lea_ucst_ctrl.curr_conn);
        app_le_audio_ucst_reset_link_info(link_idx);

        switch (g_lea_ucst_ctrl.curr_conn) {
            case APP_LE_AUDIO_UCST_CONN_LEA_DEVICE: {
                g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
                break;
            }
            case APP_LE_AUDIO_UCST_CONN_BONDED_DEVICE: {
                app_le_audio_ucst_connect_bonded_device(false);
                break;
            }
            case APP_LE_AUDIO_UCST_CONN_COORDINATED_SET_BY_SIRK: {
                app_le_audio_ucst_connect_coordinated_set(false);
                break;
            }
            case APP_LE_AUDIO_UCST_CONN_CS_AND_BONDED_DEVICE: {
                app_le_audio_ucst_connect_bonded_device(true);
                break;
            }
            default:
                break;
        }

        if (g_lea_ucst_callback) {
            app_dongle_le_race_connect_ind_t evt;

            evt.ret = ret;
            memcpy(&(evt.peer_addr), &(ind->peer_addr), sizeof(bt_addr_t));
            evt.group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
            evt.group_size = 0;
            g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_CONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
        }

        app_dongle_cm_notify_event(APP_DONGLE_CM_SOURCE_LEA, APP_DONGLE_CM_EVENT_SOURCE_STARTED, BT_STATUS_FAIL, &lea_mode);
        return;
    }

    /* LE connection timer may re-start scan.
        * Scan will be re-started if the coordinated set size of CSIS is more than one.
        */
    app_le_audio_ucst_stop_scan_all();

    g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;

    if (((APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) && (APP_LE_AUDIO_MODE_NONE == g_lea_ctrl.next_mode)) ||
        (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode)) {
        /* [Switch streaming mode] UCST -> BCST */
        app_le_audio_ucst_reset_link_info(link_idx);
        app_le_audio_ucst_disconnect(ind->connection_handle);
        return;
    }

    /* Timer may be expired. */
    if (NULL == link_info) {
        link_idx = app_le_audio_ucst_get_link_idx(ind->connection_handle);
        LE_AUDIO_MSGLOG_I("[APP][U] link_idx:%x, handle:%x", 2, link_idx, ind->connection_handle);
        if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == link_idx) {
            link_info = app_le_audio_ucst_get_available_link_info_for_new_le_connection(&link_idx);
            if (NULL == link_info) {
                /* Link full. */
                app_le_audio_ucst_disconnect(ind->connection_handle);
                return;
            }
        } else {
            /* Duplicate connect_ind? */
            return;
        }
    }

    uint8_t bond_idx;

    /* link_info: update content */
    memset(link_info, 0, sizeof(app_le_audio_ucst_link_info_t));
    link_info->handle = ind->connection_handle;
    memcpy(&(link_info->addr), &(ind->peer_addr), sizeof(bt_addr_t));
    link_info->curr_interval = ind->conn_interval;
    link_info->group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    link_info->bond_idx = APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID;

    /* search bonding info */
    if (APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID == (bond_idx = app_le_audio_ucst_get_bonded_list_idx(&(link_info->addr)))) {
        /* new device */

        if (APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM == g_lea_ucst_bonded_list.num) {
            /* bonded list full, delete oldest group */
            app_le_audio_ucst_delete_oldest_group_from_bonded_list();
        }
        if (APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING != g_lea_ucst_set_white_list.state) {
            /* add to white list */
            app_le_audio_ucst_add_white_list(&(link_info->addr));
        } else {
            /* wait remove white list finish */
            link_info->add_white_list = true;
        }

    } else {
        /* bonded device */

        /* link_info: update bonding info */
        link_info->bond_idx = bond_idx;
        link_info->group_size = g_lea_ucst_bonded_list.device[bond_idx].group_size;
        link_info->group_id = g_lea_ucst_bonded_list.device[bond_idx].group_id;
#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.curr_group) {
            g_lea_ucst_ctrl.curr_group = link_info->group_id;
            LE_AUDIO_MSGLOG_I("[APP][U] LE_CONNECT_IND, active_group:%x", 1, g_lea_ucst_ctrl.curr_group);
        }
#endif
        g_lea_ucst_ctrl.latest_group = link_info->group_id;

        /* bonded_list: update link idx */
        g_lea_ucst_bonded_list.device[bond_idx].link_idx = link_idx;

        /* group_info: update link idx */
        app_le_audio_ucst_add_link_to_group(link_info->group_id, link_idx);

        if (!g_lea_ucst_bonded_list.device[bond_idx].in_white_list) {
             g_lea_ucst_bonded_list.device[bond_idx].in_white_list = true;
            if (APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING != g_lea_ucst_set_white_list.state) {
                /* add to white list */
                app_le_audio_ucst_add_white_list(&(link_info->addr));
            } else {
                /* wait remove white list finish */
                link_info->add_white_list = true;
            }
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] LE_CONNECT_IND, handle[%x]:%x addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 9,
                      link_idx,
                      link_info->handle,
                      link_info->addr.type,
                      link_info->addr.addr[5],
                      link_info->addr.addr[4],
                      link_info->addr.addr[3],
                      link_info->addr.addr[2],
                      link_info->addr.addr[1],
                      link_info->addr.addr[0]);

    /* update connection parameter */
    if (APP_LE_AUDIO_UCST_PAUSE_STREAM_NONE == g_lea_ucst_ctrl.pause_stream) {
        LE_AUDIO_MSGLOG_I("[APP] conn_interval, handle:%x interval:%x->%x set:8", 3,
                          link_info->handle,
                          link_info->curr_interval,
                          link_info->next_interval);

        link_info->next_interval = APP_LE_AUDIO_CONN_INTERVAL_CONFIG;
        if (BT_STATUS_SUCCESS != app_le_audio_ucst_update_connection_parameter(link_info->handle, APP_LE_AUDIO_CONN_INTERVAL_CONFIG, 0x0003)) {
            link_info->next_interval = 0;
        }
    }

    app_dongle_cm_notify_event(APP_DONGLE_CM_SOURCE_LEA, APP_DONGLE_CM_EVENT_SOURCE_STARTED, BT_STATUS_SUCCESS, &lea_mode);
}

void app_le_audio_ucst_handle_connection_update_ind(bt_status_t ret, bt_gap_le_connection_update_ind_t *ind)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(ind->conn_handle))) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] LE_CONNECTION_UPDATE_IND, handle:%x ret:%x interval:%x %x->%x", 5,
                      ind->conn_handle, ret,
                      ind->conn_interval,
                      p_info->curr_interval,
                      p_info->next_interval);

    p_info->curr_interval = ind->conn_interval;
    if (p_info->next_interval == ind->conn_interval) {
        p_info->next_interval = 0;
    }
}

void app_le_audio_ucst_handle_bonding_complete_ind(bt_status_t ret, bt_gap_le_bonding_complete_ind_t *ind)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(ind->handle))) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] LE_BONDING_COMPLETE_IND, handle:%x ret:%x", 2, ind->handle, ret);

    /* enter next step */
    app_le_audio_ucst_exchange_mtu(p_info->handle);
}

void app_le_audio_ucst_handle_exchange_mtu_rsp(bt_status_t ret, bt_gatt_exchange_mtu_rsp_t *rsp)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(rsp->connection_handle))) {
        return;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_EXCHANGE_MTU != p_info->next_state) {
        LE_AUDIO_MSGLOG_I("[APP][U] exchange_mtu_rsp state NOT match, handle:%x state:%x->%x", 3,
                          rsp->connection_handle,
                          p_info->curr_state,
                          p_info->next_state);
        return;
    }

    if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) {
        /* [Switch streaming mode] UCST -> BCST */
        g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
        ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                            APP_LE_AUDIO_EVENT_STOP_SCAN_NEW_DEVICE, NULL, 0,
                            NULL, 0);
        return;
    }

    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_EXCHANGE_MTU;
    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;

    LE_AUDIO_MSGLOG_I("[APP][U] exchange_mtu_rsp, handle:%x state:%x->%x MTU:%d", 4,
                      rsp->connection_handle,
                      p_info->curr_state,
                      p_info->next_state,
                      rsp->server_rx_mtu);

    /* enter next step */
    bt_gattc_discovery_status_t status;
    status = bt_gattc_discovery_start(BT_GATTC_DISCOVERY_USER_LE_AUDIO, rsp->connection_handle, FALSE);

    LE_AUDIO_MSGLOG_I("[APP] exchange_mtu_rsp, start discovery ret:%x", 1, status);

}

void app_le_audio_ucst_handle_set_cig_parameter_cnf(bt_status_t ret, bt_gap_le_set_cig_params_cnf_t *cnf)
{
    if (BT_STATUS_SUCCESS != ret) {
        /* To do: handle error case */
        LE_AUDIO_MSGLOG_I("[APP][U] LE_SET_CIG_PARAMETERS_CNF fail, ret:%x", 1, ret);
        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] LE_SET_CIG_PARAMETERS_CNF, target:%x->%x stream_state:%x->%x", 4,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state);

    if (APP_LE_AUDIO_UCST_STREAM_STATE_SET_CIG_PARAMETER != g_lea_ucst_ctrl.next_stream_state) {
        /* To do: check */
        return;
    }

    g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_SET_CIG_PARAMETER;
    g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;

    g_lea_ucst_ctrl.is_cig_created = true;

    if (APP_LE_AUDIO_UCST_CIS_MAX_NUM < cnf->cis_count) {
        cnf->cis_count = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
    }

    g_lea_ucst_ctrl.cis_num = cnf->cis_count;

    uint8_t i, tmp;
    for (i = 0; i < g_lea_ucst_ctrl.cis_num; i++) {
        g_lea_ucst_cis_info[i].cis_handle = cnf->cis_connection_handle[i];
        g_lea_ucst_cis_info[i].cis_status = APP_LE_AUDIO_UCST_CIS_IDLE;
        g_lea_ucst_cis_info[i].acl_handle = BT_HANDLE_INVALID;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] LE_SET_CIG_PARAMETERS_CNF, cig_id:%x cis_num:%x cis_handle:%x %x", 4, cnf->cig_id, cnf->cis_count,
                      g_lea_ucst_cis_info[0].cis_handle, g_lea_ucst_cis_info[1].cis_handle);

    /* check target */
    if (app_le_audio_ucst_check_pause_stream() ||
        (APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
        if (BT_STATUS_SUCCESS != app_le_audio_close_audio_transmitter()) {
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
        }
        return;
    }

    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t ase_idx = APP_LE_AUDIO_UCST_ASE_IDX_0;

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        p_info = &g_lea_ucst_link_info[i];

        if ((BT_HANDLE_INVALID != p_info->handle) &&
            ((APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC == p_info->curr_state) ||
             (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS == p_info->curr_state)) &&
            (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == p_info->next_state) &&
            (0 == p_info->wait_event.wait_ase_event)) {

            ase_idx = APP_LE_AUDIO_UCST_ASE_IDX_0;

            LE_AUDIO_MSGLOG_I("[APP] LE_SET_CIG_PARAMETERS_CNF, i:%x handle:%x state:%x->%x source_ase_num:%x", 5,
                              i, p_info->handle, p_info->curr_state, p_info->next_state, p_info->source_ase_num);
            if (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) {
                if (p_info->source_ase_num) {
                    ase_idx = p_info->source_ase_idx;
                }
            }

            LE_AUDIO_MSGLOG_I("[APP] LE_SET_CIG_PARAMETERS_CNF, ase[%x]:%x ase_state:%x", 3, ase_idx, p_info->ase[ase_idx].id, p_info->ase[ase_idx].curr_state);

            if (ASE_STATE_QOS_CONFIGURED == p_info->ase[ase_idx].curr_state) {
                p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE;
                if (BT_STATUS_SUCCESS != app_le_audio_ucst_enable_ase(p_info->handle)) {
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                }


            } else if (ASE_STATE_CODEC_CONFIGURED == p_info->ase[ase_idx].curr_state) {
                p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC;
                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
                if (BT_STATUS_SUCCESS != app_le_audio_ucst_config_qos(p_info->handle)) {
                    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                }
            }
        }
    }
}

void app_le_audio_ucst_handle_cis_established_ind(bt_status_t ret, bt_gap_le_cis_established_ind_t *ind)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t link_idx, cis_idx, i;

    if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == (link_idx = app_le_audio_ucst_get_link_idx_by_cis(ind->connection_handle, &cis_idx))) {
        LE_AUDIO_MSGLOG_I("[APP][U] LE_CIS_ESTABLISHED_IND, link not exist (cis_hdl:%x)", 1, ind->connection_handle);
        return;
    }

    p_info = &g_lea_ucst_link_info[link_idx];

    LE_AUDIO_MSGLOG_I("[APP][U] LE_CIS_ESTABLISHED_IND, handle:%x state:%x->%x cis_handle[%x]:%x cis_status:%x", 6,
                      p_info->handle,
                      p_info->curr_state,
                      p_info->next_state,
                      cis_idx,
                      g_lea_ucst_cis_info[cis_idx].cis_handle,
                      g_lea_ucst_cis_info[cis_idx].cis_status);

    app_le_audio_ucst_send_prepare_vcmd(g_lea_ucst_link_info[link_idx].handle, APP_LE_AUDIO_PREPARE_VCMD_MODE_CONN, 0);

    if (BT_STATUS_SUCCESS != ret) {
        LE_AUDIO_MSGLOG_I("[APP][U] LE_CIS_ESTABLISHED_IND fail, cis_handle:%x ret:%x", 2, ind->connection_handle, ret);
        g_lea_ucst_cis_info[cis_idx].cis_status = APP_LE_AUDIO_UCST_CIS_IDLE;
        g_lea_ucst_cis_info[cis_idx].acl_handle = BT_HANDLE_INVALID;
        if ((p_info->ase_releasing) &&
            (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC == p_info->next_state)) {
            if (0 != p_info->wait_event.wait_ase_event) {
                return;
            }

            p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC;
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            p_info->ase_releasing = false;

        } else if (APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE <= p_info->curr_state) {
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE;
            if (BT_STATUS_SUCCESS != app_le_audio_ucst_disable_ase(p_info->handle)) {
                p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            }

            /* check the other CIS in the same LE link, and disconnect it. */
            if (APP_LE_AUDIO_UCST_CIS_MAX_NUM == p_info->cis_num) {
                i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
                while (0 != i) {
                    i--;
                    if (BT_HANDLE_INVALID != g_lea_ucst_cis_info[i].cis_handle &&
                        p_info->handle == g_lea_ucst_cis_info[i].acl_handle &&
                        (APP_LE_AUDIO_UCST_CIS_IDLE != g_lea_ucst_cis_info[i].cis_status &&
                         APP_LE_AUDIO_UCST_CIS_DISCONNECTING != g_lea_ucst_cis_info[i].cis_status)) {
                        app_le_audio_ucst_disconnect(g_lea_ucst_cis_info[i].cis_handle);
                    }
                }
            }
        }

        if (0 == app_le_audio_ucst_get_cis_num()) {
            if (app_le_audio_ucst_check_close_audio_stream()) {
                return;
            }

            if ((APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM == g_lea_ucst_ctrl.curr_stream_state) &&
                (APP_LE_AUDIO_UCST_STREAM_STATE_REMOVE_CIG != g_lea_ucst_ctrl.next_stream_state)) {
                if (g_lea_ucst_ctrl.is_cig_created) {
                    /* remove cig */
                    g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_REMOVE_CIG;
                    if (BT_STATUS_SUCCESS != app_le_audio_ucst_remove_cig()) {
                        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
                    }
                }
            }
            return;
        }

        cis_idx = APP_LE_AUDIO_UCST_CIS_MAX_NUM;

    } else {
        char conn_string[40] = {0};

        snprintf((char *)conn_string, 40, "CIS connected, handle: 0x%04x\r\n", ind->connection_handle);
        bt_app_common_at_cmd_print_report(conn_string);

        g_lea_ucst_cis_info[cis_idx].cis_status = APP_LE_AUDIO_UCST_CIS_CREATED;

        /* if other cis in same LE link is in creating state, wait for it */
        if (APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS == p_info->next_state) {
            i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
            while (0 != i) {
                i--;
                if ((p_info->handle == g_lea_ucst_cis_info[i].acl_handle) &&
                    (APP_LE_AUDIO_UCST_CIS_CREATING == g_lea_ucst_cis_info[i].cis_status)) {
                    return;
                }
            }

            p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS;
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
        }
    }

    /* Check setup ISO data path */
    uint8_t j, tmp;
    bool found = false;

    i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;

    while (0 != i) {
        i--;
        if (BT_HANDLE_INVALID != g_lea_ucst_cis_info[i].cis_handle) {
            LE_AUDIO_MSGLOG_I("[APP] CIS_ESTABLISHED_IND, check handle:%x cis_handle[%x]:%x cis_status:%x", 4,
                              g_lea_ucst_cis_info[i].acl_handle,
                              i,
                              g_lea_ucst_cis_info[i].cis_handle,
                              g_lea_ucst_cis_info[i].cis_status);

            if (APP_LE_AUDIO_UCST_CIS_CREATING == g_lea_ucst_cis_info[i].cis_status) {
                /* wait the other cis established */
                return;
            }

            if (APP_LE_AUDIO_UCST_CIS_CREATED == g_lea_ucst_cis_info[i].cis_status) {
                for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (j = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
                        continue;
                    }
#else
                    j = tmp;
#endif
                    /* check link state */
                    p_info = &g_lea_ucst_link_info[j];
                    if (p_info->handle == g_lea_ucst_cis_info[i].acl_handle) {
                        if ((APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS == p_info->curr_state) &&
                            (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == p_info->next_state) &&
                            (!p_info->ase_releasing)) {
                            if (i != cis_idx) {
                                cis_idx = i;
                                link_idx = j;
                                found = true;
                                break;
                            }
                        } else if (i == cis_idx) {
                            cis_idx = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
                        }
                        break;
                    }
                }
                if (found) {
                    break;
                }
            }
        }
    }

    if (APP_LE_AUDIO_UCST_CIS_MAX_NUM == cis_idx) {
        /* no cis created? */
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP] CIS_ESTABLISHED_IND, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

    if (app_le_audio_ucst_check_pause_stream() || (g_lea_ucst_ctrl.release) ||
        (APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
        app_le_audio_ucst_disable_ase_when_state_match(APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS);
        return;
    }

    bt_gap_le_iso_data_path_id_t iso_data_path_id = 0x01;
    p_info = &g_lea_ucst_link_info[link_idx];

    if (0 != p_info->sink_ase_num) {
        uint8_t sink_ase_num = 0;
        if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
            sink_ase_num = p_info->sink_ase_num;
        } else {
            sink_ase_num = p_info->sink_location_num;
        }
        LE_AUDIO_MSGLOG_I("[APP] CIS_ESTABLISHED_IND, sink_ase_num:%x %x", 2, sink_ase_num, p_info->sink_ase_num);
        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            if ((APP_LE_AUDIO_UCST_IS_CALL_MODE) && (p_info->sink_ase_num >= (sink_ase_num * APP_LE_AUDIO_UCST_MAX_MODE_NUM))) {
                LE_AUDIO_MSGLOG_I("[APP] CIS_ESTABLISHED_IND, sink_ase_num+i:%x curr_state:%x", 2, sink_ase_num + i, p_info->ase[sink_ase_num + i].curr_state);
                if (ASE_STATE_STREAMING == p_info->ase[sink_ase_num + i].curr_state) {
                    break;
                }
            } else {
                LE_AUDIO_MSGLOG_I("[APP] CIS_ESTABLISHED_IND, i:%x curr_state:%x", 2, i, p_info->ase[i].curr_state);
                if (ASE_STATE_STREAMING == p_info->ase[i].curr_state) {
                    break;
                }
            }
        }

        LE_AUDIO_MSGLOG_I("[APP] CIS_ESTABLISHED_IND, i:%x sink_ase_num:%x", 2, i, sink_ase_num);
        if (i == sink_ase_num) {
            return;
        }

        if (((1 == p_info->sink_location_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->sink_location)) ||
            ((2 == p_info->sink_location_num) && (0 != cis_idx))) {
            iso_data_path_id = 0x02;
        }
        LE_AUDIO_MSGLOG_I("[APP] CIS_ESTABLISHED_IND, handle:%x sink_location:%x(num:%x) iso_data_path_id:%x", 4,
                          p_info->handle,
                          p_info->sink_location,
                          p_info->sink_location_num,
                          iso_data_path_id);

    } else if (0 != p_info->source_ase_num) {
        if (((1 == p_info->source_location_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->source_location)) ||
            ((2 == p_info->source_location_num) && (0 != cis_idx))) {
            iso_data_path_id = 0x02;
        }
        LE_AUDIO_MSGLOG_I("[APP] CIS_ESTABLISHED_IND, handle:%x source_location:%x(num:%x) iso_data_path_id:%x", 4,
                          p_info->handle,
                          p_info->source_location,
                          p_info->source_location_num,
                          iso_data_path_id);
    }

    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH;
    g_lea_ucst_cis_info[cis_idx].cis_status = APP_LE_AUDIO_UCST_CIS_SETUP_DATA_PATH;
    app_le_audio_setup_iso_data_path(g_lea_ucst_cis_info[cis_idx].cis_handle, BT_GAP_LE_ISO_DATA_PATH_DIRECTION_OUTPUT, iso_data_path_id);
}

void app_le_audio_ucst_handle_setup_iso_data_path_cnf(bt_status_t ret, bt_gap_le_setup_iso_data_path_cnf_t *cnf)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t link_idx, cis_idx;

    if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == (link_idx = app_le_audio_ucst_get_link_idx_by_cis(cnf->handle, &cis_idx))) {
        LE_AUDIO_MSGLOG_I("[APP][U] LE_SETUP_ISO_DATA_PATH_CNF, link not exist (cis_hdl:%x)", 1, cnf->handle);
        return;
    }

    p_info = &g_lea_ucst_link_info[link_idx];

    LE_AUDIO_MSGLOG_I("[APP][U] LE_SETUP_ISO_DATA_PATH_CNF, handle:%x state:%x->%x cis_handle[%x]:%x cis_status:%x", 6,
                      p_info->handle,
                      p_info->curr_state,
                      p_info->next_state,
                      cis_idx,
                      g_lea_ucst_cis_info[cis_idx].cis_handle,
                      g_lea_ucst_cis_info[cis_idx].cis_status);

    if (BT_STATUS_SUCCESS != ret) {
        /* To do: handle error case */
        LE_AUDIO_MSGLOG_I("[APP][U] LE_SETUP_ISO_DATA_PATH_CNF fail, cis_handle:%x ret:%x", 2, cnf->handle, ret);
        return;
    }

    g_lea_ucst_cis_info[cis_idx].cis_status = APP_LE_AUDIO_UCST_CIS_STREAMING;

    /* Check ASE release */
    if (p_info->ase_releasing) {
        return;
    }

    if (APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH != p_info->next_state) {
        return;
    }

    p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH;
    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;

    /* check pause stream */
    uint8_t i, tmp;
    if (app_le_audio_ucst_check_pause_stream() ||
        (APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
        (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
        app_le_audio_ucst_disable_ase_when_setup_iso_data_path();
        return;
    }

    /* check if other cis in same LE link needed to setup iso data path */
    i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
    cis_idx = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
    while (0 != i) {
        i--;
        if ((APP_LE_AUDIO_UCST_CIS_CREATED == g_lea_ucst_cis_info[i].cis_status) &&
            (p_info->handle == g_lea_ucst_cis_info[i].acl_handle)) {
            cis_idx = i;
            break;
        }
    }

    if (APP_LE_AUDIO_UCST_CIS_MAX_NUM == cis_idx) {
        if (APP_LE_AUDIO_UCST_IS_CALL_MODE) {
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_START_READY;
            app_le_audio_ucst_set_receiver_start_ready(p_info->handle);

        } else {
            p_info->curr_state = APP_LE_AUDIO_UCST_LINK_STATE_STREAMING;
            p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING;
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
        }

        /* Check setup ISO data path */
        cis_idx = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
        for (i = 0; i < APP_LE_AUDIO_UCST_CIS_MAX_NUM; i++) {
            if ((BT_HANDLE_INVALID != g_lea_ucst_cis_info[i].cis_handle) &&
                (APP_LE_AUDIO_UCST_CIS_CREATED == g_lea_ucst_cis_info[i].cis_status)) {
                for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (link_idx = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
                        continue;
                    }
#else
                    link_idx = tmp;
#endif
                    p_info = &g_lea_ucst_link_info[link_idx];
                    if ((p_info->handle == g_lea_ucst_cis_info[i].acl_handle) &&
                        (false == p_info->ase_releasing)) {
                        cis_idx = i;
                        break;
                    }
                }
            }
        }

        if (APP_LE_AUDIO_UCST_CIS_MAX_NUM == cis_idx) {
            return;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP] SETUP_ISO_DATA_PATH_CNF, handle:%x state:%x->%x cis_handle[%x]:%x cis_status:%x", 6,
                      p_info->handle,
                      p_info->curr_state,
                      p_info->next_state,
                      cis_idx,
                      g_lea_ucst_cis_info[cis_idx].cis_handle,
                      g_lea_ucst_cis_info[cis_idx].cis_status);

    bt_gap_le_iso_data_path_id_t iso_data_path_id = 0x01;

    if (0 != p_info->sink_ase_num) {
        uint8_t sink_ase_num = 0;
        if ((0 == p_info->sink_location_num) || (p_info->sink_location_num >= p_info->sink_ase_num)) {
            sink_ase_num = p_info->sink_ase_num;
        } else {
            sink_ase_num = p_info->sink_location_num;
        }
        /* SINK ASE */
        for (i = 0; i < sink_ase_num; i++) {
            if ((p_info->sink_ase_num >= (sink_ase_num * APP_LE_AUDIO_UCST_MAX_MODE_NUM)) && (APP_LE_AUDIO_UCST_IS_CALL_MODE)) {
                if (ASE_STATE_STREAMING == p_info->ase[sink_ase_num + i].curr_state) {
                    break;
                }
            } else if (ASE_STATE_STREAMING == p_info->ase[i].curr_state) {
                break;
            }
        }

        if (i == sink_ase_num) {
            return;
        }

        if (((1 == p_info->sink_location_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->sink_location)) ||
            ((2 == p_info->sink_location_num) && (0 != cis_idx))) {
            iso_data_path_id = 0x02;
        }
        LE_AUDIO_MSGLOG_I("[APP] SETUP_ISO_DATA_PATH_CNF, handle:%x sink_location:%x(num:%x) iso_data_path_id:%x", 4,
                          p_info->handle,
                          p_info->sink_location,
                          p_info->sink_location_num,
                          iso_data_path_id);

    } else if (0 != p_info->source_ase_num) {
        if (((1 == p_info->source_location_num) && (AUDIO_LOCATION_FRONT_RIGHT == p_info->source_location)) ||
            ((2 == p_info->source_location_num) && (0 != cis_idx))) {
            iso_data_path_id = 0x02;
        }
        LE_AUDIO_MSGLOG_I("[APP] SETUP_ISO_DATA_PATH_CNF, handle:%x source_location:%x(num:%x) iso_data_path_id:%x", 4,
                          p_info->handle,
                          p_info->source_location,
                          p_info->source_location_num,
                          iso_data_path_id);
    }

    p_info->next_state = APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH;
    g_lea_ucst_cis_info[cis_idx].cis_status = APP_LE_AUDIO_UCST_CIS_SETUP_DATA_PATH;
    app_le_audio_setup_iso_data_path(g_lea_ucst_cis_info[cis_idx].cis_handle, BT_GAP_LE_ISO_DATA_PATH_DIRECTION_OUTPUT, iso_data_path_id);
}

void app_le_audio_ucst_handle_cis_terminated_ind(bt_status_t ret, bt_gap_le_cis_terminated_ind_t *ind)
{
    uint8_t link_idx, cis_idx;
    if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == (link_idx = app_le_audio_ucst_get_link_idx_by_cis(ind->connection_handle, &cis_idx))) {
        LE_AUDIO_MSGLOG_I("[APP][U] LE_CIS_TERMINATED_IND, link not exist (cis_hdl:%x)", 1, ind->connection_handle);
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] LE_CIS_TERMINATED_IND, handle:%x state:%x->%x cis_handle[%x]:%x cis_status:%x reason:%x cis_num", 8,
                      g_lea_ucst_link_info[link_idx].handle,
                      g_lea_ucst_link_info[link_idx].curr_state,
                      g_lea_ucst_link_info[link_idx].next_state,
                      cis_idx,
                      g_lea_ucst_cis_info[cis_idx].cis_handle,
                      g_lea_ucst_cis_info[cis_idx].cis_status,
                      ind->reason,
                      g_lea_ucst_link_info[link_idx].cis_num);

    uint8_t cis_num = 0;

    app_le_audio_ucst_send_prepare_vcmd(g_lea_ucst_link_info[link_idx].handle, APP_LE_AUDIO_PREPARE_VCMD_MODE_DISCONN, 0);

    if (BT_STATUS_SUCCESS != ret) {
        LE_AUDIO_MSGLOG_I("[APP][U] LE_CIS_TERMINATED_IND fail, cis_handle:%x ret:%x", 2, ind->connection_handle, ret);
        cis_num = app_le_audio_ucst_get_cis_num();

    } else {
        char conn_string[40] = {0};
        snprintf((char *)conn_string, 40, "CIS disconnected, handle: 0x%04x\r\n", ind->connection_handle);
        bt_app_common_at_cmd_print_report(conn_string);

        g_lea_ucst_cis_info[cis_idx].acl_handle = BT_HANDLE_INVALID;
        g_lea_ucst_cis_info[cis_idx].cis_status = APP_LE_AUDIO_UCST_CIS_IDLE;

        uint8_t tmp_cis_idx, i;

        i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
        cis_idx = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
        tmp_cis_idx = APP_LE_AUDIO_UCST_CIS_MAX_NUM;

        while (i > 0) {
            i--;
            if ((BT_HANDLE_INVALID != g_lea_ucst_cis_info[i].cis_handle) &&
                (BT_HANDLE_INVALID != g_lea_ucst_cis_info[i].acl_handle)) {
                cis_num++;

                LE_AUDIO_MSGLOG_I("[APP] CIS_TERMINATED_IND, check handle:%x cis_handle:%x cis_status:%x", 3,
                                  g_lea_ucst_cis_info[i].acl_handle,
                                  g_lea_ucst_cis_info[i].cis_handle,
                                  g_lea_ucst_cis_info[i].cis_status);

                if (APP_LE_AUDIO_UCST_CIS_DISCONNECTING == g_lea_ucst_cis_info[i].cis_status) {
                    if (g_lea_ucst_link_info[link_idx].handle == g_lea_ucst_cis_info[i].acl_handle) {
                        cis_idx = i;
                    } else {
                        tmp_cis_idx = i;
                    }
                }
            }
        }

        if (APP_LE_AUDIO_UCST_CIS_MAX_NUM != cis_idx) {
            /* delete the other CIS in the same LE link */
            LE_AUDIO_MSGLOG_I("[APP] disconnect CIS, handle:%x cis_handle[%x]:%x", 3,
                              g_lea_ucst_cis_info[cis_idx].acl_handle,
                              cis_idx,
                              g_lea_ucst_cis_info[cis_idx].cis_handle);

            app_le_audio_ucst_disconnect(g_lea_ucst_cis_info[cis_idx].cis_handle);
            return;
        } else if (BT_HCI_STATUS_CONNECTION_TIMEOUT == ind->reason &&
                   1 < g_lea_ucst_link_info[link_idx].cis_num) {
            /* When the LE link creates more than one CIS link, disconnect LE link if one of the CIS links disconnects because of CONNTECTION_TIMEOUT.
             * It may not be re-created successfully. Therefore, disconnect LE link and start over simply.
             */
            g_lea_ucst_link_info[link_idx].next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL;
            app_le_audio_ucst_disconnect(g_lea_ucst_link_info[link_idx].handle);
            return;
        }

        if ((APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_CIS == g_lea_ucst_link_info[link_idx].next_state) ||
            (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL == g_lea_ucst_link_info[link_idx].next_state) ||
            (BT_HCI_STATUS_CONNECTION_TIMEOUT == ind->reason)) {
            g_lea_ucst_link_info[link_idx].curr_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS;
            g_lea_ucst_link_info[link_idx].next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
        }

        if (APP_LE_AUDIO_UCST_CIS_MAX_NUM != tmp_cis_idx) {
            /* delete the other CIS in group mate LE link */
            LE_AUDIO_MSGLOG_I("[APP] disconnect CIS, handle:%x cis_handle[%x]:%x", 3,
                              g_lea_ucst_cis_info[tmp_cis_idx].acl_handle,
                              tmp_cis_idx,
                              g_lea_ucst_cis_info[tmp_cis_idx].cis_handle);

            app_le_audio_ucst_disconnect(g_lea_ucst_cis_info[tmp_cis_idx].cis_handle);
            return;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP] CIS_TERMINATED_IND, cis_num:%x target:%x->%x stream_state:%x->%x", 5, cis_num,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state);
    if (0 < cis_num) {
        return;
    }

    /* no cis exist */
    if (app_le_audio_ucst_check_close_audio_stream()) {
        return;
    }

    if ((APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM == g_lea_ucst_ctrl.curr_stream_state) &&
        (APP_LE_AUDIO_UCST_STREAM_STATE_REMOVE_CIG != g_lea_ucst_ctrl.next_stream_state)) {
        if (g_lea_ucst_ctrl.is_cig_created) {
            /* remove cig */
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_REMOVE_CIG;
            if (BT_STATUS_SUCCESS != app_le_audio_ucst_remove_cig()) {
                g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
            }
        }
    }
}

void app_le_audio_ucst_handle_remove_cig_cnf(bt_status_t ret, bt_gap_le_remove_cig_cnf_t *cnf)
{
    uint8_t i, tmp;
    if (BT_STATUS_SUCCESS != ret) {
        /* To do: handle error case */
        LE_AUDIO_MSGLOG_I("[APP][U] LE_REMOVE_CIG_CNF fail, ret:%x", 1, ret);
        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] LE_REMOVE_CIG_CNF, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

    for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
        if (BT_HANDLE_INVALID != g_lea_ucst_prepare_vcmd_disconn_handle[i]) {
            app_le_audio_ucst_send_prepare_vcmd(g_lea_ucst_prepare_vcmd_disconn_handle[i], APP_LE_AUDIO_PREPARE_VCMD_MODE_DISCONN, 0);
        }
        if (BT_HANDLE_INVALID != g_lea_ucst_prepare_vcmd_conn_handle[i]) {
            app_le_audio_ucst_send_prepare_vcmd(g_lea_ucst_prepare_vcmd_conn_handle[i], APP_LE_AUDIO_PREPARE_VCMD_MODE_CONN, 0);
        }
    }
    if (APP_LE_AUDIO_UCST_STREAM_STATE_REMOVE_CIG != g_lea_ucst_ctrl.next_stream_state) {
        /* To do: check */
        return;
    }

    g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
    g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;

    LE_AUDIO_MSGLOG_I("[APP][U] LE_REMOVE_CIG_CNF, cig_id:%x", 1, cnf->cig_id);

    g_lea_ucst_ctrl.is_cig_created = false;
    g_lea_ucst_ctrl.cis_num = 0;
    g_lea_ucst_ctrl.set_endpoint_tx_ready = false;

    i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
    while (i > 0) {
        i--;
        g_lea_ucst_cis_info[i].cis_handle = BT_HANDLE_INVALID;
        g_lea_ucst_cis_info[i].acl_handle = BT_HANDLE_INVALID;
        g_lea_ucst_cis_info[i].cis_status = APP_LE_AUDIO_UCST_CIS_IDLE;
    }

    if (app_le_audio_ucst_check_pause_stream() || (g_lea_ucst_ctrl.release) ||
        (((APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
          (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) &&
         ((APP_LE_AUDIO_UCST_TARGET_NONE == g_lea_ucst_ctrl.next_target) ||
          (APP_LE_AUDIO_MODE_DISABLE == g_lea_ucst_ctrl.next_target)))) {

        g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        app_le_audio_ucst_reset_release();
        if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) {
            /* [Switch streaming mode] UCST -> BCST */
            app_le_audio_start_broadcast();

        } else if (APP_LE_AUDIO_MODE_DISABLE == g_lea_ctrl.next_mode) {
            g_lea_ctrl.curr_mode = APP_LE_AUDIO_MODE_NONE;
            g_lea_ctrl.next_mode = APP_LE_AUDIO_MODE_NONE;
            app_dongle_cm_lea_mode_t lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;
            app_dongle_cm_notify_event(APP_DONGLE_CM_SOURCE_LEA, APP_DONGLE_CM_EVENT_SOURCE_END, BT_STATUS_SUCCESS, &lea_mode);
        }
        return;
    }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    bool change_active_device = false;
    LE_AUDIO_MSGLOG_I("[APP][U] REMOVE_CIG_CNF, active:%x->%x", 2,
                      g_lea_ucst_ctrl.curr_group,
                      g_lea_ucst_ctrl.next_group);
    if ((APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.curr_group) &&
        (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.next_group)) {
        change_active_device = true;
    }
#endif

    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
            continue;
        }
#else
        i = tmp;
#endif
        if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) {
            LE_AUDIO_MSGLOG_I("[APP] REMOVE_CIG_CNF, check handle:%x state:%x->%x w_ase:%x", 4, g_lea_ucst_link_info[i].handle,
                              g_lea_ucst_link_info[i].curr_state, g_lea_ucst_link_info[i].next_state,
                              g_lea_ucst_link_info[i].wait_event.wait_ase_event);
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
            if (change_active_device && (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC > g_lea_ucst_link_info[i].curr_state)) {
                continue;
            }
#endif
            if (!(((APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC == g_lea_ucst_link_info[i].curr_state) ||
                   (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS == g_lea_ucst_link_info[i].curr_state)) &&
                  ((APP_LE_AUDIO_UCST_LINK_STATE_IDLE == g_lea_ucst_link_info[i].next_state) ||
                   (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL == g_lea_ucst_link_info[i].next_state)) &&
                  (0 == g_lea_ucst_link_info[i].wait_event.wait_ase_event))) {
                return;
            }
        }
    }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    if (change_active_device) {
        g_lea_ucst_ctrl.curr_group = g_lea_ucst_ctrl.next_group;
        g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
        ble_tbs_switch_device_completed();
    } else {
        g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    }
#endif

#ifdef AIR_SILENCE_DETECTION_ENABLE
    if (APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE == g_lea_ucst_ctrl.next_target) {
        app_le_audio_silence_detection_handle_event(APP_LE_AUDIO_SILENCE_DETECTION_EVENT_START_SPECIAL_SILENCE_DETECTION, NULL);
    }
    else
#endif
    {
        g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        app_le_audio_ucst_start();
    }
}

void app_le_audio_ucst_handle_disconnect_ind(bt_status_t ret, bt_gap_le_disconnect_ind_t *ind)
{
    app_dongle_le_race_disconnect_ind_t evt;
    uint8_t link_idx, i;

    LE_AUDIO_MSGLOG_I("[APP][U] LE_DISCONNECT_IND, handle:%x ret:%x reason:%x", 3, ind->connection_handle, ret, ind->reason);

    if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == (link_idx = app_le_audio_ucst_get_link_idx(ind->connection_handle))) {
        LE_AUDIO_MSGLOG_I("[APP][U] LE_DISCONNECT_IND, link not exist (hdl:%x)", 1, ind->connection_handle);
        return;
    }

    evt.ret = ret;
    memcpy(&(evt.peer_addr), &(g_lea_ucst_link_info[link_idx].addr), sizeof(bt_addr_t));
    evt.group_id = g_lea_ucst_link_info[link_idx].group_id;
    evt.group_size = g_lea_ucst_link_info[link_idx].group_size;

    if (BT_STATUS_SUCCESS != ret) {
        if (g_lea_ucst_callback) {
            g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_DISCONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
        }
        return;
    }

    /* If all remaining LE link(s) did not receive any RELEASING notification, reset g_lea_ucst_ctrl.release to false. */
    g_lea_ucst_link_info[link_idx].release = FALSE;
    for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
        if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle &&
            g_lea_ucst_link_info[i].release) {
            break;
        }
    }

    // TODO: manage release by group?
    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= i) {
        g_lea_ucst_ctrl.release = false;
    }

    if (g_lea_ucst_waiting_disconn_flag) {
        g_lea_ucst_waiting_disconn_flag = FALSE;
        LE_AUDIO_MSGLOG_I("[APP] DISCONNECT_IND, delete_device", 0);
        app_le_audio_ucst_reset_link_info(link_idx);
        app_le_audio_ucst_check_delete_group_device();
        if (g_lea_ucst_callback) {
            g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_DISCONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
        }
        return;
    }

    /* bonded list: */
    uint8_t bond_idx;
    if (APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID != (bond_idx = app_le_audio_ucst_get_bonded_list_idx(&(g_lea_ucst_link_info[link_idx].addr)))) {
        LE_AUDIO_MSGLOG_I("[APP][U] disconnect_device, link_idx:%x in_white_list:%x", 2,
                          g_lea_ucst_bonded_list.device[bond_idx].link_idx,
                          g_lea_ucst_bonded_list.device[bond_idx].in_white_list);

        g_lea_ucst_bonded_list.device[bond_idx].link_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;

        if (BT_HCI_STATUS_REMOTE_TERMINATED_CONNECTION_DUE_TO_LOW_RESOURCES == ind->reason) {
            g_lea_ucst_bonded_list.device[bond_idx].in_white_list = false;
            app_le_audio_ucst_remove_white_list(&(g_lea_ucst_link_info[link_idx].addr));
        }
    }

    app_le_audio_ucst_remove_link_from_group(g_lea_ucst_link_info[link_idx].group_id, link_idx);
    app_le_audio_ucst_reset_link_info(link_idx);

    LE_AUDIO_MSGLOG_I("[APP] DISCONNECT_IND, target:%x->%x stream_state:%x->%x", 4,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state);


    if (g_lea_ucst_waiting_conn_flag) {
        g_lea_ucst_waiting_conn_flag = false;
        LE_AUDIO_MSGLOG_I("[APP] DISCONNECT_IND, connect_device other device", 0);
        app_le_audio_ucst_connect_device(&g_lea_ucst_waiting_conn_addr);
        g_lea_ucst_waiting_conn_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
        memset(&g_lea_ucst_waiting_conn_addr, 0, sizeof(bt_addr_t));
        if (g_lea_ucst_callback) {
            g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_DISCONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
        }
        return;
    }

    uint8_t link_num = 0;
    link_num = app_le_audio_ucst_get_link_num_ex();

#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    if (0 == link_num) {
        g_lea_ucst_ctrl.curr_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
        g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
        g_lea_ucst_ctrl.latest_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    }
#endif

    if (((APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) && (APP_LE_AUDIO_MODE_NONE == g_lea_ctrl.next_mode)) ||
        (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) ||
        (APP_LE_AUDIO_MODE_DISABLE == g_lea_ctrl.next_mode)) {
        /* [Switch streaming mode] UCST -> BCST */
        if (0 != link_num) {
            app_le_audio_ucst_disconnect_all_device();
            return;
        }
        if ((APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state) &&
            (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {
            if (((APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) && (APP_LE_AUDIO_MODE_NONE == g_lea_ctrl.next_mode)) ||
                (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode)) {
                app_le_audio_start_broadcast();
            } else {
                g_lea_ctrl.curr_mode = APP_LE_AUDIO_MODE_NONE;
                g_lea_ctrl.next_mode = APP_LE_AUDIO_MODE_NONE;
                app_dongle_cm_lea_mode_t lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;
                app_dongle_cm_notify_event(APP_DONGLE_CM_SOURCE_LEA, APP_DONGLE_CM_EVENT_SOURCE_END, BT_STATUS_SUCCESS, &lea_mode);
            }
            return;
        }
    }

    bool scan_enable = true;
    if (0 == g_lea_ucst_bonded_list.num) {
        app_le_audio_ucst_connect_coordinated_set(true);

    } else if ((APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID != bond_idx) && (g_lea_ucst_bonded_list.device[bond_idx].in_white_list)) {
        if (APP_LE_AUDIO_UCST_CONN_COORDINATED_SET_BY_SIRK == g_lea_ucst_ctrl.curr_conn) {
            app_le_audio_ucst_connect_bonded_device(true);
        } else {
            app_le_audio_ucst_connect_bonded_device(false);
        }
    } else {
        scan_enable = false;
    }

    uint8_t cis_num = 0;

    cis_num = app_le_audio_ucst_get_cis_num();

    LE_AUDIO_MSGLOG_I("[APP] DISCONNECT_IND, link_num:%x cis_num:%x cig_created:%x", 3, link_num, cis_num, g_lea_ucst_ctrl.is_cig_created);

    if (0 != link_num) {
        if (0 != cis_num) {
            app_le_audio_ucst_set_mic_channel();
            if (g_lea_ucst_callback) {
                g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_DISCONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
            }
            return;
        }

        if ((APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
            (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
            uint8_t i, tmp;
            for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
                    continue;
                }
#else
                i = tmp;
#endif
                if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) {
                    LE_AUDIO_MSGLOG_I("[APP] DISCONNECT_IND, check handle:%x state:%x->%x", 3, g_lea_ucst_link_info[i].handle,
                                      g_lea_ucst_link_info[i].curr_state, g_lea_ucst_link_info[i].next_state);

                    if ((APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS > g_lea_ucst_link_info[i].curr_state) &&
                        (APP_LE_AUDIO_UCST_LINK_STATE_IDLE != g_lea_ucst_link_info[i].next_state)) {
                        /* To do: check ase releasing */
                        if (g_lea_ucst_callback) {
                            g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_DISCONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
                        }
                        return;
                    }
                }
            }
        }
    } else {
        if (APP_LE_AUDIO_UCST_PAUSE_STREAM_DONGLE_FOTA != g_lea_ucst_ctrl.pause_stream) {
            g_lea_ucst_ctrl.pause_stream = APP_LE_AUDIO_UCST_PAUSE_STREAM_NONE;
        }
    }

    if ((APP_LE_AUDIO_UCST_STREAM_STATE_IDLE < g_lea_ucst_ctrl.curr_stream_state) &&
        (APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING >= g_lea_ucst_ctrl.curr_stream_state) &&
        (APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM != g_lea_ucst_ctrl.next_stream_state)) {

        /* stop audio stream */
        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
        if (BT_STATUS_SUCCESS != app_le_audio_close_audio_transmitter()) {
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
        }
    } else if ((0 == link_num) &&
               (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state) &&
               (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {
        g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        if (!scan_enable) {
            g_lea_ctrl.curr_mode = APP_LE_AUDIO_MODE_NONE;
            g_lea_ctrl.next_mode = APP_LE_AUDIO_MODE_NONE;
            app_dongle_cm_lea_mode_t lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;
            app_dongle_cm_notify_event(APP_DONGLE_CM_SOURCE_LEA, APP_DONGLE_CM_EVENT_SOURCE_END, BT_STATUS_SUCCESS, &lea_mode);
        }
    } else if ((0 == link_num) &&
               (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state) &&
               (APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM == g_lea_ucst_ctrl.next_stream_state)) {
        /* This may happen when restart unicast on receiving the terminated ind of the last CIS and after that the last LE link is disconnected just before
         * audio transmitter is opened which is triggered by restart unicast.
         */
        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
   }

    if (g_lea_ucst_callback) {
        g_lea_ucst_callback(APP_DONGLE_LE_RACE_EVT_DISCONNECT_IND, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, &evt);
    }
}

void app_le_audio_ucst_open_audio_transmitter_cb(void)
{
    LE_AUDIO_MSGLOG_I("[APP][U] open_audio_transmitter_cb, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

    if (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state &&
        APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM == g_lea_ucst_ctrl.next_stream_state) {
        /* On receiving DISCONNECT_IND, if there is no LE link and audio transmitter is opening, next_stream_state is set to
               * APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM. open_audio_transmitter_cb need close audio transmitter. */
        g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM;
        if (BT_STATUS_SUCCESS != app_le_audio_close_audio_transmitter()) {
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
        } else {
            /* Change the curr_target only */
            app_le_audio_ucst_stop(false);
        }
        return;
    }

    if (APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM != g_lea_ucst_ctrl.next_stream_state) {
        return;
    }

    g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM;
    g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;

    /* If it is APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE, do nothing. */
    switch (g_lea_ucst_ctrl.curr_target) {
        case APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE:
        case APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE: {
            if ((!app_le_audio_ucst_check_pause_stream()) && (APP_LE_AUDIO_MODE_BCST != g_lea_ctrl.next_mode)) {
                if (!g_lea_ucst_ctrl.is_cig_created) {
                    app_le_audio_ucst_link_info_t *p_info = NULL;
                    uint8_t i, tmp;

                    for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
                            continue;
                        }
#else
                        i = tmp;
#endif
                        p_info = &g_lea_ucst_link_info[i];

                        if ((BT_HANDLE_INVALID != p_info->handle) &&
                            ((APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC == p_info->curr_state) ||
                             (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS == p_info->curr_state)) &&
                            (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == p_info->next_state) &&
                            (0 == p_info->wait_event.wait_ase_event)) {

                            if (((APP_LE_AUDIO_CONN_INTERVAL_STREAMING != p_info->curr_interval) &&
                                 (APP_LE_AUDIO_CONN_INTERVAL_STREAMING != p_info->next_interval)) ||
                                ((APP_LE_AUDIO_CONN_INTERVAL_STREAMING == p_info->curr_interval) &&
                                 (APP_LE_AUDIO_CONN_INTERVAL_STREAMING != p_info->next_interval) &&
                                 (0 != p_info->next_interval))) {
                                LE_AUDIO_MSGLOG_I("[APP][U] update_conn_interval, handle:%x interval:%x->%x set:30", 3, p_info->handle,
                                                  p_info->curr_interval, p_info->next_interval);
                                p_info->next_interval = APP_LE_AUDIO_CONN_INTERVAL_STREAMING;
                                if (BT_STATUS_SUCCESS != app_le_audio_ucst_update_connection_parameter(p_info->handle, APP_LE_AUDIO_CONN_INTERVAL_STREAMING, 0x0002)) {
                                    p_info->next_interval = 0;
                                }
                            }
                        }
                    }

                    g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_SET_CIG_PARAMETER;
                    if (APP_LE_AUDIO_UCST_TEST_MODE_CIG_PARAM & g_lea_ucst_test_mode_flag) {
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_test_mode_set_cig_param()) {
                            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
                        }
                    } else {
                        if (BT_STATUS_SUCCESS != app_le_audio_ucst_set_cig_parameters()) {
                            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
                        }
                    }
                }
                break;
            }
        }
#ifdef AIR_SILENCE_DETECTION_ENABLE
        case APP_LE_AUDIO_UCST_TARGET_STOP_SPECIAL_SILENCE_DETECTION_MODE:
#endif
        case APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE:
        case APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE: {
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
            if (BT_STATUS_SUCCESS != app_le_audio_close_audio_transmitter()) {
                g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
            }
            break;
        }

        default:
            break;
    }
}

void app_le_audio_ucst_close_audio_transmitter_cb(void)
{
    LE_AUDIO_MSGLOG_I("[APP][U] close_audio_transmitter_cb, target:%x->%x stream_state:%x->%x p:%x r:%x", 6,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release);

    if (APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM != g_lea_ucst_ctrl.next_stream_state) {
        return;
    }

    g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
    g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;

    switch (g_lea_ucst_ctrl.curr_target) {
        case APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE:
        case APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE:
        case APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE:
        case APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE: {
            if (g_lea_ucst_ctrl.is_cig_created) {
                g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_REMOVE_CIG;
                if (BT_STATUS_SUCCESS != app_le_audio_ucst_remove_cig()) {
                    g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
                }
                return;
            }
            g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;

            if (app_le_audio_ucst_check_pause_stream() || (g_lea_ucst_ctrl.release) ||
                (((APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
                  (APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target)) &&
                 (APP_LE_AUDIO_UCST_TARGET_NONE == g_lea_ucst_ctrl.next_target))) {
                g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                app_le_audio_ucst_reset_release();
                if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) {
                    /* [Switch streaming mode] UCST -> BCST */
                    app_le_audio_start_broadcast();

                } else if (APP_LE_AUDIO_MODE_DISABLE == g_lea_ctrl.next_mode) {
                    g_lea_ctrl.curr_mode = APP_LE_AUDIO_MODE_NONE;
                    g_lea_ctrl.next_mode = APP_LE_AUDIO_MODE_NONE;
                    app_dongle_cm_lea_mode_t lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;
                    app_dongle_cm_notify_event(APP_DONGLE_CM_SOURCE_LEA, APP_DONGLE_CM_EVENT_SOURCE_END, BT_STATUS_SUCCESS, &lea_mode);
                }
                return;
            }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
            LE_AUDIO_MSGLOG_I("[APP][U] close_audio_transmitter_cb, active:%x->%x", 2,
                              g_lea_ucst_ctrl.curr_group,
                              g_lea_ucst_ctrl.next_group);
            if ((APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.curr_group) &&
                (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.next_group)) {
                if (g_lea_ucst_ctrl.curr_group != g_lea_ucst_ctrl.next_group) {
                    g_lea_ucst_ctrl.curr_group = g_lea_ucst_ctrl.next_group;
                    g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
                    ble_tbs_switch_device_completed();
                } else {
                    g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
                }
            }
#endif

#ifdef AIR_SILENCE_DETECTION_ENABLE
            if (APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE == g_lea_ucst_ctrl.next_target) {
                app_le_audio_silence_detection_handle_event(APP_LE_AUDIO_SILENCE_DETECTION_EVENT_START_SPECIAL_SILENCE_DETECTION, NULL);
            }
            else
#endif
            {
                g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                app_le_audio_ucst_start();
            }
            break;
        }

#ifdef AIR_SILENCE_DETECTION_ENABLE
        /* Audio transmitter may be closed because of LE link disconnected. */
        case APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE:
        case APP_LE_AUDIO_UCST_TARGET_STOP_SPECIAL_SILENCE_DETECTION_MODE: {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
            LE_AUDIO_MSGLOG_I("[APP][SD] close_audio_transmitter_cb, active:%x->%x", 2,
                              g_lea_ucst_ctrl.curr_group,
                              g_lea_ucst_ctrl.next_group);
            if ((APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.curr_group) &&
                (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.next_group)) {
                if (g_lea_ucst_ctrl.curr_group != g_lea_ucst_ctrl.next_group) {
                    g_lea_ucst_ctrl.curr_group = g_lea_ucst_ctrl.next_group;
                    g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
                    ble_tbs_switch_device_completed();
                } else {
                    g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
                }
            }
#endif
            g_lea_ucst_ctrl.curr_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
            app_le_audio_silence_detection_handle_event(APP_LE_AUDIO_SILENCE_DETECTION_EVENT_SPECIAL_SILENCE_DETECTION_STOPPED, NULL);
            break;
        }
#endif
        default:
            break;
    }
}

#ifdef AIR_LE_AUDIO_DO_NOT_STOP_CALL_MODE_WHEN_CALL_EXIST
/* When there is any active call when MIC0 USB port is disabled, call mode is not stopped. Therefore, need to check
  * if call mode need be stopped when all call ends.
  */
void app_le_audio_ucst_handle_call_end_event(void)
{
    app_le_audio_usb_port_mask_t streaming_port = app_le_audio_usb_get_streaming_port();
    app_le_audio_ucst_target_t curr_target = app_le_audio_ucst_get_curr_target();
    bool timer_exist = app_le_audio_usb_check_timer_exist();
    bool call_exist = app_le_audio_usb_hid_call_existing();

    LE_AUDIO_MSGLOG_I("[APP][U] handle call end: call_exist:%d streaming_port:%x curr_target:%x timer_exist:%d", 4,
                      call_exist, streaming_port, curr_target, timer_exist);

    if (!call_exist &&
        !(APP_LE_AUDIO_USB_PORT_MASK_MIC_0 & streaming_port) &&
        !timer_exist &&
        APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == curr_target) {
        if (APP_LE_AUDIO_UCST_CREATE_CIS_ALWAYS_BIDIRECTIONAL != app_le_audio_ucst_get_create_cis_mode()) {
            app_le_audio_ucst_stop(true);
        }
    }
}
#endif

bool app_le_audio_ucst_is_streaming(void)
{
    if (APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING == g_lea_ucst_ctrl.curr_stream_state) {
        return true;
    }
    return false;
}


/* Check the enable status only. Do not check usb ready. Decide the unicast mode (media or call) based on usb port enable status only. */
uint8_t app_le_audio_ucst_get_streaming_port(void)
{
    uint8_t streaming_port = app_le_audio_usb_get_streaming_port();

    if (APP_LE_AUDIO_UCST_CREATE_CIS_ALWAYS_UNIDIRECTIONAL == g_lea_ucst_ctrl.create_cis_mode) {
        streaming_port = (APP_LE_AUDIO_USB_PORT_MASK_SPK_0 | APP_LE_AUDIO_USB_PORT_MASK_SPK_1);
    } else if (APP_LE_AUDIO_UCST_CREATE_CIS_ALWAYS_BIDIRECTIONAL == g_lea_ucst_ctrl.create_cis_mode) {
        streaming_port = (APP_LE_AUDIO_USB_PORT_MASK_SPK_0 | APP_LE_AUDIO_USB_PORT_MASK_SPK_1 | APP_LE_AUDIO_USB_PORT_MASK_MIC_0);
    }

    return streaming_port;
}


void app_le_audio_ucst_start(void)
{
    uint8_t streaming_port = app_le_audio_ucst_get_streaming_port();
    uint8_t i, link_idx;
    bool bidirectional = false, is_ready = false;

    if (APP_LE_AUDIO_UCST_PAUSE_STREAM_ALL <= g_lea_ucst_ctrl.pause_stream) {
        LE_AUDIO_MSGLOG_I("[APP][U] start, p:%x", g_lea_ucst_ctrl.pause_stream);
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] start, target:%x->%x stream_state:%x->%x p:%x r:%x c:%x port:%x", 8,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release,
                      g_lea_ucst_ctrl.create_cis_mode, streaming_port);

#ifdef AIR_SILENCE_DETECTION_ENABLE
    /* Stop special silence detection mode first if it exists. */
    if (app_le_audio_silence_detection_is_speical_silence_detection_ongoing()) {
        app_le_audio_silence_detection_handle_event(APP_LE_AUDIO_SILENCE_DETECTION_EVENT_START_OTHER_MODE, NULL);
        return;
    }
#endif

    if (!streaming_port) {
        return;
    }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    LE_AUDIO_MSGLOG_I("[APP] start, active_group:%x->%x latest:%x", 3,
                      g_lea_ucst_ctrl.curr_group,
                      g_lea_ucst_ctrl.next_group,
                      g_lea_ucst_ctrl.latest_group);

    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.next_group) {
        /* change active group */
        return;
    }

    if ((APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.curr_group) &&
        (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.latest_group)) {
        g_lea_ucst_ctrl.curr_group = g_lea_ucst_ctrl.latest_group;
        g_lea_ucst_active_group = g_lea_ucst_ctrl.latest_group;
        app_le_audio_ucst_write_active_group_to_nvkey();
        ble_tbs_switch_device_completed();
        app_le_audio_ucst_set_mic_channel();
    }

    app_le_audio_ucst_group_info_t *p_group_info = NULL;

    if (NULL == (p_group_info = app_le_audio_ucst_get_group_info(g_lea_ucst_ctrl.curr_group))) {
        LE_AUDIO_MSGLOG_I("[APP][U] start, invalid group:%x", 1, g_lea_ucst_ctrl.curr_group);
        return;
    }

    if (!app_le_audio_ucst_is_group_device_all_connected(g_lea_ucst_ctrl.curr_group)) {
        app_le_audio_ucst_connect_group_device(g_lea_ucst_ctrl.curr_group);
    }

    for (i = 0; i < APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM; i++) {
        if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID != p_group_info->link_idx[i]) {
            break;
        }
    }

    if (APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM == i) {
        LE_AUDIO_MSGLOG_I("[APP][U] start, group:%x no link exist", 1, g_lea_ucst_ctrl.curr_group);
        return;
    }
#endif

    app_le_audio_ucst_lock_stream_t lock_stream = APP_LE_AUDIO_UCST_LCOK_STREAM_NONE;

    for (i = 0; i < app_le_audio_ucst_get_max_link_num(); i++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (link_idx = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[i])) {
            continue;
        }
#else
        link_idx = i;
#endif

        if (BT_HANDLE_INVALID != g_lea_ucst_link_info[link_idx].handle) {
            LE_AUDIO_MSGLOG_I("[APP][U] start, check handle:%x state:%x->%x r:%x lock:%x", 5,
                              g_lea_ucst_link_info[link_idx].handle,
                              g_lea_ucst_link_info[link_idx].curr_state, g_lea_ucst_link_info[link_idx].next_state,
                              g_lea_ucst_link_info[link_idx].ase_releasing,
                              g_lea_ucst_link_info[link_idx].lock_stream);

            if ((g_lea_ucst_link_info[link_idx].ase_releasing) ||
                (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL == g_lea_ucst_link_info[link_idx].next_state)) {
                return;
            }
            if (APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC <= g_lea_ucst_link_info[link_idx].curr_state) {
                is_ready = true;
            }
#ifdef AIR_LE_AUDIO_BA_ENABLE
            if ((APP_LE_AUDIO_UCST_LINK_STATE_READ_ASE == g_lea_ucst_link_info[link_idx].curr_state)
                && (APP_LE_AUDIO_UCST_LINK_STATE_IDLE == g_lea_ucst_link_info[link_idx].next_state)) {
                bt_status_t ret;
                g_lea_ucst_link_info[link_idx].next_state = APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC;
                if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_config_codec(g_lea_ucst_link_info[link_idx].handle))) {
                    g_lea_ucst_link_info[link_idx].next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
                    LE_AUDIO_MSGLOG_I("[APP][U] codec_config failed, handle:%x ret:%x", 2, g_lea_ucst_link_info[link_idx].handle, ret);
                }
            }
#endif
            if (lock_stream < g_lea_ucst_link_info[link_idx].lock_stream) {
                lock_stream = g_lea_ucst_link_info[link_idx].lock_stream;
            }
        }
    }

    if ((!is_ready) || (APP_LE_AUDIO_UCST_LCOK_STREAM_ALL <= lock_stream)) {
        return;
    }

    if (streaming_port & APP_LE_AUDIO_USB_PORT_MASK_MIC_0) {
        bidirectional = true;
    }

    if (bidirectional) {
        switch (g_lea_ucst_ctrl.curr_target) {
            case APP_LE_AUDIO_UCST_TARGET_NONE: {
                g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE;
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                break;
            }
            case APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE:
            case APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE: {
                g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE;
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE;
                break;
            }
            case APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE:
            case APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE: {
                g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE;
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                break;
            }
            default:
                break;
        }
    } else {
        if (APP_LE_AUDIO_UCST_PAUSE_STREAM_UNIDIRECTIONAL == g_lea_ucst_ctrl.pause_stream) {
            return;
        }
        if (APP_LE_AUDIO_UCST_LCOK_STREAM_UNIDIRECTIONAL == lock_stream) {
            return;
        }
        switch (g_lea_ucst_ctrl.curr_target) {
            case APP_LE_AUDIO_UCST_TARGET_NONE: {
                g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE;
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                break;
            }
            case APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE:
            case APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE: {
                g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE;
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                break;
            }
            case APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE:
            case APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE: {
                g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE;
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE;
                break;
            }
            default:
                break;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] start, target:%x->%x", 2, g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target);

    if ((APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) ||
        (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target)) {
#ifdef AIR_SILENCE_DETECTION_ENABLE
        if (APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == g_lea_ucst_ctrl.curr_target) {
            app_le_audio_silence_detection_set_status(APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_SILENCE);
        }
#endif
        if ((APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state) &&
            (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM;
            if (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) {
                streaming_port |= (APP_LE_AUDIO_USB_PORT_MASK_SPK_0 | APP_LE_AUDIO_USB_PORT_MASK_MIC_0);
            }
            if (BT_STATUS_SUCCESS != app_le_audio_open_audio_transmitter(bidirectional, streaming_port)) {
                g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
            }
            return;
        }

        if (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state &&
            APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM == g_lea_ucst_ctrl.next_stream_state) {
            /* This should not happen. Opening audio transmitter should be finished before a new LE link is connected and codec configured on that link. */
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM;
            return;
        }

        if (APP_LE_AUDIO_UCST_TARGET_NONE == g_lea_ucst_ctrl.next_target &&
            APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state &&
            APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM == g_lea_ucst_ctrl.next_stream_state) {
            /* Clear the STOP/DEINIT state for next_transmitter_state, if port stop is received before the current port play. */
            /* SPK_0 */
            if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_SPK_0)) {
                app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_SPK_0);
            }

            /* SPK_1 */
            if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_SPK_1)) {
                app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_SPK_1);
            }

            /* MIC_0 */
            if (bidirectional) {
                if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_MIC_0)) {
                    app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_MIC_0);
                }
            }

            return;
        }

        if ((APP_LE_AUDIO_UCST_STREAM_STATE_SET_CIG_PARAMETER == g_lea_ucst_ctrl.curr_stream_state) &&
            (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {
            /* check the all link */
            for (i = 0; i < app_le_audio_ucst_get_max_link_num(); i++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (link_idx = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[i])) {
                    continue;
                }
#else
                link_idx = i;
#endif
                if (BT_HANDLE_INVALID != g_lea_ucst_link_info[link_idx].handle) {
                    app_le_audio_ucst_check_set_ase(link_idx);
                }
            }
            return;
        }
    }

    if ((APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE == g_lea_ucst_ctrl.curr_target) ||
        (APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == g_lea_ucst_ctrl.curr_target)) {
        if ((APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING == g_lea_ucst_ctrl.curr_stream_state) &&
            (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {

            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_STREAMING;

            if (0 != app_le_audio_ucst_get_cis_num()) {
                if (app_le_audio_ucst_disable_ase_when_state_match_streaming(false)) {
                    return;
                }
            }

            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
            if (BT_STATUS_SUCCESS != app_le_audio_close_audio_transmitter()) {
                g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
            }
            return;
        }
    }
}

bool app_le_audio_ucst_stop(bool restart)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t i, tmp;

    LE_AUDIO_MSGLOG_I("[APP][U] stop, target:%x->%x stream_state:%x->%x p:%x r:%x c:%x restart:%x", 8,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                      g_lea_ucst_ctrl.pause_stream, g_lea_ucst_ctrl.release,
                      g_lea_ucst_ctrl.create_cis_mode, restart);

    app_le_audio_usb_stop_timer();

    if ((APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state) &&
        (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {
        if (restart) {
            app_le_audio_ucst_start();
            return true;
        }
        return false;
    }
    {
        for (tmp = 0; tmp < app_le_audio_ucst_get_max_link_num(); tmp++) {
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
            if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= (i = g_lea_ucst_group_info[g_lea_ucst_ctrl.curr_group].link_idx[tmp])) {
                continue;
            }
#else
            i = tmp;
#endif
            p_info = &g_lea_ucst_link_info[i];
            if (BT_HANDLE_INVALID != p_info->handle) {
                LE_AUDIO_MSGLOG_I("[APP][U] stop, check handle:%x state:%x->%x w_ase:%x r:%x lock:%x", 6,
                                  p_info->handle,
                                  p_info->curr_state, p_info->next_state,
                                  p_info->wait_event.wait_ase_event,
                                  p_info->ase_releasing,
                                  p_info->lock_stream);
            }
        }
    }

    /*if ((APP_LE_AUDIO_UCST_CREATE_CIS_ALWAYS_BIDIRECTIONAL == g_lea_ucst_ctrl.create_cis_mode) &&
        (true == restart)) {
        return true;
    }*/

#ifdef AIR_SILENCE_DETECTION_ENABLE
    if (app_le_audio_silence_detection_is_speical_silence_detection_ongoing()) {
        app_le_audio_silence_detection_handle_event(APP_LE_AUDIO_SILENCE_DETECTION_EVENT_STOP_ANY_MODE, (void *)restart);
        return true;
    }
    else {
        app_le_audio_silence_detection_stop_delay_stop_timer();
    }
#endif

    switch (g_lea_ucst_ctrl.curr_target) {
        case APP_LE_AUDIO_UCST_TARGET_NONE: {
            if ((restart) && (APP_LE_AUDIO_UCST_TARGET_NONE == g_lea_ucst_ctrl.next_target)) {
                app_le_audio_ucst_start();
                return true;
            }
            break;
        }
        case APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE: {
            g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE;
            if (restart) {
                /* start progress will correct start mode based on streaming port. */
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE;
            } else {
#ifdef AIR_SILENCE_DETECTION_ENABLE
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE;
#else
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
#endif
            }
            break;
        }
        case APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE: {
            g_lea_ucst_ctrl.curr_target = APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE;
            if (restart) {
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE;
            } else {
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
            }
            break;
        }
        case APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE: {
            if (restart) {
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE;
            } else {
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
            }
            break;
        }
        case APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE: {
            if (restart) {
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE;
            } else {
                g_lea_ucst_ctrl.next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
            }
            break;
        }
        default:
            break;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] stop, target:%x->%x", 2, g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target);

    if ((APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING == g_lea_ucst_ctrl.curr_stream_state) &&
        (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {

        g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_STREAMING;

        if (app_le_audio_ucst_disable_ase_when_state_match_streaming(true)) {
            return true;
        }

        if (0 != app_le_audio_ucst_get_cis_num()) {
            g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
            if (BT_STATUS_SUCCESS != app_le_audio_close_audio_transmitter()) {
                g_lea_ucst_ctrl.next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
            }
        }

    }

    return true;
}

void app_le_audio_ucst_pause(void)
{
    LE_AUDIO_MSGLOG_I("[APP][U] pause, p:%x", 1, g_lea_ucst_ctrl.pause_stream);

    if (APP_LE_AUDIO_UCST_PAUSE_STREAM_DONGLE_FOTA == g_lea_ucst_ctrl.pause_stream) {
        return;
    }

    if (!app_le_audio_ucst_check_pause_stream()) {
        return;
    }

    /* update connection interval */
    app_le_audio_ucst_update_active_device_connection_interval(APP_LE_AUDIO_CONN_INTERVAL_STREAMING);

    app_le_audio_ucst_stop(false);
}

bool app_le_audio_ucst_pause_ex(app_le_audio_ucst_pause_stream_t mode)
{
    LE_AUDIO_MSGLOG_I("[APP][U] pause, p:%x->%x", 2, g_lea_ucst_ctrl.pause_stream, mode);

    if (!(g_lea_ucst_ctrl.pause_stream & mode)) {
        g_lea_ucst_ctrl.pause_stream |= mode;
#if AIR_MS_TEAMS_ENABLE
        if (APP_LE_AUDIO_UCST_PAUSE_STREAM_ALL <= mode) {
            app_le_audio_usb_hid_handle_busy_call(true);
        }
#endif
    }

    if (!app_le_audio_ucst_check_pause_stream()) {
        return false;
    }

    /* update connection interval */
    app_le_audio_ucst_update_active_device_connection_interval(APP_LE_AUDIO_CONN_INTERVAL_STREAMING);

    return app_le_audio_ucst_stop(false);
}

void app_le_audio_ucst_resume_ex(app_le_audio_ucst_pause_stream_t mode)
{
    LE_AUDIO_MSGLOG_I("[APP][U] resume, target:%x->%x stream_state:%x->%x p:%x->%x r:%x", 7,
                      g_lea_ucst_ctrl.curr_target, g_lea_ucst_ctrl.next_target,
                      g_lea_ucst_ctrl.curr_stream_state, g_lea_ucst_ctrl.next_stream_state,
                      g_lea_ucst_ctrl.pause_stream, mode,
                      g_lea_ucst_ctrl.release);

    app_le_audio_ucst_reset_release();

    if (g_lea_ucst_ctrl.pause_stream & mode) {
        g_lea_ucst_ctrl.pause_stream &= ~mode;
#if AIR_MS_TEAMS_ENABLE
        if (APP_LE_AUDIO_UCST_PAUSE_STREAM_ALL > mode) {
            app_le_audio_usb_hid_handle_busy_call(false);
        }
#endif
    }

    if (app_le_audio_ucst_check_pause_stream()) {
        return;
    }

    if (!((APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING == g_lea_ucst_ctrl.curr_stream_state) &&
          (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state))) {
        app_le_audio_ucst_start();
    }
}

void app_le_audio_ucst_resume(void)
{
    if (app_le_audio_ucst_check_pause_stream()) {
        return;
    }

    app_le_audio_ucst_reset_release();

    if (!((APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING == g_lea_ucst_ctrl.curr_stream_state) &&
          (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state))) {
        app_le_audio_ucst_start();
    }
}


void app_le_audio_ucst_le_connection_timer_callback(TimerHandle_t timer_handle, void *user_data)
{
    uint8_t link_idx;
    app_le_audio_ucst_link_info_t *link_info = app_le_audio_ucst_find_connecting_link_info_by_timer_handle(timer_handle, &link_idx);

    LE_AUDIO_MSGLOG_I("[APP][U] LE connection timer expired. timer_handle:%x link_idx:%d user_data:%x conn:%x scan:%x->%x", 6,
                      timer_handle,
                      link_idx,
                      user_data,
                      g_lea_ucst_ctrl.curr_conn,
                      g_lea_ucst_ctrl.curr_scan,
                      g_lea_ucst_ctrl.next_scan);
    if (link_info) {
        /* LE connection is not finished. Otherwise, link_info will be NULL. */
        /* Timer will be deleted after the callback returns. */
        link_info->le_connection_timer_handle = NULL;
        app_le_audio_ucst_reset_link_info(link_idx);
        bt_gap_le_srv_cancel_connection();
        if (APP_LE_AUDIO_UCST_SCAN_NONE == g_lea_ucst_ctrl.curr_scan) {
            app_le_audio_ucst_connect_coordinated_set(false);
        }
    }
}


bt_status_t app_le_audio_ucst_connect_device(const bt_addr_t *addr)
{
    bt_status_t ret = BT_STATUS_FAIL;
    uint8_t link_idx;
    app_le_audio_ucst_link_info_t *link_info_connecting = app_le_audio_ucst_find_connecting_link_info_by_peer_addr(addr, NULL);
    app_le_audio_ucst_link_info_t *link_info = app_le_audio_ucst_get_available_link_info_for_new_le_connection(&link_idx);

    bt_hci_cmd_le_create_connection_t param = {
        .le_scan_interval = 0x10,
        .le_scan_window = 0x10,
        .initiator_filter_policy = BT_HCI_CONN_FILTER_ASSIGNED_ADDRESS,
        .own_address_type = BT_ADDR_RANDOM,
        .conn_interval_min = 0x0030,
        .conn_interval_max = 0x0030,
        .conn_latency = 0x0000,
        .supervision_timeout = 0x01F4,
        .minimum_ce_length = 0x0002,
        .maximum_ce_length = 0x0002,
    };

    if (link_info_connecting) {
        LE_AUDIO_MSGLOG_I("[APP][U] device is connecting, addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,
                          addr->type,
                          addr->addr[5],
                          addr->addr[4],
                          addr->addr[3],
                          addr->addr[2],
                          addr->addr[1],
                          addr->addr[0]);
        return BT_STATUS_SUCCESS;
    }

    if (!link_info) {
        /* Link full. Or there is LE connection on-going and new connection will make link full. */
        return BT_STATUS_TIMER_FULL;
    }

    /*if (g_lea_ucst_ctrl.is_creating_connection) {
        LE_AUDIO_MSGLOG_I("[APP][U] connect_device, busy", 0);
        return BT_STATUS_BUSY;
    }

    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == app_le_audio_ucst_get_link_num()) {
        LE_AUDIO_MSGLOG_I("[APP][U] connect_device, link full!", 0);
        return BT_STATUS_FAIL;
    }*/

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    if ((true == app_le_audio_ucst_is_group_full()) && (!app_le_audio_ucst_is_bonded_device(addr))) {
        LE_AUDIO_MSGLOG_I("[APP][U] connect_device, group full!", 0);
        return BT_STATUS_FAIL;
    }
#endif

    memcpy(&(param.peer_address), addr, sizeof(bt_addr_t));

#if 1
    LE_AUDIO_MSGLOG_I("[APP][U] connect_device, addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,
                      param.peer_address.type,
                      param.peer_address.addr[5],
                      param.peer_address.addr[4],
                      param.peer_address.addr[3],
                      param.peer_address.addr[2],
                      param.peer_address.addr[1],
                      param.peer_address.addr[0]);

#endif

    memcpy(&(link_info->addr), addr, sizeof(bt_addr_t));
    ret = app_le_audio_timer_start(&(link_info->le_connection_timer_handle),
                                   APP_LE_AUDIO_TIMER_LE_CONNECTION_TIME_PERIOD,
                                   app_le_audio_ucst_le_connection_timer_callback,
                                   link_info);

    if (BT_STATUS_SUCCESS == ret) {
        app_le_audio_ucst_stop_scan_all();
        ret = bt_gap_le_srv_connect(&param);

        if (BT_STATUS_SUCCESS != ret) {
            app_le_audio_timer_stop(link_info->le_connection_timer_handle);
            link_info->le_connection_timer_handle = NULL;
            app_le_audio_ucst_reset_link_info(link_idx);
            app_le_audio_ucst_connect_coordinated_set(false);
        }
    }

    return ret;
}

void app_le_audio_ucst_cancel_create_connection(void)
{
    app_le_audio_ucst_link_info_t *link_info = NULL;

    /* check cancel connection */
    for (uint32_t i = 0; APP_LE_AUDIO_UCST_LINK_MAX_NUM > i ; i++) {
        link_info = &g_lea_ucst_link_info[i];

        if ((BT_HANDLE_INVALID == link_info->handle) &&
            (NULL != link_info->le_connection_timer_handle)) {
            app_le_audio_timer_stop(link_info->le_connection_timer_handle);
            link_info->le_connection_timer_handle = NULL;
            app_le_audio_ucst_reset_link_info(i);
            bt_gap_le_cancel_connection();
        }
    }
}

bt_status_t app_le_audio_ucst_disconnect_device(bt_addr_t *addr)
{
    uint8_t i;
    uint8_t bond_idx;

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;
    while (0 != i) {
        i--;
        if ((BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) &&
            (0 == memcmp(&(g_lea_ucst_link_info[i].addr), addr, sizeof(bt_addr_t)))) {

            LE_AUDIO_MSGLOG_I("[APP][U] disconnect_device, handle:%x", 1, g_lea_ucst_link_info[i].handle);

            if (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL == g_lea_ucst_link_info[i].next_state) {
                return BT_STATUS_SUCCESS;
            }

            g_lea_ucst_link_info[i].next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL;
            if (APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID != (bond_idx = app_le_audio_ucst_get_bonded_list_idx(&(g_lea_ucst_link_info[i].addr)))) {
                g_lea_ucst_bonded_list.device[bond_idx].in_white_list = false;
            }
            if (BT_STATUS_SUCCESS == app_le_audio_ucst_disconnect(g_lea_ucst_link_info[i].handle)) {

                app_le_audio_ucst_remove_white_list(addr);
                return BT_STATUS_SUCCESS;
            }

            g_lea_ucst_link_info[i].next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
            app_le_audio_ucst_remove_white_list(addr);
            return BT_STATUS_FAIL;
        }
    }

    return BT_STATUS_FAIL;
}

bool app_le_audio_ucst_disconnect_all_device(void)
{
    uint8_t i;

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;
    while (0 != i) {
        i--;
        if ((BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) &&
            (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL == g_lea_ucst_link_info[i].next_state)) {
            return true;
        }
    }

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;
    while (0 != i) {
        i--;
        if ((BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) &&
            (APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_CIS != g_lea_ucst_link_info[i].next_state)) {
            g_lea_ucst_link_info[i].next_state = APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL;
            if (BT_STATUS_SUCCESS == app_le_audio_ucst_disconnect(g_lea_ucst_link_info[i].handle)) {
                return true;
            }
            g_lea_ucst_link_info[i].next_state = APP_LE_AUDIO_UCST_LINK_STATE_IDLE;
        }
    }

    return false;
}

void app_le_audio_ucst_delete_device(bt_addr_t *addr)
{
    app_le_audio_ucst_delete_group_from_bonded_list(addr);
}

bt_status_t app_le_audio_ucst_start_scan_device(void)
{
    bt_status_t ret;

    LE_AUDIO_MSGLOG_I("[APP][U] start_scan_device, scan:%x->%x conn:%x", 3, g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.next_scan,
                      g_lea_ucst_ctrl.curr_conn);

    g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_LEA_DEVICE;

    bt_app_common_at_cmd_print_report("Scan LEA:\r\n");

    if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_start_scan(false))) {
        g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] start_scan_device, curr_scan:%x conn:%x", 2, g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.curr_conn);

    return ret;
}

bt_status_t app_le_audio_ucst_stop_scan_device(void)
{
    LE_AUDIO_MSGLOG_I("[APP][U] stop_scan_device, scan:%x->%x conn:%x", 3, g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.next_scan,
                      g_lea_ucst_ctrl.curr_conn);

    //g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;

    bt_app_common_at_cmd_print_report("Stop scan LEA:\r\n");

    return app_le_audio_ucst_stop_scan();
}

void app_le_audio_ucst_add_bonded_device_to_white_list(void)
{
    if (0 == g_lea_ucst_bonded_list.num) {
        LE_AUDIO_MSGLOG_I("[APP][U] add_bonded_device_to_white_list, bonded_list empty", 0);
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP] add_bonded_device_to_white_list, total_num:%x idx:%x state:%x", 3,
                      g_lea_ucst_bonded_list.num,
                      g_lea_ucst_set_white_list.curr_idx,
                      g_lea_ucst_set_white_list.state);

    if (APP_LE_AUDIO_UCST_SET_WHITE_LIST_ADD_ON_GOING == g_lea_ucst_set_white_list.state) {
        g_lea_ucst_set_white_list.curr_idx++;

        if (g_lea_ucst_bonded_list.num <= g_lea_ucst_set_white_list.curr_idx) {
            memset(&g_lea_ucst_set_white_list, 0, sizeof(app_le_audio_ucst_set_white_list_t));
            g_lea_ucst_set_white_list.state = APP_LE_AUDIO_UCST_SET_WHITE_LIST_COMPLETE;
            app_le_audio_ucst_connect_bonded_device(false);
            return;
        }
    }

    bt_addr_t device;
    bt_status_t ret;
    uint8_t idx;

    idx = g_lea_ucst_set_white_list.curr_idx;
    memcpy(&device, &(g_lea_ucst_bonded_list.device[idx].addr), sizeof(bt_addr_t));

    LE_AUDIO_MSGLOG_I("[APP] white_list[%x], addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 8, idx, device.type,
                      device.addr[5], device.addr[4],
                      device.addr[3], device.addr[2],
                      device.addr[1], device.addr[0]);

    g_lea_ucst_bonded_list.device[idx].in_white_list = true;
    g_lea_ucst_set_white_list.state = APP_LE_AUDIO_UCST_SET_WHITE_LIST_ADD_ON_GOING;

    ret = bt_gap_le_set_white_list(BT_GAP_LE_ADD_TO_WHITE_LIST, &device);

    LE_AUDIO_MSGLOG_I("[APP][U] add_white_list, ret:%x", 1, ret);
}

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
void app_le_audio_ucst_set_active_group(uint8_t group)
{
    app_le_audio_ucst_group_info_t *p_group_info = NULL;

    if (NULL == (p_group_info = app_le_audio_ucst_get_group_info(group))) {
        LE_AUDIO_MSGLOG_I("[APP][U] set_active_group, invalid group:%x", 1, group);
        return;
    }

    if (0 == p_group_info->size) {
        LE_AUDIO_MSGLOG_I("[APP][U] set_active_group, not exist group:%x", 1, group);
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] set_active_group, stream_state:%x->%x group:%x active:%x->%x latest:%x", 6,
                      g_lea_ucst_ctrl.curr_stream_state,
                      g_lea_ucst_ctrl.next_stream_state,
                      group,
                      g_lea_ucst_ctrl.curr_group,
                      g_lea_ucst_ctrl.next_group,
                      g_lea_ucst_ctrl.latest_group);

    if (((group == g_lea_ucst_ctrl.curr_group) && (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.next_group)) ||
        (group == g_lea_ucst_ctrl.next_group)) {
        return;
    }


    if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.curr_group) {
        g_lea_ucst_ctrl.curr_group = group;
        g_lea_ucst_active_group = group;
        app_le_audio_ucst_write_active_group_to_nvkey();
        if (!app_le_audio_ucst_is_group_device_all_connected(group)) {
            app_le_audio_ucst_connect_group_device(group);
        }
        ble_tbs_switch_device_completed();
        app_le_audio_ucst_start();
        return;
    }

    if ((group != g_lea_ucst_ctrl.curr_group) ||
        (group != g_lea_ucst_ctrl.next_group)) {
        g_lea_ucst_active_group = group;
        app_le_audio_ucst_write_active_group_to_nvkey();

        if (!app_le_audio_ucst_is_group_device_all_connected(group)) {
            app_le_audio_ucst_connect_group_device(group);
        }
        if ((APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.curr_stream_state) &&
            (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == g_lea_ucst_ctrl.next_stream_state)) {
            g_lea_ucst_ctrl.curr_group = group;
            g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
            ble_tbs_switch_device_completed();
        } else {
            g_lea_ucst_ctrl.next_group = group;
            app_le_audio_ucst_stop(true);
        }
    }
}
#endif

void app_le_audio_ucst_register_callback(app_dongle_le_race_event_callback_t callback)
{
    g_lea_ucst_callback = callback;
}

void app_le_audio_ucst_reset_param(void)
{
    memset(&g_lea_ucst_set_white_list, 0, sizeof(app_le_audio_ucst_set_white_list_t));
    memset(&g_lea_ucst_waiting_conn_addr, 0, sizeof(bt_addr_t));

    /* reset ucst info */
    memset(&g_lea_ucst_ctrl, 0, sizeof(app_le_audio_ucst_ctrl_t));
    g_lea_ucst_ctrl.curr_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    g_lea_ucst_ctrl.latest_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;

    /* reset link_info */
    app_le_audio_ucst_reset_all_link_info();

    /* reset cis info */
    app_le_audio_ucst_reset_all_cis_info();

    app_le_audio_aird_init();

    uint8_t i = 0;
    for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
        g_lea_ucst_prepare_vcmd_conn_handle[i] = BT_HANDLE_INVALID;
        g_lea_ucst_prepare_vcmd_disconn_handle[i] = BT_HANDLE_INVALID;
    }
}

void app_le_audio_ucst_init(void)
{
    app_le_audio_ucst_reset_param();

    ble_vcp_set_default_volume(APP_LE_AUDIO_UCST_VCP_DEFAULT_VOLUME);

    bt_le_audio_source_init((BLE_TMAP_ROLE_MASK_CG | BLE_TMAP_ROLE_MASK_UMS | BLE_TMAP_ROLE_MASK_BMS),
                            app_le_audio_ucst_handle_source_evt,
                            APP_LE_AUDIO_UCST_LINK_MAX_NUM);

    ble_csip_init(app_le_audio_ucst_handle_csip_evt, APP_LE_AUDIO_UCST_LINK_MAX_NUM);

    ble_tmap_init(app_le_audio_ucst_handle_tmap_evt, APP_LE_AUDIO_UCST_LINK_MAX_NUM);

    ble_bap_client_init(app_le_audio_ucst_handle_bap_evt, APP_LE_AUDIO_UCST_LINK_MAX_NUM);
}

#endif  /* AIR_LE_AUDIO_ENABLE */

