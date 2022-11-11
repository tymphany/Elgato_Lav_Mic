/* Copyright Statement:
 *
 * (C) 2022  Airoha Technology Corp. All rights reserved.
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
#include "bt_system.h"
#include "bt_type.h"
#include "bt_hci.h"
#include "apps_events_event_group.h"
#include "bt_gap.h"
#include "bt_gap_le.h"
#include "bt_connection_manager.h"
#include "bt_callback_manager.h"
#include "bt_device_manager_le.h"
#include "bt_device_manager.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "bt_hci.h"
#include "app_dongle_common_idle_activity.h"
#include "app_dongle_connection_common.h"
#include "apps_debug.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "assert.h"
#include "bt_gap_le_service.h"

#define APP_DONGLE_CM_LOG_TAG     "[app_dongle_cm] "

#define BT_GATT_UUID16_ASCS_SERVICE             (0x184E)    /**< Audio Stream Control Service UUID. */
#define APP_DONGLE_CM_NVKEY_CSIS_DATA_LEN    18      /* NVID_BT_LEA_CSIS_DATA: | SIRK (16 bytes) | reserved (2 byte) | */

#define APP_DONGLE_CM_STATE_DISCONNECTED                  0x00
#define APP_DONGLE_CM_STATE_START_SCAN_INQUIRY            0x01
#define APP_DONGLE_CM_STATE_START_CONNECTION              0x02
#define APP_DONGLE_CM_STATE_STOP_CONNECTION               0x03
#define APP_DONGLE_CM_STATE_CONNECTED                     0x04
typedef uint8_t app_dongle_cm_state_t;

#define APP_DONGLE_CM_SCAN_STATE_DISABLED                 (1 << 0x00)
#define APP_DONGLE_CM_SCAN_STATE_ENABLING                 (1 << 0x01)
#define APP_DONGLE_CM_SCAN_STATE_ENABLED                  (1 << 0x02)
#define APP_DONGLE_CM_SCAN_STATE_DISABLING                (1 << 0x03)
typedef uint8_t app_dongle_cm_scan_state_mask_t;

#define APP_DONGLE_CM_NEXT_ACTION_NONE                    0x00
#define APP_DONGLE_CM_NEXT_ACTION_START_SCAN              0x01
#define APP_DONGLE_CM_NEXT_ACTION_STOP_SCAN               0x02
typedef uint8_t app_dongle_cm_next_action_t;

#define APP_DONGLE_CM_CONNECTION_USING_UNKNOWN                      0x00
#define APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST                 0x01
#define APP_DONGLE_CM_CONNECTION_USING_SIRK                         0x02
#define APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN                    0x03
#define APP_DONGLE_CM_CONNECTION_WAITE_FOR_PAIRING_KEY              0x04
#define APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN_FOR_USER           0x05

typedef uint8_t app_dongle_cm_connection_action_t;

#define APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN    0x01
typedef uint8_t app_dongle_cm_timer_id_t;

#define APP_DONGLE_CM_SET_WHITE_LIST_STATE_NONE             0x00
#define APP_DONGLE_CM_SET_WHITE_LIST_STATE_ADD_ON_GOING     0x01
#define APP_DONGLE_CM_SET_WHITE_LIST_STATE_REMOVE_ON_GOING  0x02
#define APP_DONGLE_CM_SET_WHITE_LIST_STATE_CLEAR_ON_GOING   0x03
#define APP_DONGLE_CM_SET_WHITE_LIST_STATE_COMPLETE         0x04
typedef uint8_t app_dongle_cm_set_white_list_state_t;

typedef struct {
    uint8_t num;
    app_dongle_cm_set_white_list_state_t set_wl_state;
} app_dongle_cm_wl_info_t;

typedef struct {
     bt_status_t (*source_started_handler)(app_dongle_cm_source_t type, bt_status_t status, void *data);
     bt_status_t (*source_end_handler)(app_dongle_cm_source_t type , bt_status_t status, void *data);
} app_dongle_cm_event_handler_t;

typedef struct {
    app_dongle_cm_source_t      source_type;
    bool                        source_registed;
    uint8_t                     connected_link_num;
    bt_addr_t                   connect_addr;
    app_dongle_cm_state_t       conn_state;
    app_dongle_cm_handle_t      cm_handle;
    app_dongle_cm_event_handler_t cm_event_handler;
} app_dongle_cm_source_info_t;

typedef struct {
    uint8_t num;
    bt_bd_addr_t *addr;
} app_dongle_cm_reconnect_list_info_t;

typedef struct {
    app_dongle_cm_link_mode_t link_mode;
    bt_key_t sirk;
    struct {
        bool is_switch_dongle_mode;
        app_dongle_cm_dongle_mode_t curr_mode;
        app_dongle_cm_dongle_mode_t next_mode;
    } dongle_mode;
    struct {
        bool wl_enable;
        bool use_sirk;
        app_dongle_cm_scan_state_mask_t scan_state_mask;
        app_dongle_cm_next_action_t next_action;
    } le_scan;
    struct {
        uint8_t scan_state;
    } edr_scan;
    app_dongle_cm_wl_info_t wl_info;
    app_dongle_cm_connection_action_t connect_style_le;
    app_dongle_cm_connection_action_t connect_style_edr;

    app_dongle_cm_reconnect_list_info_t br_edr_list;
    app_dongle_cm_reconnect_list_info_t le_list;
    app_dongle_cm_source_info_t source_info[APP_DONGLE_CM_SOURCE_MAX];
} app_dongle_cm_ctrl_contex_t;

typedef struct {
    app_dongle_cm_source_t      source_type;
    app_dongle_cm_event_handler_t cm_event_handler;
} app_dongle_cm_event_handler_table_t;

typedef void (*app_dongle_cm_timer_callback_t)(uint8_t timer_id, void *data);

typedef struct {
    uint8_t timer_id;
    TimerHandle_t timer_handle;
    app_dongle_cm_timer_callback_t callback;
    void *data;
} app_dongle_cm_timer_info_t;

static app_dongle_le_race_event_callback_t g_dongle_le_cm_race_callback = NULL;

extern bt_gap_le_srv_link_t bt_device_manager_le_get_link_type_by_addr(bt_bd_addr_t *remote_addr);
extern void app_dongle_cm_notify_switch_dongle_mode_result(bt_status_t status, app_dongle_cm_dongle_mode_t dongle_mode);
static app_dongle_cm_ctrl_contex_t *app_dongle_cm_get_ctrl_context(void);
static app_dongle_cm_source_info_t *app_dongle_cm_get_source_info(app_dongle_cm_source_t type);
;
static bt_status_t app_dongle_cm_source_started_ull_v1_handler(app_dongle_cm_source_t type, bt_status_t status, void *data);
static bt_status_t app_dongle_cm_source_started_ull_v2_handler(app_dongle_cm_source_t type, bt_status_t status, void *data);
static bt_status_t app_dongle_cm_source_started_bta_handler(app_dongle_cm_source_t type, bt_status_t status, void *data);
static bt_status_t app_dongle_cm_source_started_lea_handler(app_dongle_cm_source_t type, bt_status_t status, void *data);
static bt_status_t app_dongle_cm_source_end_ull_v1_handler(app_dongle_cm_source_t type, bt_status_t status, void *data);
static bt_status_t app_dongle_cm_source_end_ull_v2_handler(app_dongle_cm_source_t type, bt_status_t status, void *data);
static bt_status_t app_dongle_cm_source_end_bta_handler(app_dongle_cm_source_t type, bt_status_t status, void *data);
static bt_status_t app_dongle_cm_source_end_lea_handler(app_dongle_cm_source_t type, bt_status_t status, void *data);

static void app_dongle_cm_bt_power_on_cnf_handler(void);
static void app_dongle_cm_bt_power_off_cnf_handler(void);
static void app_dongle_cm_sirk_scan_timeout_handler(uint8_t timer_id, void *data);

static void app_dongle_cm_le_scan_cnf_handler(bt_status_t status);
static void app_dongle_cm_set_white_list_cnf_handler(bt_status_t status);
static void app_dongle_cm_adv_report_cnf_handler(bt_status_t status, bt_gap_le_ext_advertising_report_ind_t *data);
/*
static bt_status_t app_dongle_cm_rm_white_list(bt_addr_t *addr);
*/
static bt_status_t app_dongle_cm_clear_white_list(void);

static bt_status_t app_dongle_cm_add_white_list(bt_addr_t *addr);
static void app_dongle_cm_clear_recon_list(bool is_le);
static uint8_t app_dongle_cm_read_recon_list_from_bonded_info(uint8_t num);
static uint8_t app_dongle_cm_read_recon_list_from_le_bonded_info(uint8_t num, bool read_all);
static bt_status_t app_dongle_cm_set_white_list(void);
static app_dongle_cm_source_t app_dongle_cm_check_uuid(bt_gap_le_ext_advertising_report_ind_t *adv_report);
static void app_dongle_cm_clear_scan_state_mask(app_dongle_cm_scan_state_mask_t state);
static app_dongle_cm_scan_state_mask_t app_dongle_cm_get_scan_state_mask(void);
static bt_status_t app_dongle_cm_le_stop_scan_device_internal(void);
static bt_status_t app_dongle_cm_le_start_scan_device_internal(bool wl_enable);

static void app_dongle_cm_read_sirk(void);
static void app_dongle_cm_set_scan_state_mask(app_dongle_cm_scan_state_mask_t state);
static void app_dongle_cm_clear_scan_state_mask(app_dongle_cm_scan_state_mask_t state);
static void app_dongle_cm_set_source_conn_state(app_dongle_cm_source_info_t *source_info, app_dongle_cm_state_t state);
bt_status_t app_dongle_cm_switch_to_next_dongle_mode(app_dongle_cm_dongle_mode_t next_mode);
bt_status_t app_dongle_cm_set_ull_v2_feature_bit(bool enable);

static bt_status_t app_dongle_cm_host_event_callback(bt_msg_type_t msg, bt_status_t ret, void *buff);

static app_dongle_cm_ctrl_contex_t app_dongle_cm_ctx;
static uint8_t const g_app_dongle_ull_v2_uuid128_default[16] = {
    0x45, 0x4C, 0x42, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
    0x41, 0x07, 0xAB, 0x2D, 0x4D, 0x49, 0x52, 0x50
};

#define APP_DONGLE_CM_TIMER_MAX     0x03
app_dongle_cm_timer_info_t g_dongle_cm_timer_info[APP_DONGLE_CM_TIMER_MAX];


static const app_dongle_cm_event_handler_table_t cm_handler_table[APP_DONGLE_CM_SOURCE_MAX] = {
    {APP_DONGLE_CM_SOURCE_ULL_V1, {app_dongle_cm_source_started_ull_v1_handler, app_dongle_cm_source_end_ull_v1_handler}},
    {APP_DONGLE_CM_SOURCE_ULL_V2, {app_dongle_cm_source_started_ull_v2_handler, app_dongle_cm_source_end_ull_v2_handler}},
    {APP_DONGLE_CM_SOURCE_LEA, {app_dongle_cm_source_started_lea_handler, app_dongle_cm_source_end_lea_handler}},
    {APP_DONGLE_CM_SOURCE_BTA, {app_dongle_cm_source_started_bta_handler, app_dongle_cm_source_end_bta_handler}},
};

static void *app_dongle_cm_memcpy(void *dest, const void *src, uint32_t size)
{
    if ((NULL == dest) || (NULL == src)) {
        return NULL;
    }
    return memcpy(dest, src, size);
}

static void *app_dongle_cm_memory_alloc(uint16_t size)
{
    void *memory = (void *)pvPortMalloc(size);
    if (NULL != memory) {
        memset(memory, 0, size);
    }
    return memory;
}

static void app_dongle_cm_memory_free(void *point)
{
    if (point) {
        vPortFree(point);
    }
}

static void *app_dongle_cm_memset(void *buf, uint8_t value, uint32_t size)
{
    return memset(buf, value, size);
}

