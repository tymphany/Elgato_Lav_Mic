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

#include "FreeRTOS.h"

#include "nvkey.h"
#include "nvkey_id_list.h"

#include "apps_events_event_group.h"
#include "ui_shell_manager.h"

#include "bt_device_manager_le.h"
#include "bt_gap_le_service.h"

#include "app_le_audio.h"
#include "app_le_audio_ucst_utillity.h"
#include "app_le_audio_nvkey_struct.h"
#include "bt_le_audio_util_nvkey_struct.h"

#include "ble_csip.h"
#include "ble_bap_client.h"

#include "bt_le_audio_msglog.h"
/**************************************************************************************************
* Define
**************************************************************************************************/

/**************************************************************************************************
* Structure
**************************************************************************************************/

/**************************************************************************************************
* Variable
**************************************************************************************************/
bt_key_t g_le_audio_sirk = {0};

app_le_audio_ucst_ctrl_t g_lea_ucst_ctrl;

app_le_audio_ucst_bonded_list_t g_lea_ucst_bonded_list;

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
bt_lea_sink_info_nvkey_t g_lea_ucst_sirk_info[APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM];
#endif

uint8_t g_lea_ucst_active_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;

app_le_audio_ucst_set_white_list_t g_lea_ucst_set_white_list;

app_le_audio_ucst_group_info_t g_lea_ucst_group_info[APP_LE_AUDIO_UCST_GROUP_ID_MAX];

app_le_audio_ucst_link_info_t g_lea_ucst_link_info[APP_LE_AUDIO_UCST_LINK_MAX_NUM];

app_le_audio_ucst_cis_info_t g_lea_ucst_cis_info[APP_LE_AUDIO_UCST_CIS_MAX_NUM];

app_le_audio_qos_params_t g_lea_ucst_qos_params_spk_0 = {
    CODEC_CONFIGURATION_SAMPLING_FREQ_32KHZ,    /* sampling_freq */
    60,                                         /* sdu_size */
    SDU_INTERVAL_7P5_MS,                        /* sdu_interval */
    64,                                         /* bitrate */
    2,                                          /* rtn */
    8,                                          /* latency */
};

app_le_audio_qos_params_t g_lea_ucst_qos_params_mic_0 = {
    CODEC_CONFIGURATION_SAMPLING_FREQ_32KHZ,    /* sampling_freq */
    60,                                         /* sdu_size */
    SDU_INTERVAL_7P5_MS,                        /* sdu_interval */
    64,                                         /* bitrate */
    2,                                          /* rtn */
    8,                                          /* latency */
};

app_le_audio_qos_params_t g_lea_ucst_qos_params_spk_1 = {
    CODEC_CONFIGURATION_SAMPLING_FREQ_48KHZ,    /* sampling_freq */
    155,                                        /* sdu_size */
    SDU_INTERVAL_10_MS,                         /* sdu_interval */
    124,                                        /* bitrate */
    5,                                          /* rtn */
    20,                                         /* latency */
};

static const app_le_audio_qos_params_tbl_t g_lea_ucst_qos_params_tbl[] = {
/*  sdu_size    sdu_interval        bitrate low_rtn low_latency high_rtn   high_latency*/
    {30,        SDU_INTERVAL_7P5_MS,   32,      2,      8,       13,        75},       /* 0 16_1 */
    {45,        SDU_INTERVAL_7P5_MS,   48,      2,      8,       13,        75},       /* 1 24_1 */
    {60,        SDU_INTERVAL_7P5_MS,   64,      2,      8,       13,        75},       /* 2 32_1 */
    {75,        SDU_INTERVAL_7P5_MS,   80,      5,      15,      13,        75},       /* 3 48_1 */
    {90,        SDU_INTERVAL_7P5_MS,   96,      5,      15,      13,        75},       /* 4 48_3 */
    {117,       SDU_INTERVAL_7P5_MS,   124.8,   5,      15,      13,        75},       /* 5 48_5 */
    {30,        SDU_INTERVAL_10_MS,    24,      2,      10,      13,        95},       /* 6 8_2 */
    {40,        SDU_INTERVAL_10_MS,    32,      2,      10,      13,        95},       /* 7 16_2 */
    {60,        SDU_INTERVAL_10_MS,    48,      2,      10,      13,        95},       /* 8 24_2 */
    {80,        SDU_INTERVAL_10_MS,    64,      2,      10,      13,        95},       /* 9 32_2 */
    {100,       SDU_INTERVAL_10_MS,    80,      5,      20,      13,        95},       /* 10 48_2 */
    {120,       SDU_INTERVAL_10_MS,    96,      5,      20,      13,       100},       /* 11 48_4 */
    {155,       SDU_INTERVAL_10_MS,    124,     5,      20,      13,       100},       /* 12 48_6 */
    {75,        SDU_INTERVAL_7P5_MS,   80,      2,       8,      2,          8},       /* 13, HQ gaming audio/ HQ gaming audio and voice(DL) */
    {126,       SDU_INTERVAL_7P5_MS,   134.4,   2,       8,      2,          8},       /* 14, LL gaming audio */
    {60,        SDU_INTERVAL_7P5_MS,   64,      2,       8,      2,          8},       /* 15, HQ gaming audio and voice(DL) */
    {26,        SDU_INTERVAL_7P5_MS,   27.734,  2,       8,      13,        75},       /* 16 8_1 */
};

#define APP_LE_AUDIO_UCST_QOS_PARAMS_TBL_COUNT (sizeof(g_lea_ucst_qos_params_tbl)/sizeof(app_le_audio_qos_params_tbl_t))

extern const uint32_t g_lea_sdu_interval_tbl[];

app_le_audio_ucst_cig_params_test_t *g_lea_ucst_cig_params_test = NULL;

uint8_t g_lea_ucst_ccid_list_size = 0;

uint8_t g_lea_ucst_ccid_list[APP_LE_AUDIO_UCST_MAX_CCID_LIST_SIZE] = {0};

uint8_t g_lea_ucst_test_mode_flag = 0;

uint8_t g_lea_ucst_pts_set_size = 0;

/**************************************************************************************************
* Prototype
**************************************************************************************************/
extern bt_status_t ble_csip_discover_coordinated_set(bool enable);

extern bt_status_t ble_csip_set_by_sirk(bt_key_t sirk);

extern void bt_app_common_at_cmd_print_report(char *string);

extern bt_status_t bt_gap_le_srv_set_extended_scan(bt_gap_le_srv_set_extended_scan_parameters_t *param, bt_gap_le_srv_set_extended_scan_enable_t *enable, void *callback);

extern bt_status_t le_audio_notify_mic_mute(bt_handle_t handle, bool mic_mute);
/**************************************************************************************************
* Static Functions
**************************************************************************************************/

/**************************************************************************************************
* Public Functions
**************************************************************************************************/
void app_le_audio_ucst_print_lea_adv_addr(bt_addr_t *addr)
{
#if 0
    char conn_string[60] = {0};

    snprintf((char *)conn_string, 60, "LEA Device found! addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
             addr->type,
             addr->addr[5],
             addr->addr[4],
             addr->addr[3],
             addr->addr[2],
             addr->addr[1],
             addr->addr[0]);
    bt_app_common_at_cmd_print_report(conn_string);
#endif
#if 1
    LE_AUDIO_MSGLOG_I("[APP] LEA ADV found! addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,
                      addr->type,
                      addr->addr[5],
                      addr->addr[4],
                      addr->addr[3],
                      addr->addr[2],
                      addr->addr[1],
                      addr->addr[0]);
#endif
}

bool app_le_audio_ucst_is_lea_adv(bt_gap_le_ext_advertising_report_ind_t *adv_report)
{
    uint16_t i = 0;

    while (i < adv_report->data_length) {
        if ((adv_report->data[i] >= 3) &&
            (adv_report->data[i + 1] == BT_GAP_LE_AD_TYPE_SERVICE_DATA) &&
            (adv_report->data[i + 2] == (BT_GATT_UUID16_ASCS_SERVICE & 0xFF)) &&
            (adv_report->data[i + 3] == ((BT_GATT_UUID16_ASCS_SERVICE & 0xFF00) >> 8))) {
            LE_AUDIO_MSGLOG_I("[APP] is_lea_adv, YES!", 0);
            return true;
        }

        i += (adv_report->data[i] + 1);
    }

    //LE_AUDIO_MSGLOG_I("[APP][U] is_lea_adv, NO!", 0);
    return false;
}

bool app_le_audio_ucst_is_connected_device(const bt_addr_t *addr)
{
    uint8_t i;

#if 0
    /* For test */
    LE_AUDIO_MSGLOG_I("[APP] is_connected_device, addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,
                      addr->type,
                      addr->addr[5],
                      addr->addr[4],
                      addr->addr[3],
                      addr->addr[2],
                      addr->addr[1],
                      addr->addr[0]);
#endif

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        if (BT_HANDLE_INVALID == g_lea_ucst_link_info[i].handle) {
            continue;
        }

#if 0
        LE_AUDIO_MSGLOG_I("[APP] connected_list, handle[%x]:%x addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 9,
                          i,
                          g_lea_ucst_link_info[i].handle,
                          g_lea_ucst_link_info[i].addr.type,
                          g_lea_ucst_link_info[i].addr.addr[5],
                          g_lea_ucst_link_info[i].addr.addr[4],
                          g_lea_ucst_link_info[i].addr.addr[3],
                          g_lea_ucst_link_info[i].addr.addr[2],
                          g_lea_ucst_link_info[i].addr.addr[1],
                          g_lea_ucst_link_info[i].addr.addr[0]);
#endif

        if (0 == memcmp(&g_lea_ucst_link_info[i].addr, addr, sizeof(bt_addr_t))) {
            LE_AUDIO_MSGLOG_I("[APP] is_connected_device, YES!", 0);
            return true;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP] is_connected_device, NO!", 0);
    return false;
}

