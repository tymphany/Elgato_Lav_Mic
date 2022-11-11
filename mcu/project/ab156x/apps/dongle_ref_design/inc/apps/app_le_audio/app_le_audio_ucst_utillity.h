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

#ifndef __APP_LE_AUDIO_UCST_UTILLITY_H__
#define __APP_LE_AUDIO_UCST_UTILLITY_H__

#ifdef AIR_LE_AUDIO_ENABLE

#include "app_le_audio_utillity.h"

#include "bt_le_audio_def.h"
#include "bt_gap_le.h"

/**************************************************************************************************
* Define
**************************************************************************************************/
/* Uncomment to support mixing the uplinks of different peer devices. */
#define APP_LE_AUDIO_UCST_UPLINK_MIX_ENABLE

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
#define APP_LE_AUDIO_UCST_LINK_MAX_NUM      3
#else
#define APP_LE_AUDIO_UCST_LINK_MAX_NUM      2
#endif
#define APP_LE_AUDIO_UCST_LINK_IDX_INVALID  0xFF

#define APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM       4
#define APP_LE_AUDIO_UCST_BONDED_LIST_IDX_INVALID   0xFF
#define APP_LE_AUDIO_UCST_BONDED_RECORD_INVALID     0x80

#define APP_LE_AUDIO_UCST_CIS_MAX_NUM       2

/* ASE location number */
#define APP_LE_AUDIO_UCST_LOCATION_NUM_1    1
#define APP_LE_AUDIO_UCST_LOCATION_NUM_2    2

#define APP_LE_AUDIO_UCST_MAX_CCID_LIST_SIZE 2

/* set white list related */
#define APP_LE_AUDIO_UCST_SET_WHITE_LIST_NONE               0
#define APP_LE_AUDIO_UCST_SET_WHITE_LIST_ADD_ON_GOING       1
#define APP_LE_AUDIO_UCST_SET_WHITE_LIST_REMOVE_ON_GOING    2
#define APP_LE_AUDIO_UCST_SET_WHITE_LIST_COMPLETE           3
typedef uint8_t app_le_audio_ucst_set_white_list_state_t;

/* scan related */
#define APP_LE_AUDIO_UCST_SCAN_NONE                 0   /* scan disabled */
#define APP_LE_AUDIO_UCST_SCAN_ENABLING             1   /* scan enabling */
#define APP_LE_AUDIO_UCST_SCAN_ENABLED              2   /* scan enabled */
#define APP_LE_AUDIO_UCST_SCAN_DISABLING            3   /* scan disabling */
#define APP_LE_AUDIO_UCST_SCAN_CS_ENABLING          4   /* scan coordinated set enabling */
#define APP_LE_AUDIO_UCST_SCAN_CS_ENABLED           5   /* scan coordinated set enabled */
#define APP_LE_AUDIO_UCST_SCAN_CS_DISABLING         6   /* scan coordinated set disabling */
typedef uint8_t app_le_audio_ucst_scan_t;

/* create connection related */
#define APP_LE_AUDIO_UCST_CONN_NONE                     0
#define APP_LE_AUDIO_UCST_CONN_LEA_DEVICE               1   /* connect LE AUDIO device */
#define APP_LE_AUDIO_UCST_CONN_BONDED_DEVICE            2   /* connect bonded device */
#define APP_LE_AUDIO_UCST_CONN_COORDINATED_SET_BY_SIRK  3   /* connect coordinated set by SIRK. */
#define APP_LE_AUDIO_UCST_CONN_CS_AND_BONDED_DEVICE     4   /* connect coordinated set and bonded device. */
typedef uint8_t app_le_audio_ucst_conn_t;

/* group related: coordinated set */
#define APP_LE_AUDIO_UCST_GROUP_ID_0            0
#define APP_LE_AUDIO_UCST_GROUP_ID_1            1
#define APP_LE_AUDIO_UCST_GROUP_ID_2            2
#define APP_LE_AUDIO_UCST_GROUP_ID_3            3
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
#define APP_LE_AUDIO_UCST_GROUP_ID_MAX          2
#else
#define APP_LE_AUDIO_UCST_GROUP_ID_MAX          4
#endif
#define APP_LE_AUDIO_UCST_GROUP_ID_INVALID      0xFF
typedef uint8_t app_le_audio_ucst_group_id_t;