static int32_t app_dongle_cm_memcmp(const void *buf1, const void *buf2, uint32_t size)
{
    return memcmp(buf1, buf2, size);
}

app_dongle_cm_timer_info_t *app_dongle_cm_get_avliable_timer_info(uint8_t timer_id)
{
    uint8_t i = 0;
    for (i = 0; APP_DONGLE_CM_TIMER_MAX > i; i++) {
        if ((timer_id == g_dongle_cm_timer_info[i].timer_id) && (NULL != g_dongle_cm_timer_info[i].timer_handle)) {
             return NULL;
        }
    }
    for (i = 0; APP_DONGLE_CM_TIMER_MAX > i; i++) {
        if (NULL == g_dongle_cm_timer_info[i].timer_handle) {
            g_dongle_cm_timer_info[i].timer_id = timer_id;
            return &g_dongle_cm_timer_info[i];
        }
    }

    return NULL;
}

bt_status_t app_dongle_cm_set_ull_v2_feature_bit(bool enable)
{
   uint8_t mode = enable;
   return bt_hci_send_vendor_cmd(0xFE0C, (uint8_t *)&mode, sizeof(uint8_t));

}

app_dongle_cm_timer_info_t *app_dongle_cm_get_timer_info_by_timer_id(uint8_t timer_id)
{
    uint32_t i = 0;

    for (i = 0; APP_DONGLE_CM_TIMER_MAX > i; i++) {
        if (g_dongle_cm_timer_info[i].timer_id == timer_id && g_dongle_cm_timer_info[i].timer_handle != NULL) {
            return &g_dongle_cm_timer_info[i];
        }
    }

    return NULL;
}

void app_dongle_cm_reset_timer_info(app_dongle_cm_timer_info_t *timer)
{
    if (timer) {
        app_dongle_cm_memset(timer, 0, sizeof(app_dongle_cm_timer_info_t));
    }
}

void app_dongle_cm_timer_callback(TimerHandle_t timer_handle)
{
    uint8_t i = 0;
    if (timer_handle) {
        for (i = 0; i < APP_DONGLE_CM_TIMER_MAX; i++) {
            if (g_dongle_cm_timer_info[i].timer_handle == timer_handle) {
                g_dongle_cm_timer_info[i].callback(g_dongle_cm_timer_info[i].timer_id, g_dongle_cm_timer_info[i].data);
                app_dongle_cm_reset_timer_info(&g_dongle_cm_timer_info[i]);
                break;
            }
        }
    }
}

void app_dongle_cm_stop_timer(uint8_t timer_id)
{
    app_dongle_cm_timer_info_t *timer_info = app_dongle_cm_get_timer_info_by_timer_id(timer_id);

    if (timer_info) {
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"stop timer. timer_id:%x", 1, timer_id);
        xTimerStop(timer_info->timer_handle, 0);
        app_dongle_cm_reset_timer_info(timer_info);
    }
}

bt_status_t app_dongle_cm_start_timer(uint8_t timer_id, uint32_t timer_period, app_dongle_cm_timer_callback_t callback, void *data)
{
    app_dongle_cm_timer_info_t *timer_info = NULL;
    if (!timer_period || !callback) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"start timer fail. timer_id:%x period:%d", 2, timer_id, timer_period);
        return BT_STATUS_FAIL;
    }
    timer_info = app_dongle_cm_get_avliable_timer_info(timer_id);
    if (!timer_info) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"start timer fail. timer_id:%x period:%d", 2, timer_id, timer_period);
        return BT_STATUS_FAIL;
    }
    timer_info->timer_handle = xTimerCreate("app_dongle_com_timer",
                                            (timer_period / portTICK_PERIOD_MS),
                                            pdFALSE,
                                            NULL,
                                            app_dongle_cm_timer_callback);

    if (timer_info->timer_handle) {
        timer_info->callback = callback;
        timer_info->data = data;
        xTimerStart(timer_info->timer_handle, 0);
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"start timer. timer_id:%x period:%d", 2, timer_id, timer_period);
        return BT_STATUS_SUCCESS;
    }
    APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"start timer fail. timer_id:%x period:%d", 2, timer_id, timer_period);
    return BT_STATUS_FAIL;
}


bt_status_t app_dongle_cm_init(void)
{
    bt_status_t status;
    status = bt_callback_manager_register_callback(bt_callback_type_app_event,
                                         (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_SYSTEM),
                                         (void *)app_dongle_cm_host_event_callback);

    app_dongle_cm_link_mode_t link_mode = app_dongle_cm_ctx.link_mode;
    app_dongle_cm_memset(&app_dongle_cm_ctx, 0, sizeof(app_dongle_cm_ctrl_contex_t));
    //app_dongle_cm_ctx.link_mode = APP_DONGLE_CM_LINK_MODE_ULL_V2 | APP_DONGLE_CM_LINK_MODE_LEA;
    //app_dongle_cm_ctx.link_mode = APP_DONGLE_CM_LINK_MODE_ULL_V1 | APP_DONGLE_CM_LINK_MODE_ULL_V2;
    app_dongle_cm_ctx.link_mode = link_mode;

    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Init Done! link mode: %x", 1, app_dongle_cm_ctx.link_mode);
    return status;
}

static app_dongle_cm_ctrl_contex_t *app_dongle_cm_get_ctrl_context(void)
{
    return &app_dongle_cm_ctx;
}

static app_dongle_cm_source_info_t *app_dongle_cm_get_source_info(app_dongle_cm_source_t type)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    if (type > APP_DONGLE_CM_SOURCE_MAX) {
        return NULL;
    }
    return &ctrl_ctx->source_info[type];
}

bt_status_t app_dongle_cm_register_handle(app_dongle_cm_source_t type, const app_dongle_cm_handle_t *cm_handle)
{
    app_dongle_cm_source_info_t *source_info = NULL;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_register_handle, type: %x, cm_handle:%x", 2, type, cm_handle);
    if (cm_handle) {
        if (type >= APP_DONGLE_CM_SOURCE_MAX) {
            return BT_STATUS_FAIL;
        }
        source_info = app_dongle_cm_get_source_info(type);
        if (!source_info) {
            return BT_STATUS_FAIL;
        }
        if (source_info->source_registed) {
            return BT_STATUS_FAIL;
        }
        source_info->source_registed = true;
        source_info->source_type = type;
        source_info->cm_event_handler.source_started_handler = cm_handler_table[type].cm_event_handler.source_started_handler;
        source_info->cm_event_handler.source_end_handler = cm_handler_table[type].cm_event_handler.source_end_handler;
        app_dongle_cm_memcpy(&source_info->cm_handle, cm_handle, sizeof(app_dongle_cm_handle_t));
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

bt_status_t app_dongle_cm_deregister_handle(app_dongle_cm_source_t type)
{
    app_dongle_cm_source_info_t *source_info = NULL;
    if (type > APP_DONGLE_CM_SOURCE_MAX) {
        return BT_STATUS_FAIL;
    }
    source_info = app_dongle_cm_get_source_info(type);
    if (!source_info->source_registed) {
        return BT_STATUS_SUCCESS;
    }
    app_dongle_cm_memset(source_info, 0, sizeof(app_dongle_cm_source_info_t));
    source_info->source_type = APP_DONGLE_CM_SOURCE_INVALID;
    return BT_STATUS_SUCCESS;
}

void app_dongle_cm_set_link_mode(app_dongle_cm_link_mode_t mode)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    ctrl_ctx->link_mode = mode;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Set dongle link mode: 0x%x", 1, mode);
}

bt_status_t app_dongle_cm_notify_event (app_dongle_cm_source_t type , app_dongle_cm_event_t event, bt_status_t status, void *data)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    app_dongle_cm_source_info_t *source_info = NULL;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_notify_event, type: 0x%x, event: %d, status: %d", 3, type, event, status);
    if (type > APP_DONGLE_CM_SOURCE_MAX) {
        return BT_STATUS_FAIL;
    }
    source_info = app_dongle_cm_get_source_info(type);
    switch (event) {
        case APP_DONGLE_CM_EVENT_SOURCE_STARTED: {
            if (source_info->cm_event_handler.source_started_handler) {
                ret = source_info->cm_event_handler.source_started_handler(type, status, data);
            }
        } break;

        case APP_DONGLE_CM_EVENT_SOURCE_END: {
            if (source_info->cm_event_handler.source_end_handler) {
                ret = source_info->cm_event_handler.source_end_handler(type, status, data);
            }
        } break;

        default:
            break;
    }
    return ret;
}

static bt_status_t app_dongle_cm_host_event_callback(bt_msg_type_t msg, bt_status_t ret, void *buff)
{
    switch (msg) {
        case BT_POWER_ON_CNF: {
            app_dongle_cm_bt_power_on_cnf_handler();
            break;
        }
        case BT_POWER_OFF_CNF: {
            app_dongle_cm_bt_power_off_cnf_handler();
            break;
        }

        case BT_GAP_LE_SET_EXTENDED_SCAN_CNF: {
            app_dongle_cm_le_scan_cnf_handler(ret);
            break;
        }

        case BT_GAP_LE_EXT_ADVERTISING_REPORT_IND: {
            app_dongle_cm_adv_report_cnf_handler(ret, (bt_gap_le_ext_advertising_report_ind_t*)buff);

            break;
        }

        case BT_GAP_LE_SET_WHITE_LIST_CNF: {
            app_dongle_cm_set_white_list_cnf_handler(ret);
            break;
        }

        case BT_GAP_INQUIRY_IND: {

            break;
        }

        case BT_GAP_INQUIRY_CNF: {
            break;
        }

        case BT_GAP_INQUIRY_COMPLETE_IND: {
            break;
        }

        case BT_GAP_CANCEL_INQUIRY_CNF: {
            break;
        }

        default :
            break;

    }
    return BT_STATUS_SUCCESS;
}
/*
bt_status_t app_dongle_cm_rm_white_list(bt_addr_t *addr)
{
    bt_addr_t device;
    bt_status_t status;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    if (ctrl_ctx->le_scan.scan_state_mask & APP_DONGLE_CM_SCAN_STATE_DISABLED) {
        ctrl_ctx->wl_info.set_wl_state = APP_DONGLE_CM_SET_WHITE_LIST_STATE_REMOVE_ON_GOING;
        app_dongle_cm_memcpy(&device, addr, sizeof(bt_addr_t));
        status = bt_gap_le_set_white_list(BT_GAP_LE_REMOVE_FROM_WHITE_LIST, &device);
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_rm_white_list, status:%x", 1, status);
        return status;
    }
    return BT_STATUS_FAIL;
}
*/
static bt_status_t app_dongle_cm_clear_white_list(void)
{
    bt_status_t status;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    if (ctrl_ctx->le_scan.scan_state_mask & APP_DONGLE_CM_SCAN_STATE_DISABLED) {
        ctrl_ctx->wl_info.set_wl_state = APP_DONGLE_CM_SET_WHITE_LIST_STATE_CLEAR_ON_GOING;
        status = bt_gap_le_set_white_list(BT_GAP_LE_CLEAR_WHITE_LIST, NULL);
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_clear_white_list, status:%x", 1, status);
        return status;
    }
    return BT_STATUS_FAIL;
}

static bt_status_t app_dongle_cm_add_white_list(bt_addr_t *addr)
{
    bt_status_t status;
    status = bt_gap_le_set_white_list(BT_GAP_LE_ADD_TO_WHITE_LIST, addr);
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_add_white_list, status:%x", 1, status);
    return status;
}

