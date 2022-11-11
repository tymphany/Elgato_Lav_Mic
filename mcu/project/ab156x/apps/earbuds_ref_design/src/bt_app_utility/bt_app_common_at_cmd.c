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


#include <stdlib.h>
#include "FreeRTOS.h"
#include "atci.h"

#include "bt_app_common.h"
#ifdef MTK_FOTA_VIA_RACE_CMD
#include "race_fota.h"
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
#include "mfi_coprocessor_api.h"
#endif

#if defined(SUPPORT_ROLE_HANDOVER_SERVICE)
#include "bt_role_handover.h"
#endif
#ifdef AIR_BT_FAST_PAIR_ENABLE
#include "app_fast_pair.h"
#endif
#include "bt_gap_le.h"
#include "bt_customer_config.h"
#include "bt_device_manager_le.h"
#include "bt_gatt_over_bredr.h"
#include "bt_callback_manager.h"
#include "bt_gattc_discovery.h"

#include "multi_ble_adv_manager.h"
#include "ui_shell_manager.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "nvkey.h"
#include "nvkey_id_list.h"

#if defined(AIR_LE_AUDIO_ENABLE) || defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
#include "app_lea_service_event.h"
#endif
#ifdef AIR_LE_AUDIO_ENABLE
#include "app_le_audio.h"
#include "app_lea_service_conn_mgr.h"
#include "bt_sink_srv_le_cap_stream.h"
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le.h"
#include "ble_csis.h"
#include "bt_le_audio_sink.h"
#include "bt_sink_srv_le_volume.h"
#include "app_lea_service.h"

#include "ble_bass.h"
#include "ble_bap.h"
#include "bt_sink_srv_le_volume.h"
#include "ble_pacs.h"
#include "ble_mics.h"
#include "ble_aics.h"
#include "app_lea_service_config.h"
#ifdef AIR_LE_AUDIO_HAPS_ENABLE
#include "ble_has_def.h"
#include "ble_haps.h"
#endif
#endif

#ifdef MTK_IAP2_PROFILE_ENABLE
#include "aacp_i2c.h"
#endif

extern uint8_t at_config_adv_flag;
extern bt_status_t bt_app_common_advtising_stop(void);

static atci_status_t bt_app_comm_at_cmd_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_nrpa_adv_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_fota_hdl(atci_parse_cmd_param_t *parse_cmd);
#if defined(MTK_AWS_MCE_ENABLE)
static atci_status_t bt_app_comm_at_cmd_rho_hdl(atci_parse_cmd_param_t *parse_cmd);
#endif
extern void bt_app_common_start_scan();
extern void bt_app_common_stop_scan();
extern const bt_bd_addr_t *bt_app_common_get_local_random_addr(void);
extern bt_status_t bt_app_common_ext_advertising_start_test(uint8_t instance);
extern bt_status_t bt_app_common_ext_advertising_stop_test(uint8_t instance);

static atci_status_t bt_app_comm_at_cmd_ble_scan_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ble_cancel_conn_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ble_random_addr_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_set_fast_pair_tx_power_level(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_gatt_over_bredr(atci_parse_cmd_param_t *parse_cmd);

#ifdef MTK_IAP2_PROFILE_ENABLE
static atci_status_t bt_app_comm_at_cmd_get_mfi_cert_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_get_mfi_response_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_check_mfi_chip_hdl(atci_parse_cmd_param_t *parse_cmd);
#endif
static atci_status_t bt_app_comm_at_cmd_sniff_mode(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ext_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd);


#ifdef AIR_LE_AUDIO_ENABLE
#define BLE_CCP_PARM_MAX_LEN     20

extern bt_le_sink_srv_music_active_handle g_music_active_handle;
extern bt_le_sink_srv_call_active_handle g_call_active_handle;

extern bt_status_t ble_csis_get_sirk(bt_key_t *sirk);
extern void ble_csis_write_nvkey_sirk(bt_key_t *sirk);
extern bt_status_t bt_app_common_remove_ltk();
extern bt_status_t bt_app_common_reset_ltk();
extern bt_status_t bt_sink_srv_le_volume_pts_set_paras(uint8_t volume, uint8_t mute, uint8_t flags);
extern bool bt_comm_at_cmd_le_audio_pts_ccp_hdl(char *pChar);
#endif

#if defined(AIR_LE_AUDIO_ENABLE) || defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
extern bt_status_t bt_le_audio_at_cmd_pts_mcp_hdl(char *pChar);
extern bt_status_t bt_le_audio_at_cmd_pts_csis_hdl(char *pChar);
static atci_status_t bt_app_comm_at_cmd_le_audio_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ble_remove_bonded_device_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ble_random_addr_hdl(atci_parse_cmd_param_t *parse_cmd);
#endif

#ifdef AIR_BT_HID_ENABLE
extern bt_status_t bt_hid_io_callback(atci_parse_cmd_param_t *parse_cmd);
#endif

static atci_status_t bt_app_comm_at_cmd_hfp_codec_set_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ext_ble_ltk_2_linkkey(atci_parse_cmd_param_t *parse_cmd);


static atci_cmd_hdlr_item_t bt_app_comm_at_cmd[] = {
    {
        .command_head = "AT+BLEADV",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_adv_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+NRPA",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_nrpa_adv_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+FOTA",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_fota_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    ,
    {
        .command_head = "AT+RHO",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_rho_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
#endif
    ,
    {
        .command_head = "AT+BLESCAN",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_scan_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BLECANCELCONN",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_cancel_conn_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BLERANDOMADDR",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_random_addr_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+FPSTXPOWER",
        .command_hdlr = bt_app_comm_at_cmd_set_fast_pair_tx_power_level,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#ifdef MTK_IAP2_PROFILE_ENABLE
    {
        .command_head = "AT+GETMFICERT",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_get_mfi_cert_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+GETMFIRESP",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_get_mfi_response_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+CHECKMFICHIP",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_check_mfi_chip_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#endif
    {
        .command_head = "AT+SNIFF",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_sniff_mode,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
    ,
    {
        .command_head = "AT+EXTBLEADV", /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ext_ble_adv_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+GATTOVEREDR", /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_gatt_over_bredr,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+HFPCODEC", /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_hfp_codec_set_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+CTKD", /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ext_ble_ltk_2_linkkey,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#if defined(AIR_LE_AUDIO_ENABLE) || defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
    {
        .command_head = "AT+LEAUDIO",       /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_le_audio_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BLERMBOND",     /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_remove_bonded_device_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#endif
};
static atci_status_t bt_app_comm_at_cmd_sniff_mode(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_OK};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    bt_init_feature_mask_t bt_customer_config_feature = bt_customer_config_get_feature_mask_configuration();
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+SNIFF=<action>
                * action: "ON" / "OFF"
                */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                bt_customer_config_feature |= BT_INIT_FEATURE_MASK_DISABLE_SNIFF_MODE;
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                bt_customer_config_feature &= ~BT_INIT_FEATURE_MASK_DISABLE_SNIFF_MODE;
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


static atci_status_t bt_app_comm_at_cmd_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+BLEADV=<action>
                        * action: "ON" / "OFF"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                /* BLE ADV OFF */
                bt_status_t status = bt_app_common_advtising_stop();
                if (status == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                /* BLE ADV ON */
                bt_status_t status = bt_app_common_start_ble_adv_with_default_interval();
                if (status == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_nrpa_adv_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+NRPA=<action>
                        * action: "ON" / "OFF"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                /* NRPA ADV OFF */
                bt_status_t status = bt_app_common_advtising_stop();
                if (status == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                /* NRPA ADV ON */
                bt_bd_addr_t addr;
                bt_app_common_generate_non_resolvable_private_address(addr);
                bt_app_common_store_local_random_address(&addr);
                bt_gap_le_set_random_address((bt_bd_addr_ptr_t)addr);
                bt_status_t status = bt_app_common_nrpa_adv_start();
                if (status == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}



static atci_status_t bt_app_comm_at_cmd_fota_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+FOTA=<action>
                        * action: "CANCEL"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "CANCEL", 6)) {
#if (defined MTK_FOTA_VIA_RACE_CMD) && (defined MTK_RACE_CMD_ENABLE)
                /* STOP FOTA */
                if (RACE_ERRCODE_SUCCESS == race_fota_cancel()) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
#endif
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

void bt_app_common_at_cmd_print_report(char *string)
{
    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (response != NULL) {
        memset(response, 0, sizeof(*response));
    } else {
        return;
    }
    snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
             "%s\n", (char *)string);
    response->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
    response->response_len = strlen((char *)response->response_buf);
    if (response->response_len > 0) {
        atci_send_response(response);
    }
    vPortFree(response);
    return;
}

#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
void bt_app_common_at_cmd_rho_srv_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    LOG_MSGID_I(BT_APP, "RHO srv status_callback role 0x%x, event %d, status 0x%x", 3, role, event, status);
    if (response != NULL) {
        memset(response, 0, sizeof(*response));
    } else {
        return;
    }
    switch (event) {
        case BT_ROLE_HANDOVER_PREPARE_REQ_IND: {
            snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                     "Alert:RHO prepare\r\n");
            break;
        }
        case BT_ROLE_HANDOVER_START_IND: {
            snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                     "Alert:RHO start\r\n");
            break;
        }
        case BT_ROLE_HANDOVER_COMPLETE_IND: {
            uint8_t role_string[20] = {0};
            if (status == BT_STATUS_SUCCESS) {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    strcpy((char *)role_string, "(Partner now).\r\n");
                } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    strcpy((char *)role_string, "(Agent now).\r\n");
                } else {
                    strcpy((char *)role_string, "(error now)!\r\n");
                }
                snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "Alert:RHO succuss%s", (char *)role_string);
            } else {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    strcpy((char *)role_string, "(Agent now).\r\n");
                } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    strcpy((char *)role_string, "(Partner now).\r\n");
                } else {
                    strcpy((char *)role_string, "(error now)!\r\n");
                }
                snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "Alert:RHO fail%s", (char *)role_string);
            }
            break;
        }
    }

    response->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
    response->response_len = strlen((char *)response->response_buf);
    if (response->response_len > 0) {
        atci_send_response(response);
    }
    vPortFree(response);
}
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#define BT_EXT_TIMER_TEST_TIMER_ID             (BT_TIMER_EXT_GROUP_SINK(9))
#define BT_EXT_TIMER_TEST_TIMER_DUR            (20 * 1000)
uint32_t g_timer_data = 0x1234;

