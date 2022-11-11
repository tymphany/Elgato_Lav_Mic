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

#if defined(AIR_WIRELESS_MIC_RX_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "scenario_wireless_mic_rx.h"
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "dsp_callback.h"
#include "dsp_temp.h"
#include "dsp_dump.h"
#include "dsp_scenario.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_process.h"
#include "stream_audio_transmitter.h"
#include "stream_bt_common.h"
#include "bt_types.h"
#include "hal_gpt.h"
#include "hal_sleep_manager.h"
#include "memory_attribute.h"
#include "clk_skew_sw.h"
#include "sw_gain_interface.h"
#include "sw_buffer_interface.h"
#include "lc3plus_enc_interface.h"
#include "lc3plus_dec_interface.h"
#include "hal_audio_driver.h"
#include "bt_interface.h"
#include "compander_interface_sw.h"

/* Private define ------------------------------------------------------------*/
#define WIRELESS_MIC_RX_DL_PATH_DEBUG_DUMP                      1
#define WIRELESS_MIC_RX_UL_PATH_DEBUG_DUMP                      1
#define WIRELESS_MIC_RX_DL_PATH_DEBUG_LOG                       0
#define WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG                       0

#define UL_BT_TIMESTAMP_DIFF                                        (2)  /* 0.625ms/0.3125ms */
#define UL_PLAYEN_DELAY_FRAME_5000US                                (10-4) /* procsess time - 1ms */
#define UL_FIRST_PACKET_SAFE_DELAY_FRAME_5000US                     (2)
#define UL_BT_RETRY_WINDOW_FRAME_5000US                             (18)
#define UL_BT_INTERVAL_FRAME_5000US                                 (16)
#define UL_CLOCK_SKEW_CHECK_COUNT                                   (4)
#define UL_PLAYEN_DELAY_FRAME_5000US_LINE_OUT                       (0) /* procsess time 2ms */
#define UL_FIRST_PACKET_SAFE_DELAY_FRAME_5000US_LINE_OUT            (-2)
#define UL_PLAYEN_DELAY_FRAME_5000US_I2S_SLV_OUT                    (0) /* procsess time 2ms */
#define UL_FIRST_PACKET_SAFE_DELAY_FRAME_5000US_I2S_SLV_OUT         (-2)
#define UL_PLAYEN_DELAY_FRAME_10000US                               (10-4) /* procsess time - 1ms */
#define UL_FIRST_PACKET_SAFE_DELAY_FRAME_10000US                    (2)
#define UL_BT_RETRY_WINDOW_FRAME_10000US                            (36)
#define UL_BT_INTERVAL_FRAME_10000US                                (32)
#define UL_CLOCK_SKEW_CHECK_COUNT                                   (4)
#define UL_PLAYEN_DELAY_FRAME_10000US_LINE_OUT                      (0) /* procsess time 2ms */
#define UL_FIRST_PACKET_SAFE_DELAY_FRAME_10000US_LINE_OUT           (-2)
#define UL_PLAYEN_DELAY_FRAME_10000US_I2S_SLV_OUT                   (0) /* procsess time 2ms */
#define UL_FIRST_PACKET_SAFE_DELAY_FRAME_10000US_I2S_SLV_OUT        (-2)
#define UL_PACKET_ANCHOR_IN_MIXER_INVAILD_VALUE                     (0xFFFFFFFF)
#define UL_DL_AGENT_FIFO_SAMPLES                                    (4)

#ifndef min
#define min(_a, _b)   (((_a)<(_b))?(_a):(_b))
#endif

#ifndef max
#define max(_a, _b)   (((_a)<(_b))?(_b):(_a))
#endif

/* Private typedef -----------------------------------------------------------*/
typedef struct  {
    U16 DataOffset; /* offset of payload */
    U16 _reserved_word_02h;
    U32 TimeStamp; /* this CIS/BIS link's CLK, Unit:312.5us */
    U16 ConnEvtCnt; /* event count seem on airlog, for debug propose */
    U8 SampleSeq;  /* Sameple sequence of this SDU interval Ex:0,1,2... */
    U8 _reserved_byte_0Bh;
    U8 PduHdrLo;
    // U8 _reserved_byte_0Dh;
    U8 valid_packet; /* valid packet: 0x01, invalid packet 0x00 */
    U8 PduLen ; /* payload size */
    U8 _reserved_byte_0Fh;
    U16 DataLen;
    U16 _reserved_word_12h;
    // U32 _reserved_long_0;
    U32 crc32_value;
    U32 _reserved_long_1;
} WIRELESS_MIC_RX_HEADER;

typedef struct {
    uint32_t vol_ch;
    int32_t  vol_gain[WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER];
} vol_gain_t;

typedef union {
    vol_gain_t vol_gain;
    uint8_t connection_status[AUDIO_TRANSMITTER_WIRELESS_MIC_RX_SUB_ID_MAX][WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER];
} wireless_mic_rx_runtime_config_operation_param_t, *wireless_mic_rx_runtime_config_operation_param_p;

typedef struct {
    wireless_mic_rx_runtime_config_operation_t          config_operation;
    wireless_mic_rx_runtime_config_operation_param_t    config_param;
} wireless_mic_rx_runtime_config_param_t, *wireless_mic_rx_runtime_config_param_p;

typedef struct {
    uint32_t iso_interval; /* Unit with BT clock (312.5us) */
    uint32_t dl_timestamp_clk; /* Unit with BT clock (312.5us), indicate the first anchor of DL */
    uint32_t dl_retransmission_window_clk; /* Unit with BT clock (312.5us), valid bit[27:2] */
    uint16_t dl_timestamp_phase; /* Unit with 0.5us, valid value: 0~2499 */
    uint16_t dl_retransmission_window_phase; /* Unit with 0.5us, valid value: 0~2499 */
    uint8_t  dl_ft;
    uint8_t  dl_packet_counter; /* ISO DL packet counter & 0xFF */
    uint8_t  ul_ft;
    uint8_t  ul_packet_counter; /* ISO UL packet counter & 0xFF */
    uint32_t ul_timestamp; /* Unit with BT clock (312.5us), indicate the first anchor of UL */
    uint32_t iso_anchor_clock;
    uint16_t iso_anchor_phase;
    uint32_t ul_avm_info_addr;
    uint32_t dl_avm_info_addr;
} wireless_mic_rx_init_play_info_t;

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static wireless_mic_rx_bt_info_t wireless_mic_rx_ul_bt_info[WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER];
static wireless_mic_rx_init_play_info_t wireless_mic_rx_bt_init_play_info =
{
    .dl_retransmission_window_clk = UL_BT_RETRY_WINDOW_FRAME_5000US,
    .dl_retransmission_window_phase = 0,
};
static uint32_t wireless_mic_rx_sw_timer = 0;
static uint32_t wireless_mic_rx_sw_timer_count = 0;
static uint32_t wireless_mic_rx_packet_anchor_in_mixer = 0;
static uint32_t wireless_mic_rx_packet_size_in_mixer = 0;
static uint8_t  wireless_mic_rx_channel_connection_status[AUDIO_TRANSMITTER_WIRELESS_MIC_RX_SUB_ID_MAX][WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER] =
{
    {0b0101, 0b1010, 0b0000, 0b0000},
    {0b0101, 0b1010, 0b0000, 0b0000},
    {0b0101, 0b1010, 0b0000, 0b0000},
    {0b0101, 0b1010, 0b0000, 0b0000}
};

/* Public variables ----------------------------------------------------------*/
wireless_mic_rx_ul_handle_t *wireless_mic_rx_first_ul_handle = NULL;

/* Private functions ---------------------------------------------------------*/
EXTERN VOID StreamDSP_HWSemaphoreTake(VOID);
EXTERN VOID StreamDSP_HWSemaphoreGive(VOID);
extern VOID audio_transmitter_share_information_update_read_offset(SOURCE source, U32 ReadOffset);
extern VOID audio_transmitter_share_information_fetch(SOURCE source, SINK sink);
extern VOID audio_transmitter_share_information_update_write_offset(SINK sink, U32 WriteOffset);
extern void bt_common_share_information_fetch(SOURCE source, SINK sink);
extern void bt_common_share_information_update_read_offset(SOURCE source, U32 ReadOffset);
extern VOID audio_transmitter_share_information_fetch(SOURCE source, SINK sink);
extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase, BT_CLOCK_OFFSET_SCENARIO type);

static uint32_t wireless_mic_rx_codec_get_frame_size(audio_dsp_codec_type_t *codec_type, audio_codec_param_t *codec_param)
{
    uint32_t frame_size = 0;

    switch (*codec_type)
    {
        case AUDIO_DSP_CODEC_TYPE_LC3:
            frame_size = codec_param->lc3.bit_rate * codec_param->lc3.frame_interval / 8 / 1000 / 1000;
            break;

        case AUDIO_DSP_CODEC_TYPE_LC3PLUS:
            frame_size = codec_param->lc3plus.bit_rate * codec_param->lc3plus.frame_interval / 8 / 1000 / 1000;
            break;

        default:
            DSP_MW_LOG_E("[Wireless MIC RX][ERROR]This codec is not supported at now, %u\r\n", 1, *codec_type);
            AUDIO_ASSERT(0);
            break;
    }

    return frame_size;
}

static uint32_t usb_audio_get_frame_size(audio_dsp_codec_type_t *usb_type, audio_codec_param_t *usb_param)
{
    uint32_t frame_size = 0;
    uint32_t samples = 0;
    uint32_t channel_num = 0;
    uint32_t resolution_size = 0;

    if (*usb_type == AUDIO_DSP_CODEC_TYPE_PCM)
    {
        frame_size = 1;

        switch (usb_param->pcm.frame_interval)
        {
            case 1000:
                break;

            default:
                AUDIO_ASSERT(0);
                break;
        }

        switch (usb_param->pcm.sample_rate)
        {
            case 44100:
                samples = 44;
                break;

            case 16000:
            case 32000:
            case 48000:
            case 96000:
                samples = usb_param->pcm.sample_rate/1000;
                break;

            default:
                AUDIO_ASSERT(0);
                break;
        }

        switch (usb_param->pcm.channel_mode)
        {
            case 1:
                channel_num = 1;
                break;

            case 2:
                channel_num = 2;
                break;

            case 8:
                channel_num = 8;
                break;

            default:
                AUDIO_ASSERT(0);
                break;
        }

        switch (usb_param->pcm.format)
        {
            case AFE_PCM_FORMAT_S16_LE:
                resolution_size = 2;
                break;

            case AFE_PCM_FORMAT_S24_LE:
                resolution_size = 3;
                break;

            default:
                AUDIO_ASSERT(0);
                break;
        }
    }
    else
    {
        frame_size = 0;
    }

    frame_size = frame_size * samples * channel_num * resolution_size;

    return frame_size;
}

ATTR_TEXT_IN_IRAM static void ShareBufferCopy_D_32bit_to_D_24bit_1ch(uint32_t* src_buf, uint8_t* dest_buf1, uint32_t samples)
{
    uint32_t data;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        data = src_buf[i];
        *(dest_buf1+i*3+0) = (uint8_t)((data>> 8)&0xff);
        *(dest_buf1+i*3+1) = (uint8_t)((data>>16)&0xff);
        *(dest_buf1+i*3+2) = (uint8_t)((data>>24)&0xff);
    }
}

ATTR_TEXT_IN_IRAM static void ShareBufferCopy_D_32bit_to_I_24bit_2ch(uint32_t* src_buf1, uint32_t* src_buf2, uint8_t* dest_buf1, uint32_t samples)
{
    uint32_t data1;
    uint32_t data2;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        data1 = src_buf1[i];
        data2 = src_buf2[i];
        *(dest_buf1+i*6+0) = (uint8_t)((data1>> 8)&0xff);
        *(dest_buf1+i*6+1) = (uint8_t)((data1>>16)&0xff);
        *(dest_buf1+i*6+2) = (uint8_t)((data1>>24)&0xff);
        *(dest_buf1+i*6+3) = (uint8_t)((data2>> 8)&0xff);
        *(dest_buf1+i*6+4) = (uint8_t)((data2>>16)&0xff);
        *(dest_buf1+i*6+5) = (uint8_t)((data2>>24)&0xff);
    }
}

ATTR_TEXT_IN_IRAM static void ShareBufferCopy_D_16bit_to_D_16bit_1ch(uint16_t* src_buf, uint16_t* dest_buf1, uint32_t samples)
{
    uint16_t data;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        data = src_buf[i];
        *(dest_buf1+i) = data;
    }
}

ATTR_TEXT_IN_IRAM static void ShareBufferCopy_D_16bit_to_I_16bit_2ch(uint16_t* src_buf1, uint16_t* src_buf2, uint16_t* dest_buf1, uint32_t samples)
{
    uint32_t data1;
    uint32_t data2;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        data1 = src_buf1[i];
        data2 = src_buf2[i];
        data1 = (data1) | (data2 << 16);
        *((uint32_t* )dest_buf1+i) = data1;
    }
}

ATTR_TEXT_IN_IRAM static void ShareBufferCopy_D_32bit_to_D_16bit_1ch(uint32_t* src_buf, uint16_t* dest_buf1, uint32_t samples)
{
    uint32_t data;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        data = src_buf[i];
        *(dest_buf1+i) = (uint16_t)((data >> 16)&0xffff);
    }
}

ATTR_TEXT_IN_IRAM static void ShareBufferCopy_D_32bit_to_I_16bit_2ch(uint32_t* src_buf1, uint32_t* src_buf2, uint16_t* dest_buf1, uint32_t samples)
{
    uint32_t data1;
    uint32_t data2;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        data1 = src_buf1[i];
        data2 = src_buf2[i];
        data1 = ((data1 >> 16)&0x0000ffff) | ((data2 >> 0)&0xffff0000);
        *((uint32_t* )dest_buf1+i) = data1;
    }
}

ATTR_TEXT_IN_IRAM static void ShareBufferCopy_D_16bit_to_I_24bit_2ch(uint16_t* src_buf1, uint16_t* src_buf2, uint8_t* dest_buf1, uint32_t samples)
{
    uint32_t data32;
    uint16_t data16;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        if ((i%2) == 0)
        {
            data32 = (src_buf1[i]<<8); // 0x00XXXX00
            data16 = src_buf2[i]; //0xXXXX
            *(uint32_t *)(dest_buf1 + i*6) = data32;
            *(uint16_t *)(dest_buf1 + i*6 + 4) = data16;
        }
        else
        {
            data16 = (src_buf1[i]&0x00ff)<<8; //0xXX00
            data32 = (src_buf2[i]<<16) | ((src_buf1[i]&0xff00)>>8); // 0xXXXX00XX
            *(uint16_t *)(dest_buf1 + i*6) = data16;
            *(uint32_t *)(dest_buf1 + i*6 + 2) = data32;
        }
    }
}

ATTR_TEXT_IN_IRAM static void ShareBufferCopy_D_16bit_to_D_24bit_1ch(uint16_t* src_buf1, uint8_t* dest_buf1, uint32_t samples)
{
    uint32_t data32;
    uint16_t data16;
    uint32_t i;

    for (i = 0; i < samples; i++)
    {
        if ((i%2) == 0)
        {
            if (i != (samples - 1))
            {
                data32 = (src_buf1[i]<<8); // 0x00XXXX00
                *(uint32_t *)(dest_buf1 + (i/2)*6) = data32;
            }
            else
            {
                /* prevent overflow */
                data32 = (src_buf1[i]<<8); // 0x00XXXX00
                *(uint16_t *)(dest_buf1 + (i/2)*6) = (uint16_t)(data32&0x0000ffff);
                *(uint8_t *)(dest_buf1 + (i/2)*6 + 2) = (uint8_t)((data32&0x00ff0000)>>16);
            }
        }
        else
        {
            data16 = src_buf1[i]; //0xXXXX
            *(uint16_t *)(dest_buf1 + (i/2)*6 + 4) = data16;
        }
    }
}

