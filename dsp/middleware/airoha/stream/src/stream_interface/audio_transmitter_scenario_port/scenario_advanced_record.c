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
#include "scenario_advanced_record.h"
#ifdef AIR_SOFTWARE_DRC_ENABLE
#include "compander_interface_sw.h"
#endif

#ifdef AIR_LD_NR_ENABLE
void *p_advanced_record_ld_nr_key;
#endif



void advanced_record_init(SOURCE source, SINK sink, mcu2dsp_open_param_p open_param)
{
    UNUSED(source);
    if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_ADVANCED_RECORD_N_MIC) {
        stream_resolution_t resolution;
        extern CONNECTION_IF advanced_record_n_mic_if;
        if(open_param->stream_in_param.afe.format <= AFE_PCM_FORMAT_U16_BE) {
            stream_feature_configure_resolution((stream_feature_list_ptr_t)advanced_record_n_mic_if.pfeature_table, RESOLUTION_16BIT, CONFIG_DECODER);
            resolution = RESOLUTION_16BIT;
        }
        else{
            stream_feature_configure_resolution((stream_feature_list_ptr_t)advanced_record_n_mic_if.pfeature_table, RESOLUTION_32BIT, CONFIG_DECODER);
            resolution = RESOLUTION_32BIT;
        }
#ifdef AIR_LD_NR_ENABLE
        ld_nr_port_t *ld_nr_port;
        ld_nr_config_t ld_nr_config;
        ld_nr_port = stream_function_ld_nr_get_port(source);
        ld_nr_config.channel_num = source->param.audio.channel_num;
        ld_nr_config.frame_size  = 240 * ((resolution == RESOLUTION_16BIT)? 2:4);
        ld_nr_config.resolution  = resolution;
        ld_nr_config.sample_rate = source->param.audio.rate;

        if (p_advanced_record_ld_nr_key == NULL) {
            //PSAP_LOG_E(g_PSAP_msg_id_string_12, "hearing-aid LD_NR NVKEY NULL", 0);
            configASSERT(0);
        }

        ld_nr_config.nvkey_para = p_advanced_record_ld_nr_key;
        ld_nr_config.background_process_enable = true;
        ld_nr_config.background_process_fr_num = 2;
        stream_function_ld_nr_init(ld_nr_port, &ld_nr_config);
        DSP_MW_LOG_I("[LD_NR]p_wireless_mic_ld_nr_key 0x%x channel_num=%d, resolution:%d", 3, p_advanced_record_ld_nr_key, source->param.audio.channel_num, resolution);
#endif /* AIR_LD_NR_ENABLE */
#ifdef AIR_SOFTWARE_DRC_ENABLE
    /* sw drc init */
    sw_compander_config_t drc_config;
    drc_config.mode = SW_COMPANDER_AUDIO_MODE;
    drc_config.channel_num = source->param.audio.channel_num;
    drc_config.sample_rate = source->param.audio.rate;
    drc_config.frame_base = 8;
    drc_config.recovery_gain = 4; /* 0dB */
    drc_config.vol_default_gain = 0x08000000; /* 0dB */
    drc_config.default_nvkey_mem = NULL;
    drc_config.default_nvkey_id = NVKEY_DSP_PARA_ADVANCED_RECORD_AU_CPD;
    sw_compander_port_t *drc_port = stream_function_sw_compander_get_port(source);
    stream_function_sw_compander_init(drc_port, &drc_config);
    DSP_MW_LOG_I("[advanced_record_n_mic]sw drc 0x%x info, %d, %d, %d, %d, 0x%x, 0x%x, 0x%x\r\n", 11,
                drc_port,
                drc_config.mode,
                drc_config.channel_num,
                drc_config.sample_rate,
                drc_config.frame_base,
                drc_config.recovery_gain,
                drc_config.vol_default_gain,
                drc_config.default_nvkey_id);
#endif
    }
}

void advanced_record_deinit(SOURCE source, SINK sink)
{
    UNUSED(source);
    if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_ADVANCED_RECORD_N_MIC) {
#ifdef AIR_LD_NR_ENABLE
        ld_nr_port_t *ld_nr_port;
        ld_nr_port = stream_function_ld_nr_get_port(source);
        stream_function_ld_nr_deinit(ld_nr_port);
        preloader_pisplit_free_memory(p_advanced_record_ld_nr_key);
        p_advanced_record_ld_nr_key = NULL;
#endif /* AIR_LD_NR_ENABLE */
#ifdef AIR_SOFTWARE_DRC_ENABLE
        sw_compander_port_t *drc_port;
        drc_port = stream_function_sw_compander_get_port(source);
        stream_function_sw_compander_deinit(drc_port);
#endif
    }
}