void bt_app_common_timer_test_cb(uint32_t timer_id, uint32_t data)
{
    LOG_MSGID_I(BT_APP, "bt_app_common_timer_test_cb, id is 0x%8x, data is 0x%4x\r\n", 2, timer_id, data);
}
#endif
static atci_status_t bt_app_comm_at_cmd_rho_hdl(atci_parse_cmd_param_t *parse_cmd)
{
#if defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    uint32_t response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    LOG_MSGID_I(BT_APP, "bt_app_comm_at_cmd_rho_hdl", 0);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE: {
            /* AT+RHO */
            bt_status_t status = bt_role_handover_start();
            if (BT_STATUS_SUCCESS == status || BT_STATUS_BUSY == status) {
                response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                LOG_MSGID_I(BT_APP, "bt_app_comm_at_cmd_rho_hdl fail 0x%x", 1, status);
            }

            break;
        }

        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+RHO=<action>
                        * action: "TIMER"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "TIMER", 5)) {
                /* START A TIMER */
                LOG_MSGID_I(BT_APP, "start an 20s test timer!", 0);
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
                bt_timer_ext_status_t timer_ret = BT_TIMER_EXT_STATUS_SUCCESS;
                timer_ret = bt_timer_ext_start(BT_EXT_TIMER_TEST_TIMER_ID, (uint32_t)g_timer_data, BT_EXT_TIMER_TEST_TIMER_DUR, bt_app_common_timer_test_cb);
                if (timer_ret != BT_TIMER_EXT_STATUS_SUCCESS) {
                    LOG_MSGID_I(BT_APP, "start test timer fail!", 0);
                }
                LOG_MSGID_I(BT_APP, "start test timer success, id is 0x%8x, data is 0x%4x\r\n", 2, BT_EXT_TIMER_TEST_TIMER_ID, g_timer_data);
                response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
#endif
            }

            break;
        }

        default:
            break;
    }

    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (response != NULL) {
        memset(response, 0, sizeof(*response));
    } else {
        return ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }

    response->response_flag = response_flag;
    response->response_len = strlen((char *)response->response_buf);
    atci_send_response(response);
    vPortFree(response);
#endif /*defined(SUPPORT_ROLE_HANDOVER_SERVICE)*/
    return ATCI_STATUS_OK;
}
#endif

static atci_status_t bt_app_comm_at_cmd_ble_scan_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+BLESCAN=<action>
                        * action: "ON" / "OFF"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                /* BLE SCAN OFF */
                bt_app_common_stop_scan();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                /* BLE SCAN ON */
                bt_app_common_start_scan();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_ble_cancel_conn_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE: {
            /* AT+BLECANCELCONN*/
            bt_gap_le_cancel_connection();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_ble_random_addr_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+BLERANDOMADDR=<action>
                        * action: "GET"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "GET", 3)) {
                uint8_t temp_str[30] = {0};
                bt_bd_addr_t *addr = NULL;

#if defined(AIR_LE_AUDIO_ENABLE) || defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
                bt_gap_le_advertising_handle_t adv_handle = 0;
                bt_bd_addr_t adv_addr = {0};
                if (!multi_ble_adv_manager_get_random_addr_and_adv_handle(MULTI_ADV_INSTANCE_NOT_RHO, &adv_addr, &adv_handle)) {
                    break;
                }
                addr = &adv_addr;
#else
                addr = (bt_bd_addr_t *)bt_app_common_get_local_random_addr();
#endif
                snprintf((char *)temp_str, sizeof(temp_str), "0x%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                         (*addr)[5], (*addr)[4], (*addr)[3], (*addr)[2], (*addr)[1], (*addr)[0]);
                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+Get addrss:%s\r\n", (char *)temp_str);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_set_fast_pair_tx_power_level(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

#ifdef AIR_BT_FAST_PAIR_ENABLE
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            int8_t tx_power_level = atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
            LOG_MSGID_I(BT_APP, "set fast pair tx power level :%d", 1, tx_power_level);

            app_fast_pair_set_tx_power_level(tx_power_level);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            break;
    }
#endif

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

#ifdef MTK_IAP2_PROFILE_ENABLE

#define MAX_MFI_CERT_DATA_LEN 640
static atci_status_t bt_app_comm_at_cmd_get_mfi_cert_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    int                 r;
    uint8_t             *accessory_cert;
    uint32_t            accessory_cert_len = MAX_MFI_CERT_DATA_LEN;
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            accessory_cert = (uint8_t *)pvPortMalloc(MAX_MFI_CERT_DATA_LEN);
            if (!accessory_cert) {
                APPS_LOG_MSGID_I("malloc mfi cert data buf failed.", 0);
                break;
            }
            memset(accessory_cert, 0, MAX_MFI_CERT_DATA_LEN);

            r = mfi_coprocessor_get_accessory_cert(&accessory_cert, &accessory_cert_len);
            if (r < 0) {
                APPS_LOG_MSGID_I("get mfi cert data failed.", 0);
            } else {
                APPS_LOG_MSGID_I("get mfi cert data len: %d.", 1, accessory_cert_len);
                APPS_LOG_DUMP_I("mfi data:", accessory_cert, accessory_cert_len);
            }
            vPortFree(accessory_cert);
        }
        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


#define MAX_MFI_RESP_DATA_LEN 64
#define MAX_MFI_CHALLENGE_DATA_LEN 32
static uint8_t mfi_challenge_data[MAX_MFI_CHALLENGE_DATA_LEN] = {0};
static atci_status_t bt_app_comm_at_cmd_get_mfi_response_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    int                 r;
    uint8_t             *resp_data;
    uint32_t            resp_data_len = MAX_MFI_RESP_DATA_LEN;
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            resp_data = (uint8_t *)pvPortMalloc(MAX_MFI_RESP_DATA_LEN);
            if (!resp_data) {
                APPS_LOG_MSGID_I("malloc mfi resp data buf failed.", 0);
                break;
            }
            memset(resp_data, 0, MAX_MFI_CERT_DATA_LEN);

            r = mfi_coprocessor_gen_challenge_response(mfi_challenge_data, MAX_MFI_CHALLENGE_DATA_LEN, &resp_data, &resp_data_len);
            if (r < 0) {
                APPS_LOG_MSGID_I("get mfi resp data failed.", 0);
            } else {
                APPS_LOG_MSGID_I("get mfi resp data len: %d.", 1, resp_data_len);
                APPS_LOG_DUMP_I("mfi data:", resp_data, resp_data_len);
            }
            vPortFree(resp_data);
        }
        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


