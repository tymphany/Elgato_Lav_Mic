/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#ifdef MTK_RACE_CMD_ENABLE

#include "apps_race_cmd_event.h"

#include "apps_config_key_remapper.h"
#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_debug.h"
#include "bt_device_manager.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "apps_aws_sync_event.h"
#include "app_home_screen_idle_activity.h"
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_music_utils.h"
#endif
#include "app_dongle_le_race.h"
#include "app_dongle_race.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd.h"
#include "race_cmd_relay_cmd.h"
#include "race_noti.h"
#include "race_bt.h"
#endif

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"
#endif

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "bt_ull_service.h"
#endif
#include "app_dongle_common_idle_activity.h"


extern void vPortFree(void *);
extern void *pvPortMalloc(size_t);

#define LOG_TAG     "[apps_race_event]"

#define RACE_ID_APPLICATION_BEGIN 0x2C10
#define RACE_ID_APPLICATION_END 0x2CCF

#define RACE_ID_APP_POWER_SAVING_SET_CFG 0x2C15
#define RACE_ID_APP_POWER_SAVING_GET_CFG 0x2C16

#define RACE_ID_APP_IN_EAR_SET_MUSIC_CFG 0x2C17
#define RACE_ID_APP_IN_EAR_GET_MUSIC_CFG 0x2C18

#ifdef MTK_RACE_CMD_ENABLE
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
static void *apps_race_cmd_power_saving(ptr_race_pkt_t p_race_package, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU Hdr;
        uint16_t enable_flag;
        uint16_t timeout;
        uint16_t silence_detect_enable;
        uint16_t silence_detect_timeout;
    } PACKED power_saving_cfg_set_req;

    typedef struct {
        uint8_t result;
    } PACKED RSP;

    typedef struct {
        uint8_t result;
        uint16_t enable_flag;
        uint16_t timeout;
        uint16_t silence_detect_enable;
        uint16_t silence_detect_timeout;
    } PACKED QUERY_RSP;

    void *pEvt = NULL;

    switch (p_race_package->hdr.id) {
        /* Nnable music control */
        case RACE_ID_APP_POWER_SAVING_SET_CFG: {
            power_saving_cfg_set_req *pcmd = (power_saving_cfg_set_req *)p_race_package;
            pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                         RACE_TYPE_RESPONSE,
                                         RACE_ID_APP_POWER_SAVING_SET_CFG,
                                         sizeof(RSP),
                                         channel_id);
            if (pEvt) {
                ((RSP *)pEvt)->result = RACE_ERRCODE_FAIL;
                if (pcmd == NULL) {
                    return pEvt;
                }

                /* sync msg to partner. */
#ifdef MTK_AWS_MCE_ENABLE
                RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    ret = bt_send_aws_mce_race_cmd_data(p_race_package, pcmd->Hdr.length + (uint32_t) & (((RACE_COMMON_HDR_STRU *)0)->id), channel_id, RACE_CMD_RELAY_FROM_AGENT, 0);
                    if (RACE_ERRCODE_SUCCESS != ret) {
                        APPS_LOG_MSGID_I(LOG_TAG"set power saving cfg, sync to partner failed,", 0);
                        ((RSP *)pEvt)->result = ret;
                        return pEvt;
                    }
                }
#endif
                uint16_t data_len = pcmd->Hdr.length - 2;
                app_power_saving_cfg cfg;
                memset(&cfg, 0, sizeof(cfg));
                if (data_len >= 2 * sizeof(uint16_t)) {
                    cfg.power_saving_enable = pcmd->enable_flag;
                    cfg.timeout = pcmd->timeout;
                } else {
                    return pEvt;
                }
                if (data_len >= 4 * sizeof(uint16_t)) {
                    /* Old version not support silence detect. so only 2 type.*/
                    cfg.silence_detect_enable = pcmd->silence_detect_enable;
                    cfg.silence_detect_timeout = pcmd->silence_detect_timeout;
                }
                /* Notify app the cfg updated. */
                APPS_LOG_MSGID_I(LOG_TAG"set power saving cfg, en: %d, t:%d, silence_en: %d, t: %d, len:%d", 5, cfg.power_saving_enable, cfg.timeout, cfg.silence_detect_enable, cfg.silence_detect_timeout, length);
                if ((pcmd->enable_flag && pcmd->timeout < 1) || (cfg.silence_detect_enable && cfg.silence_detect_timeout < 1)) {
                    return pEvt;
                }
                int32_t r = app_power_save_utils_set_cfg(&cfg);
                if (r == 0) {
                    app_power_save_utils_cfg_updated_notify();
                    ((RSP *)pEvt)->result = RACE_ERRCODE_SUCCESS;
                } else {
                    return pEvt;
                }
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
            }
            break;
        }
        /* Get the music control state */
        case RACE_ID_APP_POWER_SAVING_GET_CFG: {
            pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                         RACE_TYPE_RESPONSE,
                                         RACE_ID_APP_POWER_SAVING_GET_CFG,
                                         sizeof(QUERY_RSP),
                                         channel_id);
            if (pEvt) {
                app_power_saving_cfg *cfg = app_power_saving_get_cfg();
                ((QUERY_RSP *)pEvt)->result = RACE_ERRCODE_SUCCESS;
                ((QUERY_RSP *)pEvt)->enable_flag = cfg->power_saving_enable;
                ((QUERY_RSP *)pEvt)->timeout = cfg->timeout;
                ((QUERY_RSP *)pEvt)->silence_detect_enable = cfg->silence_detect_enable;
                ((QUERY_RSP *)pEvt)->silence_detect_timeout = cfg->silence_detect_timeout;
                APPS_LOG_MSGID_I(LOG_TAG"get power saving cfg, en: %d, t:%d", 2, cfg->power_saving_enable, cfg->timeout);
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
            }
            break;
        }
    }

    return pEvt;
}
#endif