static void app_dongle_cm_clear_recon_list(bool is_le)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    if (is_le) {
        if (NULL != ctrl_ctx->le_list.addr &&  ctrl_ctx->le_list.num != 0) {
            app_dongle_cm_memory_free(ctrl_ctx->le_list.addr);
            ctrl_ctx->le_list.addr = NULL;
        }
        app_dongle_cm_memset(&ctrl_ctx->le_list, 0, sizeof(app_dongle_cm_reconnect_list_info_t));
    } else {
        if (NULL != ctrl_ctx->br_edr_list.addr && ctrl_ctx->br_edr_list.num != 0) {
            app_dongle_cm_memory_free(ctrl_ctx->br_edr_list.addr);
            ctrl_ctx->br_edr_list.addr = NULL;
        }
        app_dongle_cm_memset(&ctrl_ctx->br_edr_list, 0, sizeof(app_dongle_cm_reconnect_list_info_t));
    }
}

static uint8_t app_dongle_cm_read_recon_list_from_bonded_info(uint8_t num)
{
    uint8_t i = 0;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_clear_recon_list(false);
    ctrl_ctx->br_edr_list.num = num;
    ctrl_ctx->br_edr_list.addr = app_dongle_cm_memory_alloc(ctrl_ctx->br_edr_list.num * sizeof(bt_bd_addr_t));
    if (NULL == ctrl_ctx->br_edr_list.addr) {
        assert(0);
        app_dongle_cm_memset(&ctrl_ctx->br_edr_list, 0, sizeof(app_dongle_cm_reconnect_list_info_t));
        return 0;
    }
    bt_bd_addr_t *temp_addr = NULL;
    for (i = 1; i <= num; i ++) {
        temp_addr = bt_device_manager_remote_get_dev_by_seq_num(i);
        if (!temp_addr) {
            ctrl_ctx->br_edr_list.num = i - 1;
            if (1 == i) {
                app_dongle_cm_memory_free(ctrl_ctx->br_edr_list.addr);
                ctrl_ctx->br_edr_list.addr = NULL;
            }
           break;
        }
        app_dongle_cm_memcpy(&ctrl_ctx->br_edr_list.addr[i-1], temp_addr, sizeof(bt_bd_addr_t));
    }
    uint8_t *tempaddr = NULL;
    i = ctrl_ctx->br_edr_list.num;
    while (i > 0) {
        i--;
        tempaddr = (uint8_t *)&ctrl_ctx->br_edr_list.addr[i];
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"bonded_list[%x], addr:%02x:%02x:%02x:%02x:%02x:%02x", 7, i, tempaddr[5], tempaddr[4],
                                                                                            tempaddr[3], tempaddr[2],
                                                                                            tempaddr[1], tempaddr[0]);
    }
    return ctrl_ctx->br_edr_list.num;
}

static uint8_t app_dongle_cm_read_recon_list_from_le_bonded_info(uint8_t num, bool read_all)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_clear_recon_list(true);

    if (read_all) {
        if (0 == (ctrl_ctx->le_list.num = bt_device_manager_le_get_bonded_number())) {
            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"read le bond info fail, no bonded info!", 0);
            return 0;
        }
    } else {
        ctrl_ctx->le_list.num = num;
    }

    ctrl_ctx->le_list.addr = app_dongle_cm_memory_alloc(ctrl_ctx->le_list.num * sizeof(bt_bd_addr_t));
    if (NULL == ctrl_ctx->le_list.addr) {
        assert(0);
        app_dongle_cm_memset(&ctrl_ctx->le_list, 0, sizeof(app_dongle_cm_reconnect_list_info_t));
        return 0;
    }

    bt_device_manager_le_get_bonded_list(ctrl_ctx->le_list.addr, &ctrl_ctx->le_list.num);
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"read le bond info, bonded num is %d", 1, ctrl_ctx->le_list.num);

    uint8_t *tempaddr = NULL;
    uint8_t i;
    i = ctrl_ctx->le_list.num;
    while (i > 0) {
        i--;
        tempaddr = (uint8_t *)&ctrl_ctx->le_list.addr[i];
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"bonded_list[%x], addr:%02x:%02x:%02x:%02x:%02x:%02x", 7, i, tempaddr[5], tempaddr[4],
                                                                                            tempaddr[3], tempaddr[2],
                                                                                            tempaddr[1], tempaddr[0]);
    }
    return ctrl_ctx->le_list.num;
}

static bt_addr_type_t app_dongle_cm_get_addr_type_from_bonding_list(bt_bd_addr_t *addr)
{
    bt_device_manager_le_bonded_info_t *bonded_info = NULL;

    bonded_info = bt_device_manager_le_get_bonding_info_by_addr_ext(addr);
    if (!bonded_info) {
        return 0;
    }
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Get addr type: %d", 1, bonded_info->bt_addr.type);
    return  bonded_info->bt_addr.type;

}
static bt_status_t app_dongle_cm_set_white_list(void)
{
    uint8_t i = 0;
    bt_status_t status = BT_STATUS_FAIL;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    if (ctrl_ctx->le_scan.scan_state_mask & APP_DONGLE_CM_SCAN_STATE_DISABLED) {
        if (!ctrl_ctx->le_list.num) {
            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"BONDING LIST IS NULL, status:%x", 1, status);
            return BT_STATUS_FAIL;
        }
        ctrl_ctx->wl_info.num = 0;
        ctrl_ctx->wl_info.set_wl_state = APP_DONGLE_CM_SET_WHITE_LIST_STATE_ADD_ON_GOING;
        bt_addr_t device;
        for (i = 0; i < ctrl_ctx->le_list.num; i ++) {
            device.type = app_dongle_cm_get_addr_type_from_bonding_list(&ctrl_ctx->le_list.addr[i]);
            app_dongle_cm_memcpy(&device.addr, &ctrl_ctx->le_list.addr[i], sizeof(bt_bd_addr_t));
            status = app_dongle_cm_add_white_list(&device);
        }
        return status;
    }
    return BT_STATUS_FAIL;
}

static app_dongle_cm_source_t app_dongle_cm_check_uuid(bt_gap_le_ext_advertising_report_ind_t *adv_report)
{
    bt_bd_addr_t empty_addr = {0};
    uint16_t i = 0;
    bool is_lea_adv = false;
//    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_check_uuid", 0);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_source_info_t *source_info_ull = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
    app_dongle_cm_source_info_t *source_info_lea = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);

    if (!(adv_report->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_CONNECTABLE)) {
        return APP_DONGLE_CM_SOURCE_INVALID;
    }

    if ((!(adv_report->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED) &&
         (0 == app_dongle_cm_memcmp(adv_report->address.addr, empty_addr, sizeof(bt_bd_addr_t)))) ||
        ((adv_report->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED) &&
         (0 == app_dongle_cm_memcmp(adv_report->direct_address.addr, empty_addr, sizeof(bt_bd_addr_t))))) {
        return APP_DONGLE_CM_SOURCE_INVALID;
    }

    if (adv_report->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED && \
        bt_device_manager_le_get_link_type_by_addr(&adv_report->address.addr) & BT_GAP_LE_SRV_LINK_TYPE_LE_AUDIO) {
        return APP_DONGLE_CM_SOURCE_LEA;
    }

    while (i < adv_report->data_length) {
        if (adv_report->data[i] >= 17) {
            if ((ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) && \
                (APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_ull->conn_state) && \
                (adv_report->data[i+1] == BT_GAP_LE_AD_TYPE_128_BIT_UUID_DATA) && \
                (!app_dongle_cm_memcmp(&(adv_report->data[i+2]), &g_app_dongle_ull_v2_uuid128_default, 16))) {
                    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"FIND UUL v2 ADV!!", 0);
                    return APP_DONGLE_CM_SOURCE_ULL_V2;
            } else if ((ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) && \
                (APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_lea->conn_state) && \
                (adv_report->data[i + 1] == BT_GAP_LE_AD_TYPE_SERVICE_DATA) && \
                (adv_report->data[i + 2] == (BT_GATT_UUID16_ASCS_SERVICE & 0xFF)) && \
                (adv_report->data[i + 3] == ((BT_GATT_UUID16_ASCS_SERVICE & 0xFF00) >> 8))) {
                APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"FIND LEA ADV-1", 0);
                is_lea_adv = true;
            }
        } else if (adv_report->data[i] >= 3) {
            if ((ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) && \
                (APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_lea->conn_state) && \
                (adv_report->data[i + 1] == BT_GAP_LE_AD_TYPE_SERVICE_DATA) && \
                (adv_report->data[i + 2] == (BT_GATT_UUID16_ASCS_SERVICE & 0xFF)) && \
                (adv_report->data[i + 3] == ((BT_GATT_UUID16_ASCS_SERVICE & 0xFF00) >> 8))) {
                is_lea_adv = true;
                APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"FIND LEA ADV!!", 0);
            }
        }
        i += (adv_report->data[i] + 1);
    }
    if (is_lea_adv) {
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"FIND LEA ADV", 0);
        return APP_DONGLE_CM_SOURCE_LEA;
    }
    return APP_DONGLE_CM_SOURCE_INVALID;
}
static uint8_t app_dongle_cm_check_uuid_ex(bt_gap_le_ext_advertising_report_ind_t *adv_report)
{
    bt_bd_addr_t empty_addr = {0};
    uint16_t i = 0;
    uint8_t source_type_mask = 0x00;
    bool is_lea_adv = false;
//    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_check_uuid", 0);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_source_info_t *source_info_ull = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
    app_dongle_cm_source_info_t *source_info_lea = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);

    if (!(adv_report->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_CONNECTABLE)) {
        return source_type_mask;
    }

    if ((!(adv_report->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED) &&
         (0 == app_dongle_cm_memcmp(adv_report->address.addr, empty_addr, sizeof(bt_bd_addr_t)))) ||
        ((adv_report->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED) &&
         (0 == app_dongle_cm_memcmp(adv_report->direct_address.addr, empty_addr, sizeof(bt_bd_addr_t))))) {
        return source_type_mask;
    }

    if (adv_report->event_type & BT_GAP_LE_EXT_ADV_REPORT_EVT_MASK_DIRECTED && \
        (bt_device_manager_le_get_link_type_by_addr(&adv_report->address.addr) & BT_GAP_LE_SRV_LINK_TYPE_LE_AUDIO) &&
        (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA)) {
        return source_type_mask |= (1 << APP_DONGLE_CM_SOURCE_LEA);
    }

    while (i < adv_report->data_length) {
        if (adv_report->data[i] >= 17) {
            if ((ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) && \
                (APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_ull->conn_state) && \
                (adv_report->data[i+1] == BT_GAP_LE_AD_TYPE_128_BIT_UUID_DATA) && \
                (!app_dongle_cm_memcmp(&(adv_report->data[i+2]), &g_app_dongle_ull_v2_uuid128_default, 16))) {
                    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"FIND UUL v2 ADV!!", 0);
                    source_type_mask |= (1 << APP_DONGLE_CM_SOURCE_ULL_V2);
            } else if ((ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) && \
                (APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_lea->conn_state) && \
                (adv_report->data[i + 1] == BT_GAP_LE_AD_TYPE_SERVICE_DATA) && \
                (adv_report->data[i + 2] == (BT_GATT_UUID16_ASCS_SERVICE & 0xFF)) && \
                (adv_report->data[i + 3] == ((BT_GATT_UUID16_ASCS_SERVICE & 0xFF00) >> 8))) {
                APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"FIND LEA ADV-1", 0);
                is_lea_adv = true;
            }
        } else if (adv_report->data[i] >= 3) {
            if ((ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) && \
                (APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_lea->conn_state) && \
                (adv_report->data[i + 1] == BT_GAP_LE_AD_TYPE_SERVICE_DATA) && \
                (adv_report->data[i + 2] == (BT_GATT_UUID16_ASCS_SERVICE & 0xFF)) && \
                (adv_report->data[i + 3] == ((BT_GATT_UUID16_ASCS_SERVICE & 0xFF00) >> 8))) {
                is_lea_adv = true;
                APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"FIND LEA ADV!!", 0);
            }
        }
        i += (adv_report->data[i] + 1);
    }
    if (is_lea_adv) {
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"FIND LEA ADV", 0);
        source_type_mask |= (1 << APP_DONGLE_CM_SOURCE_LEA);
    }
    return source_type_mask;
}