#define REG_DATA_LEN_1 1
#define REG_DATA_LEN_4 4
static bool check_mfi_version_data(atci_response_t *response)
{
    uint8_t temp_buf[4] = {0};
    int32_t ret = 0;
    char *ret_str = NULL;

    /* get device version */
    ret = aacp_i2c_read(AACP_DEVICE_VERSION, temp_buf, REG_DATA_LEN_1);
    if (ret != REG_DATA_LEN_1) {
        ret_str = "FAIL: get device version fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x07) {
        ret_str = "FAIL: invalid device version";
        goto func_ret;
    }

    /* get authentication revision */
    memset(temp_buf, 0x00, 4);
    ret = aacp_i2c_read(AACP_FIRMWARE_VERSION, temp_buf, REG_DATA_LEN_1);
    if (ret != REG_DATA_LEN_1) {
        ret_str = "FAIL: get authentication version fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x01) {
        ret_str = "FIAL: invalid authentication version.";
        goto func_ret;
    }

    /* get authentication protocal major version */
    memset(temp_buf, 0x00, 4);
    ret = aacp_i2c_read(AACP_AUTH_PROTO_MAJOR_VERSION, temp_buf, REG_DATA_LEN_1);
    if (ret != REG_DATA_LEN_1) {
        ret_str = "FAIL: get authentication protocal major version fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x03) {
        ret_str = "FIAL: invalid authentication protocal major version.";
        goto func_ret;
    }

    /* get authentication protocal minor version */
    memset(temp_buf, 0x00, 4);
    ret = aacp_i2c_read(AACP_AUTH_PROTO_MINOR_VERSION, temp_buf, REG_DATA_LEN_1);
    if (ret != REG_DATA_LEN_1) {
        ret_str = "FAIL: get authentication protocal minor version fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x00) {
        ret_str = "FIAL: invalid authentication protocal minor version.";
        goto func_ret;
    }

    /* get device id */
    memset(temp_buf, 0x00, 4);
    ret = aacp_i2c_read(AACP_DEVICE_ID, temp_buf, REG_DATA_LEN_4);
    if (ret != REG_DATA_LEN_4) {
        ret_str = "FAIL: get device id fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x00 || temp_buf[1] != 0x00 ||
        temp_buf[2] != 0x03 || temp_buf[3] != 0x00) {
        ret_str = "FIAL: invalid device id.";
        goto func_ret;
    }

func_ret:
    if (ret_str != NULL) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, ret_str);
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        return false;
    }
    return true;
}


static bool check_mfi_certifacation_data(atci_response_t *response)
{
    uint32_t idx = 0;
    bool check_result = false;
    int32_t ret = 0;
    uint8_t *accessory_cert;
    uint32_t accessory_cert_len = MAX_MFI_CERT_DATA_LEN;

    /* request certification data */
    accessory_cert = (uint8_t *)pvPortMalloc(MAX_MFI_CERT_DATA_LEN);
    if (!accessory_cert) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: malloc memory fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    memset(accessory_cert, 0x00, MAX_MFI_CERT_DATA_LEN);
    ret = mfi_coprocessor_get_accessory_cert(&accessory_cert, &accessory_cert_len);
    if (ret < 0) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: get certification data fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    /* check certification data */
    //on aacp 3.0, the certification data length is between 607 and 609, inclusive.
    if (accessory_cert_len < 607 || accessory_cert_len > 609) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: certification data length is %d.", (int)accessory_cert_len);
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    for (idx = 0; idx < accessory_cert_len; idx++) {
        if (accessory_cert[idx] != 0x00) {
            break;
        }
    }
    if (idx == accessory_cert_len) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: certification data is padding by 0x00.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    if (accessory_cert != NULL) {
        vPortFree(accessory_cert);
        accessory_cert = NULL;
    }
    check_result = true;

func_ret:
    if (accessory_cert != NULL) {
        vPortFree(accessory_cert);
        accessory_cert = NULL;
    }
    return check_result;
}


static bool check_mfi_challenge_response(atci_response_t *response)
{
    bool check_result = false;
    int32_t ret = 0;
    uint32_t idx = 0;
    uint8_t *challenge_data = NULL;
    uint8_t *challenge_response_data = NULL;
    uint32_t challenge_response_len = 0;

    /* send dummy challenge data to chip */
    challenge_data = (uint8_t *)pvPortMalloc(MAX_MFI_CHALLENGE_DATA_LEN);
    if (!challenge_data) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: malloc memory fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    } else {
        memset(challenge_data, 0x77, MAX_MFI_CHALLENGE_DATA_LEN);
    }

    challenge_response_data = (uint8_t *)pvPortMalloc(MAX_MFI_RESP_DATA_LEN);
    if (!challenge_response_data) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: malloc memory fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    } else {
        memset(challenge_response_data, 0x00, MAX_MFI_RESP_DATA_LEN);
    }

    ret = mfi_coprocessor_gen_challenge_response(challenge_data,
                                                 MAX_MFI_CHALLENGE_DATA_LEN,
                                                 &challenge_response_data,
                                                 &challenge_response_len);
    if (ret < 0) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: get challenge response data fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    /* check challenge response data */
    // on aacp 3.0, the challenge response data length is 64
    if (challenge_response_len != MAX_MFI_RESP_DATA_LEN) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: challenge response data length is %d.", (int)challenge_response_len);
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    for (idx = 0; idx < challenge_response_len; idx++) {
        if (challenge_response_data[idx] != 0x00) {
            break;
        }
    }
    if (idx == challenge_response_len) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: challenge response data is padding by 0x00.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    if (challenge_data != NULL) {
        vPortFree(challenge_data);
        challenge_data = NULL;
    }

    if (challenge_response_data != NULL) {
        vPortFree(challenge_response_data);
        challenge_response_data = NULL;
    }

    check_result = true;

func_ret:
    if (challenge_data != NULL) {
        vPortFree(challenge_data);
        challenge_data = NULL;
    }
    if (challenge_response_data != NULL) {
        vPortFree(challenge_response_data);
        challenge_response_data = NULL;
    }
    return check_result;
}


static atci_status_t bt_app_comm_at_cmd_check_mfi_chip_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

    do {
        if (!check_mfi_version_data(&response)) {
            break;
        }

        if (!check_mfi_certifacation_data(&response)) {
            break;
        }

        if (!check_mfi_challenge_response(&response)) {
            break;
        }
    } while (0);

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

#endif

static atci_status_t bt_app_comm_at_cmd_ext_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t *response = pvPortMalloc(sizeof(atci_response_t));
    if (response == NULL) {
        return ATCI_STATUS_ERROR;
    }
    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+EXTBLEADV=<action>,<instance>
                        * action: "ON" / "OFF"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                /* BLE ADV OFF */
                uint8_t instance = (uint8_t)strtoul(parse_cmd->string_ptr + parse_cmd->name_len + 5, NULL, 16);
                bt_status_t status = bt_app_common_ext_advertising_stop_test(instance);
                if (status == BT_STATUS_SUCCESS) {
                    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                /* BLE ADV ON */
                uint8_t instance = (uint8_t)strtoul(parse_cmd->string_ptr + parse_cmd->name_len + 4, NULL, 16);
                bt_status_t status = bt_app_common_ext_advertising_start_test(instance);
                if (status == BT_STATUS_SUCCESS) {
                    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            }
            break;
        }

        default:
            break;
    }

    response->response_len = strlen((char *)response->response_buf);
    atci_send_response(response);
    vPortFree(response);
    return ATCI_STATUS_OK;
}