#ifdef MTK_IN_EAR_FEATURE_ENABLE
static void *apps_race_cmd_in_ear(ptr_race_pkt_t p_race_package, uint16_t length, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU Hdr;
        uint8_t req[1];
    } PACKED in_ear_cfg_set_req;

    typedef struct {
        uint8_t result;
    } PACKED RSP;

    typedef struct {
        uint8_t result;
        uint8_t enable;
    } PACKED QUERY_RSP;

    void *pEvt = NULL;

    switch (p_race_package->hdr.id) {
        /* Nnable music control */
        case RACE_ID_APP_IN_EAR_SET_MUSIC_CFG: {
            in_ear_cfg_set_req *pcmd = (in_ear_cfg_set_req *)p_race_package;
            pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                         RACE_TYPE_RESPONSE,
                                         RACE_ID_APP_IN_EAR_SET_MUSIC_CFG,
                                         sizeof(RSP),
                                         channel_id);
            if (pEvt) {
                ((RSP *)pEvt)->result = RACE_ERRCODE_FAIL;
                /* sync msg to partner. */
#ifdef MTK_AWS_MCE_ENABLE
                RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    ret = bt_send_aws_mce_race_cmd_data(p_race_package, sizeof(RACE_COMMON_HDR_STRU) + 1, channel_id, RACE_CMD_RELAY_FROM_AGENT, 0);
                    if (RACE_ERRCODE_SUCCESS != ret) {
                        APPS_LOG_MSGID_I(LOG_TAG"set music ctrl, sync to partner failed,", 0);
                        ((RSP *)pEvt)->result = ret;
                        return pEvt;
                    }
                }
#endif
                /* Notify app the cfg updated. */
                uint8_t *p_data = (uint8_t *)pvPortMalloc(sizeof(uint8_t));
                if (p_data != NULL) {
                    ui_shell_status_t status = UI_SHELL_STATUS_OK;
                    *p_data = pcmd->req[0];
                    status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                 APPS_EVENTS_INTERACTION_IN_EAR_CFG_UPDATE,
                                                 p_data, sizeof(uint8_t), NULL, 0);
                    if (status != UI_SHELL_STATUS_OK) {
                        vPortFree(p_data);
                        return pEvt;
                    }
                    ((RSP *)pEvt)->result = RACE_ERRCODE_SUCCESS;
                }
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
            }

            break;
        }
        /* Get the music control state */
        case RACE_ID_APP_IN_EAR_GET_MUSIC_CFG: {
            pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                         RACE_TYPE_RESPONSE,
                                         RACE_ID_APP_IN_EAR_GET_MUSIC_CFG,
                                         sizeof(QUERY_RSP),
                                         channel_id);
            if (pEvt) {
                ((QUERY_RSP *)pEvt)->result = RACE_ERRCODE_SUCCESS;
                uint8_t temp_music_in_ear_cfg = app_music_get_in_ear_control_state();
                uint8_t enabled = temp_music_in_ear_cfg ? 1 : 0;
                ((QUERY_RSP *)pEvt)->enable = enabled;
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
            }
            break;
        }
    }

    return pEvt;
}
#endif
#endif
void app_race_send_notify(uint16_t cfg_type, int8_t *data, uint32_t len)
{
    uint8_t channel_id;
    RACE_ERRCODE race_notify_ret;
    uint32_t serial_port;
    serial_port_dev_t dev;
    bt_handle_t handle;

    typedef struct {
        uint16_t config_type;
        uint8_t data[1];
    } PACKED NOTI;

    handle = race_bt_get_ble_conn_hdl();
    APPS_LOG_MSGID_I(LOG_TAG" send notiy, type=0x%04X data=0x%8X len=%d", 3, cfg_type, data, len);

    if (handle != BT_HANDLE_INVALID) {
        dev = SERIAL_PORT_DEV_BT_LE;
    } else {
        dev = SERIAL_PORT_DEV_BT_SPP;
    }

    serial_port = race_get_serial_port_handle(dev);

    channel_id = (uint8_t)race_get_channel_id_by_port_handle(serial_port);
    NOTI *noti = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND_WITHOUT_RSP,
                                  (uint16_t)RACE_SET_APP_COMMON_CONFIG,
                                  (uint16_t)(sizeof(uint16_t) + len),
                                  channel_id);
    if (noti == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG"claim packet failed.", 0);
        return;
    }

    noti->config_type = cfg_type;
    memcpy(&noti->data[0], data, len);

    race_notify_ret = race_noti_send(noti, channel_id, false);
    APPS_LOG_MSGID_I(LOG_TAG"notify data resule: %d.", 1, race_notify_ret);
    if (race_notify_ret != RACE_ERRCODE_SUCCESS) {
        RACE_FreePacket((void *)noti);
    }
}