static bt_status_t app_dongle_cm_enable_scan(bool wl_enable)
{
    bt_status_t status;
    le_ext_scan_item_t ext_scan_1M_item = {
        .scan_type = BT_HCI_SCAN_TYPE_PASSIVE,
        .scan_interval = 0x90,
        .scan_window = 0x30,
    };
    bt_hci_cmd_le_set_extended_scan_enable_t enable = {
        .enable = BT_HCI_ENABLE,
        .filter_duplicates = BT_HCI_ENABLE,
        .duration = 0,
        .period = 0
    };

    bt_hci_le_set_ext_scan_parameters_t param = {
        .own_address_type = BT_HCI_SCAN_ADDR_RANDOM,
        .scanning_filter_policy = BT_HCI_SCAN_FILTER_ACCEPT_ALL_ADVERTISING_PACKETS,
        .scanning_phys_mask = BT_HCI_LE_PHY_MASK_1M,
        .params_phy_1M = &ext_scan_1M_item,
        .params_phy_coded = NULL,
    };
    param.scanning_filter_policy = wl_enable ? BT_HCI_SCAN_FILTER_ACCEPT_ONLY_ADVERTISING_PACKETS_IN_WHITE_LIST : BT_HCI_SCAN_FILTER_ACCEPT_ALL_ADVERTISING_PACKETS;
    if (BT_STATUS_SUCCESS != (status = bt_gap_le_set_extended_scan(&param, &enable))) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"enable adv scan failed, status:%x", 1, status);
        if (BT_STATUS_OUT_OF_MEMORY == status) {
            assert(0);
        }
    }
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"ENABLE ADV SCAN, status:%x", 1, status);
    return status;

}

static bt_status_t app_dongle_cm_disable_scan(void)
{
    bt_status_t status;
    bt_hci_cmd_le_set_extended_scan_enable_t enable = {
        .enable = BT_HCI_DISABLE,
        .filter_duplicates = BT_HCI_DISABLE,
        .duration = 0,
        .period = 0
    };
    if (BT_STATUS_SUCCESS != (status = bt_gap_le_set_extended_scan(NULL, &enable))) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"disable adv scan failed, status:%x", 1, status);
        if (BT_STATUS_OUT_OF_MEMORY == status) {
            assert(0);
        }
    }
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"DISAVLE ADV SCAN, status:%x", 1, status);
    return status;
}

static bt_status_t app_dongle_cm_le_stop_scan_device_internal(void)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_scan_state_mask_t mask = app_dongle_cm_get_scan_state_mask();
    app_dongle_cm_link_mode_t mode = ctrl_ctx->link_mode;
    if (mode & APP_DONGLE_CM_LINK_MODE_ULL_V2 || mode & APP_DONGLE_CM_LINK_MODE_LEA) {
        if (APP_DONGLE_CM_SCAN_STATE_ENABLED == mask) {
            app_dongle_cm_set_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_DISABLING);
            status = app_dongle_cm_disable_scan();
            if (status) {
                app_dongle_cm_clear_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_DISABLING);
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"DISABLE SCAN failed, status:%x", 1, status);
            }
        } else if (mask & APP_DONGLE_CM_SCAN_STATE_DISABLING || mask == APP_DONGLE_CM_SCAN_STATE_DISABLED) {
            return BT_STATUS_SUCCESS;
        } else if (mask & APP_DONGLE_CM_SCAN_STATE_ENABLING) {
            ctrl_ctx->le_scan.next_action = APP_DONGLE_CM_NEXT_ACTION_STOP_SCAN;
            return BT_STATUS_SUCCESS;
        }
    } else {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"link mode error!", 0);
    }
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"DISABLE SCAN, mask1: 0x%x, mask2: 0x%x, next_action: 0x%x", 3, mask, app_dongle_cm_get_scan_state_mask(), ctrl_ctx->le_scan.next_action);
    return status;

}

static bt_status_t app_dongle_cm_le_start_scan_device_internal(bool wl_enable)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_scan_state_mask_t mask = app_dongle_cm_get_scan_state_mask();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"ENABLE SCAN, mask1: 0x%x, next_action: 0x%x, wl_enable: %d, wl_state: %d", 4, \
        mask, ctrl_ctx->le_scan.next_action, wl_enable, ctrl_ctx->wl_info.set_wl_state);
    if (APP_DONGLE_CM_SET_WHITE_LIST_STATE_ADD_ON_GOING == ctrl_ctx->wl_info.set_wl_state || \
        APP_DONGLE_CM_SET_WHITE_LIST_STATE_REMOVE_ON_GOING == ctrl_ctx->wl_info.set_wl_state || \
        APP_DONGLE_CM_SET_WHITE_LIST_STATE_CLEAR_ON_GOING == ctrl_ctx->wl_info.set_wl_state) {
        return BT_STATUS_FAIL;
    }
    ctrl_ctx->le_scan.wl_enable = wl_enable;
    if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2 || ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
        if (APP_DONGLE_CM_SCAN_STATE_DISABLED == mask) {
            app_dongle_cm_set_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_ENABLING);
            status = app_dongle_cm_enable_scan(wl_enable);
            if (status) {
                app_dongle_cm_clear_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_ENABLING);
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"ENABLE SCAN failed, status:%x", 1, status);
            }
        } else if (mask & APP_DONGLE_CM_SCAN_STATE_ENABLING || mask == APP_DONGLE_CM_SCAN_STATE_ENABLED) {
            return BT_STATUS_SUCCESS;
        } else if (mask & APP_DONGLE_CM_SCAN_STATE_DISABLING) {
            ctrl_ctx->le_scan.next_action = APP_DONGLE_CM_NEXT_ACTION_START_SCAN;
            return BT_STATUS_SUCCESS;
        }
    } else {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"link mode error!", 0);
    }
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"ENABLE SCAN, mask2: 0x%x next_action: 0x%x", 2, ctrl_ctx->le_scan.scan_state_mask, ctrl_ctx->le_scan.next_action);
    return status;
}

bt_status_t app_dongle_cm_start_inquiry_device(app_dongle_cm_link_mode_t mode)
{
     return bt_gap_inquiry_ext(10, 10, 0x9E8B33);
}

bt_status_t app_dongle_cm_stop_inquiry_device(app_dongle_cm_link_mode_t mode)
{
    return bt_gap_cancel_inquiry();
}

bt_status_t app_dongle_cm_edr_enable_scan_device(app_dongle_cm_link_mode_t mode)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t app_dongle_cm_edr_disable_scan_device(app_dongle_cm_link_mode_t mode)
{
    return BT_STATUS_SUCCESS;
}

static void app_dongle_cm_read_sirk(void)
{
    uint32_t size = APP_DONGLE_CM_NVKEY_CSIS_DATA_LEN;
    uint8_t data[APP_DONGLE_CM_NVKEY_CSIS_DATA_LEN] = {0};
    bt_key_t sirk = {0};
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    if (NVKEY_STATUS_OK == nvkey_read_data(NVID_BT_LEA_CSIS_DATA, data, &size)) {
        app_dongle_cm_memcpy(&ctrl_ctx->sirk, data, sizeof(bt_key_t));
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Read sirk success, sirk: %x-%x-%x-....%x-%x", 5, data[0], data[1],data[2], data[14], data[15]);
    } else {
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Read sirk fail!", 0);
    }
    if (app_dongle_cm_memcmp(&ctrl_ctx->sirk, &sirk, sizeof(bt_key_t))) {
        ctrl_ctx->le_scan.use_sirk = true;
    } else {
        ctrl_ctx->le_scan.use_sirk = false;
    }
}

static void app_dongle_cm_set_scan_state_mask(app_dongle_cm_scan_state_mask_t state)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    ctrl_ctx->le_scan.scan_state_mask |= state;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Set scan state: %x, curr mask: 0x%x", 2, state, ctrl_ctx->le_scan.scan_state_mask);
}

static void app_dongle_cm_clear_scan_state_mask(app_dongle_cm_scan_state_mask_t state)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    ctrl_ctx->le_scan.scan_state_mask &= (~ state);
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Clear scan state: %x, curr mask: 0x%x", 2, state, ctrl_ctx->le_scan.scan_state_mask);
}

static app_dongle_cm_scan_state_mask_t app_dongle_cm_get_scan_state_mask(void)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    return ctrl_ctx->le_scan.scan_state_mask;
}

static void app_dongle_cm_set_white_list_cnf_handler(bt_status_t status)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_set_white_list_cnf_handler, status: 0x%x, wl_state: %d, num: %d", 3, status, ctrl_ctx->wl_info.set_wl_state, ctrl_ctx->wl_info.num);
    if (status) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_set_white_list_cnf_handler, status: 0x%x!", 1, status);
    }
    if (APP_DONGLE_CM_SET_WHITE_LIST_STATE_ADD_ON_GOING == ctrl_ctx->wl_info.set_wl_state) {
        if (ctrl_ctx->wl_info.num != ctrl_ctx->le_list.num) {
            ctrl_ctx->wl_info.num += 1;
        }
        if (ctrl_ctx->wl_info.num == ctrl_ctx->le_list.num) {
            ctrl_ctx->wl_info.set_wl_state = APP_DONGLE_CM_SET_WHITE_LIST_STATE_COMPLETE;
            status = app_dongle_cm_le_start_scan_device_internal(APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST == ctrl_ctx->connect_style_le ? true : false);
            if (!status) {
                app_dongle_cm_start_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN, 15000, app_dongle_cm_sirk_scan_timeout_handler, NULL);
            } else {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_set_white_list_cnf_handler, start scan fail", 0);

            }
        }
    } else if (APP_DONGLE_CM_SET_WHITE_LIST_STATE_CLEAR_ON_GOING == ctrl_ctx->wl_info.set_wl_state) {
            ctrl_ctx->wl_info.num = 0;
            ctrl_ctx->wl_info.set_wl_state = APP_DONGLE_CM_SET_WHITE_LIST_STATE_NONE;
    } else if (APP_DONGLE_CM_SET_WHITE_LIST_STATE_REMOVE_ON_GOING == ctrl_ctx->wl_info.set_wl_state) {
            if (ctrl_ctx->wl_info.num) {
                ctrl_ctx->wl_info.num -= 1;
                ctrl_ctx->wl_info.set_wl_state = APP_DONGLE_CM_SET_WHITE_LIST_STATE_COMPLETE;
            }
            if (!ctrl_ctx->wl_info.num) {
                ctrl_ctx->wl_info.set_wl_state = APP_DONGLE_CM_SET_WHITE_LIST_STATE_NONE;
            }
    }

}

static void app_dongle_cm_bt_power_off_cnf_handler(void)
{
    uint8_t i = 0;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Power off", 0);
    app_dongle_cm_source_info_t *source_info = NULL;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_memset(&ctrl_ctx->le_scan, 0, sizeof(ctrl_ctx->le_scan));
    app_dongle_cm_memset(&ctrl_ctx->edr_scan, 0, sizeof(ctrl_ctx->edr_scan));
    app_dongle_cm_clear_recon_list(true);
    app_dongle_cm_clear_recon_list(false);
    for (i = 0; i < APP_DONGLE_CM_SOURCE_MAX; i ++) {
        source_info = app_dongle_cm_get_source_info(i);
        if (APP_DONGLE_CM_STATE_DISCONNECTED != source_info->conn_state) {
            app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
        }
    }
}

