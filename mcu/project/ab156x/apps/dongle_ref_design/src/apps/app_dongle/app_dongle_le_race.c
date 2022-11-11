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
#if (defined MTK_RACE_CMD_ENABLE)

#include "race_cmd.h"
#include "race_xport.h"
#include "race_noti.h"
#include "bt_gap_le.h"
#include "apps_race_cmd_event.h"
#include "ble_air_interface.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "bt_type.h"
#include "apps_debug.h"

#include "app_le_audio_air.h"
#include "app_dongle_connection_common.h"
#include "app_dongle_le_race.h"
#if (defined AIR_LE_AUDIO_UNICAST_ENABLE)
#include "app_le_audio_race.h"
#include "app_le_audio_ucst.h"
#endif
#if defined (AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
#include "app_ull_dongle_le.h"
#endif



#define LOG_TAG        "[app_dongle_le_race]"

typedef struct {
    uint8_t channel;
    race_pkt_t race_pkt;
} PACKED app_dongle_le_race_event_ui_data_t;

typedef struct {
    app_dongle_le_race_sink_device_t current_sink_device;
} app_dongle_le_race_contex_t;

static app_dongle_le_race_contex_t g_le_race_ctx;
static app_dongle_le_race_contex_t * app_dongle_le_race_get_ctx(void) {
    return &g_le_race_ctx;
}
static void app_dongle_le_race_event_callback(app_dongle_le_race_event_t event, app_dongle_le_race_sink_device_t sink_type, void *buff)
{
    RACE_ERRCODE race_notify_ret;

    APPS_LOG_MSGID_I(LOG_TAG" app_dongle_le_race_event_callback event: %d, buff : 0x%x, sink type: %d", 3, event, buff, sink_type);

    switch (event) {
        case APP_DONGLE_LE_RACE_EVT_ADV_REPORT: {
            if (buff) {
                bt_gap_le_ext_advertising_report_ind_t *report_ind = (bt_gap_le_ext_advertising_report_ind_t *)buff;
                uint8_t *name = NULL;
                uint32_t name_len = 0;
                uint32_t cursor_position = 0;
                for (; cursor_position + 2 < report_ind->data_length;) {
                    if (report_ind->data[cursor_position + 1] == BT_GAP_LE_AD_TYPE_NAME_COMPLETE) {
                        name = &(report_ind->data[cursor_position + 2]);
                        name_len = report_ind->data[cursor_position] - 1;
                        break;
                    } else {
                        cursor_position += report_ind->data[cursor_position] + 1;
                    }
                }
                app_dongle_le_adv_report_notify_t *noti = RACE_ClaimPacket((uint8_t)RACE_TYPE_NOTIFICATION,
                                                                 (uint16_t)RACE_DONGLE_LE_ADV_REPORT_NOTIFY,
                                                                 (uint16_t)(sizeof(app_dongle_le_adv_report_notify_t) + name_len),
                                                                 RACE_SERIAL_PORT_TYPE_USB);
                if (noti != NULL) {
                    memcpy(&noti->addr, &report_ind->address, sizeof(bt_addr_t));
                    noti->status = 0; /* Currently not useful. */
                    noti->rssi = report_ind->rssi;
                    if (name && name_len > 0) {
                        memcpy(noti->name, name, name_len);
                    }
                    noti->sink_type = sink_type;
                    race_notify_ret = race_noti_send(noti, RACE_SERIAL_PORT_TYPE_USB, false);
                    APPS_LOG_MSGID_I(LOG_TAG"notify_adv_report resule: 0x%X.", 1, race_notify_ret);
                    if (race_notify_ret != RACE_ERRCODE_SUCCESS) {
                        RACE_FreePacket((void *)noti);
                    }
                }
            }
            break;
        }
        case APP_DONGLE_LE_RACE_EVT_CONNECT_IND: {
            if (buff) {
                app_dongle_le_race_connect_ind_t *connect_ind = (app_dongle_le_race_connect_ind_t *)buff;
                app_dongle_le_race_connect_notify_t *noti = RACE_ClaimPacket((uint8_t)RACE_TYPE_NOTIFICATION,
                                                              (uint16_t)RACE_DONGLE_LE_CONNECT_NOTIFY,
                                                              (uint16_t)(sizeof(app_dongle_le_race_connect_notify_t)),
                                                              RACE_SERIAL_PORT_TYPE_USB);
                if (noti != NULL) {
                    noti->status = (connect_ind->ret == BT_STATUS_SUCCESS) ? 0 : 0xFF;; /* Currently not useful. */
                    noti->connected = APP_DONGLE_LE_RACE_CONNECT_EVENT_CONNECTED;
                    memcpy(&noti->addr, &connect_ind->peer_addr, sizeof(bt_addr_t));
                    noti->device_id = ble_air_get_device_id_by_address(&connect_ind->peer_addr.addr);
                    noti->group_id = connect_ind->group_id;
                    noti->sink_type = sink_type;
                    race_notify_ret = race_noti_send(noti, RACE_SERIAL_PORT_TYPE_USB, false);
                    APPS_LOG_MSGID_I(LOG_TAG"notify_le_audio_connected resule: 0x%X.", 1, race_notify_ret);
                    if (race_notify_ret != RACE_ERRCODE_SUCCESS) {
                        RACE_FreePacket((void *)noti);
                    }
                }
            }
            break;
        }
        case APP_DONGLE_LE_RACE_EVT_DISCONNECT_IND: {
            if (buff) {
                app_dongle_le_race_disconnect_ind_t *disconnect_ind = (app_dongle_le_race_disconnect_ind_t *)buff;
                app_dongle_le_race_connect_notify_t *noti = RACE_ClaimPacket((uint8_t)RACE_TYPE_NOTIFICATION,
                                                              (uint16_t)RACE_DONGLE_LE_CONNECT_NOTIFY,
                                                              (uint16_t)(sizeof(app_dongle_le_race_connect_notify_t)),
                                                              RACE_SERIAL_PORT_TYPE_USB);
                if (noti != NULL) {
                    noti->status = 0; /* Currently not useful. */
                    noti->connected = APP_DONGLE_LE_RACE_CONNECT_EVENT_DISCONNECTED;
                    memcpy(&noti->addr, &disconnect_ind->peer_addr, sizeof(bt_addr_t));
                    noti->device_id = BT_AIR_DEVICE_ID_INVAILD;
                    noti->sink_type = sink_type;
                    race_notify_ret = race_noti_send(noti, RACE_SERIAL_PORT_TYPE_USB, true);
                    APPS_LOG_MSGID_I(LOG_TAG"notify_le_audio_disconnected resule: 0x%X.", 1, race_notify_ret);
                    if (race_notify_ret != RACE_ERRCODE_SUCCESS) {
                        RACE_FreePacket((void *)noti);
                    }
                }
            }
            break;
        }
        default:
            break;
    }
}