#define APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM    2

/* ASE releated */
#define APP_LE_AUDIO_UCST_ASE_IDX_0         0
#define APP_LE_AUDIO_UCST_ASE_IDX_1         1
#define APP_LE_AUDIO_UCST_ASE_IDX_2         2
#define APP_LE_AUDIO_UCST_ASE_IDX_3         3
#define APP_LE_AUDIO_UCST_ASE_IDX_4         4
#define APP_LE_AUDIO_UCST_ASE_IDX_5         5
#define APP_LE_AUDIO_UCST_ASE_MAX_NUM       6

/* unicast link status */
#define APP_LE_AUDIO_UCST_LINK_STATE_IDLE                               0
#define APP_LE_AUDIO_UCST_LINK_STATE_EXCHANGE_MTU                       0x01
#define APP_LE_AUDIO_UCST_LINK_STATE_READ_AUDIO_CONTEXTS                0x02
#define APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_PAC                      0x03
#define APP_LE_AUDIO_UCST_LINK_STATE_READ_SINK_LOCATION                 0x04
#define APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_PAC                    0x05
#define APP_LE_AUDIO_UCST_LINK_STATE_READ_SOURCE_LOCATION               0x06
#define APP_LE_AUDIO_UCST_LINK_STATE_READ_ASE                           0x07
#define APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_CODEC                   0x08
#define APP_LE_AUDIO_UCST_LINK_STATE_CONFIG_ASE_QOS                     0x09
#define APP_LE_AUDIO_UCST_LINK_STATE_ENABLE_ASE                         0x0A
#define APP_LE_AUDIO_UCST_LINK_STATE_UPDATE_ASE_METADATA                0x0B
#define APP_LE_AUDIO_UCST_LINK_STATE_CREATE_CIS                         0x0C
#define APP_LE_AUDIO_UCST_LINK_STATE_SETUP_ISO_DATA_PATH                0x0D
#define APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_START_READY           0x0E
#define APP_LE_AUDIO_UCST_LINK_STATE_STREAMING                          0x0F
#define APP_LE_AUDIO_UCST_LINK_STATE_DISABLE_ASE                        0x10
#define APP_LE_AUDIO_UCST_LINK_STATE_SET_ASE_RECV_STOP_READY            0x11
#define APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_CIS                     0x12
#define APP_LE_AUDIO_UCST_LINK_STATE_DISCONNECT_ACL                     0x13
#define APP_LE_AUDIO_UCST_LINK_STATE_MAX                                0x14
typedef uint8_t app_le_audio_ucst_link_state_t;

/* unicast stream status */
#define APP_LE_AUDIO_UCST_STREAM_STATE_IDLE                            0x00
#define APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM              0x01
#define APP_LE_AUDIO_UCST_STREAM_STATE_SET_CIG_PARAMETER               0x02
#define APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING                       0x03
#define APP_LE_AUDIO_UCST_STREAM_STATE_STOP_STREAMING                  0x04
#define APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM               0x05
#define APP_LE_AUDIO_UCST_STREAM_STATE_REMOVE_CIG                      0x06
#define APP_LE_AUDIO_UCST_STREAM_STATE_MAX                             0x07
typedef uint8_t app_le_audio_ucst_stream_state_t;

/* CIS status */
#define APP_LE_AUDIO_UCST_CIS_IDLE              0
#define APP_LE_AUDIO_UCST_CIS_CREATING          1
#define APP_LE_AUDIO_UCST_CIS_CREATED           2
#define APP_LE_AUDIO_UCST_CIS_SETUP_DATA_PATH   3
#define APP_LE_AUDIO_UCST_CIS_STREAMING         4
#define APP_LE_AUDIO_UCST_CIS_DISCONNECTING     5
typedef uint8_t app_le_audio_ucst_cis_status_t;