bool app_le_audio_ucst_is_bonded_device(const bt_addr_t *addr)
{
    uint8_t i;

    if (0 == g_lea_ucst_bonded_list.num) {
        return false;
    }

#if 0
    LE_AUDIO_MSGLOG_I("[APP] is_bonded_device, addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,
                      addr->type,
                      addr->addr[5],
                      addr->addr[4],
                      addr->addr[3],
                      addr->addr[2],
                      addr->addr[1],
#endif

    for (i = 0; i < g_lea_ucst_bonded_list.num; i++) {
    //LE_AUDIO_MSGLOG_I("[APP][U] is_bonded_device, i:%x group:%x(size:%x)", 3, i, g_lea_ucst_bonded_list.device[i].group_id, g_lea_ucst_bonded_list.device[i].group_size);
    if ((0 != g_lea_ucst_bonded_list.device[i].group_size) &&
            (0 == memcmp(&(g_lea_ucst_bonded_list.device[i].addr), addr, sizeof(bt_addr_t)))) {
#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
            if ((APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.curr_group) &&
                (g_lea_ucst_bonded_list.device[i].group_id != g_lea_ucst_ctrl.curr_group)) {
                return false;
            }
#endif
            LE_AUDIO_MSGLOG_I("[APP] is_bonded_device, YES!", 0);
            return true;
        }
    }

    return false;
}

bool app_le_audio_ucst_is_bonded_device_all_connected(void)
{
    uint8_t i, count = 0;

    if (0 == g_lea_ucst_bonded_list.num) {
        return true;
    }

    for (i = 0; i < g_lea_ucst_bonded_list.num; i++) {
        if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID != g_lea_ucst_bonded_list.device[i].link_idx) {
            count++;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] is_bonded_device_all_connected, bond_num:%x count:%x", 2, g_lea_ucst_bonded_list.num, count);

    if (g_lea_ucst_bonded_list.num == count) {
        return true;
    }

    return false;
}

bool app_le_audio_ucst_is_active_group(uint8_t group_id)
{
    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.next_group) {
        return false;
    }

    if ((APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.curr_group) &&
        (group_id == g_lea_ucst_ctrl.curr_group)) {
        return true;
    }
    return false;
}

bool app_le_audio_ucst_is_active_group_by_handle(bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = NULL;

    if (NULL == (p_info = app_le_audio_ucst_get_link_info(handle))) {
        return false;
    }

    return app_le_audio_ucst_is_active_group(p_info->group_id);
}

bool app_le_audio_ucst_is_group_device_all_connected(uint8_t group_id)
{
    uint8_t i, num = 0;

    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= group_id) {
        LE_AUDIO_MSGLOG_I("[APP][U] is_group_device_all_connected, ERROR! group:%x", 1, group_id);
        return true;
    }

    if (0 == g_lea_ucst_group_info[group_id].size) {
        LE_AUDIO_MSGLOG_I("[APP][U] is_group_device_all_connected, ERROR! group:%x size:%x", 2,
                          group_id,
                          g_lea_ucst_group_info[group_id].size);
        return true;
    }

    for (i = 0; i < APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM; i++) {
        if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID != g_lea_ucst_group_info[group_id].link_idx[i]) {
            num++;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] is_group_device_all_connected, group:%x size:%x connected:%x", 3,
                      group_id,
                      g_lea_ucst_group_info[group_id].size,
                      num);

    if (g_lea_ucst_group_info[group_id].size == num) {
        return true;
    }

    return false;
}

bool app_le_audio_ucst_is_group_full(void)
{
    uint8_t i, count = 0;

    for (i = 0; i < APP_LE_AUDIO_UCST_GROUP_ID_MAX; i++) {
#if 0
        LE_AUDIO_MSGLOG_I("[APP][U] is_group_full, group:%x size:%x bond_num:%x", 3,
                          i,
                          g_lea_ucst_group_info[i].size,
                          g_lea_ucst_group_info[i].bond_num);
#endif
        if ((0 != g_lea_ucst_group_info[i].size) &&
            (g_lea_ucst_group_info[i].size == g_lea_ucst_group_info[i].bond_num)) {
            count++;
        }
    }

    if (count == APP_LE_AUDIO_UCST_GROUP_ID_MAX) {
        return true;
    }

    return false;
}

bt_status_t app_le_audio_ucst_stop_scan(void)
{
    LE_AUDIO_MSGLOG_I("[APP][U] stop_scan, scan:%x->%x", 2,  g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.next_scan);

    switch (g_lea_ucst_ctrl.curr_scan) {
        case APP_LE_AUDIO_UCST_SCAN_NONE:
        case APP_LE_AUDIO_UCST_SCAN_DISABLING:
        case APP_LE_AUDIO_UCST_SCAN_CS_DISABLING: {
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
            return BT_STATUS_SUCCESS;
        }
        case APP_LE_AUDIO_UCST_SCAN_CS_ENABLING:
        case APP_LE_AUDIO_UCST_SCAN_CS_ENABLED: {
            return BT_STATUS_FAIL;
        }
        case APP_LE_AUDIO_UCST_SCAN_ENABLING: {
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_DISABLING;
            return BT_STATUS_SUCCESS;
        }
        default:
            break;
    }

    /* curr_scan: APP_LE_AUDIO_UCST_SCAN_ENABLED */
    bt_status_t ret;
    bt_hci_cmd_le_set_extended_scan_enable_t enable = {
        .enable = BT_HCI_DISABLE,
        .filter_duplicates = BT_HCI_DISABLE,
        .duration = 0,
        .period = 0
    };

    g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_DISABLING;

    if (BT_STATUS_SUCCESS != (ret = bt_gap_le_srv_set_extended_scan(NULL, &enable, NULL))) {
        g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_ENABLED;
        LE_AUDIO_MSGLOG_I("[APP][U] stop_scan failed, ret:%x", 1,  ret);
    }

    return ret;
}

bt_status_t app_le_audio_ucst_stop_scan_cs(void)
{
    LE_AUDIO_MSGLOG_I("[APP][U] stop_scan_cs, scan:%x->%x", 2,  g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.next_scan);

    switch (g_lea_ucst_ctrl.curr_scan) {
        case APP_LE_AUDIO_UCST_SCAN_NONE:
        case APP_LE_AUDIO_UCST_SCAN_DISABLING:
        case APP_LE_AUDIO_UCST_SCAN_CS_DISABLING: {
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
            return BT_STATUS_SUCCESS;
        }
        case APP_LE_AUDIO_UCST_SCAN_ENABLING:
        case APP_LE_AUDIO_UCST_SCAN_ENABLED: {
            return BT_STATUS_FAIL;
        }
        case APP_LE_AUDIO_UCST_SCAN_CS_ENABLING: {
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_CS_DISABLING;
            return BT_STATUS_SUCCESS;
        }
        default:
            break;
    }

    /* curr_scan: APP_LE_AUDIO_UCST_SCAN_CS_ENABLED */

    bt_status_t ret;

    g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_CS_DISABLING;

    if (BT_STATUS_SUCCESS != (ret = ble_csip_discover_coordinated_set(false))) {
        g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_CS_ENABLED;
        LE_AUDIO_MSGLOG_I("[APP][U] stop_scan_cs failed, ret:%x", 1,  ret);
    } else {
        bt_app_common_at_cmd_print_report("Stop scan CS:\r\n");
    }

    return ret;
}

bt_status_t app_le_audio_ucst_stop_scan_all(void)
{
    if (APP_LE_AUDIO_UCST_SCAN_NONE == g_lea_ucst_ctrl.curr_scan) {
        return BT_STATUS_SUCCESS;
    }
    if ((APP_LE_AUDIO_UCST_SCAN_ENABLING <= g_lea_ucst_ctrl.curr_scan) &&
        (APP_LE_AUDIO_UCST_SCAN_DISABLING >= g_lea_ucst_ctrl.curr_scan)) {
        return app_le_audio_ucst_stop_scan();
    }
    if ((APP_LE_AUDIO_UCST_SCAN_CS_ENABLING <= g_lea_ucst_ctrl.curr_scan) &&
        (APP_LE_AUDIO_UCST_SCAN_CS_DISABLING >= g_lea_ucst_ctrl.curr_scan)) {
        return app_le_audio_ucst_stop_scan_cs();
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t app_le_audio_ucst_start_scan(bool use_white_list)
{
    bt_status_t ret;

    LE_AUDIO_MSGLOG_I("[APP][U] start_scan, scan:%x->%x use_white_list:%x", 3,  g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.next_scan, use_white_list);

    switch (g_lea_ucst_ctrl.curr_scan) {
        case APP_LE_AUDIO_UCST_SCAN_ENABLING:
        case APP_LE_AUDIO_UCST_SCAN_DISABLING:
        case APP_LE_AUDIO_UCST_SCAN_CS_ENABLING:
        case APP_LE_AUDIO_UCST_SCAN_CS_DISABLING: {
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_ENABLING;
            return BT_STATUS_SUCCESS;
        }
        case APP_LE_AUDIO_UCST_SCAN_ENABLED: {
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_ENABLING;
            if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_stop_scan())) {
                g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
            }
            return BT_STATUS_SUCCESS;
        }
        case APP_LE_AUDIO_UCST_SCAN_CS_ENABLED: {
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_ENABLING;
            if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_stop_scan_cs())) {
                g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
            }
            return BT_STATUS_SUCCESS;
        }
        default:
            break;
    }

    /* curr_scan: APP_LE_AUDIO_UCST_SCAN_NONE */
    le_ext_scan_item_t *ext_scan_1M_item;
    if (NULL == (ext_scan_1M_item = (le_ext_scan_item_t *)pvPortMalloc(sizeof(le_ext_scan_item_t)))) {
        return BT_STATUS_FAIL;
    }

    memset(ext_scan_1M_item, 0, sizeof(le_ext_scan_item_t));

    ext_scan_1M_item->scan_type = BT_HCI_SCAN_TYPE_PASSIVE;

    if (use_white_list) {
        ext_scan_1M_item->scan_interval = 0x90;
        ext_scan_1M_item->scan_window = 0x30;
    } else {
        ext_scan_1M_item->scan_interval = 0x90;
        ext_scan_1M_item->scan_window = 0x24;
    }

    bt_hci_le_set_ext_scan_parameters_t params = {
        .own_address_type = BT_HCI_SCAN_ADDR_RANDOM,
        .scanning_filter_policy = BT_HCI_SCAN_FILTER_ACCEPT_ALL_ADVERTISING_PACKETS,
        .scanning_phys_mask = BT_HCI_LE_PHY_MASK_1M,
        .params_phy_1M = ext_scan_1M_item,
        .params_phy_coded = NULL,
    };

    bt_hci_cmd_le_set_extended_scan_enable_t enable = {
        .enable = BT_HCI_ENABLE,
        .filter_duplicates = BT_HCI_ENABLE,
        .duration = 0,
        .period = 0
    };

    if (use_white_list) {
        params.scanning_filter_policy = BT_HCI_SCAN_FILTER_ACCEPT_ONLY_ADVERTISING_PACKETS_IN_WHITE_LIST;
    }
    g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_ENABLING;

    if (BT_STATUS_SUCCESS != (ret = bt_gap_le_srv_set_extended_scan(&params, &enable, NULL))) {
        g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
        LE_AUDIO_MSGLOG_I("[APP][U] start_scan failed, ret:%x", 1,  ret);
    }

    vPortFree(ext_scan_1M_item);

    return ret;
}