void app_dongle_cm_pairing_key_event_handler(void)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_link_mode_t link_mode = ctrl_ctx->link_mode;
    app_dongle_cm_source_info_t *source_info = NULL;
    bt_status_t status = BT_STATUS_SUCCESS;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"PAIRING KEY EVENT. Link mode: 0x%x, connect type le: %d", 2, link_mode, ctrl_ctx->connect_style_le);

    if (APP_DONGLE_CM_STATE_CONNECTED == ctrl_ctx->source_info[APP_DONGLE_CM_SOURCE_ULL_V1].conn_state \
        && (link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V1)) {
        //todo

    } else if (APP_DONGLE_CM_STATE_CONNECTED == ctrl_ctx->source_info[APP_DONGLE_CM_SOURCE_ULL_V2].conn_state \
        && (link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2)) {

        //todo
    } else if (APP_DONGLE_CM_STATE_CONNECTED == ctrl_ctx->source_info[APP_DONGLE_CM_SOURCE_LEA].conn_state \
        && (link_mode & APP_DONGLE_CM_SOURCE_LEA)) {
        //todo
    } else if (APP_DONGLE_CM_STATE_CONNECTED == ctrl_ctx->source_info[APP_DONGLE_CM_SOURCE_BTA].conn_state \
        && (link_mode & APP_DONGLE_CM_SOURCE_BTA)) {
        //todo
    } else if (APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN_FOR_USER == ctrl_ctx->connect_style_le) {
        //TODO
    } else { /*no device connected, will full scan*/
        if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2 || ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
            app_dongle_cm_clear_white_list();
            if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) {
                app_dongle_cm_set_ull_v2_feature_bit(true);
            } else {
                app_dongle_cm_set_ull_v2_feature_bit(false);
            }

            if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) {
                source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
                if (source_info->source_registed) {
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_SCAN_INQUIRY);
                }
            }
            if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
                source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_LINK_MODE_LEA);
                if (source_info->source_registed) {
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_SCAN_INQUIRY);
                }
            }
            ctrl_ctx->connect_style_le = ctrl_ctx->le_scan.use_sirk ? APP_DONGLE_CM_CONNECTION_USING_SIRK : APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN;
            status = app_dongle_cm_le_start_scan_device_internal(false);
            if (BT_STATUS_SUCCESS != status) {
                if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) {
                    source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
                    if (source_info->source_registed) {
                        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
                    }
                }
                if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
                    source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_LINK_MODE_LEA);
                    if (source_info->source_registed) {
                        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
                    }
                }
                return;
            }
        }

        if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V1) {
            //todo
            bt_addr_t bt_addr = {0};
            app_dongle_cm_start_source_param_t param = {0};
            param.is_air_pairing = true;
            source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V1);
            if (source_info && source_info->cm_handle.start_source) {
                status = source_info->cm_handle.start_source(bt_addr, param);
            }
            ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN;
            if (!status) {
                app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_CONNECTION);
                app_dongle_cm_memcpy(&source_info->connect_addr, ctrl_ctx->br_edr_list.addr, sizeof(bt_bd_addr_t));
            } else {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start ULL v1 error!", 0);
            }

        }

        if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_BTA) {
            //todo
        }
    }


}

app_dongle_cm_link_mode_t app_dongle_cm_get_link_mode(void)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    return ctrl_ctx->link_mode;
}

static void app_dongle_cm_sirk_scan_timeout_handler(uint8_t timer_id, void *data)
{
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_sirk_scan_timeout_handler", 0);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();

    app_dongle_cm_le_stop_scan_device_internal();
    ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_SIRK;
    app_dongle_cm_le_start_scan_device_internal(false);
}

static void app_dongle_cm_set_source_conn_state(app_dongle_cm_source_info_t *source_info, app_dongle_cm_state_t state)
{
    if (!source_info) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"NULL source_info: %d", 0);
        return;
    }
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Source type: %d curr state %d -> next sate: %d", 3,source_info->source_type, source_info->conn_state, state);
    source_info->conn_state = state;
}
static void app_dongle_cm_bt_power_on_cnf_handler(void)
{
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    uint8_t le_reconnect = 0;


    bt_status_t status = BT_STATUS_FAIL;
    app_dongle_cm_source_info_t *source_info = NULL;

    app_dongle_cm_set_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_DISABLED);

    if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2 || ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
        if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) {
            app_dongle_cm_set_ull_v2_feature_bit(true);
        } else {
            app_dongle_cm_set_ull_v2_feature_bit(false);
        }

        app_dongle_cm_read_sirk();

        le_reconnect = app_dongle_cm_read_recon_list_from_le_bonded_info(0, true);
        if (le_reconnect) {
            status = app_dongle_cm_set_white_list();
            ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST;
        } else if (ctrl_ctx->le_scan.use_sirk) {
            ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_SIRK;
            status = app_dongle_cm_le_start_scan_device_internal(false);
        } else {
            ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_WAITE_FOR_PAIRING_KEY;
            //status = app_dongle_cm_le_start_scan_device_internal(false);
        }
        if (APP_DONGLE_CM_CONNECTION_WAITE_FOR_PAIRING_KEY != ctrl_ctx->connect_style_le) {
//        if (1) {

            if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) {
                source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
                if (source_info->source_registed) {
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_SCAN_INQUIRY);
                }
            }
            if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
                source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);
                if (source_info->source_registed) {
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_SCAN_INQUIRY);
                }
            }
            status = app_dongle_cm_le_start_scan_device_internal(APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST == ctrl_ctx->connect_style_le ? true : false);
            if (status) {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"start scan fail: %d", 1, status);
            }
        }
    }
    if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V1) {
        bt_addr_t bt_addr = {0};
        app_dongle_cm_start_source_param_t param = {0};
        source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V1);
        if (app_dongle_cm_read_recon_list_from_bonded_info(1)) {
            ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST;
            app_dongle_cm_memcpy(&bt_addr.addr, ctrl_ctx->br_edr_list.addr, sizeof(bt_bd_addr_t));
            param.is_air_pairing = false;
            if (source_info->cm_handle.start_source) {
                status = source_info->cm_handle.start_source(bt_addr, param);
            }

            if (!status) {
                app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_CONNECTION);
                app_dongle_cm_memcpy(&source_info->connect_addr, ctrl_ctx->br_edr_list.addr, sizeof(bt_bd_addr_t));
            } else {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start ULL v1 error!", 0);
            }

        } else {
            ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_WAITE_FOR_PAIRING_KEY;
        }
    }

    if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_BTA) {
        //TODO
    }

    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"BT Power on: link mode:%x, connect_style1: 0x%x, connect_style_edr:0x%x", 3, ctrl_ctx->link_mode, ctrl_ctx->connect_style_le, ctrl_ctx->connect_style_edr);


}

static void app_dongle_cm_le_scan_cnf_handler(bt_status_t status)
{
    app_dongle_cm_scan_state_mask_t mask = app_dongle_cm_get_scan_state_mask();
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    uint8_t next_action = ctrl_ctx->le_scan.next_action;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_le_scan_cnf_handler, mask:%x, next_action: %x", 2, mask, ctrl_ctx->le_scan.next_action);
    if (BT_STATUS_SUCCESS != status) {
        if (mask & APP_DONGLE_CM_SCAN_STATE_ENABLING) {
            app_dongle_cm_clear_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_ENABLING);
            if (APP_DONGLE_CM_NEXT_ACTION_STOP_SCAN == ctrl_ctx->le_scan.next_action) {
                ctrl_ctx->le_scan.next_action = APP_DONGLE_CM_NEXT_ACTION_NONE;
            }
        } else if (mask & APP_DONGLE_CM_SCAN_STATE_DISABLING) {
            app_dongle_cm_clear_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_DISABLING);
            if (APP_DONGLE_CM_NEXT_ACTION_START_SCAN == ctrl_ctx->le_scan.next_action) {
                ctrl_ctx->le_scan.next_action = APP_DONGLE_CM_NEXT_ACTION_NONE;
            }
        }
    } else {
        if (mask & APP_DONGLE_CM_SCAN_STATE_ENABLING) {
            app_dongle_cm_clear_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_ENABLING);
            app_dongle_cm_clear_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_DISABLED);
            app_dongle_cm_set_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_ENABLED);
            if (APP_DONGLE_CM_NEXT_ACTION_STOP_SCAN == ctrl_ctx->le_scan.next_action) {
                ctrl_ctx->le_scan.next_action = APP_DONGLE_CM_NEXT_ACTION_NONE;
                app_dongle_cm_le_stop_scan_device_internal();
            }
        } else if (mask & APP_DONGLE_CM_SCAN_STATE_DISABLING) {
            app_dongle_cm_clear_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_DISABLING);
            app_dongle_cm_clear_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_ENABLED);
            app_dongle_cm_set_scan_state_mask(APP_DONGLE_CM_SCAN_STATE_DISABLED);
            if (APP_DONGLE_CM_NEXT_ACTION_START_SCAN == ctrl_ctx->le_scan.next_action) {
                ctrl_ctx->le_scan.next_action = APP_DONGLE_CM_NEXT_ACTION_NONE;
                app_dongle_cm_le_start_scan_device_internal(ctrl_ctx->le_scan.wl_enable);
            }
        }
    }
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"scan cnf, mask1: 0x%x, mask2: 0x%x, next_action: 0x%x", 2, mask, app_dongle_cm_get_scan_state_mask(), next_action);

}