#if defined(AIR_LE_AUDIO_ENABLE) || defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
static atci_status_t bt_app_comm_at_cmd_ble_remove_bonded_device_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    char *pChar = NULL;
    uint8_t buf[BT_BD_ADDR_LEN + 1] = {0};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        pChar = parse_cmd->string_ptr + parse_cmd->name_len + 1;

        if (0 == memcmp(pChar, "ADDR", 4)) {
            uint8_t i;

            /* AT+BLERMBOND=ADDR,<addrType>,<addr0, addr1, addr2, addr3, addr4, addr5> */
            pChar = strtok(pChar, ",");
            for (i = 0; i < (BT_BD_ADDR_LEN + 1); i++) {
                pChar = strtok(NULL, ",");
                buf[i] = atoi(pChar);
            }
            LOG_MSGID_I(BT_APP, "[AT_CMD] BLE Remove bonded device, type:%x Addr:%x %x %x %x %x %x", 7, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

        } else if (0 == memcmp(pChar, "ALL", 3)) {
            bt_device_manager_le_clear_all_bonded_info();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

        } else if (0 == memcmp(pChar, "GET", 3)) {
            uint8_t count = bt_device_manager_le_get_bonded_number();
            bt_bd_addr_t *peer_addr = (bt_bd_addr_t *)pvPortMalloc(count * sizeof(bt_bd_addr_t));
            if (peer_addr) {
                bt_device_manager_le_get_bonded_list(peer_addr, &count);
            } else {
                count = 0;
            }
            vPortFree(peer_addr);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        }
        if (response.response_flag == ATCI_RESPONSE_FLAG_APPEND_OK) {
            bt_device_manager_le_remove_bonded_device((bt_addr_t *)buf);
        }
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_le_audio_adv_hdl(char *pChar)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    uint8_t low_power_mode = 0;
    uint32_t size = sizeof(low_power_mode);

    /* Read system mode is in low power mode or not. */
    if ((NVKEY_STATUS_OK != nvkey_read_data(NVID_APP_SYSTEM_MODE_SETTING, &low_power_mode, &size)) || (0 == low_power_mode)) {
        /* AT+LEAUDIO=ADV,<ACTION> */
        /* <ACTION>: PTS_ON, PTS_OFF, PTS_RESUME */
        if (0 == memcmp(pChar, "PTS_ON", 6)) {
            multi_ble_adv_manager_start_ble_adv();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else if (0 == memcmp(pChar, "PTS_OFF", 7)) {
            multi_ble_adv_manager_pause_ble_adv();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else if (0 == memcmp(pChar, "PTS_RESUME", 10)) {
            multi_ble_adv_manager_resume_ble_adv();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else {
        LOG_MSGID_I(BT_APP, "[AT_CMD] NOT in low power mode! AT+LEAUDIO=ADV can only be used in low power mode", 0);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
    } else {
        /* AT+LEAUDIO=ADV,<ACTION> */
        /* <ACTION>: ON, OFF, RESUME */
        if (0 == memcmp(pChar, "ON", 2)) {
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_RESTART_ADV);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                                EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_RESTART_ADV,
                                NULL, 0, NULL, 0);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else if (0 == memcmp(pChar, "OFF", 3)) {
            multi_ble_adv_manager_pause_ble_adv();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else if (0 == memcmp(pChar, "RESUME", 6)) {
            multi_ble_adv_manager_resume_ble_adv();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        }
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

#if defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE)
extern void app_le_ull_get_sirk(bt_key_t *sirk);
extern void app_le_ull_write_nvkey_sirk(bt_key_t *sirk);
#endif
static atci_status_t bt_app_comm_at_cmd_le_audio_sirk_hdl(char *pChar)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    bt_key_t sirk = {0};

    /* SIRK */
    /* AT+LEAUDIO=SIRK,<ACTION> */
    /* <ACTION>: SET, GET */
    if (0 == memcmp(pChar, "GET", 3)) {
        uint8_t temp_str[50] = {0};
#ifdef AIR_LE_AUDIO_ENABLE
        ble_csis_get_sirk(&sirk);
#else
        app_le_ull_get_sirk(&sirk);
#endif
        snprintf((char *)temp_str, sizeof(temp_str), "%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X",
                 sirk[0], sirk[1], sirk[2], sirk[3], sirk[4], sirk[5], sirk[6], sirk[7],
                 sirk[8], sirk[9], sirk[10], sirk[11], sirk[12], sirk[13], sirk[14], sirk[15]);
        snprintf((char *)response.response_buf, sizeof(response.response_buf), "SIRK:%s\r\n", (char *)temp_str);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

    } else if (0 == memcmp(pChar, "SET", 3)) {
        /* AT+LEAUDIO=SIRK,SET,<B0>,<B1>,<B2>,<B3>,<B4>,<B5>,<B6>,<B7>,<B8>,<B9>,<B10>,<B11>,<B12>,<B13>,<B14>,<B15> */
        pChar = strchr(pChar, ',');
        pChar++;

        if (sscanf(pChar, "%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x",
                   (unsigned int *)&sirk[0], (unsigned int *)&sirk[1], (unsigned int *)&sirk[2], (unsigned int *)&sirk[3],
                   (unsigned int *)&sirk[4], (unsigned int *)&sirk[5], (unsigned int *)&sirk[6], (unsigned int *)&sirk[7],
                   (unsigned int *)&sirk[8], (unsigned int *)&sirk[9], (unsigned int *)&sirk[10], (unsigned int *)&sirk[11],
                   (unsigned int *)&sirk[12], (unsigned int *)&sirk[13], (unsigned int *)&sirk[14], (unsigned int *)&sirk[15]) > 0) {

            LOG_MSGID_I(common, "[SIRK] SET:%x-%x-%x-%x-%x-%x-%x-%x", 8, sirk[0], sirk[1], sirk[2], sirk[3], sirk[4], sirk[5], sirk[6], sirk[7]);
            LOG_MSGID_I(common, "[SIRK]:%x-%x-%x-%x-%x-%x-%x-%x", 8, sirk[8], sirk[9], sirk[10], sirk[11], sirk[12], sirk[13], sirk[14], sirk[15]);
#ifdef AIR_LE_AUDIO_ENABLE
            ble_csis_write_nvkey_sirk(&sirk);
#else
            app_le_ull_write_nvkey_sirk(&sirk);
#endif
        }
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

#ifdef AIR_LE_AUDIO_CIS_ENABLE

#define BT_PTS_STATE_IDLE                    (0)
#define BT_PTS_STATE_WRITE                   (1)
#define BT_PTS_STATE_WRITE_WITHOUT_RESPONSE  (2)
#define BT_PTS_STATE_READ                    (3)
uint8_t bt_app_pts_state = BT_PTS_STATE_IDLE;

static atci_status_t bt_app_comm_at_cmd_le_audio_media_hdl(char *pChar)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    bt_status_t result = BT_STATUS_UNSUPPORTED;
    bt_handle_t handle = g_music_active_handle.handle;//bt_sink_srv_cap_get_link_handle(0xFF);
    bt_le_audio_sink_action_param_t le_param = {
        .service_idx = BLE_MCP_GMCS_INDEX,
    };

    /* AT+LEAUDIO=MEDIA,<ACTION> */
    /* <ACTION>: PLAY, PAUSE, NEXT, PREV */

    if (0 == memcmp(pChar, "PLAY", 4)) {
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PLAY, &le_param);

    } else if (0 == memcmp(pChar, "PAUSE", 5)) {
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PAUSE, &le_param);

    } else if (0 == memcmp(pChar, "NEXT", 4)) {
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_NEXT_TRACK, &le_param);

    } else if (0 == memcmp(pChar, "PREV", 4)) {
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PREVIOUS_TRACK, &le_param);
    }

    LOG_MSGID_I(BT_APP, "[AT_CMD] Media result:%x", 1, result);

    if (BT_STATUS_SUCCESS == result) {
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static bt_status_t bt_app_comm_at_cmd_le_audio_call_hdl(char *pChar)
{
    bt_status_t result = BT_STATUS_UNSUPPORTED;
    uint8_t buf[BLE_CCP_PARM_MAX_LEN] = {0};

    /* AT+LEAUDIO=CALL,<ACTION> */
    /* <ACTION>: ACCEPT, TERMINATE, PLACE */
    if (0 == memcmp(pChar, "ACCEPT", 6)) {
        bt_le_audio_sink_call_action_param_t le_param = {
            .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
            .length = 2,
        };
        le_param.call_control_point = (ble_tbs_call_control_point_t *)buf;
        buf[0] = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_ACCEPT;
        buf[1] = 1; /* call index */
        result = bt_le_audio_sink_send_action(g_call_active_handle.accept_handle, BT_LE_AUDIO_SINK_ACTION_CALL_ACCEPT, &le_param);

    } else if (0 == memcmp(pChar, "TERMINATE", 9)) {
        bt_le_audio_sink_call_action_param_t le_param = {
            .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
            .length = 2,
        };
        le_param.call_control_point = (ble_tbs_call_control_point_t *)buf;
        buf[0] = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_TERMINATE;
        buf[1] = 1; /* call index */
        result = bt_le_audio_sink_send_action(g_call_active_handle.reject_handle, BT_LE_AUDIO_SINK_ACTION_CALL_TERMINATE, &le_param);

    } else if (0 == memcmp(pChar, "PLACE", 5)) {
        bt_le_audio_sink_call_action_param_t le_param = {
            .service_idx = BLE_CCP_SERVICE_INDEX_GTBS,
            .length = 5,
        };
        le_param.call_control_point = (ble_tbs_call_control_point_t *)buf;
        buf[0] = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_ORIGINATE;
        buf[1] = 3; /* uri length */
        buf[2] = 1; /* uri */
        buf[3] = 2; /* uri */
        buf[4] = 3; /* uri */
        result = bt_le_audio_sink_send_action(g_call_active_handle.dial_handle, BT_LE_AUDIO_SINK_ACTION_CALL_ORIGINATE, &le_param);

    }
    LOG_MSGID_I(BT_APP, "[AT_CMD] Call result:%x \r\n", 1, result);
    return result;
}

static bool bt_app_comm_at_cmd_le_audio_volume_hdl(char *pChar)
{
    bt_handle_t handle = bt_sink_srv_cap_stream_get_service_ble_link();

    /* AT+LEAUDIO=VOLUME,<ACTION> */
    /* <ACTION>: MUTE, UNMUTE, UP, DOWN, SET */
    if (0 == memcmp(pChar, "MUTE", 4)) {
        bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_MUTE, NULL);
    } else if (0 == memcmp(pChar, "UNMUTE", 6)) {
        bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_UNMUTE, NULL);

    } else if (0 == memcmp(pChar, "UP", 2)) {

        bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_UP, NULL);

    } else if (0 == memcmp(pChar, "DOWN", 4)) {
        bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_DOWN, NULL);

    } else if (0 == memcmp(pChar, "SET", 3)) {
        /* AT+LEAUDIO=VOLUME,SET,<volume> */
        bt_sink_srv_le_set_absolute_volume_t param;
        char *buf = (pChar + 4);
        param.volume = (uint8_t)strtoul(buf, NULL, 10);
        if (param.volume > 15) {
            return false;
        }
        param.volume *= BT_SINK_LE_VOLUME_VALUE_STEP;
        if (BT_STATUS_SUCCESS != bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_SET_ABSOLUTE_VOLUME, &param)) {
            return false;
        }

    } else if (0 == memcmp(pChar, "PTS_SET", 7)) {
        uint8_t volume, mute, flag;

        pChar = strchr(pChar, ',');
        pChar++;
        volume = atoi(pChar);

        pChar = strchr(pChar, ',');
        pChar++;
        mute = atoi(pChar);

        pChar = strchr(pChar, ',');
        pChar++;
        flag = atoi(pChar);

        bt_sink_srv_le_volume_pts_set_paras(volume, mute, flag);
    }
    return true;
}
#endif