bt_status_t app_le_audio_ucst_start_scan_cs(void)
{
    bt_status_t ret;

    LE_AUDIO_MSGLOG_I("[APP][U] start_scan_CS, scan:%x->%x", 2,  g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.next_scan);

    switch (g_lea_ucst_ctrl.curr_scan) {
        case APP_LE_AUDIO_UCST_SCAN_ENABLING:
        case APP_LE_AUDIO_UCST_SCAN_DISABLING:
        case APP_LE_AUDIO_UCST_SCAN_CS_ENABLING:
        case APP_LE_AUDIO_UCST_SCAN_CS_DISABLING: {
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_CS_ENABLING;
            return BT_STATUS_SUCCESS;
        }
        case APP_LE_AUDIO_UCST_SCAN_ENABLED: {
            /* stop_scan--> start_scan_cs */
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_CS_ENABLING;
            if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_stop_scan())) {
                g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
            }
            return BT_STATUS_SUCCESS;
        }
        case APP_LE_AUDIO_UCST_SCAN_CS_ENABLED: {
            /* stop_scan_cs--> start_scan_cs */
            g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_CS_ENABLING;
            if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_stop_scan_cs())) {
                g_lea_ucst_ctrl.next_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
            }
            return BT_STATUS_SUCCESS;
        }
        default:
            break;
    }

    /* curr_scan: APP_LE_AUDIO_UCST_SCAN_NONE */

    g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_CS_ENABLING;

    if (BT_STATUS_SUCCESS != (ret = ble_csip_discover_coordinated_set_by_sirk(true, g_le_audio_sirk))) {
        g_lea_ucst_ctrl.curr_scan = APP_LE_AUDIO_UCST_SCAN_NONE;
        LE_AUDIO_MSGLOG_I("[APP][U] start_scan_CS failed, ret:%x", 1,  ret);

    } else {
        bt_app_common_at_cmd_print_report("Scan CS:\r\n");
    }

    return ret;
}

void app_le_audio_ucst_add_link_to_group(uint8_t group_id, uint8_t link_idx)
{
    uint8_t i;

    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= group_id) {
        return;
    }

    for (i = 0; i < APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM; i++) {
        if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID == g_lea_ucst_group_info[group_id].link_idx[i]) {
            g_lea_ucst_group_info[group_id].link_idx[i] = link_idx;
            break;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] add_link_to_group, group:%x(size:%x) link_idx:(%x %x) bond_num:%x", 5,
                      group_id,
                      g_lea_ucst_group_info[group_id].size,
                      g_lea_ucst_group_info[group_id].link_idx[0],
                      g_lea_ucst_group_info[group_id].link_idx[1],
                      g_lea_ucst_group_info[group_id].bond_num);
}

void app_le_audio_ucst_remove_link_from_group(uint8_t group_id, uint8_t link_idx)
{
    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= group_id) {
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] rm_link_from_group, group:%x(size:%x) link_idx:(%x %x)", 4,
                      group_id,
                      g_lea_ucst_group_info[group_id].size,
                      g_lea_ucst_group_info[group_id].link_idx[0],
                      g_lea_ucst_group_info[group_id].link_idx[1]);

    uint8_t i;

    for (i = 0; i < APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM; i++) {
        if (link_idx == g_lea_ucst_group_info[group_id].link_idx[i]) {
            g_lea_ucst_group_info[group_id].link_idx[i] = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
            break;
        }
    }
}

void app_le_audio_ucst_set_cig_parameter_test(uint8_t bn, uint8_t nse, uint8_t ft, uint16_t iso_interval)
{
    if (NULL == g_lea_ucst_cig_params_test) {
        if (NULL == (g_lea_ucst_cig_params_test = (app_le_audio_ucst_cig_params_test_t *)pvPortMalloc(sizeof(app_le_audio_ucst_cig_params_test_t)))) {
            LE_AUDIO_MSGLOG_I("[APP][U] set_cig_parameter_test, malloc fail", 0);
            return;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] set_cig_parameter_test, bn:%d nse:%d ft:%d iso_interval:%d", 4, bn, nse, ft, iso_interval);
    g_lea_ucst_cig_params_test->bn = bn;
    g_lea_ucst_cig_params_test->nse = nse;
    g_lea_ucst_cig_params_test->ft = ft;
    g_lea_ucst_cig_params_test->iso_interval = iso_interval;
}

void app_le_audio_ucst_set_qos_params(uint8_t sampling_rate, uint8_t sel_setting, bool high_reliability, app_le_audio_ucst_stream_port_t port)
{
    uint8_t sampling_freq;

    LE_AUDIO_MSGLOG_I("[APP][U] set_qos_params, sampling_rate:%d sel_setting:%d high_reliability:%x port:%x", 4, sampling_rate, sel_setting, high_reliability, port);

    if (APP_LE_AUDIO_UCST_QOS_PARAMS_TBL_COUNT <= sel_setting) {
        LE_AUDIO_MSGLOG_I("[APP][U] set_qos_params, invalid sel_setting:%d max:%d", 2, sel_setting, APP_LE_AUDIO_UCST_QOS_PARAMS_TBL_COUNT);
        return;
    }

    switch (port) {
        case APP_LE_AUDIO_UCST_STREAM_PORT_SPK_0: {
            if (APP_LE_AUDIO_SAMPLING_FREQ_INVALID == (sampling_freq = app_le_audio_get_sample_freq(sampling_rate))) {
                LE_AUDIO_MSGLOG_I("[APP][U] set_qos_params SPK_0, invalid sampling_rate:%x", 1, sampling_rate);
                return;
            }

            g_lea_ucst_qos_params_spk_0.sampling_freq = sampling_freq;

            g_lea_ucst_qos_params_spk_0.sdu_size = g_lea_ucst_qos_params_tbl[sel_setting].sdu_size;
            g_lea_ucst_qos_params_spk_0.sdu_interval = g_lea_ucst_qos_params_tbl[sel_setting].sdu_interval;
            g_lea_ucst_qos_params_spk_0.bitrate = g_lea_ucst_qos_params_tbl[sel_setting].bitrate;

            if (high_reliability) {
                g_lea_ucst_qos_params_spk_0.rtn = g_lea_ucst_qos_params_tbl[sel_setting].high_rtn;
                g_lea_ucst_qos_params_spk_0.latency = g_lea_ucst_qos_params_tbl[sel_setting].high_latency;
            } else {
                g_lea_ucst_qos_params_spk_0.rtn = g_lea_ucst_qos_params_tbl[sel_setting].low_rtn;
                g_lea_ucst_qos_params_spk_0.latency = g_lea_ucst_qos_params_tbl[sel_setting].low_latency;
            }

            LE_AUDIO_MSGLOG_I("[APP][U] set_qos_params SPK_0, %d %d %d %d %d %d %d", 7,
                              sampling_rate,
                              g_lea_ucst_qos_params_spk_0.sampling_freq,
                              g_lea_ucst_qos_params_spk_0.sdu_size,
                              g_lea_ucst_qos_params_spk_0.sdu_interval,
                              (int)(g_lea_ucst_qos_params_spk_0.bitrate * 10),
                              g_lea_ucst_qos_params_spk_0.rtn,
                              g_lea_ucst_qos_params_spk_0.latency);
            break;
        }

        case APP_LE_AUDIO_UCST_STREAM_PORT_SPK_1: {
            if (APP_LE_AUDIO_SAMPLING_FREQ_INVALID == (sampling_freq = app_le_audio_get_sample_freq(sampling_rate))) {
                LE_AUDIO_MSGLOG_I("[APP][U] set_qos_params SPK_1, invalid sampling_rate:%x", 1, sampling_rate);
                return;
            }

            g_lea_ucst_qos_params_spk_1.sampling_freq = sampling_freq;

            g_lea_ucst_qos_params_spk_1.bitrate = g_lea_ucst_qos_params_tbl[sel_setting].bitrate;
            g_lea_ucst_qos_params_spk_1.sdu_interval = g_lea_ucst_qos_params_tbl[sel_setting].sdu_interval;
            g_lea_ucst_qos_params_spk_1.sdu_size = g_lea_ucst_qos_params_tbl[sel_setting].sdu_size;

            if (high_reliability) {
                g_lea_ucst_qos_params_spk_1.rtn = g_lea_ucst_qos_params_tbl[sel_setting].high_rtn;
                g_lea_ucst_qos_params_spk_1.latency = g_lea_ucst_qos_params_tbl[sel_setting].high_latency;
            } else {
                g_lea_ucst_qos_params_spk_1.rtn = g_lea_ucst_qos_params_tbl[sel_setting].low_rtn;
                g_lea_ucst_qos_params_spk_1.latency = g_lea_ucst_qos_params_tbl[sel_setting].low_latency;
            }

            LE_AUDIO_MSGLOG_I("[APP][U] set_qos_params SPK_1, %d %d %d %d %d %d %d", 7,
                              sampling_rate,
                              g_lea_ucst_qos_params_spk_1.sampling_freq,
                              g_lea_ucst_qos_params_spk_1.sdu_size,
                              g_lea_ucst_qos_params_spk_1.sdu_interval,
                              (int)(g_lea_ucst_qos_params_spk_1.bitrate * 10),
                              g_lea_ucst_qos_params_spk_1.rtn,
                              g_lea_ucst_qos_params_spk_1.latency);
            break;
        }
        case APP_LE_AUDIO_UCST_STREAM_PORT_MIC_0: {
            if (APP_LE_AUDIO_SAMPLING_FREQ_INVALID == (sampling_freq = app_le_audio_get_sample_freq(sampling_rate))) {
                LE_AUDIO_MSGLOG_I("[APP][U] set_qos_params MIC_0, invalid sampling_rate:%x", 1, sampling_rate);
                return;
            }

            g_lea_ucst_qos_params_mic_0.sampling_freq = sampling_freq;

            g_lea_ucst_qos_params_mic_0.bitrate = g_lea_ucst_qos_params_tbl[sel_setting].bitrate;
            g_lea_ucst_qos_params_mic_0.sdu_interval = g_lea_ucst_qos_params_tbl[sel_setting].sdu_interval;
            g_lea_ucst_qos_params_mic_0.sdu_size = g_lea_ucst_qos_params_tbl[sel_setting].sdu_size;

            if (high_reliability) {
                g_lea_ucst_qos_params_mic_0.rtn = g_lea_ucst_qos_params_tbl[sel_setting].high_rtn;
                g_lea_ucst_qos_params_mic_0.latency = g_lea_ucst_qos_params_tbl[sel_setting].high_latency;
            } else {
                g_lea_ucst_qos_params_mic_0.rtn = g_lea_ucst_qos_params_tbl[sel_setting].low_rtn;
                g_lea_ucst_qos_params_mic_0.latency = g_lea_ucst_qos_params_tbl[sel_setting].low_latency;
            }

            LE_AUDIO_MSGLOG_I("[APP][U] set_qos_params MIC_0, %d %d %d %d %d %d %d", 7,
                              sampling_rate,
                              g_lea_ucst_qos_params_mic_0.sampling_freq,
                              g_lea_ucst_qos_params_mic_0.sdu_size,
                              g_lea_ucst_qos_params_mic_0.sdu_interval,
                              (int)(g_lea_ucst_qos_params_mic_0.bitrate * 10),
                              g_lea_ucst_qos_params_mic_0.rtn,
                              g_lea_ucst_qos_params_mic_0.latency);
            break;
        }
        default:
            break;
    }
}

void app_le_audio_ucst_set_create_cis_mode(app_le_audio_ucst_create_cis_mode_t create_cis_mode)
{
    LE_AUDIO_MSGLOG_I("[APP][U] set_create_cis_mode:%x", 1, create_cis_mode);

    if (APP_LE_AUDIO_UCST_CREATE_CIS_ALWAYS_BIDIRECTIONAL < create_cis_mode) {
        return;
    }

    g_lea_ucst_ctrl.create_cis_mode = create_cis_mode;
}

uint8_t app_le_audio_ucst_set_new_group_info(bt_key_t *sirk, uint8_t link_idx)
{
    uint8_t group_id = 0;

    if (g_lea_ucst_pts_set_size) {
        for (group_id = 0; group_id < APP_LE_AUDIO_UCST_GROUP_ID_MAX; group_id++) {
            if (g_lea_ucst_pts_set_size == g_lea_ucst_group_info[group_id].size) {
                break;
            }
        }
    }

    if ((!g_lea_ucst_pts_set_size) || (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= group_id)) {
        for (group_id = 0; group_id < APP_LE_AUDIO_UCST_GROUP_ID_MAX; group_id++) {
            if (0 == g_lea_ucst_group_info[group_id].size) {
                if (NULL != sirk) {
                    memcpy((uint8_t *)&g_lea_ucst_group_info[group_id].sirk, sirk, sizeof(bt_key_t));
                }

                g_lea_ucst_group_info[group_id].size = 1;    /* update later or no CSIS, 1 device */

                if (g_lea_ucst_pts_set_size) {
                    /* some PTS cases which need 2 or more PTS dongles may have no CSIS */
                    g_lea_ucst_group_info[group_id].size = g_lea_ucst_pts_set_size;
                }

                g_lea_ucst_group_info[group_id].bond_num = 1;
                g_lea_ucst_group_info[group_id].link_idx[0] = link_idx;
                break;
            }
        }
    }

    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > group_id) {
        LE_AUDIO_MSGLOG_I("[APP][U] set_group_info, group:%x(size:%x) link_idx:(%x %x) bond_num:%x", 5,
                          group_id,
                          g_lea_ucst_group_info[group_id].size,
                          g_lea_ucst_group_info[group_id].link_idx[0],
                          g_lea_ucst_group_info[group_id].link_idx[1],
                          g_lea_ucst_group_info[group_id].bond_num);
    }

    return group_id;
}