#define APP_LE_AUDIO_UCST_TARGET_NONE               0x00
#define APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE   0x01
#define APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE    0x02
#define APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE    0x03
#define APP_LE_AUDIO_UCST_TARGET_STOP_CALL_MODE     0x04
#define APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE     0x05
#define APP_LE_AUDIO_UCST_TARGET_STOP_SPECIAL_SILENCE_DETECTION_MODE     0x06
typedef uint8_t app_le_audio_ucst_target_t;

/* stream port */
#define APP_LE_AUDIO_UCST_STREAM_PORT_SPK_0         0
#define APP_LE_AUDIO_UCST_STREAM_PORT_SPK_1         1
#define APP_LE_AUDIO_UCST_STREAM_PORT_MIC_0         2
#define APP_LE_AUDIO_UCST_STREAM_PORT_MAX           3
typedef uint8_t app_le_audio_ucst_stream_port_t;

#define APP_LE_AUDIO_UCST_CREATE_CIS_BY_USB_PORT            0x00    /* create cis base on usb port */
#define APP_LE_AUDIO_UCST_CREATE_CIS_ALWAYS_UNIDIRECTIONAL  0x01
#define APP_LE_AUDIO_UCST_CREATE_CIS_ALWAYS_BIDIRECTIONAL   0x02
typedef uint8_t app_le_audio_ucst_create_cis_mode_t;

#define APP_LE_AUDIO_UCST_LCOK_STREAM_NONE              0x00
#define APP_LE_AUDIO_UCST_LCOK_STREAM_UNIDIRECTIONAL    0x01
#define APP_LE_AUDIO_UCST_LCOK_STREAM_ALL               0x02
#define APP_LE_AUDIO_UCST_LCOK_STREAM_EARPHONE_FOTA     0x04
typedef uint8_t app_le_audio_ucst_lock_stream_t;

#define APP_LE_AUDIO_UCST_PAUSE_STREAM_NONE             0x00
#define APP_LE_AUDIO_UCST_PAUSE_STREAM_UNIDIRECTIONAL   0x01    /* To do: only pause unidirectional stream */
#define APP_LE_AUDIO_UCST_PAUSE_STREAM_ALL              0x02    /* pause unidirectional & bidirectional stream */
#define APP_LE_AUDIO_UCST_PAUSE_STREAM_DONGLE_FOTA      0x04    /* pause all stream until dongle fota complete*/
typedef uint8_t app_le_audio_ucst_pause_stream_t;

#define APP_LE_AUDIO_UCST_TEST_MODE_ENABLE                  0x01
#define APP_LE_AUDIO_UCST_TEST_MODE_PAUSE_BEFORE_CONFIG_ASE 0x02
#define APP_LE_AUDIO_UCST_TEST_MODE_SCAN_RSI_DATA           0x04
#define APP_LE_AUDIO_UCST_TEST_MODE_CIG_PARAM               0x08

/**************************************************************************************************
* Structure
**************************************************************************************************/
typedef struct {
    bt_addr_t addr;
    uint8_t group_id;       /* group id is valid when group size != 0 */
    uint8_t group_size;     /* group size: the number of devices in the group */
    uint8_t link_idx;       /* the link index if the device is connected */
    uint8_t in_white_list;
    uint8_t deleting;
} app_le_audio_ucst_bonded_device_t;

typedef struct {
    uint8_t num;                                                                        /* the number of bonded devices */
    app_le_audio_ucst_bonded_device_t device[APP_LE_AUDIO_UCST_BONDED_LIST_MAX_NUM];    /* bonded device list */
} app_le_audio_ucst_bonded_list_t;

typedef struct {
    uint8_t curr_idx;
    app_le_audio_ucst_set_white_list_state_t state;
} app_le_audio_ucst_set_white_list_t;

typedef struct {
    //uint8_t *pac_record;                        /**< Data of PAC records. */
    uint8_t pac_record_length;                  /**< Length of PAC records. */
    uint8_t num_of_record;                      /**< Number of PAC records in this characteristic. */
    uint8_t is_supported;
} app_le_audio_pac_t;

typedef struct {
    uint8_t id;
    uint8_t curr_state;
    bt_le_audio_direction_t direction;
} app_le_audio_ase_t;

typedef struct {
    uint8_t wait_ase_event;                     /* Number of ASE events expected to receive.  */
    uint8_t wait_ase_cp_event;                  /* Number of ASE control point events expected to receive. */
} app_le_audio_wait_bap_event_t;