void app_race_notify_mmi_state(uint32_t mmi_state)
{
    /* If connected ULL dongle, need notify to dongle->HID->PC. */
    uint8_t dev[2] = { 0 };
    uint32_t i;
    uint32_t dev_count = 0;
    uint8_t channel_id;
    RACE_ERRCODE race_notify_ret;

    typedef struct {
        uint8_t mmi_state;
    } PACKED NOTI;
#if defined(AIR_USB_ENABLE) && defined(AIR_USB_HID_ENABLE)
    dev[dev_count] = RACE_SERIAL_PORT_TYPE_USB;
    dev_count++;
    APPS_LOG_MSGID_I(LOG_TAG"Add USB in notify mmi state port", 0);
#endif
    for (i = 0; i < dev_count; i++) {
        channel_id = race_get_channel_id_by_port_type(dev[i]);
        NOTI *noti = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND_WITHOUT_RSP,
                                      (uint16_t)RACE_GET_MMI_STATE,
                                      (uint16_t)(sizeof(NOTI)),
                                      channel_id);
        if (noti == NULL) {
            APPS_LOG_MSGID_E(LOG_TAG"notify_mmi_state[%d] claim packet failed", 1, i);
            return;
        }
        noti->mmi_state = mmi_state;
        race_notify_ret = race_noti_send(noti, channel_id, false);
        APPS_LOG_MSGID_I(LOG_TAG"notify_mmi_state resule: %d.", 1, race_notify_ret);
        if (race_notify_ret != RACE_ERRCODE_SUCCESS) {
            RACE_FreePacket((void *)noti);
        }
    }
}