void app_le_audio_ucst_set_ccid_list(uint8_t ccid_list_size, uint8_t *ccid_list)
{
    if ((APP_LE_AUDIO_UCST_MAX_CCID_LIST_SIZE < ccid_list_size) || (NULL == ccid_list)){
        LE_AUDIO_MSGLOG_I("[APP] set_ccid_list, err ccid_list_size:%x", 1, ccid_list_size);
        return;
    }
    if (0 == ccid_list_size) {
        memset(g_lea_ucst_ccid_list, 0, APP_LE_AUDIO_UCST_MAX_CCID_LIST_SIZE * sizeof(uint8_t));

    } else {
        memcpy(g_lea_ucst_ccid_list, ccid_list, ccid_list_size * sizeof(uint8_t));
    }

    LE_AUDIO_MSGLOG_I("[APP] set_ccid_list, ccid_list_size:%x %x %x", 3, g_lea_ucst_ccid_list_size, g_lea_ucst_ccid_list[0], g_lea_ucst_ccid_list[1]);
    g_lea_ucst_ccid_list_size = ccid_list_size;
}

void app_le_audio_ucst_set_test_mode(uint8_t test_mode)
{
    /* 0: Diable test mode */
    /* 1: Enable test mode and stop before config codec */
    /* 2: Enable test mode */
    g_lea_ucst_test_mode_flag = test_mode;
}

void app_le_audio_ucst_set_sirk(bt_key_t *sirk, bool update_nvkey)
{
    memcpy((uint8_t *)&g_le_audio_sirk, (uint8_t *)sirk, sizeof(bt_key_t));

    if (update_nvkey) {
        bt_lea_csis_data_nvkey_t data_nvkey;
        nvkey_status_t ret = NVKEY_STATUS_ERROR;
        uint32_t size = BLE_CSIS_NVKEY_DATA_LEN;

        /* NVID_BT_LEA_CSIS_DATA: | SIRK (16 bytes) | reserved (2 byte) | */
        if (NVKEY_STATUS_OK == nvkey_read_data(NVID_BT_LEA_CSIS_DATA, (uint8_t*)&data_nvkey, &size)) {
            memcpy(&data_nvkey, (uint8_t *)sirk, sizeof(bt_key_t));
            ret = nvkey_write_data(NVID_BT_LEA_CSIS_DATA, (uint8_t*)&data_nvkey, size);
        }

        LE_AUDIO_MSGLOG_I("[APP][U] set_sirk, nvkey write ret:%x", 1, ret);
    }
}

bt_key_t *app_le_audio_ucst_get_sirk(bool from_nvkey)
{
    if (from_nvkey) {
        bt_lea_csis_data_nvkey_t data_nvkey;
        uint32_t size = BLE_CSIS_NVKEY_DATA_LEN;

        /* NVID_BT_LEA_CSIS_DATA: | SIRK (16 bytes) | reserved (2 byte) | */
        if (NVKEY_STATUS_OK == nvkey_read_data(NVID_BT_LEA_CSIS_DATA, (uint8_t*)&data_nvkey, &size)) {
            memcpy((uint8_t *)&g_le_audio_sirk, (uint8_t*)&data_nvkey, sizeof(bt_key_t));
            LE_AUDIO_MSGLOG_I("[APP][U] get_sirk, nvkey read success", 0);
            return &g_le_audio_sirk;
        }

        LE_AUDIO_MSGLOG_I("[APP][U] get_sirk, nvkey read failed", 0);
        return NULL;

    }

    return &g_le_audio_sirk;
}

void app_le_audio_ucst_set_csip_sirk(void)
{
    app_le_audio_ucst_get_sirk(true);
    ble_csip_set_by_sirk(g_le_audio_sirk);
}

bt_handle_t app_le_audio_ucst_get_handle(uint8_t link_idx)
{
    return g_lea_ucst_link_info[link_idx].handle;
}


uint8_t app_le_audio_ucst_get_link_num(void)
{
    uint8_t i = 0, count = 0;

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) {
            count++;
        }
    }

    return count;
}

uint8_t app_le_audio_ucst_get_link_idx(bt_handle_t handle)
{
    uint8_t i = 0;

    if (BT_HANDLE_INVALID == handle) {
        return APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
    }

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        if (handle == g_lea_ucst_link_info[i].handle) {
            return i;
        }
    }

    return APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
}

uint8_t app_le_audio_ucst_get_link_idx_by_cis(bt_handle_t cis_handle, uint8_t *cis_idx)
{
    uint8_t i, link_idx;

    i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;

    while (i > 0) {
        i--;
        if (g_lea_ucst_cis_info[i].cis_handle == cis_handle) {
            if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID != (link_idx = app_le_audio_ucst_get_link_idx(g_lea_ucst_cis_info[i].acl_handle))) {
                *cis_idx = i;
                return link_idx;
            }
        }
    }

    *cis_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
    return APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
}

uint8_t app_le_audio_ucst_get_link_idx_by_addr(bt_addr_t *addr)
{
    uint8_t i = 0;

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        if (0 == memcmp((uint8_t *)&g_lea_ucst_link_info[i].addr, addr, sizeof(bt_addr_t))) {
            return i;
        }
    }

    return APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
}

app_le_audio_ucst_link_info_t *app_le_audio_ucst_get_link_info(bt_handle_t handle)
{
    uint8_t i = 0;

    if (BT_HANDLE_INVALID == handle) {
        return NULL;
    }

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        if (handle == g_lea_ucst_link_info[i].handle) {
            return &g_lea_ucst_link_info[i];
        }
    }

    return NULL;
}

app_le_audio_ucst_link_info_t *app_le_audio_ucst_get_link_info_by_idx(uint8_t link_idx)
{
    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= link_idx) {
        return NULL;
    }

    return &g_lea_ucst_link_info[link_idx];
}