/******************************************************************************/
/*               ULL audio 2.0 dongle UL path Private Functions               */
/******************************************************************************/
static wireless_mic_rx_ul_handle_t *wireless_mic_rx_ul_get_handle(void *owner)
{
    uint32_t saved_mask;
    int16_t count;
    wireless_mic_rx_ul_handle_t *rx_handle = NULL;
    wireless_mic_rx_ul_handle_t *c_handle = NULL;

    rx_handle = malloc(sizeof(wireless_mic_rx_ul_handle_t));
    if (rx_handle == NULL)
    {
        AUDIO_ASSERT(0);
    }
    memset(rx_handle, 0, sizeof(wireless_mic_rx_ul_handle_t));

    rx_handle->total_number = -1;
    rx_handle->index = -1;
    rx_handle->owner = owner;
    rx_handle->next_ul_handle = NULL;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (wireless_mic_rx_first_ul_handle == NULL)
    {
        /* there is no item on the handle list */
        rx_handle->total_number = 1;
        rx_handle->index = 1;

        /* set this handle as the head of the handle list */
        wireless_mic_rx_first_ul_handle = rx_handle;
    }
    else
    {
        /* there are other items on the handle list */
        count = 1;
        c_handle = wireless_mic_rx_first_ul_handle;
        while (c_handle != NULL)
        {
            count += 1;

            c_handle->total_number += 1;
            if (c_handle->total_number <= 0)
            {
                /* error status */
                AUDIO_ASSERT(0);
            }

            if (c_handle->next_ul_handle == NULL)
            {
                /* c_handle is the last item on the list now */
                rx_handle->total_number = c_handle->total_number;
                rx_handle->index = c_handle->total_number;

                /* rx_handle is the last item on the list now */
                c_handle->next_ul_handle = rx_handle;

                break;
            }

            c_handle = c_handle->next_ul_handle;
        }
        if ((c_handle == NULL) || (rx_handle->total_number != count))
        {
            AUDIO_ASSERT(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return rx_handle;
}

static void wireless_mic_rx_ul_release_handle(wireless_mic_rx_ul_handle_t *handle)
{
    uint32_t saved_mask, i, count;
    wireless_mic_rx_ul_handle_t *l_handle = NULL;
    wireless_mic_rx_ul_handle_t *c_handle = NULL;
    wireless_mic_rx_ul_handle_t *rx_handle = NULL;

    if ((handle == NULL) || (wireless_mic_rx_first_ul_handle == NULL))
    {
        AUDIO_ASSERT(0);
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (wireless_mic_rx_first_ul_handle->total_number <= 0)
    {
        /* error status */
        AUDIO_ASSERT(0);
    }
    else if ((wireless_mic_rx_first_ul_handle->total_number == 1) &&
            (wireless_mic_rx_first_ul_handle == handle))
    {
        /* this handle is only one item on handle list */
        if (wireless_mic_rx_first_ul_handle->next_ul_handle != NULL)
        {
            AUDIO_ASSERT(0);
        }

        /* there is no item on the handle list */
        wireless_mic_rx_first_ul_handle = NULL;
    }
    else if ((wireless_mic_rx_first_ul_handle->total_number > 1) &&
            (wireless_mic_rx_first_ul_handle == handle))
    {
        /* this handle is the first item on handle list, but there are other handles on the list */
        c_handle = wireless_mic_rx_first_ul_handle;
        count = wireless_mic_rx_first_ul_handle->total_number;
        for (i=0; i < count; i++)
        {
            c_handle->total_number -= 1;
            c_handle->index -= 1;

            c_handle = c_handle->next_ul_handle;
        }
        if (c_handle != NULL)
        {
            /* error status */
            AUDIO_ASSERT(0);
        }

        /* change the next iten to the head */
        wireless_mic_rx_first_ul_handle = handle->next_ul_handle;
    }
    else
    {
        /* this handle is the middle item on handle list */
        c_handle = wireless_mic_rx_first_ul_handle;
        count = wireless_mic_rx_first_ul_handle->total_number;
        if (count == 1)
        {
            /* error status */
            AUDIO_ASSERT(0);
        }
        for (i=0; i < count; i++)
        {
            if (c_handle == handle)
            {
                /* find out the handle on the list */
                rx_handle = handle;
                l_handle->next_ul_handle = c_handle->next_ul_handle;
            }

            if (rx_handle == NULL)
            {
                c_handle->total_number -= 1;
            }
            else
            {
                c_handle->total_number -= 1;
                c_handle->index -= 1;
            }

            l_handle = c_handle;
            c_handle = c_handle->next_ul_handle;
        }
        if ((c_handle != NULL) || (rx_handle == NULL))
        {
            /* error status */
            AUDIO_ASSERT(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    free(handle);
}

ATTR_TEXT_IN_IRAM static bool wireless_mic_rx_ul_play_time_is_arrived(wireless_mic_rx_ul_handle_t *rx_handle, uint32_t bt_clk)
{
    bool ret = false;

    if ((rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED) || (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_READY))
    {
        if (rx_handle->source_info.bt_in.first_anchor_bt_clk < rx_handle->source_info.bt_in.play_en_bt_clk)
        {
            if (bt_clk >= rx_handle->source_info.bt_in.play_en_bt_clk)
            {
                /* --------- ........ --------- anchor ---------- bt_clk ---------- playen --------- ........ -------- */
                ret = true;
            }
            else if ((bt_clk < rx_handle->source_info.bt_in.play_en_bt_clk) && (bt_clk < rx_handle->source_info.bt_in.first_anchor_bt_clk))
            {
                /* ---------- bt_clk --------- ........ --------- anchor  --------- ........ ---------- playen -------- */
                ret = true;
            }
        }
        else
        {
            if ((bt_clk >= rx_handle->source_info.bt_in.play_en_bt_clk) && (bt_clk < rx_handle->source_info.bt_in.first_anchor_bt_clk))
            {
                /* ---------- playen ---------- bt_clk --------- ........ --------- anchor  --------- ........  -------- */
                ret = true;
            }
        }
    }

    return ret;
}

ATTR_TEXT_IN_IRAM static void wireless_mic_rx_ul_ccni_handler(hal_ccni_event_t event, void * msg)
{
    SOURCE source;
    hal_ccni_message_t *ccni_msg = msg;
    wireless_mic_rx_ul_handle_t *c_handle;
    uint32_t saved_mask;
    uint32_t gpt_count;
    uint32_t mcu_gpt_count;
    uint32_t mcu_frame_count;
    uint32_t bt_clk;
    uint16_t bt_phase;
    uint32_t write_offset;
    uint32_t i;
    UNUSED(event);

    /* check if there is any UL stream */
    if (wireless_mic_rx_first_ul_handle == NULL)
    {
        DSP_MW_LOG_W("[Wireless MIC RX][UL][WARNNING]ul rx_handle is NULL\r\n", 0);
        goto _ccni_return;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    /* get timestamp for debug */
    mcu_gpt_count = ccni_msg->ccni_message[0];
    mcu_frame_count = ccni_msg->ccni_message[1];
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);
    hal_nvic_restore_interrupt_mask(saved_mask);

    /* trigger all started UL stream one by one */
    c_handle = wireless_mic_rx_first_ul_handle;
    for (i = 0; i < (uint32_t)wireless_mic_rx_first_ul_handle->total_number; i++)
    {
        if ((c_handle->stream_status == WIRELESS_MIC_RX_STREAM_START) || (c_handle->stream_status == WIRELESS_MIC_RX_STREAM_RUNNING))
        {
            switch (c_handle->sub_id)
            {
                case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
                    /* get source */
                    source = (SOURCE)c_handle->owner;
                    if ((source == NULL) || (source->transform == NULL))
                    {
                        break;
                    }
                    /* set timestamp for debug */
                    MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);
                    if (wireless_mic_rx_ul_fetch_time_is_arrived(c_handle, bt_clk, bt_phase))
                    {
                        c_handle->ccni_in_bt_count  = mcu_gpt_count;
                        c_handle->ccni_in_gpt_count = gpt_count;
                    }
                    /* save the unprocessed frame count in the share buffer from the mcu side */
                    c_handle->sink_info.usb_out.mcu_frame_count = mcu_frame_count;
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    /* increase fetch count */
                    if (c_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY)
                    {
                        /* trigger stream to find out the play time */
                        c_handle->fetch_count = 1;
                    }
                    else if (c_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_READY)
                    {
                        /* check if the current bt clock is play time */
                        MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);
                        if (wireless_mic_rx_ul_play_time_is_arrived(c_handle, bt_clk))
                        {
                            c_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED;
                            /* update share buffer write offset here for more accurate latency */
                            audio_transmitter_share_information_fetch(NULL, source->transform->sink);
                            write_offset = (source->transform->sink->streamBuffer.ShareBufferInfo.write_offset + source->transform->sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_size * c_handle->process_frames) % (source->transform->sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_size * source->transform->sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_num);
                            audio_transmitter_share_information_update_write_offset(source->transform->sink, write_offset);
                            c_handle->process_frames = 0;
                            c_handle->fetch_count = 0;
                            DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x]stream is played, %d, 0x%x, 0x%x\r\n", 4, c_handle, c_handle->process_frames, bt_clk, write_offset);
                        }
                    }
                    else if (c_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED)
                    {
                        /* trigger stream do something */
                        c_handle->fetch_count = 0;
                    }
                    else
                    {
                        c_handle->fetch_count = 0;
                    }
                    hal_nvic_restore_interrupt_mask(saved_mask);
                    /* Handler the stream */
                    AudioCheckTransformHandle(source->transform);
                    break;

                case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
                case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
                    break;

                default:
                    AUDIO_ASSERT(0);
                    break;
            }
        }
        /* switch to the next dl stream */
        c_handle = c_handle->next_ul_handle;
    }
    if (c_handle != NULL)
    {
        AUDIO_ASSERT(0);
    }

_ccni_return:
    return;
}

ATTR_TEXT_IN_IRAM static void wireless_mic_rx_software_timer_handler(void *user_data)
{
    uint32_t saved_mask;
    uint32_t gpt_count;
    wireless_mic_rx_ul_handle_t *c_handle;
    SOURCE source;
    uint32_t i;
    UNUSED(user_data);

    /* repeat this timer */
    hal_gpt_sw_start_timer_us(wireless_mic_rx_sw_timer, 1000, wireless_mic_rx_software_timer_handler, NULL);

    /* check if there is any UL stream */
    if (wireless_mic_rx_first_ul_handle == NULL)
    {
        DSP_MW_LOG_W("[Wireless MIC RX][UL][WARNNING]ul rx_handle is NULL\r\n", 0);
        return;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    /* get timestamp for debug */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);
    hal_nvic_restore_interrupt_mask(saved_mask);

    /* trigger all started UL stream one by one */
    c_handle = wireless_mic_rx_first_ul_handle;
    for (i = 0; i < (uint32_t)wireless_mic_rx_first_ul_handle->total_number; i++)
    {
        if ((c_handle->stream_status == WIRELESS_MIC_RX_STREAM_START) || (c_handle->stream_status == WIRELESS_MIC_RX_STREAM_RUNNING))
        {
            switch (c_handle->sub_id)
            {
                case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
                case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
                    /* get source */
                    source = (SOURCE)c_handle->owner;
                    if ((source == NULL) || (source->transform == NULL))
                    {
                        break;
                    }
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    /* increase fetch count */
                    if (c_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY)
                    {
                        /* set timestamp for debug */
                        c_handle->ccni_in_bt_count  = gpt_count;
                        c_handle->ccni_in_gpt_count = gpt_count;
                        /* trigger stream to find out the play time */
                        c_handle->fetch_count = 1;
                        /* Handler the stream */
                        AudioCheckTransformHandle(source->transform);
                    }
                    hal_nvic_restore_interrupt_mask(saved_mask);
                    break;

                case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
                    break;

                default:
                    AUDIO_ASSERT(0);
                    break;
            }
        }
        /* switch to the next dl stream */
        c_handle = c_handle->next_ul_handle;
    }
    if (c_handle != NULL)
    {
        AUDIO_ASSERT(0);
    }
}

static void wireless_mic_rx_software_timer_start(wireless_mic_rx_ul_handle_t *rx_handle)
{
    uint32_t saved_mask;
    UNUSED(rx_handle);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (wireless_mic_rx_sw_timer_count == 0)
    {
        hal_gpt_sw_get_timer(&wireless_mic_rx_sw_timer);
        hal_gpt_sw_start_timer_us(wireless_mic_rx_sw_timer, 1000, wireless_mic_rx_software_timer_handler, NULL);
    }
    wireless_mic_rx_sw_timer_count += 1;
    if (wireless_mic_rx_sw_timer_count == 0)
    {
        AUDIO_ASSERT(0);
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[Wireless MIC RX][UL]software timer start, %d\r\n", 1, wireless_mic_rx_sw_timer_count);
}

static void wireless_mic_rx_software_timer_stop(wireless_mic_rx_ul_handle_t *rx_handle)
{
    uint32_t saved_mask;
    UNUSED(rx_handle);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (wireless_mic_rx_sw_timer_count == 0)
    {
        AUDIO_ASSERT(0);
    }
    wireless_mic_rx_sw_timer_count -= 1;
    if (wireless_mic_rx_sw_timer_count == 0)
    {
        hal_gpt_sw_stop_timer_ms(wireless_mic_rx_sw_timer);
        hal_gpt_sw_free_timer(wireless_mic_rx_sw_timer);
        wireless_mic_rx_sw_timer = 0;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[Wireless MIC RX][UL]software timer stop, %d\r\n", 1, wireless_mic_rx_sw_timer_count);
}

ATTR_TEXT_IN_IRAM void wireless_mic_rx_ul_mixer_precallback(sw_mixer_member_t *member, void *para)
{
    uint32_t saved_mask;
    SOURCE source;
    wireless_mic_rx_ul_handle_t *rx_handle;
    wireless_mic_rx_ul_handle_t *c_handle;
    uint32_t i;
    uint8_t *stream_ch_buffer;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (wireless_mic_rx_first_ul_handle->total_number > 1)
    {
        source = (SOURCE)(member->owner);
        rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
        if (rx_handle->data_status == WIRELESS_MIC_RX_UL_DATA_IN_MIXER)
        {
            /*  because the required ul packet data are in other stream's mixer and the decoder output of the currnet stream size is 0,
                we need to set the frame size of the current stream to the effective size for the following features are running correctly */
            stream_function_modify_output_size(para, wireless_mic_rx_packet_size_in_mixer);
            /*  because the required ul packet data are in other stream's mixer,
                we need to set the channel data of the current stream to 0, then the effective data in other stream's mixer will mixed with these 0 data */
            for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
            {
                stream_ch_buffer = stream_function_get_inout_buffer(para, i + 1);
                memset(stream_ch_buffer, 0, wireless_mic_rx_packet_size_in_mixer);
            }
        }
        else if (rx_handle->data_status == WIRELESS_MIC_RX_UL_DATA_NORMAL)
        {
            /*  because the required ul packet data are in the current stream's mixer,
                we need to record the newest packet size for the other stream */
            wireless_mic_rx_packet_size_in_mixer = stream_function_get_output_size(para);
            /*  because the required ul packet data are in the current stream's mixer,
                we need to clean other streams input channel and the oldest data in the other streams will not be mixed with the current stream */
            c_handle = wireless_mic_rx_first_ul_handle;
            for (i = 0; i < (uint32_t)wireless_mic_rx_first_ul_handle->total_number; i++)
            {
                if (c_handle != rx_handle)
                {
                    stream_function_sw_mixer_member_input_buffers_clean(c_handle->mixer_member, false);
                }
                c_handle = c_handle->next_ul_handle;
            }
        }
        else if (rx_handle->data_status == WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER_BUT_UPDATE_FETCH)
        {
            /*  output 5ms silence */
            stream_function_modify_output_size(para, wireless_mic_rx_packet_size_in_mixer);
            for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
            {
                stream_ch_buffer = stream_function_get_inout_buffer(para, i + 1);
                memset(stream_ch_buffer, 0, wireless_mic_rx_packet_size_in_mixer);
            }
            /* forced to bypass all following sw mixer operations */
            stream_function_sw_mixer_member_force_to_exit(member);
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);
}

ATTR_TEXT_IN_IRAM void wireless_mic_rx_ul_mixer_do_connections(wireless_mic_rx_ul_handle_t *rx_handle, uint8_t *channel_connection_status)
{
    int16_t i, j, k;
    uint32_t connection_status;
    uint32_t main_channel_num;
    wireless_mic_rx_ul_handle_t *c_handle;

    /* do connection one by one channel */
    for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
    {
        /* process the current stream's connection */
        connection_status = channel_connection_status[i];
        if (connection_status == 0)
        {
            /* input channel number is 0, process the next status */
            continue;
        }

        /* find out main input channel and do connections */
        main_channel_num = 0;
        if ((connection_status&(0x1<<i)) != 0)
        {
            main_channel_num = i+1;
            connection_status = connection_status&(~(0x1<<i));
        }
        else
        {
            for (j = 0; j < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; j++)
            {
                if ((connection_status&(0x1<<j)) != 0)
                {
                    main_channel_num = j+1;
                    connection_status = connection_status&(~(0x1<<j));
                    break;
                }
            }
        }
        stream_function_sw_mixer_channel_connect(rx_handle->mixer_member, main_channel_num, SW_MIXER_CHANNEL_ATTRIBUTE_MAIN, rx_handle->mixer_member, i+1);
        /* process other input channel and do connections*/
        if (connection_status != 0)
        {
            for (j = 0; j < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; j++)
            {
                if ((connection_status&(0x1<<j)) != 0)
                {
                    stream_function_sw_mixer_channel_connect(rx_handle->mixer_member, j+1, SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL, rx_handle->mixer_member, i+1);
                }
            }
        }

        /* process the other streams, maybe the decoder output are in other stream */
        if (wireless_mic_rx_first_ul_handle->total_number > 1)
        {
            c_handle = wireless_mic_rx_first_ul_handle;
            for (j = 0; j < wireless_mic_rx_first_ul_handle->total_number; j++)
            {
                connection_status = channel_connection_status[i];
                if (c_handle != rx_handle)
                {
                    for (k = 0; k < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; k++)
                    {
                        if ((connection_status&(0x1<<k)) != 0)
                        {
                            stream_function_sw_mixer_channel_connect(c_handle->mixer_member, k+1, SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL, rx_handle->mixer_member, i+1);
                        }
                    }
                }
                c_handle = c_handle->next_ul_handle;
            }
        }
    }
}

ATTR_TEXT_IN_IRAM void wireless_mic_rx_ul_mixer_all_stream_connections_update(wireless_mic_rx_ul_handle_t *rx_handle, bool new_stream)
{
    int16_t i;
    wireless_mic_rx_ul_handle_t *c_handle;
    uint32_t saved_mask;
    uint8_t *channel_connection_status;

    if (new_stream)
    {
        /* not only update this new stream's connection status, but also connect this stream into other stream */
        c_handle = wireless_mic_rx_first_ul_handle;
        for (i = 0; i < wireless_mic_rx_first_ul_handle->total_number; i++)
        {
            channel_connection_status = wireless_mic_rx_channel_connection_status[c_handle->sub_id];
            wireless_mic_rx_ul_mixer_do_connections(c_handle, channel_connection_status);
            /* switch the next stream */
            c_handle = c_handle->next_ul_handle;
        }
    }
    else
    {
        /* only update this stream's connection status */
        channel_connection_status = wireless_mic_rx_channel_connection_status[rx_handle->sub_id];
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        stream_function_sw_mixer_output_channel_disconnect_all(rx_handle->mixer_member);
        hal_nvic_restore_interrupt_mask(saved_mask);
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        wireless_mic_rx_ul_mixer_do_connections(rx_handle, channel_connection_status);
        hal_nvic_restore_interrupt_mask(saved_mask);
    }

    DSP_MW_LOG_I("[Wireless MIC RX]][UL][stream_connections_update] update stream %d connection status: %u, 0x%x, 0x%x, 0x%x, 0x%x.",
                6,
                new_stream,
                rx_handle->sub_id,
                wireless_mic_rx_channel_connection_status[rx_handle->sub_id][0],
                wireless_mic_rx_channel_connection_status[rx_handle->sub_id][1],
                wireless_mic_rx_channel_connection_status[rx_handle->sub_id][2],
                wireless_mic_rx_channel_connection_status[rx_handle->sub_id][3]);
}

static void wireless_mic_rx_ul_bt_in_init(wireless_mic_rx_ul_handle_t *rx_handle, audio_transmitter_open_param_p audio_transmitter_open_param, bt_common_open_param_p bt_common_open_param)
{
    uint32_t i, j;
    uint32_t frame_interval;
    uint32_t sample_rate;
    hal_audio_format_t sample_format;
    uint32_t frame_samples;
    uint32_t frame_size;
    uint32_t saved_mask;
    uint32_t bit_rate;
    uint32_t channel_num = 0;
    uint32_t channel_mode;
    audio_dsp_codec_type_t codec_type;
    UNUSED(audio_transmitter_open_param);

    for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
    {
        if (bt_common_open_param->scenario_param.wireless_mic_rx_param.bt_link_info[i].share_info == NULL)
        {
            continue;
        }
        else
        {
            /* update channel num */
            channel_num += 1;
            /* update bt link settings to global state machine */
            for (j = 0; j < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; j++)
            {
                if (hal_memview_cm4_to_dsp0((uint32_t)(bt_common_open_param->scenario_param.wireless_mic_rx_param.bt_link_info[i].share_info)) == (uint32_t)(wireless_mic_rx_ul_bt_info[j].bt_link_info.share_info))
                {
                    /* check bt link settings */
                    if (memcmp(&(bt_common_open_param->scenario_param.wireless_mic_rx_param.bt_link_info[i].codec_type), &(wireless_mic_rx_ul_bt_info[j].bt_link_info.codec_type), sizeof(wireless_mic_rx_bt_link_info_t)-sizeof(void *)) != 0)
                    {
                        /* same share buffer, same codec settings */
                        DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]codec setting is different\r\n", 0);
                        AUDIO_ASSERT(0);
                    }
                    /* in here, it means this bt link's setting has been used */
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    wireless_mic_rx_ul_bt_info[j].user_count += 1;
                    hal_nvic_restore_interrupt_mask(saved_mask);
                    rx_handle->source_info.bt_in.bt_info[i] = &wireless_mic_rx_ul_bt_info[j];
                }
            }
            if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
            {
                /* in here, it means this bt link's setting has not been used */
                for (j = 0; j < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; j++)
                {
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    if (wireless_mic_rx_ul_bt_info[j].bt_link_info.share_info == NULL)
                    {
                        if (wireless_mic_rx_ul_bt_info[j].user_count != 0)
                        {
                            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]user count is not right, %u\r\n", 1, wireless_mic_rx_ul_bt_info[j].user_count);
                            AUDIO_ASSERT(0);
                        }
                        wireless_mic_rx_ul_bt_info[j].user_count += 1;
                        wireless_mic_rx_ul_bt_info[j].seq_num = 0;
                        wireless_mic_rx_ul_bt_info[j].blk_index = 0;
                        wireless_mic_rx_ul_bt_info[j].blk_index_previous = 0;
                        memcpy(&(wireless_mic_rx_ul_bt_info[j].bt_link_info), &(bt_common_open_param->scenario_param.wireless_mic_rx_param.bt_link_info[i]), sizeof(wireless_mic_rx_bt_link_info_t));
                        wireless_mic_rx_ul_bt_info[j].bt_link_info.share_info = (void *)hal_memview_cm4_to_dsp0((uint32_t)(wireless_mic_rx_ul_bt_info[j].bt_link_info.share_info));
                        rx_handle->source_info.bt_in.bt_info[i] = &wireless_mic_rx_ul_bt_info[j];
                        hal_nvic_restore_interrupt_mask(saved_mask);
                        break;
                    }
                    hal_nvic_restore_interrupt_mask(saved_mask);
                }
                if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
                {
                    /* not found suitable bt info channel */
                    DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]all bt info are used\r\n", 0);
                    AUDIO_ASSERT(0);
                }
            }
            if ((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_type == AUDIO_DSP_CODEC_TYPE_LC3PLUS)
            {
                codec_type     = AUDIO_DSP_CODEC_TYPE_LC3PLUS;
                bit_rate       = (rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_param.lc3plus.bit_rate;
                frame_interval = (rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_param.lc3plus.frame_interval;
                sample_rate    = (rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_param.lc3plus.sample_rate;
                sample_format  = (rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_param.lc3plus.sample_format;
                channel_mode   = (rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_param.lc3plus.channel_mode;
                frame_samples  = sample_rate/1000*frame_interval/1000;
                frame_size     = wireless_mic_rx_codec_get_frame_size(&((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_type), &((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_param));
            }
            else
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]codec is not supported, %d\r\n", 1, (rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.codec_type);
                AUDIO_ASSERT(0);
            }
            /* update codec type */
            if (rx_handle->source_info.bt_in.codec_type == 0)
            {
                rx_handle->source_info.bt_in.codec_type = codec_type;
            }
            else if (rx_handle->source_info.bt_in.codec_type != codec_type)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]codec_type is not right, %u, %u\r\n", 2, rx_handle->source_info.bt_in.codec_type, codec_type);
                AUDIO_ASSERT(0);
            }
            /* update frame interval */
            if (rx_handle->source_info.bt_in.frame_interval == 0)
            {
                rx_handle->source_info.bt_in.frame_interval = frame_interval;
            }
            else if (rx_handle->source_info.bt_in.frame_interval != frame_interval)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]frame_interval is not right, %u, %u\r\n", 2, rx_handle->source_info.bt_in.frame_interval, frame_interval);
                AUDIO_ASSERT(0);
            }
            /* update frame sample rate */
            if (rx_handle->source_info.bt_in.sample_rate == 0)
            {
                rx_handle->source_info.bt_in.sample_rate = sample_rate;
            }
            else if (rx_handle->source_info.bt_in.sample_rate != sample_rate)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]sample_rate is not right, %u, %u\r\n", 2, rx_handle->source_info.bt_in.sample_rate, sample_rate);
                AUDIO_ASSERT(0);
            }
            /* update frame sample format */
            if (rx_handle->source_info.bt_in.sample_format == 0)
            {
                rx_handle->source_info.bt_in.sample_format = sample_format;
            }
            else if (rx_handle->source_info.bt_in.sample_format != sample_format)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]sample_format is not right, %u, %u\r\n", 2, rx_handle->source_info.bt_in.sample_format, sample_format);
                AUDIO_ASSERT(0);
            }
            /* update frame samples */
            if (rx_handle->source_info.bt_in.frame_samples == 0)
            {
                rx_handle->source_info.bt_in.frame_samples = frame_samples;
            }
            else if (rx_handle->source_info.bt_in.frame_samples != frame_samples)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]frame_samples is not right, %u, %u\r\n", 2, rx_handle->source_info.bt_in.frame_samples, frame_samples);
                AUDIO_ASSERT(0);
            }
            /* update frame samples */
            if (rx_handle->source_info.bt_in.frame_size == 0)
            {
                rx_handle->source_info.bt_in.frame_size = frame_size;
            }
            else if (rx_handle->source_info.bt_in.frame_size != frame_size)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]frame_size is not right, %u, %u\r\n", 2, rx_handle->source_info.bt_in.frame_size, frame_size);
                AUDIO_ASSERT(0);
            }
            /* update bit_rate */
            if (rx_handle->source_info.bt_in.bit_rate == 0)
            {
                rx_handle->source_info.bt_in.bit_rate = bit_rate;
            }
            else if (rx_handle->source_info.bt_in.bit_rate != bit_rate)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]bit_rate is not right, %u, %u\r\n", 2, rx_handle->source_info.bt_in.bit_rate, bit_rate);
                AUDIO_ASSERT(0);
            }
            /* update bit_rate */
            if (rx_handle->source_info.bt_in.channel_mode == 0)
            {
                rx_handle->source_info.bt_in.channel_mode = channel_mode;
            }
            else if (rx_handle->source_info.bt_in.channel_mode != channel_mode)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]channel_mode is not right, %u, %u\r\n", 2, rx_handle->source_info.bt_in.channel_mode, channel_mode);
                AUDIO_ASSERT(0);
            }
        }
    }
    rx_handle->source_info.bt_in.channel_num = channel_num;
}

static void wireless_mic_rx_ul_bt_in_deinit(wireless_mic_rx_ul_handle_t *rx_handle)
{
    uint32_t i;
    uint32_t saved_mask;

    for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
    {
        if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
        {
            continue;
        }
        else
        {
            if ((rx_handle->source_info.bt_in.bt_info[i])->user_count == 0)
            {
                DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]user_count is not right, %u\r\n", 1, (rx_handle->source_info.bt_in.bt_info[i])->user_count);
                AUDIO_ASSERT(0);
            }
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            (rx_handle->source_info.bt_in.bt_info[i])->user_count -= 1;
            if ((rx_handle->source_info.bt_in.bt_info[i])->user_count == 0)
            {
                memset(rx_handle->source_info.bt_in.bt_info[i], 0, sizeof(wireless_mic_rx_bt_info_t));
            }
            rx_handle->source_info.bt_in.bt_info[i] = NULL;
            hal_nvic_restore_interrupt_mask(saved_mask);
        }
    }
}