bool app_dongle_le_race_cmd_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;
    RACE_ERRCODE race_ret;
    bt_status_t bt_status = BT_STATUS_FAIL;
    app_dongle_le_race_contex_t *ctx = app_dongle_le_race_get_ctx();
    if (extra_data && data_len >= sizeof(app_dongle_le_race_event_ui_data_t)) {
        app_dongle_le_race_event_ui_data_t *race_data = (app_dongle_le_race_event_ui_data_t *)extra_data;
        APPS_LOG_MSGID_I(LOG_TAG" app_dongle_le_race_cmd_event_proc, CMD = %x", 1, race_data->race_pkt.hdr.id);
        switch (race_data->race_pkt.hdr.id) {
            case RACE_DONGLE_LE_SET_LINK_TYPE: {
                app_dongle_le_race_set_link_type_cmd_t *set_link_type = (app_dongle_le_race_set_link_type_cmd_t *) & (race_data->race_pkt.payload);

                APPS_LOG_MSGID_I(LOG_TAG"Set link type: 0x%X.", 1, set_link_type->type);
                ctx->current_sink_device = set_link_type->type;
                app_dongle_le_race_set_link_type_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                          (uint16_t)RACE_DONGLE_LE_SET_LINK_TYPE,
                                                                          (uint16_t)(sizeof(app_dongle_le_race_set_link_type_rsp_t)),
                                                                          race_data->channel);
                if (response != NULL) {
                    response->status = 0;
                    response->type = set_link_type->type;
                    race_ret = race_noti_send(response, race_data->channel, false);
                    APPS_LOG_MSGID_I(LOG_TAG"send rsp of set link type resule: 0x%X.", 1, race_ret);
                    if (race_ret != RACE_ERRCODE_SUCCESS) {
                        RACE_FreePacket((void *)response);
                    }
                }
                break;
            }
            case RACE_DONGLE_LE_SCAN: {
                app_dongle_le_race_scan_cmd_t *scan_cmd_data = (app_dongle_le_race_scan_cmd_t *) & (race_data->race_pkt.payload);

                if (scan_cmd_data->start_scan) {
                    bt_status = app_dongle_cm_le_start_scan_device();
                    if (bt_status == BT_STATUS_SUCCESS) {
                        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LE_SCAN_END);
                        if (scan_cmd_data->timeout_seconds) {
                            ui_shell_send_event(false, EVENT_PRIORITY_HIGHEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                APPS_EVENTS_INTERACTION_LE_SCAN_END, NULL, 0, NULL, scan_cmd_data->timeout_seconds * 1000);
                        }
                    }
                } else {
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LE_SCAN_END);
                    bt_status = app_dongle_cm_le_stop_scan_device();
                }
                APPS_LOG_MSGID_I(LOG_TAG" app_dongle_le_race_cmd_event_proc, start scan(%d), bt_ret = 0x%x, timeout = %d",
                                  3, scan_cmd_data->start_scan, bt_status, scan_cmd_data->timeout_seconds);
                app_dongle_le_race_scan_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                          (uint16_t)RACE_DONGLE_LE_SCAN,
                                                                          (uint16_t)(sizeof(app_dongle_le_race_scan_rsp_t)),
                                                                          race_data->channel);
                if (response != NULL) {
                    response->status = (bt_status == BT_STATUS_SUCCESS) ? 0 : 0xFF;
                    race_ret = race_noti_send(response, race_data->channel, false);
                    APPS_LOG_MSGID_I(LOG_TAG"send rsp of RACE_DONGLE_LE_SCAN resule: 0x%X.", 1, race_ret);
                    if (race_ret != RACE_ERRCODE_SUCCESS) {
                        RACE_FreePacket((void *)response);
                    }
                }
                break;
            }
            case RACE_DONGLE_LE_CONNECT: {
                app_dongle_le_race_connect_cmd_t *race_conn_data = (app_dongle_le_race_connect_cmd_t *) & (race_data->race_pkt.payload);
                bt_addr_t address;
                app_dongle_cm_source_t source_type = APP_DONGLE_CM_SOURCE_INVALID;
                memcpy(&address, &race_conn_data->addr, sizeof(bt_addr_t));
                if (APP_DONGLE_LE_RACE_SINK_DEVICE_LEA == ctx->current_sink_device) {
                    source_type = APP_DONGLE_CM_SOURCE_LEA;
                } else if (APP_DONGLE_LE_RACE_SINK_DEVICE_ULL_V2 == ctx->current_sink_device) {
                    source_type = APP_DONGLE_CM_SOURCE_ULL_V2;
                } else {
                    break;
                }
                if (race_conn_data->connect) {
                    bt_status = app_dongle_cm_le_create_connection(source_type, address);
                } else {
                    if (APP_DONGLE_LE_RACE_SINK_DEVICE_LEA == ctx->current_sink_device) {
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
                        bt_status = app_le_audio_ucst_disconnect_device(&address);
#endif
                    } else if (APP_DONGLE_LE_RACE_SINK_DEVICE_ULL_V2 == ctx->current_sink_device) {
                        bt_status = app_dongle_cm_le_disconnect(source_type, address);
                    }
                }
                APPS_LOG_MSGID_I(LOG_TAG" app_dongle_le_race_cmd_event_proc, sink type: %d, connect(%d), bt_ret = 0x%x",
                                  3, ctx->current_sink_device, race_conn_data->connect, bt_status);
                app_dongle_le_race_connect_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                             (uint16_t)RACE_DONGLE_LE_CONNECT,
                                                                             (uint16_t)(sizeof(app_dongle_le_race_connect_rsp_t)),
                                                                             race_data->channel);
                if (response != NULL) {
                    response->connect = race_conn_data->connect;
                    memcpy(&response->addr, &race_conn_data->addr, sizeof(bt_addr_t));
                    response->status = (bt_status == BT_STATUS_SUCCESS) ? 0 : 0xFF;
                    race_ret = race_noti_send(response, race_data->channel, false);
                    APPS_LOG_MSGID_I(LOG_TAG"send rsp of RACE_DONGLE_LE_CONNECT resule: 0x%X.", 1, race_ret);
                    if (race_ret != RACE_ERRCODE_SUCCESS) {
                        RACE_FreePacket((void *)response);
                    }
                }
                break;
            }
            case RACE_DONGLE_LE_GET_DEVICE_STATUS: {
#if (defined AIR_LE_AUDIO_UNICAST_ENABLE) || (defined AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
                app_dongle_le_race_get_device_status_cmd_t *race_get_status_cmd = (app_dongle_le_race_get_device_status_cmd_t *) & (race_data->race_pkt.payload);
#endif
                APPS_LOG_MSGID_I(LOG_TAG" get device status, sink type: 0x%x", 1, ctx->current_sink_device);
                if (APP_DONGLE_LE_RACE_SINK_DEVICE_LEA == ctx->current_sink_device) {
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
                    app_le_audio_race_get_device_status_handler(race_data->channel, race_get_status_cmd);
#else
                    RACE_ERRCODE race_ret;
                    app_dongle_le_race_get_device_status_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                                         (uint16_t)RACE_DONGLE_LE_GET_DEVICE_STATUS,
                                                                                         (uint16_t)(sizeof(app_dongle_le_race_get_device_status_rsp_t)),
                                                                                          race_data->channel);

                    if (response) {
                        response->status = 0xFF;
                        race_ret = race_noti_send(response, race_data->channel, false);
                        if (race_ret != RACE_ERRCODE_SUCCESS) {
                            RACE_FreePacket((void *)response);
                        }
                    }

#endif
                }else if (APP_DONGLE_LE_RACE_SINK_DEVICE_ULL_V2 == ctx->current_sink_device) {
                    //TODO
#if defined (AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
                    app_ull_dongle_le_get_device_status_handler(race_data->channel, race_get_status_cmd);
#else
                    RACE_ERRCODE race_ret;
                    app_dongle_le_race_get_device_status_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                                         (uint16_t)RACE_DONGLE_LE_GET_DEVICE_STATUS,
                                                                                         (uint16_t)(sizeof(app_dongle_le_race_get_device_status_rsp_t)),
                                                                                          race_data->channel);

                    if (response) {
                        response->status = 0xFF;
                        race_ret = race_noti_send(response, race_data->channel, false);
                        if (race_ret != RACE_ERRCODE_SUCCESS) {
                            RACE_FreePacket((void *)response);
                        }
                    }