#ifdef AIR_LE_AUDIO_BIS_ENABLE
static bool bt_app_comm_at_cmd_le_audio_broadcast_hdl(char *pChar)
{
    /* AT+LEAUDIO=BROADCAST,<ACTION> */
    /* <ACTION>: SCAN,L  SCAN,R  STOP */
    bt_status_t status = BT_STATUS_SUCCESS;
    if (0 == memcmp(pChar, "SCAN,L", 6)) {
        bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
        scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_LEFT;
        scan_param.bms_address = NULL;
        scan_param.duration = DEFAULT_SCAN_TIMEOUT;
        ble_bap_config_bis_sync_state(BT_BASS_BIS_SYNC_NO_PREFERENCE);
        status = bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
    } else if (0 == memcmp(pChar, "SCAN,R", 6)) {
        bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
        scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_RIGHT;
        scan_param.bms_address = NULL;
        scan_param.duration = DEFAULT_SCAN_TIMEOUT;
        ble_bap_config_bis_sync_state(BT_BASS_BIS_SYNC_NO_PREFERENCE);
        status = bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
    } else if ((0 == memcmp(pChar, "L", 1)) || (0 == memcmp(pChar, "R", 1))) {
        /* AT+LEAUDIO=BROADCAST,<Audio Channel>,<Address>,<Address type> */
        /* <Audio Channel>: L or R */
        /* <Address>: XX:XX:XX:XX:XX:XX */
        /* <Address type>: 0 (public) or 1 (random) */
        bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
        unsigned int value = 0;
        bt_addr_t addr = {0};
        uint8_t i = BT_BD_ADDR_LEN;
        bool left = (0 == memcmp(pChar, "L", 1));
        if (NULL == (pChar = strchr(pChar, ','))) {
            return false;
        }
        pChar++;

        while(i--) {
            sscanf(pChar, "%2x", &value);
            addr.addr[i] = (uint8_t)value;
            if (i) {
                if (NULL == (pChar = strchr(pChar, ':'))) {
                    return false;
                }
                pChar++;
            } else {
                if (NULL != (pChar = strchr(pChar, ','))) {
                    pChar++;
                    sscanf(pChar, "%2x", &value);
                    addr.type = (uint8_t)value;
                    if (BT_STATUS_SUCCESS == ble_bap_set_white_list_ex(BT_GAP_LE_ADD_TO_WHITE_LIST, addr.type, addr.addr)) {
                        scan_param.scan_type = BT_HCI_SCAN_FILTER_ACCEPT_ONLY_ADVERTISING_PACKETS_IN_WHITE_LIST;
                    }
                }
            }
        }

        scan_param.audio_channel_allocation = (left ? AUDIO_LOCATION_FRONT_LEFT : AUDIO_LOCATION_FRONT_RIGHT);
        scan_param.duration = DEFAULT_SCAN_TIMEOUT;
        scan_param.bms_address = (bt_bd_addr_t *)&addr.addr[0];

        LOG_MSGID_I(common, "BIS left=%d addr=%02X:%02X:%02X:%02X:%02X:%02X type=%02X", 8, left,
                    addr.addr[5], addr.addr[4], addr.addr[3], addr.addr[2], addr.addr[1], addr.addr[0], addr.type);

        ble_bap_config_bis_sync_state(BT_BASS_BIS_SYNC_NO_PREFERENCE);
        status = bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);

    } else if(0 == memcmp(pChar, "SYNC,", 5)) {
        bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
        scan_param.duration = DEFAULT_SCAN_TIMEOUT;
        bt_bd_addr_t addr = {0};
        uint8_t bis_idx = 1;
        uint16_t addr_5,addr_4,addr_3,addr_2,addr_1,addr_0;
        uint32_t bis_sync_state = 1;
        pChar = strchr(pChar, ',');
        pChar++;

        if (sscanf(pChar, "%2x:%2x:%2x:%2x:%2x:%2x", (unsigned int *)&addr_5, (unsigned int *)&addr_4,
                (unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0) > 0) {

            scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_RIGHT;
            addr[5] = addr_5;
            addr[4] = addr_4;
            addr[3] = addr_3;
            addr[2] = addr_2;
            addr[1] = addr_1;
            addr[0] = addr_0;
            scan_param.bms_address = (bt_bd_addr_t*)&addr[0];

            LOG_MSGID_I(common, "addr:%x-%x-%x-%x-%x-%x" , 6,
                        addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

            pChar = strchr(pChar, ',');
            pChar++;

            if (sscanf(pChar, "%d", (unsigned int *)&bis_idx) > 0) {
                if (bis_idx) {
                    bis_sync_state = bis_sync_state << (bis_idx - 1);
                    ble_bap_config_bis_sync_state(bis_sync_state);
                    bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
                }
            }
        }

    } else if (0 == memcmp(pChar, "STOP", 4)) {
        uint8_t bis_indices[1] = {0};
        bt_sink_srv_cap_stream_stop_broadcast_reception();
        bt_sink_srv_cap_stream_set_big_sync_info(1, 1, bis_indices);

    } else if (0 == memcmp(pChar, "ENCRYPT", 7)) {
        bool encryption;
        pChar = strchr(pChar, ',');
        pChar++;
        if (sscanf(pChar,"%2x", (unsigned int *)&encryption) > 0) {
            LOG_MSGID_I(common, "[BRST CODE] Encrypt:%x" , 1, encryption);
            ble_bap_set_broadcast_encryption(encryption);
        }

    }else if (0 == memcmp(pChar, "CODE,", 5)) {
        uint8_t code[16];
        pChar = strchr(pChar, ',');
        pChar++;

        if (sscanf(pChar, "%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x",
            (unsigned int *)&code[0], (unsigned int *)&code[1], (unsigned int *)&code[2], (unsigned int *)&code[3],
            (unsigned int *)&code[4], (unsigned int *)&code[5], (unsigned int *)&code[6], (unsigned int *)&code[7],
            (unsigned int *)&code[8], (unsigned int *)&code[9], (unsigned int *)&code[10], (unsigned int *)&code[11],
            (unsigned int *)&code[12], (unsigned int *)&code[13], (unsigned int *)&code[14], (unsigned int *)&code[15]) > 0) {

            LOG_MSGID_I(common, "[BRST CODE] SET:%x-%x-%x-%x-%x-%x-%x-%x" , 8, code[0], code[1], code[2], code[3], code[4], code[5], code[6], code[7]);
            LOG_MSGID_I(common, "[BRST CODE]:%x-%x-%x-%x-%x-%x-%x-%x" , 8, code[8], code[9], code[10], code[11], code[12], code[13], code[14], code[15]);
            bt_sink_srv_cap_stream_set_broadcast_code(code);
        }

    } else if (0 == memcmp(pChar, "CONFIG", 6)) {
        uint8_t big_handle = 0;
        uint8_t bis_indices[2] = {0};

        pChar = strchr(pChar, ',');
        pChar++;

        big_handle = atoi(pChar);

        if (big_handle > 4) {
            return false;
        }

        pChar = strchr(pChar, ',');
        pChar++;

        bis_indices[0] = atoi(pChar);

        LOG_MSGID_I(common, "[BIS] config sync big info big_handle[%d] bis_indices[%d]",
                    2, big_handle, bis_indices[0]);

        bt_sink_srv_cap_stream_set_big_sync_info(big_handle, 1, bis_indices);

    }else if (0 == memcmp(pChar, "NOTIFY_ALL", 10)) {
        ble_bap_bass_notify_all_clients();

    }else if (0 == memcmp(pChar, "KEEP_PA", 7)) {
        bool is_keep_pa = true;
        pChar = strchr(pChar, ',');
        pChar++;
        if (sscanf(pChar,"%2x", (unsigned int *)&is_keep_pa) > 0) {
            bt_sink_srv_cap_stream_keep_pa(is_keep_pa);
        }

    } else {
        return false;
    }

    if (status != BT_STATUS_SUCCESS) {
        return false;
    } else {
        return true;
    }
}
#endif

#ifdef AIR_LE_AUDIO_ENABLE
static atci_status_t bt_app_comm_at_cmd_le_audio_pacs_hdl(char *pChar)
{
    atci_status_t ret_at = ATCI_STATUS_ERROR;
    bt_handle_t handle;
    uint8_t i;

    /* AT+LEAUDIO=PACS,<ACTION> */
    /* <ACTION>: UPDATE_AVAILABLE_AUDIO_CONTEXTS */

    if (0 == memcmp(pChar, "UPDATE_AVAILABLE_AUDIO_CONTEXTS", 31)) {
        ble_pacs_set_available_audio_contexts(AUDIO_CONTENT_TYPE_NOTIFICATION, AUDIO_CONTENT_TYPE_NOTIFICATION);

        for (i = 0; i < APP_LE_AUDIO_MAX_LINK_NUM; i++) {
            if (BT_HANDLE_INVALID != (handle = app_lea_conn_mgr_get_handle(i))) {
                if (true == ble_pacs_send_available_audio_contexts_notify(handle)) {
                    LOG_MSGID_I(BT_APP, "[AT_CMD] PACS UPDATE_AVAILABLE_AUDIO_CONTEXTS handle:0x%x", 1, handle);
                    ret_at = ATCI_STATUS_OK;
                }
            }
        }

    } else if (0 == memcmp(pChar, "SET_AVAILABLE_AUDIO_CONTEXTS", 28)) {
        ble_pacs_set_available_audio_contexts(AUDIO_CONTENT_TYPE_NOTIFICATION, AUDIO_CONTENT_TYPE_NOTIFICATION);
        ret_at = ATCI_STATUS_OK;
    }

    return ret_at;
}

static atci_status_t bt_app_comm_at_cmd_le_audio_mics_hdl(char *pChar)
{
    bt_status_t result = BT_STATUS_UNSUPPORTED;
    atci_status_t ret_at = ATCI_STATUS_ERROR;
    bt_handle_t handle = BT_HANDLE_INVALID;
    int i = 0;

    /* AT+LEAUDIO=MICS,<ACTION> */
    /* <ACTION>: SET_MUTE, SET_UNMUTE */

    for (i = 0; i < APP_LE_AUDIO_MAX_LINK_NUM; i++) {
        if (BT_HANDLE_INVALID != (handle = app_lea_conn_mgr_get_handle(i))) {
            LOG_MSGID_I(BT_APP, "[AT_CMD] MICS get handle:0x%x", 1, handle);
            break;
        }
    }

    if (0 == memcmp(pChar, "SET_MUTE", 8)) {
        LOG_MSGID_I(BT_APP, "[AT_CMD] MICS SET_MUTE handle:0x%x", 1, handle);
        result = ble_mics_set_mute(handle, BLE_MICS_MUTE_STATE_MUTE);

    } else if (0 == memcmp(pChar, "SET_UNMUTE", 10)) {
        LOG_MSGID_I(BT_APP, "[AT_CMD] MICS SET_UNMUTE handle:0x%x", 1, handle);
        result = ble_mics_set_mute(handle, BLE_MICS_MUTE_STATE_UNMUTE);

    } else if (0 == memcmp(pChar, "SET_DISABLE", 11)) {
        LOG_MSGID_I(BT_APP, "[AT_CMD] MICS SET_DISABLE handle:0x%x", 1, handle);
        result = ble_mics_set_mute(handle, BLE_MICS_MUTE_STATE_DISABLE);

    } else if (0 == memcmp(pChar, "AICS_SET_AUTO", 13)) {
        LOG_MSGID_I(BT_APP, "[AT_CMD] MICS AICS_SET_AUTO handle:0x%x", 1, handle);
        result = ble_aics_set_audio_input_gain_mode(handle, 0, BLE_AICS_GAIN_MODE_AUTOMATIC);

    } else if (0 == memcmp(pChar, "AICS_SET_ONLY_AUTO", 18)) {
        LOG_MSGID_I(BT_APP, "[AT_CMD] MICS AICS_SET_ONLY_AUTO handle:0x%x", 1, handle);
        result = ble_aics_set_audio_input_gain_mode(handle, 0, BLE_AICS_GAIN_MODE_AUTOMATIC_ONLY);

    } else if (0 == memcmp(pChar, "AICS_SET_MANUAL", 15)) {
        LOG_MSGID_I(BT_APP, "[AT_CMD] MICS AICS_SET_MANUAL handle:0x%x", 1, handle);
        result = ble_aics_set_audio_input_gain_mode(handle, 0, BLE_AICS_GAIN_MODE_MANUAL);

    } else if (0 == memcmp(pChar, "AICS_SET_ONLY_MANUAL", 20)) {
        LOG_MSGID_I(BT_APP, "[AT_CMD] MICS AICS_SET_ONLY_MANUAL handle:0x%x", 1, handle);
        result = ble_aics_set_audio_input_gain_mode(handle, 0, BLE_AICS_GAIN_MODE_MANUAL_ONLY);

    }

    if (BT_STATUS_SUCCESS == result) {
        ret_at = ATCI_STATUS_OK;
    }

    return ret_at;
}
#endif

#ifdef AIR_LE_AUDIO_HAPS_ENABLE
static atci_status_t bt_app_comm_at_cmd_le_audio_haps_hdl(uint16_t len, char *pChar)
{
    extern bt_status_t ble_haps_change_feature(ble_ha_features_t features);

    /* AT+LEAUDIO=HAPS,<ACTION> */
    /* <ACTION>: ADD, REMOVE, CHANGE, ACTIVE */
    bt_status_t status = BT_STATUS_SUCCESS;
    char *e = pChar + len;
    uint8_t i;

    *e = 0;

    printf("[HAPS AT] len = %d\r\n", len);

    if (len == 4 && 0 == memcmp(pChar, "INIT", 4))
    {
        printf("[HAPS AT] INIT\r\n");

        if (ble_haps_init_server(APP_LE_AUDIO_MAX_LINK_NUM)) status = ATCI_STATUS_ERROR;
    }
    else if (len > 8 && 0 == memcmp(pChar, "ADD", 3))
    {
        // index, property, name
        bt_status_t rtv;
        char *para[3];
        uint8_t index, property;

        pChar += 3;

        for (i = 0; i < 3; i++)
        {
            pChar = strchr(pChar, ',');

            if (!pChar || pChar >= e) return ATCI_STATUS_ERROR;

            pChar++;
            para[i] = pChar;

            if (pChar >= e) return ATCI_STATUS_ERROR;
        }

        index = atoi(para[0]);
        property = atoi(para[1]);

        rtv = ble_haps_add_new_preset(index, property, e - para[2], para[2]);

        if (rtv) status = ATCI_STATUS_ERROR;
    }
    else if (len > 7 && 0 == memcmp(pChar, "REMOVE", 6))
    {
        // index
        bt_status_t rtv;
        uint8_t index;

        pChar += 6;
        pChar = strchr(pChar, ',');

        if (!pChar || pChar >= e) return ATCI_STATUS_ERROR;

        pChar++;
        index = atoi(pChar);

        if (pChar >= e) return ATCI_STATUS_ERROR;

        rtv = ble_haps_remove_preset(index);

        if (rtv) status = ATCI_STATUS_ERROR;
    }
    else if (len > 11 && 0 == memcmp(pChar, "CHANGE", 6))
    {
        // index, flag, property, name
        bt_status_t rtv;
        char *para[4];
        uint8_t index, flag, property = 0;

        pChar += 6;

        for (i = 0; i < 4; i++)
        {
            pChar = strchr(pChar, ',');

            if (!pChar) return ATCI_STATUS_ERROR;

            pChar++;
            para[i] = pChar;

            if (pChar >= e) return ATCI_STATUS_ERROR;
        }

        index = atoi(para[0]);
        flag = atoi(para[1]);
        property = atoi(para[2]);

        if (!(flag & HAS_CHANGE_FLAG_PRESET_NAME)) para[3] = e;

        rtv = ble_haps_change_preset(index, flag, property, e - para[3], para[3]);

        if (rtv) status = ATCI_STATUS_ERROR;
    }
    else if (len > 7 && 0 == memcmp(pChar, "ACTIVE", 6))
    {
        // index
        bt_status_t rtv;
        uint8_t index;

        pChar += 6;
        pChar = strchr(pChar, ',');

        if (!pChar) return ATCI_STATUS_ERROR;

        pChar++;
        index = atoi(pChar);

        if (pChar >= e) return ATCI_STATUS_ERROR;

        rtv = ble_haps_set_active_preset(index);

        if (rtv) status = ATCI_STATUS_ERROR;
    }
    else if (len > 8 && 0 == memcmp(pChar, "FEATURE", 7))
    {
        ble_ha_features_t features = {HEARING_AID_TYPE_BINAURAL, 0, 1, 1, 1, 0};
        bt_status_t rtv;
        uint8_t param[5], count = 0;

        pChar += 7;

        for (i = 0; i < 5; i++)
        {
            pChar = strchr(pChar, ',');

            if (!pChar || pChar >= e) break;

            count++;
            pChar++;
            param[i] = atoi(pChar);
            pChar++;

            if (pChar >= e) break;
        }

        if (!count) return ATCI_STATUS_ERROR;

        if (count > 4) features.wr_support = !!param[4];
        if (count > 3) features.dynamic = !!param[3];
        if (count > 2) features.independent = !!param[2];
        if (count > 1) features.sync_support = !!param[1];
        if (param[0] > 2) return ATCI_STATUS_ERROR;

        features.hearing_aid_type = param[0];

        rtv = ble_haps_change_feature(features);

        if (rtv) status = ATCI_STATUS_ERROR;
    }
    else
        status = ATCI_STATUS_ERROR;

    return status;
}
#endif

static atci_status_t bt_app_comm_at_cmd_le_audio_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_status_t ret_at = ATCI_STATUS_ERROR;
    char *pChar = NULL;
    atci_response_t *response = NULL;
    response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (response == NULL) {
        return ret_at;
    }
    memset(response, 0, sizeof(atci_response_t));
    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        pChar = parse_cmd->string_ptr + parse_cmd->name_len + 1;

        if (0 == memcmp(pChar, "SIRK", 4)) {
            /* SIRK */
            /* AT+LEAUDIO=SIRK,<ACTION> */
            /* <ACTION>: SET, GET */
            pChar = strchr(pChar, ',');
            pChar++;
            ret_at = bt_app_comm_at_cmd_le_audio_sirk_hdl(pChar);
#ifdef AIR_LE_AUDIO_CIS_ENABLE
        } else if (0 == memcmp(pChar, "MEDIA", 5)) {
            /* Media */
            /* AT+LEAUDIO=MEDIA,<ACTION> */
            /* <ACTION>: PLAY, PAUSE, NEXT, PREV */
            pChar = strchr(pChar, ',');
            pChar++;
            ret_at = bt_app_comm_at_cmd_le_audio_media_hdl(pChar);
        } else if (0 == memcmp(pChar, "VOLUME", 6)) {
            /* Volume */
            /* AT+LEAUDIO=VOLUME,<ACTION> */
            /* <ACTION>: MUTE, UNMUTE, UP, DOWN, SET */
            pChar = strchr(pChar, ',');
            pChar++;
            if (!bt_app_comm_at_cmd_le_audio_volume_hdl(pChar)) {
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            } else {
                ret_at = ATCI_STATUS_OK;
            }
        } else if (0 == memcmp(pChar, "CALL", 4)) {
            /* Call */
            /* AT+LEAUDIO=CALL,<ACTION> */
            /* <ACTION>: ACCEPT, TERMINATE, PLACE */
            pChar = strchr(pChar, ',');
            pChar++;
            if (BT_STATUS_SUCCESS == bt_app_comm_at_cmd_le_audio_call_hdl(pChar)) {
                ret_at = ATCI_STATUS_OK;
            }
        } else if (0 == memcmp(pChar, "PTSCSIS", 7)) {
            pChar = strchr(pChar, ',');
            pChar++;
            #define BT_APP_PTS_CSIS_STATE_FLAG   (1)
            if (NULL != strstr(pChar, "CSIS/SR/SP/BV-04-C")) {
                if (BT_PTS_STATE_IDLE == bt_app_pts_state) {
                    bt_app_pts_state = BT_APP_PTS_CSIS_STATE_FLAG;
                    bt_device_manager_le_clear_all_bonded_info();
                    bt_app_common_remove_ltk();
                } else { /* Must reset LTK */
                    bt_app_pts_state = BT_PTS_STATE_IDLE;
                    bt_app_common_reset_ltk();
                }
            }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
        } else if (0 == memcmp(pChar, "BROADCAST", 9)) {
            /* Broadcast */
            /* AT+LEAUDIO=BROADCAST,<ACTION> */
            /* <ACTION>: SCAN, STOP */
            pChar = strchr(pChar, ',');
            pChar++;
            if (!bt_app_comm_at_cmd_le_audio_broadcast_hdl(pChar)) {
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            } else {
                ret_at = ATCI_STATUS_OK;
            }
#endif
        } else if (0 == memcmp(pChar, "ADV", 3)) {
            /* Enalbe LE Audio ADV, low power mode use only */
            /* AT+LEAUDIO=ADV,<ACTION> */
            /* <ACTION>: ON, OFF, RESUME */
            pChar = strchr(pChar, ',');
            pChar++;
            ret_at = bt_app_comm_at_cmd_le_audio_adv_hdl(pChar);
#ifdef AIR_LE_AUDIO_ENABLE
        } else if (0 == memcmp(pChar, "RESET", 5)) {
            /* LE Audio connection reset */
            /* AT+LEAUDIO=RESET */
            app_le_audio_reset();
#endif
#ifdef AIR_LE_AUDIO_HAPS_ENABLE
        } else if (0 == memcmp(pChar, "HAPS", 4)) {
            /* LE Audio HAPS */
            /* AT+LEAUDIO=HAPS,<ACTION> */
            /* <ACTION>: ADD, REMOVE, CHANGE, ACTIVE */
            pChar = strchr(pChar + 4, ',');

            if (pChar && (parse_cmd->string_ptr + parse_cmd->string_len - 2) > pChar)
            {
                pChar++;
                ret_at = bt_app_comm_at_cmd_le_audio_haps_hdl(parse_cmd->string_ptr + parse_cmd->string_len - 2 - pChar, pChar);
            }

            if (ret_at) response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
#endif
        }
        #ifdef AIR_LE_AUDIO_ENABLE
        else if (0 == memcmp(pChar, "ASCS", 4)) {
            /* ASCS operation*/
            /* AT+LEAUDIO=ASCS,<ACTION1>,<ACTION2> */
            /* <ACTION1>: RELEASE */
            pChar = strchr(pChar, ',');
            pChar++;

            if (0 == memcmp(pChar, "RELEASE", 7)) {
                /* ASCS release ASE ID*/
                /* AT+LEAUDIO=ASCS,RELEASE,<ACTION2> */
                /* <ACTION2>: 1, 2, 3 */
                bt_handle_t handle = bt_sink_srv_cap_get_link_handle(0);
                uint8_t ase_id = 0xFF;

                pChar = strchr(pChar, ',');
                pChar++;

                if (0 == memcmp(pChar, "1", 1)) {
                    ase_id = 1;
                } else if (0 == memcmp(pChar, "2", 1)) {
                    ase_id = 2;
                } else if (0 == memcmp(pChar, "3", 1)) {
                    ase_id = 3;
                }
                bt_sink_srv_cap_stream_release_autonomously(handle, ase_id, FALSE, 0);
            }
            else
            {
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }

        } else if (0 == memcmp(pChar, "PACS", 4)) {
            /* PACS */
            /* AT+LEAUDIO=PACS,<ACTION> */
            /* <ACTION>: UPDATE_AVAILABLE_AUDIO_CONTEXTS */
            pChar = strchr(pChar, ',');
            pChar++;

            if (ATCI_STATUS_OK != (ret_at = bt_app_comm_at_cmd_le_audio_pacs_hdl(pChar))) {
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
        } else if (0 == memcmp(pChar, "START_ADV", 9)) {
            app_lea_service_start_advertising(TRUE, FALSE, 0);

        } else if (0 == memcmp(pChar, "MICS", 4)) {
            /* MICS */
            /* AT+LEAUDIO=MICS,<HANDLE>,<ACTION> */
            /* <ACTION>: SET_MUTE, SET_UNMUTE */
            pChar = strchr(pChar, ',');
            pChar++;

            if (ATCI_STATUS_OK != (ret_at = bt_app_comm_at_cmd_le_audio_mics_hdl(pChar))) {
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }

        } else if (0 == memcmp(pChar, "PTSMCP", 6)) {
            /* PTS MCP */
            /* AT+LEAUDIO=PTSMCP,<cmdType>,<cmdNum>
            * "R,xx" --> Read characteristic xx
            * "W,xx" --> Write characteristic xx
            * "N,xx" --> Write CCCD xx
            */
            pChar = strchr(pChar, ',');
            pChar++;
            bt_le_audio_at_cmd_pts_mcp_hdl(pChar);
        } else if (0 == memcmp(pChar, "PTSCSIS", 7)) {
             /* PTS CSIS */
            /* AT+LEAUDIO=PTSCSIS,<cmdType>,<cmdNum> */
            pChar = strchr(pChar, ',');
            pChar++;
            bt_le_audio_at_cmd_pts_csis_hdl(pChar);
        } else if (0 == memcmp(pChar, "PTSCCP", 6)) {
            /* PTS CCP */
            /* AT+LEAUDIO=PTSCCP,<cmdType>,<cmdNum>
            * "R,xx" --> Read characteristic xx
            * "W,xx" --> Write characteristic xx
            * "N,xx" --> Write CCCD xx
            */
            pChar = strchr(pChar, ',');
            pChar++;
            bt_comm_at_cmd_le_audio_pts_ccp_hdl(pChar);

        } else if (0 == memcmp(pChar, "REMLTK", 6)) {
            /* AT+LEAUDIO=REMLTK */
            bt_app_common_remove_ltk();
        }
        #endif
        else {
            response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
    }

    response->response_len = strlen((char *)response->response_buf);
    atci_send_response(response);
    vPortFree(response);
    return ret_at;
}

#endif  /* defined(AIR_LE_AUDIO_ENABLE) || defined(AIR_BLE_ULTRA_LOW_LATENCY_ENABLE) */

static atci_status_t bt_app_comm_at_cmd_hfp_codec_set_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    char *token = strtok(parse_cmd->string_ptr, ",");
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR};

    if (strstr(token, "set") != NULL) {
        token = strtok(NULL, ",");

        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        if (strstr(token, "CVSD") != NULL) {
            update_hfp_audio_codec(BT_HFP_CODEC_TYPE_CVSD);
        } else if (strstr(token, "MSBC") != NULL) {
            update_hfp_audio_codec(BT_HFP_CODEC_TYPE_MSBC);
        } else {
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            char *pstr = (char *)response.response_buf;
            snprintf(pstr, ATCI_UART_TX_FIFO_BUFFER_SIZE, "unknown type, ");
        }

        response.response_len = strlen((char *)response.response_buf);
    } else {
        bt_hfp_audio_codec_type_t type = get_hfp_audio_codec();
        char *pstr = (char *)response.response_buf;
        snprintf(pstr, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "codec mask: 0x%x", type);

        response.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        response.response_len = strlen((char *)response.response_buf);
    }
    atci_send_response(&response);
    return ATCI_STATUS_OK;

}

