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

#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE) || defined(AIR_ULL_AUDIO_V2_DONGLE_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "scenario_dongle_common.h"
#include "scenario_ble_audio.h"
#include "hal_audio_driver.h"
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
#if (defined AIR_GAMING_MODE_DONGLE_V2_I2S_MST_IN_ENABLE) || (defined AIR_GAMING_MODE_DONGLE_V2_I2S_SLV_IN_ENABLE) || (defined AIR_GAMING_MODE_DONGLE_V2_LINE_IN_ENABLE) || defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
bool g_i2s_slv_common_tracking_start_flag = false;
void audio_dongle_dl_afe_start(void *dongle_handle, audio_dongle_type_t dongle_type)
{
    audio_dongle_afe_in_info_t   *afe_in     = NULL;
    hal_audio_memory_parameter_t *mem_handle = NULL;
    SOURCE                       source      = NULL;
    SINK                         sink        = NULL;
    uint32_t                     gpt_count   = 0;
    hal_audio_agent_t            agent       = HAL_AUDIO_AGENT_ERROR;

    if (dongle_type == AUDIO_DONGLE_TYPE_BLE) {
        afe_in     = &((ble_audio_dongle_dl_handle_t *)dongle_handle)->source_info.afe_in;
        mem_handle = &((ble_audio_dongle_dl_handle_t *)dongle_handle)->source->param.audio.mem_handle;
        source     = ((ble_audio_dongle_dl_handle_t *)dongle_handle)->source;
        sink       = ((ble_audio_dongle_dl_handle_t *)dongle_handle)->sink;
        /* special handle for ble dongle*/
        // uint32_t saved_mask;
        // hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->fetch_flag = 0;
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->play_en_status = 0;
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->stream_status = BLE_AUDIO_DONGLE_UL_STREAM_INIT;
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->frame_count = 0;
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->frame_index = 0;
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->compen_samples = 0;
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->process_frames = 0;
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->drop_frames = 0;
        // ((ble_audio_dongle_dl_handle_t *)dongle_handle)->seq_num = 0;
        // hal_nvic_restore_interrupt_mask(saved_mask);

    } else {

    }

    if (!afe_in) {
        AUDIO_ASSERT(0 && "dongle type is not correct");
        return;
    }

    if ((mem_handle->memory_select != HAL_AUDIO_MEMORY_UL_SLAVE_DMA) &&
        (mem_handle->memory_select != HAL_AUDIO_MEMORY_UL_SLAVE_TDM)) {
        // interconn mode
        agent = hal_memory_convert_agent(mem_handle->memory_select);
        /* disable AFE irq here */
        hal_memory_set_irq_enable(agent, HAL_AUDIO_CONTROL_OFF);
    }

    /* set flag */
    source->param.audio.drop_redundant_data_at_first_time = true; // not used ?

    /* enable afe agent here */
    if ((source->param.audio.mem_handle.pure_agent_with_src != true) && // i2s slv interconn tracking mode
        (source->param.audio.AfeBlkControl.u4asrcflag != true)) {       // i2s slv vdma tracking mode
        /* interconn non-tracking mode */
        hal_audio_trigger_start_parameter_t start_parameter;
        start_parameter.memory_select = mem_handle->memory_select;
        start_parameter.enable = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        /* set agent regsiters' address */
        switch (agent) {
            case HAL_AUDIO_AGENT_MEMORY_VUL1:
                afe_in->afe_vul_cur_addr  = AFE_VUL_CUR;
                afe_in->afe_vul_base_addr = AFE_VUL_BASE;
                break;
            case HAL_AUDIO_AGENT_MEMORY_VUL2:
                afe_in->afe_vul_cur_addr  = AFE_VUL2_CUR;
                afe_in->afe_vul_base_addr = AFE_VUL2_BASE;
                break;
            case HAL_AUDIO_AGENT_MEMORY_VUL3:
                afe_in->afe_vul_cur_addr  = AFE_VUL3_CUR;
                afe_in->afe_vul_base_addr = AFE_VUL3_BASE;
                break;
            default:
                //ULL_V2_LOG_E("[ULL Audio V2][DL][AFE IN] ERROR: unknow agent = 0x%x", 1, agent);
                AUDIO_ASSERT(0);
        }
    } else {
        /* Tracking mode */
        if ((source->scenario_type == AUDIO_SCENARIO_TYPE_ULL_AUDIO_V2_DONGLE_DL_I2S_SLV_IN_0) ||
            (source->scenario_type == AUDIO_SCENARIO_TYPE_BLE_AUDIO_DONGLE_MUSIC_I2S_IN)) {
            if (source->param.audio.AfeBlkControl.u4asrcflag) { // vdma mode use hwsrc1
                if (mem_handle->memory_select == HAL_AUDIO_MEMORY_UL_VUL2) {
                    //ULL_V2_LOG_I("[ULL Audio V2][DL][AFE IN] ERROR: start with HWSRC2, this data path is not support, please check memory select", 0);
                    afe_in->afe_vul_cur_addr  = ASM2_CH01_OBUF_WRPNT;
                    afe_in->afe_vul_base_addr = ASM2_OBUF_SADR;
                    AUDIO_ASSERT(0);
                } else {
                    afe_in->afe_vul_cur_addr  = ASM_CH01_OBUF_WRPNT;
                    afe_in->afe_vul_base_addr = ASM_OBUF_SADR;
                }
            } else { // interconn mode use hwsrc2
                afe_in->afe_vul_cur_addr  = ASM2_CH01_OBUF_WRPNT;
                afe_in->afe_vul_base_addr = ASM2_OBUF_SADR;
            }
            g_i2s_slv_common_tracking_start_flag = false;
        } else {
            //ULL_V2_LOG_E("[ULL Audio V2][DL][AFE IN] ERROR: tracking mode, unknow agent = 0x%x", 1, agent);
            AUDIO_ASSERT(0);
        }
    }
    /* clear stream info */
    source->streamBuffer.BufferInfo.ReadOffset  = 0;
    source->streamBuffer.BufferInfo.WriteOffset = 0;
    source->streamBuffer.BufferInfo.bBufferIsFull = false;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);
    DSP_MW_LOG_I("[Dongle Common][DL][AFE IN] scenario type %d-%d, start stream gpt count = 0x%x, cur_addr = 0x%x, base_addr = 0x%x, cur_ro = 0x%x, cur_base = 0x%x", 7,
                source->scenario_type,
                sink->scenario_type,
                gpt_count,
                afe_in->afe_vul_cur_addr,
                afe_in->afe_vul_base_addr,
                AFE_GET_REG(afe_in->afe_vul_cur_addr),
                AFE_GET_REG(afe_in->afe_vul_base_addr)
                );

}
#endif /* afe in */

#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