#endif

                }

                break;
            }
            case RACE_DONGLE_LE_GET_DEVICE_LIST: {
                APPS_LOG_MSGID_I(LOG_TAG" get device list, sink type: 0x%x", 1, ctx->current_sink_device);

                if (APP_DONGLE_LE_RACE_SINK_DEVICE_LEA == ctx->current_sink_device) {
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
                    app_le_audio_race_get_device_list_handler(race_data->channel);
#else
                    RACE_ERRCODE race_ret;
                    app_dongle_le_race_get_device_list_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                                         (uint16_t)RACE_DONGLE_LE_GET_DEVICE_LIST,
                                                                                         (uint16_t)(sizeof(app_dongle_le_race_get_device_list_rsp_t)),
                                                                                          race_data->channel);

                    if (response) {
                        response->status = 0xFF;
                        race_ret = race_noti_send(response, race_data->channel, false);
                        if (race_ret != RACE_ERRCODE_SUCCESS) {
                            RACE_FreePacket((void *)response);
                        }
                    }

#endif
                } else if (APP_DONGLE_LE_RACE_SINK_DEVICE_ULL_V2 == ctx->current_sink_device) {
                    //TODO
#if defined (AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
                    app_ull_dongle_le_get_device_list_handler(race_data->channel);
#else
                    RACE_ERRCODE race_ret;
                    app_dongle_le_race_get_device_list_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                                         (uint16_t)RACE_DONGLE_LE_GET_DEVICE_LIST,
                                                                                         (uint16_t)(sizeof(app_dongle_le_race_get_device_list_rsp_t)),
                                                                                          race_data->channel);

                    if (response) {
                        response->status = 0xFF;
                        race_ret = race_noti_send(response, race_data->channel, false);
                        if (race_ret != RACE_ERRCODE_SUCCESS) {
                            RACE_FreePacket((void *)response);
                        }
                    }