app_le_audio_ucst_link_info_t *app_le_audio_ucst_get_link_info_by_addr(bt_addr_t *addr)
{
    uint8_t i = 0;

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        if (0 == memcmp((uint8_t *)&g_lea_ucst_link_info[i].addr, addr, sizeof(bt_addr_t))) {
            return &g_lea_ucst_link_info[i];
        }
    }

    return NULL;
}

app_le_audio_ucst_group_info_t *app_le_audio_ucst_get_group_info(uint8_t group_id)
{
    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= group_id) {
        return NULL;
    }

    return &g_lea_ucst_group_info[group_id];
}

uint8_t app_le_audio_ucst_get_group_id_by_sirk(bt_key_t *sirk)
{
    uint8_t i;

    i = APP_LE_AUDIO_UCST_GROUP_ID_MAX;
    while (i > 0) {
        i--;
        if ((0 != g_lea_ucst_group_info[i].size) &&
            (0 == memcmp((uint8_t *) & (g_lea_ucst_group_info[i].sirk), sirk, sizeof(bt_key_t)))) {
            return i;
        }
    }

    return APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
}

uint8_t app_le_audio_ucst_get_group_link_num(uint8_t group_id)
{
    uint8_t i, link_num = 0;

    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= group_id) {
        return 0;
    }

    if (0 == g_lea_ucst_group_info[group_id].size) {
        return 0;
    }

    for (i = 0; i < APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM; i++) {
        if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID != g_lea_ucst_group_info[group_id].link_idx[i]) {
            link_num++;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] get_group_link_num, group:%x size:%x link_num:%x", 3,
                      group_id,
                      g_lea_ucst_group_info[group_id].size,
                      link_num);

    return link_num;
}

uint8_t app_le_audio_ucst_get_bonded_list_idx(bt_addr_t *addr)
{
    uint8_t i;

    if (0 == g_lea_ucst_bonded_list.num) {
        return APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID;
    }

#if 0
    LE_AUDIO_MSGLOG_I("[APP] is_bonded_device, addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,
                      addr->type,
                      addr->addr[5],
                      addr->addr[4],
                      addr->addr[3],
                      addr->addr[2],
                      addr->addr[1],
#endif

    for (i = 0; i < g_lea_ucst_bonded_list.num; i++) {
    if ((0 != g_lea_ucst_bonded_list.device[i].group_size) &&
            (0 == memcmp(&(g_lea_ucst_bonded_list.device[i].addr), addr, sizeof(bt_addr_t)))) {
            LE_AUDIO_MSGLOG_I("[APP][U] is_bonded_device, YES! i:%x group:%x(size:%x)", 3,
                              i,
                              g_lea_ucst_bonded_list.device[i].group_id,
                              g_lea_ucst_bonded_list.device[i].group_size);
            return i;
        }
    }

    return APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID;
}

uint8_t app_le_audio_ucst_get_empty_bonded_list_idx(void)
{
    uint8_t i;

    if (APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM == g_lea_ucst_bonded_list.num) {
        LE_AUDIO_MSGLOG_I("[APP][U] bonded_list, num:%x", 1, g_lea_ucst_bonded_list.num);
        return APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID;
    }

#if 0
    LE_AUDIO_MSGLOG_I("[APP] is_bonded_device, addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x", 7,
                      addr->type,
                      addr->addr[5],
                      addr->addr[4],
                      addr->addr[3],
                      addr->addr[2],
                      addr->addr[1],
#endif

    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
        if ((APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_bonded_list.device[i].group_id) &&
            (0 == g_lea_ucst_bonded_list.device[i].group_size)) {
            LE_AUDIO_MSGLOG_I("[APP][U] get_empty_bonded_list, i:%x group:%x(size:%x)", 3, i, g_lea_ucst_bonded_list.device[i].group_id, g_lea_ucst_bonded_list.device[i].group_size);
            return i;
        }
    }

    return APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID;
}

uint8_t app_le_audio_ucst_get_cis_num(void)
{
    uint8_t i, cis_num = 0;

    i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;

    while (i > 0) {
        i--;
        if ((BT_HANDLE_INVALID != g_lea_ucst_cis_info[i].cis_handle) &&
            (BT_HANDLE_INVALID != g_lea_ucst_cis_info[i].acl_handle)) {
            cis_num++;
        }
    }

    return cis_num;
}

app_le_audio_ucst_conn_t app_le_audio_ucst_get_curr_conn_type(void)
{
    return g_lea_ucst_ctrl.curr_conn;
}

app_le_audio_ucst_target_t app_le_audio_ucst_get_curr_target(void)
{
    return g_lea_ucst_ctrl.curr_target;
}

app_le_audio_ucst_pause_stream_t app_le_audio_ucst_get_pause_stream_flag(void)
{
    return g_lea_ucst_ctrl.pause_stream;
}

app_le_audio_ucst_create_cis_mode_t app_le_audio_ucst_get_create_cis_mode(void)
{
    return g_lea_ucst_ctrl.create_cis_mode;
}

uint32_t app_le_audio_ucst_get_sampling_rate(bool mic)
{
    uint8_t sampling_freq;

    if (mic) {
        sampling_freq = g_lea_ucst_qos_params_mic_0.sampling_freq;

    } else {
        if (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) {
            sampling_freq = g_lea_ucst_qos_params_spk_0.sampling_freq;
        } else {
            sampling_freq = g_lea_ucst_qos_params_spk_1.sampling_freq;
        }
    }

    switch (sampling_freq) {
        case CODEC_CONFIGURATION_SAMPLING_FREQ_8KHZ:
            return 8000;
        case CODEC_CONFIGURATION_SAMPLING_FREQ_16KHZ:
            return 16000;
        case CODEC_CONFIGURATION_SAMPLING_FREQ_24KHZ:
            return 24000;
        case CODEC_CONFIGURATION_SAMPLING_FREQ_32KHZ:
            return 32000;
        case CODEC_CONFIGURATION_SAMPLING_FREQ_44_1KHZ:
            return 44100;
        case CODEC_CONFIGURATION_SAMPLING_FREQ_48KHZ:
            return 48000;
        default:
            break;
    }

    return 0;
}

uint16_t app_le_audio_ucst_get_sdu_size(bool mic)
{
    if (mic) {
        return g_lea_ucst_qos_params_mic_0.sdu_size;
    }

    if (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) {
        return g_lea_ucst_qos_params_spk_0.sdu_size;
    }

    return g_lea_ucst_qos_params_spk_1.sdu_size;
}

uint32_t app_le_audio_ucst_get_sdu_interval(bool mic)
{
    uint8_t sdu_interval;

    if (mic) {
        sdu_interval = g_lea_ucst_qos_params_mic_0.sdu_interval;

    } else {
        if (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) {
            sdu_interval = g_lea_ucst_qos_params_spk_0.sdu_interval;
        } else {
            sdu_interval = g_lea_ucst_qos_params_spk_1.sdu_interval;
        }
    }

    return g_lea_sdu_interval_tbl[sdu_interval];
}

float app_le_audio_ucst_get_bitrate(bool mic)
{
    if (mic) {
        return g_lea_ucst_qos_params_mic_0.bitrate;
    }
    if (APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == g_lea_ucst_ctrl.curr_target) {
        return g_lea_ucst_qos_params_spk_0.bitrate;
    }
    return g_lea_ucst_qos_params_spk_1.bitrate;
}

uint32_t app_le_audio_ucst_get_location(uint8_t idx, uint32_t location)
{
    uint8_t i;

    if (32 <= idx) {
        return 0;
    }

    location = (location >> idx);
    i = idx;

    for(; i < 32; i++) {
        LE_AUDIO_MSGLOG_I("[APP][U] get_location, i:%x idx:%x location:%x", 3, i, idx, location);
        if (location & 0x00000001) {
            break;
        }
        location = (location >> 1);
        idx++;
    }

    LE_AUDIO_MSGLOG_I("[APP][U] get_location, idx:%x", 1, idx);

    if (32 == idx) {
        return 0;
    }

    return (1 << idx);
}

uint8_t app_le_audio_ucst_get_location_count(uint32_t location)
{
    uint8_t i, count = 0;

    for(i = 0; i < 32; i++) {
        if (location & 0x00000001) {
            count++;
        }
        location = (location >> 1);
    }

    LE_AUDIO_MSGLOG_I("[APP][U] get_location_count, count:%x", 1, count);

    if (APP_LE_AUDIO_UCST_LOCATION_NUM_2 < count) {
        count = APP_LE_AUDIO_UCST_LOCATION_NUM_2;
    }

    return count;
}

uint32_t app_le_audio_ucst_get_available_channel(void)
{
    uint8_t i;

    for (i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
        if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) {
            return g_lea_ucst_link_info[i].sink_location;
        }
    }

    return AUDIO_LOCATION_NONE;
}

uint8_t app_le_audio_ucst_get_active_group(void)
{
    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_ctrl.next_group) {
        return g_lea_ucst_ctrl.next_group;
    }

    return g_lea_ucst_ctrl.curr_group;
}

uint8_t app_le_audio_ucst_get_active_group_address(app_le_audio_ucst_active_group_addr_list_t *addr_list)
{
    app_le_audio_ucst_group_info_t *p_group = NULL;
    app_le_audio_ucst_link_info_t *p_info = NULL;
    uint8_t group_id, num = 0;

    if (NULL == addr_list) {
        return num;
    }

    group_id = app_le_audio_ucst_get_active_group();

    if (NULL == (p_group = app_le_audio_ucst_get_group_info(group_id))) {
        return num;
    }


    if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID != p_group->link_idx[0]) {
        if (NULL != (p_info = app_le_audio_ucst_get_link_info_by_idx(p_group->link_idx[0]))) {
            LE_AUDIO_MSGLOG_I("[APP][U] get_active_group_addr num:%x link_idx[0]:%x handle:%x", 3, num, p_group->link_idx[0], p_info->handle);
            memcpy((uint8_t *)&(addr_list->addr[num]), (uint8_t *)&(p_info->addr), sizeof(bt_addr_t));
            num++;
        }
    }

    if (APP_LE_AUDIO_UCST_LINK_IDX_INVALID != p_group->link_idx[1]) {
        if (NULL != (p_info = app_le_audio_ucst_get_link_info_by_idx(p_group->link_idx[1]))) {
            LE_AUDIO_MSGLOG_I("[APP][U] get_active_group_addr num:%x link_idx[1]:%x handle:%x", 3, num, p_group->link_idx[1], p_info->handle);
            memcpy((uint8_t *)&(addr_list->addr[num]), (uint8_t *)&(p_info->addr), sizeof(bt_addr_t));
            num++;
        }
    }
    addr_list->num = num;

    return num;
}