/******************************************************************************/
/*          ULL audio 2.0 dongle UL USB-out path Private Functions            */
/******************************************************************************/
static void wireless_mic_rx_ul_usb_out_init(wireless_mic_rx_ul_handle_t *rx_handle, audio_transmitter_open_param_p audio_transmitter_open_param, bt_common_open_param_p bt_common_open_param)
{
    uint32_t i;
    uint32_t sample_size = 0;
    n9_dsp_share_info_ptr p_share_info;
    stream_resolution_t stream_resolution = RESOLUTION_16BIT;
    uint32_t saved_mask;

    /* bt in info init */
    wireless_mic_rx_ul_bt_in_init(rx_handle, audio_transmitter_open_param, bt_common_open_param);
    if (rx_handle->source_info.bt_in.frame_interval == 5000)
    {
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        rx_handle->source_info.bt_in.play_en_delay                    = UL_PLAYEN_DELAY_FRAME_5000US;
        rx_handle->source_info.bt_in.play_en_first_packet_safe_delay  = UL_FIRST_PACKET_SAFE_DELAY_FRAME_5000US;
        rx_handle->source_info.bt_in.bt_retry_window                  = wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk+wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase/625;
        rx_handle->source_info.bt_in.bt_interval                      = UL_BT_INTERVAL_FRAME_5000US;
        rx_handle->source_info.bt_in.bt_channel_anchor_diff           = UL_BT_INTERVAL_FRAME_5000US/2;
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
    else if (rx_handle->source_info.bt_in.frame_interval == 10000)
    {
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        rx_handle->source_info.bt_in.play_en_delay                    = UL_PLAYEN_DELAY_FRAME_10000US;
        rx_handle->source_info.bt_in.play_en_first_packet_safe_delay  = UL_FIRST_PACKET_SAFE_DELAY_FRAME_10000US;
        rx_handle->source_info.bt_in.bt_retry_window                  = wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk+wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase/625;
        rx_handle->source_info.bt_in.bt_interval                      = UL_BT_INTERVAL_FRAME_10000US;
        rx_handle->source_info.bt_in.bt_channel_anchor_diff           = UL_BT_INTERVAL_FRAME_10000US/2;
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
    else
    {
        DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]frame_interval is not supported, %u\r\n", 1, rx_handle->source_info.bt_in.frame_interval);
        AUDIO_ASSERT(0);
    }
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]bt in info, %u, %u, %u, %u, %u, %u, %u, %u, %u, %d, %u, %u, %u\r\n", 16,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->source_info.bt_in.channel_num,
                rx_handle->source_info.bt_in.channel_mode,
                rx_handle->source_info.bt_in.sample_rate,
                rx_handle->source_info.bt_in.sample_format,
                rx_handle->source_info.bt_in.frame_size,
                rx_handle->source_info.bt_in.frame_samples,
                rx_handle->source_info.bt_in.frame_interval,
                rx_handle->source_info.bt_in.bit_rate,
                rx_handle->source_info.bt_in.play_en_delay,
                rx_handle->source_info.bt_in.play_en_first_packet_safe_delay,
                rx_handle->source_info.bt_in.bt_retry_window,
                rx_handle->source_info.bt_in.bt_interval,
                rx_handle->source_info.bt_in.bt_channel_anchor_diff);
    for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
    {
        if (rx_handle->source_info.bt_in.bt_info[i] != NULL)
        {
            p_share_info = (n9_dsp_share_info_ptr)((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.share_info);
            DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]bt in channel %u info, %u, %u, %u, %u, %u, 0x%x, 0x%x\r\n", 11,
                        audio_transmitter_open_param->scenario_type,
                        audio_transmitter_open_param->scenario_sub_id,
                        rx_handle,
                        i+1,
                        (rx_handle->source_info.bt_in.bt_info[i])->seq_num,
                        (rx_handle->source_info.bt_in.bt_info[i])->user_count,
                        (rx_handle->source_info.bt_in.bt_info[i])->blk_index,
                        (rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous,
                        (rx_handle->source_info.bt_in.bt_info[i])->crc32_init,
                        p_share_info,
                        hal_memview_infrasys_to_dsp0(p_share_info->start_addr));
        }
    }

    /* usb out info init */
    rx_handle->sink_info.usb_out.channel_num       = audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_param.pcm.channel_mode;
    rx_handle->sink_info.usb_out.sample_rate       = audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_param.pcm.sample_rate;
    rx_handle->sink_info.usb_out.sample_format     = audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_param.pcm.format;
    rx_handle->sink_info.usb_out.frame_size        = usb_audio_get_frame_size(&audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_type, &audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_param);
    rx_handle->sink_info.usb_out.frame_samples     = audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_param.pcm.sample_rate/audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_param.pcm.frame_interval;
    rx_handle->sink_info.usb_out.frame_interval    = audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_param.pcm.frame_interval;
    rx_handle->sink_info.usb_out.frame_max_num     = (rx_handle->source_info.bt_in.frame_interval*2) / audio_transmitter_open_param->scenario_param.wireless_mic_rx_param.codec_param.pcm.frame_interval;
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]usb out info, %u, %u, %u, %u, %u, %u, %u\r\n", 10,
                audio_transmitter_open_param->scenario_type,
                audio_transmitter_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->sink_info.usb_out.channel_num,
                rx_handle->sink_info.usb_out.sample_rate,
                rx_handle->sink_info.usb_out.sample_format,
                rx_handle->sink_info.usb_out.frame_size,
                rx_handle->sink_info.usb_out.frame_samples,
                rx_handle->sink_info.usb_out.frame_interval,
                rx_handle->sink_info.usb_out.frame_max_num);

    /* codec init */
    if (rx_handle->source_info.bt_in.codec_type == AUDIO_DSP_CODEC_TYPE_LC3PLUS)
    {
        lc3plus_dec_port_config_t lc3plus_dec_config;
        if (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE)
        {
            lc3plus_dec_config.sample_bits  = 16;
            sample_size = sizeof(uint16_t);
            stream_resolution = RESOLUTION_16BIT;
        }
        else if (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE)
        {
            lc3plus_dec_config.sample_bits  = 24;
            sample_size = sizeof(uint32_t);
            stream_resolution = RESOLUTION_32BIT;
        }
        else
        {
            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]sample_format is not supported, %u\r\n", 1, rx_handle->source_info.bt_in.sample_format);
            AUDIO_ASSERT(0);
        }
        if (rx_handle->source_info.bt_in.channel_mode == 0x2)
        {
            /* each bt link is stereo data */
            lc3plus_dec_config.sample_rate      = rx_handle->source_info.bt_in.sample_rate;
            lc3plus_dec_config.bit_rate         = rx_handle->source_info.bt_in.bit_rate;
            lc3plus_dec_config.channel_mode     = LC3PLUS_DEC_CHANNEL_MODE_MULTI_STEREO;
            lc3plus_dec_config.in_channel_num   = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.out_channel_num  = rx_handle->source_info.bt_in.channel_num*2;
            lc3plus_dec_config.frame_interval   = rx_handle->source_info.bt_in.frame_interval;
            lc3plus_dec_config.frame_size       = rx_handle->source_info.bt_in.frame_size;
            lc3plus_dec_config.frame_samples    = rx_handle->source_info.bt_in.frame_samples;
            lc3plus_dec_config.plc_enable       = 1;
            lc3plus_dec_config.plc_method       = LC3PLUS_PLCMETH_ADV_TDC_NS;
        }
        else
        {
            lc3plus_dec_config.sample_rate      = rx_handle->source_info.bt_in.sample_rate;
            lc3plus_dec_config.bit_rate         = rx_handle->source_info.bt_in.bit_rate;
            lc3plus_dec_config.channel_mode     = LC3PLUS_DEC_CHANNEL_MODE_MULTI_MONO;
            lc3plus_dec_config.in_channel_num   = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.out_channel_num  = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.frame_interval   = rx_handle->source_info.bt_in.frame_interval;
            lc3plus_dec_config.frame_size       = rx_handle->source_info.bt_in.frame_size;
            lc3plus_dec_config.frame_samples    = rx_handle->source_info.bt_in.frame_samples;
            lc3plus_dec_config.plc_enable       = 1;
            lc3plus_dec_config.plc_method       = LC3PLUS_PLCMETH_ADV_TDC_NS;
        }
        stream_codec_decoder_lc3plus_init(LC3PLUS_DEC_PORT_0, rx_handle->source, &lc3plus_dec_config);
        DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]lc3plus info, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 12,
                    audio_transmitter_open_param->scenario_type,
                    audio_transmitter_open_param->scenario_sub_id,
                    rx_handle,
                    lc3plus_dec_config.sample_bits,
                    lc3plus_dec_config.sample_rate,
                    lc3plus_dec_config.bit_rate,
                    lc3plus_dec_config.channel_mode,
                    lc3plus_dec_config.in_channel_num,
                    lc3plus_dec_config.out_channel_num,
                    lc3plus_dec_config.frame_interval,
                    lc3plus_dec_config.frame_samples,
                    lc3plus_dec_config.frame_size);
    }

    /* sw mxier init */
    sw_mixer_callback_config_t       callback_config;
    sw_mixer_input_channel_config_t  in_ch_config;
    sw_mixer_output_channel_config_t out_ch_config;
    stream_function_sw_mixer_init(SW_MIXER_PORT_0);
    callback_config.preprocess_callback = wireless_mic_rx_ul_mixer_precallback;
    callback_config.postprocess_callback = NULL;
    in_ch_config.total_channel_number = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    in_ch_config.resolution = stream_resolution;
    in_ch_config.input_mode = SW_MIXER_CHANNEL_MODE_OVERWRITE;
    in_ch_config.buffer_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    out_ch_config.total_channel_number = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    out_ch_config.resolution = stream_resolution;
    rx_handle->mixer_member= stream_function_sw_mixer_member_create((void *)rx_handle->source, SW_MIXER_MEMBER_MODE_NO_BYPASS, &callback_config, &in_ch_config, &out_ch_config);
    stream_function_sw_mixer_member_register(SW_MIXER_PORT_0, rx_handle->mixer_member, false);
    stream_function_sw_mixer_member_set_output_fixed_32bit(rx_handle->mixer_member, true);
    wireless_mic_rx_ul_mixer_all_stream_connections_update(rx_handle, true);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw mixer 0x%x info, %d, %d, %d, %d, %d, %d\r\n", 10,
                audio_transmitter_open_param->scenario_type,
                audio_transmitter_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->mixer_member,
                in_ch_config.total_channel_number,
                in_ch_config.resolution,
                in_ch_config.input_mode,
                in_ch_config.buffer_size,
                out_ch_config.total_channel_number,
                out_ch_config.resolution);

    /* sw drc init */
    sw_compander_config_t drc_config;
    drc_config.mode = SW_COMPANDER_AUDIO_MODE;
    drc_config.channel_num = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    drc_config.sample_rate = rx_handle->source_info.bt_in.sample_rate;
    drc_config.frame_base = 8;
    drc_config.recovery_gain = 4; /* +24dB */
    drc_config.vol_default_gain = 0x7ECA9CD2; /* +24dB */
    drc_config.default_nvkey_mem = NULL;
    drc_config.default_nvkey_id = NVKEY_DSP_PARA_MIX_AU_CPD;
    rx_handle->drc_port = stream_function_sw_compander_get_port(rx_handle->source);
    stream_function_sw_compander_init(rx_handle->drc_port, &drc_config);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw drc 0x%x info, %d, %d, %d, %d, 0x%x, 0x%x\r\n", 10,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->drc_port,
                drc_config.mode,
                drc_config.channel_num,
                drc_config.sample_rate,
                drc_config.frame_base,
                drc_config.recovery_gain,
                drc_config.default_nvkey_id);

    /* sw clk skew init */
    sw_clk_skew_config_t sw_clk_skew_config;
    rx_handle->clk_skew_port = stream_function_sw_clk_skew_get_port(rx_handle->source);
    sw_clk_skew_config.channel = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    if (stream_resolution == RESOLUTION_16BIT)
    {
        sw_clk_skew_config.bits = 16;
    }
    else
    {
        sw_clk_skew_config.bits = 32;
    }
    sw_clk_skew_config.order = C_Flp_Ord_5;
    sw_clk_skew_config.skew_io_mode = C_Skew_Oup;
    sw_clk_skew_config.skew_compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME;
    sw_clk_skew_config.skew_work_mode = SW_CLK_SKEW_CONTINUOUS;
    sw_clk_skew_config.max_output_size = 2*rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    sw_clk_skew_config.continuous_frame_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    stream_function_sw_clk_skew_init(rx_handle->clk_skew_port, &sw_clk_skew_config);
    rx_handle->compen_samples = 0;
    rx_handle->clk_skew_count = 0;
    rx_handle->clk_skew_watermark_samples = (WIRELESS_MIC_RX_RESERVED_PROCESS_TIME + 1)*rx_handle->source_info.bt_in.sample_rate/1000; /* process time 2ms + clock skew 1ms */
    rx_handle->clk_skew_compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME;
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw clk skew 0x%x info, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 15,
                audio_transmitter_open_param->scenario_type,
                audio_transmitter_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->clk_skew_port,
                sw_clk_skew_config.channel,
                sw_clk_skew_config.bits,
                sw_clk_skew_config.order,
                sw_clk_skew_config.skew_io_mode,
                sw_clk_skew_config.skew_compensation_mode,
                sw_clk_skew_config.skew_work_mode,
                sw_clk_skew_config.max_output_size,
                sw_clk_skew_config.continuous_frame_size,
                rx_handle->compen_samples,
                rx_handle->clk_skew_count,
                rx_handle->clk_skew_watermark_samples);

    /* sw buffer init */
    sw_buffer_config_t buffer_config;
    buffer_config.mode = SW_BUFFER_MODE_FIXED_OUTPUT_LENGTH;
    buffer_config.total_channels = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    buffer_config.watermark_max_size = 4*rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    buffer_config.watermark_min_size = 0;
    buffer_config.output_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    rx_handle->buffer_port = stream_function_sw_buffer_get_port(rx_handle->source);
    stream_function_sw_buffer_init(rx_handle->buffer_port, &buffer_config);
    /* prefill 1ms data */
    stream_function_sw_buffer_config_channel_prefill_size(rx_handle->buffer_port, 0, (WIRELESS_MIC_RX_RESERVED_PROCESS_TIME - 1)*rx_handle->source_info.bt_in.sample_rate/1000*sample_size, true);
    rx_handle->buffer_default_output_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw buffer 0x%x info, %d, %d, %d, %d, %d, %d\r\n", 11,
                audio_transmitter_open_param->scenario_type,
                audio_transmitter_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->buffer_port,
                buffer_config.mode,
                buffer_config.total_channels,
                buffer_config.watermark_max_size,
                buffer_config.watermark_min_size,
                buffer_config.output_size,
                rx_handle->buffer_default_output_size);

    /* sw gain init */
    int32_t default_gain;
    sw_gain_config_t default_config;
    default_config.resolution = stream_resolution;
    default_config.target_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    default_config.up_step = 25;
    default_config.up_samples_per_step = rx_handle->sink_info.usb_out.sample_rate/1000;
    default_config.down_step = -25;
    default_config.down_samples_per_step = rx_handle->sink_info.usb_out.sample_rate/1000;
    rx_handle->gain_port = stream_function_sw_gain_get_port(rx_handle->source);
    stream_function_sw_gain_init(rx_handle->gain_port, WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER, &default_config);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 1, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 2, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 3, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 4, default_gain);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw gain 0x%x info, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 12,
                audio_transmitter_open_param->scenario_type,
                audio_transmitter_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->gain_port,
                default_config.resolution,
                default_config.target_gain,
                default_config.up_step,
                default_config.up_samples_per_step,
                default_config.down_step,
                default_config.down_samples_per_step,
                bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L,
                bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R);

    /* sw src init */
    src_fixed_ratio_config_t sw_src_config;
    rx_handle->src_in_frame_samples     = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000;
    rx_handle->src_in_frame_size        = rx_handle->src_in_frame_samples * sample_size;
    rx_handle->src_out_frame_samples    = rx_handle->sink_info.usb_out.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000;
    rx_handle->src_out_frame_size       = rx_handle->src_out_frame_samples * sample_size;
    sw_src_config.channel_number        = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    sw_src_config.in_sampling_rate      = rx_handle->source_info.bt_in.sample_rate;
    sw_src_config.out_sampling_rate     = rx_handle->sink_info.usb_out.sample_rate;
    sw_src_config.resolution            = stream_resolution;
    sw_src_config.multi_cvt_mode        = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE;
    sw_src_config.cvt_num               = 1;
    sw_src_config.with_codec            = false;
    rx_handle->src_port                 = stream_function_src_fixed_ratio_get_port(rx_handle->source);
    stream_function_src_fixed_ratio_init(rx_handle->src_port, &sw_src_config);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw src 0x%x info, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 13,
                audio_transmitter_open_param->scenario_type,
                audio_transmitter_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->src_port,
                sw_src_config.multi_cvt_mode,
                sw_src_config.channel_number,
                sw_src_config.resolution,
                sw_src_config.in_sampling_rate,
                sw_src_config.out_sampling_rate,
                rx_handle->src_in_frame_samples,
                rx_handle->src_in_frame_size,
                rx_handle->src_out_frame_samples,
                rx_handle->src_out_frame_size);
}

static void wireless_mic_rx_ul_usb_out_deinit(wireless_mic_rx_ul_handle_t *rx_handle)
{
    /* bt in info deinit */
    wireless_mic_rx_ul_bt_in_deinit(rx_handle);

    /* codec deinit */
    stream_codec_decoder_lc3plus_deinit(LC3PLUS_DEC_PORT_0, rx_handle->source);

    /* sw mixer deinit */
    stream_function_sw_mixer_member_unregister(SW_MIXER_PORT_0, rx_handle->mixer_member);
    stream_function_sw_mixer_member_delete(rx_handle->mixer_member);
    stream_function_sw_mixer_deinit(SW_MIXER_PORT_0);

    /* sw drc deinit */
    stream_function_sw_compander_deinit(rx_handle->drc_port);

    /* sw clk skew deinit */
    stream_function_sw_clk_skew_deinit(rx_handle->clk_skew_port);

    /* sw buffer deinit */
    stream_function_sw_buffer_deinit(rx_handle->buffer_port);

    /* sw gain deinit */
    stream_function_sw_gain_deinit(rx_handle->gain_port);

    /* sw src deinit */
    stream_function_src_fixed_ratio_deinit(rx_handle->src_port);
}

ATTR_TEXT_IN_IRAM static int32_t wireless_mic_rx_ul_usb_clock_skew_check(wireless_mic_rx_ul_handle_t *rx_handle)
{
    int32_t compensatory_samples = 0;
    uint32_t remain_samples_in_share_buffer;
    uint32_t remain_samples_in_sw_buffer;
    uint32_t frame_samples;
    uint32_t saved_mask;
    int32_t frac_rpt;
    uint32_t sample_size;

    sample_size = (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE) ? sizeof(int16_t) : sizeof(int32_t);

    if (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED)
    {
        /* get remain samples */
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        frame_samples = rx_handle->source_info.bt_in.sample_rate/1000;
        remain_samples_in_share_buffer = rx_handle->sink_info.usb_out.mcu_frame_count_latch*frame_samples;
        remain_samples_in_sw_buffer = stream_function_sw_buffer_get_channel_used_size(rx_handle->buffer_port, 1) / sample_size;
        hal_nvic_restore_interrupt_mask(saved_mask);

        if ((int32_t)(remain_samples_in_share_buffer+remain_samples_in_sw_buffer) > rx_handle->clk_skew_watermark_samples)
        {
            if (rx_handle->clk_skew_count > -UL_CLOCK_SKEW_CHECK_COUNT)
            {
                rx_handle->clk_skew_count -= 1;
            }
        }
        else if ((int32_t)(remain_samples_in_share_buffer+remain_samples_in_sw_buffer) < rx_handle->clk_skew_watermark_samples)
        {
            if (rx_handle->clk_skew_count < UL_CLOCK_SKEW_CHECK_COUNT)
            {
                rx_handle->clk_skew_count += 1;
            }
        }
        else
        {
            // if ((rx_handle->clk_skew_count < UL_CLOCK_SKEW_CHECK_COUNT) && (rx_handle->clk_skew_count > 0))
            // {
            //     rx_handle->clk_skew_count -= 1;
            // }
            // else if ((rx_handle->clk_skew_count > UL_CLOCK_SKEW_CHECK_COUNT) && (rx_handle->clk_skew_count < 0))
            // {
            //     rx_handle->clk_skew_count += 1;
            // }
            rx_handle->clk_skew_count = 0;
        }

        if (rx_handle->clk_skew_count == UL_CLOCK_SKEW_CHECK_COUNT)
        {
            compensatory_samples = 1;
        }
        else if (rx_handle->clk_skew_count == -UL_CLOCK_SKEW_CHECK_COUNT)
        {
            compensatory_samples = -1;
            stream_function_sw_clk_skew_get_frac_rpt(rx_handle->clk_skew_port, 1, &frac_rpt);
            if (frac_rpt == -(rx_handle->clk_skew_compensation_mode - 1))
            {
                /* in here clock skew will cut 1 sample data, so we need to make sure the buffer output size is N * 1ms */
                rx_handle->buffer_output_size = (rx_handle->buffer_default_output_size/sample_size + remain_samples_in_sw_buffer - 1)/frame_samples*frame_samples*sample_size;
            }
        }
        else
        {
            compensatory_samples = 0;
        }
    }

    return compensatory_samples;
}