#endif

                }

                break;
            }
            case RACE_DONGLE_LE_SWITCH_ACTIVE_DEVICE: {
                app_dongle_le_race_switch_active_audio_cmd_t *race_switch_device_cmd = (app_dongle_le_race_switch_active_audio_cmd_t *) & (race_data->race_pkt.payload);
                APPS_LOG_MSGID_I(LOG_TAG" switch active audio, sink type: 0x%x", 1, ctx->current_sink_device);
                if (APP_DONGLE_LE_RACE_SINK_DEVICE_LEA == ctx->current_sink_device) {
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
                    app_le_audio_race_switch_active_device_handler(race_data->channel, race_switch_device_cmd);
#else
                    RACE_ERRCODE race_ret;
                    app_dongle_le_race_switch_active_audio_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                                         (uint16_t)RACE_DONGLE_LE_SWITCH_ACTIVE_DEVICE,
                                                                                         (uint16_t)(sizeof(app_dongle_le_race_switch_active_audio_rsp_t)),
                                                                                          race_data->channel);

                    if (response) {
                        response->set_or_get = race_switch_device_cmd->set_or_get;
                        response->status = 0xFF;
                        response->group_id = 0x00;
                        race_ret = race_noti_send(response, race_data->channel, false);
                        if (race_ret != RACE_ERRCODE_SUCCESS) {
                            RACE_FreePacket((void *)response);
                        }
                    }

#endif
                } else if (APP_DONGLE_LE_RACE_SINK_DEVICE_ULL_V2 == ctx->current_sink_device) {
                    //TODO
#if defined (AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
                    app_ull_dongle_le_switch_active_device_handler(race_data->channel, race_switch_device_cmd);
#else
                    RACE_ERRCODE race_ret;
                    app_dongle_le_race_switch_active_audio_rsp_t *response = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                                                                         (uint16_t)RACE_DONGLE_LE_SWITCH_ACTIVE_DEVICE,
                                                                                         (uint16_t)(sizeof(app_dongle_le_race_switch_active_audio_rsp_t)),
                                                                                          race_data->channel);

                    if (response) {
                        response->set_or_get = race_switch_device_cmd->set_or_get;
                        response->status = 0xFF;
                        response->group_id = 0x00;
                        race_ret = race_noti_send(response, race_data->channel, false);
                        if (race_ret != RACE_ERRCODE_SUCCESS) {
                            RACE_FreePacket((void *)response);
                        }
                    }