static void *apps_race_cmd_handler(ptr_race_pkt_t p_race_package, uint16_t length, uint8_t channel_id)
{
    void *pEvt = NULL;
    APPS_LOG_MSGID_I(LOG_TAG"apps_race_cmd_handler, type[0x%X], id[0x%X]", 2, p_race_package->hdr.type, p_race_package->hdr.id);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    if (p_race_package->hdr.type == RACE_TYPE_COMMAND ||
        p_race_package->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP) {
        switch (p_race_package->hdr.id) {
            case RACE_GET_MMI_STATE: {
                typedef struct {
                    uint8_t mmi_state;
                } PACKED RSP;
                pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                             RACE_TYPE_RESPONSE,
                                             RACE_GET_MMI_STATE,
                                             sizeof(RSP),
                                             channel_id);
                if (pEvt) {
                    ((RSP *)pEvt)->mmi_state = apps_config_key_get_mmi_state();
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
                }
                break;
            }

            case RACE_SET_APP_COMMON_CONFIG: {
                typedef struct {
                    uint8_t status;
                    uint16_t config_type;
                } PACKED RSP;
                typedef struct {
                    RACE_COMMON_HDR_STRU cmdhdr;
                    uint16_t config_type;
                    uint8_t config_data[0];
                } PACKED CMD;
                CMD *cmd = (CMD *)p_race_package;
                pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                             RACE_TYPE_RESPONSE,
                                             RACE_SET_APP_COMMON_CONFIG,
                                             sizeof(RSP),
                                             channel_id);
                if (pEvt == NULL) {
                    APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
                    break;
                }

                uint16_t data_len = cmd->cmdhdr.length - 4;
                APPS_LOG_MSGID_I(LOG_TAG" race set app common config: handler_id=0x%04X, config_type=0x%04X, data_len=%d",
                                 3, p_race_package->hdr.id, cmd->config_type, data_len);
                switch (cmd->config_type) {
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                    case APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG: {
                        void *p_data = &(cmd->config_data);
                        if (pEvt) {
                            ((RSP *)pEvt)->status = RACE_ERRCODE_FAIL;
                            ((RSP *)pEvt)->config_type = APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG;
                            /* sync msg to partner. */
#ifdef MTK_AWS_MCE_ENABLE
                            RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
                            if (role == BT_AWS_MCE_ROLE_AGENT) {
                                ret = bt_send_aws_mce_race_cmd_data(p_race_package, p_race_package->hdr.length + (uint32_t) & (((RACE_COMMON_HDR_STRU *)0)->id), channel_id, RACE_CMD_RELAY_FROM_AGENT, 0);
                                if (RACE_ERRCODE_SUCCESS != ret) {
                                    APPS_LOG_MSGID_I(LOG_TAG"set power saving cfg, sync to partner failed. ", 0);
                                    ((RSP *)pEvt)->status = ret;
                                    break;
                                }
                                APPS_LOG_MSGID_I(LOG_TAG"set power saving cfg, sync to partner success. ", 0);
                            }
#endif
                            /* Notify app the cfg updated. */
                            app_power_saving_cfg cfg;
                            memset(&cfg, 0, sizeof(cfg));
                            if (data_len >= 2 * sizeof(uint16_t)) {
                                cfg.power_saving_enable = *(uint16_t *)p_data;
                                cfg.timeout = *((uint16_t *)p_data + 1);
                            }
                            if (data_len >= 4 * sizeof(uint16_t)) {
                                cfg.silence_detect_enable = *((uint16_t *)p_data + 2);
                                cfg.silence_detect_timeout = *((uint16_t *)p_data + 3);
                            }
                            APPS_LOG_MSGID_I(LOG_TAG" set power saving cfg, en: %d, t:%d, en_silence:%d, t:%d, len:%d",
                                             5, cfg.power_saving_enable, cfg.timeout, cfg.silence_detect_enable, cfg.silence_detect_timeout, length);

                            if ((cfg.power_saving_enable && cfg.timeout < 1) || (cfg.silence_detect_enable && cfg.silence_detect_timeout < 1)) {
                                break;
                            }

                            int32_t r = app_power_save_utils_set_cfg(&cfg);
                            if (r == 0) {
                                APPS_LOG_MSGID_I(LOG_TAG" set power saving cfg success. ", 0);
                                app_power_save_utils_cfg_updated_notify();
                                ((RSP *)pEvt)->status = RACE_ERRCODE_SUCCESS;
                            } else {
                                APPS_LOG_MSGID_I(LOG_TAG" set power saving cfg fail. ", 0);
                                ((RSP *)pEvt)->status = RACE_ERRCODE_FAIL;
                                break;
                            }
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG" RACE_ClaimPacketAppID return NULL. ", 0);
                        }
                        break;
                    }
#endif
                    case APPS_RACE_CMD_CONFIG_TYPE_USB_MODE: {
                        ((RSP *)pEvt)->config_type = APPS_RACE_CMD_CONFIG_TYPE_USB_MODE;
                        uint32_t mode = cmd->config_data[0];
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_SET_USB_MODE,
                                            (void *)mode, 0, NULL, 0);
                        ((RSP *)pEvt)->status = RACE_ERRCODE_SUCCESS;
                        break;
                    }
                    default:
                        if (pEvt) {
                            RACE_FreePacket(pEvt);
                            pEvt = NULL;
                        }
                        break;
                }
                break;
            }
            case RACE_GET_APP_COMMON_CONFIG: {
                typedef struct {
                    RACE_COMMON_HDR_STRU cmdhdr;
                    uint16_t config_type;
                } PACKED CMD;
                CMD *cmd = (CMD *)p_race_package;
                APPS_LOG_MSGID_I(LOG_TAG" race get app common config: handler_id=0x%04X, config_type=0x%04X",
                                 2, p_race_package->hdr.id, cmd->config_type);
                switch (cmd->config_type) {
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                    case APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG: {
                        typedef struct {
                            uint8_t status;
                            uint16_t config_type;
                            uint16_t enable_flag;
                            uint16_t timeout;
                            uint16_t silence_detect_enable;
                            uint16_t silence_detect_timeout;
                        } PACKED QUERY_RSP;
                        pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                                     RACE_TYPE_RESPONSE,
                                                     RACE_GET_APP_COMMON_CONFIG,
                                                     sizeof(QUERY_RSP),
                                                     channel_id);
                        if (pEvt != NULL) {
                            ((QUERY_RSP *)pEvt)->config_type = APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG;
                            app_power_saving_cfg *cfg = app_power_saving_get_cfg();
                            ((QUERY_RSP *)pEvt)->status = RACE_ERRCODE_SUCCESS;
                            ((QUERY_RSP *)pEvt)->enable_flag = cfg->power_saving_enable;
                            ((QUERY_RSP *)pEvt)->timeout = cfg->timeout;
                            ((QUERY_RSP *)pEvt)->silence_detect_enable = cfg->silence_detect_enable;
                            ((QUERY_RSP *)pEvt)->silence_detect_timeout = cfg->silence_detect_timeout;
                            APPS_LOG_MSGID_I(LOG_TAG"get power saving cfg, en: %d, t:%d, en_silence: %d, t:%d", 4, cfg->power_saving_enable, cfg->timeout, cfg->silence_detect_enable, cfg->silence_detect_timeout);
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG" get power saving pevt return NULL", 0);
                        }
                        break;
                    }
