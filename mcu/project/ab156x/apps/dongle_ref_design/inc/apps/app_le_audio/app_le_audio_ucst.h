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

#ifndef __APP_LE_AUDIO_UCST_H__
#define __APP_LE_AUDIO_UCST_H__

#ifdef AIR_LE_AUDIO_ENABLE

#include "app_le_audio_ucst_utillity.h"
#include "bt_gattc.h"
#include "app_dongle_le_race.h"
/**************************************************************************************************
* Define
**************************************************************************************************/

/**************************************************************************************************
* Public function
**************************************************************************************************/
bool app_le_audio_ucst_is_streaming(void);

void app_le_audio_ucst_start(void);

bool app_le_audio_ucst_stop(bool restart);

bool app_le_audio_ucst_pause_ex(app_le_audio_ucst_pause_stream_t mode);

void app_le_audio_ucst_pause(void);

void app_le_audio_ucst_resume_ex(app_le_audio_ucst_pause_stream_t mode);

void app_le_audio_ucst_resume(void);

bt_status_t app_le_audio_ucst_connect_device(const bt_addr_t *addr);      /* connect device by bd_addr */

void app_le_audio_ucst_cancel_create_connection(void);

bt_status_t app_le_audio_ucst_disconnect_device(bt_addr_t *addr);   /* disconnect device by bd_addr */

bool app_le_audio_ucst_disconnect_all_device(void);     /* disconnect all device */

bt_status_t app_le_audio_ucst_start_scan_device(void);  /* start scan LE AUDIO device */

bt_status_t app_le_audio_ucst_stop_scan_device(void);   /* stop scan LE AUDIO device */

void app_le_audio_ucst_add_bonded_device_to_white_list(void);

#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
void app_le_audio_ucst_set_active_group(uint8_t group);
#endif

void app_le_audio_ucst_register_callback(app_dongle_le_race_event_callback_t callback);

void app_le_audio_ucst_init(void);

bool app_le_audio_ucst_check_pause_stream(void);

void app_le_audio_ucst_open_audio_transmitter_cb(void);

void app_le_audio_ucst_close_audio_transmitter_cb(void);

void app_le_audio_ucst_delete_device(bt_addr_t *addr);

bt_status_t app_le_audio_ucst_config_codec(bt_handle_t handle);

bt_status_t app_le_audio_ucst_update_metadata(bt_handle_t handle);

bt_status_t app_le_audio_ucst_release_ase(bt_handle_t handle);

bt_status_t app_le_audio_ucst_set_receiver_start_ready(bt_handle_t handle);

bt_status_t app_le_audio_ucst_set_receiver_stop_ready(bt_handle_t handle);

bt_status_t app_le_audio_ucst_disable_ase(bt_handle_t handle);

void app_le_audio_ucst_set_mic_channel(void);

uint8_t app_le_audio_ucst_get_streaming_port(void);

#endif /* AIR_LE_AUDIO_ENABLE */
#endif /* __APP_LE_AUDIO_UCST_H__ */