typedef struct {
    bt_addr_t addr;                             /* peer device address */
    bt_handle_t handle;                         /* connection handle */
    app_le_audio_ucst_link_state_t curr_state;
    app_le_audio_ucst_link_state_t next_state;
    uint8_t group_id;
    uint8_t group_size;
    uint16_t curr_interval;
    uint16_t next_interval;

    app_le_audio_pac_t sink_pac;
    uint32_t sink_location;
    uint16_t sink_available_contexts;
    uint8_t sink_ase_num;                       /* Number of sink ASEs  */
    uint8_t sink_location_num;
    uint8_t sink_audio_channel_num;

    app_le_audio_pac_t source_pac;
    uint32_t source_location;
    uint16_t source_available_contexts;
    uint8_t source_ase_num;                     /* Number of source ASEs  */
    uint8_t source_location_num;
    uint8_t source_audio_channel_num;
    uint8_t source_ase_idx;

    app_le_audio_ase_t ase[APP_LE_AUDIO_UCST_ASE_MAX_NUM];
    app_le_audio_wait_bap_event_t wait_event;
    bool release;
    TimerHandle_t le_connection_timer_handle;
    uint8_t read_ase_num;
    uint8_t ase_releasing;
    uint8_t cis_num;

    uint8_t bond_idx;
    uint8_t add_white_list;
    app_le_audio_ucst_lock_stream_t lock_stream;
#ifdef AIR_SILENCE_DETECTION_ENABLE
    bool remote_device_bredr_connected; /* If the remote device connects to any BR/EDR device or not. */
    bool disconnect_cis_for_silence; /* If CIS need be disconnected when silence is detected. */
#endif
} app_le_audio_ucst_link_info_t;

typedef struct {
    bt_handle_t cis_handle;                     /* CIS connection handle */
    bt_handle_t acl_handle;                     /* ACL connection handle */
    app_le_audio_ucst_cis_status_t cis_status;
} app_le_audio_ucst_cis_info_t;

typedef struct {
    app_le_audio_ucst_scan_t curr_scan;
    app_le_audio_ucst_scan_t next_scan;
    app_le_audio_ucst_conn_t curr_conn;
    app_le_audio_ucst_target_t curr_target;
    app_le_audio_ucst_target_t next_target;
    app_le_audio_ucst_stream_state_t curr_stream_state;
    app_le_audio_ucst_stream_state_t next_stream_state;
    app_le_audio_qos_params_t qos_params_spk_0;
    app_le_audio_qos_params_t qos_params_spk_1;
    app_le_audio_qos_params_t qos_params_mic_0;
    app_le_audio_ucst_pause_stream_t pause_stream;
    app_le_audio_ucst_create_cis_mode_t create_cis_mode;
    uint8_t is_cig_created;
    uint8_t cig_id;
    uint8_t cis_num;
    uint8_t release;
    uint8_t set_endpoint_tx_ready;
    app_le_audio_ucst_group_id_t curr_group;        /* the group id of the current active group */
    app_le_audio_ucst_group_id_t next_group;        /* the group id of the next active group */
    app_le_audio_ucst_group_id_t latest_group;      /* the group id of the latest connected group */
    //uint8_t is_creating_connection;
#ifdef APP_LE_AUDIO_UCST_UPLINK_MIX_ENABLE
    bool uplink_mix_enable;
#endif
} app_le_audio_ucst_ctrl_t;

typedef struct {
    uint8_t bn;
    uint8_t nse;
    uint8_t ft;
    uint16_t iso_interval;                      /* ISO interval = SDU interval*bn, bn>=1 */
} app_le_audio_ucst_cig_params_test_t;

typedef struct {
    bt_key_t sirk;
    uint8_t size;
    uint8_t bond_num;
    uint8_t link_idx[APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM];
} app_le_audio_ucst_group_info_t;

typedef struct {
    uint8_t num;
    bt_addr_t addr[APP_LE_AUDIO_UCST_GROUP_LINK_MAX_NUM];
} app_le_audio_ucst_active_group_addr_list_t;