#endif
                    case APPS_RACE_CMD_CONFIG_TYPE_USB_MODE: {
                        typedef struct {
                            uint8_t status;
                            uint16_t config_type;
                            uint8_t mode;
                        } PACKED get_usb_mode_response_t;
                        uint8_t mode = app_dongle_common_idle_activity_get_current_mode();

                        pEvt = RACE_ClaimPacketAppID(p_race_package->hdr.pktId.field.app_id,
                                                     RACE_TYPE_RESPONSE,
                                                     RACE_GET_APP_COMMON_CONFIG,
                                                     sizeof(get_usb_mode_response_t),
                                                     channel_id);
                        if (pEvt) {
                            ((get_usb_mode_response_t *)pEvt)->status = RACE_ERRCODE_SUCCESS;
                            ((get_usb_mode_response_t *)pEvt)->config_type = APPS_RACE_CMD_CONFIG_TYPE_USB_MODE;
                            ((get_usb_mode_response_t *)pEvt)->mode = mode;
                        }
                        break;
                    }
                    default:
                        break;
                }

                break;
            }

                /* In-ear app set or get cfg. */
                /* In-ear app set or get cfg. */
#ifdef MTK_RACE_CMD_ENABLE
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            case RACE_ID_APP_IN_EAR_SET_MUSIC_CFG:
            case RACE_ID_APP_IN_EAR_GET_MUSIC_CFG:
                return apps_race_cmd_in_ear(p_race_package, length, channel_id);