bool bt_app_ctkd_enable = false;

static atci_status_t bt_app_comm_at_cmd_ext_ble_ltk_2_linkkey(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t *response = pvPortMalloc(sizeof(atci_response_t));
    if (response == NULL) {
        return ATCI_STATUS_ERROR;
    }
    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+CTKD=<action> */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                bt_app_ctkd_enable = false;
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                bt_app_ctkd_enable = true;
                extern void bt_app_common_set_ctkd_config();
                bt_app_common_set_ctkd_config();
            }
            break;
        }

        default:
            break;
    }
    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    response->response_len = strlen((char *)response->response_buf);
    atci_send_response(response);
    vPortFree(response);
    return ATCI_STATUS_OK;
}



static bt_status_t bt_app_common_at_cmd_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    switch (msg) {
        case BT_GATT_OVER_BREDR_CONNECT_CNF: {
            bt_gatt_over_bredr_connect_cnf_t *cnf = (bt_gatt_over_bredr_connect_cnf_t *)buff;
            LOG_MSGID_I(BT_APP, "gatt over bredr connect confirmation,connection_handle = %02x,remote_mtu = %d",
                        2, cnf->connection_handle, cnf->remote_rx_mtu);
            uint8_t i = 0;
            for (i = 0; i < 6; i++) {
                LOG_MSGID_I(BT_APP, "%02x", 1, cnf->address[i]);
            }
        }
        break;
        case BT_GATT_OVER_BREDR_DISCONNECT_IND: {
            LOG_MSGID_I(BT_APP, "gatt over bredr disconnect indication", 0);
        }
        break;
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}
static void bt_app_comm_at_cmd_copy_str_to_addr(uint8_t *addr, const char *str)
{
    unsigned int i, value;
    int using_long_format = 0;
    int using_hex_sign = 0;

    if (str[2] == ':' || str[2] == '-') {
        using_long_format = 1;
    }

    if (str[1] == 'x') {
        using_hex_sign = 2;
    }

    for (i = 0; i < 6; i++) {
        sscanf(str + using_hex_sign + i * (2 + using_long_format), "%02x", &value);
        addr[5 - i] = (uint8_t) value;
    }
}
bt_gatt_over_bredr_service_handle_t service_handle_ctx = {
    .gap_start_handle = 0x0001,
    .gap_end_handle = 0x0009,
    .gatt_start_handle = 0x0011,
    .gatt_end_handle = 0x0011
};

