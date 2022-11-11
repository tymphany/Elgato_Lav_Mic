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

/* Includes ------------------------------------------------------------------*/
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)

#include "audio_transmitter_playback_port.h"
#include "bt_sink_srv_ami.h"
#include "hal_audio_message_struct_common.h"
#include "hal_dvfs_internal.h"
#include "scenario_dongle_common.h"

#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
#include "scenario_ble_audio.h"
#endif
#ifdef AIR_USB_AUDIO_ENABLE
#include "usbaudio_drv.h"
#endif
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
uint32_t dl_stream_status = 0;
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint32_t ble_audio_codec_get_frame_size(audio_dsp_codec_type_t *codec_type, audio_codec_param_t *codec_param)
{
    uint32_t frame_size = 0;

    if (*codec_type != AUDIO_DSP_CODEC_TYPE_LC3) {
        AUDIO_ASSERT(0);
    }

    frame_size = codec_param->lc3.bit_rate * codec_param->lc3.frame_interval / 8 / 1000 / 1000;

    return frame_size;
}
/****************************************************************************************************************************************************/
/*                                                      AFE IN COMMON                                                                               */
/****************************************************************************************************************************************************/
#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE || defined AIR_GAMING_MODE_DONGLE_V2_LINE_IN_ENABLE || defined AIR_GAMING_MODE_DONGLE_V2_I2S_SLV_IN_ENABLE || defined AIR_GAMING_MODE_DONGLE_V2_I2S_MST_IN_ENABLE
#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_GAMING_MODE_DONGLE_V2_LINE_IN_ENABLE
extern void bt_sink_srv_am_set_volume(bt_sink_srv_am_stream_type_t in_out, bt_sink_srv_audio_setting_vol_info_t *vol_info);
uint32_t g_dongle_line_in_default_d_gain = 0;
void audio_dongle_config_line_in_parameter(uint32_t id, audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    hal_audio_get_stream_in_setting_config(AU_DSP_LINEIN, &open_param->stream_in_param);
    open_param->stream_in_param.afe.memory = HAL_AUDIO_MEM1;
    switch (id) {
#ifdef AIR_GAMING_MODE_DONGLE_V2_LINE_IN_ENABLE
        case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_LINE_IN):
            /* ULL 2.0 Line in parameters */
            open_param->stream_in_param.afe.format = AFE_PCM_FORMAT_U32_LE;
            #ifdef AIR_BTA_IC_PREMIUM_G2
                open_param->stream_in_param.afe.sampling_rate = 48000;  // G2 -> 48K
                open_param->stream_in_param.afe.frame_size    = 240;    // 5ms irq period
            #else
                open_param->stream_in_param.afe.sampling_rate = 96000;  // G3 -> 96K
                open_param->stream_in_param.afe.frame_size    = 480;    // 5ms irq period
            #endif
            open_param->stream_in_param.afe.frame_number = 4;
            open_param->stream_in_param.afe.irq_period   = 0;
            open_param->stream_in_param.afe.hw_gain      = false;
            break;
#endif /* AIR_GAMING_MODE_DONGLE_V2_LINE_IN_ENABLE */
#ifdef AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE
        case (AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE << 16 | AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_LINE_IN):
            /* LE Dongle Line in parameters */
            open_param->stream_in_param.afe.format        = AFE_PCM_FORMAT_S16_LE;
            open_param->stream_in_param.afe.sampling_rate = 48000; // le music mode: 48k-10ms-16bit
            open_param->stream_in_param.afe.frame_size    = 480;
            open_param->stream_in_param.afe.frame_number  = 4;
            open_param->stream_in_param.afe.irq_period    = 0;
            open_param->stream_in_param.afe.hw_gain       = false;
            break;
#endif /* AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE */
        default:
            TRANSMITTER_LOG_E("[Dongle] ERROR: line in is not support in id %d, please check the func !", 1, id);
            assert(0);
            break;
    }
    /* Just use line in volume to align ADC Gain with config tool */
    bt_sink_srv_audio_setting_vol_info_t vol_info;
    memset(&vol_info, 0, sizeof(bt_sink_srv_audio_setting_vol_info_t));
    vol_info.type = VOL_LINE_IN;
    vol_info.vol_info.lineIN_vol_info.dev_out = HAL_AUDIO_DEVICE_HEADSET;
    vol_info.vol_info.lineIN_vol_info.dev_in  = HAL_AUDIO_DEVICE_MAIN_MIC;
    vol_info.vol_info.lineIN_vol_info.lev_in  = 15; // not care digital gain, only need analog gain
    bt_sink_srv_am_set_volume(STREAM_IN,  &vol_info);
    g_dongle_line_in_default_d_gain = audio_get_gain_in_in_dB(15, GAIN_DIGITAL, VOL_LINE_IN);
}
#endif /* le + ull2.0 line in */