void app_le_audio_ucst_reset_cig_parameter_test(void)
{
    if (NULL != g_lea_ucst_cig_params_test) {
        vPortFree(g_lea_ucst_cig_params_test);
        g_lea_ucst_cig_params_test = NULL;
    }
}

void app_le_audio_ucst_reset_all_cis_info(void)
{
    uint8_t i;

    memset(g_lea_ucst_cis_info, 0, sizeof(app_le_audio_ucst_cis_info_t)*APP_LE_AUDIO_UCST_CIS_MAX_NUM);

    i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;
    while (i > 0) {
        i--;
        g_lea_ucst_cis_info[i].cis_handle = BT_HANDLE_INVALID;
        g_lea_ucst_cis_info[i].acl_handle = BT_HANDLE_INVALID;
    }
}

void app_le_audio_ucst_reset_link_info(uint8_t link_idx)
{
    app_le_audio_ucst_link_info_t *link_info = NULL;
    uint8_t i = APP_LE_AUDIO_UCST_CIS_MAX_NUM;

    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM <= link_idx) {
        LE_AUDIO_MSGLOG_W("[APP] reset_link_info. out of range. link_idx:%d", 1, link_idx);
        return;
    }

    link_info = &g_lea_ucst_link_info[link_idx];

    while (i > 0) {
        i--;
        if (link_info->handle == g_lea_ucst_cis_info[i].acl_handle) {
            g_lea_ucst_cis_info[i].acl_handle = BT_HANDLE_INVALID;
            g_lea_ucst_cis_info[i].cis_status = APP_LE_AUDIO_UCST_CIS_IDLE;
            /* keep cis handle for device reconnect */
        }
    }

    if (link_info->le_connection_timer_handle) {
        LE_AUDIO_MSGLOG_I("[APP] reset_link_info. timer_handle:%x", 1, link_info->le_connection_timer_handle);
        app_le_audio_timer_stop(link_info->le_connection_timer_handle);
    }

    memset(link_info, 0, sizeof(app_le_audio_ucst_link_info_t));
    link_info->handle = BT_HANDLE_INVALID;
    link_info->group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    link_info->bond_idx = APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID;
}

void app_le_audio_ucst_reset_all_link_info(void)
{
    for (int32_t i = 0; i < APP_LE_AUDIO_UCST_LINK_MAX_NUM; i++) {
        app_le_audio_ucst_reset_link_info(i);
    }
}

void app_le_audio_ucst_reset_group_info(uint8_t group_id)
{
    uint8_t i;

    if (APP_LE_AUDIO_UCST_GROUP_ID_MAX <= group_id) {
        return;
    }

    memset(&g_lea_ucst_group_info[group_id], 0, sizeof(app_le_audio_ucst_group_info_t));

    for (i = 0; i < APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM; i++) {
        g_lea_ucst_group_info[group_id].link_idx[i] = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
    }

    if (g_lea_ucst_ctrl.curr_group == group_id) {
        if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID == g_lea_ucst_ctrl.next_group) {
            g_lea_ucst_ctrl.curr_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
        }
    }
    if (g_lea_ucst_active_group == group_id) {
        g_lea_ucst_active_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
        app_le_audio_ucst_write_active_group_to_nvkey();
    }
}

void app_le_audio_ucst_reset_all_group_info(void)
{
    uint8_t i;

    i = APP_LE_AUDIO_UCST_GROUP_ID_MAX;
    while (i > 0) {
        i--;
        app_le_audio_ucst_reset_group_info(i);
    }
}

void app_le_audio_ucst_reset_bonded_list(uint8_t bond_idx)
{
    if (APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM <= bond_idx) {
        return;
    }

    memset(&(g_lea_ucst_bonded_list.device[bond_idx]), 0, sizeof(app_le_audio_ucst_bonded_device_t));
    g_lea_ucst_bonded_list.device[bond_idx].group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    g_lea_ucst_bonded_list.device[bond_idx].link_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
}

void app_le_audio_ucst_reset_all_bonded_list(void)
{
    uint8_t i;

    memset(&g_lea_ucst_bonded_list, 0, sizeof(app_le_audio_ucst_bonded_list_t));

    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
        g_lea_ucst_bonded_list.device[i].group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
        g_lea_ucst_bonded_list.device[i].link_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
    }

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    memset(&g_lea_ucst_sirk_info[0], 0, sizeof(bt_lea_sink_info_nvkey_t)*APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM);

    for (i = 0; i < APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM; i++) {
        g_lea_ucst_sirk_info[i].group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    }
#endif
}

void app_le_audio_ucst_reset_active_group(void)
{
    g_lea_ucst_active_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    g_lea_ucst_ctrl.curr_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    g_lea_ucst_ctrl.next_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
    g_lea_ucst_ctrl.latest_group = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
}

void app_le_audio_ucst_reset_all_bonded_info(void)
{
    bt_device_manager_le_clear_all_bonded_info();
    app_le_audio_ucst_reset_all_bonded_list();
    app_le_audio_ucst_write_bonded_list_to_nvkey();
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
    void app_le_audio_ucst_write_sirk_list_to_nvkey();
#endif
    app_le_audio_ucst_reset_all_group_info();
    g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
    app_le_audio_ucst_stop_scan_all();
}

void app_le_audio_ucst_refresh_bonded_list(uint8_t empty_idx)
{
    uint8_t i;

    if (0 == g_lea_ucst_bonded_list.num) {
        return;
    }

    if (empty_idx < (g_lea_ucst_bonded_list.num - 1)) {
        for (i = empty_idx; i < g_lea_ucst_bonded_list.num; i++) {
            //LE_AUDIO_MSGLOG_I("[APP][U] refresh_bonded_list, %x", 1, i);
            if (g_lea_ucst_bonded_list.num == (i + 1)) {
                break;
            }
            memcpy(&(g_lea_ucst_bonded_list.device[i]), &(g_lea_ucst_bonded_list.device[i + 1]), sizeof(app_le_audio_ucst_bonded_device_t));
            app_le_audio_ucst_reset_bonded_list(i + 1);
        }
    }

#if 1 /* for test */
    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
        bt_addr_t *addr = NULL;
        addr = &(g_lea_ucst_bonded_list.device[i].addr);
        LE_AUDIO_MSGLOG_I("[APP][U] refresh_bonded_list[%x], addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x group:%x(size:%x)", 10, i,
                          addr->type,
                          addr->addr[5],
                          addr->addr[4],
                          addr->addr[3],
                          addr->addr[2],
                          addr->addr[1],
                          addr->addr[0],
                          g_lea_ucst_bonded_list.device[i].group_id,
                          g_lea_ucst_bonded_list.device[i].group_size);
    }
#endif
}

void app_le_audio_ucst_refresh_all_bonded_list(void)
{
    uint8_t i, num;
    bool write_nvkey = false;

    if (0 == g_lea_ucst_bonded_list.num) {
        return;
    }

    /* remove invalid bonded record */
    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
        if ((APP_LE_AUDIO_UCST_GROUP_ID_INVALID != g_lea_ucst_bonded_list.device[i].group_id) &&
            (APP_LE_AUDIO_UCST_BONDED_RECORD_INVALID & g_lea_ucst_bonded_list.device[i].group_id)) {
            app_le_audio_ucst_reset_bonded_list(i);
            app_le_audio_ucst_refresh_bonded_list(i);
            g_lea_ucst_bonded_list.num--;
            write_nvkey = true;
        }
    }

    num = 0;
    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
        if (APP_LE_AUDIO_UCST_GROUP_ID_MAX > g_lea_ucst_bonded_list.device[i].group_id) {
            num++;
        }
    }

    g_lea_ucst_bonded_list.num = num;

    /* write to nvkey */
    if (write_nvkey) {
        app_le_audio_ucst_write_bonded_list_to_nvkey();
    }

    LE_AUDIO_MSGLOG_I("[APP] refresh_all_bonded_list, num:%x write_nvkey:%x", 2, g_lea_ucst_bonded_list.num, write_nvkey);
}

void app_le_audio_ucst_refresh_group_info(void)
{
    uint8_t i, group_id;

    if (0 == g_lea_ucst_bonded_list.num) {
        return;
    }

    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
        if ((APP_LE_AUDIO_UCST_GROUP_ID_MAX > (group_id = g_lea_ucst_bonded_list.device[i].group_id)) &&
            (0 != g_lea_ucst_bonded_list.device[i].group_size)) {
            g_lea_ucst_group_info[group_id].size = g_lea_ucst_bonded_list.device[i].group_size;



            g_lea_ucst_group_info[group_id].bond_num++;
        }
    }

#if 1   /* TEST */
    for (group_id = 0; group_id < APP_LE_AUDIO_UCST_GROUP_ID_MAX; group_id++) {
        LE_AUDIO_MSGLOG_I("[APP] refresh_group_info, group:%x(size:%x) link_idx:(%x %x) bond_num:%x", 5,
                          group_id,
                          g_lea_ucst_group_info[group_id].size,
                          g_lea_ucst_group_info[group_id].link_idx[0],
                          g_lea_ucst_group_info[group_id].link_idx[1],
                          g_lea_ucst_group_info[group_id].bond_num);
    }
#endif
}