static void app_dongle_cm_adv_report_cnf_handler(bt_status_t status, bt_gap_le_ext_advertising_report_ind_t *data)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_gap_le_srv_link_t link_type_mask = 0;
    app_dongle_cm_source_info_t *source_info1 = NULL;
    app_dongle_cm_source_info_t *source_info2 = NULL;
    app_dongle_cm_source_t source_type = APP_DONGLE_CM_SOURCE_INVALID;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    if (BT_STATUS_SUCCESS != status) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"adv report fail, status is 0x%x!", 1, status);
        return;
    }
    app_dongle_cm_source_info_t *source_info_ull = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
    app_dongle_cm_source_info_t *source_info_lea = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);
    if (APP_DONGLE_CM_STATE_START_SCAN_INQUIRY != source_info_ull->conn_state
        && APP_DONGLE_CM_STATE_START_SCAN_INQUIRY != source_info_lea->conn_state) {
        //APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"error conn state! lea: %d, ull2.0:%d !", 2, source_info_lea->conn_state, source_info_ull->conn_state);
        return;
    }
    if (APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN_FOR_USER == ctrl_ctx->connect_style_le) {
        if (!g_dongle_le_cm_race_callback) {
            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"g_dongle_le_cm_race_callback is null!", 0);
            return;
        }
        source_type = app_dongle_cm_check_uuid_ex(data);
        if (source_type & (1 << APP_DONGLE_CM_SOURCE_LEA) \
            && (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA)) {
            g_dongle_le_cm_race_callback(APP_DONGLE_LE_RACE_EVT_ADV_REPORT, APP_DONGLE_LE_RACE_SINK_DEVICE_LEA, data);
        }

        if (source_type & (1 << APP_DONGLE_CM_SOURCE_ULL_V2) \
            && (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2)) {
            g_dongle_le_cm_race_callback(APP_DONGLE_LE_RACE_EVT_ADV_REPORT, APP_DONGLE_LE_RACE_SINK_DEVICE_ULL_V2, data);
        }
        return;
    } else {
        source_type = app_dongle_cm_check_uuid(data);
    }
    /*APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"ADV Report, source_type: %d, link mode: %d, conn type: %d", 3,
        source_type, ctrl_ctx->link_mode, ctrl_ctx->connect_style_le);*/

    switch (source_type) {
        case APP_DONGLE_CM_SOURCE_ULL_V2: {
            if (APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST == ctrl_ctx->connect_style_le) {
                link_type_mask = bt_device_manager_le_get_link_type_by_addr(&data->address.addr);
                APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Source type is ULL v2, link type:0x%x", 1, link_type_mask);
                if (link_type_mask & BT_GAP_LE_SRV_LINK_TYPE_ULL_V2) {

                } else {
                    APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Link type is not ULL v2, link type:0x%x", 1, link_type_mask);
                    //break;
                }
            } else if (APP_DONGLE_CM_CONNECTION_USING_SIRK == ctrl_ctx->connect_style_le) {

            } else if (APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN_FOR_USER == ctrl_ctx->connect_style_le) {
                return;
            }

            if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2 && APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_ull->conn_state) {
                source_info1 = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
                if (source_info1->source_registed &&
                    source_info1->cm_handle.precheck &&
                    source_info1->cm_handle.start_source) {
                    app_dongle_cm_start_source_param_t param = {0};
                    if (ctrl_ctx->le_scan.use_sirk \
                        && APP_DONGLE_CM_CONNECTION_USING_SIRK == ctrl_ctx->connect_style_le) {
                        if (BT_STATUS_SUCCESS == (ret = source_info1->cm_handle.precheck((app_dongle_cm_precheck_data_t*)data))) {
                            APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Precheck PASS(ULL v2)!!", 0);
                            ret = source_info1->cm_handle.start_source(data->address, param);
                        } else {
                            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Precheck FAIL(ULL v2)!!", 0);
                            return;
                        }
                    } else {
                        ret = source_info1->cm_handle.start_source(data->address, param);
                    }
                    if (ret) {
                        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start source fail(ULL v2), type: 0x%x!", 1, APP_DONGLE_CM_SOURCE_ULL_V2);
                        return;
                    } else {
                        app_dongle_cm_set_source_conn_state(source_info1, APP_DONGLE_CM_STATE_START_CONNECTION);
                        app_dongle_cm_memcpy(&source_info1->connect_addr, &data->address, sizeof(bt_bd_addr_t));
                        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"START Source success(ULL v2)!!", 0);
                    }
                    app_dongle_cm_stop_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN);
                    app_dongle_cm_le_stop_scan_device_internal();
                    source_info2 = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);
                    if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA
                        && APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info2->conn_state) {
                        app_dongle_cm_set_source_conn_state(source_info2, APP_DONGLE_CM_STATE_DISCONNECTED);
                    }
                } else {
                    APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error Source Info(ULL v2)!!", 0);
                }
            } else {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error Link Mode(ULL v2) or ULL v2 not open!!", 0);
            }
        } break;

        case APP_DONGLE_CM_SOURCE_LEA: {
            if (APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST == ctrl_ctx->connect_style_le && \
                ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2 && \
                ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA && \
                APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_ull->conn_state) {
                link_type_mask = bt_device_manager_le_get_link_type_by_addr(&data->address.addr);
                APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Source type is LEA, link type:0x%x", 1, link_type_mask);
                if (link_type_mask & BT_GAP_LE_SRV_LINK_TYPE_LE_AUDIO) {

                } else {
                    APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Link type is not LEA, link type:0x%x", 1, link_type_mask);
                    break;
                }
            } else if (APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN_FOR_USER == ctrl_ctx->connect_style_le) {
                return;
            }

            if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA && APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info_lea->conn_state) {
                source_info1 = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);
                if (source_info1->source_registed &&
                    source_info1->cm_handle.precheck &&
                    source_info1->cm_handle.start_source) {
                    app_dongle_cm_start_source_param_t param = {0};
                    param.lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;
                    if (ctrl_ctx->le_scan.use_sirk \
                        && APP_DONGLE_CM_CONNECTION_USING_SIRK == ctrl_ctx->connect_style_le) {
                         if (BT_STATUS_SUCCESS == (ret = source_info1->cm_handle.precheck((app_dongle_cm_precheck_data_t*)data))) {
                             APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Precheck PASS(LEA)!!", 0);
                             ret = source_info1->cm_handle.start_source(data->address, param);
                         } else {
                             APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Precheck FAIL(LEA)!!", 0);
                         }
                     } else {
                         ret = source_info1->cm_handle.start_source(data->address, param);
                     }
                    if (ret) {
                        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start source fail(LEA), type: 0x%x!", 1, APP_DONGLE_CM_SOURCE_LEA);
                        return;
                    } else {
                        app_dongle_cm_set_source_conn_state(source_info1, APP_DONGLE_CM_STATE_START_CONNECTION);
                        app_dongle_cm_memcpy(&source_info1->connect_addr, &data->address, sizeof(bt_bd_addr_t));
                        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"START Source success(LEA)!!", 0);
                    }

                    if (ret) {
                        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start source fail, type: 0x%x!", 1, APP_DONGLE_CM_SOURCE_LEA);
                        return;
                    }
                    app_dongle_cm_stop_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN);
                    app_dongle_cm_le_stop_scan_device_internal();
                    source_info2 = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
                    if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2
                        && APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info2->conn_state) {
                        app_dongle_cm_set_source_conn_state(source_info2, APP_DONGLE_CM_STATE_DISCONNECTED);
                    }
                }else {
                    APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error Source Info(LEA)!!", 0);
                }
            } else {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error Link Mode(LEA) or LEA not open!!", 0);
            }
        } break;

        default:
            break;
    }

}

static bt_status_t app_dongle_cm_source_started_ull_v2_handler(app_dongle_cm_source_t type, bt_status_t status, void *data)
{
    uint8_t i = 0;
    bt_status_t ret = BT_STATUS_SUCCESS;
    app_dongle_cm_source_info_t *source_info = NULL;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_stop_source_param_t param = {0};
    ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
    ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
    app_dongle_cm_stop_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN);
    app_dongle_cm_le_stop_scan_device_internal();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_started_ull_v2_handler,status: %x", 1, status);
    if (BT_STATUS_SUCCESS == status) {
        for (i = APP_DONGLE_CM_SOURCE_ULL_V1; i < APP_DONGLE_CM_SOURCE_MAX; i ++) {
            if (i != type && ctrl_ctx->link_mode & (1 << i)) {
                source_info = app_dongle_cm_get_source_info(i);
                if (APP_DONGLE_CM_STATE_START_CONNECTION <= source_info->conn_state) {
                    if (source_info->source_registed && source_info->cm_handle.stop_source) {
                        bt_addr_t bt_addr = {0};
                        app_dongle_cm_memcpy(&bt_addr, &source_info->connect_addr, sizeof(bt_bd_addr_t));
                        ret = source_info->cm_handle.stop_source(bt_addr, param);
                    }
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_STOP_CONNECTION);
                } else if (APP_DONGLE_CM_STATE_DISCONNECTED < source_info->conn_state) {
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
                }

            }
        }
        source_info = app_dongle_cm_get_source_info(type);
        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_CONNECTED);
        ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_ULL_V2;
    } else {
        source_info = app_dongle_cm_get_source_info(type);
        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
        ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
    }
    if (ctrl_ctx->dongle_mode.is_switch_dongle_mode) {
        ctrl_ctx->dongle_mode.is_switch_dongle_mode = false;
        app_dongle_cm_notify_switch_dongle_mode_result(status, APP_DONGLE_CM_DONGLE_MODE_ULL_V2);
    }

    return ret;
}

static bt_status_t app_dongle_cm_source_started_lea_handler(app_dongle_cm_source_t type, bt_status_t status, void *data)
{
    uint8_t i = 0;
    bt_status_t ret = BT_STATUS_SUCCESS;
    app_dongle_cm_source_info_t *source_info = NULL;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_stop_source_param_t param = {0};
    app_dongle_cm_lea_mode_t * lea_mode = (app_dongle_cm_lea_mode_t *)data;
    ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
    ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
    if (NULL == lea_mode) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_started_lea_handler,lea mode is NULL", 0);
        return ret;
    }
    app_dongle_cm_stop_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN);
    app_dongle_cm_le_stop_scan_device_internal();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_started_lea_handler,status: %x, lea mode: %d", 2, status, *lea_mode);
    if (BT_STATUS_SUCCESS == status) {
        for (i = APP_DONGLE_CM_SOURCE_ULL_V1; i < APP_DONGLE_CM_SOURCE_MAX; i ++) {
            if (i != type && ctrl_ctx->link_mode & (1 << i)) {
                source_info = app_dongle_cm_get_source_info(i);
                if (APP_DONGLE_CM_STATE_START_CONNECTION <= source_info->conn_state) {
                    if (source_info->source_registed && source_info->cm_handle.stop_source) {
                        bt_addr_t bt_addr = {0};
                        app_dongle_cm_memcpy(&bt_addr, &source_info->connect_addr, sizeof(bt_addr_t));
                        ret = source_info->cm_handle.stop_source(bt_addr, param);
                    }
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_STOP_CONNECTION);
                } else if (APP_DONGLE_CM_STATE_DISCONNECTED < source_info->conn_state) {
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
                }

            }
        }
        source_info = app_dongle_cm_get_source_info(type);
        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_CONNECTED);
        if ((NULL != lea_mode) && APP_DONGLE_CM_LEA_MODE_CIS == *lea_mode) {
            ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_LEA_CIS;
        } else if ((NULL != lea_mode) && APP_DONGLE_CM_LEA_MODE_BIS == *lea_mode) {
            ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_LEA_BIS;
        } else {
            ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
        }
    } else {
        source_info = app_dongle_cm_get_source_info(type);
        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
        ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
    }
    if (ctrl_ctx->dongle_mode.is_switch_dongle_mode) {
        ctrl_ctx->dongle_mode.is_switch_dongle_mode = false;
        if ((NULL != lea_mode) && APP_DONGLE_CM_LEA_MODE_CIS == *lea_mode) {
            app_dongle_cm_notify_switch_dongle_mode_result(status, APP_DONGLE_CM_DONGLE_MODE_LEA_CIS);
        } else if ((NULL != lea_mode) && APP_DONGLE_CM_LEA_MODE_BIS == *lea_mode) {
            app_dongle_cm_notify_switch_dongle_mode_result(status, APP_DONGLE_CM_DONGLE_MODE_LEA_BIS);
        }
    }

    return ret;

}

static bt_status_t app_dongle_cm_source_end_ull_v2_handler(app_dongle_cm_source_t type, bt_status_t status, void *data)
{
    app_dongle_cm_source_info_t *source_info = app_dongle_cm_get_source_info(type);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_end_ull_v2_handler,status: %x", 1, status);
    if (BT_STATUS_SUCCESS == status) {
        if (ctrl_ctx->link_mode & (1 << type)) {
            app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            if (APP_DONGLE_CM_DONGLE_MODE_ULL_V2 == ctrl_ctx->dongle_mode.curr_mode) {
                ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
                if (ctrl_ctx->dongle_mode.is_switch_dongle_mode) {
                    app_dongle_cm_switch_to_next_dongle_mode(ctrl_ctx->dongle_mode.next_mode);
                    ctrl_ctx->dongle_mode.next_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
                }
            }
        }

    }
    return BT_STATUS_SUCCESS;
}

static bt_status_t app_dongle_cm_source_end_lea_handler(app_dongle_cm_source_t type, bt_status_t status, void *data)
{
    app_dongle_cm_source_info_t *source_info = app_dongle_cm_get_source_info(type);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_end_lea_handler,status: %x", 1, status);
    if (BT_STATUS_SUCCESS == status) {
        if (ctrl_ctx->link_mode & (1 << type)) {
            app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            if (APP_DONGLE_CM_DONGLE_MODE_LEA_CIS == ctrl_ctx->dongle_mode.curr_mode || APP_DONGLE_CM_DONGLE_MODE_LEA_BIS == ctrl_ctx->dongle_mode.curr_mode) {
                ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
                if (ctrl_ctx->dongle_mode.is_switch_dongle_mode) {
                    app_dongle_cm_switch_to_next_dongle_mode(ctrl_ctx->dongle_mode.next_mode);
                    ctrl_ctx->dongle_mode.next_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
                }

            }
        }
    }
    return BT_STATUS_SUCCESS;

}