/******************************************************************************/
/*          ULL audio 2.0 dongle UL line-out path Private Functions            */
/******************************************************************************/
static void wireless_mic_rx_ul_line_out_init(wireless_mic_rx_ul_handle_t *rx_handle, au_afe_open_param_p afe_open_param, bt_common_open_param_p bt_common_open_param)
{
    uint32_t i;
    uint32_t sample_size = 0;
    n9_dsp_share_info_ptr p_share_info;
    stream_resolution_t stream_resolution = RESOLUTION_16BIT;
    uint32_t saved_mask;

    /* bt in info init */
    wireless_mic_rx_ul_bt_in_init(rx_handle, (audio_transmitter_open_param_p)afe_open_param, bt_common_open_param);
    if (rx_handle->source_info.bt_in.frame_interval == 5000)
    {
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        rx_handle->source_info.bt_in.play_en_delay                    = UL_PLAYEN_DELAY_FRAME_5000US_LINE_OUT;
        rx_handle->source_info.bt_in.play_en_first_packet_safe_delay  = UL_FIRST_PACKET_SAFE_DELAY_FRAME_5000US_LINE_OUT;
        rx_handle->source_info.bt_in.bt_retry_window                  = wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk+wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase/625;
        rx_handle->source_info.bt_in.bt_interval                      = UL_BT_INTERVAL_FRAME_5000US;
        rx_handle->source_info.bt_in.bt_channel_anchor_diff           = UL_BT_INTERVAL_FRAME_5000US/2;
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
    else if (rx_handle->source_info.bt_in.frame_interval == 10000)
    {
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        rx_handle->source_info.bt_in.play_en_delay                    = UL_PLAYEN_DELAY_FRAME_10000US_LINE_OUT;
        rx_handle->source_info.bt_in.play_en_first_packet_safe_delay  = UL_FIRST_PACKET_SAFE_DELAY_FRAME_10000US_LINE_OUT;
        rx_handle->source_info.bt_in.bt_retry_window                  = wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk+wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase/625;
        rx_handle->source_info.bt_in.bt_interval                      = UL_BT_INTERVAL_FRAME_10000US;
        rx_handle->source_info.bt_in.bt_channel_anchor_diff           = UL_BT_INTERVAL_FRAME_10000US/2;
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
    else
    {
        DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]frame_interval is not supported, %u\r\n", 1, rx_handle->source_info.bt_in.frame_interval);
        AUDIO_ASSERT(0);
    }
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]bt in info, %u, %u, %u, %u, %u, %u, %u, %u, %u, %d, %u, %u, %u\r\n", 16,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->source_info.bt_in.channel_num,
                rx_handle->source_info.bt_in.channel_mode,
                rx_handle->source_info.bt_in.sample_rate,
                rx_handle->source_info.bt_in.sample_format,
                rx_handle->source_info.bt_in.frame_size,
                rx_handle->source_info.bt_in.frame_samples,
                rx_handle->source_info.bt_in.frame_interval,
                rx_handle->source_info.bt_in.bit_rate,
                rx_handle->source_info.bt_in.play_en_delay,
                rx_handle->source_info.bt_in.play_en_first_packet_safe_delay,
                rx_handle->source_info.bt_in.bt_retry_window,
                rx_handle->source_info.bt_in.bt_interval,
                rx_handle->source_info.bt_in.bt_channel_anchor_diff);
    for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
    {
        if (rx_handle->source_info.bt_in.bt_info[i] != NULL)
        {
            p_share_info = (n9_dsp_share_info_ptr)((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.share_info);
            DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]bt in channel %u info, %u, %u, %u, %u, %u, 0x%x, 0x%x\r\n", 11,
                        bt_common_open_param->scenario_type,
                        bt_common_open_param->scenario_sub_id,
                        rx_handle,
                        i+1,
                        (rx_handle->source_info.bt_in.bt_info[i])->seq_num,
                        (rx_handle->source_info.bt_in.bt_info[i])->user_count,
                        (rx_handle->source_info.bt_in.bt_info[i])->blk_index,
                        (rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous,
                        (rx_handle->source_info.bt_in.bt_info[i])->crc32_init,
                        p_share_info,
                        hal_memview_infrasys_to_dsp0(p_share_info->start_addr));
        }
    }

    /* line out info init */
    rx_handle->sink_info.line_out.channel_num       = 2;
    rx_handle->sink_info.line_out.sample_rate       = afe_open_param->stream_out_sampling_rate;
    rx_handle->sink_info.line_out.sample_format     = afe_open_param->format;
    rx_handle->sink_info.line_out.frame_samples     = afe_open_param->frame_size;
    if (rx_handle->sink_info.line_out.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE)
    {
        sample_size = sizeof(uint16_t);
    }
    else
    {
        sample_size = sizeof(uint32_t);
    }
    rx_handle->sink_info.line_out.frame_size        = rx_handle->sink_info.line_out.frame_samples*sample_size;
    rx_handle->sink_info.line_out.frame_interval    = rx_handle->source_info.bt_in.frame_interval;
    rx_handle->sink_info.line_out.frame_max_num     = (rx_handle->source_info.bt_in.frame_interval) / 1000;
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]line out info, %u, %u, %u, %u, %u, %u, %u\r\n", 10,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->sink_info.line_out.channel_num,
                rx_handle->sink_info.line_out.sample_rate,
                rx_handle->sink_info.line_out.sample_format,
                rx_handle->sink_info.line_out.frame_size,
                rx_handle->sink_info.line_out.frame_samples,
                rx_handle->sink_info.line_out.frame_interval,
                rx_handle->sink_info.line_out.frame_max_num);

    /* codec init */
    if (rx_handle->source_info.bt_in.codec_type == AUDIO_DSP_CODEC_TYPE_LC3PLUS)
    {
        lc3plus_dec_port_config_t lc3plus_dec_config;
        if (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE)
        {
            lc3plus_dec_config.sample_bits  = 16;
            sample_size = sizeof(uint16_t);
            stream_resolution = RESOLUTION_16BIT;
        }
        else if (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE)
        {
            lc3plus_dec_config.sample_bits  = 24;
            sample_size = sizeof(uint32_t);
            stream_resolution = RESOLUTION_32BIT;
        }
        else
        {
            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]sample_format is not supported, %u\r\n", 1, rx_handle->source_info.bt_in.sample_format);
            AUDIO_ASSERT(0);
        }
        if (rx_handle->source_info.bt_in.channel_mode == 0x2)
        {
            /* each bt link is stereo data */
            lc3plus_dec_config.sample_rate      = rx_handle->source_info.bt_in.sample_rate;
            lc3plus_dec_config.bit_rate         = rx_handle->source_info.bt_in.bit_rate;
            lc3plus_dec_config.channel_mode     = LC3PLUS_DEC_CHANNEL_MODE_MULTI_STEREO;
            lc3plus_dec_config.in_channel_num   = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.out_channel_num  = rx_handle->source_info.bt_in.channel_num*2;
            lc3plus_dec_config.frame_interval   = rx_handle->source_info.bt_in.frame_interval;
            lc3plus_dec_config.frame_size       = rx_handle->source_info.bt_in.frame_size;
            lc3plus_dec_config.frame_samples    = rx_handle->source_info.bt_in.frame_samples;
            lc3plus_dec_config.plc_enable       = 1;
            lc3plus_dec_config.plc_method       = LC3PLUS_PLCMETH_ADV_TDC_NS;
        }
        else
        {
            /* each bt link is mono */
            lc3plus_dec_config.sample_rate      = rx_handle->source_info.bt_in.sample_rate;
            lc3plus_dec_config.bit_rate         = rx_handle->source_info.bt_in.bit_rate;
            lc3plus_dec_config.channel_mode     = LC3PLUS_DEC_CHANNEL_MODE_MULTI_MONO;
            lc3plus_dec_config.in_channel_num   = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.out_channel_num  = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.frame_interval   = rx_handle->source_info.bt_in.frame_interval;
            lc3plus_dec_config.frame_size       = rx_handle->source_info.bt_in.frame_size;
            lc3plus_dec_config.frame_samples    = rx_handle->source_info.bt_in.frame_samples;
            lc3plus_dec_config.plc_enable       = 1;
            lc3plus_dec_config.plc_method       = LC3PLUS_PLCMETH_ADV_TDC_NS;
        }
        stream_codec_decoder_lc3plus_init(LC3PLUS_DEC_PORT_0, rx_handle->source, &lc3plus_dec_config);
        DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]lc3plus info, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 12,
                    bt_common_open_param->scenario_type,
                    bt_common_open_param->scenario_sub_id,
                    rx_handle,
                    lc3plus_dec_config.sample_bits,
                    lc3plus_dec_config.sample_rate,
                    lc3plus_dec_config.bit_rate,
                    lc3plus_dec_config.channel_mode,
                    lc3plus_dec_config.in_channel_num,
                    lc3plus_dec_config.out_channel_num,
                    lc3plus_dec_config.frame_interval,
                    lc3plus_dec_config.frame_samples,
                    lc3plus_dec_config.frame_size);
    }

    /* dummy state machine for source_readbuf() */
    rx_handle->buffer_default_output_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;

    /* sw mxier init */
    sw_mixer_callback_config_t       callback_config;
    sw_mixer_input_channel_config_t  in_ch_config;
    sw_mixer_output_channel_config_t out_ch_config;
    stream_function_sw_mixer_init(SW_MIXER_PORT_0);
    callback_config.preprocess_callback = wireless_mic_rx_ul_mixer_precallback;
    callback_config.postprocess_callback = NULL;
    in_ch_config.total_channel_number = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    in_ch_config.resolution = stream_resolution;
    in_ch_config.input_mode = SW_MIXER_CHANNEL_MODE_OVERWRITE;
    in_ch_config.buffer_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    out_ch_config.total_channel_number = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    out_ch_config.resolution = stream_resolution;
    rx_handle->mixer_member= stream_function_sw_mixer_member_create((void *)rx_handle->source, SW_MIXER_MEMBER_MODE_NO_BYPASS, &callback_config, &in_ch_config, &out_ch_config);
    stream_function_sw_mixer_member_register(SW_MIXER_PORT_0, rx_handle->mixer_member, false);
    stream_function_sw_mixer_member_set_output_fixed_32bit(rx_handle->mixer_member, true);
    wireless_mic_rx_ul_mixer_all_stream_connections_update(rx_handle, true);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw mixer 0x%x info, %d, %d, %d, %d, %d, %d\r\n", 10,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->mixer_member,
                in_ch_config.total_channel_number,
                in_ch_config.resolution,
                in_ch_config.input_mode,
                in_ch_config.buffer_size,
                out_ch_config.total_channel_number,
                out_ch_config.resolution);

    /* sw drc init */
    sw_compander_config_t drc_config;
    drc_config.mode = SW_COMPANDER_AUDIO_MODE;
    drc_config.channel_num = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    drc_config.sample_rate = rx_handle->source_info.bt_in.sample_rate;
    drc_config.frame_base = 8;
    drc_config.recovery_gain = 4; /* +24dB */
    drc_config.vol_default_gain = 0x7ECA9CD2; /* +24dB */
    drc_config.default_nvkey_mem = NULL;
    drc_config.default_nvkey_id = NVKEY_DSP_PARA_MIX_AU_CPD;
    rx_handle->drc_port = stream_function_sw_compander_get_port(rx_handle->source);
    stream_function_sw_compander_init(rx_handle->drc_port, &drc_config);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw drc 0x%x info, %d, %d, %d, %d, 0x%x, 0x%x, 0x%x\r\n", 11,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->drc_port,
                drc_config.mode,
                drc_config.channel_num,
                drc_config.sample_rate,
                drc_config.frame_base,
                drc_config.recovery_gain,
                drc_config.vol_default_gain,
                drc_config.default_nvkey_id);

    /* sw gain init */
    int32_t default_gain;
    sw_gain_config_t default_config;
    default_config.resolution = stream_resolution;
    default_config.target_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    default_config.up_step = 25;
    default_config.up_samples_per_step = rx_handle->sink_info.line_out.sample_rate/1000;
    default_config.down_step = -25;
    default_config.down_samples_per_step = rx_handle->sink_info.line_out.sample_rate/1000;
    rx_handle->gain_port = stream_function_sw_gain_get_port(rx_handle->source);
    stream_function_sw_gain_init(rx_handle->gain_port, WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER, &default_config);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 1, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 2, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 3, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 4, default_gain);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw gain 0x%x info, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 12,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->gain_port,
                default_config.resolution,
                default_config.target_gain,
                default_config.up_step,
                default_config.up_samples_per_step,
                default_config.down_step,
                default_config.down_samples_per_step,
                bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L,
                bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R);
}

static void wireless_mic_rx_ul_line_out_deinit(wireless_mic_rx_ul_handle_t *rx_handle)
{
    /* bt in info deinit */
    wireless_mic_rx_ul_bt_in_deinit(rx_handle);

    /* codec deinit */
    stream_codec_decoder_lc3plus_deinit(LC3PLUS_DEC_PORT_0, rx_handle->source);

    /* sw mixer deinit */
    stream_function_sw_mixer_member_unregister(SW_MIXER_PORT_0, rx_handle->mixer_member);
    stream_function_sw_mixer_member_delete(rx_handle->mixer_member);
    stream_function_sw_mixer_deinit(SW_MIXER_PORT_0);

    /* sw drc deinit */
    stream_function_sw_compander_deinit(rx_handle->drc_port);

    /* sw gain deinit */
    stream_function_sw_gain_deinit(rx_handle->gain_port);
}

static void wireless_mic_rx_ul_line_out_start(wireless_mic_rx_ul_handle_t *rx_handle)
{
    uint32_t gpt_count;
    uint32_t sample_size;
    hal_audio_memory_parameter_t *mem_handle = &rx_handle->sink->param.audio.mem_handle;
    hal_audio_agent_t agent;

    /* get agent */
    agent = hal_memory_convert_agent(mem_handle->memory_select);
    /* disable AFE irq here */
    hal_memory_set_irq_enable(agent, HAL_AUDIO_CONTROL_OFF);

    /* set agent regsiters' address */
    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            rx_handle->sink_info.line_out.afe_cur_addr  = AFE_DL1_CUR;
            rx_handle->sink_info.line_out.afe_base_addr = AFE_DL1_BASE;
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL2:
            rx_handle->sink_info.line_out.afe_cur_addr  = AFE_DL2_CUR;
            rx_handle->sink_info.line_out.afe_base_addr = AFE_DL2_BASE;
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL3:
            rx_handle->sink_info.line_out.afe_cur_addr  = AFE_DL3_CUR;
            rx_handle->sink_info.line_out.afe_base_addr = AFE_DL3_BASE;
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL12:
            rx_handle->sink_info.line_out.afe_cur_addr  = AFE_DL12_CUR;
            rx_handle->sink_info.line_out.afe_base_addr = AFE_DL12_BASE;
            break;

        default:
            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR][handle 0x%x][scenario %d-%d]line-out stream start, unknow agent = 0x%x", 4,
                rx_handle,
                rx_handle->source->param.bt_common.scenario_type,
                rx_handle->source->param.bt_common.scenario_sub_id,
                agent);
            AUDIO_ASSERT(0);
    }

    /* set 2.771ms prefill size for process time 2ms + 0.771ms for line-out/i2s-slv-out aligned */
    if (rx_handle->sink_info.line_out.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE)
    {
        sample_size = sizeof(uint16_t);
    }
    else
    {
        sample_size = sizeof(uint32_t);
    }
    rx_handle->sink->streamBuffer.BufferInfo.WriteOffset = WIRELESS_MIC_RX_RESERVED_PROCESS_TIME*sample_size*rx_handle->sink_info.line_out.channel_num*(rx_handle->sink_info.line_out.sample_rate/1000)+sample_size*rx_handle->sink_info.line_out.channel_num*37;
    rx_handle->sink->streamBuffer.BufferInfo.ReadOffset  = 0;

    /* start 1ms timer for trigger stream */
    wireless_mic_rx_software_timer_start(rx_handle);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x][scenario %d-%d]line-out stream start, gpt count = 0x%x, start_addr = 0x%x, write_offset = 0x%x, read_offset = 0x%x, length = 0x%x, cur_addr = 0x%x, base_addr = 0x%x, cur_ro = 0x%x, cur_base = 0x%x", 12,
                rx_handle,
                rx_handle->source->param.bt_common.scenario_type,
                rx_handle->source->param.bt_common.scenario_sub_id,
                gpt_count,
                rx_handle->sink->streamBuffer.BufferInfo.startaddr[0],
                rx_handle->sink->streamBuffer.BufferInfo.WriteOffset,
                rx_handle->sink->streamBuffer.BufferInfo.ReadOffset,
                rx_handle->sink->streamBuffer.BufferInfo.length,
                rx_handle->sink_info.line_out.afe_cur_addr,
                rx_handle->sink_info.line_out.afe_base_addr,
                AFE_GET_REG(rx_handle->sink_info.line_out.afe_cur_addr),
                AFE_GET_REG(rx_handle->sink_info.line_out.afe_base_addr));
}

static void wireless_mic_rx_ul_line_out_stop(wireless_mic_rx_ul_handle_t *rx_handle)
{
    uint32_t gpt_count;
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (rx_handle->stream_status == WIRELESS_MIC_RX_STREAM_START)
    {
        /* stop timer if the stream is not running */
        wireless_mic_rx_software_timer_stop(rx_handle);
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x][scenario %d-%d]line-out stream start, gpt count = 0x%x", 4,
                rx_handle,
                rx_handle->source->param.bt_common.scenario_type,
                rx_handle->source->param.bt_common.scenario_sub_id,
                gpt_count);
}

ATTR_TEXT_IN_IRAM static int32_t wireless_mic_rx_ul_line_out_clock_skew_check(wireless_mic_rx_ul_handle_t *rx_handle)
{
    int32_t compensatory_samples = 0;
    UNUSED(rx_handle);

    return compensatory_samples;
}

