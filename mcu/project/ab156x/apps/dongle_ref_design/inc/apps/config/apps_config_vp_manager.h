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

/**
 * File: apps_config_vp_manager.h
 *
 * Description: This file defines the interface of apps_config_vp_manager.c.
 *
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef __APPS_CONFIG_VP_MANAGER_H__
#define __APPS_CONFIG_VP_MANAGER_H__

/********************************************************
 * External Function
 ********************************************************/


/**
 *  @brief This enum defines the priority of VP.
 */
typedef enum {
    VOICE_PROMPT_PRIO_LOW,     /**<  Lowest priority VP. */
    VOICE_PROMPT_PRIO_MEDIUM,  /**<  Common VP. */
    VOICE_PROMPT_PRIO_HIGH,    /**<  High priority VP. */
    VOICE_PROMPT_PRIO_ULTRA,   /**<  Voice broadcast & ringtone. */
    VOICE_PROMPT_PRIO_EXTREME, /**<  Only for power off VP. */
    VOICE_PROMPT_PRIO_TOTAL    /**<  Total priority, not used. */
} app_vp_prio_t;

/**
 *  @brief This enum defines the error code of VP module.
 */
typedef enum {
    VP_ERR_CODE_SUCCESS = 0,    /**<  The playback is successful. */
    VP_ERR_CODE_START_PLAY,     /**<  The playback is started. */
    VP_ERR_CODE_FAIL,           /**<  The playback is failed. */
    VP_ERR_CODE_FILE_NO_FOUND,  /**<  The VP file is not found. */
    VP_ERR_CODE_STOP,           /**<  The playback is stopped. */
    VP_ERR_CODE_PREEMPTED,      /**<  The playback is preempted by higher priority VP. */
    VP_ERR_CODE_SYNC_FAIL,      /**<  A sync VP sync fail due to AWS disconnect. */
    VP_ERR_CODE_SYNC_STOP_FAIL,      /**<  Stop VP sync fail due to AWS disconnect. */
} vp_err_code;

typedef void (*app_vp_play_callback_t)(uint32_t idx, vp_err_code err);  /* Type of APP VP callback function. */

/**
* @brief      This function requires to play a VP, there will occur preemption if the VP priority is higher than the current playing VP.
* @param[in]  tone_idx, the VP index which requires playing, refer to apps_config_vp_index_list.h.
* @param[in]  need_sync, true means that the VP needs to be synchronized to partner. Only can be true on agent.
* @param[in]  delay_time, after the time, agent and partner play the VP at the same time.
* @param[in]  level, the priority of the VP.
* @param[in]  cleanup, if true, will empty all queueing VP and reject any new VP from now on. This flag should be used only when system power off!!!
* @param[in]  callback, feedback VP play result.
* @return     If the operation is completed successfully, the return value is an ID(should > 0) which use in stop procedure, otherwise 0 means fail.
*/
uint16_t apps_config_set_vp(uint32_t vp_index, bool need_sync, uint32_t sync_delay_time, app_vp_prio_t level, bool cleanup, app_vp_play_callback_t callback);


#endif /* __APPS_CONFIG_VP_MANAGER_H__ */