static bt_status_t app_dongle_cm_source_started_ull_v1_handler(app_dongle_cm_source_t type, bt_status_t status, void *data)
{
    app_dongle_cm_stop_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN);
    app_dongle_cm_le_stop_scan_device_internal();
    uint8_t i = 0;
    bt_status_t ret = BT_STATUS_SUCCESS;
    app_dongle_cm_source_info_t *source_info = NULL;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_stop_source_param_t param = {0};
    ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
    ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
    app_dongle_cm_stop_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN);
    app_dongle_cm_le_stop_scan_device_internal();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_started_ull_v1_handler,status: %x", 1, status);
    if (BT_STATUS_SUCCESS == status) {
        for (i = APP_DONGLE_CM_SOURCE_ULL_V1; i < APP_DONGLE_CM_SOURCE_MAX; i ++) {
            if (i != type && ctrl_ctx->link_mode & (1 << i)) {
                source_info = app_dongle_cm_get_source_info(i);
                if (APP_DONGLE_CM_STATE_START_CONNECTION <= source_info->conn_state) {
                    if (source_info->source_registed && source_info->cm_handle.stop_source) {
                        bt_addr_t bt_addr = {0};
                        app_dongle_cm_memcpy(&bt_addr, &source_info->connect_addr, sizeof(bt_bd_addr_t));
                        ret = source_info->cm_handle.stop_source(bt_addr, param);
                    }
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_STOP_CONNECTION);
                } else if (APP_DONGLE_CM_STATE_DISCONNECTED < source_info->conn_state) {
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
                }
            }

        }
        source_info = app_dongle_cm_get_source_info(type);
        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_CONNECTED);
        ctrl_ctx->dongle_mode.curr_mode= APP_DONGLE_CM_DONGLE_MODE_ULL_V1;
    } else {
        source_info = app_dongle_cm_get_source_info(type);
        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
        ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
    }
    if (ctrl_ctx->dongle_mode.is_switch_dongle_mode) {
        ctrl_ctx->dongle_mode.is_switch_dongle_mode = false;
        app_dongle_cm_notify_switch_dongle_mode_result(status, APP_DONGLE_CM_DONGLE_MODE_ULL_V1);
    }

    return ret;

}
static bt_status_t app_dongle_cm_source_started_bta_handler(app_dongle_cm_source_t type, bt_status_t status, void *data)
{
    app_dongle_cm_stop_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN);
    app_dongle_cm_le_stop_scan_device_internal();
    uint8_t i = 0;
    bt_status_t ret = BT_STATUS_SUCCESS;
    app_dongle_cm_source_info_t *source_info = NULL;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_stop_source_param_t param = {0};
    ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
    ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;

    app_dongle_cm_stop_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN);
    app_dongle_cm_le_stop_scan_device_internal();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_started_bta_handler,status: %x", 1, status);
    if (BT_STATUS_SUCCESS == status) {
        for (i = APP_DONGLE_CM_SOURCE_ULL_V1; i < APP_DONGLE_CM_SOURCE_MAX; i ++) {
            if (i != type && ctrl_ctx->link_mode & (1 << i)) {
                source_info = app_dongle_cm_get_source_info(i);
                if (APP_DONGLE_CM_STATE_START_CONNECTION <= source_info->conn_state) {
                    if (source_info->source_registed && source_info->cm_handle.stop_source) {
                        bt_addr_t bt_addr = {0};
                        app_dongle_cm_memcpy(&bt_addr, &source_info->connect_addr, sizeof(bt_bd_addr_t));
                        ret = source_info->cm_handle.stop_source(bt_addr, param);
                    }
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_STOP_CONNECTION);
                } else if (APP_DONGLE_CM_STATE_DISCONNECTED < source_info->conn_state) {
                    app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
                }

            }

        }
        source_info = app_dongle_cm_get_source_info(type);
        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_CONNECTED);
        ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_BTA;
    } else {
        source_info = app_dongle_cm_get_source_info(type);
        app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
        ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;

    }
    if (ctrl_ctx->dongle_mode.is_switch_dongle_mode) {
        ctrl_ctx->dongle_mode.is_switch_dongle_mode = false;
        app_dongle_cm_notify_switch_dongle_mode_result(status, APP_DONGLE_CM_DONGLE_MODE_BTA);
    }

    return ret;


}
static bt_status_t app_dongle_cm_source_end_ull_v1_handler(app_dongle_cm_source_t type, bt_status_t status, void *data)
{
    app_dongle_cm_source_info_t *source_info = app_dongle_cm_get_source_info(type);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_end_ull_v1_handler,status: %x", 1, status);
    if (BT_STATUS_SUCCESS == status) {
        if (ctrl_ctx->link_mode & (1 << type) && source_info->conn_state == APP_DONGLE_CM_STATE_STOP_CONNECTION) {
            app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            if (APP_DONGLE_CM_DONGLE_MODE_ULL_V1 == ctrl_ctx->dongle_mode.curr_mode) {
                ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
                if (ctrl_ctx->dongle_mode.is_switch_dongle_mode) {
                    app_dongle_cm_switch_to_next_dongle_mode(ctrl_ctx->dongle_mode.next_mode);
                    ctrl_ctx->dongle_mode.next_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
                }

            }
        }
    }

    return BT_STATUS_SUCCESS;
}

static bt_status_t app_dongle_cm_source_end_bta_handler(app_dongle_cm_source_t type, bt_status_t status, void *data)
{
    app_dongle_cm_source_info_t *source_info = app_dongle_cm_get_source_info(type);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_source_end_bta_handler,status: %x", 1, status);
    if (BT_STATUS_SUCCESS == status) {
        if (ctrl_ctx->link_mode & (1 << type) && source_info->conn_state == APP_DONGLE_CM_STATE_STOP_CONNECTION) {
            app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            if (APP_DONGLE_CM_DONGLE_MODE_BTA == ctrl_ctx->dongle_mode.curr_mode) {
                ctrl_ctx->dongle_mode.curr_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
                if (ctrl_ctx->dongle_mode.is_switch_dongle_mode) {
                    app_dongle_cm_switch_to_next_dongle_mode(ctrl_ctx->dongle_mode.next_mode);
                    ctrl_ctx->dongle_mode.next_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
                }
            }
        }
    }

    return BT_STATUS_SUCCESS;
}

static app_dongle_cm_source_t app_dongle_cm_get_source_type_by_dongle_mode(app_dongle_cm_dongle_mode_t mode)
{
    //APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"dongle mode: %x", 1, mode);
    switch (mode) {
        case APP_DONGLE_CM_DONGLE_MODE_ULL_V1: {
            return APP_DONGLE_CM_SOURCE_ULL_V1;
        } break;

        case APP_DONGLE_CM_DONGLE_MODE_ULL_V2: {
            return APP_DONGLE_CM_SOURCE_ULL_V2;
        } break;

        case APP_DONGLE_CM_DONGLE_MODE_LEA_CIS: 
        case APP_DONGLE_CM_DONGLE_MODE_LEA_BIS: {
            return APP_DONGLE_CM_SOURCE_LEA;
        } break;

        case APP_DONGLE_CM_DONGLE_MODE_BTA: {
            return APP_DONGLE_CM_SOURCE_BTA;
        } break;
        default: 
            break;
    }
    return APP_DONGLE_CM_SOURCE_INVALID;
}
bt_status_t app_dongle_cm_switch_to_next_dongle_mode(app_dongle_cm_dongle_mode_t next_mode)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_source_t start_source_type = app_dongle_cm_get_source_type_by_dongle_mode(next_mode);
    app_dongle_cm_source_info_t *start_source_info = app_dongle_cm_get_source_info(start_source_type);
    bt_addr_t bt_addr = {0};

    app_dongle_cm_start_source_param_t start_param =  {0};
    if (!start_source_info) {
        return BT_STATUS_FAIL;
    }

    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_switch_to_next_dongle_mode, curr_mode: %d, next_mode: %d, state: %d", 3, ctrl_ctx->dongle_mode.curr_mode, next_mode, start_source_info->conn_state);

    if (APP_DONGLE_CM_STATE_DISCONNECTED != start_source_info->conn_state) {
        return status;
    }
    if (start_source_type == APP_DONGLE_CM_SOURCE_ULL_V2 || \
        (start_source_type == APP_DONGLE_CM_SOURCE_LEA && APP_DONGLE_CM_DONGLE_MODE_LEA_CIS == next_mode)) {
        uint8_t le_reconnect = app_dongle_cm_read_recon_list_from_le_bonded_info(0, true);
        if (le_reconnect) {
            ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST;
            app_dongle_cm_set_source_conn_state(start_source_info, APP_DONGLE_CM_STATE_START_SCAN_INQUIRY);
            status = app_dongle_cm_set_white_list();
            if (status) {
                ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Set wl error!", 1, next_mode);
                return BT_STATUS_FAIL;
            }
            //app_dongle_cm_start_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN, 15000, app_dongle_cm_sirk_scan_timeout_handler, NULL);
            //status = app_dongle_cm_le_start_scan_device_internal(true);
        } else if (ctrl_ctx->le_scan.use_sirk) {
            ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_SIRK;
            status = app_dongle_cm_le_start_scan_device_internal(false);
        } else {
            ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN;
            status = app_dongle_cm_le_start_scan_device_internal(false);
        }
        if (status) {
            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start scan error!", 1, next_mode);
            app_dongle_cm_set_source_conn_state(start_source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            return BT_STATUS_FAIL;
        }
    } else if (start_source_type == APP_DONGLE_CM_SOURCE_LEA && APP_DONGLE_CM_DONGLE_MODE_LEA_BIS == next_mode) {
        start_param.lea_mode = APP_DONGLE_CM_LEA_MODE_BIS;
        if (start_source_info->cm_handle.start_source) {
            status = start_source_info->cm_handle.start_source(bt_addr, start_param);
            if (status) {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start BIS error!", 1, next_mode);
                return BT_STATUS_FAIL;
            } else {
                app_dongle_cm_set_source_conn_state(start_source_info, APP_DONGLE_CM_STATE_START_CONNECTION);
            }
        } else {
            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"start_source is null!", 1, next_mode);
            return BT_STATUS_FAIL;
        }
    } else if (APP_DONGLE_CM_SOURCE_ULL_V1 == start_source_type) {
        if (app_dongle_cm_read_recon_list_from_bonded_info(1)) {
            app_dongle_cm_memcpy(&bt_addr.addr, ctrl_ctx->br_edr_list.addr, sizeof(bt_bd_addr_t));
            start_param.is_air_pairing = false;
            ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN;
        } else {
            start_param.is_air_pairing = true;
            ctrl_ctx->connect_style_edr = APP_DONGLE_CM_CONNECTION_USING_BONDING_LIST;
        }
        if (start_source_info->cm_handle.start_source) {
            status = start_source_info->cm_handle.start_source(bt_addr, start_param);
            if (!status) {
                app_dongle_cm_set_source_conn_state(start_source_info, APP_DONGLE_CM_STATE_START_CONNECTION);
                app_dongle_cm_memcpy(&start_source_info->connect_addr, ctrl_ctx->br_edr_list.addr, sizeof(bt_bd_addr_t));
            } else {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start ULL v1 error!", 1, next_mode);
                return BT_STATUS_FAIL;
            }
        } else {
            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"start_source is null!", 1, next_mode);
            return BT_STATUS_FAIL;
        }
    }else if (APP_DONGLE_CM_SOURCE_BTA == start_source_type) {
        //todo
    } else {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error mode or source type!", 1, next_mode);
        return BT_STATUS_FAIL;
    }
    return status;
}

