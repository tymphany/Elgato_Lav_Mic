
/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

/**
 * File: app_ms_teams_utils.c
 *
 * Description:
 * This file is used to provide the common function for ms_teams
 *
 */

#include "app_ms_teams_utils.h"
#include "ui_shell_activity.h"
#include "ui_shell_manager.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd.h"
#include "race_cmd_relay_cmd.h"
#include "race_noti.h"
#include "race_bt.h"
#endif
#include "app_ms_teams_idle_activity.h"
#include "ms_teams_sys_mem.h"
#include <string.h>

#define TAG "[MS TEAMS] utils "

static void ms_teams_event_handler(ms_teams_event_t ev, uint32_t sub_event, uint8_t *data, uint32_t data_len)
{
    uint32_t shell_event = 0;
    uint8_t *data_copy = NULL;
    ui_shell_status_t ui_ret = UI_SHELL_STATUS_OK;

    APPS_LOG_MSGID_I(TAG"teams event callback: 0x%x, 0x%x, 0x%x, 0x%x.", 4, ev, sub_event, data, data_len);
#ifdef MTK_AWS_MCE_ENABLE
    /* TODO: send event and data to partner. */
#endif
    shell_event = ((ev << 16) & 0xFFFF0000) | (sub_event & 0xFFFF);
    if (data != NULL && data_len != 0) {
        data_copy = (uint8_t *)pvPortMalloc(data_len);
        if (data_copy == NULL) {
            return;
        }
        memcpy(data_copy, data, data_len);
    } else {
        data_len = 0;
    }

    ui_ret = ui_shell_send_event(true, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_MS_TEAMS,
                                 shell_event,
                                 data_copy,
                                 data_len,
                                 NULL, 0);
    if (ui_ret != UI_SHELL_STATUS_OK && data_len != 0) {
        APPS_LOG_MSGID_E(TAG"teams event handler send event fail.", 0);
        vPortFree(data_copy);
    }
}

void app_ms_teams_init()
{
    ms_teams_config_t config = {
#ifndef ULL_DONGLE
        MS_TEAMS_BT_SRV,
#else
        MS_TEAMS_USB_PROXY
#endif
        ms_teams_event_handler,
        NULL,
        0x01
    };

    ms_teams_init(&config);
}

bool app_ms_teams_is_dongle_connected()
{
#ifdef AIR_LE_AUDIO_ENABLE
    return true;
#else
    return false;
#endif
}