/******************************************************************************/
/*         ULL audio 2.0 dongle UL i2s-slv-out path Private Functions         */
/******************************************************************************/
static void wireless_mic_rx_ul_i2s_slv_out_init(wireless_mic_rx_ul_handle_t *rx_handle, au_afe_open_param_p afe_open_param, bt_common_open_param_p bt_common_open_param)
{
    uint32_t i;
    uint32_t sample_size = 0;
    n9_dsp_share_info_ptr p_share_info;
    stream_resolution_t stream_resolution = RESOLUTION_16BIT;
    uint32_t saved_mask;

    /* bt in info init */
    wireless_mic_rx_ul_bt_in_init(rx_handle, (audio_transmitter_open_param_p)afe_open_param, bt_common_open_param);
    if (rx_handle->source_info.bt_in.frame_interval == 5000)
    {
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        rx_handle->source_info.bt_in.play_en_delay                    = UL_PLAYEN_DELAY_FRAME_5000US_I2S_SLV_OUT;
        rx_handle->source_info.bt_in.play_en_first_packet_safe_delay  = UL_FIRST_PACKET_SAFE_DELAY_FRAME_5000US_I2S_SLV_OUT;
        rx_handle->source_info.bt_in.bt_retry_window                  = wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk+wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase/625;
        rx_handle->source_info.bt_in.bt_interval                      = UL_BT_INTERVAL_FRAME_5000US;
        rx_handle->source_info.bt_in.bt_channel_anchor_diff           = UL_BT_INTERVAL_FRAME_5000US/2;
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
    else if (rx_handle->source_info.bt_in.frame_interval == 10000)
    {
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        rx_handle->source_info.bt_in.play_en_delay                    = UL_PLAYEN_DELAY_FRAME_10000US_I2S_SLV_OUT;
        rx_handle->source_info.bt_in.play_en_first_packet_safe_delay  = UL_FIRST_PACKET_SAFE_DELAY_FRAME_10000US_I2S_SLV_OUT;
        rx_handle->source_info.bt_in.bt_retry_window                  = wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk+wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase/625;
        rx_handle->source_info.bt_in.bt_interval                      = UL_BT_INTERVAL_FRAME_10000US;
        rx_handle->source_info.bt_in.bt_channel_anchor_diff           = UL_BT_INTERVAL_FRAME_10000US/2;
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
    else
    {
        DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]frame_interval is not supported, %u\r\n", 1, rx_handle->source_info.bt_in.frame_interval);
        AUDIO_ASSERT(0);
    }
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]bt in info, %u, %u, %u, %u, %u, %u, %u, %u, %u, %d, %u, %u, %u\r\n", 16,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->source_info.bt_in.channel_num,
                rx_handle->source_info.bt_in.channel_mode,
                rx_handle->source_info.bt_in.sample_rate,
                rx_handle->source_info.bt_in.sample_format,
                rx_handle->source_info.bt_in.frame_size,
                rx_handle->source_info.bt_in.frame_samples,
                rx_handle->source_info.bt_in.frame_interval,
                rx_handle->source_info.bt_in.bit_rate,
                rx_handle->source_info.bt_in.play_en_delay,
                rx_handle->source_info.bt_in.play_en_first_packet_safe_delay,
                rx_handle->source_info.bt_in.bt_retry_window,
                rx_handle->source_info.bt_in.bt_interval,
                rx_handle->source_info.bt_in.bt_channel_anchor_diff);
    for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
    {
        if (rx_handle->source_info.bt_in.bt_info[i] != NULL)
        {
            p_share_info = (n9_dsp_share_info_ptr)((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.share_info);
            DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]bt in channel %u info, %u, %u, %u, %u, %u, 0x%x, 0x%x\r\n", 11,
                        bt_common_open_param->scenario_type,
                        bt_common_open_param->scenario_sub_id,
                        rx_handle,
                        i+1,
                        (rx_handle->source_info.bt_in.bt_info[i])->seq_num,
                        (rx_handle->source_info.bt_in.bt_info[i])->user_count,
                        (rx_handle->source_info.bt_in.bt_info[i])->blk_index,
                        (rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous,
                        (rx_handle->source_info.bt_in.bt_info[i])->crc32_init,
                        p_share_info,
                        hal_memview_infrasys_to_dsp0(p_share_info->start_addr));
        }
    }

    /* i2s slv out info init */
    rx_handle->sink_info.i2s_slv_out.channel_num       = 2;
    rx_handle->sink_info.i2s_slv_out.sample_rate       = afe_open_param->stream_out_sampling_rate;
    rx_handle->sink_info.i2s_slv_out.sample_format     = afe_open_param->format;
    rx_handle->sink_info.i2s_slv_out.frame_samples     = afe_open_param->frame_size;
    if (rx_handle->sink_info.i2s_slv_out.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE)
    {
        sample_size = sizeof(uint16_t);
    }
    else
    {
        sample_size = sizeof(uint32_t);
    }
    rx_handle->sink_info.i2s_slv_out.frame_size        = rx_handle->sink_info.i2s_slv_out.frame_samples*sample_size;
    rx_handle->sink_info.i2s_slv_out.frame_interval    = rx_handle->source_info.bt_in.frame_interval;
    rx_handle->sink_info.i2s_slv_out.frame_max_num     = (rx_handle->source_info.bt_in.frame_interval) / 1000;
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]i2s slv out info, %u, %u, %u, %u, %u, %u, %u\r\n", 10,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->sink_info.i2s_slv_out.channel_num,
                rx_handle->sink_info.i2s_slv_out.sample_rate,
                rx_handle->sink_info.i2s_slv_out.sample_format,
                rx_handle->sink_info.i2s_slv_out.frame_size,
                rx_handle->sink_info.i2s_slv_out.frame_samples,
                rx_handle->sink_info.i2s_slv_out.frame_interval,
                rx_handle->sink_info.i2s_slv_out.frame_max_num);

    /* codec init */
    if (rx_handle->source_info.bt_in.codec_type == AUDIO_DSP_CODEC_TYPE_LC3PLUS)
    {
        lc3plus_dec_port_config_t lc3plus_dec_config;
        if (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE)
        {
            lc3plus_dec_config.sample_bits  = 16;
            sample_size = sizeof(uint16_t);
            stream_resolution = RESOLUTION_16BIT;
        }
        else if (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE)
        {
            lc3plus_dec_config.sample_bits  = 24;
            sample_size = sizeof(uint32_t);
            stream_resolution = RESOLUTION_32BIT;
        }
        else
        {
            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]sample_format is not supported, %u\r\n", 1, rx_handle->source_info.bt_in.sample_format);
            AUDIO_ASSERT(0);
        }
        if (rx_handle->source_info.bt_in.channel_mode == 0x2)
        {
            /* each bt link is stereo data */
            lc3plus_dec_config.sample_rate      = rx_handle->source_info.bt_in.sample_rate;
            lc3plus_dec_config.bit_rate         = rx_handle->source_info.bt_in.bit_rate;
            lc3plus_dec_config.channel_mode     = LC3PLUS_DEC_CHANNEL_MODE_MULTI_STEREO;
            lc3plus_dec_config.in_channel_num   = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.out_channel_num  = rx_handle->source_info.bt_in.channel_num*2;
            lc3plus_dec_config.frame_interval   = rx_handle->source_info.bt_in.frame_interval;
            lc3plus_dec_config.frame_size       = rx_handle->source_info.bt_in.frame_size;
            lc3plus_dec_config.frame_samples    = rx_handle->source_info.bt_in.frame_samples;
            lc3plus_dec_config.plc_enable       = 1;
            lc3plus_dec_config.plc_method       = LC3PLUS_PLCMETH_ADV_TDC_NS;
        }
        else
        {
            /* each bt link is mono */
            lc3plus_dec_config.sample_rate      = rx_handle->source_info.bt_in.sample_rate;
            lc3plus_dec_config.bit_rate         = rx_handle->source_info.bt_in.bit_rate;
            lc3plus_dec_config.channel_mode     = LC3PLUS_DEC_CHANNEL_MODE_MULTI_MONO;
            lc3plus_dec_config.in_channel_num   = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.out_channel_num  = rx_handle->source_info.bt_in.channel_num;
            lc3plus_dec_config.frame_interval   = rx_handle->source_info.bt_in.frame_interval;
            lc3plus_dec_config.frame_size       = rx_handle->source_info.bt_in.frame_size;
            lc3plus_dec_config.frame_samples    = rx_handle->source_info.bt_in.frame_samples;
            lc3plus_dec_config.plc_enable       = 1;
            lc3plus_dec_config.plc_method       = LC3PLUS_PLCMETH_ADV_TDC_NS;
        }
        stream_codec_decoder_lc3plus_init(LC3PLUS_DEC_PORT_0, rx_handle->source, &lc3plus_dec_config);
        DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]lc3plus info, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 12,
                    bt_common_open_param->scenario_type,
                    bt_common_open_param->scenario_sub_id,
                    rx_handle,
                    lc3plus_dec_config.sample_bits,
                    lc3plus_dec_config.sample_rate,
                    lc3plus_dec_config.bit_rate,
                    lc3plus_dec_config.channel_mode,
                    lc3plus_dec_config.in_channel_num,
                    lc3plus_dec_config.out_channel_num,
                    lc3plus_dec_config.frame_interval,
                    lc3plus_dec_config.frame_samples,
                    lc3plus_dec_config.frame_size);
    }

    /* dummy state machine for source_readbuf() */
    rx_handle->buffer_default_output_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;

    /* sw mxier init */
    sw_mixer_callback_config_t       callback_config;
    sw_mixer_input_channel_config_t  in_ch_config;
    sw_mixer_output_channel_config_t out_ch_config;
    stream_function_sw_mixer_init(SW_MIXER_PORT_0);
    callback_config.preprocess_callback = wireless_mic_rx_ul_mixer_precallback;
    callback_config.postprocess_callback = NULL;
    in_ch_config.total_channel_number = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    in_ch_config.resolution = stream_resolution;
    in_ch_config.input_mode = SW_MIXER_CHANNEL_MODE_OVERWRITE;
    in_ch_config.buffer_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    out_ch_config.total_channel_number = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    out_ch_config.resolution = stream_resolution;
    rx_handle->mixer_member= stream_function_sw_mixer_member_create((void *)rx_handle->source, SW_MIXER_MEMBER_MODE_NO_BYPASS, &callback_config, &in_ch_config, &out_ch_config);
    stream_function_sw_mixer_member_register(SW_MIXER_PORT_0, rx_handle->mixer_member, false);
    stream_function_sw_mixer_member_set_output_fixed_32bit(rx_handle->mixer_member, true);
    wireless_mic_rx_ul_mixer_all_stream_connections_update(rx_handle, true);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw mixer 0x%x info, %d, %d, %d, %d, %d, %d\r\n", 10,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->mixer_member,
                in_ch_config.total_channel_number,
                in_ch_config.resolution,
                in_ch_config.input_mode,
                in_ch_config.buffer_size,
                out_ch_config.total_channel_number,
                out_ch_config.resolution);

    /* sw drc init */
    sw_compander_config_t drc_config;
    drc_config.mode = SW_COMPANDER_AUDIO_MODE;
    drc_config.channel_num = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    drc_config.sample_rate = rx_handle->source_info.bt_in.sample_rate;
    drc_config.frame_base = 8;
    drc_config.recovery_gain = 4;  /* +24dB */
    drc_config.vol_default_gain = 0x7ECA9CD2; /* +24dB */
    drc_config.default_nvkey_mem = NULL;
    drc_config.default_nvkey_id = NVKEY_DSP_PARA_MIX_AU_CPD;
    rx_handle->drc_port = stream_function_sw_compander_get_port(rx_handle->source);
    stream_function_sw_compander_init(rx_handle->drc_port, &drc_config);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw drc 0x%x info, %d, %d, %d, %d, 0x%x, 0x%x\r\n", 10,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->drc_port,
                drc_config.mode,
                drc_config.channel_num,
                drc_config.sample_rate,
                drc_config.frame_base,
                drc_config.recovery_gain,
                drc_config.default_nvkey_id);

    /* sw clk skew init */
    sw_clk_skew_config_t sw_clk_skew_config;
    rx_handle->clk_skew_port = stream_function_sw_clk_skew_get_port(rx_handle->source);
    sw_clk_skew_config.channel = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
    if (stream_resolution == RESOLUTION_16BIT)
    {
        sw_clk_skew_config.bits = 16;
    }
    else
    {
        sw_clk_skew_config.bits = 32;
    }
    sw_clk_skew_config.order = C_Flp_Ord_5;
    sw_clk_skew_config.skew_io_mode = C_Skew_Oup;
    sw_clk_skew_config.skew_compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME;
    sw_clk_skew_config.skew_work_mode = SW_CLK_SKEW_CONTINUOUS;
    sw_clk_skew_config.max_output_size = 2*rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    sw_clk_skew_config.continuous_frame_size = rx_handle->source_info.bt_in.sample_rate/1000*rx_handle->source_info.bt_in.frame_interval/1000*sample_size;
    stream_function_sw_clk_skew_init(rx_handle->clk_skew_port, &sw_clk_skew_config);
    rx_handle->compen_samples = 0;
    rx_handle->clk_skew_count = 0;
    rx_handle->clk_skew_watermark_samples = (WIRELESS_MIC_RX_RESERVED_PROCESS_TIME + 1)*rx_handle->source_info.bt_in.sample_rate/1000; // 3ms
    rx_handle->clk_skew_compensation_mode = SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME;
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw clk skew 0x%x info, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 15,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->clk_skew_port,
                sw_clk_skew_config.channel,
                sw_clk_skew_config.bits,
                sw_clk_skew_config.order,
                sw_clk_skew_config.skew_io_mode,
                sw_clk_skew_config.skew_compensation_mode,
                sw_clk_skew_config.skew_work_mode,
                sw_clk_skew_config.max_output_size,
                sw_clk_skew_config.continuous_frame_size,
                rx_handle->compen_samples,
                rx_handle->clk_skew_count,
                rx_handle->clk_skew_watermark_samples);

    /* sw gain init */
    int32_t default_gain;
    sw_gain_config_t default_config;
    default_config.resolution = stream_resolution;
    default_config.target_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    default_config.up_step = 25;
    default_config.up_samples_per_step = rx_handle->sink_info.i2s_slv_out.sample_rate/1000;
    default_config.down_step = -25;
    default_config.down_samples_per_step = rx_handle->sink_info.i2s_slv_out.sample_rate/1000;
    rx_handle->gain_port = stream_function_sw_gain_get_port(rx_handle->source);
    stream_function_sw_gain_init(rx_handle->gain_port, WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER, &default_config);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 1, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 2, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 3, default_gain);
    default_gain = bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R;
    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, 4, default_gain);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][scenario %d-%d][handle 0x%x]sw gain 0x%x info, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 12,
                bt_common_open_param->scenario_type,
                bt_common_open_param->scenario_sub_id,
                rx_handle,
                rx_handle->gain_port,
                default_config.resolution,
                default_config.target_gain,
                default_config.up_step,
                default_config.up_samples_per_step,
                default_config.down_step,
                default_config.down_samples_per_step,
                bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_L,
                bt_common_open_param->scenario_param.wireless_mic_rx_param.gain_default_R);
}

static void wireless_mic_rx_ul_i2s_slv_out_deinit(wireless_mic_rx_ul_handle_t *rx_handle)
{
    /* bt in info deinit */
    wireless_mic_rx_ul_bt_in_deinit(rx_handle);

    /* codec deinit */
    stream_codec_decoder_lc3plus_deinit(LC3PLUS_DEC_PORT_0, rx_handle->source);

    /* sw mixer deinit */
    stream_function_sw_mixer_member_unregister(SW_MIXER_PORT_0, rx_handle->mixer_member);
    stream_function_sw_mixer_member_delete(rx_handle->mixer_member);
    stream_function_sw_mixer_deinit(SW_MIXER_PORT_0);

    /* sw drc deinit */
    stream_function_sw_compander_deinit(rx_handle->drc_port);

    /* sw clk skew deinit */
    stream_function_sw_clk_skew_deinit(rx_handle->clk_skew_port);

    /* sw gain deinit */
    stream_function_sw_gain_deinit(rx_handle->gain_port);
}

static void wireless_mic_rx_ul_i2s_slv_out_start(wireless_mic_rx_ul_handle_t *rx_handle)
{
    uint32_t gpt_count;
    // uint32_t sample_size;
    hal_audio_memory_parameter_t *mem_handle = &rx_handle->sink->param.audio.mem_handle;
    hal_audio_agent_t agent;

    /* get agent */
    agent = hal_memory_convert_agent(mem_handle->memory_select);
    /* disable AFE irq here */
    hal_memory_set_irq_enable(agent, HAL_AUDIO_CONTROL_OFF);

    /* set agent regsiters' address */
    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            rx_handle->sink_info.i2s_slv_out.afe_cur_addr  = AFE_DL1_CUR;
            rx_handle->sink_info.i2s_slv_out.afe_base_addr = AFE_DL1_BASE;
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL2:
            rx_handle->sink_info.i2s_slv_out.afe_cur_addr  = AFE_DL2_CUR;
            rx_handle->sink_info.i2s_slv_out.afe_base_addr = AFE_DL2_BASE;
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL3:
            rx_handle->sink_info.i2s_slv_out.afe_cur_addr  = AFE_DL3_CUR;
            rx_handle->sink_info.i2s_slv_out.afe_base_addr = AFE_DL3_BASE;
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL12:
            rx_handle->sink_info.i2s_slv_out.afe_cur_addr  = AFE_DL12_CUR;
            rx_handle->sink_info.i2s_slv_out.afe_base_addr = AFE_DL12_BASE;
            break;

        default:
            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR][handle 0x%x][scenario %d-%d]i2s-slv-out stream start, unknow agent = 0x%x", 4,
                rx_handle,
                rx_handle->source->param.bt_common.scenario_type,
                rx_handle->source->param.bt_common.scenario_sub_id,
                agent);
            AUDIO_ASSERT(0);
    }

    /* Note: set 2ms prefill size in Sink_Audio_BufferInfo_Rst() */
    // if (rx_handle->sink_info.i2s_slv_out.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE)
    // {
    //     sample_size = sizeof(uint16_t);
    // }
    // else
    // {
    //     sample_size = sizeof(uint32_t);
    // }
    // rx_handle->sink->streamBuffer.BufferInfo.WriteOffset = sample_size*rx_handle->sink_info.i2s_slv_out.channel_num*(3*rx_handle->sink_info.i2s_slv_out.sample_rate/1000);
    // rx_handle->sink->streamBuffer.BufferInfo.ReadOffset  = 0;

    /* start 1ms timer for trigger stream */
    wireless_mic_rx_software_timer_start(rx_handle);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x][scenario %d-%d]i2s-slv-out stream start, gpt count = 0x%x, start_addr = 0x%x, write_offset = 0x%x, read_offset = 0x%x, length = 0x%x, cur_addr = 0x%x, base_addr = 0x%x, cur_ro = 0x%x, cur_base = 0x%x", 12,
                rx_handle,
                rx_handle->source->param.bt_common.scenario_type,
                rx_handle->source->param.bt_common.scenario_sub_id,
                gpt_count,
                rx_handle->sink->streamBuffer.BufferInfo.startaddr[0],
                rx_handle->sink->streamBuffer.BufferInfo.WriteOffset,
                rx_handle->sink->streamBuffer.BufferInfo.ReadOffset,
                rx_handle->sink->streamBuffer.BufferInfo.length,
                rx_handle->sink_info.i2s_slv_out.afe_cur_addr,
                rx_handle->sink_info.i2s_slv_out.afe_base_addr,
                AFE_GET_REG(rx_handle->sink_info.i2s_slv_out.afe_cur_addr),
                AFE_GET_REG(rx_handle->sink_info.i2s_slv_out.afe_base_addr));
}

static void wireless_mic_rx_ul_i2s_slv_stop(wireless_mic_rx_ul_handle_t *rx_handle)
{
    uint32_t gpt_count;
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (rx_handle->stream_status == WIRELESS_MIC_RX_STREAM_START)
    {
        /* stop timer if the stream is not running */
        wireless_mic_rx_software_timer_stop(rx_handle);
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x][scenario %d-%d]i2s-slv-out stream start, gpt count = 0x%x", 4,
                rx_handle,
                rx_handle->source->param.bt_common.scenario_type,
                rx_handle->source->param.bt_common.scenario_sub_id,
                gpt_count);
}

ATTR_TEXT_IN_IRAM static int32_t wireless_mic_rx_ul_i2s_slv_clock_skew_check(wireless_mic_rx_ul_handle_t *rx_handle)
{
#if 0
    int32_t compensatory_samples = 0;
    int32_t remain_samples;
    int32_t sample_size = 0;
    uint32_t write_offset;
    uint32_t read_offset;

    if (rx_handle->sink_info.i2s_slv_out.sample_format >= HAL_AUDIO_PCM_FORMAT_S24_LE)
    {
        sample_size = sizeof(int32_t);
    }
    else
    {
        sample_size = sizeof(int16_t);
    }

    if (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED)
    {
        /* get remain samples in AFE SRAM */
        write_offset = rx_handle->sink->streamBuffer.BufferInfo.WriteOffset;
        read_offset  = rx_handle->sink->streamBuffer.BufferInfo.ReadOffset;
        if (write_offset >= read_offset)
        {
            remain_samples = (write_offset - read_offset) / sample_size / rx_handle->sink_info.i2s_slv_out.channel_num;
        }
        else
        {
            remain_samples = (rx_handle->sink->streamBuffer.BufferInfo.length + write_offset - read_offset) / sample_size / rx_handle->sink_info.i2s_slv_out.channel_num;
        }

        remain_samples = remain_samples + UL_DL_AGENT_FIFO_SAMPLES;
        if (remain_samples > rx_handle->clk_skew_watermark_samples)
        {
            if (rx_handle->clk_skew_count > -UL_CLOCK_SKEW_CHECK_COUNT)
            {
                rx_handle->clk_skew_count -= 1;
            }
        }
        else if (remain_samples < rx_handle->clk_skew_watermark_samples)
        {
            if (rx_handle->clk_skew_count < UL_CLOCK_SKEW_CHECK_COUNT)
            {
                rx_handle->clk_skew_count += 1;
            }
        }
        else
        {
            // if ((rx_handle->clk_skew_count < UL_CLOCK_SKEW_CHECK_COUNT) && (rx_handle->clk_skew_count > 0))
            // {
            //     rx_handle->clk_skew_count -= 1;
            // }
            // else if ((rx_handle->clk_skew_count > UL_CLOCK_SKEW_CHECK_COUNT) && (rx_handle->clk_skew_count < 0))
            // {
            //     rx_handle->clk_skew_count += 1;
            // }
            rx_handle->clk_skew_count = 0;
        }

        if (rx_handle->clk_skew_count == UL_CLOCK_SKEW_CHECK_COUNT)
        {
            compensatory_samples = 1;
        }
        else if (rx_handle->clk_skew_count == -UL_CLOCK_SKEW_CHECK_COUNT)
        {
            compensatory_samples = -1;
        }
        else
        {
            compensatory_samples = 0;
        }
    }

    return compensatory_samples;

#else

    /* use HWSRC to tracking */
    int32_t compensatory_samples = 0;
    UNUSED(rx_handle);

    return compensatory_samples;
#endif /*  */
}

/******************************************************************************/
/*             ULL audio 2.0 dongle UL stream Private Functions               */
/******************************************************************************/
ATTR_TEXT_IN_IRAM static wireless_mic_rx_first_packet_status_t wireless_mic_rx_ul_first_packet_check(wireless_mic_rx_ul_handle_t *rx_handle, uint32_t bt_clk, uint16_t bt_phase)
{
    uint32_t i;
    n9_dsp_share_info_ptr p_share_info;
    uint16_t blk_index = 0;
    uint16_t blk_num;
    uint16_t read_offset;
    uint32_t total_buffer_size;
    WIRELESS_MIC_RX_HEADER *p_ull_audio_header;
    wireless_mic_rx_first_packet_status_t first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY;
    uint32_t anchor = 0;
    uint32_t anchor_timeout = 0;
    uint32_t anchor_playen = 0;
    uint32_t anchor_playen_safe =0;
    UNUSED(bt_phase);

    for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
    {
        if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
        {
            continue;
        }
        else
        {
            /* set read blk to the write blk - 1 and get this read blk' header */
            p_share_info = (n9_dsp_share_info_ptr)((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.share_info);
            if (p_share_info->write_offset == 0)
            {
                continue;
            }
            total_buffer_size = p_share_info->sub_info.block_info.block_size * p_share_info->sub_info.block_info.block_num;
            StreamDSP_HWSemaphoreTake();
            read_offset = (p_share_info->write_offset+total_buffer_size-p_share_info->sub_info.block_info.block_size)%total_buffer_size;
            p_ull_audio_header = (WIRELESS_MIC_RX_HEADER *)(hal_memview_infrasys_to_dsp0(p_share_info->start_addr) + read_offset);
            StreamDSP_HWSemaphoreGive();
            blk_index = read_offset/p_share_info->sub_info.block_info.block_size;
            blk_num = p_share_info->sub_info.block_info.block_num;

            /* check packet's timestamp */
            if (p_ull_audio_header->valid_packet == 0)
            {
                continue;
            }
            else
            {
                anchor_timeout = p_ull_audio_header->TimeStamp;
                anchor = (anchor_timeout+0x10000000-rx_handle->source_info.bt_in.bt_retry_window) & 0x0fffffff;
                anchor_playen = (anchor_timeout+rx_handle->source_info.bt_in.play_en_delay) & 0x0fffffff;
                anchor_playen_safe = (anchor_timeout+rx_handle->source_info.bt_in.play_en_first_packet_safe_delay) & 0x0fffffff;

                /* check if this packet is suitable for play */
                if (anchor < anchor_playen_safe)
                {
                    if ((bt_clk >= anchor) && (bt_clk <= anchor_playen_safe))
                    {
                        /* --------- ........ --------- anchor ---------- bt_clk ---------- anchor_playen_safe --------- ........ -------- */
                        first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_READY;
                        goto _find_first_packet;
                    }
                }
                else
                {
                    if ((bt_clk < anchor) && (bt_clk <= anchor_playen_safe))
                    {
                        /* --------- ........ ---------- bt_clk ---------- anchor_playen_safe --------- ........ --------- anchor -------- */
                        first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_READY;
                        goto _find_first_packet;
                    }
                    else if ((bt_clk >= anchor) && (bt_clk > anchor_playen_safe))
                    {
                        /* --------- ........ ---------- anchor_playen_safe --------- ........ --------- anchor -------- bt_clk ---------- */
                        first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_READY;
                        goto _find_first_packet;
                    }
                }
            }
        }
    }

_find_first_packet:
    if (first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_READY)
    {
        /* update play info */
        rx_handle->source_info.bt_in.play_en_bt_clk = anchor_playen;
        rx_handle->source_info.bt_in.first_anchor_bt_clk = anchor;
        rx_handle->source_info.bt_in.first_packet_bt_clk = anchor_timeout;
        rx_handle->source_info.bt_in.fetch_anchor_previous = (anchor_timeout+0x10000000-rx_handle->source_info.bt_in.bt_interval) & 0x0fffffff;
        rx_handle->source_info.bt_in.fetch_anchor = anchor_timeout;
        for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
        {
            if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
            {
                continue;
            }
            else
            {
                if (((rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous == 0) && ((rx_handle->source_info.bt_in.bt_info[i])->blk_index == 0))
                {
                    (rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous = (blk_index+blk_num-1)%blk_num;
                    (rx_handle->source_info.bt_in.bt_info[i])->blk_index = blk_index;
                }
            }
        }
        DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x] first packet is ready %d, 0x%x, 0x%x, 0x%x, 0x%x", 6,
                        rx_handle,
                        blk_index,
                        rx_handle->source_info.bt_in.first_anchor_bt_clk,
                        rx_handle->source_info.bt_in.first_packet_bt_clk,
                        bt_clk,
                        rx_handle->source_info.bt_in.play_en_bt_clk);
    }
    else
    {
        DSP_MW_LOG_W("[Wireless MIC RX][UL][WARNNING][handle 0x%x] first packet is not ready %d, 0x%x, 0x%x, 0x%x, 0x%x", 6,
                        rx_handle,
                        blk_index,
                        anchor,
                        anchor_timeout,
                        bt_clk,
                        anchor_playen);
    }

    return first_packet_status;
}

ATTR_TEXT_IN_IRAM static bool wireless_mic_rx_ul_fetch_time_is_timeout(wireless_mic_rx_ul_handle_t *rx_handle, uint32_t bt_clk, uint16_t bt_phase)
{
    bool ret = false;
    uint32_t fetch_anchor;
    uint32_t fetch_anchor_next;
    uint32_t fetch_anchor_previous;
    UNUSED(bt_phase);

    if ((rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED) || (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_READY))
    {
        if ((wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase%625) == 0)
        {
            fetch_anchor = rx_handle->source_info.bt_in.fetch_anchor;
        }
        else
        {
            fetch_anchor = (rx_handle->source_info.bt_in.fetch_anchor+0x10000000-1) & 0x0fffffff;
        }
        fetch_anchor_previous = rx_handle->source_info.bt_in.fetch_anchor_previous;
        // fetch_anchor_previous = (rx_handle->source_info.bt_in.fetch_anchor+0x10000000-rx_handle->source_info.bt_in.bt_retry_window) & 0x0fffffff;
        fetch_anchor_next = (rx_handle->source_info.bt_in.fetch_anchor+rx_handle->source_info.bt_in.bt_interval*1) & 0x0fffffff;
        // fetch_anchor_next_2 = (rx_handle->source_info.bt_in.fetch_anchor+rx_handle->source_info.bt_in.bt_interval*2) & 0x0fffffff;
        if (fetch_anchor < fetch_anchor_next)
        {
            if (bt_clk >= fetch_anchor_next)
            {
                /* --------- ........ --------- fetch_anchor ---------- fetch_anchor_next ---------- bt_clk ---------- fetch_anchor_next_2 --------- ........ -------- */
                ret = true;
            }
            else if ((bt_clk < fetch_anchor) && (bt_clk < fetch_anchor_previous) && (bt_clk < fetch_anchor_next))
            {
                /* --------- ........ --------  bt_clk --------- ........ --------- fetch_anchor_previous --------- fetch_anchor ---------- fetch_anchor_next ---------- fetch_anchor_next_2 ---------- */
                ret = true;
            }
        }
        else
        {
            if ((bt_clk >= fetch_anchor_next) && (bt_clk < fetch_anchor_previous))
            {
                /* --------- fetch_anchor_next ---------- fetch_anchor_next_2 --------- ........ ---------- bt_clk --------- ........ --------- fetch_anchor_previous --------- fetch_anchor -------- */
                /* --------- fetch_anchor_next ---------- bt_clk ---------- fetch_anchor_next_2 --------- ........ --------- ........ --------- fetch_anchor_previous --------- fetch_anchor -------- */
                ret = true;
            }
        }
    }

    #if WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][DEBUG][fetch_time_is_timeout]: %u, 0x%0x, 0x%0x, 0x%0x, 0x%0x, 0x%0x, 0x%0x, %d", 8, ret, bt_clk, bt_phase, fetch_anchor_previous, fetch_anchor, fetch_anchor_next, current_timestamp, hal_nvic_query_exception_number());
    #endif /* WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG */

    return ret;
}

ATTR_TEXT_IN_IRAM static void wireless_mic_rx_ul_process_timeout_packets(wireless_mic_rx_ul_handle_t *rx_handle, uint32_t bt_clk, uint16_t bt_phase)
{
    uint32_t i;
    n9_dsp_share_info_ptr p_share_info;
    uint32_t blk_size;
    uint32_t blk_num;
    uint32_t blk_index;
    uint32_t blk_index_previous;
    bool do_drop = false;
    uint32_t drop_frames = 0;
    uint32_t fetch_anchor_backup = rx_handle->source_info.bt_in.fetch_anchor_previous;

    while (wireless_mic_rx_ul_fetch_time_is_timeout(rx_handle, bt_clk, bt_phase) == true)
    {
        /* drop this packet */
        drop_frames += 1;
        rx_handle->source_info.bt_in.fetch_anchor_previous = rx_handle->source_info.bt_in.fetch_anchor;
        rx_handle->source_info.bt_in.fetch_anchor = (rx_handle->source_info.bt_in.fetch_anchor + rx_handle->source_info.bt_in.bt_interval) & 0x0fffffff;
        MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);
    }
    if (wireless_mic_rx_first_ul_handle->total_number <= 1)
    {
        if (drop_frames != 0)
        {
            do_drop = true;
        }
    }
    else
    {
        if ((drop_frames != 0) && (fetch_anchor_backup == wireless_mic_rx_packet_anchor_in_mixer))
        {
            /* multi stream case: the current stream is the first stream that process the new packet, so only the current stream do drop */
            do_drop = true;
        }
    }
    if (do_drop)
    {
        /* drop all timeout packets */
        for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
        {
            if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
            {
                continue;
            }
            else
            {
                /* get blk info */
                p_share_info = (n9_dsp_share_info_ptr)((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.share_info);
                blk_size     = p_share_info->sub_info.block_info.block_size;
                blk_num      = p_share_info->sub_info.block_info.block_num;
                /* update blk index */
                // (rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous = (rx_handle->source_info.bt_in.bt_info[i])->blk_index;
                blk_index_previous = (rx_handle->source_info.bt_in.bt_info[i])->blk_index;
                blk_index = ((rx_handle->source_info.bt_in.bt_info[i])->blk_index + drop_frames) % blk_num;
                (rx_handle->source_info.bt_in.bt_info[i])->blk_index = blk_index;
            }
        }
        DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR][handle 0x%x] packet is timeout %d, %d, %d, 0x%x, 0x%x, 0x%x", 7,
                        rx_handle,
                        drop_frames,
                        blk_index_previous,
                        blk_index,
                        rx_handle->source_info.bt_in.fetch_anchor_previous,
                        rx_handle->source_info.bt_in.fetch_anchor,
                        bt_clk);
    }
}