#endif

                }
                break;
            }
            default:
                break;
        }
    }

    return ret;
}

void *app_dongle_le_race_cmd_handler(ptr_race_pkt_t p_race_package, uint16_t length, uint8_t channel_id)
{
    void *response = NULL;
    APPS_LOG_MSGID_I(LOG_TAG" app_dongle_le_race_cmd_handler, Race ID: 0x%x", 1, p_race_package->hdr.id);
    switch (p_race_package->hdr.id) {
        case RACE_DONGLE_LE_SCAN:
        case RACE_DONGLE_LE_ADV_REPORT:
        case RACE_DONGLE_LE_CONNECT:
        case RACE_DONGLE_LE_GET_DEVICE_STATUS:
        case RACE_DONGLE_LE_GET_DEVICE_LIST: 
        case RACE_DONGLE_LE_SWITCH_ACTIVE_DEVICE:
        case RACE_DONGLE_LE_SET_LINK_TYPE: {
            uint32_t data_len = sizeof(app_dongle_le_race_event_ui_data_t) + p_race_package->hdr.length - sizeof(p_race_package->hdr.id);
            app_dongle_le_race_event_ui_data_t *extra_data = (app_dongle_le_race_event_ui_data_t *)pvPortMalloc(data_len);
            if (extra_data) {
                memcpy(&extra_data->race_pkt, p_race_package, sizeof(race_pkt_t) + p_race_package->hdr.length - sizeof(p_race_package->hdr.id));
                extra_data->channel = channel_id;
                ui_shell_send_event(false, EVENT_PRIORITY_HIGHEST, EVENT_GROUP_UI_SHELL_APP_DONGLE_LE_RACE,
                                    0, (void *)extra_data, data_len, NULL, 0);
            }
            break;
        }
        default:
            break;
    }

    return response;
}

bool app_dongle_le_race_interaction_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_status_t bt_status;
    switch (event_id) {
        case APPS_EVENTS_INTERACTION_LE_SCAN_END: {
            bt_status = app_dongle_cm_le_stop_scan_device();
            APPS_LOG_MSGID_I(LOG_TAG" timeout, stop scan, bt_ret = 0x%x", 1, bt_status);
            ret = true;
            break;
        }
        default:
            break;
    }

    return ret;
}
void app_dongle_le_race_init(void)
{
    APPS_LOG_MSGID_I(LOG_TAG" app_dongle_le_race_init", 0);
    memset(&g_le_race_ctx, 0, sizeof (app_dongle_le_race_contex_t));
    g_le_race_ctx.current_sink_device = APP_DONGLE_LE_RACE_SINK_DEVICE_LEA;
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
    app_le_audio_ucst_register_callback(app_dongle_le_race_event_callback);
#endif
    app_dongle_cm_le_register_race_callback(app_dongle_le_race_event_callback);
#if defined (AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
    app_ull_dongle_le_register_race_callback(app_dongle_le_race_event_callback);
#endif
}

#endif