#if defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE || defined AIR_GAMING_MODE_DONGLE_V2_I2S_MST_IN_ENABLE || defined AIR_GAMING_MODE_DONGLE_V2_I2S_SLV_IN_ENABLE
void audio_dongle_config_i2s_in_parameter(uint32_t id, audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param->stream_in_param);
    open_param->stream_in_param.afe.memory = HAL_AUDIO_MEM1;
    if (open_param->stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_SLAVE) {
        #ifdef AIR_BTA_IC_PREMIUM_G2
            clock_mux_sel(CLK_AUD_GPSRC_SEL, 2); // boost hwsrc convert speed
        #endif
    }
    switch (id) {
#if defined AIR_GAMING_MODE_DONGLE_V2_I2S_MST_IN_ENABLE || defined AIR_GAMING_MODE_DONGLE_V2_I2S_SLV_IN_ENABLE
        case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_I2S_MST_IN_0):
        case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_I2S_SLV_IN_0):
            open_param->stream_in_param.afe.format = AFE_PCM_FORMAT_S32_LE;
            /* hwsrc input rate */
            open_param->stream_in_param.afe.sampling_rate = config->scenario_config.ull_audio_v2_dongle_config.dl_config.source_param.i2s_mst_in_param.codec_param.pcm.sample_rate;
            /* hwsrc output rate */
            #if (defined AIR_GAMING_MODE_DONGLE_V2_I2S_SLV_IN_TRACKING_VDMA_MODE_ENABLE) || (defined AIR_GAMING_MODE_DONGLE_V2_I2S_SLV_IN_TRACKING_MEMIF_MODE_ENABLE)
                /* tracking mode */
                if (config->scenario_sub_id == AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_I2S_SLV_IN_0) {
                    open_param->stream_in_param.afe.stream_out_sampling_rate = config->scenario_config.ull_audio_v2_dongle_config.dl_config.sink_param.bt_out_param.link_param[0].codec_param.lc3plus.sample_rate;
                    open_param->stream_in_param.afe.frame_size = open_param->stream_in_param.afe.stream_out_sampling_rate * 5 / 1000; // period 5ms
                } else {
                    /* there is no need to support tracking mode in i2s master in */
                    open_param->stream_in_param.afe.frame_size = open_param->stream_in_param.afe.sampling_rate * 5 / 1000; // period 5ms
                }
            #else
                /* In non-tracking mode, ull2.0 only support 48K/96K input media data */
                open_param->stream_in_param.afe.frame_size = open_param->stream_in_param.afe.sampling_rate * 5 / 1000; // period 5ms
            #endif
            open_param->stream_in_param.afe.frame_number        = 4;
            open_param->stream_in_param.afe.irq_period          = 0;
            open_param->stream_in_param.afe.hw_gain             = false;
            if (open_param->stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
                open_param->stream_in_param.afe.audio_path_input_rate    = open_param->stream_in_param.afe.sampling_rate;
                //open_param->stream_in_param.afe.audio_path_output_rate   = open_param->stream_in_param.afe.sampling_rate;
            }
            // // I2S setting
            // open_param->stream_in_param.afe.i2s_format          = config->scenario_config.ull_audio_v2_dongle_config.dl_config.source_param.i2s_mst_in_param.i2s_fromat;
            // open_param->stream_in_param.afe.i2s_word_length     = config->scenario_config.ull_audio_v2_dongle_config.dl_config.source_param.i2s_mst_in_param.i2s_word_length;
            break;