/**************************************************************************************************
* Public function
**************************************************************************************************/
void app_le_audio_ucst_print_lea_adv_addr(bt_addr_t *addr);

bool app_le_audio_ucst_is_lea_adv(bt_gap_le_ext_advertising_report_ind_t *adv_report);

bool app_le_audio_ucst_is_connected_device(const bt_addr_t *addr);

bool app_le_audio_ucst_is_bonded_device(const bt_addr_t *addr);

bool app_le_audio_ucst_is_bonded_device_all_connected(void);

bool app_le_audio_ucst_is_active_group(uint8_t group_id);

bool app_le_audio_ucst_is_active_group_by_handle(bt_handle_t handle);

bool app_le_audio_ucst_is_group_device_all_connected(uint8_t group_id);

bool app_le_audio_ucst_is_group_full(void);

bt_status_t app_le_audio_ucst_stop_scan(void);      /* stop scan LE AUDIO devices */

bt_status_t app_le_audio_ucst_stop_scan_cs(void);   /* stop scan coordinated set devices */

bt_status_t app_le_audio_ucst_stop_scan_all(void);  /* stop current scan (scan LE AUDIO devices or scan coordinated set devices)*/

bt_status_t app_le_audio_ucst_start_scan(bool use_white_list);     /* start scan LE AUDIO devices */

bt_status_t app_le_audio_ucst_start_scan_cs(void);  /* start scan coordinated set devices */

void app_le_audio_ucst_add_link_to_group(uint8_t group_id, uint8_t link_idx);

void app_le_audio_ucst_remove_link_from_group(uint8_t group_id, uint8_t link_idx);

void app_le_audio_ucst_set_qos_params(uint8_t sampling_rate, uint8_t sel_setting, bool high_reliability, app_le_audio_ucst_stream_port_t port);

void app_le_audio_ucst_set_create_cis_mode(app_le_audio_ucst_create_cis_mode_t create_cis_mode);

uint8_t app_le_audio_ucst_set_new_group_info(bt_key_t *sirk, uint8_t link_idx);

void app_le_audio_ucst_set_sirk(bt_key_t *sirk, bool update_nvkey);

bt_key_t *app_le_audio_ucst_get_sirk(bool from_nvkey);

uint8_t app_le_audio_ucst_get_link_num(void);

uint8_t app_le_audio_ucst_get_link_idx(bt_handle_t handle);

uint8_t app_le_audio_ucst_get_link_idx_by_cis(bt_handle_t cis_handle, uint8_t *cis_idx);

uint8_t app_le_audio_ucst_get_link_idx_by_addr(bt_addr_t *addr);

app_le_audio_ucst_link_info_t *app_le_audio_ucst_get_link_info(bt_handle_t handle);

app_le_audio_ucst_link_info_t *app_le_audio_ucst_get_link_info_by_idx(uint8_t link_idx);

app_le_audio_ucst_link_info_t *app_le_audio_ucst_get_link_info_by_addr(bt_addr_t *addr);

app_le_audio_ucst_group_info_t *app_le_audio_ucst_get_group_info(uint8_t group_id);

uint8_t app_le_audio_ucst_get_active_group_address(app_le_audio_ucst_active_group_addr_list_t *addr_list);

uint8_t app_le_audio_ucst_get_group_id_by_sirk(bt_key_t *sirk);

uint8_t app_le_audio_ucst_get_group_link_num(uint8_t group_id);

uint8_t app_le_audio_ucst_get_bonded_list_idx(bt_addr_t *addr);

uint8_t app_le_audio_ucst_get_empty_bonded_list_idx(void);

uint8_t app_le_audio_ucst_get_cis_num(void);

app_le_audio_ucst_conn_t app_le_audio_ucst_get_curr_conn_type(void);

app_le_audio_ucst_target_t app_le_audio_ucst_get_curr_target(void);

app_le_audio_ucst_pause_stream_t app_le_audio_ucst_get_pause_stream_flag(void);

app_le_audio_ucst_create_cis_mode_t app_le_audio_ucst_get_create_cis_mode(void);

uint32_t app_le_audio_ucst_get_sampling_rate(bool mic);

uint16_t app_le_audio_ucst_get_sdu_size(bool mic);