static atci_status_t bt_app_comm_at_cmd_gatt_over_bredr(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t *response = pvPortMalloc(sizeof(atci_response_t));
    if (response == NULL) {
        LOG_MSGID_I(BT_APP, "gatt over bredr connect response is null.", 0);
        return ATCI_STATUS_ERROR;
    }
    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "CONNECT", 7)) {
                LOG_MSGID_I(BT_APP, "gatt over bredr connect.", 0);
                uint8_t addr[6];
                const char *addr_str = parse_cmd->string_ptr + parse_cmd->name_len + 1 + strlen("CONNECT ");
                bt_app_comm_at_cmd_copy_str_to_addr(addr, addr_str);
                bt_gatt_over_bredr_connect((const bt_bd_addr_t *)addr);
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                LOG_MSGID_I(BT_APP, "gatt over bredr switch on.", 0);
                bt_gatt_over_bredr_switch(true);
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                LOG_MSGID_I(BT_APP, "gatt over bredr switch off.", 0);
                bt_gatt_over_bredr_switch(false);
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "DISCONNECT", 10)) {
                LOG_MSGID_I(BT_APP, "gatt over bredr disconnect.", 0);
                const char *str = parse_cmd->string_ptr + parse_cmd->name_len + 1 + strlen("DISCONNECT ");
                bt_gatt_over_bredr_disconnect(strtoul(str, NULL, 16));
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "SETSERVICE", 10)) {
                LOG_MSGID_I(BT_APP, "gatt over bredr set service.", 0);
                const char *str = parse_cmd->string_ptr + parse_cmd->name_len + 1 + strlen("SETSERVICE ");
                service_handle_ctx.gap_start_handle = strtoul(str, NULL, 16);
                LOG_MSGID_I(BT_APP, "gap_start_handle = %02x.", 1, service_handle_ctx.gap_start_handle);
                str += 5;
                service_handle_ctx.gap_end_handle =  strtoul(str, NULL, 16);
                LOG_MSGID_I(BT_APP, "gap_end_handle = %02x.", 1, service_handle_ctx.gap_end_handle);
                str += 5;
                service_handle_ctx.gatt_start_handle =  strtoul(str, NULL, 16);
                LOG_MSGID_I(BT_APP, "gatt_start_handle = %02x.", 1, service_handle_ctx.gatt_start_handle);
                str += 5;
                service_handle_ctx.gatt_end_handle = strtoul(str, NULL, 16);
                LOG_MSGID_I(BT_APP, "gatt_end_handle = %02x.", 1, service_handle_ctx.gatt_end_handle);
            } else {

            }
        }
        break;
        default:
            break;
    }
    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    response->response_len = strlen((char *)response->response_buf);
    atci_send_response(response);
    vPortFree(response);
    return ATCI_STATUS_OK;
}
void bt_app_comm_at_cmd_init(void)
{
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    bt_role_handover_callbacks_t callbacks = {0};
    callbacks.status_cb = bt_app_common_at_cmd_rho_srv_status_callback;
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_BT_CMD, &callbacks);
#endif
    atci_register_handler(bt_app_comm_at_cmd, sizeof(bt_app_comm_at_cmd) / sizeof(atci_cmd_hdlr_item_t));
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_SYSTEM | MODULE_MASK_GATT, (void *)bt_app_common_at_cmd_event_callback);
}