#endif /* AIR_GAMING_MODE_DONGLE_V2_LINE_IN_ENABLE */

#if defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
        case (AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE << 16 | AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_I2S_IN) :
            open_param->stream_in_param.afe.format = AFE_PCM_FORMAT_S16_LE;
            #ifdef AIR_BLE_AUDIO_DONGLE_I2S_IN_TRACKING_VDMA_MODE_ENABLE
                if (open_param->stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_SLAVE) {
                    // i2s slave in vdma driver is default configured with 32-bit.
                    open_param->stream_in_param.afe.format = AFE_PCM_FORMAT_S32_LE;
                }
            #endif
            open_param->stream_in_param.afe.frame_number        = 4;
            open_param->stream_in_param.afe.irq_period          = 0;
            open_param->stream_in_param.afe.hw_gain             = false;
            open_param->stream_in_param.afe.sampling_rate       = 48000;
            open_param->stream_in_param.afe.stream_out_sampling_rate       = 48000;
            open_param->stream_in_param.afe.frame_size = 480;
            break;
#endif /* AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE */
        default:
            break;
    }
}
#endif /* le + ull2.0 i2s in */

void audio_dongle_set_stream_in_afe(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    open_param->param.stream_in = STREAM_IN_AFE;
    open_param->stream_in_param.afe.stream_channel = HAL_AUDIO_DIRECT;
    uint32_t id = (config->scenario_type << 16) | config->scenario_sub_id;
    switch (id) {
        /* line in case */
        #if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE
            case (AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE << 16 | AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_LINE_IN) :
                audio_dongle_config_line_in_parameter(id, config, open_param);
                break;
        #endif
        #if defined AIR_GAMING_MODE_DONGLE_V2_LINE_IN_ENABLE
            case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_LINE_IN) :
                audio_dongle_config_line_in_parameter(id, config, open_param);
                break;
        #endif
        /* i2s in case */
        #if defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
            case (AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE << 16 | AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_I2S_IN) :
                audio_dongle_config_i2s_in_parameter(id, config, open_param);
                break;
        #endif
        #if defined AIR_GAMING_MODE_DONGLE_V2_I2S_MST_IN_ENABLE
            case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_I2S_MST_IN_0) :
                audio_dongle_config_i2s_in_parameter(id, config, open_param);
                break;
        #endif
        #if defined AIR_GAMING_MODE_DONGLE_V2_I2S_SLV_IN_ENABLE
            case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_I2S_SLV_IN_0) :
                audio_dongle_config_i2s_in_parameter(id, config, open_param);
                break;
        #endif
        default:
            TRANSMITTER_LOG_E("[Dongle] ERROR: id %d is not support, please check the func !", 1, id);
            assert(0);
            break;
    }
}

void audio_dongle_set_start_avm_config(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    uint32_t id = (config->scenario_type << 16) | config->scenario_sub_id;
    switch (id) {
        #if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE
            case (AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE << 16 | AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_LINE_IN) :
                goto DL_CASE;
                break;
        #endif
        #if defined AIR_GAMING_MODE_DONGLE_V2_LINE_IN_ENABLE
            case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_LINE_IN) :
                goto DL_CASE;
                break;
        #endif
        #if defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
            case (AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE << 16 | AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_I2S_IN) :
                goto DL_CASE;
                break;
        #endif
        #if defined AIR_GAMING_MODE_DONGLE_V2_I2S_MST_IN_ENABLE
            case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_I2S_MST_IN_0) :
                goto DL_CASE;
                break;
        #endif
        #if defined AIR_GAMING_MODE_DONGLE_V2_I2S_SLV_IN_ENABLE
            case (AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE << 16 | AUDIO_TRANSMITTER_ULL_AUDIO_V2_DONGLE_DL_I2S_SLV_IN_0) :
                goto DL_CASE;
                break;
        #endif
        default:
            TRANSMITTER_LOG_E("[Dongle] ERROR: id %d is not support, please check the func !", 1, id);
            assert(0);
            break;
    }