void app_le_audio_ucst_read_bonded_list_from_nvkey(void)
{
    nvkey_status_t ret = NVKEY_STATUS_OK;
    uint32_t size = APP_LE_AUDIO_NVKEY_BONDED_LIST_LEN;
    bt_lea_ucst_bonded_list_nvkey_t buf_nvkey;
    uint8_t i;

    /* read NVID_BT_LEA_BONDED_LIST: 36 byte */
    /* NVID_BT_LEA_BONDED_LIST: 36 = | bonded_device * APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM (4) | */
    /* bonded_device: | addr (7 bytes) + group_id (1 byte) + group_size(1 byte) | */
    ret = nvkey_read_data(NVID_BT_LEA_BONDED_LIST, (uint8_t*)&buf_nvkey, &size);
    LE_AUDIO_MSGLOG_I("[APP][U] read_bonded_list(nvkey), ret:%x size:%d", 2, ret, size);

    if (NVKEY_STATUS_OK == ret) {
        uint8_t num = 0;

        /* update bonded list */
        for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
            memcpy(&(g_lea_ucst_bonded_list.device[i].addr), &buf_nvkey.device[i].addr, sizeof(bt_addr_t));
            g_lea_ucst_bonded_list.device[i].group_id = buf_nvkey.device[i].group_id;
            g_lea_ucst_bonded_list.device[i].in_white_list = true;
            if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID != g_lea_ucst_bonded_list.device[i].group_id) {
                num++;
            }
            g_lea_ucst_bonded_list.device[i].group_size =  buf_nvkey.device[i].group_size;

#if 1   /* For test */
            {
                bt_addr_t *addr = NULL;
                addr = &(g_lea_ucst_bonded_list.device[i].addr);
                LE_AUDIO_MSGLOG_I("[APP] bonded_list[%x], addrType:%x addr:%02x:%02x:%02x:%02x:%02x:%02x group:%x(size:%x)", 10, i,
                                  addr->type,
                                  addr->addr[5],
                                  addr->addr[4],
                                  addr->addr[3],
                                  addr->addr[2],
                                  addr->addr[1],
                                  addr->addr[0],
                                  g_lea_ucst_bonded_list.device[i].group_id,
                                  g_lea_ucst_bonded_list.device[i].group_size);
            }
#endif
        }

        g_lea_ucst_bonded_list.num = num;
        LE_AUDIO_MSGLOG_I("[APP] bonded_list, total_num:%x", 1, g_lea_ucst_bonded_list.num);
    }
}

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
void app_le_audio_ucst_read_sirk_list_from_nvkey(void)
{
    nvkey_status_t ret = NVKEY_STATUS_OK;
    uint32_t size = APP_LE_AUDIO_NVKEY_SIRK_LIST_LEN;

    /* read NVID_BT_LEA_SIRK_LIST: 34 byte */
    /* NVID_BT_LEA_SIRK_LIST: 34 = (| SIRK (16 bytes) + group_id (1 byte) |) * 2 */
    ret = nvkey_read_data(NVID_BT_LEA_SIRK_LIST, (uint8_t *)&g_lea_ucst_sirk_info, &size);
    LE_AUDIO_MSGLOG_I("[APP][U] read_sirk_list(nvkey), ret:%x size:%d", 2, ret, size);

    if (NVKEY_STATUS_OK == ret) {
        bt_lea_sink_info_nvkey_t *ptr = NULL;
        uint8_t i;

        for (i = 0; i < APP_LE_AUDIO_NVKEY_SIRK_INFO_MAX_NUM; i++) {
            ptr = &g_lea_ucst_sirk_info[i];
            LE_AUDIO_MSGLOG_I("[APP][U] read_sirk_list, [%d]group:%x sirk:%x %x %x %x %x %x %x %x", 10,
                              i, ptr->group_id,
                              ptr->sirk[0], ptr->sirk[1], ptr->sirk[2], ptr->sirk[3],
                              ptr->sirk[4], ptr->sirk[5], ptr->sirk[6], ptr->sirk[7]);
            LE_AUDIO_MSGLOG_I("[APP][U] read_sirk_list, sirk:%x %x %x %x %x %x %x %x", 8,
                              ptr->sirk[8], ptr->sirk[9], ptr->sirk[10], ptr->sirk[11],
                              ptr->sirk[12], ptr->sirk[13], ptr->sirk[14], ptr->sirk[15]);
        }
    }
}
#endif

void app_le_audio_ucst_read_active_group_from_nvkey(void)
{
    nvkey_status_t ret = NVKEY_STATUS_OK;
    uint32_t size = APP_LE_AUDIO_NVKEY_ACTIVE_GROUP_LEN;

    /* read NVID_BT_LEA_ACTIVE_GROUP: 1 byte */
    /* NVID_BT_LEA_ACTIVE_GROUP: 1 = | active_group (1 byte) | */
    ret = nvkey_read_data(NVID_BT_LEA_ACTIVE_GROUP, &g_lea_ucst_active_group, &size);
    LE_AUDIO_MSGLOG_I("[APP][U] read_active_group(nvkey), ret:%x size:%d active_group:%x", 3, ret, size, g_lea_ucst_active_group);

    if (NVKEY_STATUS_OK == ret) {
        /* update active group */
        if (APP_LE_AUDIO_UCST_GROUP_ID_INVALID != g_lea_ucst_active_group) {
            g_lea_ucst_ctrl.curr_group = g_lea_ucst_active_group;
        }
    }
}

void app_le_audio_ucst_write_bonded_list_to_nvkey(void)
{
    nvkey_status_t ret = NVKEY_STATUS_ERROR;
    uint32_t size = APP_LE_AUDIO_NVKEY_BONDED_LIST_LEN;
    bt_lea_ucst_bonded_list_nvkey_t buf_nvkey;
    uint8_t i = 0, j = 0;

    /* write NVID_BT_LEA_BONDED_LIST: 36 byte */
    /* NVID_BT_LEA_BONDED_LIST: 36 = | bonded_device * APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM (4) | */
    /* bonded_device: | addr (7 bytes) + group_id (1 byte) + group_size(1 byte) | */
    for (i = 0; i < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; i++) {
        for (; j < APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM; j++) {
            if (!g_lea_ucst_bonded_list.device[j].deleting) {
                break;
            }
        }
        if (APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM > j) {
            memcpy(&buf_nvkey.device[i].addr, &(g_lea_ucst_bonded_list.device[j].addr), sizeof(bt_addr_t));
            buf_nvkey.device[i].group_id = g_lea_ucst_bonded_list.device[j].group_id;
            buf_nvkey.device[i].group_size = g_lea_ucst_bonded_list.device[j].group_size;

        } else {
            memset(&buf_nvkey.device[i].addr, 0, sizeof(bt_addr_t));
            buf_nvkey.device[i].group_id = APP_LE_AUDIO_UCST_GROUP_ID_INVALID;
            buf_nvkey.device[i].group_size = 0;
        }
        LE_AUDIO_MSGLOG_I("[APP][U] write_bonded_list, [%x] %x, %02x %02x %02x %02x %02x %02x, %x %x", 10, i,
                          buf_nvkey.device[i].addr.type,
                          buf_nvkey.device[i].addr.addr[0],
                          buf_nvkey.device[i].addr.addr[1],
                          buf_nvkey.device[i].addr.addr[2],
                          buf_nvkey.device[i].addr.addr[3],
                          buf_nvkey.device[i].addr.addr[4],
                          buf_nvkey.device[i].addr.addr[5],
                          buf_nvkey.device[i].group_id,
                          buf_nvkey.device[i].group_size);
        j++;
    }

    ret = nvkey_write_data(NVID_BT_LEA_BONDED_LIST, (uint8_t*)&buf_nvkey, size);
    LE_AUDIO_MSGLOG_I("[APP][U] write_bonded_list(nvkey), ret:%x", 1, ret);
}

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
void app_le_audio_ucst_write_sirk_list_to_nvkey(void)
{
    nvkey_status_t ret = NVKEY_STATUS_OK;
    uint32_t size = APP_LE_AUDIO_NVKEY_SIRK_LIST_LEN;

    /* read NVID_BT_LEA_SIRK_LIST: 34 byte */
    /* NVID_BT_LEA_SIRK_LIST: 34 = (| SIRK (16 bytes) + group_id (1 byte) |) * 2 */
    ret = nvkey_write_data(NVID_BT_LEA_SIRK_LIST, (uint8_t *)&g_lea_ucst_sirk_info, size);
    LE_AUDIO_MSGLOG_I("[APP][U] write_sirk_list(nvkey), ret:%x size:%d", 2, ret, size);
}
#endif

void app_le_audio_ucst_write_active_group_to_nvkey(void)
{
    nvkey_status_t ret = NVKEY_STATUS_ERROR;
    uint32_t size = APP_LE_AUDIO_NVKEY_ACTIVE_GROUP_LEN;

    /* write NVID_BT_LEA_ACTIVE_GROUP: 1 byte */
    /* NVID_BT_LEA_ACTIVE_GROUP: 1 = | active_group (1 byte) | */
    ret = nvkey_write_data(NVID_BT_LEA_ACTIVE_GROUP, &g_lea_ucst_active_group, size);
    LE_AUDIO_MSGLOG_I("[APP][U] write_active_group(nvkey), ret:%x active_group:%x", 2, ret, g_lea_ucst_active_group);
}

bt_status_t app_le_audio_ucst_connect_bonded_device(bool search_cs)
{
    bt_status_t ret;

    if (0 == g_lea_ucst_bonded_list.num) {
        LE_AUDIO_MSGLOG_I("[APP] connect_bonded_device, bonded_list empty", 0);
        return BT_STATUS_CONNECTION_NOT_FOUND;
    }

    if (APP_LE_AUDIO_UCST_SET_WHITE_LIST_ADD_ON_GOING == g_lea_ucst_set_white_list.state) {
        LE_AUDIO_MSGLOG_I("[APP] connect_bonded_device, wait for set white list", 0);
        return BT_STATUS_BUSY;
    }

    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == app_le_audio_ucst_get_link_num()) {
        LE_AUDIO_MSGLOG_I("[APP][U] connect_bonded_device, link full", 0);
#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        return BT_STATUS_FAIL;
#endif
    }

    LE_AUDIO_MSGLOG_I("[APP][U] connect_bonded_device, scan:%x->%x conn:%x", 3,
                      g_lea_ucst_ctrl.curr_scan,
                      g_lea_ucst_ctrl.next_scan,
                      g_lea_ucst_ctrl.curr_conn);

    bt_app_common_at_cmd_print_report("Scan bonded device:\r\n");

    if (search_cs) {
        g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_CS_AND_BONDED_DEVICE;    /* scan bonded device and CS */
    } else {
        g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_BONDED_DEVICE;           /* scan bonded device only */
    }

    if (APP_LE_AUDIO_UCST_CONN_BONDED_DEVICE == g_lea_ucst_ctrl.curr_conn) {
        if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_start_scan(true))) {      /* use white list */
            g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
        }
    } else {
        if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_start_scan(false))) {     /* normal scan */
            g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] connect_bonded_device, curr_scan:%x conn:%x", 2, g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.curr_conn);

    return ret;
}