uint32_t app_le_audio_ucst_get_sdu_interval(bool mic);

float app_le_audio_ucst_get_bitrate(bool mic);

uint32_t app_le_audio_ucst_get_location(uint8_t idx, uint32_t location);

uint8_t app_le_audio_ucst_get_location_count(uint32_t location);

uint32_t app_le_audio_ucst_get_available_channel(void);

uint8_t app_le_audio_ucst_get_active_group(void);

void app_le_audio_ucst_reset_cig_parameter_test(void);

void app_le_audio_ucst_reset_all_cis_info(void);

void app_le_audio_ucst_reset_link_info(uint8_t link_idx);

void app_le_audio_ucst_reset_all_link_info(void);

void app_le_audio_ucst_reset_group_info(uint8_t group_id);

void app_le_audio_ucst_reset_all_group_info(void);

void app_le_audio_ucst_reset_bonded_list(uint8_t bond_idx);

void app_le_audio_ucst_reset_all_bonded_list(void);

void app_le_audio_ucst_reset_active_group(void);

void app_le_audio_ucst_reset_all_bonded_info(void);

void app_le_audio_ucst_refresh_bonded_list(uint8_t empty_idx);

void app_le_audio_ucst_refresh_group_info(void);

void app_le_audio_ucst_read_bonded_list_from_nvkey(void);

void app_le_audio_ucst_read_active_group_from_nvkey(void);

void app_le_audio_ucst_write_bonded_list_to_nvkey(void);

void app_le_audio_ucst_write_active_group_to_nvkey(void);

bt_status_t app_le_audio_ucst_connect_bonded_device(bool search_cs);        /* scan and connect bonded devices (also cs device) */

bt_status_t app_le_audio_ucst_connect_coordinated_set(bool use_nvkey_sirk); /* scan and connect coordinated set devices */

bt_status_t app_le_audio_ucst_find_device(void);                /* scan and connect devices (bonded devices or coordinated set devices)*/

void app_le_audio_ucst_sync_lock_stream_flag(app_le_audio_ucst_link_info_t *p_info, app_le_audio_ucst_lock_stream_t lock, bool set);

app_le_audio_ucst_link_info_t *app_le_audio_ucst_find_connecting_link_info_by_timer_handle(TimerHandle_t timer_handle, uint8_t *link_idx);

app_le_audio_ucst_link_info_t *app_le_audio_ucst_get_available_link_info_for_new_le_connection(uint8_t *link_idx);

app_le_audio_ucst_link_info_t *app_le_audio_ucst_find_connecting_link_info_by_peer_addr(const bt_addr_t *addr, uint8_t *link_idx);

void app_le_audio_ucst_set_cig_parameter_test(uint8_t bn, uint8_t nse, uint8_t ft, uint16_t iso_interval);

void app_le_audio_ucst_set_ccid_list(uint8_t ccid_list_size, uint8_t *ccid_list);

app_le_audio_ucst_ctrl_t *app_le_audio_ucst_get_ctrl(void);

void app_le_audio_ucst_set_csip_sirk(void);

#ifdef APP_LE_AUDIO_UCST_UPLINK_MIX_ENABLE
void app_le_audio_ucst_set_uplink_mix_status(bool enable);

bool app_le_audio_ucst_get_uplink_mix_status(void);
#endif

#ifdef AIR_SILENCE_DETECTION_ENABLE
bt_status_t app_le_audio_ucst_set_remote_device_bredr_connection_status(bool bredr_connected, bt_handle_t handle);

bt_status_t app_le_audio_ucst_get_remote_device_bredr_connection_status(bool *bredr_connected, bt_handle_t handle);

bt_status_t app_le_audio_ucst_set_disconnect_cis_for_silence(bool disconnect_cis, bt_handle_t handle);

bt_status_t app_le_audio_ucst_get_disconnect_cis_for_silence(bool *disconnect_cis, bt_handle_t handle);

bool app_le_audio_silence_detection_disconnect_cis_for_silence(void);
#endif

#endif /* AIR_LE_AUDIO_ENABLE */
#endif /* __APP_LE_AUDIO_UCST_UTILLITY_H__ */