ATTR_TEXT_IN_IRAM static bool wireless_mic_rx_ul_fetch_timestamp_is_correct(wireless_mic_rx_ul_handle_t *rx_handle, uint32_t timestamp)
{
    bool ret = false;
    uint32_t fetch_anchor_max;
    uint32_t fetch_anchor_min;

    if ((rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED) || (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_READY))
    {
        if (timestamp == rx_handle->source_info.bt_in.fetch_anchor)
        {
            ret = true;
        }
        else
        {
            fetch_anchor_max = (rx_handle->source_info.bt_in.fetch_anchor+rx_handle->source_info.bt_in.bt_channel_anchor_diff) & 0x0fffffff;
            fetch_anchor_min = (rx_handle->source_info.bt_in.fetch_anchor+0x10000000-rx_handle->source_info.bt_in.bt_channel_anchor_diff) & 0x0fffffff;
            if (fetch_anchor_max > fetch_anchor_min)
            {
                if ((timestamp < fetch_anchor_max) && (timestamp > fetch_anchor_min))
                {
                    ret = true;
                }
            }
            else
            {
                if ((timestamp < fetch_anchor_max) || (timestamp > fetch_anchor_min))
                {
                    ret = true;
                }
            }
        }
    }

    return ret;
}

ATTR_TEXT_IN_IRAM static bool wireless_mic_rx_ul_packet_is_processed_by_other_stream(wireless_mic_rx_ul_handle_t *rx_handle)
{
    bool ret = false;

    if (wireless_mic_rx_first_ul_handle->total_number > 1)
    {
        if (wireless_mic_rx_packet_anchor_in_mixer == rx_handle->source_info.bt_in.fetch_anchor)
        {
            ret = true;
        }
    }

    return ret;
}

/* Public functions ----------------------------------------------------------*/
/******************************************************************************/
/*               ULL audio 2.0 dongle common Public Functions                 */
/******************************************************************************/
ATTR_TEXT_IN_IRAM bool wireless_mic_rx_ul_fetch_time_is_arrived(wireless_mic_rx_ul_handle_t *rx_handle, uint32_t bt_clk, uint16_t bt_phase)
{
    bool ret = false;
    uint32_t fetch_anchor_previous_10;
    uint32_t fetch_anchor;
    UNUSED(bt_phase);

    if ((rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED) || (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_READY))
    {
        fetch_anchor_previous_10 = (rx_handle->source_info.bt_in.fetch_anchor_previous+0x10000000-rx_handle->source_info.bt_in.bt_interval*9) & 0x0fffffff;
        // fetch_anchor = (rx_handle->source_info.bt_in.fetch_anchor + wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase/625) & 0x0fffffff;
        if ((wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase%625) == 0)
        {
            fetch_anchor = rx_handle->source_info.bt_in.fetch_anchor;
        }
        else
        {
            fetch_anchor = (rx_handle->source_info.bt_in.fetch_anchor+0x10000000-1) & 0x0fffffff;
        }
        if (fetch_anchor_previous_10 < fetch_anchor)
        {
            if (bt_clk >= fetch_anchor)
            {
                /* --------- ........ --------- fetch_anchor_previous ---------- fetch_anchor ---------- bt_clk --------- ........ -------- */
                ret = true;
            }
            else if ((bt_clk < fetch_anchor) && (bt_clk < fetch_anchor_previous_10))
            {
                /* ---------- bt_clk --------- ........ --------- fetch_anchor_previous_10 --------- ........ --------- fetch_anchor_previous ---------- fetch_anchor -------- */
                ret = true;
            }
        }
        else
        {
            if ((bt_clk >= fetch_anchor) && (bt_clk < fetch_anchor_previous_10))
            {
                /* ---------- fetch_anchor ---------- bt_clk --------- ........ --------- fetch_anchor_previous_10 --------- fetch_anchor_previous -------- */
                /* --------- fetch_anchor_previous ---------- fetch_anchor ---------- bt_clk --------- ........ --------- fetch_anchor_previous_10  --------- */
                ret = true;
            }
        }
    }

    #if WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][DEBUG][fetch_time_is_arrived]: %u, 0x%0x, 0x%0x, 0x%0x, 0x%0x, 0x%0x, %d", 7, ret, bt_clk, bt_phase, fetch_anchor_previous_10, fetch_anchor, current_timestamp, hal_nvic_query_exception_number());
    #endif /* WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG */

    return ret;
}

void wireless_mic_rx_init_play_info(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    wireless_mic_rx_init_play_info_t *play_info;
    wireless_mic_rx_ul_handle_t *c_handle;
    uint32_t i;
    uint32_t saved_mask;
    UNUSED(ack);

    /* save play info to the global variables */
    play_info = (wireless_mic_rx_init_play_info_t *)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    memcpy(&wireless_mic_rx_bt_init_play_info, play_info, sizeof(wireless_mic_rx_init_play_info_t));

    /* update uplink BT transmission window clk */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (wireless_mic_rx_first_ul_handle != NULL)
    {
        c_handle = wireless_mic_rx_first_ul_handle;
        for (i=0; i < (uint32_t)wireless_mic_rx_first_ul_handle->total_number; i++)
        {
            if ((wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk != 0) && (c_handle->source_info.bt_in.bt_retry_window != wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk))
            {
                c_handle->source_info.bt_in.bt_retry_window = wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk+wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase/625;
            }
            /* switch the next handle */
            c_handle = c_handle->next_ul_handle;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[Wireless MIC RX][UL][config] play_info->dl_timestamp_clk %d",   1, wireless_mic_rx_bt_init_play_info.dl_retransmission_window_clk);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][config] play_info->dl_timestamp_phase %d", 1, wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase);
}

ATTR_TEXT_IN_IRAM void wireless_mic_rx_playen_disable(SINK sink)
{
    UNUSED(sink);

    hal_audio_memory_parameter_t *mem_handle = &sink->param.audio.mem_handle;
    hal_audio_agent_t agent = hal_memory_convert_agent(mem_handle->memory_select);

    /* set DAC_CON0 */
    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            AFE_SET_REG(AFE_DAC_CON0, 0x1<<1, 0x1<<1);
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL2:
            AFE_SET_REG(AFE_DAC_CON0, 0x1<<2, 0x1<<2);
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL3:
            AFE_SET_REG(AFE_DAC_CON0, 0x1<<5, 0x1<<5);
            break;

        case HAL_AUDIO_AGENT_MEMORY_DL12:
            AFE_SET_REG(AFE_DAC_CON0, 0x1<<8, 0x1<<8);
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    /* reset playen */
    hal_memory_set_palyen(agent, false);
}

/******************************************************************************/
/*               ULL audio 2.0 dongle UL path Public Functions                */
/******************************************************************************/
uint32_t wireless_mic_rx_ul_get_stream_in_max_size_each_channel(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    uint32_t stream_in_size = 0;
    UNUSED(source);
    UNUSED(sink);

    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_MST_OUT_0:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            stream_in_size = 2*((rx_handle->source_info.bt_in.frame_size+sizeof(lc3plus_dec_frame_status_t)+3)/4*4);
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    return stream_in_size;
}

uint32_t wireless_mic_rx_ul_get_stream_in_channel_number(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    uint32_t channel_number = 0;
    UNUSED(source);
    UNUSED(sink);

    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_MST_OUT_0:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            channel_number = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    return channel_number;
}

stream_samplerate_t wireless_mic_rx_ul_get_stream_in_sampling_rate_each_channel(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    stream_samplerate_t samplerate = 0;
    UNUSED(source);
    UNUSED(sink);

    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_MST_OUT_0:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            samplerate = rx_handle->source_info.bt_in.sample_rate/1000;
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    return samplerate;
}

uint32_t wireless_mic_rx_ul_get_stream_out_max_size_each_channel(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    uint32_t stream_out_size = 0;
    uint32_t frame_samples;
    uint32_t frame_sample_size;
    UNUSED(source);
    UNUSED(sink);

    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
            frame_samples = max(rx_handle->sink_info.usb_out.frame_samples, rx_handle->source_info.bt_in.frame_samples*1000/rx_handle->source_info.bt_in.frame_interval);
            if ((rx_handle->sink_info.usb_out.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE) || (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE))
            {
                frame_sample_size = sizeof(int32_t);
            }
            else
            {
                frame_sample_size = sizeof(int16_t);
            }
            stream_out_size = 2*(rx_handle->sink_info.usb_out.frame_max_num*frame_samples*frame_sample_size);
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
            frame_samples = max(rx_handle->sink_info.line_out.frame_samples, rx_handle->source_info.bt_in.frame_samples*1000/rx_handle->source_info.bt_in.frame_interval);
            if ((rx_handle->sink_info.line_out.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE) || (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE))
            {
                frame_sample_size = sizeof(int32_t);
            }
            else
            {
                frame_sample_size = sizeof(int16_t);
            }
            stream_out_size = 2*(rx_handle->sink_info.line_out.frame_max_num*frame_samples*frame_sample_size);
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            frame_samples = max(rx_handle->sink_info.i2s_slv_out.frame_samples, rx_handle->source_info.bt_in.frame_samples*1000/rx_handle->source_info.bt_in.frame_interval);
            if ((rx_handle->sink_info.i2s_slv_out.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE) || (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE))
            {
                frame_sample_size = sizeof(int32_t);
            }
            else
            {
                frame_sample_size = sizeof(int16_t);
            }
            stream_out_size = 2*(rx_handle->sink_info.i2s_slv_out.frame_max_num*frame_samples*frame_sample_size);
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    return stream_out_size;
}

uint32_t wireless_mic_rx_ul_get_stream_out_channel_number(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    uint32_t channel_number = 0;
    UNUSED(source);
    UNUSED(sink);

    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            channel_number = WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER;
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    return channel_number;
}

stream_samplerate_t wireless_mic_rx_ul_get_stream_out_sampling_rate_each_channel(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    stream_samplerate_t samplerate = 0;
    UNUSED(source);
    UNUSED(sink);

    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
            samplerate = rx_handle->sink_info.usb_out.sample_rate/1000;
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
            samplerate = rx_handle->sink_info.line_out.sample_rate/1000;
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            samplerate = rx_handle->sink_info.i2s_slv_out.sample_rate/1000;
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    return samplerate;
}

void wireless_mic_rx_ul_init(SOURCE source, SINK sink, audio_transmitter_open_param_p audio_transmitter_open_param, bt_common_open_param_p bt_common_open_param)
{
    /* get handle for application config */
    wireless_mic_rx_ul_handle_t *rx_handle = wireless_mic_rx_ul_get_handle(source);
    uint32_t saved_mask;

    source->param.bt_common.scenario_param.dongle_handle = (void *)rx_handle;
    rx_handle->source = source;
    rx_handle->sink   = sink;
    rx_handle->sub_id = bt_common_open_param->scenario_sub_id;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (rx_handle->total_number == 1)
    {
        /* reset state machine */
        wireless_mic_rx_packet_anchor_in_mixer = UL_PACKET_ANCHOR_IN_MIXER_INVAILD_VALUE;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    /* task config */
    source->taskId = DAV_TASK_ID;
    sink->taskid = DAV_TASK_ID;

    /* init audio info */
    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
            wireless_mic_rx_ul_usb_out_init(rx_handle, audio_transmitter_open_param, bt_common_open_param);
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
            wireless_mic_rx_ul_line_out_init(rx_handle, (au_afe_open_param_p)audio_transmitter_open_param, bt_common_open_param);
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            wireless_mic_rx_ul_i2s_slv_out_init(rx_handle, (au_afe_open_param_p)audio_transmitter_open_param, bt_common_open_param);
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    /* lock sleep because sleep wake-up time will consume the stream process time */
    hal_sleep_manager_lock_sleep(SLEEP_LOCK_DONGLE_AUDIO);

    /* register ccni handler */
    bt_common_register_isr_handler(wireless_mic_rx_ul_ccni_handler);

    /* stream status update */
    rx_handle->stream_status = WIRELESS_MIC_RX_STREAM_INIT;
}

void wireless_mic_rx_ul_deinit(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    uint32_t saved_mask;
    UNUSED(sink);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (rx_handle->total_number == 1)
    {
        /* unregister ccni handler */
        bt_common_unregister_isr_handler(wireless_mic_rx_ul_ccni_handler);
        /* reset state machine */
        wireless_mic_rx_packet_anchor_in_mixer = UL_PACKET_ANCHOR_IN_MIXER_INVAILD_VALUE;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    /* deinit audio info */
    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
            wireless_mic_rx_ul_usb_out_deinit(rx_handle);
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
            wireless_mic_rx_ul_line_out_deinit(rx_handle);
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            wireless_mic_rx_ul_i2s_slv_out_deinit(rx_handle);
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    /* unlock sleep */
    hal_sleep_manager_unlock_sleep(SLEEP_LOCK_DONGLE_AUDIO);

    /* stream status update */
    rx_handle->stream_status = WIRELESS_MIC_RX_STREAM_DEINIT;

    /* release handle */
    wireless_mic_rx_ul_release_handle(rx_handle);
    source->param.bt_common.scenario_param.dongle_handle = NULL;
}

void wireless_mic_rx_ul_start(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    UNUSED(sink);

    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
            rx_handle->fetch_count = 0;
            rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY;
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
            rx_handle->fetch_count = 0;
            rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY;
            wireless_mic_rx_ul_line_out_start(rx_handle);
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            rx_handle->fetch_count = 0;
            rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY;
            wireless_mic_rx_ul_i2s_slv_out_start(rx_handle);
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    /* stream status update */
    rx_handle->stream_status = WIRELESS_MIC_RX_STREAM_START;
}

void wireless_mic_rx_ul_stop(SOURCE source, SINK sink)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    UNUSED(sink);

    switch (rx_handle->sub_id)
    {
        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0:
            rx_handle->fetch_count = 0;
            rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY;
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT:
            wireless_mic_rx_ul_line_out_stop(rx_handle);
            rx_handle->fetch_count = 0;
            rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY;
            break;

        case AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0:
            wireless_mic_rx_ul_i2s_slv_stop(rx_handle);
            rx_handle->fetch_count = 0;
            rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY;
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    /* stream status update */
    rx_handle->stream_status = WIRELESS_MIC_RX_STREAM_START;
}

bool wireless_mic_rx_ul_config(SOURCE source, stream_config_type type, U32 value)
{
    bool ret = true;
    wireless_mic_rx_runtime_config_param_p config_param = (wireless_mic_rx_runtime_config_param_p)value;
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    uint32_t vol_ch;
    int32_t vol_gain;
    sw_gain_config_t old_config;
    uint32_t i;
    wireless_mic_rx_ul_handle_t *c_handle;
    UNUSED(source);
    UNUSED(type);
    UNUSED(value);

    switch (config_param->config_operation)
    {
        case WIRELESS_MIC_RX_CONFIG_OP_SET_UL_VOL_INFO:
            vol_ch = config_param->config_param.vol_gain.vol_ch;
            if (vol_ch != 0)
            {
                vol_gain = config_param->config_param.vol_gain.vol_gain[vol_ch-1];
                stream_function_sw_gain_get_config(rx_handle->gain_port, vol_ch, &old_config);
                DSP_MW_LOG_I("[Wireless MIC RX][UL][config][handle 0x%x]scenario %d-%d change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 6,
                            rx_handle,
                            source->param.bt_common.scenario_type,
                            source->param.bt_common.scenario_sub_id,
                            vol_ch,
                            old_config.target_gain,
                            vol_gain);
                stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, vol_ch, vol_gain);
            }
            else
            {
                for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
                {
                    vol_gain = config_param->config_param.vol_gain.vol_gain[i];
                    vol_ch = 1+i;
                    stream_function_sw_gain_get_config(rx_handle->gain_port, vol_ch, &old_config);
                    DSP_MW_LOG_I("[Wireless MIC RX][UL][config][handle 0x%x]scenario %d-%d change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 6,
                                rx_handle,
                                source->param.bt_common.scenario_type,
                                source->param.bt_common.scenario_sub_id,
                                vol_ch,
                                old_config.target_gain,
                                vol_gain);
                    stream_function_sw_gain_configure_gain_target(rx_handle->gain_port, vol_ch, vol_gain);
                }
            }
            break;

        case WIRELESS_MIC_RX_CONFIG_OP_SET_UL_CONNECTION_INFO:
            /* connection info managerment */
            c_handle = wireless_mic_rx_first_ul_handle;
            for (i = 0; i < (uint32_t)wireless_mic_rx_first_ul_handle->total_number; i++)
            {
                /* check if the connection is differnet with the current status */
                if (memcmp(&(wireless_mic_rx_channel_connection_status[c_handle->sub_id][0]), &(config_param->config_param.connection_status[c_handle->sub_id][0]), sizeof(wireless_mic_rx_channel_connection_status[0])) != 0)
                {
                    memcpy(&(wireless_mic_rx_channel_connection_status[c_handle->sub_id][0]), &(config_param->config_param.connection_status[c_handle->sub_id][0]), sizeof(wireless_mic_rx_channel_connection_status[0]));
                    wireless_mic_rx_ul_mixer_all_stream_connections_update(c_handle, false);
                }
                /* switch the next stream */
                c_handle = c_handle->next_ul_handle;
            }
            for (i = 0; i < AUDIO_TRANSMITTER_WIRELESS_MIC_RX_SUB_ID_MAX; i++)
            {
                /* update the state machine */
                if (memcmp(&(wireless_mic_rx_channel_connection_status[i][0]), &(config_param->config_param.connection_status[i][0]), sizeof(wireless_mic_rx_channel_connection_status[0])) != 0)
                {
                    memcpy(&(wireless_mic_rx_channel_connection_status[i][0]), &(config_param->config_param.connection_status[i][0]), sizeof(wireless_mic_rx_channel_connection_status[0]));
                }
                DSP_MW_LOG_I("[Wireless MIC RX]][UL][config] stream scenario %d connection status: 0x%x, 0x%x, 0x%x, 0x%x.",
                            5,
                            i,
                            wireless_mic_rx_channel_connection_status[i][0],
                            wireless_mic_rx_channel_connection_status[i][1],
                            wireless_mic_rx_channel_connection_status[i][2],
                            wireless_mic_rx_channel_connection_status[i][3]);
            }
            DSP_MW_LOG_I("[Wireless MIC RX][UL][config][handle 0x%x]scenario %d-%d change connection done\r\n", 3,
                        rx_handle,
                        source->param.bt_common.scenario_type,
                        source->param.bt_common.scenario_sub_id);
            break;

        default:
            DSP_MW_LOG_E("[Wireless MIC RX][UL][config] unknow cmd:%d\r\n", 1, config_param->config_operation);
            break;
    }

    return ret;
}

ATTR_TEXT_IN_IRAM bool wireless_mic_rx_ul_source_get_avail_size(SOURCE source, uint32_t *avail_size)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    uint32_t bt_clk;
    uint16_t bt_phase;
    uint32_t saved_mask;

    /* get current bt clk */
    MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);

    if (rx_handle->fetch_count != 0)
    {
        /* If there is fetch requirement, we must wake up the stream even there is no packet on the share buffer */
        *avail_size = rx_handle->source_info.bt_in.frame_size;
    }
    else if (wireless_mic_rx_ul_fetch_time_is_arrived(rx_handle, bt_clk, bt_phase))
    {
        /* If there is data in the share buffer, we must wake up the stream to process it */
        *avail_size = rx_handle->source_info.bt_in.frame_size;
        /* USB-out case: latch unprocess frame count in the share buffer in irq level */
        if ((rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0) && (hal_nvic_query_exception_number() > 0))
        {
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            rx_handle->sink_info.usb_out.mcu_frame_count_latch = rx_handle->sink_info.usb_out.mcu_frame_count;
            hal_nvic_restore_interrupt_mask(saved_mask);
        }
        /* i2s-slv-out case: special case for debugging */
        if ((rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0) && (hal_nvic_query_exception_number() > 0))
        {
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            if ((bt_clk - rx_handle->ccni_in_bt_count) > rx_handle->source_info.bt_in.bt_interval)
            {
                rx_handle->ccni_in_bt_count = bt_clk;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &(rx_handle->ccni_in_gpt_count));
            }
            hal_nvic_restore_interrupt_mask(saved_mask);
        }
    }
    else
    {
        *avail_size = 0;
    }

    return true;
}

