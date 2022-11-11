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

#ifdef AIR_BT_CODEC_BLE_V2_ENABLED

/* Includes ------------------------------------------------------------------*/
#include "dsp_feature_interface.h"
#include "dsp_audio_process.h"
#include "lc3_dec_interface_v2.h"
#include "dsp_dump.h"
#include "bt_types.h"
#include "assert.h"
#include "sink_inter.h"
#include "source_inter.h"
#include "memory_attribute.h"
#include "preloader_pisplit.h"
#ifdef MTK_BT_A2DP_LC3_USE_PIC
#include "lc3_codec_portable.h"
#endif /* MTK_BT_A2DP_LC3_USE_PIC */

/* Private define ------------------------------------------------------------*/
#define LC3_PLC_MUTE_OUT_THD        10

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static lc3_dec_port_para_t lc3_dec_port_para[LC3_DEC_PORT_MAX];

/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
lc3_dec_status_t stream_codec_decoder_lc3_v2_init(lc3_dec_port_t port, void *user, lc3_dec_port_config_t *config)
{
    uint32_t saved_mask;
    int32_t i;
    void **p_user = NULL;

    if (port >= LC3_DEC_PORT_MAX) {
        AUDIO_ASSERT(0);
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (lc3_dec_port_para[port].count == 0) {
        /* update status */
        lc3_dec_port_para[port].status = LC3_DEC_PORT_STATUS_INIT;

        /* config codec informations */
        lc3_dec_port_para[port].sample_rate = config->sample_rate;
        lc3_dec_port_para[port].sample_bits = config->sample_bits;
        lc3_dec_port_para[port].bit_rate    = config->bit_rate;
        lc3_dec_port_para[port].dec_mode    = config->dec_mode;
        lc3_dec_port_para[port].channel_mode = config->channel_mode;
        lc3_dec_port_para[port].delay = config->delay;
        lc3_dec_port_para[port].frame_samples = config->frame_samples;
        lc3_dec_port_para[port].frame_interval = config->frame_interval;
        lc3_dec_port_para[port].frame_size = config->frame_size;
        lc3_dec_port_para[port].plc_mode = config->plc_mode;
        lc3_dec_port_para[port].plc_enable = config->plc_enable;
        lc3_dec_port_para[port].in_channel_num = config->in_channel_num;
        lc3_dec_port_para[port].out_channel_num = config->out_channel_num;
        if (lc3_dec_port_para[port].channel_mode == LC3_DEC_CHANNEL_MODE_MONO) {
            if (lc3_dec_port_para[port].in_channel_num != 1) {
                AUDIO_ASSERT(0);
            }
        } else if (lc3_dec_port_para[port].channel_mode == LC3_DEC_CHANNEL_MODE_STEREO) {
            if ((lc3_dec_port_para[port].in_channel_num != 1) || (lc3_dec_port_para[port].out_channel_num != 2)) {
                AUDIO_ASSERT(0);
            }
        } else {
            if (lc3_dec_port_para[port].in_channel_num != lc3_dec_port_para[port].out_channel_num) {
                AUDIO_ASSERT(0);
            }
        }
    }

    /* increase status count */
    lc3_dec_port_para[port].count += 1;
    if (lc3_dec_port_para[port].count == 0) {
        AUDIO_ASSERT(0);
    }

    /* register this user into this port */
    for (i = LC3_DEC_USER_COUNT - 1; i >= 0; i--) {
        if (lc3_dec_port_para[port].user[i] == user) {
            p_user = &(lc3_dec_port_para[port].user[i]);
            break;
        } else if (lc3_dec_port_para[port].user[i] == NULL) {
            p_user = &(lc3_dec_port_para[port].user[i]);
        }
    }
    if (p_user != NULL) {
        *p_user = user;
    } else {
        AUDIO_ASSERT(0);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return LC3_DEC_STATUS_OK;
}

lc3_dec_status_t stream_codec_decoder_lc3_v2_deinit(lc3_dec_port_t port, void *user)
{
    uint32_t saved_mask;
    int32_t i;
    bool mem_flag = false;
    void *WorkMemPtr = NULL;
    void **p_user = NULL;

    if (port >= LC3_DEC_PORT_MAX) {
        AUDIO_ASSERT(0);
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* decrease status count */
    if (lc3_dec_port_para[port].count == 0) {
        AUDIO_ASSERT(0);
    }
    lc3_dec_port_para[port].count -= 1;

    if (lc3_dec_port_para[port].count == 0) {
        /* reset codec informations */
        lc3_dec_port_para[port].sample_rate = 0;
        lc3_dec_port_para[port].sample_bits = 0;
        lc3_dec_port_para[port].bit_rate    = 0;
        lc3_dec_port_para[port].dec_mode = 0;
        lc3_dec_port_para[port].channel_mode = 0;
        lc3_dec_port_para[port].frame_samples = 0;
        lc3_dec_port_para[port].frame_interval = 0;
        lc3_dec_port_para[port].frame_size = 0;
        lc3_dec_port_para[port].plc_mode = 0;
        lc3_dec_port_para[port].plc_enable = 0;
        lc3_dec_port_para[port].in_channel_num = 0;
        lc3_dec_port_para[port].out_channel_num = 0;
        lc3_dec_port_para[port].work_buffer_size = 0;
        WorkMemPtr   = lc3_dec_port_para[port].work_mem_ptr;
        lc3_dec_port_para[port].work_mem_ptr = NULL;

        /* set memory operation flag */
        mem_flag = true;
    }

    /* unregister this user into this port */
    for (i = LC3_DEC_USER_COUNT - 1; i >= 0; i--) {
        if (lc3_dec_port_para[port].user[i] == user) {
            p_user = &(lc3_dec_port_para[port].user[i]);
            break;
        }
    }
    if (p_user != NULL) {
        *p_user = NULL;
    } else {
        AUDIO_ASSERT(0);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    if (mem_flag) {
        /* free working memory */
        if (WorkMemPtr) {
            preloader_pisplit_free_memory(WorkMemPtr);
        }
    }

    return LC3_DEC_STATUS_OK;
}

bool stream_codec_decoder_lc3_v2_initialize(void *para)
{
    uint32_t i, j;
    lc3_dec_port_para_t *lc3_dec_port = NULL;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    uint32_t saved_mask;
    void *work_mem;
    LC3_ERR_T lc3_ret;

    /* find out which port is belong to this stream */
    for (i = 0; i < LC3_DEC_PORT_MAX; i++) {
        for (j = 0; j < LC3_DEC_USER_COUNT; j++) {
            /* Check if this source or sink has already be regitsered into this index */
            if ((lc3_dec_port_para[i].user[j] == stream_ptr->source) ||
                (lc3_dec_port_para[i].user[j] == stream_ptr->sink)) {
                lc3_dec_port = &lc3_dec_port_para[i];
                goto FIND_DECODER_PORT;
            }
        }
    }
    if (lc3_dec_port == NULL) {
        AUDIO_ASSERT(0);
        return true;
    }
FIND_DECODER_PORT:

    /* status check */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (lc3_dec_port->status == LC3_DEC_PORT_STATUS_INIT) {
        lc3_dec_port->status = LC3_DEC_PORT_STATUS_RUNNING;
    } else if (lc3_dec_port->status == LC3_DEC_PORT_STATUS_RUNNING) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return false;
    } else {
        DSP_MW_LOG_I("[LC3_DEC] error status:%d", 1, lc3_dec_port->status);
        AUDIO_ASSERT(0);
        return true;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    /* get working memory and do decoder init */
    switch (lc3_dec_port->channel_mode) {
        case LC3_DEC_CHANNEL_MODE_MONO:
        case LC3_DEC_CHANNEL_MODE_STEREO:
            /* get working memory */
            lc3_dec_port->work_instance_count = 1;
            lc3_dec_port->work_buffer_size = LC3_Dec_Get_MemSize(lc3_dec_port->dec_mode);
            /* 8byte-align */
            lc3_dec_port->work_buffer_size = (lc3_dec_port->work_buffer_size + 7) / 8 * 8;
            /* first word is used for plc count */
            lc3_dec_port->work_buffer_size += sizeof(uint32_t) * 2;
            lc3_dec_port->work_mem_ptr = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, lc3_dec_port->work_buffer_size);
            if (lc3_dec_port->work_mem_ptr == NULL) {
                AUDIO_ASSERT(0);
            }
            /* plc count init */
            *((uint32_t *)(lc3_dec_port->work_mem_ptr)) = LC3_PLC_MUTE_OUT_THD;

            /* decoder init */
            work_mem = (void *)((uint32_t)(lc3_dec_port->work_mem_ptr) + sizeof(uint32_t) * 2);
            lc3_ret = LC3_Dec_Init(work_mem,
                                   lc3_dec_port->sample_bits,
                                   lc3_dec_port->sample_rate,
                                   lc3_dec_port->in_channel_num,
                                   lc3_dec_port->dec_mode,
                                   lc3_dec_port->plc_mode,
                                   lc3_dec_port->frame_interval / 100,
                                   LC3_NO_DELAY_COMPENSATION);
            if (LC3_OK != lc3_ret) {
                DSP_MW_LOG_I("[LC3_DEC] Init Fail, %d!", 1, lc3_ret);
                AUDIO_ASSERT(0);
            }
            break;

        /* dual-mono mode */
        case LC3_DEC_CHANNEL_MODE_DUAL_MONO:
            /* get working memory */
            lc3_dec_port->work_instance_count = lc3_dec_port->in_channel_num;
            lc3_dec_port->work_buffer_size = LC3_Dec_Get_MemSize(lc3_dec_port->dec_mode);
            /* 8byte-align */
            lc3_dec_port->work_buffer_size = (lc3_dec_port->work_buffer_size + 7) / 8 * 8;
            /* first word is used for plc count */
            lc3_dec_port->work_buffer_size += sizeof(uint32_t) * 2;
            lc3_dec_port->work_mem_ptr = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, lc3_dec_port->work_buffer_size * lc3_dec_port->work_instance_count);
            if (lc3_dec_port->work_mem_ptr == NULL) {
                AUDIO_ASSERT(0);
            }
            /* plc count init */
            *((uint32_t *)(lc3_dec_port->work_mem_ptr)) = LC3_PLC_MUTE_OUT_THD;

            /* decoder init one by one channel */
            for (i = 0; i < lc3_dec_port->work_instance_count; i++) {
                /* plc count init */
                *((uint32_t *)((uint32_t)(lc3_dec_port->work_mem_ptr) + i * lc3_dec_port->work_buffer_size)) = 0;

                /* decoder init */
                work_mem = (void *)((uint32_t)(lc3_dec_port->work_mem_ptr) + i * lc3_dec_port->work_buffer_size + sizeof(uint32_t) * 2);
                lc3_ret = LC3_Dec_Init(work_mem,
                                       lc3_dec_port->sample_bits,
                                       lc3_dec_port->sample_rate,
                                       1,
                                       lc3_dec_port->dec_mode,
                                       lc3_dec_port->plc_mode,
                                       lc3_dec_port->frame_interval / 100,
                                       lc3_dec_port->delay);
                if (LC3_OK != lc3_ret) {
                    DSP_MW_LOG_I("[LC3_DEC] Init Fail, %d!", 1, lc3_ret);
                    AUDIO_ASSERT(0);
                }
            }
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    DSP_MW_LOG_I("[LC3_DEC] Init Success! %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, version = 0x%x", 16,
                 lc3_dec_port->sample_bits,
                 lc3_dec_port->sample_rate,
                 lc3_dec_port->bit_rate,
                 lc3_dec_port->dec_mode,
                 lc3_dec_port->channel_mode,
                 lc3_dec_port->delay,
                 lc3_dec_port->plc_mode,
                 lc3_dec_port->plc_enable,
                 lc3_dec_port->frame_samples,
                 lc3_dec_port->frame_interval,
                 lc3_dec_port->frame_size,
                 lc3_dec_port->in_channel_num,
                 lc3_dec_port->out_channel_num,
                 lc3_dec_port->work_instance_count,
                 lc3_dec_port->work_buffer_size,
                 LC3_Get_Version());

    return false;
}

ATTR_TEXT_IN_IRAM bool stream_codec_decoder_lc3_v2_process(void *para)
{
    uint32_t i, j;
    lc3_dec_port_para_t *lc3_dec_port = NULL;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    int16_t *in_buf1;
    int16_t *out_buf1;
    int16_t *out_buf2;
    uint32_t in_frame_size;
    uint32_t out_data_size;
    uint32_t out_samples;
    uint32_t total_out_data_size;
    uint32_t frame_num;
    uint32_t currnet_frame;
    void *work_mem;
    lc3_dec_frame_status_t *p_frame_status;
    uint32_t *p_plc_count;
    LC3_ERR_T lc3_ret;

    /* find out which port is belong to this stream */
    for (i = 0; i < LC3_DEC_PORT_MAX; i++) {
        for (j = 0; j < LC3_DEC_USER_COUNT; j++) {
            /* Check if this source or sink has already be regitsered into this index */
            if ((lc3_dec_port_para[i].user[j] == stream_ptr->source) ||
                (lc3_dec_port_para[i].user[j] == stream_ptr->sink)) {
                lc3_dec_port = &lc3_dec_port_para[i];
                goto FIND_DECODER_PORT;
            }
        }
    }
    if (lc3_dec_port == NULL) {
        AUDIO_ASSERT(0);
        return true;
    }
FIND_DECODER_PORT:

    in_frame_size = stream_codec_get_input_size(para);
    if (in_frame_size == 0) {
        /* config output size */
        stream_codec_modify_output_size(para, in_frame_size);

        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&lc3_dec_port->finish_gpt_count);

        return false;
    }

    if ((in_frame_size % (lc3_dec_port->frame_size + sizeof(lc3_dec_frame_status_t))) != 0) {
        AUDIO_ASSERT(0);
        return true;
    }

    /* do decoder processs */
    switch (lc3_dec_port->channel_mode) {
        case LC3_DEC_CHANNEL_MODE_MONO:
            total_out_data_size = 0;
            frame_num = in_frame_size / (lc3_dec_port->frame_size + sizeof(lc3_dec_frame_status_t));
            for (currnet_frame = 0; currnet_frame < frame_num; currnet_frame++) {
                out_samples = lc3_dec_port->frame_samples;
                out_data_size = out_samples * lc3_dec_port->sample_bits / 8;

                /* get frame status and input and output buffer of this frame */
                p_frame_status = (lc3_dec_frame_status_t *)((uint32_t)stream_codec_get_1st_input_buffer(para) + currnet_frame * (lc3_dec_port->frame_size + sizeof(lc3_dec_frame_status_t)));
                in_buf1  = (int16_t *)((uint32_t)stream_codec_get_1st_input_buffer(para) + currnet_frame * (lc3_dec_port->frame_size + sizeof(lc3_dec_frame_status_t)) + sizeof(lc3_dec_frame_status_t));
                out_buf1 = (int16_t *)((uint32_t)stream_codec_get_1st_output_buffer(para) + total_out_data_size);

                /* get working memory and plc count of this channel */
                p_plc_count = (uint32_t *)((uint32_t)(lc3_dec_port->work_mem_ptr));
                work_mem = (void *)((uint32_t)(lc3_dec_port->work_mem_ptr) + sizeof(uint32_t) * 2);

                /* frame status check */
                if (*p_frame_status == LC3_DEC_FRAME_STATUS_NORMAL) {
                    lc3_ret = LC3_Dec_Prcs(work_mem,
                                           (uint8_t *)in_buf1,
                                           (uint8_t *)out_buf1,
                                           lc3_dec_port->frame_size,
                                           0,
                                           0);
                    if (LC3_OK != lc3_ret) {
                        DSP_MW_LOG_I("[LC3_DEC] Process Fail! %d", 1, lc3_ret);
                        /* do PLC for avoid this error */
                        *p_frame_status = LC3_DEC_FRAME_STATUS_PLC;
                        // AUDIO_ASSERT(0);
                    } else {
                        *p_plc_count = 0;
                    }
                }
                if (*p_frame_status == LC3_DEC_FRAME_STATUS_PLC) {
                    if ((lc3_dec_port->plc_enable == 0) || (*p_plc_count >= LC3_PLC_MUTE_OUT_THD)) {
                        memset(out_buf1, 0, out_data_size);
                    } else {
                        lc3_ret = LC3_Dec_Prcs(work_mem,
                                               (uint8_t *)in_buf1,
                                               (uint8_t *)out_buf1,
                                               lc3_dec_port->frame_size,
                                               1,
                                               0);
                        if (LC3_DECODE_ERROR != lc3_ret) {
                            DSP_MW_LOG_I("[LC3_DEC] PLC Process Fail! %d", 1, lc3_ret);
                            memset(out_buf1, 0, out_data_size);
                            // AUDIO_ASSERT(0);
                        }
                        *p_plc_count = (*p_plc_count) + 1;
                    }
                } else if (*p_frame_status == LC3_DEC_FRAME_STATUS_BYPASS_DECODER) {
                    /* there is a requirement that the decoder do not output any data */
                    out_data_size = 0;
                    // DSP_MW_LOG_I("[LC3_DEC] decoder output is 0!", 0);
                }

                if (out_data_size != 0) {
                    /* copy mono data into other channels */
                    for (i = 0; i < lc3_dec_port->out_channel_num - 1; i++) {
                        out_buf2 = (int16_t *)((uint32_t)stream_codec_get_output_buffer(para, i + 2) + total_out_data_size);
                        memcpy(out_buf2, out_buf1, out_data_size);
                    }
                }

                /* update total output size */
                total_out_data_size += out_data_size;
            }

            break;

        case LC3_DEC_CHANNEL_MODE_STEREO:
            total_out_data_size = 0;
            DSP_MW_LOG_I("[LC3_DEC] Not Support now!", 0);
            AUDIO_ASSERT(0);
            break;

        case LC3_DEC_CHANNEL_MODE_DUAL_MONO:
            frame_num = in_frame_size / (lc3_dec_port->frame_size + sizeof(lc3_dec_frame_status_t));
            /* process each channel one by one */
            for (i = 0; i < lc3_dec_port->work_instance_count; i++) {
                total_out_data_size = 0;
                /* get working memory and plc count of this channel */
                p_plc_count = (uint32_t *)((uint32_t)(lc3_dec_port->work_mem_ptr) + i * lc3_dec_port->work_buffer_size);
                work_mem = (void *)((uint32_t)(lc3_dec_port->work_mem_ptr) + i * lc3_dec_port->work_buffer_size + sizeof(uint32_t) * 2);
                for (currnet_frame = 0; currnet_frame < frame_num; currnet_frame++) {
                    out_samples = lc3_dec_port->frame_samples;
                    out_data_size = out_samples * lc3_dec_port->sample_bits / 8;

                    /* get frame status and input and output buffer of this frame */
                    p_frame_status = (lc3_dec_frame_status_t *)((uint32_t)stream_codec_get_input_buffer(para, i + 1) + currnet_frame * (lc3_dec_port->frame_size + sizeof(lc3_dec_frame_status_t)));
                    in_buf1  = (int16_t *)((uint32_t)stream_codec_get_input_buffer(para, i + 1) + currnet_frame * (lc3_dec_port->frame_size + sizeof(lc3_dec_frame_status_t)) + sizeof(lc3_dec_frame_status_t));
                    out_buf1 = (int16_t *)((uint32_t)stream_codec_get_output_buffer(para, i + 1) + total_out_data_size);

                    /* frame status check */
                    if (*p_frame_status == LC3_DEC_FRAME_STATUS_NORMAL) {
                        lc3_ret = LC3_Dec_Prcs(work_mem,
                                               (uint8_t *)in_buf1,
                                               (uint8_t *)out_buf1,
                                               lc3_dec_port->frame_size,
                                               0,
                                               0);
                        if (LC3_OK != lc3_ret) {
                            DSP_MW_LOG_I("[LC3_DEC] Process Fail! %d", 1, lc3_ret);
                            /* do PLC for avoid this error */
                            *p_frame_status = LC3_DEC_FRAME_STATUS_PLC;
                            // AUDIO_ASSERT(0);
                        } else {
                            *p_plc_count = 0;
                        }
                    }
                    if (*p_frame_status == LC3_DEC_FRAME_STATUS_PLC) {
                        if ((lc3_dec_port->plc_enable == 0) || (*p_plc_count >= LC3_PLC_MUTE_OUT_THD)) {
                            memset(out_buf1, 0, out_data_size);
                        } else {
                            lc3_ret = LC3_Dec_Prcs(work_mem,
                                                   (uint8_t *)in_buf1,
                                                   (uint8_t *)out_buf1,
                                                   lc3_dec_port->frame_size,
                                                   1,
                                                   0);
                            if (LC3_DECODE_ERROR != lc3_ret) {
                                DSP_MW_LOG_I("[LC3_DEC] PLC Process Fail! %d", 1, lc3_ret);
                                memset(out_buf1, 0, out_data_size);
                                // AUDIO_ASSERT(0);
                            }
                            *p_plc_count = (*p_plc_count) + 1;
                        }
                    } else if (*p_frame_status == LC3_DEC_FRAME_STATUS_BYPASS_DECODER) {
                        /* there is a requirement that the decoder do not output any data */
                        out_data_size = 0;
                        // DSP_MW_LOG_I("[LC3_DEC] decoder output is 0!", 0);
                    }

                    /* update total output size */
                    total_out_data_size += out_data_size;
                }
            }

            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    stream_codec_modify_output_samplingrate(para, lc3_dec_port->sample_rate / 1000);
    stream_codec_modify_output_size(para, total_out_data_size);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&lc3_dec_port->finish_gpt_count);

    return false;
}

#endif /* AIR_BT_CODEC_BLE_V2_ENABLED */