DL_CASE:
    /* sync start */
    start_param->param.stream_in              = STREAM_IN_AFE;
    start_param->param.stream_out             = STREAM_OUT_BT_COMMON;
    start_param->stream_in_param.afe.aws_flag = true;
    return;
// UL_CASE:
//     /* TBD: UL case */
//     return;
}
#endif /* All Afe in Type */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
void audio_dongle_set_stream_out_bt_common(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    /* get codec frame size */
    uint32_t payload_size = ble_audio_codec_get_frame_size(&(config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_type),
                                                    &(config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param));
    if ((payload_size == 0) || (payload_size != config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_size)) {
        TRANSMITTER_LOG_E("[BLE Audio Dongle] ERROR: id [%d]-[%d] codec frame size %d, %d\r\n", 4,
            config->scenario_type,
            config->scenario_sub_id,
            payload_size,
            config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_size);
        AUDIO_ASSERT(0);
    }
    open_param->param.stream_out = STREAM_OUT_BT_COMMON;
    open_param->stream_out_param.bt_common.scenario_type = config->scenario_type;
    open_param->stream_out_param.bt_common.scenario_sub_id = config->scenario_sub_id;
    open_param->stream_out_param.bt_common.share_info_type = SHARE_BUFFER_INFO_TYPE;
    open_param->stream_out_param.bt_common.data_notification_frequency = 1;
    open_param->stream_out_param.bt_common.max_payload_size = payload_size + sizeof(LE_AUDIO_HEADER);
    open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.period                         = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period;
    open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.channel_enable                 = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.channel_enable;
    open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.codec_type                     = config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_type;//AUDIO_DSP_CODEC_TYPE_LC3
    memcpy(&(open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.codec_param.lc3), &(config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3), sizeof(audio_codec_lc3_t));
    /* upper layer will prepare channel 2's share memory */
    open_param->stream_out_param.bt_common.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_1);
    // if (dl_stream_status == 0)
    // {
    //     memset((((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->start_addr), 0, BT_AVM_SHARE_BUFFER_SIZE);
    // }
    // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->read_offset         = 0;
    // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->write_offset        = 0;
    // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_size = (payload_size+sizeof(LE_AUDIO_HEADER)+3)/4*4; //4B align
    // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_num  = BT_AVM_SHARE_BUFFER_SIZE / (((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_size);
    open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2 = (uint8_t *)open_param->stream_out_param.bt_common.p_share_info;
    /* upper layer will prepare channel 1's share memory */
    open_param->stream_out_param.bt_common.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_0);
    // if (dl_stream_status == 0)
    // {
    //     memset((((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->start_addr), 0, BT_AVM_SHARE_BUFFER_SIZE);
    // }
    // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->read_offset         = 0;
    // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->write_offset        = 0;
    // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_size = (payload_size+sizeof(LE_AUDIO_HEADER)+3)/4*4; //4B align
    // ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_num  = BT_AVM_SHARE_BUFFER_SIZE / (((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.p_share_info))->sub_info.block_size);
    open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1 = (uint8_t *)open_param->stream_out_param.bt_common.p_share_info;
    if (config->scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) {
        dl_stream_status |= 0x1;
    } else {
        dl_stream_status |= 0x2;
    }
    TRANSMITTER_LOG_I("[BLE Audio Dongle][dl] codec setting: %u, %u, 0x%x, %u, %u, %u, %u, %u, %u, 0x%x, 0x%x, 0x%x, 0x%x\r\n", 13,
                        config->scenario_sub_id,
                        config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period,
                        config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.channel_enable,
                        payload_size,
                        config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_type,
                        config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.sample_rate,
                        config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.channel_mode,
                        config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_interval,
                        config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.bit_rate,
                        open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1,
                        ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_1))->start_addr,
                        open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2,
                        ((n9_dsp_share_info_t *)(open_param->stream_out_param.bt_common.scenario_param.ble_audio_dongle_param.share_buffer_channel_2))->start_addr);
}
#endif
#endif /* dongle side */