#endif
                /* Power saving */
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            case RACE_ID_APP_POWER_SAVING_SET_CFG:
            case RACE_ID_APP_POWER_SAVING_GET_CFG:
                return apps_race_cmd_power_saving(p_race_package, length, channel_id);
#endif
#endif
#ifdef MTK_RACE_CMD_ENABLE
            case RACE_DONGLE_LE_SCAN:
            case RACE_DONGLE_LE_ADV_REPORT_NOTIFY:
            case RACE_DONGLE_LE_CONNECT:
            case RACE_DONGLE_LE_GET_DEVICE_STATUS:
            case RACE_DONGLE_LE_GET_DEVICE_LIST: 
            case RACE_DONGLE_LE_SWITCH_ACTIVE_DEVICE:
            case RACE_DONGLE_LE_SET_LINK_TYPE: {
                return app_dongle_le_race_cmd_handler(p_race_package, length, channel_id);
            }
#endif
            case RACE_ULL_DONGLE_CONNECT:
            case RACE_DONGLE_CONTROL_REMOTE:
#if (defined AIR_BLE_ULTRA_LOW_LATENCY_ENABLE) && (defined AIR_WIRELESS_MIC_ENABLE)
            case RACE_DONGLE_GET_REMOTE_RSSI:
#endif
            {
                return apps_dongle_race_cmd_handler(p_race_package, length, channel_id);
            }
            default:
                break;
        }
    }
    return pEvt;
}