ATTR_TEXT_IN_IRAM uint32_t wireless_mic_rx_ul_source_copy_payload(SOURCE source, uint8_t *dst_buf, uint32_t length)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    DSP_STREAMING_PARA_PTR stream = DSP_Streaming_Get(source, source->transform->sink);
    uint32_t saved_mask;
    wireless_mic_rx_first_packet_status_t first_packet_status;
    uint32_t bt_clk;
    uint16_t bt_phase;
    uint32_t remain_samples;
    uint32_t frame_samples;
    uint32_t i;
    n9_dsp_share_info_ptr p_share_info;
    uint16_t read_offset;
    WIRELESS_MIC_RX_HEADER *p_ull_audio_header;
    uint8_t *src_buf;
    int32_t sample_size = 0;
    uint32_t play_en_nclk;
    uint16_t play_en_intra_clk;
    uint32_t play_en_clk;
    uint16_t play_en_phase;
    wireless_mic_rx_ul_handle_t *c_handle;
    bool play_en_is_same;

    /* dongle status check */
    switch (rx_handle->stream_status)
    {
        /* In this status stage, it means the stream is not ready */
        case WIRELESS_MIC_RX_STREAM_DEINIT:
        case WIRELESS_MIC_RX_STREAM_INIT:
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            rx_handle->fetch_count = 0;
            hal_nvic_restore_interrupt_mask(saved_mask);
            break;

        /* In this status stage, we will check if the newest packet is suitable for playing */
        case WIRELESS_MIC_RX_STREAM_START:
            /* get current bt clock */
            MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);
            first_packet_status = wireless_mic_rx_ul_first_packet_check(rx_handle, bt_clk, bt_phase);
            switch (first_packet_status)
            {
                case WIRELESS_MIC_RX_UL_FIRST_PACKET_NOT_READY:
                case WIRELESS_MIC_RX_UL_FIRST_PACKET_TIMEOUT:
                    /* reset fetch count */
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    rx_handle->fetch_count = 0;
                    hal_nvic_restore_interrupt_mask(saved_mask);
                    break;

                case WIRELESS_MIC_RX_UL_FIRST_PACKET_READY:
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    /* update stream status */
                    rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_READY;
                    rx_handle->stream_status = WIRELESS_MIC_RX_STREAM_RUNNING;
                    if ((rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT) || (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0))
                    {
                        hal_audio_memory_parameter_t *mem_handle = &rx_handle->sink->param.audio.mem_handle;
                        /* set play en to enable agent */
                        play_en_clk = rx_handle->source_info.bt_in.play_en_bt_clk;
                        play_en_phase = (wireless_mic_rx_bt_init_play_info.dl_retransmission_window_phase%625);
                        if (play_en_phase != 0)
                        {
                            /* clk-(625-(dl_retransmission_window_phase%625)) == clk-1+625-(625-(dl_retransmission_window_phase%625)) */
                            play_en_clk = (play_en_clk+0x10000000-1) & 0x0fffffff;
                            // play_en_phase = 625 - (625-play_en_phase);
                        }
                        MCE_TransBT2NativeClk(play_en_clk, play_en_phase, &play_en_nclk, &play_en_intra_clk, BT_CLK_Offset);
                        /* Note: prefill buffer length is 4 samples, so read offset need to be add 32(4sample*4byte*2ch) for debugging the latency */
                        AFE_SET_REG(AFE_MEMIF_PBUF_SIZE, 0, 0xC03); //reduce pbuffer size of DL1 and DL3
                        AFE_SET_REG(AFE_MEMIF_MINLEN, 0x1001, 0XF00F);//reduce pbuffer size of DL1 and DL3
                        hal_audio_afe_set_play_en(play_en_nclk, (uint32_t)play_en_intra_clk);
                        /* get agent */
                        hal_audio_agent_t agent;
                        agent = hal_memory_convert_agent(mem_handle->memory_select);
                        /* enable AFE irq here */
                        hal_memory_set_irq_enable(agent, HAL_AUDIO_CONTROL_ON);
                        /* stop timer */
                        wireless_mic_rx_software_timer_stop(rx_handle);
                        /* update state machine */
                        rx_handle->fetch_count = 0;
                        // rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED;
                        DSP_MW_LOG_I("[Wireless MIC RX][UL] Play_En config with bt_clk:0x%08x, bt_phase:0x%08x, play_en_nclk:0x%08x, play_en_intra_clk:0x%08x", 4, play_en_clk, play_en_phase, play_en_nclk, play_en_intra_clk);
                    }
                    hal_nvic_restore_interrupt_mask(saved_mask);
                    break;

                /* Error status */
                default:
                    AUDIO_ASSERT(0);
                    break;
            }
            break;

        /* In this status stage, the stream is started and we will set flag to fetch a new packet */
        case WIRELESS_MIC_RX_STREAM_RUNNING:
            break;

        /* Error status */
        default:
            AUDIO_ASSERT(0);
            break;
    }

    /* process the frame when the stream is running */
    rx_handle->drop_frames = 0;
    MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);
    if (rx_handle->stream_status == WIRELESS_MIC_RX_STREAM_RUNNING)
    {
        if (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_READY)
        {
            if ((rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT) || (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0))
            {
                /* line out and i2s slve out case */
                rx_handle->first_packet_status = WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED;
                rx_handle->buffer_output_size = 0;
                hal_nvic_restore_interrupt_mask(saved_mask);
                if (wireless_mic_rx_first_ul_handle->total_number > 1)
                {
                    /* check if the fetch time of the first packet is arrived if there is other streams */
                    if (wireless_mic_rx_ul_fetch_time_is_arrived(rx_handle, bt_clk, bt_phase) == true)
                    {
                        /* do decode */
                        rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_NORMAL;
                    }
                    else
                    {
                        play_en_clk = 0x10000000; /* invalid value */
                        play_en_is_same = true;
                        c_handle = wireless_mic_rx_first_ul_handle;
                        for (i = 0; i < (uint32_t)(wireless_mic_rx_first_ul_handle->total_number); i++)
                        {
                            if ((c_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT) || (c_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0))
                            {
                                if (play_en_clk == 0x10000000)
                                {
                                    play_en_clk = c_handle->source_info.bt_in.play_en_bt_clk;
                                }
                                else if (play_en_clk != c_handle->source_info.bt_in.play_en_bt_clk)
                                {
                                    play_en_is_same = false;
                                }
                            }
                            else
                            {
                                /* if there is a USB-out stream, the first packet's data status of line-out / i2s-slv-out stream always be WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER_BUT_UPDATE_FETCH */
                                play_en_is_same = false;
                            }
                            /* switch the next handle */
                            c_handle = c_handle->next_ul_handle;
                        }
                        if (play_en_is_same == true)
                        {
                            /* if the play_en of all AFE out streams are the same, only allow the stream to do decode */
                            /* this is to prevent that all stream data status are WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER_BUT_UPDATE_FETCH when the all the streams are started at the same time */
                            rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_NORMAL;
                        }
                        else
                        {
                            /* bypass decoder */
                            rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER_BUT_UPDATE_FETCH;
                        }
                    }
                }
                else
                {
                    /* do decode if there is only one stream */
                    rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_NORMAL;
                }
                hal_nvic_restore_interrupt_mask(saved_mask);
            }
            else
            {
                /* usb out case */
                sample_size = (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE) ? sizeof(int16_t) : sizeof(int32_t);
                frame_samples = rx_handle->source_info.bt_in.sample_rate/1000;
                /* first packet must not be timeout, so do not need to if the new packet is timeout here */
                /* fetch first packet data into sw buffer but not play */
                if (wireless_mic_rx_ul_fetch_time_is_arrived(rx_handle, bt_clk, bt_phase) == true)
                {
                    /* do decode */
                    rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_NORMAL;
                    rx_handle->buffer_output_size = rx_handle->buffer_default_output_size + frame_samples*sample_size;
                }
                else
                {
                    /* bypass decoder */
                    rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER;
                    rx_handle->buffer_output_size = frame_samples*sample_size;
                }
            }
        }
        else if (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED)
        {
            sample_size = (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE) ? sizeof(int16_t) : sizeof(int32_t);
            if (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0)
            {
                frame_samples = rx_handle->source_info.bt_in.sample_rate/1000;
                remain_samples = stream_function_sw_buffer_get_channel_used_size(rx_handle->buffer_port, 1) / sample_size;
                /* process timeout packets */
                wireless_mic_rx_ul_process_timeout_packets(rx_handle, bt_clk, bt_phase);
                /* check if the fetch time is arrived */
                if (wireless_mic_rx_ul_fetch_time_is_arrived(rx_handle, bt_clk, bt_phase) == true)
                {
                    /* do decode */
                    rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_NORMAL;
                    rx_handle->buffer_output_size = rx_handle->buffer_default_output_size + remain_samples/frame_samples*frame_samples*sample_size;
                }
                else
                {
                    /* bypass decoder */
                    rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER;
                    rx_handle->buffer_output_size = remain_samples/frame_samples*frame_samples*sample_size;
                }
            }
            else
            {
                /* do not check remain samples on the line-out and the i2s-slv out case */
                remain_samples = 0;
                /* process timeout packets */
                wireless_mic_rx_ul_process_timeout_packets(rx_handle, bt_clk, bt_phase);
                /* check if the fetch time is arrived */
                if (wireless_mic_rx_ul_fetch_time_is_arrived(rx_handle, bt_clk, bt_phase) == true)
                {
                    /* do decode */
                    rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_NORMAL;
                    rx_handle->buffer_output_size = 0;
                }
                else
                {
                    /* bypass decoder */
                    rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER;
                    rx_handle->buffer_output_size = 0;
                }
            }
        }
        else
        {
            /* bypass decoder */
            rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER;
            rx_handle->buffer_output_size = 0;
        }
    }
    else
    {
        /* bypass decoder */
        rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER;
        rx_handle->buffer_output_size = 0;
    }

    /* check if other stream has process this packet */
    if (rx_handle->data_status == WIRELESS_MIC_RX_UL_DATA_NORMAL)
    {
        if (wireless_mic_rx_ul_packet_is_processed_by_other_stream(rx_handle))
        {
            /* update status for bypass decoder */
            rx_handle->data_status = WIRELESS_MIC_RX_UL_DATA_IN_MIXER;
        }
        else
        {
            /* update mixer status */
            wireless_mic_rx_packet_anchor_in_mixer = rx_handle->source_info.bt_in.fetch_anchor;
        }
    }

    /* process the new packet */
    switch (rx_handle->data_status)
    {
        case WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER:
            rx_handle->source_info.bt_in.channel_data_status = 0;
            for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
            {
                if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
                {
                    continue;
                }
                else
                {
                    dst_buf = (uint8_t *)stream->callback.EntryPara.in_ptr[i];
                    /* set decoder do bypass flag */
                    *((lc3plus_dec_frame_status_t *)dst_buf) = LC3PLUS_DEC_FRAME_STATUS_BYPASS_DECODER;
                }
            }
            break;

        case WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER_BUT_UPDATE_FETCH:
            rx_handle->source_info.bt_in.channel_data_status = 0;
            for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
            {
                if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
                {
                    continue;
                }
                else
                {
                    dst_buf = (uint8_t *)stream->callback.EntryPara.in_ptr[i];
                    /* set decoder do bypass flag */
                    *((lc3plus_dec_frame_status_t *)dst_buf) = LC3PLUS_DEC_FRAME_STATUS_BYPASS_DECODER;
                }
            }
            /* update fetch timestamp */
            rx_handle->source_info.bt_in.fetch_anchor_previous = rx_handle->source_info.bt_in.fetch_anchor;
            rx_handle->source_info.bt_in.fetch_anchor = (rx_handle->source_info.bt_in.fetch_anchor + rx_handle->source_info.bt_in.bt_interval) & 0x0fffffff;
            break;

        case WIRELESS_MIC_RX_UL_DATA_PLC:
            rx_handle->source_info.bt_in.channel_data_status = 0;
            for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
            {
                if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
                {
                    continue;
                }
                else
                {
                    dst_buf = (uint8_t *)stream->callback.EntryPara.in_ptr[i];
                    /* set decoder do PLC flag */
                    *((lc3plus_dec_frame_status_t *)dst_buf) = LC3PLUS_DEC_FRAME_STATUS_PLC;
                }
            }
            break;

        case WIRELESS_MIC_RX_UL_DATA_NORMAL:
            rx_handle->drop_frames += 1;
            rx_handle->source_info.bt_in.channel_data_status = 0;
            for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
            {
                if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
                {
                    continue;
                }
                else
                {
                    dst_buf = (uint8_t *)stream->callback.EntryPara.in_ptr[i];
                    p_share_info = (n9_dsp_share_info_ptr)((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.share_info);
                    read_offset = (rx_handle->source_info.bt_in.bt_info[i])->blk_index*p_share_info->sub_info.block_info.block_size;
                    p_ull_audio_header = (WIRELESS_MIC_RX_HEADER *)(hal_memview_infrasys_to_dsp0(p_share_info->start_addr) + read_offset);
                    src_buf = (uint8_t *)(hal_memview_infrasys_to_dsp0(p_share_info->start_addr) + read_offset + sizeof(WIRELESS_MIC_RX_HEADER));
                    if (p_ull_audio_header->valid_packet == 0)
                    {
                        /* set decoder do PLC flag */
                        *((lc3plus_dec_frame_status_t *)dst_buf) = LC3PLUS_DEC_FRAME_STATUS_PLC;
                    }
                    else
                    {
                        /* check frame size */
                        if (p_ull_audio_header->PduLen != rx_handle->source_info.bt_in.frame_size)
                        {
                            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR][handle 0x%x] frame_size is not right %d, %d", 3,
                                            rx_handle,
                                            p_ull_audio_header->PduLen,
                                            rx_handle->source_info.bt_in.frame_size);
                            AUDIO_ASSERT(0);
                        }
                        /* check frame timestamp */
                        if (wireless_mic_rx_ul_fetch_timestamp_is_correct(rx_handle, p_ull_audio_header->TimeStamp))
                        {
                            /* set decoder do decode flag */
                            *((lc3plus_dec_frame_status_t *)dst_buf) = LC3PLUS_DEC_FRAME_STATUS_NORMAL;
                            /* copy frame data into the stream buffer */
                            memcpy(dst_buf+sizeof(lc3plus_dec_frame_status_t), src_buf, rx_handle->source_info.bt_in.frame_size);
                            rx_handle->source_info.bt_in.channel_data_status |= 0x1<<i;
                        }
                        else
                        {
                            /* set decoder do PLC flag */
                            *((lc3plus_dec_frame_status_t *)dst_buf) = LC3PLUS_DEC_FRAME_STATUS_PLC;
                            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR][handle 0x%x] timestamp is not right %d, 0x%x, 0x%x, 0x%x, 0x%x", 6,
                                            rx_handle,
                                            (rx_handle->source_info.bt_in.bt_info[i])->blk_index,
                                            rx_handle->source_info.bt_in.fetch_anchor_previous,
                                            rx_handle->source_info.bt_in.fetch_anchor,
                                            p_ull_audio_header->TimeStamp,
                                            bt_clk);
                        }
                    }
                }
            }
            /* update fetch timestamp */
            rx_handle->source_info.bt_in.fetch_anchor_previous = rx_handle->source_info.bt_in.fetch_anchor;
            rx_handle->source_info.bt_in.fetch_anchor = (rx_handle->source_info.bt_in.fetch_anchor + rx_handle->source_info.bt_in.bt_interval) & 0x0fffffff;
            break;

        case WIRELESS_MIC_RX_UL_DATA_IN_MIXER:
            rx_handle->source_info.bt_in.channel_data_status = 0;
            for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
            {
                if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
                {
                    continue;
                }
                else
                {
                    dst_buf = (uint8_t *)stream->callback.EntryPara.in_ptr[i];
                    /* set decoder do bypass flag */
                    *((lc3plus_dec_frame_status_t *)dst_buf) = LC3PLUS_DEC_FRAME_STATUS_BYPASS_DECODER;
                }
            }
            /* set drop frame to 0 because other stream has process this packet */
            rx_handle->drop_frames = 0;
            /* update fetch timestamp */
            rx_handle->source_info.bt_in.fetch_anchor_previous = rx_handle->source_info.bt_in.fetch_anchor;
            rx_handle->source_info.bt_in.fetch_anchor = (rx_handle->source_info.bt_in.fetch_anchor + rx_handle->source_info.bt_in.bt_interval) & 0x0fffffff;
            break;

        default:
            AUDIO_ASSERT(0);
            break;
    }

    /* update stream status */
    stream->callback.EntryPara.in_size = rx_handle->source_info.bt_in.frame_size+sizeof(lc3plus_dec_frame_status_t);
    if (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE)
    {
        stream->callback.EntryPara.resolution.source_in_res = RESOLUTION_16BIT;
        stream->callback.EntryPara.resolution.feature_res   = RESOLUTION_16BIT;
        stream->callback.EntryPara.resolution.sink_out_res  = RESOLUTION_16BIT;
        stream->callback.EntryPara.resolution.process_res   = RESOLUTION_16BIT;
    }
    else
    {
        stream->callback.EntryPara.resolution.source_in_res = RESOLUTION_32BIT;
        stream->callback.EntryPara.resolution.feature_res   = RESOLUTION_32BIT;
        stream->callback.EntryPara.resolution.sink_out_res  = RESOLUTION_32BIT;
        stream->callback.EntryPara.resolution.process_res   = RESOLUTION_32BIT;
    }

    /* do clock skew */
    if (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0)
    {
        /* configure clock skew settings */
        if (rx_handle->data_status != WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER)
        {
            rx_handle->compen_samples = wireless_mic_rx_ul_usb_clock_skew_check(rx_handle);
            stream_function_sw_clk_skew_configure_compensation_samples(rx_handle->clk_skew_port, rx_handle->compen_samples);
        }
        else
        {
            stream_function_sw_clk_skew_configure_compensation_samples(rx_handle->clk_skew_port, 0);
        }

        /* configure buffer output size */
        stream_function_sw_buffer_config_channel_output_size(rx_handle->buffer_port, 0, rx_handle->buffer_output_size);
    }
    else if (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0)
    {
        /* configure clock skew settings */
        if (rx_handle->data_status != WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER)
        {
            rx_handle->compen_samples = wireless_mic_rx_ul_i2s_slv_clock_skew_check(rx_handle);
            stream_function_sw_clk_skew_configure_compensation_samples(rx_handle->clk_skew_port, rx_handle->compen_samples);
        }
        else
        {
            stream_function_sw_clk_skew_configure_compensation_samples(rx_handle->clk_skew_port, 0);
        }
    }
    else
    {
        /* configure clock skew settings */
        if (rx_handle->data_status != WIRELESS_MIC_RX_UL_DATA_BYPASS_DECODER)
        {
            rx_handle->compen_samples = wireless_mic_rx_ul_line_out_clock_skew_check(rx_handle);
        }
    }

    #if WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][DEBUG][source_copy_payload]: 0x%0x, 0x%0x, 0x%0x, 0x%0x, 0x%0x, 0x%0x, %d", 7, rx_handle->data_status, rx_handle->drop_frames, rx_handle->source_info.bt_in.fetch_anchor_previous, rx_handle->source_info.bt_in.fetch_anchor, rx_handle->source_info.bt_in.channel_data_status, current_timestamp, hal_nvic_query_exception_number());
    #endif /* WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG */

    return length;
}

ATTR_TEXT_IN_IRAM bool wireless_mic_rx_ul_source_get_new_read_offset(SOURCE source, U32 amount, U32 *new_read_offset)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    uint32_t i;
    n9_dsp_share_info_ptr p_share_info;
    uint32_t blk_size;
    uint32_t blk_num;
    uint32_t blk_index;
    UNUSED(amount);
    UNUSED(new_read_offset);

    if (rx_handle->drop_frames != 0)
    {
        for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
        {
            if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
            {
                continue;
            }
            else
            {
                /* get blk info */
                p_share_info = (n9_dsp_share_info_ptr)((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.share_info);
                blk_size     = p_share_info->sub_info.block_info.block_size;
                blk_num      = p_share_info->sub_info.block_info.block_num;
                /* get new blk index */
                // (rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous = (rx_handle->source_info.bt_in.bt_info[i])->blk_index;
                blk_index = ((rx_handle->source_info.bt_in.bt_info[i])->blk_index + rx_handle->drop_frames) % blk_num;
                // (rx_handle->source_info.bt_in.bt_info[i])->blk_index = blk_index;
                /* update read index */
                StreamDSP_HWSemaphoreTake();
                p_share_info->read_offset = blk_index * blk_size;
                StreamDSP_HWSemaphoreGive();
            }
        }
    }

    /* we will update the read offsets of the different share buffers in here directly, so return false to aviod the upper layer update read offset */
    return false;
}

