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

 
#ifndef __APP_DONGLE_CONNECTION_COMMON_H__
#define __APP_DONGLE_CONNECTION_COMMON_H__


#include "bt_gap.h"
#include "bt_gap_le.h"
#include "app_dongle_le_race.h"

#define APP_DONGLE_CM_SOURCE_ULL_V1    0x00    /**< ULL v1 source types . */
#define APP_DONGLE_CM_SOURCE_ULL_V2    0x01    /**< ULL v2 source types . */
#define APP_DONGLE_CM_SOURCE_LEA       0x02    /**< LEA source types . */
#define APP_DONGLE_CM_SOURCE_BTA       0x03    /**< BTA source types . */
#define APP_DONGLE_CM_SOURCE_MAX       0x04    /**< Max source types . */
#define APP_DONGLE_CM_SOURCE_INVALID   APP_DONGLE_CM_SOURCE_MAX + 1    /**< Invalid source types . */
typedef uint8_t app_dongle_cm_source_t;  /**< Dongle source types . */

#define APP_DONGLE_CM_LINK_MODE_ULL_V1          (1 << APP_DONGLE_CM_SOURCE_ULL_V1)
#define APP_DONGLE_CM_LINK_MODE_ULL_V2          (1 << APP_DONGLE_CM_SOURCE_ULL_V2)
#define APP_DONGLE_CM_LINK_MODE_LEA             (1 << APP_DONGLE_CM_SOURCE_LEA)
#define APP_DONGLE_CM_LINK_MODE_BTA             (1 << APP_DONGLE_CM_SOURCE_BTA)
typedef uint8_t app_dongle_cm_link_mode_t;

#define APP_DONGLE_CM_EVENT_SOURCE_STARTED            0x01  /**< This event indicates that profile about the source has connected. */
#define APP_DONGLE_CM_EVENT_SOURCE_END                0x02  /**< This event indicates that profile about the source has disconnected. */
typedef uint8_t app_dongle_cm_event_t;  /**< The event type defination for ULL v1/ULL v2/BTA/LEA to notify dongle connection manager . */

typedef struct {
    union {
        bt_gap_le_ext_advertising_report_ind_t adv_ind;      /**< This parameter is associated parameter type in the callback for #BT_GAP_LE_EXT_ADVERTISING_REPORT_IND event . */
        bt_gap_inquiry_ind_t                   inquiry_ind;  /**< This paramter defines the #BT_GAP_INQUIRY_IND event parameter type . */
    };
}app_dongle_cm_precheck_data_t; /**< The structure  defination for pre check data . */

typedef uint8_t app_dongle_cm_ull_v1_parma_t;

#define APP_DONGLE_CM_LEA_MODE_UNKNOWN   0x00
#define APP_DONGLE_CM_LEA_MODE_CIS       0x01
#define APP_DONGLE_CM_LEA_MODE_BIS       0x02
typedef uint8_t app_dongle_cm_lea_mode_t;

#define APP_DONGLE_CM_DONGLE_MODE_NONE      0x00
#define APP_DONGLE_CM_DONGLE_MODE_ULL_V1    0x01
#define APP_DONGLE_CM_DONGLE_MODE_ULL_V2    0x02
#define APP_DONGLE_CM_DONGLE_MODE_LEA_CIS   0x03
#define APP_DONGLE_CM_DONGLE_MODE_LEA_BIS   0x04
#define APP_DONGLE_CM_DONGLE_MODE_BTA       0x05
typedef uint8_t app_dongle_cm_dongle_mode_t;

typedef struct {
    union {
        app_dongle_cm_ull_v1_parma_t      is_air_pairing; /**< true: using air pairing, False: using cm connect . */
        app_dongle_cm_lea_mode_t          lea_mode;
    };
}app_dongle_cm_start_source_param_t; /**< The structure  defination for pre check data . */

typedef struct {
    union {
        uint8_t param; /**< TODO*/
    };
}app_dongle_cm_stop_source_param_t; /**< The structure  defination for pre check data . */

typedef struct {
     bt_status_t (*start_source)(const bt_addr_t addr, app_dongle_cm_start_source_param_t param);
     bt_status_t (*stop_source)(const bt_addr_t addr, app_dongle_cm_stop_source_param_t param);
     bt_status_t (*precheck)(app_dongle_cm_precheck_data_t *check_data);
} app_dongle_cm_handle_t;

bt_status_t app_dongle_cm_init(void);
bt_status_t app_dongle_cm_register_handle(app_dongle_cm_source_t type, const app_dongle_cm_handle_t *cm_handle);
bt_status_t app_dongle_cm_deregister_handle(app_dongle_cm_source_t type);
bt_status_t app_dongle_cm_notify_event (app_dongle_cm_source_t type , app_dongle_cm_event_t event, bt_status_t status, void *data);
void app_dongle_cm_set_link_mode(app_dongle_cm_link_mode_t mode);
void app_dongle_cm_pairing_key_event_handler(void);
app_dongle_cm_link_mode_t app_dongle_cm_get_link_mode(void);
bt_status_t app_dongle_cm_switch_dongle_mode(app_dongle_cm_dongle_mode_t mode);
bt_status_t app_dongle_cm_le_start_scan_device(void);
bt_status_t app_dongle_cm_le_stop_scan_device(void);
bt_status_t app_dongle_cm_le_create_connection(app_dongle_cm_source_t source_type, bt_addr_t addr);
bt_status_t app_dongle_cm_le_disconnect(app_dongle_cm_source_t source_type, bt_addr_t addr);
void app_dongle_cm_le_register_race_callback(app_dongle_le_race_event_callback_t callback);

#endif