bt_status_t app_dongle_cm_switch_dongle_mode(app_dongle_cm_dongle_mode_t mode)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_source_t start_source_type = app_dongle_cm_get_source_type_by_dongle_mode(mode);
    app_dongle_cm_source_t stop_source_type = app_dongle_cm_get_source_type_by_dongle_mode(ctrl_ctx->dongle_mode.curr_mode);

    if (APP_DONGLE_CM_DONGLE_MODE_NONE == ctrl_ctx->dongle_mode.curr_mode) {

    }

    app_dongle_cm_source_info_t *start_source_info = app_dongle_cm_get_source_info(start_source_type);
    app_dongle_cm_source_info_t *stop_source_info = app_dongle_cm_get_source_info(stop_source_type);

    bt_addr_t bt_addr = {0};
    //bt_key_t sirk = {0};
    app_dongle_cm_start_source_param_t start_param =  {0};
    app_dongle_cm_stop_source_param_t stop_param = {0};
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_switch_dongle_mode, curr_mode: %d, next_mode: %d", 2, ctrl_ctx->dongle_mode.curr_mode, mode);
    if (mode == ctrl_ctx->dongle_mode.curr_mode) {
        app_dongle_cm_notify_switch_dongle_mode_result(BT_STATUS_SUCCESS, mode);
        return BT_STATUS_SUCCESS;
    }
    if (!start_source_info) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Source info is null!", 1, mode);
        return BT_STATUS_FAIL;
    }
    if (!(ctrl_ctx->link_mode & (1 << start_source_type)) || !start_source_info->source_registed) {
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Link mode not support or not registed!", 1, mode);
        return BT_STATUS_FAIL;
    }
    if (start_source_type == stop_source_type && APP_DONGLE_CM_SOURCE_LEA == start_source_type) {
        if (APP_DONGLE_CM_DONGLE_MODE_LEA_CIS == mode) {
        start_param.lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;
/*
            uint8_t le_reconnect = app_dongle_cm_read_recon_list_from_le_bonded_info(0, true);
            if (le_reconnect) {
               status = app_dongle_cm_set_white_list();
               if (status) {
                   APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Set wl error!", 1, mode);
                   return BT_STATUS_FAIL;
               }
               app_dongle_cm_start_timer(APP_DONGLE_CM_TIMER_ID_BONDING_LIST_SCAN, 6000, app_dongle_cm_sirk_scan_timeout_handler, NULL);
               status = app_dongle_cm_le_start_scan_device_internal(true);
            } else if (app_dongle_cm_memcmp(&ctrl_ctx->sirk, &sirk, sizeof(bt_key_t))) {
               ctrl_ctx->le_scan.use_sirk = true;
               status = app_dongle_cm_le_start_scan_device_internal(false);
            } else {
               status = app_dongle_cm_le_start_scan_device_internal(false);
            }
            if (status) {
               APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start scan error!", 1, mode);
               return BT_STATUS_FAIL;
            }
            app_dongle_cm_set_source_conn_state(start_source_info, APP_DONGLE_CM_STATE_START_SCAN_INQUIRY);
            return status;
*/
        } else {
            start_param.lea_mode = APP_DONGLE_CM_LEA_MODE_BIS;
        }

        if (start_source_info->cm_handle.start_source) {
            status = start_source_info->cm_handle.start_source(bt_addr, start_param);
            if (status) {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start LEA error!", 1, mode);
                return BT_STATUS_FAIL;
            } else {
                app_dongle_cm_set_source_conn_state(start_source_info, APP_DONGLE_CM_STATE_START_CONNECTION);
            }
        } else {
            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"start_source is null!", 1, mode);
            return BT_STATUS_FAIL;
        }
        return status;

    } else if (APP_DONGLE_CM_SOURCE_ULL_V2 == stop_source_type ) {
            app_dongle_cm_set_ull_v2_feature_bit(false);
    }
    if (APP_DONGLE_CM_SOURCE_ULL_V2 == start_source_type) {
        app_dongle_cm_set_ull_v2_feature_bit(true);
    }

    if (start_source_type != stop_source_type) {
        /*stop source*/
        if (NULL != stop_source_info && APP_DONGLE_CM_STATE_DISCONNECTED < stop_source_info->conn_state && \
           stop_source_info->cm_handle.stop_source) {
            APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_switch_dongle_mode, state: %d", 1, stop_source_info->conn_state);
            ctrl_ctx->dongle_mode.is_switch_dongle_mode = true;
            ctrl_ctx->dongle_mode.next_mode = mode;
            if (APP_DONGLE_CM_STATE_CONNECTED == stop_source_info->conn_state) {
               app_dongle_cm_memcpy(&bt_addr, &stop_source_info->connect_addr, sizeof(bt_bd_addr_t));
               status = stop_source_info->cm_handle.stop_source(bt_addr, stop_param);
               if (BT_STATUS_SUCCESS == status) {
                   app_dongle_cm_set_source_conn_state(stop_source_info, APP_DONGLE_CM_STATE_STOP_CONNECTION);
               } else {
                   APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Stop source error-1!", 1, mode);
               }
            } else if (APP_DONGLE_CM_STATE_STOP_CONNECTION == stop_source_info->conn_state) {
               APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Wait for stop complete event back.", 1, mode);
            } else {
               APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error state-1!", 1, mode);
            }
            if (BT_STATUS_SUCCESS != status) {
               ctrl_ctx->dongle_mode.is_switch_dongle_mode = false;
               ctrl_ctx->dongle_mode.next_mode = APP_DONGLE_CM_DONGLE_MODE_NONE;
            }
        } else {
            if (!stop_source_info) {
                uint8_t i = 0;
                for (i = 0; i < APP_DONGLE_CM_SOURCE_MAX; i ++) {
                    stop_source_info = app_dongle_cm_get_source_info(i);
                    if (APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == stop_source_info->conn_state) {
                        app_dongle_cm_le_stop_scan_device_internal();
                        app_dongle_cm_set_source_conn_state(stop_source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
                    } else if (APP_DONGLE_CM_STATE_START_CONNECTION == stop_source_info->conn_state) {
                        app_dongle_cm_memcpy(&bt_addr, &stop_source_info->connect_addr, sizeof(bt_bd_addr_t));
                        if (NULL != stop_source_info->cm_handle.stop_source && \
                            BT_STATUS_SUCCESS == stop_source_info->cm_handle.stop_source(bt_addr, stop_param)) {
                            app_dongle_cm_set_source_conn_state(stop_source_info, APP_DONGLE_CM_STATE_STOP_CONNECTION);
                        } else {
                            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Stop source error-2!", 1, mode);
                            return BT_STATUS_FAIL;
                        }
                    } else {
                         APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error state-2!", 1, mode);
                    }
                }
            }
            status = app_dongle_cm_switch_to_next_dongle_mode(mode);
        }
    }else {
        app_dongle_cm_notify_switch_dongle_mode_result(BT_STATUS_SUCCESS, mode);
        APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"Next mode and current mode is same!", 1, mode);
    }

    return status;
}

bt_status_t app_dongle_cm_le_start_scan_device(void)
{
    bt_status_t status;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_source_info_t *source_info = NULL;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_le_start_scan_device", 0);
    app_dongle_cm_le_stop_scan_device_internal();
    ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_FULL_SCAN_FOR_USER;
    if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) {
        source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
        if (source_info->source_registed) {
            app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_SCAN_INQUIRY);
        }
    }
    if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
        source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);
        if (source_info->source_registed) {
            app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_SCAN_INQUIRY);
        }
    }
    status = app_dongle_cm_le_start_scan_device_internal(false);
    if (BT_STATUS_FAIL == status) {
        ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
        if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) {
            source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
            if (source_info->source_registed && APP_DONGLE_CM_STATE_DISCONNECTED == source_info->conn_state) {
                app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            }
        }
        if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
            source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);
            if (source_info->source_registed && APP_DONGLE_CM_STATE_DISCONNECTED == source_info->conn_state) {
                app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            }
        }

    }
    return status;
}

bt_status_t app_dongle_cm_le_stop_scan_device(void)
{
    bt_status_t status;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_le_stop_scan_device", 0);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    ctrl_ctx->connect_style_le = APP_DONGLE_CM_CONNECTION_USING_UNKNOWN;
    status = app_dongle_cm_le_stop_scan_device_internal();
    if (!status) {
        app_dongle_cm_source_info_t *source_info = NULL;
        if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_ULL_V2) {
            source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_ULL_V2);
            if (source_info->source_registed && APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info->conn_state) {
                app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            }
        }
        if (ctrl_ctx->link_mode & APP_DONGLE_CM_LINK_MODE_LEA) {
            source_info = app_dongle_cm_get_source_info(APP_DONGLE_CM_SOURCE_LEA);
            if (source_info->source_registed && APP_DONGLE_CM_STATE_START_SCAN_INQUIRY == source_info->conn_state) {
                app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_DISCONNECTED);
            }
        }

    }

    return status;

}

bt_status_t app_dongle_cm_le_create_connection(app_dongle_cm_source_t source_type, bt_addr_t addr)
{
    bt_status_t status;
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_source_info_t *source_info = app_dongle_cm_get_source_info(source_type);
    app_dongle_cm_source_info_t *temp_source_info = NULL;
    uint8_t i = 0;
    bool is_multi_link = false;
    if (APP_DONGLE_CM_SOURCE_ULL_V2 == source_type) {
        app_dongle_cm_set_ull_v2_feature_bit(true);
    } else {
        app_dongle_cm_set_ull_v2_feature_bit(false);
    }

    for (i =0; i < APP_DONGLE_CM_SOURCE_MAX; i ++) {
        temp_source_info = app_dongle_cm_get_source_info(i);
        if (source_type != i) {
            if (APP_DONGLE_CM_STATE_CONNECTED == temp_source_info->conn_state) {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_le_create_connection, no resource", 0);
                return BT_STATUS_FAIL;
            }
        } else {
            if (APP_DONGLE_CM_STATE_CONNECTED == temp_source_info->conn_state) {
                if (APP_DONGLE_CM_SOURCE_LEA != source_type) {
                    APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_le_create_connection, no resource, type: %d", 1, source_type);
                    return BT_STATUS_FAIL;
                }
                is_multi_link = true;
                APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_le_create_connection, connect second link", 0);
            }
        }
    }

    if (((1 << source_type) & ctrl_ctx->link_mode) && source_info) {
        if (source_info->source_registed &&
            source_info->cm_handle.start_source) {
            app_dongle_cm_start_source_param_t param = {0};
            if (APP_DONGLE_CM_SOURCE_LEA == source_type) {
                param.lea_mode = APP_DONGLE_CM_LEA_MODE_CIS;
            }

            status = source_info->cm_handle.start_source(addr, param);
            if (status) {
                APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Start source fail, type: 0x%x!", 1, APP_DONGLE_CM_SOURCE_ULL_V2);
                return status;
            }
            if (!is_multi_link) {
                app_dongle_cm_set_source_conn_state(source_info, APP_DONGLE_CM_STATE_START_CONNECTION);
                app_dongle_cm_memcpy(&source_info->connect_addr, &addr, sizeof(bt_addr_t));
                APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"START Source success!!", 0);
            }
        } else {
            APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error Source Info!!", 0);
            status = BT_STATUS_FAIL;
        }
    } else {
        status = BT_STATUS_FAIL;
        APPS_LOG_MSGID_E(APP_DONGLE_CM_LOG_TAG"Error Link Mode(ULL v2) or ULL v2 not open!!", 0);
    }
    return status;
}

bt_status_t app_dongle_cm_le_disconnect(app_dongle_cm_source_t source_type, bt_addr_t addr)
{
    bt_status_t status;
    APPS_LOG_MSGID_I(APP_DONGLE_CM_LOG_TAG"app_dongle_cm_le_disconnect, source_type: %d", 1, source_type);
    app_dongle_cm_ctrl_contex_t *ctrl_ctx = app_dongle_cm_get_ctrl_context();
    app_dongle_cm_source_info_t *source_info = app_dongle_cm_get_source_info(source_type);
    if (((1 << source_type) & ctrl_ctx->link_mode) && source_info) {
        if (source_info->source_registed &&
            source_info->cm_handle.stop_source) {
            bt_addr_t bt_addr = {0};
            app_dongle_cm_stop_source_param_t param = {0};
            app_dongle_cm_memcpy(&bt_addr, &source_info->connect_addr, sizeof(bt_addr_t));

            status = source_info->cm_handle.stop_source(bt_addr, param);
        } else {
            status = BT_STATUS_FAIL;
        }
    } else {
        status = BT_STATUS_FAIL;
    }

    return status;

}

void app_dongle_cm_le_register_race_callback(app_dongle_le_race_event_callback_t callback)
{
    g_dongle_le_cm_race_callback = callback;
}