#if defined AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "race_cmd_informational.h"
#include "bt_sink_srv_ami.h"
#include <stdlib.h>
#define SDK_VERSION_PREFIX   ("IoT_SDK_for_BT_Audio_V")
static void *apps_race_cmd_event_read_sdk_version_rsp_handler(ptr_race_pkt_t cmd, uint8_t channel_id)
{
    typedef struct {
        uint8_t length_of_read_bytes;
        char version_str[0];
    } PACKED race_read_sdk_version_rsp_t;
    if (cmd && cmd->hdr.type == RACE_TYPE_RESPONSE) {
        APPS_LOG_MSGID_I(LOG_TAG"received the sdk version: size: 0x%x", 1, cmd->hdr.length);
        if (cmd->hdr.length > sizeof(cmd->hdr.id) + sizeof(race_read_sdk_version_rsp_t)) {
            uint32_t str_len = cmd->hdr.length - sizeof(cmd->hdr.id) + sizeof(race_read_sdk_version_rsp_t);
            race_read_sdk_version_rsp_t *rsp_data = (race_read_sdk_version_rsp_t *)cmd->payload;
            /* Customer should remap the version here. Must implement version_id and codec_version. */
            uint32_t version_id = 0;
            uint32_t i;
            uint32_t version_end_position = str_len;
            if (memcmp(rsp_data->version_str, SDK_VERSION_PREFIX, sizeof(SDK_VERSION_PREFIX) - 1) == 0) {
                for (i = sizeof(SDK_VERSION_PREFIX) - 1; i < str_len; i++) {
                    if (rsp_data->version_str[i] != '.'
                        && (rsp_data->version_str[i] < '0' || rsp_data->version_str[i] > '9')) {
                        version_end_position = i;
                        break;
                    }
                }
                char *pure_version_str = (char *)pvPortMalloc(version_end_position - (sizeof(SDK_VERSION_PREFIX) - 1) + 1);
                if (pure_version_str) {
                    memcpy(pure_version_str,
                        rsp_data->version_str + sizeof(SDK_VERSION_PREFIX) - 1,
                        version_end_position - (sizeof(SDK_VERSION_PREFIX) - 1));
                    pure_version_str[version_end_position - (sizeof(SDK_VERSION_PREFIX) - 1)] = '\0';
                    APPS_LOG_I(LOG_TAG"pure_version is %s", pure_version_str);
                    char *saveptr = NULL;
                    char *param1;
                    param1 = strtok_r(pure_version_str, ".", &saveptr);
                    version_id |= (atoi(param1) & 0xFF) << 24;
                    for (i = 1; i < 4; i++) {
                        param1 = strtok_r(NULL, ".", &saveptr);
                        if (param1) {
                            version_id |= (atoi(param1) & 0xFF) << (24 - (i * 8));
                        } else {
                            break;
                        }
                    }
                    APPS_LOG_MSGID_I(LOG_TAG"received the sdk version: version_id : 0x%x", 1, version_id);
#if 0
                    codec_version = (version_id >= 0x03000000) ? 0 : 0x21073101;
                    bt_sink_srv_ami_audio_set_codec_config(AUDIO_SCENARIO_TYPE_GAMING_MODE_VOICE_DONGLE_USB_OUT,
                                                                                    AUDIO_DSP_CODEC_TYPE_OPUS,
                                                                                    (void *)codec_version,
                                                                                    0);
#endif
                    bt_ull_action(BT_ULL_ACTION_SET_VERSION, &version_id, sizeof(version_id));
                }
            }
            
        }
    }
    return NULL;
}

void apps_race_cmd_event_on_race_spp_connected(void)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    uint32_t channel_id = RACE_SERIAL_PORT_TYPE_SPP;//(uint8_t)race_get_channel_id_by_port_handle(SERIAL_PORT_DEV_BT_SPP);
    void *read_version_cmd = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND,
                                (uint16_t)RACE_READ_SDK_VERSION,
                                (uint16_t)(0),
                                channel_id);
    if (read_version_cmd == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG"claim read_version_cmd packet failed.", 0);
        return;
    }
    ret = race_flush_packet((uint8_t *)read_version_cmd, channel_id);
    
    APPS_LOG_MSGID_I(LOG_TAG"send RACE_READ_SDK_VERSION ret,0x%x", 1, ret);
    if (RACE_ERRCODE_SUCCESS != ret) {
        RACE_FreePacket(read_version_cmd);
    }
}
#endif

void apps_race_cmd_event_init(void)
{
#if defined(MTK_RACE_CMD_ENABLE)
    RACE_HANDLER handler = {
        .id_start = RACE_ID_APPLICATION_BEGIN,
        .id_end = RACE_ID_APPLICATION_END,
        .handler = apps_race_cmd_handler
    };
    race_status_t ret = RACE_Register_Handler(&handler);
    APPS_LOG_MSGID_I(LOG_TAG" app race register fail: ret=%d", 1, ret);
#if defined AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    race_cmd_information_register_read_sdk_version_rsp_callback(apps_race_cmd_event_read_sdk_version_rsp_handler);
#endif
#endif
}



#endif /* #ifdef MTK_RACE_CMD_ENABLE */