ATTR_TEXT_IN_IRAM void wireless_mic_rx_ul_source_drop_postprocess(SOURCE source, uint32_t amount)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)source->param.bt_common.scenario_param.dongle_handle;
    DSP_STREAMING_PARA_PTR stream = DSP_Streaming_Get(source, source->transform->sink);
    uint32_t i;
    uint32_t read_offset;
    n9_dsp_share_info_ptr p_share_info;
    uint32_t blk_size;
    uint32_t blk_num;
    uint32_t blk_index = 0;
    uint32_t blk_index_previous = 0;
    WIRELESS_MIC_RX_HEADER *p_ull_audio_header;
    uint32_t saved_mask;
    uint32_t bt_clk;
    uint16_t bt_phase;
    uint32_t remain_samples_in_share_buffer;
    uint32_t remain_samples_in_sw_buffer;
    uint32_t frame_samples;
    uint32_t sample_size = 0;
    uint8_t *read_pointer;
    uint32_t data_size;
    int32_t frac_rpt;
    UNUSED(amount);
    UNUSED(stream);

    /* update time stamp */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&rx_handle->data_out_gpt_count);
    hal_gpt_get_duration_count(rx_handle->ccni_in_gpt_count, rx_handle->data_out_gpt_count, &rx_handle->stream_process_count);

    /* get sample size */
    sample_size = (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE) ? sizeof(int16_t) : sizeof(int32_t);

    /* audio dump */
    #ifdef AIR_AUDIO_DUMP_ENABLE
    #if WIRELESS_MIC_RX_UL_PATH_DEBUG_DUMP
    if (rx_handle->data_status == WIRELESS_MIC_RX_UL_DATA_NORMAL)
    {
        LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.in_ptr[0])+sizeof(lc3plus_dec_frame_status_t), rx_handle->source_info.bt_in.frame_size, SOURCE_IN1);
        LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.in_ptr[1])+sizeof(lc3plus_dec_frame_status_t), rx_handle->source_info.bt_in.frame_size, SOURCE_IN2);
        LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.in_ptr[2])+sizeof(lc3plus_dec_frame_status_t), rx_handle->source_info.bt_in.frame_size, SOURCE_IN3);
        LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.in_ptr[3])+sizeof(lc3plus_dec_frame_status_t), rx_handle->source_info.bt_in.frame_size, SOURCE_IN4);
        /* dump channel 1's decoder output data */
        stream_function_sw_mixer_channel_input_get_data_info(rx_handle->mixer_member, 1, &read_pointer, NULL, &data_size);
        LOG_AUDIO_DUMP(read_pointer, data_size, VOICE_TX_MIC_0);
        /* dump channel 2's decoder output data */
        stream_function_sw_mixer_channel_input_get_data_info(rx_handle->mixer_member, 2, &read_pointer, NULL, &data_size);
        LOG_AUDIO_DUMP(read_pointer, data_size, VOICE_TX_MIC_1);
        /* dump channel 3's decoder output data */
        stream_function_sw_mixer_channel_input_get_data_info(rx_handle->mixer_member, 3, &read_pointer, NULL, &data_size);
        LOG_AUDIO_DUMP(read_pointer, data_size, VOICE_TX_MIC_2);
        /* dump channel 4's decoder output data */
        stream_function_sw_mixer_channel_input_get_data_info(rx_handle->mixer_member, 4, &read_pointer, NULL, &data_size);
        LOG_AUDIO_DUMP(read_pointer, data_size, VOICE_TX_MIC_3);
    }
    if (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0)
    {
        if (rx_handle->buffer_output_size != 0)
        {
            if (rx_handle->sink_info.usb_out.channel_num == 2)
            {
                LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.out_ptr[0]), rx_handle->process_frames*rx_handle->sink_info.usb_out.frame_samples*sample_size, VOICE_TX_OUT);
                LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.out_ptr[1]), rx_handle->process_frames*rx_handle->sink_info.usb_out.frame_samples*sample_size, VOICE_RX_PLC_IN);
            }
            else
            {
                LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.out_ptr[0]), rx_handle->process_frames*rx_handle->sink_info.usb_out.frame_samples*sample_size, VOICE_TX_OUT);
            }
        }
    }
    else if (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_LINE_OUT)
    {
        LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.out_ptr[0]), rx_handle->sink_info.line_out.frame_samples*rx_handle->sink_info.line_out.frame_interval/1000*sample_size, VOICE_TX_REF);
        LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.out_ptr[1]), rx_handle->sink_info.line_out.frame_samples*rx_handle->sink_info.line_out.frame_interval/1000*sample_size, VOICE_TX_NR_OUT);
    }
    else if (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0)
    {
        stream_function_sw_clk_skew_get_output_size(rx_handle->clk_skew_port, &data_size);
        LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.out_ptr[0]), data_size, VOICE_TX_CPD_IN);
        LOG_AUDIO_DUMP((uint8_t *)(stream->callback.EntryPara.out_ptr[1]), data_size, VOICE_TX_CPD_OUT);
    }
    #endif /* WIRELESS_MIC_RX_UL_PATH_DEBUG_DUMP */
    #endif /* AIR_AUDIO_DUMP_ENABLE */

    /* output debug log */
    MCE_GetBtClk((BTCLK*)&bt_clk, (BTPHASE*)&bt_phase, BT_CLK_Offset);
    for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
    {
        if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
        {
            continue;
        }
        else
        {
            blk_index_previous = (rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous;
            blk_index = (rx_handle->source_info.bt_in.bt_info[i])->blk_index;
            break;
        }
    }
    if (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_USB_OUT_0)
    {
        frame_samples = rx_handle->source_info.bt_in.sample_rate/1000;
        remain_samples_in_share_buffer = rx_handle->sink_info.usb_out.mcu_frame_count_latch*frame_samples;
        remain_samples_in_sw_buffer = stream_function_sw_buffer_get_channel_used_size(rx_handle->buffer_port, 1) / sample_size;
        stream_function_sw_clk_skew_get_frac_rpt(rx_handle->clk_skew_port, 1, &frac_rpt);
        DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x][source_drop_postprocess] usb0-out %d, %d, %d, %d, %d, %d, 0x%x, 0x%x, 0x%x, %d, %d, %d, %d, %d, %d, 0x%x, 0x%x, 0x%x, %d", 20,
                        rx_handle,
                        rx_handle->fetch_count,
                        rx_handle->stream_status,
                        rx_handle->first_packet_status,
                        rx_handle->data_status,
                        rx_handle->drop_frames,
                        blk_index,
                        rx_handle->source_info.bt_in.fetch_anchor_previous,
                        bt_clk,
                        rx_handle->source_info.bt_in.channel_data_status,
                        rx_handle->buffer_output_size/sample_size,
                        remain_samples_in_share_buffer,
                        remain_samples_in_sw_buffer,
                        rx_handle->clk_skew_count,
                        frac_rpt,
                        rx_handle->compen_samples,
                        rx_handle->ccni_in_bt_count,
                        rx_handle->data_out_bt_count,
                        rx_handle->ccni_in_gpt_count,
                        rx_handle->stream_process_count);
    }
    else if (rx_handle->sub_id == AUDIO_TRANSMITTER_WIRELESS_MIC_RX_UL_I2S_SLV_OUT_0)
    {
        rx_handle->sink_info.i2s_slv_out.pre_write_offset = rx_handle->sink_info.i2s_slv_out.cur_write_offset;
        rx_handle->sink_info.i2s_slv_out.pre_read_offset  = rx_handle->sink_info.i2s_slv_out.cur_read_offset;
        rx_handle->sink_info.i2s_slv_out.cur_write_offset = source->transform->sink->streamBuffer.BufferInfo.WriteOffset;
        rx_handle->sink_info.i2s_slv_out.cur_read_offset  = source->transform->sink->streamBuffer.BufferInfo.ReadOffset;
        rx_handle->sink_info.i2s_slv_out.afe_cur          = AFE_GET_REG(rx_handle->sink_info.i2s_slv_out.afe_cur_addr);
        rx_handle->sink_info.i2s_slv_out.afe_base         = AFE_GET_REG(rx_handle->sink_info.i2s_slv_out.afe_base_addr);
        DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x][source_drop_postprocess] i2ss-out %d, %d, %d, %d, %d, %d, 0x%x, 0x%x, 0x%x, %d, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, %d", 19,
                        rx_handle,
                        rx_handle->fetch_count,
                        rx_handle->stream_status,
                        rx_handle->first_packet_status,
                        rx_handle->data_status,
                        rx_handle->drop_frames,
                        blk_index,
                        rx_handle->source_info.bt_in.fetch_anchor_previous,
                        bt_clk,
                        rx_handle->source_info.bt_in.channel_data_status,
                        rx_handle->clk_skew_count,
                        rx_handle->compen_samples,
                        rx_handle->sink_info.i2s_slv_out.cur_write_offset,
                        rx_handle->sink_info.i2s_slv_out.cur_read_offset,
                        rx_handle->sink_info.i2s_slv_out.afe_cur,
                        rx_handle->ccni_in_bt_count,
                        rx_handle->ccni_in_gpt_count,
                        rx_handle->data_out_gpt_count,
                        rx_handle->stream_process_count);
    }
    else
    {
        rx_handle->sink_info.line_out.pre_write_offset = rx_handle->sink_info.line_out.cur_write_offset;
        rx_handle->sink_info.line_out.pre_read_offset  = rx_handle->sink_info.line_out.cur_read_offset;
        rx_handle->sink_info.line_out.cur_write_offset = source->transform->sink->streamBuffer.BufferInfo.WriteOffset;
        rx_handle->sink_info.line_out.cur_read_offset  = source->transform->sink->streamBuffer.BufferInfo.ReadOffset;
        rx_handle->sink_info.line_out.afe_cur          = AFE_GET_REG(rx_handle->sink_info.line_out.afe_cur_addr);
        rx_handle->sink_info.line_out.afe_base         = AFE_GET_REG(rx_handle->sink_info.line_out.afe_base_addr);
        DSP_MW_LOG_I("[Wireless MIC RX][UL][handle 0x%x][source_drop_postprocess] line-out %d, %d, %d, %d, %d, %d, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, %d", 17,
                        rx_handle,
                        rx_handle->fetch_count,
                        rx_handle->stream_status,
                        rx_handle->first_packet_status,
                        rx_handle->data_status,
                        rx_handle->drop_frames,
                        blk_index,
                        rx_handle->source_info.bt_in.fetch_anchor_previous,
                        bt_clk,
                        rx_handle->source_info.bt_in.channel_data_status,
                        rx_handle->sink_info.line_out.cur_write_offset,
                        rx_handle->sink_info.line_out.cur_read_offset,
                        rx_handle->sink_info.line_out.afe_cur,
                        rx_handle->ccni_in_bt_count,
                        rx_handle->ccni_in_gpt_count,
                        rx_handle->data_out_gpt_count,
                        rx_handle->stream_process_count);
    }

    /* drop packets */
    if (rx_handle->drop_frames != 0)
    {
        for (i = 0; i < WIRELESS_MIC_RX_DATA_CHANNEL_NUMBER; i++)
        {
            if (rx_handle->source_info.bt_in.bt_info[i] == NULL)
            {
                continue;
            }
            else
            {
                /* get blk info */
                p_share_info = (n9_dsp_share_info_ptr)((rx_handle->source_info.bt_in.bt_info[i])->bt_link_info.share_info);
                blk_size     = p_share_info->sub_info.block_info.block_size;
                blk_num      = p_share_info->sub_info.block_info.block_num;
                /* update blk index */
                (rx_handle->source_info.bt_in.bt_info[i])->blk_index_previous = (rx_handle->source_info.bt_in.bt_info[i])->blk_index;
                blk_index_previous = (rx_handle->source_info.bt_in.bt_info[i])->blk_index;
                blk_index = ((rx_handle->source_info.bt_in.bt_info[i])->blk_index + rx_handle->drop_frames) % blk_num;
                (rx_handle->source_info.bt_in.bt_info[i])->blk_index = blk_index;
                /* clear valid flag */
                while (blk_index != blk_index_previous)
                {
                    /* get packet header */
                    read_offset = blk_index_previous*blk_size;
                    p_ull_audio_header = (WIRELESS_MIC_RX_HEADER *)(hal_memview_infrasys_to_dsp0(p_share_info->start_addr) + read_offset);
                    /* clear valid flag */
                    p_ull_audio_header->valid_packet = 0;
                    /* switch the next block */
                    blk_index_previous = (blk_index_previous+1)% blk_num;
                }
            }
        }
        rx_handle->drop_frames = 0;
    }

    /* decrease fetch count */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (rx_handle->fetch_count != 0)
    {
        rx_handle->fetch_count -= 1;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);
}

bool wireless_mic_rx_ul_source_close(SOURCE source)
{
    UNUSED(source);

    return true;
}

ATTR_TEXT_IN_IRAM bool wireless_mic_rx_ul_sink_get_avail_size(SINK sink, uint32_t *avail_size)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)(sink->transform->source->param.bt_common.scenario_param.dongle_handle);
    uint32_t frame_num;

    if (sink->streamBuffer.ShareBufferInfo.read_offset < sink->streamBuffer.ShareBufferInfo.write_offset)
    {
        /* normal case */
        *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_num
                    - sink->streamBuffer.ShareBufferInfo.write_offset
                    + sink->streamBuffer.ShareBufferInfo.read_offset;
    }
    else if (sink->streamBuffer.ShareBufferInfo.read_offset == sink->streamBuffer.ShareBufferInfo.write_offset)
    {
        if(sink->streamBuffer.ShareBufferInfo.bBufferIsFull == true)
        {
            /* buffer is full, so read_offset == write_offset */
            *avail_size = 0;
        }
        else
        {
            /* buffer is empty, so read_offset == write_offset */
            *avail_size = sink->streamBuffer.ShareBufferInfo.length;
        }
    }
    else
    {
        /* buffer wrapper case */
        *avail_size = sink->streamBuffer.ShareBufferInfo.read_offset - sink->streamBuffer.ShareBufferInfo.write_offset;
    }

    frame_num = *avail_size / sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_size;
    if ((frame_num < rx_handle->sink_info.usb_out.frame_max_num) && (rx_handle->fetch_count != 0))
    {
        /* in each time, there must be at least 10ms data free space in the share buffer */
        DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]frame num is not right, %u, %u, %u\r\n", 3, frame_num, rx_handle->sink_info.usb_out.frame_max_num, rx_handle->fetch_count);
        AUDIO_ASSERT(0);
    }

    return true;
}

ATTR_TEXT_IN_IRAM uint32_t wireless_mic_rx_ul_sink_copy_payload(SINK sink, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)(sink->transform->source->param.bt_common.scenario_param.dongle_handle);
    DSP_STREAMING_PARA_PTR stream = DSP_Streaming_Get(sink->transform->source, sink);
    uint32_t process_frames;
    uint32_t saved_mask;
    uint32_t i;
    uint32_t payload_size;
    UNUSED(src_buf);

    /* copy pcm data into the share buffer one by one 1ms */
    if (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE)
    {
        if (length%(sizeof(int32_t)*rx_handle->sink_info.usb_out.frame_samples) != 0)
        {
            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]length is not right, %u\r\n", 1, length);
            AUDIO_ASSERT(0);
        }
        process_frames = ((length/sizeof(int32_t))/rx_handle->sink_info.usb_out.frame_samples);
    }
    else
    {
        if (length%(sizeof(int16_t)*rx_handle->sink_info.usb_out.frame_samples) != 0)
        {
            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]length is not right, %u\r\n", 1, length);
            AUDIO_ASSERT(0);
        }
        process_frames = ((length/sizeof(int16_t))/rx_handle->sink_info.usb_out.frame_samples);
    }
    for (i = 0; i < process_frames; i++)
    {
        /* get dst buffer */
        dst_buf = (uint8_t *)(sink->streamBuffer.ShareBufferInfo.start_addr + ((sink->streamBuffer.ShareBufferInfo.write_offset + sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_size*i) % (sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_num)));

        if ((rx_handle->sink_info.usb_out.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE) && (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE))
        {
            /* copy pcm samples into the share buffer */
            if (rx_handle->sink_info.usb_out.channel_num == 2)
            {
                // DSP_D2I_BufferCopy_16bit((U16 *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                //                         (U16 *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                //                         (U16 *)(stream->callback.EntryPara.out_ptr[1])+i*rx_handle->sink_info.usb_out.frame_samples,
                //                         rx_handle->sink_info.usb_out.frame_samples);
                ShareBufferCopy_D_16bit_to_I_16bit_2ch( (uint16_t *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint16_t *)(stream->callback.EntryPara.out_ptr[1])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint16_t *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                                                        rx_handle->sink_info.usb_out.frame_samples);
                payload_size = rx_handle->sink_info.usb_out.frame_samples*2*sizeof(int16_t);
            }
            else
            {
                // memcpy(dst_buf+sizeof(audio_transmitter_frame_header_t), (U16 *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples, rx_handle->sink_info.usb_out.frame_samples*sizeof(int16_t));
                ShareBufferCopy_D_16bit_to_D_16bit_1ch( (uint16_t *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint16_t *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                                                        rx_handle->sink_info.usb_out.frame_samples);
                payload_size = rx_handle->sink_info.usb_out.frame_samples*sizeof(int16_t);
            }
        }
        else if ((rx_handle->sink_info.usb_out.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE) && (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE))
        {
            /* copy pcm samples into the share buffer */
            if (rx_handle->sink_info.usb_out.channel_num == 2)
            {
                ShareBufferCopy_D_32bit_to_I_16bit_2ch( (uint32_t *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint32_t *)(stream->callback.EntryPara.out_ptr[1])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint16_t *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                                                        rx_handle->sink_info.usb_out.frame_samples);
                payload_size = rx_handle->sink_info.usb_out.frame_samples*2*sizeof(int16_t);
            }
            else
            {
                ShareBufferCopy_D_32bit_to_D_16bit_1ch( (uint32_t *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint16_t *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                                                        rx_handle->sink_info.usb_out.frame_samples);
                payload_size = rx_handle->sink_info.usb_out.frame_samples*sizeof(int16_t);
            }
        }
        else if ((rx_handle->sink_info.usb_out.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE) && (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S16_LE))
        {
            /* copy pcm samples into the share buffer */
            if (rx_handle->sink_info.usb_out.channel_num == 2)
            {
                ShareBufferCopy_D_16bit_to_I_24bit_2ch( (uint16_t *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint16_t *)(stream->callback.EntryPara.out_ptr[1])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint8_t *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                                                        rx_handle->sink_info.usb_out.frame_samples);
                payload_size = rx_handle->sink_info.usb_out.frame_samples*2*3;
            }
            else
            {
                ShareBufferCopy_D_16bit_to_D_24bit_1ch( (uint16_t *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint8_t *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                                                        rx_handle->sink_info.usb_out.frame_samples);
                payload_size = rx_handle->sink_info.usb_out.frame_samples*3;
            }
        }
        else if ((rx_handle->sink_info.usb_out.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE) && (rx_handle->source_info.bt_in.sample_format == HAL_AUDIO_PCM_FORMAT_S24_LE))
        {
            /* copy pcm samples into the share buffer */
            if (rx_handle->sink_info.usb_out.channel_num == 2)
            {
                ShareBufferCopy_D_32bit_to_I_24bit_2ch( (uint32_t *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint32_t *)(stream->callback.EntryPara.out_ptr[1])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint8_t *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                                                        rx_handle->sink_info.usb_out.frame_samples);
                payload_size = rx_handle->sink_info.usb_out.frame_samples*2*3;
            }
            else
            {
                ShareBufferCopy_D_32bit_to_D_24bit_1ch( (uint32_t *)(stream->callback.EntryPara.out_ptr[0])+i*rx_handle->sink_info.usb_out.frame_samples,
                                                        (uint8_t *)(dst_buf+sizeof(audio_transmitter_frame_header_t)),
                                                        rx_handle->sink_info.usb_out.frame_samples);
                payload_size = rx_handle->sink_info.usb_out.frame_samples*3;
            }
        }
        else
        {
            DSP_MW_LOG_E("[Wireless MIC RX][UL][ERROR]sample_format is not supported, %u, %u\r\n", 2, rx_handle->sink_info.usb_out.sample_format, rx_handle->source_info.bt_in.sample_format);
            AUDIO_ASSERT(0);
        }

        /* write seq number and payload_len into the share buffer */
        ((audio_transmitter_frame_header_t *)dst_buf)->seq_num      = rx_handle->sink_info.usb_out.seq_num;
        ((audio_transmitter_frame_header_t *)dst_buf)->payload_len  = payload_size;

        /* update seq number */
        rx_handle->sink_info.usb_out.seq_num = (rx_handle->sink_info.usb_out.seq_num+1)&0xffff;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED)
    {
        rx_handle->process_frames = process_frames;
    }
    else
    {
        /* If stream is not played, only copy data into share buffer but not update write offset */
        rx_handle->process_frames += process_frames;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    return payload_size;
}

ATTR_TEXT_IN_IRAM void wireless_mic_rx_ul_sink_query_write_offset(SINK sink, uint32_t *write_offset)
{
    wireless_mic_rx_ul_handle_t *rx_handle = (wireless_mic_rx_ul_handle_t *)(sink->transform->source->param.bt_common.scenario_param.dongle_handle);
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (rx_handle->first_packet_status == WIRELESS_MIC_RX_UL_FIRST_PACKET_PLAYED)
    {
        /* If stream is played, update write offset */
        *write_offset = (sink->streamBuffer.ShareBufferInfo.write_offset + sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_size * rx_handle->process_frames) % (sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.block_num);
        audio_transmitter_share_information_update_write_offset(sink, *write_offset);
    }
    else
    {
        /* If stream is not played, only copy data into share buffer but not update write offset */
        *write_offset = sink->streamBuffer.ShareBufferInfo.write_offset;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    /* update time stamp */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&rx_handle->data_out_bt_count);

    #if WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[Wireless MIC RX][UL][DEBUG][sink_query_write_offset]: %u, %u, %d, %d", 4, rx_handle->first_packet_status, *write_offset, current_timestamp, hal_nvic_query_exception_number());
    #endif /* WIRELESS_MIC_RX_UL_PATH_DEBUG_LOG */
}

bool wireless_mic_rx_ul_sink_close(SINK sink)
{
    UNUSED(sink);

    return true;
}

#endif /* AIR_WIRELESS_MIC_RX_ENABLE */