bt_status_t app_le_audio_ucst_connect_coordinated_set(bool use_nvkey_sirk)
{
    uint8_t link_num = 0;
    bt_status_t ret;

    if (APP_LE_AUDIO_UCST_LINK_MAX_NUM == (link_num = app_le_audio_ucst_get_link_num())) {
        LE_AUDIO_MSGLOG_I("[APP][U] connect_cs, link full:%x", 1, link_num);
#ifndef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
        return BT_STATUS_FAIL;
#endif
    }

    if (use_nvkey_sirk) {
        app_le_audio_ucst_get_sirk(true);
    }

#if 1   /* for test */
    {
        LE_AUDIO_MSGLOG_I("[APP][U] connect_cs, scan:%x->%x conn:%x", 3, g_lea_ucst_ctrl.curr_scan, g_lea_ucst_ctrl.next_scan,
                          g_lea_ucst_ctrl.curr_conn);

        LE_AUDIO_MSGLOG_I("[APP] connect_cs, sirk:%x-%x-%x-%x-%x-%x-%x-%x", 8,
                          g_le_audio_sirk[0],
                          g_le_audio_sirk[1],
                          g_le_audio_sirk[2],
                          g_le_audio_sirk[3],
                          g_le_audio_sirk[4],
                          g_le_audio_sirk[5],
                          g_le_audio_sirk[6],
                          g_le_audio_sirk[7]);
        LE_AUDIO_MSGLOG_I("[APP] connect_cs, sirk:%x-%x-%x-%x-%x-%x-%x-%x", 8,
                          g_le_audio_sirk[8],
                          g_le_audio_sirk[9],
                          g_le_audio_sirk[10],
                          g_le_audio_sirk[11],
                          g_le_audio_sirk[12],
                          g_le_audio_sirk[13],
                          g_le_audio_sirk[14],
                          g_le_audio_sirk[15]);
    }
#endif

    ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                        APP_LE_AUDIO_EVENT_START_SCAN_NEW_DEVICE, NULL, 0,
                        NULL, 0);

    g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_COORDINATED_SET_BY_SIRK;

    ble_csip_set_by_sirk(g_le_audio_sirk);
    if (BT_STATUS_SUCCESS != (ret = app_le_audio_ucst_start_scan(false))) {
        g_lea_ucst_ctrl.curr_conn = APP_LE_AUDIO_UCST_CONN_NONE;
    }

    return ret;
}

bt_status_t app_le_audio_ucst_find_device(void)
{
    if (0 != g_lea_ucst_bonded_list.num) {
        return app_le_audio_ucst_connect_bonded_device(false);
    }

    return app_le_audio_ucst_connect_coordinated_set(true);
}

void app_le_audio_ucst_notify_mic_mute(bool mic_mute)
{
    uint8_t i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        le_audio_notify_mic_mute(g_lea_ucst_link_info[i].handle, mic_mute);
    }
}

void app_le_audio_ucst_sync_lock_stream_flag(app_le_audio_ucst_link_info_t *p_info, app_le_audio_ucst_lock_stream_t lock, bool set)
{
    app_le_audio_ucst_link_info_t *p_info_tmp = NULL;
    uint8_t i = 0;

    if (NULL == p_info) {
        return;
    }

    if (p_info->group_size == 1) {
        return;
    }

    i = APP_LE_AUDIO_UCST_LINK_MAX_NUM;

    while (i > 0) {
        i--;
        p_info_tmp = &g_lea_ucst_link_info[i];
        if ((p_info->group_id == p_info_tmp->group_id) &&
            (p_info->handle != p_info_tmp->handle) &&
            (BT_HANDLE_INVALID != p_info_tmp->handle)) {
            if (set) {
                p_info_tmp->lock_stream |= lock;
            } else {
                p_info_tmp->lock_stream &= ~lock;
            }
            break;
        }
    }
}

app_le_audio_ucst_link_info_t *app_le_audio_ucst_find_connecting_link_info_by_timer_handle(TimerHandle_t timer_handle, uint8_t *link_idx)
{
    if (link_idx) {
        *link_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
    }

    if (timer_handle) {
        for (uint32_t i = 0; APP_LE_AUDIO_UCST_LINK_MAX_NUM > i ; i++) {
            if (BT_HANDLE_INVALID == g_lea_ucst_link_info[i].handle &&
                timer_handle == g_lea_ucst_link_info[i].le_connection_timer_handle) {
                if (link_idx) {
                    *link_idx = i;
                }
                return &g_lea_ucst_link_info[i];
            }
        }
    }

    return NULL;
}


app_le_audio_ucst_link_info_t *app_le_audio_ucst_get_available_link_info_for_new_le_connection(uint8_t *link_idx)
{
    if (link_idx) {
        *link_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
    }

    for (uint32_t i = 0; APP_LE_AUDIO_UCST_LINK_MAX_NUM > i ; i++) {
        if (BT_HANDLE_INVALID == g_lea_ucst_link_info[i].handle &&
            NULL == g_lea_ucst_link_info[i].le_connection_timer_handle) {
            if (link_idx) {
                *link_idx = i;
            }
            return &g_lea_ucst_link_info[i];
        }
    }

    return NULL;
}


app_le_audio_ucst_link_info_t *app_le_audio_ucst_find_connecting_link_info_by_peer_addr(const bt_addr_t *addr, uint8_t *link_idx)
{
    if (link_idx) {
        *link_idx = APP_LE_AUDIO_UCST_LINK_IDX_INVALID;
    }

    if (addr) {
        for (uint32_t i = 0; APP_LE_AUDIO_UCST_LINK_MAX_NUM > i ; i++) {
            if (BT_HANDLE_INVALID == g_lea_ucst_link_info[i].handle &&
                g_lea_ucst_link_info[i].le_connection_timer_handle &&
                (0 == memcmp(&(g_lea_ucst_link_info[i].addr), addr, sizeof(bt_addr_t)))) {
                if (link_idx) {
                    *link_idx = i;
                }
                return &g_lea_ucst_link_info[i];
            }
        }
    }

    LE_AUDIO_MSGLOG_I("[APP][U] le_connection_timer not found link_idx:%x", 1, link_idx);
    return NULL;
}


app_le_audio_ucst_ctrl_t *app_le_audio_ucst_get_ctrl(void)
{
    return &g_lea_ucst_ctrl;
}


#ifdef APP_LE_AUDIO_UCST_UPLINK_MIX_ENABLE
void app_le_audio_ucst_set_uplink_mix_status(bool enable)
{
    g_lea_ucst_ctrl.uplink_mix_enable = enable;
}


bool app_le_audio_ucst_get_uplink_mix_status(void)
{
    return g_lea_ucst_ctrl.uplink_mix_enable;
}
#endif

#ifdef AIR_SILENCE_DETECTION_ENABLE
bt_status_t app_le_audio_ucst_set_remote_device_bredr_connection_status(bool bredr_connected, bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = app_le_audio_ucst_get_link_info(handle);

    if (!p_info) {
        return BT_STATUS_FAIL;
    }

    p_info->remote_device_bredr_connected = bredr_connected;
    return BT_STATUS_SUCCESS;
}


bt_status_t app_le_audio_ucst_get_remote_device_bredr_connection_status(bool *bredr_connected, bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = app_le_audio_ucst_get_link_info(handle);

    if (!bredr_connected || !p_info) {
        return BT_STATUS_FAIL;
    }

    *bredr_connected = p_info->remote_device_bredr_connected;
    return BT_STATUS_SUCCESS;
}

bt_status_t app_le_audio_ucst_set_disconnect_cis_for_silence(bool disconnect_cis, bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = app_le_audio_ucst_get_link_info(handle);

    if (!p_info) {
        return BT_STATUS_FAIL;
    }

    p_info->disconnect_cis_for_silence = disconnect_cis;
    return BT_STATUS_SUCCESS;
}


bt_status_t app_le_audio_ucst_get_disconnect_cis_for_silence(bool *disconnect_cis, bt_handle_t handle)
{
    app_le_audio_ucst_link_info_t *p_info = app_le_audio_ucst_get_link_info(handle);

    if (!disconnect_cis || !p_info) {
        return BT_STATUS_FAIL;
    }

    *disconnect_cis = p_info->disconnect_cis_for_silence;
    return BT_STATUS_SUCCESS;
}


bool app_le_audio_silence_detection_is_remote_device_connected_bredr_device(void)
{
    /* bredr is considered to be connected if there is at least one remote device connected with any bredr device. */
    for (uint32_t i = 0; APP_LE_AUDIO_UCST_LINK_MAX_NUM > i; i++) {
        if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) {
            if (g_lea_ucst_link_info[i].remote_device_bredr_connected
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
                && app_le_audio_ucst_is_active_group_by_handle(g_lea_ucst_link_info[i].handle)
#endif
                ) {
                return TRUE;
            }
        }
    }

    return FALSE;
}


bool app_le_audio_silence_detection_disconnect_cis_for_silence(void)
{
    uint32_t i = 0;
    bool remote_bredr_connected = app_le_audio_silence_detection_is_remote_device_connected_bredr_device();
    bool disconnect_cis = TRUE;

    /* If there is any bredr device connected to the remote device, disconnect CIS always to let A2DP be allowed
        * when silence is deteced in the dongle side.
        */
    if (!remote_bredr_connected) {
        /* If there is at least one device setting it to FALSE, do not disconnect CIS.
               * (The setting may be reported latter for the late connected LEA device).
               * The devices in the same coordinated set should have the same setting.
               */
        for (i = 0; APP_LE_AUDIO_UCST_LINK_MAX_NUM > i; i++) {
            if (BT_HANDLE_INVALID != g_lea_ucst_link_info[i].handle) {
                if (!g_lea_ucst_link_info[i].disconnect_cis_for_silence) {
                    disconnect_cis = FALSE;
                    break;
                }
            }
        }
    }

    /* If there is no bredr device connecting to the remote LEA device, the decision depends on the setting
        * of the remote device.
        */
    LE_AUDIO_MSGLOG_I("[APP][SD] disconnect cis for silence:%x", 1, disconnect_cis);
    return disconnect_cis;
}

#endif

#endif  /* AIR_LE_AUDIO_ENABLE */

