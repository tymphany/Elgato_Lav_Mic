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

#ifdef AIR_USB_AUDIO_ENABLE

#if defined(MTK_USB_AUDIO_V2_ENABLE)
#error "MTK_USB_AUDIO_V2_ENABLE, USB-Audio version is not supported !"
#endif

/* C library */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* USB Middleware includes */
#include "usb.h"
#include "usb_host_detect.h"
#include "usb_nvkey_struct.h"
#include "usb_resource.h"
#include "usbaudio_drv.h"

/* Other Middleware includes */
#ifdef AIR_NVDM_ENABLE
#include "nvkey.h"
#include "nvkey_id_list.h"
#endif /* AIR_NVDM_ENABLE */

/* Hal includes */
#include "hal.h"
#include "hal_usb.h"
#include "hal_usb_internal.h"
#include "hal_resource_assignment.h"

/* Other includes */
#include "memory_attribute.h"

#if defined(USB_AUDIO_TX_TIME_MEASURE) || defined(USB_AUDIO_RX_TIME_MEASURE)
#include "swla.h"
#endif

#if defined(USB_AUDIO_DUMP_RX1) || defined(USB_AUDIO_DUMP_RX2) || defined(USB_AUDIO_DUMP_TX1)
#include "audio_dump.h"
#endif

/* Syslog create module for usbaudio_drv.c */
#include "syslog.h"
log_create_module_variant(USBAUDIO_DRV, DEBUG_LOG_ON, PRINT_LEVEL_INFO);

#if defined(AIR_USB_AUDIO_ENABLE)

#define AUDFREQ_TO_U32(FREQ) ((FREQ.data[0] << 0) + (FREQ.data[1] << 8) + (FREQ.data[2] << 16))
#define U32_TO_AUDFREQ(U32)  ({((U32 & 0x0000FF) >> 0), ((U32 & 0x00FF00) >> 8), ((U32 & 0xFF0000) >> 16)})

#ifdef AIR_USB_AUDIO_1_SPK_ENABLE
#define AIR_USB_AUDIO_1_SPK_FEATURE_ENABLE (true)
#else
#define AIR_USB_AUDIO_1_SPK_FEATURE_ENABLE (false)
#endif /* AIR_USB_AUDIO_1_SPK_ENABLE */

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
#define AIR_USB_AUDIO_1_MIC_FEATURE_ENABLE (true)
#else
#define AIR_USB_AUDIO_1_MIC_FEATURE_ENABLE (false)
#endif /* AIR_USB_AUDIO_1_MIC_ENABLE */

#ifdef AIR_USB_AUDIO_2_SPK_ENABLE
#define AIR_USB_AUDIO_2_SPK_FEATURE_ENABLE (true)
#else
#define AIR_USB_AUDIO_2_SPK_FEATURE_ENABLE (false)
#endif /* AIR_USB_AUDIO_2_SPK_ENABLE */

#ifdef AIR_USB_AUDIO_2_MIC_ENABLE
#define AIR_USB_AUDIO_2_MIC_FEATURE_ENABLE (true)
#else
#define AIR_USB_AUDIO_2_MIC_FEATURE_ENABLE (false)
#endif /* AIR_USB_AUDIO_2_MIC_ENABLE */

UsbAudioStruct USB_Audio[2];
UsbAudio_Struct g_UsbAudio[2] = {
    /* Audio 1 device settings */
    {
        .spk_feature_en    = AIR_USB_AUDIO_1_SPK_FEATURE_ENABLE,
        .spk_alt2_en       = false,
        .spk_alt3_en       = false,
        .mic_feature_en    = AIR_USB_AUDIO_1_MIC_FEATURE_ENABLE,
        .mic_alt2_en       = false,
        .mic_alt3_en       = false,
        .spk_terminal_type = 0x0402,
        .spk_cur           = 0xF59A,
        .spk_min           = 0xB600,
        .spk_max           = 0x0000,
        .spk_res           = 0x0100,
        .mic_terminal_type = 0x0402,
        .mic_cur           = 0xF59A,
        .mic_min           = 0xB600,
        .mic_max           = 0x0000,
        .mic_res           = 0x0100,
    },
    /* Audio 2 device settings */
    {
        .spk_feature_en    = false,
        .spk_alt2_en       = true,  /* default is true and decided by USB_AUDIO_RX1_ALT2_ENABLE or USB_Aduio_Set_RX2_Alt2 */
        .mic_feature_en    = false,
        .spk_terminal_type = 0x0402,
        .spk_cur           = 0xF59A,
        .spk_min           = 0xB600,
        .spk_max           = 0x0000,
        .spk_res           = 0x0100,
        .mic_terminal_type = 0x0402,
        .mic_cur           = 0xF59A,
        .mic_min           = 0xB600,
        .mic_max           = 0x0000,
        .mic_res           = 0x0100,
    },
};

/* static functions */
static int16_t usb_audio_dscr_interface_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t usb_audio_dscr_control_header_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t usb_audio_dscr_it_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t usb_audio_dscr_ot_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t usb_audio_dscr_mixer_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t usb_audio_dscr_selector_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t usb_audio_dscr_feature_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t usb_audio_dscr_as_general_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t usb_audio_dscr_as_format_type_serialize(void *dscr, void *out, uint16_t ava_size) __unused;
static int16_t dscr_list_serialize(usb_audio_dscr_hdlr_t *dscr_list, uint8_t len, uint8_t *out, uint16_t ava_len) __unused;

static uint8_t _USB_Audio_Channel_Check(uint8_t ch)
{
    if(ch>USB_AUDIO_MAX_CHANNEL_NUM){
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Check_Channel ch:%d ?? force to 0", 1, ch);
        ch = 0;
    }

    return ch;
}

/******************************************  Configuration Start    *********************************************/
static usb_audio_dscr_interface_t audio1_interface_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio1_ControlIf_Create */
    .bAlternateSetting  = 0x00,
    .bNumEndpoints      = 0x00,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x01,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio1_ControlIf_Create */
};

static usb_audio_dscr_control_header_t audio1_ach_dscr = {
    .bLength            = 0x00, /* Overwrite by USB_Audio1_ControlIf_Create */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_HEADER,
    .bcdADC             = 0x0100,
    .wTotalLength       = 0x0047, /* Overwrite by USB_Audio1_ControlIf_Create */
    .bInCollection      = 0x02,   /* Overwrite by USB_Audio1_ControlIf_Create */
    .baInterfaceNr      =         /* Overwrite by USB_Audio1_ControlIf_Create */
    {
        0x01,
        0x02,
    },
};

static usb_audio_dscr_it_t audio1_spk_it_dscr = {
    .bLength            = 0x0C,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_INPUT_TERMINAL,
    .bTerminalID        = 0x01,
    .wTerminalType      = 0x0101,
    .bAssocTerminal     = 0x00,
    .bNrChannels        = 0x02,
    .wChannelConfig     = 0x0003, /* (L, R) */
    .iChannelNames      = 0x00,
    .iTerminal          = 0x00,
};

static usb_audio_dscr_feature_t audio1_spk_feature_dscr = {
    .bLength            = 0x0A,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_FEATURE_UNIT,
    .bUnitID            = 0x02,
    .bSourceID          = 0x01,
    .bControlSize       = 0x01,
    .bmaControls =
        {
            0x01, /* bmaControls(0); Master Channel support Mute      */
            0x02, /* bmaControls(1); Logical Channel 1 support Volume */
            0x02, /* bmaControls(2); Logical Channel 2 support Volume */
        },
    .iFeature   = 0x00,
    .control_nr = 0x03, /* num of bmaControls */
};

static usb_audio_dscr_ot_t audio1_spk_ot_dscr = {
    .bLength            = 0x09,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_OUTPUT_TERMINAL,
    .bTerminalID        = 0x03,
    .wTerminalType      = 0x0402,
    .bAssocTerminal     = 0x00,
    .bSourceID          = 0x02,
    .iTerminal          = 0x00,
};

static usb_audio_dscr_it_t audio1_mic_it_dscr = {
    .bLength            = 0x0C,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_INPUT_TERMINAL,
    .bTerminalID        = 0x05,
    .wTerminalType      = 0x0201,
    .bAssocTerminal     = 0x0B,
    .bNrChannels        = 0x01,
    .wChannelConfig     = 0x0000,
    .iChannelNames      = 0x00,
    .iTerminal          = 0x00,
};

static usb_audio_dscr_feature_t audio1_mic_feature_dscr = {
    .bLength            = 0x09,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_FEATURE_UNIT,
    .bUnitID            = 0x06,
    .bSourceID          = 0x05,
    .bControlSize       = 0x01,
    .bmaControls =
        {
            0x01, /* bmaControls(0); Master Channel support Mute      */
            0x02, /* bmaControls(1); Logical Channel 1 support Volume */
        },
    .iFeature   = 0x00,
    .control_nr = 0x02, /* num of bmaControls */
};

static usb_audio_dscr_selector_t audio1_selector_dscr = {
    .bLength            = 0x07,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_SELECTOR_UNIT,
    .bUnitID            = 0x04,
    .bNrInPins          = 0x01,
    .baSourceID =
        {
            0x06,
        },
    .iSelector = 0x00,
};

static usb_audio_dscr_ot_t audio1_mic_ot_dscr = {
    .bLength            = 0x09,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_OUTPUT_TERMINAL,
    .bTerminalID        = 0x07,
    .wTerminalType      = 0x0101,
    .bAssocTerminal     = 0x00,
    .bSourceID          = 0x04,
    .iTerminal          = 0x00,
};

static usb_audio_dscr_hdlr_t audio1_aci_dscrs[] = {
    {&audio1_interface_dscr, usb_audio_dscr_interface_serialize},
    {&audio1_ach_dscr, usb_audio_dscr_control_header_serialize},
};

static usb_audio_dscr_hdlr_t audio1_spk_dscrs[] = {
    {&audio1_spk_it_dscr, usb_audio_dscr_it_serialize},
    {&audio1_spk_feature_dscr, usb_audio_dscr_feature_serialize},
    {&audio1_spk_ot_dscr, usb_audio_dscr_ot_serialize},
};

static usb_audio_dscr_hdlr_t audio1_mic_dscrs[] = {
    {&audio1_mic_it_dscr, usb_audio_dscr_it_serialize},
    {&audio1_mic_feature_dscr, usb_audio_dscr_feature_serialize},
    {&audio1_selector_dscr, usb_audio_dscr_selector_serialize},
    {&audio1_mic_ot_dscr, usb_audio_dscr_ot_serialize},
};

static usb_audio_dscr_interface_t audio2_interface_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio2_ControlIf_Create */
    .bAlternateSetting  = 0x00,
    .bNumEndpoints      = 0x00,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x01,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio2_ControlIf_Create */
};

static usb_audio_dscr_control_header_t audio2_ach_dscr = {
    .bLength            = 0x00, /* Overwrite by USB_Audio2_ControlIf_Create */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_HEADER,
    .bcdADC             = 0x0100,
    .wTotalLength       = 0x0047, /* Overwrite by USB_Audio2_ControlIf_Create */
    .bInCollection      = 0x02,   /* Overwrite by USB_Audio2_ControlIf_Create */
    .baInterfaceNr      =         /* Overwrite by USB_Audio2_ControlIf_Create */
    {
        0x01,
        0x02,
    },
};

static usb_audio_dscr_it_t audio2_spk_it_dscr = {
    .bLength            = 0x0C,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_INPUT_TERMINAL,
    .bTerminalID        = 0x09,
    .wTerminalType      = 0x0101,
    .bAssocTerminal     = 0x00,
    .bNrChannels        = 0x02,
    .wChannelConfig     = 0x0003, /* (L, R) */
    .iChannelNames      = 0x00,
    .iTerminal          = 0x00,
};

static usb_audio_dscr_feature_t audio2_spk_feature_dscr = {
    .bLength            = 0x0A,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_FEATURE_UNIT,
    .bUnitID            = 0x0A,
    .bSourceID          = 0x09,
    .bControlSize       = 0x01,
    .bmaControls =
        {
            0x01, /* bmaControls(0); Master Channel support Mute      */
            0x02, /* bmaControls(1); Logical Channel 1 support Volume */
            0x02, /* bmaControls(2); Logical Channel 2 support Volume */
        },
    .iFeature   = 0x00,
    .control_nr = 0x03, /* num of bmaControls */
};

static usb_audio_dscr_ot_t audio2_spk_ot_dscr = {
    .bLength            = 0x09,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_OUTPUT_TERMINAL,
    .bTerminalID        = 0x0B,
    .wTerminalType      = 0x0302,
    .bAssocTerminal     = 0x00,
    .bSourceID          = 0x0A,
    .iTerminal          = 0x00,
};

static usb_audio_dscr_it_t audio2_mic_it_dscr = {
    .bLength            = 0x0C,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_INPUT_TERMINAL,
    .bTerminalID        = 0x0C,
    .wTerminalType      = 0x0201,
    .bAssocTerminal     = 0x0B,
    .bNrChannels        = 0x01,
    .wChannelConfig     = 0x0000,
    .iChannelNames      = 0x00,
    .iTerminal          = 0x00,
};

static usb_audio_dscr_feature_t audio2_mic_feature_dscr = {
    .bLength            = 0x09,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_FEATURE_UNIT,
    .bUnitID            = 0x0D,
    .bSourceID          = 0x0C,
    .bControlSize       = 0x01,
    .bmaControls =
        {
            0x01, /* bmaControls(0); Master Channel support Mute      */
            0x02, /* bmaControls(1); Logical Channel 1 support Volume */
        },
    .iFeature   = 0x00,
    .control_nr = 0x02, /* num of bmaControls */
};

static usb_audio_dscr_selector_t audio2_selector_dscr = {
    .bLength            = 0x07,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_SELECTOR_UNIT,
    .bUnitID            = 0x0E,
    .bNrInPins          = 0x01,
    .baSourceID =
        {
            0x0D,
        },
    .iSelector = 0x00,
};

static usb_audio_dscr_ot_t audio2_mic_ot_dscr = {
    .bLength            = 0x09,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AC_SUBTYPE_OUTPUT_TERMINAL,
    .bTerminalID        = 0x0F,
    .wTerminalType      = 0x0101,
    .bAssocTerminal     = 0x00,
    .bSourceID          = 0x0E,
    .iTerminal          = 0x00,
};

static usb_audio_dscr_hdlr_t audio2_aci_dscrs[] = {
    {&audio2_interface_dscr, usb_audio_dscr_interface_serialize},
    {&audio2_ach_dscr, usb_audio_dscr_control_header_serialize},
};

static usb_audio_dscr_hdlr_t audio2_spk_dscrs[] = {
    {&audio2_spk_it_dscr, usb_audio_dscr_it_serialize},
    {&audio2_spk_feature_dscr, usb_audio_dscr_feature_serialize},
    {&audio2_spk_ot_dscr, usb_audio_dscr_ot_serialize},
};

static usb_audio_dscr_hdlr_t audio2_mic_dscrs[] = {
    {&audio2_mic_it_dscr, usb_audio_dscr_it_serialize},
    {&audio2_mic_feature_dscr, usb_audio_dscr_feature_serialize},
    {&audio2_selector_dscr, usb_audio_dscr_selector_serialize},
    {&audio2_mic_ot_dscr, usb_audio_dscr_ot_serialize},
};

/****************************************** Configuration End *********************************************/
static usb_audio_dscr_interface_t audio1_spk_stream_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio1_StreamIf_Create */
    .bAlternateSetting  = 0x00,
    .bNumEndpoints      = 0x00,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio1_StreamIf_Create */
};

static usb_audio_dscr_interface_t audio1_spk_stream_alt_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio1_StreamIf_Create */
    .bAlternateSetting  = 0x01,
    .bNumEndpoints      = 0x01,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio1_StreamIf_Create */
};

#ifdef USB_AUDIO_RX1_ALT2_ENABLE
static usb_audio_dscr_interface_t audio1_spk_stream_alt2_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio1_StreamIf_Create */
    .bAlternateSetting  = 0x02,
    .bNumEndpoints      = 0x01,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio1_StreamIf_Create */
};
#endif

#ifdef USB_AUDIO_RX1_ALT3_ENABLE
static usb_audio_dscr_interface_t audio1_spk_stream_alt3_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio1_StreamIf_Create */
    .bAlternateSetting  = 0x03,
    .bNumEndpoints      = 0x01,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio1_StreamIf_Create */
};
#endif

static usb_audio_dscr_as_general_t audio1_spk_stream_general_dscr = {
    .bLength            = 0x07,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_GENERAL,
    .bTerminalLink      = 0x01, /* Overwrite by USB_Audio1_StreamIf_Create */
    .bDelay             = 0x01,
    .wFormatTag         = 0x0001, /* PCM */
};

static usb_audio_dscr_as_format_type_t audio1_spk_stream_format_dscr = {
    .bLength            = 0x08 + 3 * 1, /* 8 + freq_size(3) * num_of_freq */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_FORMAT_TYPE,
    .bFormatType        = 0x01, /* FORMAT_TYPE_I        */
    .bNrChannels        = 0x02, /* 2 channels           */
    .bSubFrameSize      = 0x02, /* 2 bytes per subframe */
    .bBitResolution     = 0x10, /* 16 bits per sample   */
    .bSamFreqType       = 0x01, /* num of tSamFreq      */
    .tSamFreq =
        {
            {.data = {0x80, 0xBB, 0x00}}, /* tSamFreq[1]; 48000 Hz */
            {.data = {0x00, 0x77, 0x01}}, /* tSamFreq[2]; 96000 Hz */
        },
};

#ifdef USB_AUDIO_RX1_ALT2_ENABLE
static usb_audio_dscr_as_format_type_t audio1_spk_stream_alt2_format_dscr = {
    .bLength            = 0x08 + 3 * 2, /* 8 + freq_size(3) * num_of_freq */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_FORMAT_TYPE,
    .bFormatType        = 0x01, /* FORMAT_TYPE_I        */
    .bNrChannels        = 0x02, /* 2 channels           */
    .bSubFrameSize      = 0x03, /* 3 bytes per subframe */
    .bBitResolution     = 0x18, /* 24 bits per sample   */
    .bSamFreqType       = 0x02, /* num of tSamFreq      */
    .tSamFreq =
        {
            {.data = {0x80, 0xBB, 0x00}}, /* tSamFreq[1]; 48000 Hz */
            {.data = {0x00, 0x77, 0x01}}, /* tSamFreq[2]; 96000 Hz */
        },
};
#endif

#ifdef USB_AUDIO_RX1_ALT3_ENABLE
static usb_audio_dscr_as_format_type_t audio1_spk_stream_alt3_format_dscr = {
    .bLength            = 0x08 + 3 * 2, /* 8 + freq_size(3) * num_of_freq */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_FORMAT_TYPE,
    .bFormatType        = 0x01, /* FORMAT_TYPE_I        */
    .bNrChannels        = 0x02, /* 2 channels           */
    .bSubFrameSize      = 0x03, /* 3 bytes per subframe */
    .bBitResolution     = 0x18, /* 24 bits per sample   */
    .bSamFreqType       = 0x01, /* num of tSamFreq      */
    .tSamFreq =
        {
            {.data = {0x00, 0xEE, 0x02}}, /* tSamFreq[1]; 192000 Hz */
        },
};
#endif

static usb_audio_dscr_hdlr_t audio1_spk_as_if_dscrs[] = {
    {&audio1_spk_stream_if_dscr, usb_audio_dscr_interface_serialize},
};

static usb_audio_dscr_hdlr_t audio1_spk_as_if_alt_dscrs[] = {
    {&audio1_spk_stream_alt_if_dscr, usb_audio_dscr_interface_serialize},
    {&audio1_spk_stream_general_dscr, usb_audio_dscr_as_general_serialize},
    {&audio1_spk_stream_format_dscr, usb_audio_dscr_as_format_type_serialize},
};

#ifdef USB_AUDIO_RX1_ALT2_ENABLE
static usb_audio_dscr_hdlr_t audio1_spk_as_if_alt2_dscrs[] = {
    {&audio1_spk_stream_alt2_if_dscr, usb_audio_dscr_interface_serialize},
    {&audio1_spk_stream_general_dscr, usb_audio_dscr_as_general_serialize},
    {&audio1_spk_stream_alt2_format_dscr, usb_audio_dscr_as_format_type_serialize},
};
#endif

#ifdef USB_AUDIO_RX1_ALT3_ENABLE
static usb_audio_dscr_hdlr_t audio1_spk_as_if_alt3_dscrs[] = {
    {&audio1_spk_stream_alt3_if_dscr, usb_audio_dscr_interface_serialize},
    {&audio1_spk_stream_general_dscr, usb_audio_dscr_as_general_serialize},
    {&audio1_spk_stream_alt3_format_dscr, usb_audio_dscr_as_format_type_serialize},
};
#endif

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
static usb_audio_dscr_interface_t audio1_mic_stream_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio_StreamIf_Microphone_Create */
    .bAlternateSetting  = 0x00,
    .bNumEndpoints      = 0x00,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio_StreamIf_Microphone_Create */
};

static usb_audio_dscr_interface_t audio1_mic_stream_alt_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio_StreamIf_Microphone_Create */
    .bAlternateSetting  = 0x01,
    .bNumEndpoints      = 0x01,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio_StreamIf_Microphone_Create */
};

#ifdef USB_AUDIO_TX1_ALT2_ENABLE
static usb_audio_dscr_interface_t audio1_mic_stream_alt2_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio_StreamIf_Microphone_Create */
    .bAlternateSetting  = 0x02,
    .bNumEndpoints      = 0x01,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio_StreamIf_Microphone_Create */
};
#endif

static usb_audio_dscr_as_general_t audio1_mic_stream_general_dscr = {
    .bLength            = 0x07,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_GENERAL,
    .bTerminalLink      = 0x07, /* Overwrite by USB_Audio_StreamIf_Microphone_Create */
    .bDelay             = 0x01,
    .wFormatTag         = 0x0001, /* PCM */
};

static usb_audio_dscr_as_format_type_t audio1_mic_stream_format_dscr = {
    .bLength            = 0x08 + 3 * 2, /* 8 + freq_size(3) * num_of_freq */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_FORMAT_TYPE,
    .bFormatType        = 0x01,     /* FORMAT_TYPE_I */
    .bNrChannels        = 0x01,     /* 1 channel */
    .bSubFrameSize      = 0x02,     /* 2 bytes per subframe */
    .bBitResolution     = 0x10,     /* 16 bits per sample */
    .bSamFreqType       = 0x02,     /* num of tSamFreq      */
    .tSamFreq =
        {
            {.data = {0x80, 0x3E, 0x00}}, /* tSamFreq[1]; 16000 Hz */
            {.data = {0x80, 0xBB, 0x00}}, /* tSamFreq[2]; 48000 Hz */
        },
};

#ifdef USB_AUDIO_TX1_ALT2_ENABLE
static usb_audio_dscr_as_format_type_t audio1_mic_stream_alt2_format_dscr = {
    .bLength            = 0x08 + 3 * 2, /* 8 + freq_size(3) * num_of_freq */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_FORMAT_TYPE,
    .bFormatType        = 0x01,     /* FORMAT_TYPE_I */
    .bNrChannels        = 0x01,     /* 1 channel */
    .bSubFrameSize      = 0x03,     /* 3 bytes per subframe */
    .bBitResolution     = 0x18,     /* 24 bits per sample */
    .bSamFreqType       = 0x02,     /* num of tSamFreq      */
    .tSamFreq =
        {
            {.data = {0x80, 0x3E, 0x00}}, /* tSamFreq[1]; 16000 Hz */
            {.data = {0x80, 0xBB, 0x00}}, /* tSamFreq[2]; 48000 Hz */
        },
};
#endif

static usb_audio_dscr_hdlr_t audio1_mic_as_if_dscrs[] = {
    {&audio1_mic_stream_if_dscr, usb_audio_dscr_interface_serialize},
};

static usb_audio_dscr_hdlr_t audio1_mic_as_if_alt_dscrs[] = {
    {&audio1_mic_stream_alt_if_dscr, usb_audio_dscr_interface_serialize},
    {&audio1_mic_stream_general_dscr, usb_audio_dscr_as_general_serialize},
    {&audio1_mic_stream_format_dscr, usb_audio_dscr_as_format_type_serialize},
};

#ifdef USB_AUDIO_TX1_ALT2_ENABLE
static usb_audio_dscr_hdlr_t audio1_mic_as_if_alt2_dscrs[] = {
    {&audio1_mic_stream_alt2_if_dscr, usb_audio_dscr_interface_serialize},
    {&audio1_mic_stream_general_dscr, usb_audio_dscr_as_general_serialize},
    {&audio1_mic_stream_alt2_format_dscr, usb_audio_dscr_as_format_type_serialize},
};
#endif

#endif /* AIR_USB_AUDIO_1_MIC_ENABLE */

static usb_audio_dscr_interface_t audio2_spk_stream_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio2_StreamIf_Create */
    .bAlternateSetting  = 0x00,
    .bNumEndpoints      = 0x00,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio2_StreamIf_Create */
};

static usb_audio_dscr_interface_t audio2_spk_stream_alt_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio2_StreamIf_Create */
    .bAlternateSetting  = 0x01,
    .bNumEndpoints      = 0x01,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio2_StreamIf_Create */
};

#ifdef USB_AUDIO_RX2_ALT2_ENABLE
static usb_audio_dscr_interface_t audio2_spk_stream_alt2_if_dscr = {
    .bLength            = USB_IFDSC_LENGTH,
    .bDescriptorType    = USB_INTERFACE,
    .bInterfaceNumber   = 0x00, /* Overwrite by USB_Audio2_StreamIf_Create */
    .bAlternateSetting  = 0x02,
    .bNumEndpoints      = 0x01,
    .bInterfaceClass    = 0x01,
    .bInterfaceSubClass = 0x02,
    .bInterfaceProtocol = USB_AUDIO_INTERFACE_PROTOCOL,
    .iInterface         = 0x00, /* Overwrite by USB_Audio2_StreamIf_Create */
};
#endif

static usb_audio_dscr_as_general_t audio2_spk_stream_general_dscr = {
    .bLength            = 0x07,
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_GENERAL,
    .bTerminalLink      = 0x09, /* Overwrite by USB_Audio2_StreamIf_Create */
    .bDelay             = 0x03,
    .wFormatTag         = 0x0001, /* PCM */
};

static usb_audio_dscr_as_format_type_t audio2_spk_stream_format_dscr = {
    .bLength            = 0x08 + 3 * 2, /* 8 + freq_size(3) * num_of_freq */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_FORMAT_TYPE,
    .bFormatType        = 0x01, /* FORMAT_TYPE_I        */
    .bNrChannels        = 0x02, /* 2 channels           */
    .bSubFrameSize      = 0x03, /* 3 bytes per subframe */
    .bBitResolution     = 0x18, /* 24 bits per sample   */
    .bSamFreqType       = 0x02, /* num of tSamFreq      */
    .tSamFreq =
        {
            {.data = {0x80, 0xBB, 0x00}}, /* tSamFreq[1]; 48000 Hz */
            {.data = {0x00, 0x77, 0x01}}, /* tSamFreq[2]; 96000 Hz */
        },
};

#ifdef USB_AUDIO_RX2_ALT2_ENABLE
static usb_audio_dscr_as_format_type_t audio2_spk_stream_alt2_format_dscr = {
    .bLength            = 0x08 + 3 * 2, /* 8 + freq_size(3) * num_of_freq */
    .bDescriptorType    = 0x24,
    .bDescriptorSubtype = USB_AUDIO_AS_SUBTYPE_FORMAT_TYPE,
    .bFormatType        = 0x01, /* FORMAT_TYPE_I        */
    .bNrChannels        = 0x02, /* 2 channels           */
    .bSubFrameSize      = 0x03, /* 2 bytes per subframe */
    .bBitResolution     = 0x18, /* 24 bits per sample   */
    .bSamFreqType       = 0x02, /* num of tSamFreq      */
    .tSamFreq =
        {
            {.data = {0x80, 0xBB, 0x00}}, /* tSamFreq[1]; 48000 Hz */
            {.data = {0x00, 0x77, 0x01}}, /* tSamFreq[2]; 96000 Hz */
        },
};
#endif

static usb_audio_dscr_hdlr_t audio2_spk_as_if_dscrs[] = {
    {&audio2_spk_stream_if_dscr, usb_audio_dscr_interface_serialize},
};

static usb_audio_dscr_hdlr_t audio2_spk_as_if_alt_dscrs[] = {
    {&audio2_spk_stream_alt_if_dscr, usb_audio_dscr_interface_serialize},
    {&audio2_spk_stream_general_dscr, usb_audio_dscr_as_general_serialize},
    {&audio2_spk_stream_format_dscr, usb_audio_dscr_as_format_type_serialize},
};

#ifdef USB_AUDIO_RX2_ALT2_ENABLE
static usb_audio_dscr_hdlr_t audio2_spk_as_if_alt2_dscrs[] = {
    {&audio2_spk_stream_alt2_if_dscr, usb_audio_dscr_interface_serialize},
    {&audio2_spk_stream_general_dscr, usb_audio_dscr_as_general_serialize},
    {&audio2_spk_stream_alt2_format_dscr, usb_audio_dscr_as_format_type_serialize},
};
#endif

/****************************************** Speaker Endpoint Start *********************************************/

const uint8_t audio_stream_ep_out_dscr[] = {
    /* Speaker Usage*/
    0x09,             /*bLength;*/
    USB_ENDPOINT,     /*bDescriptorType;*/
    0x00,             /*bEndpointAddress;*/
    USB_EP_ISO | (USB_ISO_ADAPTIVE << 2), /*bmAttributes;*/
    0x00,             /*wMaxPacketSize[2]; Will be replaced by HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED &*/
    0x00,             /*wMaxPacketSize[2]; HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_FULL_SPEED.*/
    0x01,             /*bInterval; Will be changed to 0x04 if high-speed @ USB_Audio_StreamIf_Speed_Reset().*/
    0x00,             /*bRefresh;*/
    0x00,             /*bSynchAddress;*/

    0x07,             /*bLength*/
    0x25,             /*bDescriptorType; Audio Endpoint Descriptor*/
    0x01,             /*bDescriptorSubtype; General*/
    0x01,             /*bmAttributes; Sampling Frequency*/
    0x00,             /*bLockDelayUnits*/
    0x00,             /*wLockDelay; LSB*/
    0x00,             /*wLockDelay; MSB*/
};
/****************************************** Speaker Endpoint End *********************************************/

/****************************************** Microphone Endpoint Start *********************************************/
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
const uint8_t audio_stream_ep_in_dscr[] = { /* microphone usage*/
    0x09,             /*bLength;*/
    USB_ENDPOINT,     /*bDescriptorType;*/
    0x00,             /*bEndpointAddress;*/
    USB_EP_ISO | (USB_ISO_ADAPTIVE << 2), /*bmAttributes;*/
    0x02,             /*wMaxPacketSize[2]; Will be replaced by HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED &*/
    0x00,             /*wMaxPacketSize[2]; HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_FULL_SPEED.*/
    0x01,             /*bInterval; Will be changed to 0x04 if high-speed @ USB_Audio_StreamIf_Speed_Reset().*/
    0x00,             /*bRefresh;*/
    0x00,             /*bSynchAddress;*/

    0x07,             /*bLength*/
    0x25,             /*bDescriptorType; Audio Endpoint Descriptor*/
    0x01,             /*bDescriptorSubtype; General*/
    0x01,             /*bmAttributes; Sampling Frequency*/
    0x00,             /*bLockDelayUnits*/
    0x00,             /*wLockDelay; LSB*/
    0x00,             /*wLockDelay; MSB*/
};
#endif
/******************************************Microphone Endpoint end*********************************************/
/**
 * Final setting would be set up by USB_Audio_Get_RX_Alt_Byte befor descriptort generator of usb_resource.c
 * 1. USB_Software_Create 2. USB_Software_Speed_Init
 */
static unsigned int USB_Audio_Stream_Speed_Reset_Packet_Size(uint32_t port, bool b_other_speed)
{
    unsigned int  max_packet;

    /**
     * NOTE
     * Each endpoint cost some bus bandwidth. The total cost of each
     * endpoint may not over 100%. If over 100%, Windows will show
     * "Not enough USB controller resources", and usb device can't
     * normally work.
     *
     * The isochronous transaction cost persent could refers to
     * USB2.0 spec 5.6.5.
     */
    max_packet = USB_AUDIO_RX_EP_MAX_SIZE;

    return max_packet;
}

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
static unsigned int USB_Audio_Stream_Microphone_Speed_Reset_Packet_Size(uint32_t port, bool b_other_speed)
{
    unsigned int  max_packet;

    max_packet = USB_AUDIO_TX_EP_MAX_SIZE;

    return max_packet;
}
#endif

/************************************************************
    USB Audio SetInterface Convertor
*************************************************************/
/* Convertor from Alt number to enable/disable */
bool USB_Audio_RX_SetAlt2Enable(uint32_t port, uint8_t alt_num)
{
    bool enable = false;

    if((port == 0) && (alt_num == 0)){
        enable = false;
    }
    else if((port == 0) && (alt_num == 1)){
        enable = true;
    }
    #ifdef USB_AUDIO_RX1_ALT2_ENABLE
    else if((port == 0) && (alt_num == 2)){
        enable = true;
    }
    #endif
    #ifdef USB_AUDIO_RX1_ALT3_ENABLE
    else if((port == 0) && (alt_num == 3)){
        enable = true;
    }
    #endif
    else if((port == 1) && (alt_num == 0)){
        enable = false;
    }
    else if((port == 1) && (alt_num == 1)){
        enable = true;
    }
    #ifdef USB_AUDIO_RX2_ALT2_ENABLE
     else if((port == 1) && (alt_num == 2)){
        enable = true;
    }
    #endif
    else {
        enable = false;
    }

    return enable;
}

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
bool USB_Audio_TX_SetAlt2Enable(uint32_t port, uint8_t alt_num)
{
    bool enable = false;

    if((port == 0) && (alt_num == 0)){
        enable = false;
    }
    else if((port == 0) && (alt_num == 1)){
        enable = true;
    }
    #ifdef USB_AUDIO_TX1_ALT2_ENABLE
    else if((port == 0) && (alt_num == 2)){
        enable = true;
    }
    #endif
    else {
        enable = false;
    }

    return enable;
}
#endif

/************************************************************
    USB Audio Get/Set interface and endpoint ID
*************************************************************/
uint8_t audio1_rx_if_id, audio1_rx_ep_id;
uint8_t audio1_tx_if_id, audio1_tx_ep_id;
uint8_t audio2_rx_if_id, audio2_rx_ep_id;

void USB_Audio_Init_Chat_Game_Info()
{
    audio1_rx_if_id = USB_AUDIO_UNUSED_ID;
    audio1_rx_ep_id = USB_AUDIO_UNUSED_ID;
    audio1_tx_if_id = USB_AUDIO_UNUSED_ID;
    audio1_tx_ep_id = USB_AUDIO_UNUSED_ID;
    audio2_rx_if_id = USB_AUDIO_UNUSED_ID;
    audio2_rx_ep_id = USB_AUDIO_UNUSED_ID;
}

void USB_Audio_Get_Chat_Info(uint8_t *rx_if_id, uint8_t *rx_ep_id, uint8_t *tx_if_id, uint8_t *tx_ep_id)
{
    *rx_if_id = audio1_rx_if_id;
    *rx_ep_id = audio1_rx_ep_id;
    *tx_if_id = audio1_tx_if_id;
    *tx_ep_id = audio1_tx_ep_id;
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_Chat_Info rx_if_id:0x%X rx_ep_id:0x%X tx_if_id:0x%X tx_ep_id:0x%X", 4,
                *rx_if_id, *rx_ep_id, *tx_if_id, *tx_ep_id);
}

void USB_Audio_Get_Game_Info(uint8_t *rx_if_id, uint8_t *rx_ep_id)
{
    *rx_if_id = audio2_rx_if_id;
    *rx_ep_id = audio2_rx_ep_id;
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_Game_Info rx_if_id:0x%X rx_ep_id:0x%X", 2, *rx_if_id, *rx_ep_id);
}

void USB_Audio_Set_Chat_Info(uint8_t rx_if_id, uint8_t rx_ep_id, uint8_t tx_if_id, uint8_t tx_ep_id)
{
    if (rx_if_id != USB_AUDIO_UNUSED_ID) {
        audio1_rx_if_id = rx_if_id;
    }
    if (rx_ep_id != USB_AUDIO_UNUSED_ID) {
        audio1_rx_ep_id = rx_ep_id;
    }
    if (tx_if_id != USB_AUDIO_UNUSED_ID) {
        audio1_tx_if_id = tx_if_id;
    }
    if (tx_ep_id != USB_AUDIO_UNUSED_ID) {
        audio1_tx_ep_id = tx_ep_id;
    }
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Chat_Info rx_if_id:0x%X rx_ep_id:0x%X tx_if_id:0x%X tx_ep_id:0x%X", 4,
                audio1_rx_if_id, audio1_rx_ep_id, audio1_tx_if_id, audio1_tx_ep_id);
}

void USB_Audio_Set_Game_Info(uint8_t rx_if_id, uint8_t rx_ep_id)
{
    if (rx_if_id != USB_AUDIO_UNUSED_ID) {
        audio2_rx_if_id = rx_if_id;
    }
    if (rx_ep_id != USB_AUDIO_UNUSED_ID) {
        audio2_rx_ep_id = rx_ep_id;
    }
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Game_Info rx_if_id:0x%X rx_ep_id:0x%X", 2, audio2_rx_if_id, audio2_rx_ep_id);
}


/************************************************************
    Interface initialization functions
*************************************************************/
uint8_t USB_Audio_Get_StreamIf_Num(uint8_t port)
{
    uint8_t if_id;

    if (g_UsbAudio[port].stream_interface_id == 0xFF) {
        /* Get resource number and register to gUsbDevice */
        g_UsbAudio[port].stream_if_info = USB_Get_Interface_Number(&if_id);
        g_UsbAudio[port].stream_interface_id = if_id;
    }

    return g_UsbAudio[port].stream_interface_id;
}

/************************************************************
    USB Audio setting by APP layer functions
*************************************************************/
void USB_Aduio_Set_RX1_Alt1(uint8_t smaple_rate_num, uint8_t smaple_size, uint8_t channel, uint32_t *sample_rate)
{
    audio1_spk_stream_format_dscr.bLength          = 0x08 + 3 * smaple_rate_num;
    audio1_spk_stream_format_dscr.bSamFreqType     = smaple_rate_num;
    audio1_spk_stream_format_dscr.bSubFrameSize    = smaple_size;
    audio1_spk_stream_format_dscr.bBitResolution   = smaple_size * 8;
    audio1_spk_stream_format_dscr.bNrChannels      = channel;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt1 smaple_rate_num:%d smaple_size:%d channel:%d", 3,
                smaple_rate_num, smaple_size, channel);

    uint8_t i;
    for(i=0; i<USB_AUDIO_DSCR_MAX_FREQ_NUM; i++) {
        audio1_spk_stream_format_dscr.tSamFreq[i].data[0] = (uint8_t)(sample_rate[i]);
        audio1_spk_stream_format_dscr.tSamFreq[i].data[1] = (uint8_t)(sample_rate[i] >> 8);
        audio1_spk_stream_format_dscr.tSamFreq[i].data[2] = (uint8_t)(sample_rate[i] >> 16);
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt1 smaple_rate: %d %d %d %d %d", 5,
                audio1_spk_stream_format_dscr.tSamFreq[0],
                audio1_spk_stream_format_dscr.tSamFreq[1],
                audio1_spk_stream_format_dscr.tSamFreq[2],
                audio1_spk_stream_format_dscr.tSamFreq[3],
                audio1_spk_stream_format_dscr.tSamFreq[4]);
}

void USB_Aduio_Set_RX1_Alt2(bool alt2_en, uint8_t smaple_rate_num, uint8_t smaple_size, uint8_t channel, uint32_t *sample_rate)
{
#ifdef USB_AUDIO_RX1_ALT2_ENABLE
    g_UsbAudio[0].spk_alt2_en = alt2_en;

    if(alt2_en == false){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt2 alt2_en is disable", 0);
        return;
    }

    audio1_spk_stream_alt2_format_dscr.bLength          = 0x08 + 3 * smaple_rate_num;
    audio1_spk_stream_alt2_format_dscr.bSamFreqType     = smaple_rate_num;
    audio1_spk_stream_alt2_format_dscr.bSubFrameSize    = smaple_size;
    audio1_spk_stream_alt2_format_dscr.bBitResolution   = smaple_size * 8;
    audio1_spk_stream_alt2_format_dscr.bNrChannels      = channel;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt2 smaple_rate_num:%d smaple_size:%d channel:%d", 3,
                smaple_rate_num, smaple_size, channel);

    uint8_t i;
    for(i=0; i<USB_AUDIO_DSCR_MAX_FREQ_NUM; i++) {
        audio1_spk_stream_alt2_format_dscr.tSamFreq[i].data[0] = (uint8_t)(sample_rate[i]);
        audio1_spk_stream_alt2_format_dscr.tSamFreq[i].data[1] = (uint8_t)(sample_rate[i] >> 8);
        audio1_spk_stream_alt2_format_dscr.tSamFreq[i].data[2] = (uint8_t)(sample_rate[i] >> 16);
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt2 smaple_rate: %d %d %d %d %d", 5,
                audio1_spk_stream_alt2_format_dscr.tSamFreq[0],
                audio1_spk_stream_alt2_format_dscr.tSamFreq[1],
                audio1_spk_stream_alt2_format_dscr.tSamFreq[2],
                audio1_spk_stream_alt2_format_dscr.tSamFreq[3],
                audio1_spk_stream_alt2_format_dscr.tSamFreq[4]);
#else
    LOG_MSGID_W(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt2 USB_AUDIO_RX1_ALT2_ENABLE is not enable", 0);
#endif
}

void USB_Aduio_Set_RX1_Alt3(bool alt3_en, uint8_t smaple_rate_num, uint8_t smaple_size, uint8_t channel, uint32_t *sample_rate)
{
#ifdef USB_AUDIO_RX1_ALT3_ENABLE
    g_UsbAudio[0].spk_alt3_en = alt3_en;

    if(alt3_en == false) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt3 alt3_en is disable", 0);
        return;
    }

    audio1_spk_stream_alt3_format_dscr.bLength          = 0x08 + 3 * smaple_rate_num;
    audio1_spk_stream_alt3_format_dscr.bSamFreqType     = smaple_rate_num;
    audio1_spk_stream_alt3_format_dscr.bSubFrameSize    = smaple_size;
    audio1_spk_stream_alt3_format_dscr.bBitResolution   = smaple_size * 8;
    audio1_spk_stream_alt3_format_dscr.bNrChannels      = channel;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt3 smaple_rate_num:%d smaple_size:%d channel:%d", 3,
                smaple_rate_num, smaple_size, channel);

    uint8_t i;
    for(i=0; i<USB_AUDIO_DSCR_MAX_FREQ_NUM; i++) {
        audio1_spk_stream_alt3_format_dscr.tSamFreq[i].data[0] = (uint8_t)(sample_rate[i]);
        audio1_spk_stream_alt3_format_dscr.tSamFreq[i].data[1] = (uint8_t)(sample_rate[i] >> 8);
        audio1_spk_stream_alt3_format_dscr.tSamFreq[i].data[2] = (uint8_t)(sample_rate[i] >> 16);
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt3 smaple_rate: %d %d %d %d %d", 5,
                audio1_spk_stream_alt3_format_dscr.tSamFreq[0],
                audio1_spk_stream_alt3_format_dscr.tSamFreq[1],
                audio1_spk_stream_alt3_format_dscr.tSamFreq[2],
                audio1_spk_stream_alt3_format_dscr.tSamFreq[3],
                audio1_spk_stream_alt3_format_dscr.tSamFreq[4]);
#else
    LOG_MSGID_W(USBAUDIO_DRV, "USB_Aduio_Set_RX1_Alt3 USB_AUDIO_RX1_ALT3_ENABLE is not enable", 0);
#endif
}


void USB_Aduio_Set_RX2_Alt1(uint8_t smaple_rate_num, uint8_t smaple_size, uint8_t channel, uint32_t *sample_rate)
{
    audio2_spk_stream_format_dscr.bLength          = 0x08 + 3 * smaple_rate_num;
    audio2_spk_stream_format_dscr.bSamFreqType     = smaple_rate_num;
    audio2_spk_stream_format_dscr.bSubFrameSize    = smaple_size;
    audio2_spk_stream_format_dscr.bBitResolution   = smaple_size * 8;
    audio2_spk_stream_format_dscr.bNrChannels      = channel;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX2_Alt1 smaple_rate_num:%d smaple_size:%d channel:%d ", 3,
                smaple_rate_num, smaple_size, channel);

    uint8_t i;
    for(i=0; i<USB_AUDIO_DSCR_MAX_FREQ_NUM; i++) {
        audio2_spk_stream_format_dscr.tSamFreq[i].data[0] = (uint8_t)(sample_rate[i]);
        audio2_spk_stream_format_dscr.tSamFreq[i].data[1] = (uint8_t)(sample_rate[i] >> 8);
        audio2_spk_stream_format_dscr.tSamFreq[i].data[2] = (uint8_t)(sample_rate[i] >> 16);
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX2_Alt1 smaple_rate: %d %d %d %d %d", 5,
                audio2_spk_stream_format_dscr.tSamFreq[0],
                audio2_spk_stream_format_dscr.tSamFreq[1],
                audio2_spk_stream_format_dscr.tSamFreq[2],
                audio2_spk_stream_format_dscr.tSamFreq[3],
                audio2_spk_stream_format_dscr.tSamFreq[4]);
}

void USB_Aduio_Set_RX2_Alt2(bool alt2_en, uint8_t smaple_rate_num, uint8_t smaple_size, uint8_t channel, uint32_t *sample_rate)
{
#ifdef USB_AUDIO_RX2_ALT2_ENABLE
    g_UsbAudio[1].spk_alt2_en = alt2_en;

    if(alt2_en == false){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX2_Alt2 alt2_en is disable", 0);
        return;
    }

    audio2_spk_stream_alt2_format_dscr.bLength          = 0x08 + 3 * smaple_rate_num;
    audio2_spk_stream_alt2_format_dscr.bSamFreqType     = smaple_rate_num;
    audio2_spk_stream_alt2_format_dscr.bSubFrameSize    = smaple_size;
    audio2_spk_stream_alt2_format_dscr.bBitResolution   = smaple_size * 8;
    audio2_spk_stream_alt2_format_dscr.bNrChannels      = channel;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX2_Alt2 smaple_rate_num:%d smaple_size:%d channel:%d ", 3,
                smaple_rate_num, smaple_size, channel);

    uint8_t i;
    for(i=0; i<USB_AUDIO_DSCR_MAX_FREQ_NUM; i++) {
        audio2_spk_stream_alt2_format_dscr.tSamFreq[i].data[0] = (uint8_t)(sample_rate[i]);
        audio2_spk_stream_alt2_format_dscr.tSamFreq[i].data[1] = (uint8_t)(sample_rate[i] >> 8);
        audio2_spk_stream_alt2_format_dscr.tSamFreq[i].data[2] = (uint8_t)(sample_rate[i] >> 16);
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_RX2_Alt2 smaple_rate: %d %d %d %d %d", 5,
                audio2_spk_stream_alt2_format_dscr.tSamFreq[0],
                audio2_spk_stream_alt2_format_dscr.tSamFreq[1],
                audio2_spk_stream_alt2_format_dscr.tSamFreq[2],
                audio2_spk_stream_alt2_format_dscr.tSamFreq[3],
                audio2_spk_stream_alt2_format_dscr.tSamFreq[4]);
    return;
#else
    LOG_MSGID_W(USBAUDIO_DRV, "USB_Aduio_Set_RX2_Alt2 USB_AUDIO_RX2_ALT2_ENABLE is not enable", 0);
#endif
}


void USB_Aduio_Set_TX1_Alt1(uint8_t smaple_rate_num, uint8_t smaple_size, uint8_t channel, uint32_t *sample_rate)
{
#if defined(AIR_USB_AUDIO_1_MIC_ENABLE)
    audio1_mic_stream_format_dscr.bLength          = 0x08 + 3 * smaple_rate_num;
    audio1_mic_stream_format_dscr.bSamFreqType     = smaple_rate_num;
    audio1_mic_stream_format_dscr.bSubFrameSize    = smaple_size;
    audio1_mic_stream_format_dscr.bBitResolution   = smaple_size * 8;
    audio1_mic_stream_format_dscr.bNrChannels      = channel;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_TX1_Alt1 smaple_rate_num:%d smaple_size:%d channel:%d", 3,
                smaple_rate_num, smaple_size, channel);

    uint8_t i;
    for(i=0; i<USB_AUDIO_DSCR_MAX_FREQ_NUM; i++) {
        audio1_mic_stream_format_dscr.tSamFreq[i].data[0] = (uint8_t)(sample_rate[i]);
        audio1_mic_stream_format_dscr.tSamFreq[i].data[1] = (uint8_t)(sample_rate[i] >> 8);
        audio1_mic_stream_format_dscr.tSamFreq[i].data[2] = (uint8_t)(sample_rate[i] >> 16);
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_TX1_Alt1 smaple_rate: %d %d %d %d %d", 5,
                audio1_mic_stream_format_dscr.tSamFreq[0],
                audio1_mic_stream_format_dscr.tSamFreq[1],
                audio1_mic_stream_format_dscr.tSamFreq[2],
                audio1_mic_stream_format_dscr.tSamFreq[3],
                audio1_mic_stream_format_dscr.tSamFreq[4]);
#else
    LOG_MSGID_W(USBAUDIO_DRV, "USB_Aduio_Set_TX1_Alt1 AIR_USB_AUDIO_1_MIC_ENABLE is not enable", 0);
#endif
}

void USB_Aduio_Set_TX1_Alt2(bool alt2_en, uint8_t smaple_rate_num, uint8_t smaple_size, uint8_t channel, uint32_t *sample_rate)
{
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
#ifdef USB_AUDIO_TX1_ALT2_ENABLE
    g_UsbAudio[0].mic_alt2_en = alt2_en;

    if(alt2_en == false){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_TX1_Alt2 alt2_en is disable", 0);
        return;
    }

    audio1_mic_stream_alt2_format_dscr.bLength          = 0x08 + 3 * smaple_rate_num;
    audio1_mic_stream_alt2_format_dscr.bSamFreqType     = smaple_rate_num;
    audio1_mic_stream_alt2_format_dscr.bSubFrameSize    = smaple_size;
    audio1_mic_stream_alt2_format_dscr.bBitResolution   = smaple_size * 8;
    audio1_mic_stream_alt2_format_dscr.bNrChannels      = channel;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_TX1_Alt2 smaple_rate_num:%d smaple_size:%d channel:%d", 3,
                smaple_rate_num, smaple_size, channel);

    uint8_t i;
    for(i=0; i<USB_AUDIO_DSCR_MAX_FREQ_NUM; i++) {
        audio1_mic_stream_alt2_format_dscr.tSamFreq[i].data[0] = (uint8_t)(sample_rate[i]);
        audio1_mic_stream_alt2_format_dscr.tSamFreq[i].data[1] = (uint8_t)(sample_rate[i] >> 8);
        audio1_mic_stream_alt2_format_dscr.tSamFreq[i].data[2] = (uint8_t)(sample_rate[i] >> 16);
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Aduio_Set_TX1_Alt2 smaple_rate: %d %d %d %d %d", 5,
                audio1_mic_stream_alt2_format_dscr.tSamFreq[0],
                audio1_mic_stream_alt2_format_dscr.tSamFreq[1],
                audio1_mic_stream_alt2_format_dscr.tSamFreq[2],
                audio1_mic_stream_alt2_format_dscr.tSamFreq[3],
                audio1_mic_stream_alt2_format_dscr.tSamFreq[4]);
#else
    LOG_MSGID_W(USBAUDIO_DRV, "USB_Aduio_Set_TX1_Alt2 AIR_USB_AUDIO_1_MIC_ENABLE is not enable", 0);
#endif
#endif
}


void usb_audio_set_audio_card(uint32_t port, bool spk, bool mic)
{
    g_UsbAudio[port].spk_feature_en = spk;
    g_UsbAudio[port].mic_feature_en = mic;
}

void usb_audio_set_terminal_type(
    uint32_t port,
    usb_audio_termt_t spk_terminal,
    usb_audio_termt_t mic_terminal)
{
    LOG_MSGID_I(USBAUDIO_DRV, "usb_audio_set_terminal_type port:%d spk:0x%04X mic:0x%04X", 3, port, spk_terminal, mic_terminal);

    g_UsbAudio[port].spk_terminal_type = spk_terminal;
    g_UsbAudio[port].mic_terminal_type = mic_terminal;
}

/************************************************************
    Interface initialization functions
*************************************************************/
/* Audio control interface create function, prepare descriptor */
void USB_Audio_ControlIf_Create(uint32_t port, void *ifname)
{
}

void USB_Audio1_ControlIf_Create(void *ifname)
{
    bool spk_en;
    bool mic_en;
    uint8_t ac_if_id;
    uint8_t spk_if_id;
    uint8_t spk_ep_id;
    uint8_t mic_if_id;
    uint8_t mic_ep_id;
    int16_t ach_length = 0;
    int16_t len        = 0;
    uint8_t *dscr_out;

    spk_en = g_UsbAudio[0].spk_feature_en;
    mic_en = g_UsbAudio[0].mic_feature_en;

    g_UsbAudio[0].control_if_info      = USB_Get_Interface_Number(&ac_if_id);
    g_UsbAudio[0].control_interface_id = ac_if_id;

    if (spk_en) {
        g_UsbAudio[0].stream_if_info      = USB_Get_Interface_Number(&spk_if_id);
        g_UsbAudio[0].stream_interface_id = spk_if_id;
        g_UsbAudio[0].stream_ep_out_info  = USB_Get_Iso_Rx_Ep(&spk_ep_id);
        g_UsbAudio[0].stream_ep_out_id    = spk_ep_id;
    }

    if (mic_en) {
        g_UsbAudio[0].stream_microphone_if_info      = USB_Get_Interface_Number(&mic_if_id);
        g_UsbAudio[0].stream_microphone_interface_id = mic_if_id;
        g_UsbAudio[0].stream_ep_in_info              = USB_Get_Iso_Tx_Ep(&mic_ep_id);
        g_UsbAudio[0].stream_ep_in_id                = mic_ep_id;
    }

    audio1_interface_dscr.bInterfaceNumber = g_UsbAudio[0].control_interface_id;
    audio1_interface_dscr.iInterface       = USB_String_Get_Id_By_Usage(USB_STRING_USAGE_AUDIO1);
    audio1_spk_ot_dscr.wTerminalType       = g_UsbAudio[0].spk_terminal_type;
    audio1_mic_it_dscr.wTerminalType       = g_UsbAudio[0].mic_terminal_type;

    audio1_ach_dscr.bInCollection = 0;
    if (spk_en) {
        audio1_ach_dscr.baInterfaceNr[0] = g_UsbAudio[0].stream_interface_id;
        audio1_ach_dscr.bInCollection++;
    }
    if (mic_en) {
        audio1_ach_dscr.baInterfaceNr[1] = g_UsbAudio[0].stream_microphone_interface_id;
        audio1_ach_dscr.bInCollection++;
    }
    audio1_ach_dscr.bLength = 8 + audio1_ach_dscr.bInCollection;

    /* calculate total length of AC Header */
    ach_length += audio1_ach_dscr.bLength;
    if (spk_en) {
        for (int i = 0; i < sizeof(audio1_spk_dscrs) / sizeof(usb_audio_dscr_hdlr_t); i++) {
            /* offset 0 is always length of dscr */
            ach_length += ((uint8_t *)(audio1_spk_dscrs[i].dscr))[0];
        }
    }
    if (mic_en) {
        for (int i = 0; i < sizeof(audio1_mic_dscrs) / sizeof(usb_audio_dscr_hdlr_t); i++) {
            /* offset 0 is always length of dscr */
            ach_length += ((uint8_t *)(audio1_mic_dscrs[i].dscr))[0];
        }
    }
    audio1_ach_dscr.wTotalLength = ach_length;

    /* write dscr to usb interface struct */
    dscr_out = g_UsbAudio[0].control_if_info->ifdscr.classif;
    len += dscr_list_serialize(audio1_aci_dscrs, sizeof(audio1_aci_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out + len, 1024);
    if (spk_en) {
        len += dscr_list_serialize(audio1_spk_dscrs, sizeof(audio1_spk_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out + len, 1024);
    }
    if (mic_en) {
        len += dscr_list_serialize(audio1_mic_dscrs, sizeof(audio1_mic_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out + len, 1024);
    }
    g_UsbAudio[0].control_if_info->ifdscr_size = len;

    /* check if length if correct. If not, assert! */
    if (len != audio1_interface_dscr.bLength + audio1_ach_dscr.wTotalLength) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_ControlIf_Create the length of descriptor is error:%d != %d, assert!",
                    2, len, audio1_interface_dscr.bLength + audio1_ach_dscr.wTotalLength);
        assert(0);
    }

    /* register interface callback */
    g_UsbAudio[0].control_if_info->if_class_specific_hdlr = USB_Audio1_Ep0_Command;
}

void USB_Audio2_ControlIf_Create(void *ifname)
{
    bool spk_en;
    bool mic_en;
    uint8_t ac_if_id;
    uint8_t spk_if_id;
    uint8_t spk_ep_id;
    uint8_t mic_if_id;
    uint8_t mic_ep_id;
    int16_t ach_length = 0;
    int16_t len        = 0;
    void *dscr_out;

    spk_en = g_UsbAudio[1].spk_feature_en;
    mic_en = g_UsbAudio[1].mic_feature_en;

    g_UsbAudio[1].control_if_info      = USB_Get_Interface_Number(&ac_if_id);
    g_UsbAudio[1].control_interface_id = ac_if_id;

    if (spk_en) {
        g_UsbAudio[1].stream_if_info      = USB_Get_Interface_Number(&spk_if_id);
        g_UsbAudio[1].stream_interface_id = spk_if_id;
        g_UsbAudio[1].stream_ep_out_info  = USB_Get_Iso_Rx_Ep(&spk_ep_id);
        g_UsbAudio[1].stream_ep_out_id    = spk_ep_id;
    }

    if (mic_en) {
        g_UsbAudio[1].stream_microphone_if_info      = USB_Get_Interface_Number(&mic_if_id);
        g_UsbAudio[1].stream_microphone_interface_id = mic_if_id;
        g_UsbAudio[1].stream_ep_in_info              = USB_Get_Iso_Tx_Ep(&mic_ep_id);
        g_UsbAudio[1].stream_ep_in_id                = mic_ep_id;
    }

    audio2_interface_dscr.bInterfaceNumber = g_UsbAudio[1].control_interface_id;
    audio2_interface_dscr.iInterface       = USB_String_Get_Id_By_Usage(USB_STRING_USAGE_AUDIO2);
    audio2_spk_ot_dscr.wTerminalType       = g_UsbAudio[1].spk_terminal_type;

    audio2_ach_dscr.bInCollection = 0;
    if (spk_en) {
        audio2_ach_dscr.baInterfaceNr[0] = g_UsbAudio[1].stream_interface_id;
        audio2_ach_dscr.bInCollection++;
    }
    if (mic_en) {
        audio2_ach_dscr.baInterfaceNr[1] = g_UsbAudio[1].stream_microphone_interface_id;
        audio2_ach_dscr.bInCollection++;
    }
    audio2_ach_dscr.bLength = 8 + audio2_ach_dscr.bInCollection;

    /* calculate total length of audio2 AC Header */
    ach_length += audio2_ach_dscr.bLength;
    if (spk_en) {
        for (int i = 0; i < sizeof(audio2_spk_dscrs) / sizeof(usb_audio_dscr_hdlr_t); i++) {
            /* offset 0 is always length of dscr */
            ach_length += ((uint8_t *)(audio2_spk_dscrs[i].dscr))[0];
        }
    }
    if (mic_en) {
        for (int i = 0; i < sizeof(audio2_mic_dscrs) / sizeof(usb_audio_dscr_hdlr_t); i++) {
            /* offset 0 is always length of dscr */
            ach_length += ((uint8_t *)(audio2_mic_dscrs[i].dscr))[0];
        }
    }
    audio2_ach_dscr.wTotalLength = ach_length;

    /* write dscr to usb interface struct */
    dscr_out = g_UsbAudio[1].control_if_info->ifdscr.classif;
    len += dscr_list_serialize(audio2_aci_dscrs, sizeof(audio2_aci_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out + len, 1024);
    if (spk_en) {
        len += dscr_list_serialize(audio2_spk_dscrs, sizeof(audio2_spk_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out + len, 1024);
    }
    if (mic_en) {
        len += dscr_list_serialize(audio2_mic_dscrs, sizeof(audio2_mic_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out + len, 1024);
    }
    g_UsbAudio[1].control_if_info->ifdscr_size = len;

    /* check if length if correct. If not, assert! */
    if (len != audio2_interface_dscr.bLength + audio2_ach_dscr.wTotalLength) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio2_ControlIf_Create the length of descriptor is error:%d != %d, assert!",
                    2, len, audio2_interface_dscr.bLength + audio2_ach_dscr.wTotalLength);
        assert(0);
    }

    /* register interface callback */
    g_UsbAudio[1].control_if_info->if_class_specific_hdlr = USB_Audio2_Ep0_Command;
}

void USB_Audio_ControlIf_Reset(uint32_t port)
{
}

void USB_Audio1_ControlIf_Reset(void)
{
}

void USB_Audio2_ControlIf_Reset(void)
{
}

void USB_Audio_ControlIf_Enable(uint32_t port)
{
}

void USB_Audio1_ControlIf_Enable(void)
{
}
void USB_Audio2_ControlIf_Enable(void)
{
}

/* Audio stream interface speed reset function, enable EP's speed-specific descriptor */
void USB_Audio_ControlIf_Speed_Reset(uint32_t port, bool b_other_speed)
{
}

void USB_Audio1_ControlIf_Speed_Reset(bool b_other_speed)
{
}

void USB_Audio2_ControlIf_Speed_Reset(bool b_other_speed)
{
}

/* Audio stream interface create function, prepare descriptor */
void USB_Audio_StreamIf_Create(uint32_t port, void *ifname)
{
}

void USB_Audio1_StreamIf_Create(void *ifname)
{
    uint8_t if_id, ep_rx_id;
    uint8_t port = 0;
    uint8_t *dscr_out;
    int16_t len;

    if(g_UsbAudio[port].spk_feature_en == false) {
        return;
    }

    if_id = g_UsbAudio[port].stream_interface_id;
    ep_rx_id = g_UsbAudio[port].stream_ep_out_id;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Create port:%d if_id:0x%X ep_rx_id:0x%X", 3,
                port, g_UsbAudio[port].stream_interface_id, g_UsbAudio[port].stream_ep_out_id);

    audio1_spk_stream_if_dscr.bInterfaceNumber = if_id;
    audio1_spk_stream_alt_if_dscr.bInterfaceNumber = if_id;
    audio1_spk_stream_general_dscr.bTerminalLink = audio1_spk_it_dscr.bTerminalID;

    /* generate audio stream interface */
    g_UsbAudio[port].stream_if_info->interface_name_ptr = (char *)ifname;

    dscr_out = g_UsbAudio[port].stream_if_info->ifdscr.classif;
    len = dscr_list_serialize(audio1_spk_as_if_dscrs, sizeof(audio1_spk_as_if_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
    g_UsbAudio[port].stream_if_info->ifdscr_size = len;

    /* generate audio stream alternate interface */
    dscr_out = g_UsbAudio[port].stream_if_info->alternate_if_info[0].ifdscr.classif;
    len      = dscr_list_serialize(audio1_spk_as_if_alt_dscrs, sizeof(audio1_spk_as_if_alt_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
    g_UsbAudio[port].stream_if_info->alternate_if_info[0].ifdscr_size = len;

    /* generate ep of alternate interface */
    g_UsbAudio[port].stream_if_info->alternate_if_info[0].ep_info[0] = g_UsbAudio[port].stream_ep_out_info;
    g_UsbAudio[port].stream_if_info->alternate_if_info[0].ep_info[0]->epdscr_size = sizeof(audio_stream_ep_out_dscr);

#ifdef USB_AUDIO_RX1_ALT2_ENABLE
    if(g_UsbAudio[port].spk_alt2_en) {
        audio1_spk_stream_alt2_if_dscr.bInterfaceNumber = if_id;

        /* generate audio stream alternate interface */
        dscr_out = g_UsbAudio[port].stream_if_info->alternate_if_info[1].ifdscr.classif;
        len      = dscr_list_serialize(audio1_spk_as_if_alt2_dscrs, sizeof(audio1_spk_as_if_alt2_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
        g_UsbAudio[port].stream_if_info->alternate_if_info[1].ifdscr_size = len;

        /* generate ep of alternate interface */
        g_UsbAudio[port].stream_if_info->alternate_if_info[1].ep_info[0] = g_UsbAudio[port].stream_ep_out_info;
        g_UsbAudio[port].stream_if_info->alternate_if_info[1].ep_info[0]->epdscr_size = sizeof(audio_stream_ep_out_dscr);
    }
#endif

#ifdef USB_AUDIO_RX1_ALT3_ENABLE
    if(g_UsbAudio[port].spk_alt3_en) {
        audio1_spk_stream_alt3_if_dscr.bInterfaceNumber = if_id;

        /* generate audio stream alternate interface */
        dscr_out = g_UsbAudio[port].stream_if_info->alternate_if_info[2].ifdscr.classif;
        len      = dscr_list_serialize(audio1_spk_as_if_alt3_dscrs, sizeof(audio1_spk_as_if_alt3_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
        g_UsbAudio[port].stream_if_info->alternate_if_info[2].ifdscr_size = len;

        /* generate ep of alternate interface */
        g_UsbAudio[port].stream_if_info->alternate_if_info[2].ep_info[0] = g_UsbAudio[port].stream_ep_out_info;
        g_UsbAudio[port].stream_if_info->alternate_if_info[2].ep_info[0]->epdscr_size = sizeof(audio_stream_ep_out_dscr);
    }
#endif

    memcpy((uint32_t *) & (g_UsbAudio[port].stream_ep_out_info->epdesc.classep), audio_stream_ep_out_dscr, sizeof(audio_stream_ep_out_dscr));

    g_UsbAudio[port].stream_if_info->if_class_specific_hdlr = USB_Audio1_Ep0_Command; /* Command: "SetCur" for "Sampling freq".*/

    /* RX Endpoint handler */
    g_UsbAudio[port].stream_ep_out_info->ep_reset = USB_Audio1_IsoOut_Reset;
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_RX, ep_rx_id, USB_Audio1_IsoOut_Hdr);

    /* Endpoint descriptor */
    g_UsbAudio[port].stream_ep_out_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_OUT | ep_rx_id); /*OutPipe*/
    g_UsbAudio[port].stream_ep_out_info->ep_status.epout_status.byEP = ep_rx_id;

    /* Ep_id include direction */
    USB_Audio_Set_Chat_Info(if_id, (USB_EP_DIR_OUT | ep_rx_id), USB_AUDIO_UNUSED_ID, USB_AUDIO_UNUSED_ID);

    if (g_UsbAudio[port].rx_dma_buffer == NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Create port:%d rx buffer size:%d", 2, port, USB_AUDIO_RX_BUFFER_SIZE);
        g_UsbAudio[port].rx_dma_buffer = (uint8_t *)USB_Get_Memory(USB_AUDIO_RX_BUFFER_SIZE);
        g_UsbAudio[port].rx_dma_buffer_len = USB_AUDIO_RX_BUFFER_SIZE;
    }

    hal_usb_get_dma_channel(0, ep_rx_id, HAL_USB_EP_DIRECTION_RX, false);
}

void USB_Audio2_StreamIf_Create(void *ifname)
{
    uint8_t if_id, ep_rx_id;
    uint8_t port = 1;
    uint8_t *dscr_out;
    int16_t len;

    if(g_UsbAudio[port].spk_feature_en == false) {
        return;
    }

    if_id = g_UsbAudio[port].stream_interface_id;
    ep_rx_id = g_UsbAudio[port].stream_ep_out_id;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio2_StreamIf_Create port:%d if_id:0x%X ep_rx_id:0x%X", 3,
                port, g_UsbAudio[port].stream_interface_id, g_UsbAudio[port].stream_ep_out_id);

    audio2_spk_stream_if_dscr.bInterfaceNumber = if_id;
    audio2_spk_stream_alt_if_dscr.bInterfaceNumber = if_id;
    audio2_spk_stream_general_dscr.bTerminalLink = audio2_spk_it_dscr.bTerminalID;

    /* generate audio stream interface */
    g_UsbAudio[port].stream_if_info->interface_name_ptr = (char *)ifname;

    dscr_out = g_UsbAudio[port].stream_if_info->ifdscr.classif;
    len = dscr_list_serialize(audio2_spk_as_if_dscrs, sizeof(audio2_spk_as_if_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
    g_UsbAudio[port].stream_if_info->ifdscr_size = len;

    /* generate audio stream alternate interface */
    dscr_out = g_UsbAudio[port].stream_if_info->alternate_if_info[0].ifdscr.classif;
    len      = dscr_list_serialize(audio2_spk_as_if_alt_dscrs, sizeof(audio2_spk_as_if_alt_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
    g_UsbAudio[port].stream_if_info->alternate_if_info[0].ifdscr_size = len;

    /* generate ep of alternate interface */
    g_UsbAudio[port].stream_if_info->alternate_if_info[0].ep_info[0] = g_UsbAudio[port].stream_ep_out_info;
    g_UsbAudio[port].stream_if_info->alternate_if_info[0].ep_info[0]->epdscr_size = sizeof(audio_stream_ep_out_dscr);

#ifdef USB_AUDIO_RX2_ALT2_ENABLE
    if(g_UsbAudio[port].spk_alt2_en) {
        audio2_spk_stream_alt2_if_dscr.bInterfaceNumber = if_id;

        /* generate audio stream alternate interface */
        dscr_out = g_UsbAudio[port].stream_if_info->alternate_if_info[1].ifdscr.classif;
        len      = dscr_list_serialize(audio2_spk_as_if_alt2_dscrs, sizeof(audio2_spk_as_if_alt2_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
        g_UsbAudio[port].stream_if_info->alternate_if_info[1].ifdscr_size = len;

        /* generate ep of alternate interface */
        g_UsbAudio[port].stream_if_info->alternate_if_info[1].ep_info[0] = g_UsbAudio[port].stream_ep_out_info;
        g_UsbAudio[port].stream_if_info->alternate_if_info[1].ep_info[0]->epdscr_size = sizeof(audio_stream_ep_out_dscr);
    }
#endif

    memcpy((uint32_t *) & (g_UsbAudio[port].stream_ep_out_info->epdesc.classep), audio_stream_ep_out_dscr, sizeof(audio_stream_ep_out_dscr));

    g_UsbAudio[port].stream_if_info->if_class_specific_hdlr = USB_Audio2_Ep0_Command; /* Command: "SetCur" for "Sampling freq".*/

    /* RX Endpoint handler */
    g_UsbAudio[port].stream_ep_out_info->ep_reset = USB_Audio2_IsoOut_Reset;
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_RX, ep_rx_id, USB_Audio2_IsoOut_Hdr);

    /* Endpoint descriptor */
    g_UsbAudio[port].stream_ep_out_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_OUT | ep_rx_id); /*OutPipe*/
    g_UsbAudio[port].stream_ep_out_info->ep_status.epout_status.byEP = ep_rx_id;

    /* Ep_id include direction */
    USB_Audio_Set_Game_Info(if_id, (USB_EP_DIR_OUT | ep_rx_id));

    if (g_UsbAudio[port].rx_dma_buffer == NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio2_StreamIf_Create port:%d rx buffer size:%d", 2, port, USB_AUDIO_RX_BUFFER_SIZE);
        g_UsbAudio[port].rx_dma_buffer = (uint8_t *)USB_Get_Memory(USB_AUDIO_RX_BUFFER_SIZE);
        g_UsbAudio[port].rx_dma_buffer_len = USB_AUDIO_RX_BUFFER_SIZE;
    }

    hal_usb_get_dma_channel(0, ep_rx_id, HAL_USB_EP_DIRECTION_RX, false);
}

/* Audio stream interface reset function, enable EP */
void USB_Audio_StreamIf_Reset(uint32_t port)
{
    if(g_UsbAudio[port].spk_feature_en == false) {
        return;
    }
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Reset port:%d", 1, port);

    g_UsbAudio[port].rxpipe = &g_UsbAudio[port].stream_ep_out_info->ep_status.epout_status;

    /* Stop DMA */
    hal_usb_stop_dma_channel(g_UsbAudio[port].rxpipe->byEP, HAL_USB_EP_DIRECTION_RX);
}

void USB_Audio1_StreamIf_Reset(void)
{
    USB_Audio_StreamIf_Reset(0);
}
void USB_Audio2_StreamIf_Reset(void)
{
    USB_Audio_StreamIf_Reset(1);
}

void USB_Audio_StreamIf_Enable(uint32_t port)
{
    if(g_UsbAudio[port].spk_feature_en == false) {
        return;
    }
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Enable port:%d", 1, port);

    /* Stop DMA */
    /* Compliance test tool will set configuration , but no reset */
    hal_usb_stop_dma_channel(g_UsbAudio[port].rxpipe->byEP, HAL_USB_EP_DIRECTION_RX);

    /*Non-DMA*/
    hal_usb_enable_rx_endpoint(g_UsbAudio[port].rxpipe->byEP, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, true);
}
void USB_Audio1_StreamIf_Enable(void)
{
    USB_Audio_StreamIf_Enable(0);
}

void USB_Audio2_StreamIf_Enable(void)
{
    USB_Audio_StreamIf_Enable(1);
}


/* Audio stream interface speed reset function, enable EP's speed-specific descriptor */
void USB_Audio_StreamIf_Speed_Reset(uint32_t port, bool b_other_speed)
{
    uint32_t temp_max_size;

    if(g_UsbAudio[port].spk_feature_en == false) {
        return;
    }
    temp_max_size = USB_Audio_Stream_Speed_Reset_Packet_Size(port, (bool)b_other_speed);

    g_UsbAudio[port].stream_ep_out_info->epdesc.stdep.wMaxPacketSize[0] = temp_max_size & 0xff;
    g_UsbAudio[port].stream_ep_out_info->epdesc.stdep.wMaxPacketSize[1] = (temp_max_size >> 8) & 0xff;

    if (hal_usb_is_high_speed() == true) {
        g_UsbAudio[port].stream_ep_out_info->epdesc.stdep.bInterval = 4; /* (2^(4-1))*0.125us=1ms*/
    } else {
        g_UsbAudio[port].stream_ep_out_info->epdesc.stdep.bInterval = 1; /* (2^(1-1))*1ms=1ms*/
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Speed_Reset port:%d wMaxPacketSize:%d b_other_speed=%d", 3, port, temp_max_size, b_other_speed);
}

void USB_Audio1_StreamIf_Speed_Reset(bool b_other_speed)
{
    USB_Audio_StreamIf_Speed_Reset(0, b_other_speed);
}

void USB_Audio2_StreamIf_Speed_Reset(bool b_other_speed)
{
    USB_Audio_StreamIf_Speed_Reset(1, b_other_speed);
}

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
/* Audio stream interface create function, prepare descriptor */
void USB_Audio_StreamIf_Microphone_Create(void *ifname)
{
    uint8_t if_id, ep_tx_id;
    uint8_t port = 0;
    uint8_t *dscr_out;
    int16_t len;

    if(g_UsbAudio[port].mic_feature_en == false) {
        return;
    }

    if_id = g_UsbAudio[port].stream_microphone_interface_id;
    ep_tx_id = g_UsbAudio[port].stream_ep_in_id;

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Microphone_Create port:0x%X if_id:0x%X ep_tx_id:0x%X", 3,
                port, g_UsbAudio[port].stream_microphone_interface_id, g_UsbAudio[port].stream_ep_in_id);

    audio1_mic_stream_if_dscr.bInterfaceNumber = if_id;
    audio1_mic_stream_alt_if_dscr.bInterfaceNumber = if_id;
    audio1_mic_stream_general_dscr.bTerminalLink = audio1_mic_ot_dscr.bTerminalID;

    /* generate audio stream interface */
    g_UsbAudio[port].stream_microphone_if_info->interface_name_ptr = (char *)ifname;

    dscr_out = g_UsbAudio[port].stream_microphone_if_info->ifdscr.classif;
    len = dscr_list_serialize(audio1_mic_as_if_dscrs, sizeof(audio1_mic_as_if_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
    g_UsbAudio[port].stream_microphone_if_info->ifdscr_size = len;

    /* generate audio stream alternate interface */
    dscr_out = g_UsbAudio[port].stream_microphone_if_info->alternate_if_info[0].ifdscr.classif;
    len      = dscr_list_serialize(audio1_mic_as_if_alt_dscrs, sizeof(audio1_mic_as_if_alt_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
    g_UsbAudio[port].stream_microphone_if_info->alternate_if_info[0].ifdscr_size = len;

    /* generate ep of alternate interface */
    g_UsbAudio[port].stream_microphone_if_info->alternate_if_info[0].ep_info[0] = g_UsbAudio[port].stream_ep_in_info;
    g_UsbAudio[port].stream_microphone_if_info->alternate_if_info[0].ep_info[0]->epdscr_size = sizeof(audio_stream_ep_in_dscr);

#ifdef USB_AUDIO_TX1_ALT2_ENABLE
    if(g_UsbAudio[port].mic_alt2_en) {
        audio1_mic_stream_alt2_if_dscr.bInterfaceNumber = if_id;

        /* generate audio stream alternate interface */
        dscr_out = g_UsbAudio[port].stream_microphone_if_info->alternate_if_info[1].ifdscr.classif;
        len      = dscr_list_serialize(audio1_mic_as_if_alt2_dscrs, sizeof(audio1_mic_as_if_alt2_dscrs) / sizeof(usb_audio_dscr_hdlr_t), dscr_out, USB_MAX_IFDSC_LENGTH);
        g_UsbAudio[port].stream_microphone_if_info->alternate_if_info[1].ifdscr_size = len;

        /* generate ep of alternate interface */
        g_UsbAudio[port].stream_microphone_if_info->alternate_if_info[1].ep_info[0] = g_UsbAudio[port].stream_ep_in_info;
        g_UsbAudio[port].stream_microphone_if_info->alternate_if_info[1].ep_info[0]->epdscr_size = sizeof(audio_stream_ep_in_dscr);
    }
#endif


    memcpy((uint32_t *) & (g_UsbAudio[port].stream_ep_in_info->epdesc.classep), audio_stream_ep_in_dscr, sizeof(audio_stream_ep_in_dscr));

    g_UsbAudio[port].stream_microphone_if_info->if_class_specific_hdlr = USB_Audio1_Ep0_Command; /* Command: "SetCur" for "Sampling freq".*/

    /* TX Endpoint handler */
    g_UsbAudio[port].stream_ep_in_info->ep_reset = USB_Audio_IsoIn_Reset;
    hal_usb_register_driver_callback(HAL_USB_DRV_HDLR_EP_TX, ep_tx_id, USB_Audio_IsoIn_Hdr);

    /* Endpoint descriptor */
    g_UsbAudio[port].stream_ep_in_info->epdesc.stdep.bEndpointAddress = (USB_EP_DIR_IN | ep_tx_id); /*InPipe*/
    g_UsbAudio[port].stream_ep_in_info->ep_status.epout_status.byEP = ep_tx_id;

    /* Ep_id include direction */
    USB_Audio_Set_Chat_Info(USB_AUDIO_UNUSED_ID, USB_AUDIO_UNUSED_ID, if_id, (USB_EP_DIR_IN | ep_tx_id));

    if (g_UsbAudio[port].tx_dma_buffer == NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Microphone_Create tx buffer size:%d", 1, USB_AUDIO_TX_BUFFER_SIZE);
        g_UsbAudio[port].tx_dma_buffer = (uint8_t *)USB_Get_Memory(USB_AUDIO_TX_BUFFER_SIZE);
        g_UsbAudio[port].tx_dma_buffer_len = USB_AUDIO_TX_BUFFER_SIZE;
    }

    hal_usb_get_dma_channel(ep_tx_id, 0, HAL_USB_EP_DIRECTION_TX, false);
}

/* Audio stream interface reset function, enable EP */
void USB_Audio_StreamIf_Microphone_Reset(void)
{
    uint32_t port = 0;

    if(g_UsbAudio[port].mic_feature_en == false) {
        return;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Microphone_Reset", 0);

    g_UsbAudio[port].txpipe = &g_UsbAudio[port].stream_ep_in_info->ep_status.epout_status;
    hal_usb_stop_dma_channel(g_UsbAudio[port].txpipe->byEP, HAL_USB_EP_DIRECTION_TX);
}

void USB_Audio_StreamIf_Microphone_Enable(void)
{
    uint32_t port = 0;

    if(g_UsbAudio[port].mic_feature_en == false) {
        return;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Microphone_Enable", 0);

    hal_usb_stop_dma_channel(g_UsbAudio[port].txpipe->byEP, HAL_USB_EP_DIRECTION_TX);
    hal_usb_enable_tx_endpoint(g_UsbAudio[port].txpipe->byEP, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, true);
}


/* Audio stream interface speed reset function, enable EP's speed-specific descriptor */
void USB_Audio_StreamIf_Microphone_Speed_Reset(bool b_other_speed)
{
    uint32_t port = 0;

    if(g_UsbAudio[port].mic_feature_en == false) {
        return;
    }

    uint32_t temp_max_size;
    temp_max_size = USB_Audio_Stream_Microphone_Speed_Reset_Packet_Size(port, (bool)b_other_speed);

    g_UsbAudio[port].stream_ep_in_info->epdesc.stdep.wMaxPacketSize[0] = temp_max_size & 0xff;
    g_UsbAudio[port].stream_ep_in_info->epdesc.stdep.wMaxPacketSize[1] = (temp_max_size >> 8) & 0xff;

    if (hal_usb_is_high_speed() == true) {
        g_UsbAudio[port].stream_ep_in_info->epdesc.stdep.bInterval = 4; /* (2^(4-1))*0.125us=1ms*/
    } else {
        g_UsbAudio[port].stream_ep_in_info->epdesc.stdep.bInterval = 1; /* (2^(1-1))*1ms=1ms*/
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_StreamIf_Speed_Microphone_Reset wMaxPacketSize:%d b_other_speed:%d", 2, temp_max_size, b_other_speed);
}
#endif /* AIR_USB_AUDIO_1_MIC_ENABLE*/

/*********************************************************************************************
    global variable g_UsbAudio[port] initialize and release functions
*********************************************************************************************/
/* USB Audio 1 : Speaker */
bool g_usb_audio_spk1_enable = 0;
bool g_usb_audio_spk1_status = 0;

/* USB Audio 2 : Speaker */
bool g_usb_audio_spk2_feature= 0;
bool g_usb_audio_spk2_enable = 0;
bool g_usb_audio_spk2_status = 0;

/* Get USB Audio setting from speaker or microphone */
/* USB Audio 1 : Speaker */
uint16_t USB_Audio_Get_Spk1_Cur(uint8_t ch)
{
    ch = _USB_Audio_Channel_Check(ch);
    return g_UsbAudio[0].spk_cur_channel_vol[ch];
}

uint16_t USB_Audio_Get_Spk1_Min(void)
{
    return g_UsbAudio[0].spk_min;
}

uint16_t USB_Audio_Get_Spk1_Max(void)
{
    return g_UsbAudio[0].spk_max;
}

uint16_t USB_Audio_Get_Spk1_Res(void)
{
    return g_UsbAudio[0].spk_res;
}

/* USB Audio 1 : Microphone */
uint16_t USB_Audio_Get_Mic1_Cur(uint8_t ch)
{
    ch = _USB_Audio_Channel_Check(ch);
    return g_UsbAudio[0].mic_cur_channel_vol[ch];
}

uint16_t USB_Audio_Get_Mic1_Min(void)
{
    return g_UsbAudio[0].mic_min;
}

uint16_t USB_Audio_Get_Mic1_Max(void)
{
    return g_UsbAudio[0].mic_max;
}

uint16_t USB_Audio_Get_Mic1_Res(void)
{
    return g_UsbAudio[0].mic_res;
}

/* USB Audio 2 : Speaker */
uint16_t USB_Audio_Get_Spk2_Cur(uint8_t ch)
{
    ch = _USB_Audio_Channel_Check(ch);
    return g_UsbAudio[1].spk_cur_channel_vol[ch];
}

uint16_t USB_Audio_Get_Spk2_Min(void)
{
    return g_UsbAudio[1].spk_min;
}

uint16_t USB_Audio_Get_Spk2_Max(void)
{
    return g_UsbAudio[1].spk_max;
}

uint16_t USB_Audio_Get_Spk2_Res(void)
{
    return g_UsbAudio[1].spk_res;
}

/* Set up USB Audio setting from NVkey or by default */
void USB_Audio_Setting_By_NVKey(void)
{
    nvkey_status_t nvkey_status;
    uint32_t nvkey_size;
    usb_nvkey_audio_device_cfg_t audio_device_cfg;

    /* USB Audio 1 Speaker */
    nvkey_size   = sizeof(audio_device_cfg);
    nvkey_status = nvkey_read_data(NVID_USB_SETTING_1, (uint8_t *)&audio_device_cfg, &nvkey_size);
    if (nvkey_status == NVKEY_STATUS_OK) {
        g_UsbAudio[0].spk_terminal_type = audio_device_cfg.terminal_type;
        g_UsbAudio[0].spk_cur           = audio_device_cfg.cur;
        g_UsbAudio[0].spk_min           = audio_device_cfg.min;
        g_UsbAudio[0].spk_max           = audio_device_cfg.max;
        g_UsbAudio[0].spk_res           = audio_device_cfg.res;
    }

    #ifdef AIR_DCHS_MODE_ENABLE
        g_UsbAudio[0].spk_terminal_type = USB_AUDIO_TERMT_HEADSET;
    #endif

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Setting_By_NVKey audio1 spk:0x%04X volume cur:0x%04X min:0x%04X max:0x%04X res:0x%04X nvkey_status:%d", 6,
                audio_device_cfg.terminal_type, audio_device_cfg.cur, audio_device_cfg.min, audio_device_cfg.max, audio_device_cfg.res, nvkey_status);

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    /* USB Audio 1 Microphone */
    nvkey_size   = sizeof(audio_device_cfg);
    nvkey_status = nvkey_read_data(NVID_USB_SETTING_2, (uint8_t *)&audio_device_cfg, &nvkey_size);
    if (nvkey_status == NVKEY_STATUS_OK) {
        g_UsbAudio[0].mic_terminal_type = audio_device_cfg.terminal_type;
        g_UsbAudio[0].mic_cur           = audio_device_cfg.cur;
        g_UsbAudio[0].mic_min           = audio_device_cfg.min;
        g_UsbAudio[0].mic_max           = audio_device_cfg.max;
        g_UsbAudio[0].mic_res           = audio_device_cfg.res;
    }
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Setting_By_NVKey audio1 mic:0x%04X volume cur:0x%04X min:0x%04X max:0x%04X res:0x%04X nvkey_status:%d", 6,
                audio_device_cfg.terminal_type, audio_device_cfg.cur, audio_device_cfg.min, audio_device_cfg.max, audio_device_cfg.res, nvkey_status);
#endif /* AIR_USB_AUDIO_1_MIC_ENABLE */

#ifdef AIR_USB_AUDIO_2_SPK_ENABLE
    /* USB Audio 2 : Speaker, default no use */
    g_usb_audio_spk2_feature = FALSE;
    usb_nvkey_device_cfg_t device_cfg;

    /* USB Audio 2 Speaker enable */
    nvkey_size   = sizeof(usb_nvkey_device_cfg_t);
    nvkey_status = nvkey_read_data(NVID_USB_SETTING_0, (uint8_t *)&device_cfg, &nvkey_size);
    if (nvkey_status == NVKEY_STATUS_OK) {
        g_usb_audio_spk2_feature = device_cfg.duble_audio_device;
        g_UsbAudio[1].spk_feature_en = device_cfg.duble_audio_device;
    }
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Setting_By_NVKey audio2 spk feature_en:%d nvkey_status:%d", 2, g_usb_audio_spk2_feature, nvkey_status);

    /* USB Audio 2 Speaker */
    if (g_usb_audio_spk2_feature) {
        nvkey_size   = sizeof(audio_device_cfg);
        nvkey_status = nvkey_read_data(NVID_USB_SETTING_3, (uint8_t *)&audio_device_cfg, &nvkey_size);
        if (nvkey_status == NVKEY_STATUS_OK) {
            g_UsbAudio[1].spk_terminal_type = audio_device_cfg.terminal_type;
            g_UsbAudio[1].spk_cur           = audio_device_cfg.cur;
            g_UsbAudio[1].spk_min           = audio_device_cfg.min;
            g_UsbAudio[1].spk_max           = audio_device_cfg.max;
            g_UsbAudio[1].spk_res           = audio_device_cfg.res;
        }
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Setting_By_NVKey audio2:0x%04X volume cur:0x%04X min:0x%04X max:0x%04X res:0x%04X nvkey_status:%d", 6,
                    audio_device_cfg.terminal_type, audio_device_cfg.cur, audio_device_cfg.min, audio_device_cfg.max, audio_device_cfg.res, nvkey_status);
    }
#endif /* AIR_USB_AUDIO_2_SPK_ENABLE */
}

void USB_Init_Audio_Status(void)
{
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Init_Audio_Status", 0);

    /* Initilize the Chat/Game if_id and ep_id */
    USB_Audio_Init_Chat_Game_Info();

    /* USB Audio v1 */
    USB_Audio[0].audio_version = USB_AUDIO_VERSION_V1;
    USB_Audio[1].audio_version = USB_AUDIO_VERSION_V1;

    g_UsbAudio[0].rxpipe = NULL;
    g_UsbAudio[1].rxpipe = NULL;
    g_UsbAudio[0].stream_interface_id = 0xFF;
    g_UsbAudio[1].stream_interface_id = 0xFF;
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    g_UsbAudio[0].txpipe = NULL;
    g_UsbAudio[1].txpipe = NULL;
    g_UsbAudio[0].stream_microphone_interface_id = 0xFF;
    g_UsbAudio[1].stream_microphone_interface_id = 0xFF;
#endif

    USB_Audio[0].initialized = true;
    USB_Audio[1].initialized = true;
}

/* Release global variable g_UsbAudio[port] */
void USB_Release_Audio_Status(void)
{
    LOG_MSGID_I(USBAUDIO_DRV, "USB_Release_Audio_Status", 0);

    if (g_UsbAudio[0].rxpipe != NULL) {
        hal_usb_release_dma_channel(0, g_UsbAudio[0].rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, false);
    }
    if (g_UsbAudio[1].rxpipe != NULL) {
        hal_usb_release_dma_channel(0, g_UsbAudio[1].rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, false);
    }

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    if (g_UsbAudio[0].txpipe != NULL) {
        hal_usb_release_dma_channel(g_UsbAudio[0].txpipe->byEP, 0, HAL_USB_EP_DIRECTION_TX, false);
    }
    if (g_UsbAudio[1].txpipe != NULL) {
        hal_usb_release_dma_channel(g_UsbAudio[1].txpipe->byEP, 0, HAL_USB_EP_DIRECTION_TX, false);
    }
#endif

    if (USB_Audio[0].unplug_cb) {
        USB_Send_Message(USB_AUDIO_UNPLUG, NULL);
    }
    if (USB_Audio[1].unplug_cb) {
        USB_Send_Message(USB_AUDIO_UNPLUG, NULL);
    }

    USB_Init_Audio_Status();

    if (g_UsbAudio[0].rx_dma_buffer != NULL) {
        USB_Free_Memory((void *)g_UsbAudio[0].rx_dma_buffer);
        g_UsbAudio[0].rx_dma_buffer = NULL;
    }
    if (g_UsbAudio[1].rx_dma_buffer != NULL) {
        USB_Free_Memory((void *)g_UsbAudio[1].rx_dma_buffer);
        g_UsbAudio[1].rx_dma_buffer = NULL;
    }
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    if (g_UsbAudio[0].tx_dma_buffer != NULL) {
        USB_Free_Memory((void *)g_UsbAudio[0].tx_dma_buffer);
        g_UsbAudio[0].tx_dma_buffer = NULL;
    }
    if (g_UsbAudio[1].tx_dma_buffer != NULL) {
        USB_Free_Memory((void *)g_UsbAudio[1].tx_dma_buffer);
        g_UsbAudio[1].tx_dma_buffer = NULL;
    }
#endif
    if (gUsbDevice.conf != NULL) {
        USB_Free_Memory((void *)gUsbDevice.conf);
        gUsbDevice.conf = NULL;
    }
}

/************************************************************
    EP0 handle functions
************************************************************/
/* Parse class specific request */
static uint8_t g_usb_audio_mute;
uint32_t g_usb_audio_sample_rate = 48000;
uint8_t g_usb_audio_sample_size = 2; /* Unit: byte */
uint8_t g_usb_audio_channel = 2;

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
static uint8_t g_usb_audio_microphone_mute;
uint32_t g_usb_audio_mic_sample_rate = 16000;
static uint32_t g_usb_audio_mic_sample_size = 1; /* Unit: byte */
static uint32_t g_usb_audio_mic_channel = 1;
#endif
static uint16_t g_usb_audio_wIndex;
static uint16_t g_usb_audio_wValue;
static uint16_t g_usb_audio_wLength;

static uint8_t g_usb_audio2_mute;
uint32_t g_usb_audio2_sample_rate = 48000;
uint8_t g_usb_audio2_sample_size = 2; /* byte */
uint8_t g_usb_audio2_channel = 2;
static uint16_t g_usb_audio2_wIndex;
static uint16_t g_usb_audio2_wValue;
static uint16_t g_usb_audio2_wLength;


int32_t USB_Audio_Volume_dB_Convertor(int16_t raw_data)
{
    int32_t dB;

    /* dB = ((int32_t) raw_data) / 256 * 10000 */
    dB = (((int32_t) raw_data) * 625) / 16;

    LOG_MSGID_I(USB_DEBUG, "USB_Audio_Volume_dB_Convertor raw_data:0x%04X dB:%d", 2, raw_data, dB);

    return dB;
}

uint16_t USB_Audio_Volume_0to100_Convertor(uint16_t raw_data, uint16_t volume_min, uint16_t volume_max)
{
    uint16_t volume, step;

    if(raw_data == 0)
        raw_data = 0xffff;
    if(volume_max == 0)
        volume_max = 0xffff;

    /* 0~100% = min ~ max */
    step = (volume_max - volume_min) / 100;
    volume = (raw_data - volume_min) / step;

    LOG_MSGID_I(USB_DEBUG, "USB_Audio_Volume_0to100_Convertor volume:%d raw_data:0x%04X volume_max:0x%04X volume_min:0x%04X step:%d", 5, volume, raw_data, volume_max, volume_min, step);

    return volume;
}

static void USB_Audio1_Ep0_DataReceived(void *data)
{
    bool stall = false;
    uint16_t wValue  = g_usb_audio_wValue;
    uint16_t wIndex  = g_usb_audio_wIndex;
    uint16_t wLength = g_usb_audio_wLength;

    uint8_t  spk_ep_addr = (USB_EP_DIR_OUT | g_UsbAudio[0].stream_ep_out_id);
    uint16_t spk_wIndex  = (0x02 << 8 | g_UsbAudio[0].control_interface_id);

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    uint8_t  mic_ep_addr = (USB_EP_DIR_IN | g_UsbAudio[0].stream_ep_in_id);
    uint16_t mic_wIndex  = (0x06 << 8 | g_UsbAudio[0].control_interface_id);
#endif

    /* Check EP0 data is enough */
    USB_Check_EP0_DataLen(0, wLength);

    if (wValue == 0x0100 && wIndex == spk_ep_addr) {
        hal_usb_read_endpoint_fifo(0, wLength, &g_usb_audio_sample_rate);
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_Ep0_DataReceived speaker1 change sample rate to %d Hz", 1, g_usb_audio_sample_rate);
        USB_Send_Message(USB_AUDIO_SET_SAMPLING_FREQ, (void *)(g_usb_audio_sample_rate + (spk_ep_addr << 24)));
    }
    else if (wValue == 0x0100 && wIndex == spk_wIndex) {
        hal_usb_read_endpoint_fifo(0, wLength, &g_usb_audio_mute);
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_Ep0_DataReceived speaker1 change mute to %d", 1, g_usb_audio_mute);
        USB_Send_Message(USB_AUDIO_SET_MUTE, (void *)(g_usb_audio_mute + (spk_ep_addr << 24)));
    }
    else if ((wValue == 0x0201 || wValue == 0x0202) && wIndex == spk_wIndex) {
        uint8_t ch_index = wValue - 0x0201;
        ch_index = _USB_Audio_Channel_Check(ch_index);
        hal_usb_read_endpoint_fifo(0, wLength, &g_UsbAudio[0].spk_cur_channel_vol[ch_index]);
        USB_Send_Message(USB_AUDIO_SET_VOLUME, (void *)(g_UsbAudio[0].spk_cur_channel_vol[ch_index] + (ch_index << 16) + (spk_ep_addr << 24)));
    }
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    else if ((wValue == 0x0201 || wValue == 0x0202) && wIndex == mic_wIndex) {
        uint8_t ch_index = wValue - 0x0201;
        ch_index = _USB_Audio_Channel_Check(ch_index);
        hal_usb_read_endpoint_fifo(0, wLength, &g_UsbAudio[0].mic_cur_channel_vol[ch_index]);
        USB_Send_Message(USB_MIC_SET_VOLUME, (void *)(g_UsbAudio[0].mic_cur_channel_vol[ch_index] + (ch_index << 16) + (mic_ep_addr << 24)));
    }
    else if (wValue == 0x0100 && wIndex == mic_wIndex) {
        hal_usb_read_endpoint_fifo(0, wLength, &g_usb_audio_microphone_mute);
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_Ep0_DataReceived microphone change mute to %d", 1, g_usb_audio_microphone_mute);
        USB_Send_Message(USB_MIC_SET_MUTE, (void *)(g_usb_audio_microphone_mute + (mic_ep_addr << 24)));
    }
    else if (wValue == 0x0100 && wIndex == mic_ep_addr) {
        hal_usb_read_endpoint_fifo(0, wLength, &g_usb_audio_mic_sample_rate);
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_Ep0_DataReceived microphone change sample rate to %d Hz", 1, g_usb_audio_mic_sample_rate);
        USB_Send_Message(USB_MIC_SET_SAMPLING_FREQ, (void *)(g_usb_audio_mic_sample_rate + (mic_ep_addr << 24)));
    }
#endif
    else {
        stall = true;
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_Ep0_DataReceived is out of case wValue:0x%X wIndex:0x%X", 2,
                    wValue, wIndex);
    }

    gUsbDevice.ep0_state = USB_EP0_RX_STATUS;
    hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, stall, true);
}

static void USB_Audio2_Ep0_DataReceived(void *data)
{
    bool stall = false;
    uint16_t wValue  = g_usb_audio2_wValue;
    uint16_t wIndex  = g_usb_audio2_wIndex;
    uint16_t wLength = g_usb_audio2_wLength;

    uint8_t  spk_ep_addr = (USB_EP_DIR_OUT | g_UsbAudio[1].stream_ep_out_id);
    uint16_t spk_wIndex  = (0x0A << 8 | g_UsbAudio[1].control_interface_id);

    /* Check EP0 data is enough */
    USB_Check_EP0_DataLen(0, wLength);

    /*wValue: control num  << 8 | channel*/
    /*wIndex: feature_unit << 8 | interface*/
    /*For audio_2,  wIndex =0x0A03*/
    if (wValue == 0x0100 && wIndex == spk_ep_addr) {
        hal_usb_read_endpoint_fifo(0, wLength, &g_usb_audio2_sample_rate);
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio2_Ep0_DataReceived speaker2 change sample rate to %d Hz", 1, g_usb_audio2_sample_rate);
        USB_Send_Message(USB_AUDIO_SET_SAMPLING_FREQ, (void *)(g_usb_audio2_sample_rate + (spk_ep_addr << 24)));
    }
    else if (wValue == 0x0100 && wIndex == spk_wIndex) {
        hal_usb_read_endpoint_fifo(0, wLength, &g_usb_audio2_mute);
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio2_Ep0_DataReceived speaker2 change mute to %d", 1, g_usb_audio2_mute);
        USB_Send_Message(USB_AUDIO_SET_MUTE, (void *)(g_usb_audio2_mute + (spk_ep_addr << 24)));
    }
    else if ((wValue == 0x0201 || wValue == 0x0202) && wIndex == spk_wIndex) {
        uint8_t ch_index = wValue - 0x0201;
        ch_index = _USB_Audio_Channel_Check(ch_index);
        hal_usb_read_endpoint_fifo(0, wLength, &g_UsbAudio[1].spk_cur_channel_vol[ch_index]);
        USB_Send_Message(USB_AUDIO_SET_VOLUME, (void *)(g_UsbAudio[1].spk_cur_channel_vol[ch_index] + (ch_index << 16) + (spk_ep_addr << 24)));
    }
    else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio2_Ep0_DataReceived is out of case wValue:0x%X wIndex:0x%X", 2,
                    wValue, wIndex);
        stall = true;
    }

    gUsbDevice.ep0_state = USB_EP0_RX_STATUS;
    hal_usb_update_endpoint_0_state(HAL_USB_EP0_DRV_STATE_READ_END, stall, true);
}

void USB_Audio1_V1_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool  bError = false;
    uint16_t usb_aduio1_spk_wIndex = (0x02 << 8 | g_UsbAudio[0].control_interface_id);
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    uint16_t usb_aduio1_mic_wIndex = (0x06 << 8 | g_UsbAudio[0].control_interface_id);
#endif

    /*wValue: control num << 8 | channel*/
    /*wIndex: interface << 8 | feature_unit*/
    switch (pcmd->bRequest) {
        case USB_AUDIO_1_REQ_GET_CUR:
            if (pcmd->wValue == 0x0100 && pcmd->wIndex == 0x01) { /* USB out EP for speaker*/
                USB_Generate_EP0Data(pep0state, pcmd, &g_usb_audio_sample_rate, pcmd->wLength);
            }
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
            else if (pcmd->wValue == 0x0100 && pcmd->wIndex == 0x81) { /* USB in EP for microphone*/
                USB_Generate_EP0Data(pep0state, pcmd, &g_usb_audio_mic_sample_rate, pcmd->wLength);
            } else if (pcmd->wValue == 0x0100 && pcmd->wIndex == usb_aduio1_mic_wIndex) { /* HSB 0x01: Mute Control; LSB 0x00: Master Channel. HSB 0x06: Feature Unit ID; LSB 0x00: Interface #*/
                USB_Generate_EP0Data(pep0state, pcmd, &g_usb_audio_microphone_mute, pcmd->wLength);
            } else if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio1_mic_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                uint8_t ch_index = pcmd->wValue - 0x0201;
                ch_index = _USB_Audio_Channel_Check(ch_index);
                LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_V1_Ep0_Command get mic volume, ch:%d raw:0x%04x", 2, ch_index+1, g_UsbAudio[0].mic_cur_channel_vol[ch_index]);
                USB_Generate_EP0Data(pep0state, pcmd, &g_UsbAudio[0].mic_cur_channel_vol[ch_index], pcmd->wLength);
            }
#endif
            else if (pcmd->wValue == 0x0100 && pcmd->wIndex == usb_aduio1_spk_wIndex) { /* HSB 0x01: Mute Control; LSB 0x00: Master Channel. HSB 0x02: Feature Unit ID; LSB 0x00: Interface #*/
                USB_Generate_EP0Data(pep0state, pcmd, &g_usb_audio_mute, pcmd->wLength);
            }  else if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio1_spk_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                uint8_t ch_index = pcmd->wValue - 0x0201;
                ch_index = _USB_Audio_Channel_Check(ch_index);
                LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_V1_Ep0_Command get speaker volume, ch:%d raw:0x%04x", 2, ch_index+1, g_UsbAudio[0].spk_cur_channel_vol[ch_index]);
                USB_Generate_EP0Data(pep0state, pcmd, &g_UsbAudio[0].spk_cur_channel_vol[ch_index], pcmd->wLength);
            }  else {
                bError = true;
            }
            break;
        case USB_AUDIO_1_REQ_GET_MIN:
            if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio1_spk_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[0].spk_min), pcmd->wLength);
            }
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
            else if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio1_mic_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[0].mic_min), pcmd->wLength);
            }
#endif
            else {
                bError = true;
            }
            break;
        case USB_AUDIO_1_REQ_GET_MAX:
            /* tell host the current setting */
            if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio1_spk_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[0].spk_max), pcmd->wLength);
            }
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
            else if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio1_mic_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[0].mic_max), pcmd->wLength);
            }
#endif
            else {
                bError = true;
            }
            break;
        case USB_AUDIO_1_REQ_GET_RES:
            if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio1_spk_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[0].spk_res), pcmd->wLength);
            }
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
            else if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio1_mic_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[0].mic_res), pcmd->wLength);
            }
#endif
            else {
                bError = true;
            }
            break;
        case USB_AUDIO_1_REQ_SET_CUR:
            g_usb_audio_wValue = pcmd->wValue;
            g_usb_audio_wIndex = pcmd->wIndex;
            g_usb_audio_wLength = pcmd->wLength;
            gUsbDevice.ep0_rx_handler = (usb_ep0_rx_ptr)USB_Audio1_Ep0_DataReceived;
            gUsbDevice.ep0_state = USB_EP0_RX;
            break;
        default:
            bError = true;
            break;
    }


    if (bError != true) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_V1_Ep0_Command bmRequestType:0x%02X bRequest:0x%02X wValue:0x%04X wIndex:0x%04X wLength:0x%04X",
                    5, pcmd->bmRequestType, pcmd->bRequest, pcmd->wValue, pcmd->wIndex, pcmd->wLength);
    } else{
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_V1_Ep0_Command is out of case bmRequestType:0x%02X bRequest:0x%02X wValue:0x%04X wIndex:0x%04X wLength:0x%04X",
                    5, pcmd->bmRequestType, pcmd->bRequest, pcmd->wValue, pcmd->wIndex, pcmd->wLength);
    }

    /* Stall command if an error occured */
    USB_EP0_Command_Hdlr(bError);
}


void USB_Audio2_V1_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    bool  bError = false;
    uint16_t usb_aduio2_spk_wIndex = (0x0A << 8 | g_UsbAudio[1].control_interface_id);

    /*wValue: control num << 8 | channel*/
    /*wIndex: feature_unit << 8 | interface*/
    /*For audio_2, the wValue = 0x0100,   wIndex =0X0A03*/
    switch (pcmd->bRequest) {
        case USB_AUDIO_1_REQ_GET_CUR:
            if (pcmd->wValue == 0x0100 && pcmd->wIndex == 0x01) { /* USB out EP for speaker*/
                USB_Generate_EP0Data(pep0state, pcmd, &g_usb_audio2_sample_rate, pcmd->wLength);
            } else if (pcmd->wValue == 0x0100 && pcmd->wIndex == usb_aduio2_spk_wIndex) { /* HSB 0x01: Mute Control; LSB 0x00: Master Channel. HSB 0x02: Feature Unit ID; LSB 0x00: Interface #*/
                USB_Generate_EP0Data(pep0state, pcmd, &g_usb_audio2_mute, pcmd->wLength);
            } else if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio2_spk_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                uint8_t ch_index = pcmd->wValue - 0x0201;
                ch_index = _USB_Audio_Channel_Check(ch_index);
                LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio2_V1_Ep0_Command get speaker volume, ch:%d raw:0x%04x", 2, ch_index+1, g_UsbAudio[1].spk_cur_channel_vol[ch_index]);
                USB_Generate_EP0Data(pep0state, pcmd, &g_UsbAudio[1].spk_cur_channel_vol[ch_index], pcmd->wLength);
            } else {
                bError = true;
            }
            break;
        case USB_AUDIO_1_REQ_GET_MIN:
            if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio2_spk_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[1].spk_min), pcmd->wLength);
            } else {
                bError = true;
            }
            break;
        case USB_AUDIO_1_REQ_GET_MAX:
            /* tell host the current setting */
            if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio2_spk_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[1].spk_max), pcmd->wLength);
            } else {
                bError = true;
            }
            break;
        case USB_AUDIO_1_REQ_GET_RES:
            if ((pcmd->wValue == 0x0201 || pcmd->wValue == 0x0202) && (pcmd->wIndex == usb_aduio2_spk_wIndex)) { /* HSB 0x02: Volume Control; LSB 0x01: Channel 1*/
                USB_Generate_EP0Data(pep0state, pcmd, (void *)(&g_UsbAudio[1].spk_res), pcmd->wLength);
            } else {
                bError = true;
            }
            break;
        case USB_AUDIO_1_REQ_SET_CUR:
            g_usb_audio2_wValue = pcmd->wValue;
            g_usb_audio2_wIndex = pcmd->wIndex;
            g_usb_audio2_wLength = pcmd->wLength;
            gUsbDevice.ep0_rx_handler = (usb_ep0_rx_ptr)USB_Audio2_Ep0_DataReceived;
            gUsbDevice.ep0_state = USB_EP0_RX;
            break;
        default:
            bError = true;
            break;
    }

    if (bError != true) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio2_V1_Ep0_Command bmRequestType:0x%02X bRequest:0x%02X wValue:0x%04X wIndex:0x%04X wLength:0x%04X",
                    5, pcmd->bmRequestType, pcmd->bRequest, pcmd->wValue, pcmd->wIndex, pcmd->wLength);
    } else{
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio2_V1_Ep0_Command is out of case bmRequestType:0x%02X bRequest:0x%02X wValue:0x%04X wIndex:0x%04X wLength:0x%04X",
                    5, pcmd->bmRequestType, pcmd->bRequest, pcmd->wValue, pcmd->wIndex, pcmd->wLength);
    }

    /* Stall command if an error occured */
    USB_EP0_Command_Hdlr(bError);
}

void USB_Audio1_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    if (USB_Audio[0].audio_version == USB_AUDIO_VERSION_V1) {
        USB_Audio1_V1_Ep0_Command(pep0state, pcmd);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Ep0_Command version:%d is not %d", 2, (uint8_t)USB_Audio[0].audio_version, USB_AUDIO_VERSION_V1);
    }
}

void USB_Audio2_Ep0_Command(Usb_Ep0_Status *pep0state, Usb_Command *pcmd)
{
    if (USB_Audio[1].audio_version == USB_AUDIO_VERSION_V1) {
        USB_Audio2_V1_Ep0_Command(pep0state, pcmd);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Ep0_Command version:%d is not %d", 2, (uint8_t)USB_Audio[0].audio_version, USB_AUDIO_VERSION_V1);
    }
}

/************************************************************
    Iso EP OUT handle functions
*************************************************************/
/* Set up RX, corresponding smaple size and channel */
uint32_t USB_Audio_Set_RX_Alt(uint32_t port, uint8_t alt_num)
{
    usb_audio_dscr_as_format_type_t *p = NULL;

    if((port == 0) && (alt_num == 1)) { /* Speaker 1 */
        p = &audio1_spk_stream_format_dscr;
    }
#ifdef USB_AUDIO_RX1_ALT2_ENABLE
    else if((port == 0) && (alt_num == 2)) {
        p = &audio1_spk_stream_alt2_format_dscr;
    }
#endif
#ifdef USB_AUDIO_RX1_ALT3_ENABLE
    else if((port == 0) && (alt_num == 3)) {
        p = &audio1_spk_stream_alt3_format_dscr;
    }
#endif
    else if((port == 1) && (alt_num == 1)) { /* Speaker 2 */
        p = &audio2_spk_stream_format_dscr;
    }
#ifdef USB_AUDIO_RX2_ALT2_ENABLE
    else if((port == 1) && (alt_num == 2)) {
        p = &audio2_spk_stream_alt2_format_dscr;
    }
#endif

    if (port == 0) {
        if (p) {
            g_usb_audio_sample_size = p->bSubFrameSize;
            g_usb_audio_channel = p->bNrChannels;
        }
        else {
            /* g_usb_audio_sample_rate set by host, clear by alt_num = 0 */
            g_usb_audio_sample_size = 0;
            g_usb_audio_channel     = 0;
        }
    }
    else if (port == 1) {
        if (p) {
            g_usb_audio2_sample_size = p->bSubFrameSize;
            g_usb_audio2_channel = p->bNrChannels;
        }
        else {
            /* g_usb_audio2_sample_rate set by host, clear by alt_num = 0 */
            g_usb_audio2_sample_size = 0;
            g_usb_audio2_channel     = 0;
        }
    }

    if(port == 0) {
         LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_RX_Alt port:%d alt_num:%d sample_size:%d channel:%d", 4,
                     port, alt_num, g_usb_audio_sample_size, g_usb_audio_channel);
    } else if (port == 1) {
         LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_RX_Alt port:%d alt_num:%d sample_size:%d channel:%d", 4,
                     port, alt_num, g_usb_audio2_sample_size, g_usb_audio2_channel);
    }

    return 0;
}

uint8_t USB_Audio_Get_RX_Channel(uint32_t port)
{
    uint8_t channel;

    if(port == 0){
        channel = g_usb_audio_channel;
    } else {
        channel = g_usb_audio2_channel;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_RX_Channel port:%d channel:%d", 2, port, channel);
    return channel;
}

uint8_t USB_Audio_Get_RX_Sample_Size(uint32_t port)
{
    uint8_t sample_size;

    if(port == 0){
        sample_size = g_usb_audio_sample_size;
    } else {
        sample_size = g_usb_audio2_sample_size;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_RX_Sample_Size port:%d sample_size:%d ", 2, port, sample_size);
    return sample_size;
}

uint32_t USB_Audio_Get_RX_Sample_Rate(uint32_t port)
{
    uint32_t sample_rate = 0;

    if (is_vusb_ready()) {
        if (port == 0) {
            sample_rate = g_usb_audio_sample_rate;
        } else if (port == 1) {
            sample_rate = g_usb_audio2_sample_rate;
        }
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_RX_Sample_Rate port:%d sample_rate:%d", 2, port, sample_rate);
    return sample_rate;
}

/* USB_AUDIO_DSCR_MAX_FREQ_NUM */
uint32_t Get_Maximum(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e)
{
   uint32_t i = 0;

   if (a > i) i = a;
   if (b > i) i = b;
   if (c > i) i = c;
   if (d > i) i = d;
   if (e > i) i = e;

   return i;
}

uint32_t USB_Audio_Get_RX_Sample_Rate_Max(uint32_t port, uint8_t alt_num)
{
    uint32_t sample_rate_max = 0;
    usb_audio_dscr_as_format_type_t *p = NULL;

    if((port == 0) && (alt_num == 1)) { /* Speaker 1 */
        p = &audio1_spk_stream_format_dscr;
    }
#ifdef USB_AUDIO_RX1_ALT2_ENABLE
    else if((port == 0) && (alt_num == 2)) {
        p = &audio1_spk_stream_alt2_format_dscr;
    }
#endif
#ifdef USB_AUDIO_RX1_ALT3_ENABLE
    else if((port == 0) && (alt_num == 3)) {
        p = &audio1_spk_stream_alt3_format_dscr;
    }
#endif
    else if((port == 1) && (alt_num == 1)) { /* Speaker 2 */
        p = &audio2_spk_stream_format_dscr;
    }
#ifdef USB_AUDIO_RX2_ALT2_ENABLE
    else if((port == 1) && (alt_num == 2)) {
        p = &audio2_spk_stream_alt2_format_dscr;
    }
#endif

    if (p) {
        sample_rate_max = Get_Maximum(AUDFREQ_TO_U32(p->tSamFreq[0]),
                                      AUDFREQ_TO_U32(p->tSamFreq[1]),
                                      AUDFREQ_TO_U32(p->tSamFreq[2]),
                                      AUDFREQ_TO_U32(p->tSamFreq[3]),
                                      AUDFREQ_TO_U32(p->tSamFreq[4]));
    }
    else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Get_RX_Sample_Rate_Max port:%d sample rate is 0", 1, port);
        sample_rate_max = 0;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_RX_Sample_Rate_Max port:%d sample rate max:%d", 2, port, sample_rate_max);

    return sample_rate_max;
}

uint32_t USB_Audio_Get_RX_Alt_Byte(uint32_t port, uint8_t alt_num)
{
    /* byte = sample rate x sample size x channel */
    bool hb_enable = false;
    uint8_t hb_packet;
    uint32_t byte;
    uint32_t sample_rate_max;
    usb_audio_dscr_as_format_type_t *p = NULL;

    /* Unint: K HZ */
    sample_rate_max = USB_Audio_Get_RX_Sample_Rate_Max(port, alt_num);
    sample_rate_max /= 1000;

    if((port == 0) && (alt_num == 1)) { /* Speaker 1 */
        p = &audio1_spk_stream_format_dscr;
    }
#ifdef USB_AUDIO_RX1_ALT2_ENABLE
    else if((port == 0) && (alt_num == 2)) {
        p = &audio1_spk_stream_alt2_format_dscr;
    }
#endif
#ifdef USB_AUDIO_RX1_ALT3_ENABLE
    else if((port == 0) && (alt_num == 3)) {
        p = &audio1_spk_stream_alt3_format_dscr;
    }
#endif
    else if((port == 1) && (alt_num == 1)) { /* Speaker 2 */
        p = &audio2_spk_stream_format_dscr;
    }
#ifdef USB_AUDIO_RX2_ALT2_ENABLE
    else if((port == 1) && (alt_num == 2)) {
        p = &audio2_spk_stream_alt2_format_dscr;
    }
#endif

    if (p) {   
        byte = sample_rate_max * p->bSubFrameSize * p->bNrChannels;
    }   
    else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Get_RX_Alt_Byte port:%d alt_num:%d byte is 0", 2, port, alt_num);
        byte = 0;
    }

    /* USB 2.0 Spec - 5.9.2 High Bandwidth Isochronous Endpoints */
    hb_packet = byte / HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED;
    if(hb_packet != 0){
        hb_enable = true;
    }

    if(hb_enable){
       /* USB 2.0 Spec - 9.6.6 Endpoint
        * Bits 12..11 specify the number of additional transaction opportunities per microframe:
        * 00 = None (1 transaction per microframe)
        * 01 = 1 additional (2 per microframe)
        * 10 = 2 additional (3 per microframe)
        * 11 = Reserved
        */
        byte /= (hb_packet + 1); /* Convert 1 additional to 2 per microframe e.g. 1152 = 576 + 576 */
        byte |= hb_packet << 11;

        hal_usb_set_high_bandwidth(true, hb_packet);
    } else {
        hal_usb_set_high_bandwidth(false, hb_packet);
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_RX_Alt_Byte port:%d alt_num:%d byte:%d hb_packet:%d", 4,
               port, alt_num, byte, hb_packet);

    /* Include data_size packet inb bits 12..11 */
    return byte;
}

static uint32_t USB_Audio_Get_RX_Bytes(uint32_t port)
{
    /* byte = sample rate x sample size x channel */
    uint32_t byte;

    if(port == 0){
        byte =(g_usb_audio_sample_rate/1000) * g_usb_audio_sample_size * g_usb_audio_channel;
    } else if (port == 1){
        byte = (g_usb_audio2_sample_rate/1000) * g_usb_audio2_sample_size * g_usb_audio2_channel;
    } else {
        byte = 0;
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Get_RX_Bytes port:%d is not valid", 1, port);
    }

    return byte;
}

static uint32_t USB_Audio_Buffer_Space(uint32_t port)
{
    uint32_t byte = USB_Audio_Get_RX_Bytes(port);

    if (g_UsbAudio[port].rx_dma_buffer_is_full) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Buffer_Space RX buffer is full ERROR!!", 0);
        return 0;
    } else {
        if ((g_UsbAudio[port].rx_dma_buffer_write + byte) == g_UsbAudio[port].rx_dma_buffer_len) {
            /*LOG_MSGID_I(USBAUDIO_DRV, "usb error USB_Audio_Buffer_Space() reach full\n", 0);*/
        }
        return byte;
    }
}

void *USB_Audio_Rx_Buffer_Get_Read_Address(uint32_t port)
{
    return g_UsbAudio[port].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_read;
}

uint32_t USB_Audio_Rx_Buffer_Get_Bytes(uint32_t port)
{
    uint32_t write_temp, read_temp, rc;

    write_temp = g_UsbAudio[port].rx_dma_buffer_write;
    read_temp = g_UsbAudio[port].rx_dma_buffer_read;

    if (write_temp > read_temp) {
        rc = write_temp - read_temp;
    } else if (write_temp < read_temp || g_UsbAudio[port].rx_dma_buffer_is_full == 1) {
        rc = g_UsbAudio[port].rx_dma_buffer_len - read_temp;
    } else {
        rc = 0;
    }

    return rc;
}

void USB_Audio_Rx_Buffer_Drop_Bytes(uint32_t port, uint32_t bytes)
{
    uint32_t read_temp;

    if (g_UsbAudio[port].rx_dma_buffer_len > g_UsbAudio[port].rx_dma_buffer_read + bytes) {
        read_temp = g_UsbAudio[port].rx_dma_buffer_read + bytes;
    } else {
        read_temp = bytes - (g_UsbAudio[port].rx_dma_buffer_len - g_UsbAudio[port].rx_dma_buffer_read);
    }

    g_UsbAudio[port].rx_dma_buffer_read = read_temp;
    g_UsbAudio[port].rx_dma_buffer_is_full = 0;

    return;
}


/************************************************************
    Iso EP OUT handler
*************************************************************/
#ifdef USB_AUDIO_RX_TIME_MEASURE
uint32_t usb_rx1_time1_start = 0;
uint32_t usb_rx1_time1_end   = 0;
uint32_t usb_rx1_time2_start = 0;
uint32_t usb_rx1_time2_end   = 0;

uint32_t usb_rx2_time1_start = 0;
uint32_t usb_rx2_time1_end   = 0;
uint32_t usb_rx2_time2_start = 0;
uint32_t usb_rx2_time2_end   = 0;
#endif

/* EP Iso Out interrupt handler, called by EP interrupt */
void USB_Audio1_IsoOut_Hdr(void)
{
#ifdef USB_AUDIO_RX_TIME_MEASURE
    SLA_CustomLogging("RX1", SA_START);
    SLA_CustomLogging("RX1", SA_LABEL);

    /* Timer1 - Check the ISO out is expected 1ms/transaction */
    uint32_t time_gap = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx1_time1_start);

    time_gap = usb_rx1_time1_start - usb_rx1_time1_end;

    if ((time_gap > 1500) && (time_gap < 5000)) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr is not 1ms time gap:%dus", 1, time_gap);
        assert(0);
    } else {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio1_IsoOut_Hdr time measurement gap:%dus", 1, time_gap);
    }

    usb_rx1_time1_end = usb_rx1_time1_start;

    /* Timer2 - Check time consuming of each operation */
    uint32_t time_gap1 = 0, time_gap2 = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx1_time2_start);
#endif

    uint32_t port = 0;
    uint32_t nCount;
    hal_usb_status_t dma_ret = HAL_USB_STATUS_ERROR;

    if (USB_Audio[port].initialized == false) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr not be initialized", 0);
        return;
    }

    /* Check the rx byte is valid */
    nCount = USB_Audio_Get_RX_Bytes(port);
    if(nCount == 0){
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr rx bytes is 0 sample_rate:%d sample_size:%d channel:%d", 3,
                    g_usb_audio_sample_rate, g_usb_audio_sample_size, g_usb_audio_channel);
        hal_usb_clear_rx_endpoint_fifo(g_UsbAudio[port].rxpipe->byEP, HAL_USB_EP_TRANSFER_ISO, false);
        hal_usb_set_endpoint_rx_ready(g_UsbAudio[port].rxpipe->byEP);
        return;
    }

    // LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr rx byte:%d", 1, nCount);

    if (g_UsbAudio[port].rx_dma_buffer_is_full) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr rx_dma_buffer is full", 0);
    } else {
        uint32_t nSpace = USB_Audio_Buffer_Space(port);

        if (nSpace < nCount) {
            LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr no enough space", 0);
            nCount = nSpace;
        }

        /* If nCount > 1024 then change to DMA mode */
        if(nCount < HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED){
            if (g_UsbAudio[port].rx_dma_buffer_write >= g_UsbAudio[port].rx_dma_buffer_read) {
                hal_usb_read_endpoint_fifo(g_UsbAudio[port].rxpipe->byEP, nCount, g_UsbAudio[port].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_write);
            } else {
                if ((g_UsbAudio[port].rx_dma_buffer_write + nCount) <= g_UsbAudio[port].rx_dma_buffer_read) {
                    hal_usb_read_endpoint_fifo(g_UsbAudio[port].rxpipe->byEP, nCount, g_UsbAudio[port].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_write);
                } else {
                    LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr() wptr over rptr", 0);
                }
            }
        } else {
            hal_usb_start_dma_channel(g_UsbAudio[0].rxpipe->byEP,
                                      HAL_USB_EP_DIRECTION_RX,
                                      HAL_USB_EP_TRANSFER_ISO,
                                      (void *)(hal_memview_mcu_to_infrasys((uint32_t)g_UsbAudio[0].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_write)),
                                      nCount,
                                      NULL, //hal_usb_dma_handler_t callback,
                                      NULL, //bool callback_upd_run,
                                      HAL_USB_DMA1_TYPE);

            /* Polling for DMA ready */
            uint32_t dma_timeout_start, dma_timeout_end, dma_timeout_duration;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &dma_timeout_start);
            while(dma_ret != HAL_USB_STATUS_OK){
                dma_ret = hal_usb_is_dma_ready(g_UsbAudio[0].rxpipe->byEP, HAL_USB_EP_DIRECTION_RX);

                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &dma_timeout_end);
                dma_timeout_duration = dma_timeout_end - dma_timeout_start;
                //LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr DMA is not ready %dus", 1, dma_timeout_duration);

                /* Timeout > 1ms */
                if(dma_timeout_duration > 1000){
                    LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr wait DMA ready to timeout %dus", 1, dma_timeout_duration);
                    return;
                }
            }
        }

#ifdef USB_AUDIO_DUMP_RX1
        LOG_AUDIO_DUMP((g_UsbAudio[port].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_write), nCount, USB_AUDIO_RX1);
#endif

#if 0 /* Debug log for audio data*/
        uint8_t *a;
        a = g_UsbAudio[port].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_write;

        LOG_MSGID_I(USBAUDIO_DRV, "USB [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X] - 1s", 8,
                    a[0],a[1],a[2],a[3],a[4],a[5],a[6],a[7]);
        LOG_MSGID_I(USBAUDIO_DRV, "USB [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X] - 1e", 8,
                    a[552+0],a[552+1],a[552+2],a[552+3],a[552+4],a[552+5],a[552+6],a[552+7]);
        LOG_MSGID_I(USBAUDIO_DRV, "USB [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X] - 2s", 8,
                    a[576+0],a[576+1],a[576+2],a[576+3],a[576+4],a[576+5],a[576+6],a[576+7]);
        LOG_MSGID_I(USBAUDIO_DRV, "USB [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X] - 2e", 8,
                    a[576+0+552],a[576+1+552],a[576+2+552],a[576+3+552],a[576+4+552],a[576+5+552],a[576+6+552],a[576+7+552]);
#endif

        g_UsbAudio[port].rx_dma_buffer_write += nCount;

        if (g_UsbAudio[port].rx_dma_buffer_write == g_UsbAudio[port].rx_dma_buffer_len) {
            g_UsbAudio[port].rx_dma_buffer_write = 0;
        }
    }

#ifdef USB_AUDIO_RX_TIME_MEASURE
        /* Timer2 - Get data */
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx1_time2_end);
        time_gap1 = usb_rx1_time2_end - usb_rx1_time2_start;

        /* Timer2 - Audio callback */
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx1_time2_start);
#endif

    if (USB_Audio[port].rx_cb) {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio1_IsoOut_Hdr enter RX callback", 0);
        hal_usb_set_endpoint_rx_ready(g_UsbAudio[port].rxpipe->byEP);
        USB_Audio[port].rx_cb();
    } else {
        LOG_MSGID_E(USB_DEBUG, "USB_Audio1_IsoOut_Hdr RX callback is NULL", 0);
        hal_usb_set_endpoint_rx_ready(g_UsbAudio[port].rxpipe->byEP);
        USB_Audio_Rx_Buffer_Drop_Bytes(port, nCount);
    }

#ifdef USB_AUDIO_RX_TIME_MEASURE
    /* Timer2 - Audio callback */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx1_time2_end);
    time_gap2 = usb_rx1_time2_end - usb_rx1_time2_start;

    if((time_gap1 + time_gap2) > 500){
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio1_IsoOut_Hdr (1)Get data: %dus (2)audio callback: %dus > 500us", 2, time_gap1, time_gap2);
    } else {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio1_IsoOut_Hdr (1)Get data: %dus (2)audio callback: %dus", 2, time_gap1, time_gap2);
    }

    SLA_CustomLogging("RX1", SA_STOP);
#endif

    return;

}

/* EP Iso Out interrupt handler, called by EP interrupt */
void USB_Audio2_IsoOut_Hdr(void)
{
#ifdef USB_AUDIO_RX_TIME_MEASURE
    SLA_CustomLogging("RX2", SA_START);
    SLA_CustomLogging("RX2", SA_LABEL);

    /* Timer1 - Check the ISO out is expected 1ms/transaction */
    uint32_t time_gap = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx2_time1_start);

    time_gap = usb_rx2_time1_start - usb_rx2_time1_end;

    if ((time_gap > 1500) && (time_gap < 5000)) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio2_IsoOut_Hdr is not 1ms time gap:%dus", 2, time_gap);
        assert(0);
    } else {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio2_IsoOut_Hdr time measurement gap:%dus", 2, time_gap);
    }

    usb_rx2_time1_end = usb_rx2_time1_start;

    /* Timer2 - Check time consuming of each operation */
    uint32_t time_gap1 = 0, time_gap2 = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx2_time2_start);
#endif

    uint32_t port = 1;
    uint32_t nCount;

    if (USB_Audio[port].initialized == false) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio2_IsoOut_Hdr was not be initialized", 0);
        return;
    }

    /* Check the rx byte is valid */
    nCount = USB_Audio_Get_RX_Bytes(port);
    if(nCount == 0){
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio2_IsoOut_Hdr rx bytes is 0 sample_rate:%d sample_size:%d channel:%d", 3,
                    g_usb_audio2_sample_rate, g_usb_audio2_sample_size, g_usb_audio2_channel);
        hal_usb_clear_rx_endpoint_fifo(g_UsbAudio[port].rxpipe->byEP, HAL_USB_EP_TRANSFER_ISO, false);
        hal_usb_set_endpoint_rx_ready(g_UsbAudio[port].rxpipe->byEP);
        return;
    }

    if (g_UsbAudio[port].rx_dma_buffer_is_full) {
        LOG_MSGID_E(USBAUDIO_DRV, "ERROR!! USB_Audio2_IsoOut_Hdr rx buffer_is_full", 0);
    } else {
        uint32_t nSpace = USB_Audio_Buffer_Space(port);

        if (nSpace < nCount) {
            LOG_MSGID_E(USBAUDIO_DRV, "ERROR!! USB_Audio2_IsoOut_Hdr no enough space", 0);
            nCount = nSpace;
        }

        if (g_UsbAudio[port].rx_dma_buffer_write >= g_UsbAudio[port].rx_dma_buffer_read) {
            hal_usb_read_endpoint_fifo(g_UsbAudio[port].rxpipe->byEP, nCount, g_UsbAudio[port].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_write);
        } else {
            if ((g_UsbAudio[port].rx_dma_buffer_write + nCount) <= g_UsbAudio[port].rx_dma_buffer_read) {
                hal_usb_read_endpoint_fifo(g_UsbAudio[port].rxpipe->byEP, nCount, g_UsbAudio[port].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_write);
            } else {
                LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio2_IsoOut_Hdr() wptr over rptr", 0);
            }
        }

#ifdef USB_AUDIO_DUMP_RX2
        LOG_AUDIO_DUMP((g_UsbAudio[port].rx_dma_buffer + g_UsbAudio[port].rx_dma_buffer_write), nCount, USB_AUDIO_RX2);
#endif

        g_UsbAudio[port].rx_dma_buffer_write += nCount;

        if (g_UsbAudio[port].rx_dma_buffer_write == g_UsbAudio[port].rx_dma_buffer_len) {
            g_UsbAudio[port].rx_dma_buffer_write = 0;
        }
    }

#ifdef USB_AUDIO_RX_TIME_MEASURE
    /* Timer2 - Get data */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx2_time2_end);
    time_gap1 = usb_rx2_time2_end - usb_rx2_time2_start;

    /* Timer2 - Audio callback */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx2_time2_start);
#endif

    if (USB_Audio[port].rx_cb) {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio2_IsoOut_Hdr enter RX callback", 0);
        hal_usb_set_endpoint_rx_ready(g_UsbAudio[port].rxpipe->byEP);
        USB_Audio[port].rx_cb();
    } else {
        LOG_MSGID_E(USB_DEBUG, "USB_Audio2_IsoOut_Hdr RX callback is NULL", 0);
        hal_usb_set_endpoint_rx_ready(g_UsbAudio[port].rxpipe->byEP);
        USB_Audio_Rx_Buffer_Drop_Bytes(port, nCount);
    }

#ifdef USB_AUDIO_RX_TIME_MEASURE
    /* Timer2 - Audio callback */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_rx2_time2_end);
    time_gap2 = usb_rx2_time2_end - usb_rx2_time2_start;

    if((time_gap1 + time_gap2) > 500){
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio2_IsoOut_Hdr (1)Get data: %dus (2)audio callback: %dus > 500us", 2, time_gap1, time_gap2);
    } else {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio2_IsoOut_Hdr (1)Get data: %dus (2)audio callback: %dus", 2, time_gap1, time_gap2);
    }

    SLA_CustomLogging("RX2", SA_STOP);
#endif

}


/* EP Iso Out reset handler */
void USB_Audio1_IsoOut_Reset(void)
{
    g_UsbAudio[0].rxpipe = &g_UsbAudio[0].stream_ep_out_info->ep_status.epout_status;
}
void USB_Audio2_IsoOut_Reset(void)
{
    g_UsbAudio[1].rxpipe = &g_UsbAudio[1].stream_ep_out_info->ep_status.epout_status;
}

/************************************************************
    Iso EP IN handle functions
*************************************************************/
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
/* Set up TX, corresponding smaple size and channel */
uint32_t USB_Audio_Set_TX_Alt(uint32_t port, uint8_t alt_num)
{
    usb_audio_dscr_as_format_type_t *p = NULL;

    /* g_usb_audio_sample_rate set by host */
    if((port == 0) && (alt_num == 1)){
        p = &audio1_mic_stream_format_dscr;
    }
    #ifdef USB_AUDIO_TX1_ALT2_ENABLE
    else if((port == 0) && (alt_num == 2)){
        p = &audio1_mic_stream_alt2_format_dscr;
    }
    #endif

    if (port == 0) {
        if (p) {
            g_usb_audio_mic_sample_size = p->bSubFrameSize;
            g_usb_audio_mic_channel = p->bNrChannels;
        }
        else {
            /* g_usb_audio_sample_rate set by host, clear by alt_num = 0 */
            g_usb_audio_mic_sample_size = 0;
            g_usb_audio_mic_channel     = 0;
        }
    }

    if(port == 0){
         LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_TX_Alt port:%d alt_num:%d sample_size:%d channel:%d", 4,
                     port, alt_num, g_usb_audio_mic_sample_size, g_usb_audio_mic_channel);
    } else {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_TX_Alt port:%d is invalid", 4, port);
    }

    return 0;
}

uint8_t USB_Audio_Get_TX_Channel(uint32_t port)
{
    uint8_t channel = 0;

    if(port == 0){
        channel = g_usb_audio_mic_channel;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_TX_Channel port:%d channel:%d", 2, port, channel);
    return channel;
}

uint8_t USB_Audio_Get_TX_Sample_Size(uint32_t port)
{
    uint8_t sample_size = 0;

    if(port == 0){
        sample_size = g_usb_audio_mic_sample_size;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_TX_Sample_Size port:%d sample_size:%d ", 2, port, sample_size);
    return sample_size;
}

uint32_t USB_Audio_Get_TX_Sample_Rate(uint32_t port)
{
    uint32_t sample_rate = 0;

    if (is_vusb_ready()) {
        if (port == 0) {
            sample_rate = g_usb_audio_mic_sample_rate;
        }
    }

    return sample_rate;
}

uint32_t USB_Audio_Get_TX_Sample_Rate_Max(uint32_t port, uint8_t alt_num)
{
    uint32_t sample_rate_max = 0;
    usb_audio_dscr_as_format_type_t *p = NULL;



    if((port == 0) && (alt_num == 1)){
        p = &audio1_mic_stream_format_dscr;
    }
    #ifdef USB_AUDIO_TX1_ALT2_ENABLE
    else if ((port == 0) && (alt_num == 2)){
        p = &audio1_mic_stream_alt2_format_dscr;
    }
    #endif


    if(p){
        sample_rate_max = Get_Maximum(AUDFREQ_TO_U32(p->tSamFreq[0]),
                                      AUDFREQ_TO_U32(p->tSamFreq[1]),
                                      AUDFREQ_TO_U32(p->tSamFreq[2]),
                                      AUDFREQ_TO_U32(p->tSamFreq[3]),
                                      AUDFREQ_TO_U32(p->tSamFreq[4]));
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Get_TX_Sample_Rate_Max port:%d sample rate = 0 ERROR !!", 1, port);
        sample_rate_max = 0;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_TX_Sample_Rate_Max port:%d sample rate max:%d", 2, port, sample_rate_max);
    return sample_rate_max;
}

uint32_t USB_Audio_Get_TX_Alt_Byte(uint32_t port, uint8_t alt_num)
{
    /* byte = sample rate x sample size x channel */
    uint32_t byte = 0;
    uint32_t sample_rate_max;
    usb_audio_dscr_as_format_type_t *p = NULL;

    /* Unint: K HZ */
    sample_rate_max = USB_Audio_Get_TX_Sample_Rate_Max(port, alt_num);
    sample_rate_max /= 1000;

    /* Microphone 1 */
    if((port == 0) && (alt_num == 1)) {
        p = &audio1_mic_stream_format_dscr;
    }
    #ifdef USB_AUDIO_TX1_ALT2_ENABLE
    else if((port == 0) && (alt_num == 2)){
         p = &audio1_mic_stream_alt2_format_dscr;
    }
    #endif

    if(p) {
        byte = sample_rate_max * p->bSubFrameSize * p->bNrChannels;
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Get_TX_Alt_Byte port:%d alt_num:%d byte is 0", 2, port, alt_num);
        return byte;
    }

    LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Get_TX_Alt_Byte port:%d alt_num:%d byte:%d", 3, port, alt_num, byte);
    return byte;
}

uint32_t USB_Audio_Get_TX_Bytes(uint32_t port)
{
    /* byte = sample rate x sample size x channel */
    uint32_t byte = 0;

    if(port == 0){
        byte =(g_usb_audio_mic_sample_rate/1000) * g_usb_audio_mic_sample_size * g_usb_audio_mic_channel;
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Get_RX_Bytes port:%d is not valid", 1, port);
    }

    return byte;
}

#ifdef USB_AUDIO_TEST_TONE
/* 5000HZ, 48000FS, 1Channels, 16bits */
static const uint16_t test_5khz_tone[48] = {
    0x0000, 0x4c18, 0x78bd, 0x737c, 0x3e7f, 0xefb0, 0xa79d, 0x8412,
    0x93c0, 0xd02b, 0x205a, 0x632b, 0x7d00, 0x632b, 0x205a, 0xd02b,
    0x93c0, 0x8412, 0xa79d, 0xefb0, 0x3e80, 0x737c, 0x78bd, 0x4c18,
    0x0000, 0xb3e8, 0x8743, 0x8c84, 0xc180, 0x1050, 0x5863, 0x7bee,
    0x6c40, 0x2fd5, 0xdfa6, 0x9cd5, 0x8300, 0x9cd5, 0xdfa6, 0x2fd5,
    0x6c40, 0x7bee, 0x5863, 0x1050, 0xc181, 0x8c84, 0x8743, 0xb3e8,
};
#endif

/* Create maxiumn size of tx buffer for mute data */
static const uint8_t mic_mute_data[USB_AUDIO_TX_EP_MAX_SIZE] = {0};

/* Send audio data API by endpoint */
uint32_t USB_Audio_TX_SendData(uint32_t port, uint32_t len , uint8_t *data)
{
    uint32_t ep_id = g_UsbAudio[port].stream_ep_in_id;
    uint32_t ret = len;
    uint32_t nCount;

    LOG_MSGID_I(USB_DEBUG, "USB_Audio_TX_SendData ep_in_id[%x] data[%x] len[%x]", 3, ep_id, data, len);

    /* Check the length is valid */
    if (len == 0) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_TX_SendData Data length is 0", 0);
    }

    nCount = USB_Audio_Get_TX_Bytes(port);
    if (len != nCount) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_TX_SendData Data length %d byte dismatch with %d byte", 2, len, nCount);
    }

#ifdef USB_AUDIO_DUMP_TX1
    LOG_AUDIO_DUMP(data, len, USB_AUDIO_TX1);
#endif

    /* Set data from share buffer into USB FIFO */
    hal_usb_write_endpoint_fifo(ep_id, len, (void *)data);

    /* Send usb data */
    hal_usb_set_endpoint_tx_ready(ep_id);

    /* Print debug log */
    LOG_MSGID_I(USB_DEBUG, "USB_Audio_TX_SendData [%02X][%02X][%02X][%02X][%02X][%02X][%02X][%02X]", 8,
                data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);

    return ret;
}


/************************************************************
    Iso EP IN handler
*************************************************************/
#ifdef USB_AUDIO_TX_TIME_MEASURE
uint32_t usb_tx1_time1_start = 0;
uint32_t usb_tx1_time1_end  = 0;
uint32_t usb_tx1_time2_start = 0;
uint32_t usb_tx1_time2_end  = 0;
#endif

/* EP Iso In interrupt handler, called by EP interrupt */
void USB_Audio_IsoIn_Hdr(void)
{
#ifdef USB_AUDIO_TX_TIME_MEASURE
    SLA_CustomLogging("TX1", SA_START);
    SLA_CustomLogging("TX1", SA_LABEL);

    /* Timer1 - Check the ISO in is expected 1ms/transaction */
    uint32_t time_gap = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_tx1_time1_start);

    time_gap = usb_tx1_time1_start - usb_tx1_time1_end;

    if ((time_gap > 1500) && (time_gap < 5000)) {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_IsoIn_Hdr is not 1ms time gap:%dus", 2, time_gap);
        assert(0);
    } else {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio_IsoIn_Hdr time measurement gap:%dus", 2, time_gap);
    }

    usb_tx1_time1_end = usb_tx1_time1_start;

    /* Timer2 - Check time consuming of each operation */
    uint32_t time_gap1;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_tx1_time2_start);
#endif

    uint32_t port = 0;
    uint32_t nCount;

    nCount = USB_Audio_Get_TX_Bytes(port);

#ifndef USB_AUDIO_TEST_TONE
    if (USB_Audio[port].tx_cb) {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio_IsoIn_Hdr TX callback", 0);
        USB_Audio[port].tx_cb();
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_IsoIn_Hdr TX callback is NULL", 0);
        /**
         * NOTE: Force convert mic_mute_data from (const uint8_t *) to (uint8_t *).
         *       Avoid discarded-qualifiers build warning.
         */
        USB_Audio_TX_SendData(port, nCount, (uint8_t *)mic_mute_data);
    }
#else
    USB_Audio_TX_SendData(port, nCount, test_5khz_tone);
#endif

#ifdef USB_AUDIO_TX_TIME_MEASURE
    /* Timer2 - Send data */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &usb_tx1_time2_end);
    time_gap1 = usb_tx1_time2_end - usb_tx1_time2_start;

    if(time_gap1 > 500){
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_IsoIn_Hdr send data: %dus > 500us", 1, time_gap1);
    } else {
        LOG_MSGID_I(USB_DEBUG, "USB_Audio_IsoIn_Hdr send data: %dus", 1, time_gap1);
    }
    SLA_CustomLogging("TX1", SA_STOP);
#endif
}

/* EP Iso Out reset handler */
void USB_Audio_IsoIn_Reset(void)
{
    g_UsbAudio[0].txpipe = &g_UsbAudio[0].stream_ep_in_info->ep_status.epout_status;
}
#endif

void USB_Audio_Register_Rx_Callback(uint32_t port, AUDIO_RX_FUNC rx_cb)
{
    if (rx_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Rx_Callback port %d callback:0x%X ", 2, port, rx_cb);
    } else {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Rx_Callback port %d is NULL", 1, port);
    }

    USB_Audio[port].rx_cb = rx_cb;
}

void USB_Audio_Register_Tx_Callback(uint32_t port, AUDIO_TX_FUNC tx_cb)
{
    if (tx_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Tx_Callback port %d callback:0x%X ", 2, port, tx_cb);
    } else {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Tx_Callback port %d is NULL", 1, port);
    }

    USB_Audio[port].tx_cb = tx_cb;
}

void USB_Audio_Register_SetInterface_Callback(uint32_t port, AUDIO_SETINTERFACE_FUNC setinterface_cb)
{
    if (setinterface_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_SetInterface_Callback port:%d callback:0x%X", 2, port, setinterface_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_SetInterface_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].setinterface_cb = setinterface_cb;
}

void USB_Audio_Register_SetSamplingRate_Callback(uint32_t port, AUDIO_SETSAMPLINGRATE_FUNC setsamplingrate_cb)
{
    if (setsamplingrate_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_SetSamplingRate_Callback port:%d callback:0x%X", 2, port, setsamplingrate_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_SetSamplingRate_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].setsamplingrate_cb = setsamplingrate_cb;
}

void USB_Audio_Register_SetSampleSize_Callback(uint32_t port, AUDIO_SETSAMPLESIZE_FUNC setsamplesize_cb)
{
    if (setsamplesize_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_SetSampleSize_Callback port:%d callback:0x%X", 2, port, setsamplesize_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_SetSampleSize_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].setsamplesize_cb = setsamplesize_cb;
}

void USB_Audio_Register_SetChannel_Callback(uint32_t port, AUDIO_SETCHANNEL_FUNC setchannel_cb)
{
    if (setchannel_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_SetChannel_Callback port:%d callback:0x%X", 2, port, setchannel_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_SetChannel_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].setchannel_cb = setchannel_cb;
}

void USB_Audio_Register_Unplug_Callback(uint32_t port, AUDIO_UNPLUG_FUNC unplug_cb)
{
    if (unplug_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Unplug_Callback port:%d callback:0x%X", 2, port, unplug_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Unplug_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].unplug_cb = unplug_cb;
}

void USB_Audio_Register_VolumeChange_Callback(uint32_t port, AUDIO_VOLUMECHANGE_FUNC volumechange_cb)
{
    if (volumechange_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_VolumeChange_Callback port:%d callback:0x%X", 2, port, volumechange_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_VolumeChange_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].volumechange_cb = volumechange_cb;
}

void USB_Audio_Register_Mute_Callback(uint32_t port, AUDIO_MUTE_FUNC mute_cb)
{
    if (mute_cb != NULL) {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Mute_Callback port:%d callback:0x%X", 2, port, mute_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Mute_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].mute_cb = mute_cb;
}

#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
void USB_Audio_Register_Mic_SetInterface_Callback(uint32_t port, AUDIO_SETINTERFACE_FUNC setinterface_cb)
{
    port = 0;

    if(setinterface_cb != NULL){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Mic_SetInterface_Callback port:%d callback:0x%X", 2, port, setinterface_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Mic_SetInterface_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].setinterface_cb_mic = setinterface_cb;
}

void USB_Audio_Register_Mic_SetSamplingRate_Callback(uint32_t port, AUDIO_SETSAMPLINGRATE_FUNC setsamplingrate_cb)
{
    port = 0;

    if(setsamplingrate_cb != NULL){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Mic_SetSamplingRate_Callback port:%d callback:0x%X", 2, port, setsamplingrate_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Mic_SetSamplingRate_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].setsamplingrate_cb_mic = setsamplingrate_cb;
}

void USB_Audio_Register_Mic_SetSampleSize_Callback(uint32_t port, AUDIO_SETSAMPLESIZE_FUNC setsamplesize_cb)
{
    port = 0;

    if(setsamplesize_cb != NULL){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Mic_SetSampleSize_Callback port:%d callback:0x%X", 2, port, setsamplesize_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Mic_SetSampleSize_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].setsamplesize_cb_mic = setsamplesize_cb;
}

void USB_Audio_Register_Mic_SetChannel_Callback(uint32_t port, AUDIO_SETCHANNEL_FUNC setchannel_cb)
{
    port = 0;

    if(setchannel_cb != NULL){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Mic_SetChannel_Callback port:%d callback:0x%X", 2, port, setchannel_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Mic_SetChannel_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].setchannel_cb_mic = setchannel_cb;
}

void USB_Audio_Register_Mic_Unplug_Callback(uint32_t port, AUDIO_UNPLUG_FUNC unplug_cb)
{
    port = 0;

    if(unplug_cb != NULL){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Mic_Unplug_Callback port:%d callback:0x%X", 2, port, unplug_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Mic_Unplug_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].unplug_cb_mic = unplug_cb;
}

void USB_Audio_Register_Mic_VolumeChange_Callback(uint32_t port, AUDIO_VOLUMECHANGE_FUNC volumechange_cb)
{
    port = 0;

    if(volumechange_cb != NULL){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Mic_VolumeChange_Callback port:%d callback:0x%X", 2, port, volumechange_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Mic_VolumeChange_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].volumechange_cb_mic = volumechange_cb;
}

void USB_Audio_Register_Mic_Mute_Callback(uint32_t port, AUDIO_MUTE_FUNC mute_cb)
{
    port = 0;

    if(mute_cb != NULL){
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Register_Mic_Mute_Callback port:%d callback:0x%X", 2, port, mute_cb);
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Register_Mic_Mute_Callback port:%d is NULL", 1, port);
    }

    USB_Audio[port].mute_cb_mic = mute_cb;
}

#endif

uint32_t USB_Audio_Get_Len_Received_Data(uint32_t port)
{
    uint32_t write_temp, read_temp, rc;

    write_temp = g_UsbAudio[port].rx_dma_buffer_write;
    read_temp = g_UsbAudio[port].rx_dma_buffer_read;

    if (write_temp > read_temp) {
        rc = write_temp - read_temp;
    } else if (g_UsbAudio[port].rx_dma_buffer_is_full == 1) {
        rc = g_UsbAudio[port].rx_dma_buffer_len;
    } else if (write_temp < read_temp) {
        rc = g_UsbAudio[port].rx_dma_buffer_len - read_temp + write_temp;
    } else {
        rc = 0;
    }

    return rc;
}

uint32_t USB_Audio_Read_Data(uint32_t port, void *dst, uint32_t len)
{
    uint32_t read_bytes = 0;
    uint32_t data_bytes;

    data_bytes = USB_Audio_Rx_Buffer_Get_Bytes(port);
    if (data_bytes >= len) {
        memcpy(dst,  USB_Audio_Rx_Buffer_Get_Read_Address(port), len);
        USB_Audio_Rx_Buffer_Drop_Bytes(port, len);
        read_bytes = len;
    } else {
        memcpy(dst,  USB_Audio_Rx_Buffer_Get_Read_Address(port), data_bytes);
        USB_Audio_Rx_Buffer_Drop_Bytes(port, data_bytes);
        read_bytes = data_bytes;
        data_bytes = USB_Audio_Rx_Buffer_Get_Bytes(port);
        if (data_bytes >= len - read_bytes) {
            memcpy(dst + read_bytes,  USB_Audio_Rx_Buffer_Get_Read_Address(port), len - read_bytes);
            USB_Audio_Rx_Buffer_Drop_Bytes(port, len - read_bytes);
            read_bytes = len;
        } else {
            memcpy(dst + read_bytes, USB_Audio_Rx_Buffer_Get_Read_Address(port), data_bytes);
            USB_Audio_Rx_Buffer_Drop_Bytes(port, data_bytes);
            read_bytes += data_bytes;
        }
    }

    return read_bytes;
}

/************************************************************
    USB main.c handle functions
************************************************************/
void USB_Audio_Set_Interface(uint32_t msgs)
{
    bool alt2en, usb_tx_rx; /* TX:1 RX:0 */
    uint8_t if_num, alt_num, hb_packet;
    uint32_t port, audio_num, byte;

    if_num = (uint8_t)(((uint32_t)(msgs)) >> 16);
    alt_num = (uint8_t)(((uint32_t)(msgs)) & 0x0000FFFF);

    /* Set TX/RX and port number */
    if(if_num == g_UsbAudio[USB_AUDIO_1_PORT].stream_interface_id){
        port = USB_AUDIO_1_PORT;
        usb_tx_rx = USB_AUDIO_RX;
    } else if(if_num == g_UsbAudio[USB_AUDIO_2_PORT].stream_interface_id){
        port = USB_AUDIO_2_PORT;
        usb_tx_rx = USB_AUDIO_RX;
    } else if (if_num == g_UsbAudio[USB_AUDIO_1_PORT].stream_microphone_interface_id) {
        port = USB_AUDIO_1_PORT;
        usb_tx_rx = USB_AUDIO_TX;
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface if_num:%d is invalid", 1, if_num);
        port = USB_AUDIO_1_PORT;
        usb_tx_rx = USB_AUDIO_RX;
        return;
    }

    audio_num = port + 1; // Port0: Audio1, Port1:Audio2

    //LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface if_id:%d alt:%d port:%d usb_tx_rx:%d", 4,
    //            if_num, alt_num, port, usb_tx_rx);

    /* RX, Speaker */
    if(usb_tx_rx == USB_AUDIO_RX){
        /* Convertor from Alt number to enable/disable */
        alt2en = USB_Audio_RX_SetAlt2Enable(port, alt_num);

        if (alt2en) {
            /* High Band-width mode */
            byte = USB_Audio_Get_RX_Alt_Byte(port, alt_num);

            /* If high-band width mode is enanbled, need to re-init rxmap */
            hb_packet = byte & USB_RXMAP_M1_MASK;
            if(hb_packet){
                hal_usb_init_rxmap(g_UsbAudio[port].stream_ep_out_id, byte);
            }

            /* Convert to byte and packet */
            byte = byte &USB_RXMAP_MAX_PAYLOAD_MASK;

            LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface audio%d speaker enable alt:%d max_byte:%d packet:%d", 4, audio_num, alt_num, byte, hb_packet);
            hal_usb_enable_rx_endpoint(g_UsbAudio[port].stream_ep_out_id, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, false);
            hal_usb_clear_rx_endpoint_fifo(g_UsbAudio[port].stream_ep_out_id, HAL_USB_EP_TRANSFER_ISO, false);

            /* Reset buffer related variables. */
            g_UsbAudio[port].rx_dma_buffer_is_full = 0;
            g_UsbAudio[port].rx_dma_buffer_read = 0;
            g_UsbAudio[port].rx_dma_buffer_write = 0;
        } else {
            LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface audio%d speaker disable", 1, audio_num);
            hal_usb_disable_rx_endpoint(g_UsbAudio[port].stream_ep_out_id);

            /* Reset buffer related variables. */
            g_UsbAudio[port].rx_dma_buffer_is_full = 0;
            g_UsbAudio[port].rx_dma_buffer_read = 0;
            g_UsbAudio[port].rx_dma_buffer_write = 0;
        }
    }

    /* RX, Microphone */
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    if(usb_tx_rx == USB_AUDIO_TX){
        /* Convertor from Alt number to enable/disable */
        alt2en = USB_Audio_TX_SetAlt2Enable(port, alt_num);

        if (alt2en){
            LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface audio%d microphone enable alt:%d", 2, audio_num, alt_num);
            hal_usb_enable_tx_endpoint(g_UsbAudio[port].stream_ep_in_id, HAL_USB_EP_TRANSFER_ISO, HAL_USB_EP_USE_NO_DMA, false);
            hal_usb_clear_tx_endpoint_fifo(g_UsbAudio[port].stream_ep_in_id, HAL_USB_EP_TRANSFER_ISO, false);
            hal_usb_set_endpoint_tx_ready(g_UsbAudio[port].stream_ep_in_id);
        } else {
        LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface audio%d microphone disable", 1, audio_num);
            hal_usb_disable_tx_endpoint(g_UsbAudio[port].stream_ep_in_id);
        }
    }
#endif
}

void USB_Audio_Set_Interface_CB(uint32_t msgs)
{
    bool alt2en, usb_tx_rx; /* TX:1 RX:0 */
    uint8_t if_num, alt_num;
    uint32_t port, audio_num;

    if_num = (uint8_t)(((uint32_t)(msgs)) >> 16);
    alt_num = (uint8_t)(((uint32_t)(msgs)) & 0x0000FFFF);

    /* Set TX/RX and port number */
    if(if_num == g_UsbAudio[0].stream_interface_id){
        port = USB_AUDIO_1_PORT;
        usb_tx_rx = USB_AUDIO_RX;
    } else if(if_num == g_UsbAudio[1].stream_interface_id){
        port = USB_AUDIO_2_PORT;
        usb_tx_rx = USB_AUDIO_RX;
    } else if (if_num == g_UsbAudio[0].stream_microphone_interface_id) {
        port = USB_AUDIO_1_PORT;
        usb_tx_rx = USB_AUDIO_TX;
    } else {
        LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB if_num:%d is invalid", 1, if_num);
        port = USB_AUDIO_1_PORT;
        usb_tx_rx = USB_AUDIO_RX;
        return;
    }

    audio_num = port + 1; // Port0: Audio1, Port1:Audio2

    //LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB if_id:%d alt:%d port:%d usb_tx_rx:%d", 4,
    //            if_num, alt_num, port, usb_tx_rx);

    /* RX, Speaker */
    if(usb_tx_rx == USB_AUDIO_RX){
        /* Convertor from Alt number to enable/disable */
        alt2en = USB_Audio_RX_SetAlt2Enable(port, alt_num);

        if(alt2en == false){
            if (USB_Audio[port].setinterface_cb) {
                LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setinterface_cb:0x%X", 2, audio_num, USB_Audio[port].setinterface_cb);
                USB_Audio[port].setinterface_cb(if_num, alt2en);
            } else {
                LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setinterface_cb is NULL", 1, audio_num);
            }
        }

        /* According to ALT num, set up sample size and channel 
           - Set up setting before enabling speaker
           - Clear setting after disabling speaker
        */
        USB_Audio_Set_RX_Alt(port, alt_num);

        if (USB_Audio[port].setsamplesize_cb) {
            LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setsamplesize_cb:0x%X", 2, audio_num, USB_Audio[port].setsamplesize_cb);
            USB_Audio[port].setsamplesize_cb(g_UsbAudio[port].stream_ep_out_id, USB_Audio_Get_RX_Sample_Size(port));
        } else {
            LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setsamplesize_cb is NULL", 1, audio_num);
        }

        if (USB_Audio[port].setchannel_cb) {
            LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setchannel_cb:0x%X", 2, audio_num, USB_Audio[port].setchannel_cb);
            USB_Audio[port].setchannel_cb(g_UsbAudio[port].stream_ep_out_id, USB_Audio_Get_RX_Channel(port));
        } else {
            LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setchannel_cb is NULL", 1, audio_num);
        }

        if(alt2en == true){
            if (USB_Audio[port].setinterface_cb) {
                LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setinterface_cb:0x%X", 2, audio_num, USB_Audio[port].setinterface_cb);
                USB_Audio[port].setinterface_cb(if_num, alt2en);
            } else {
                LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setinterface_cb is NULL", 1, audio_num);
            }
        }
    }

    /* RX, Microphone */
#ifdef AIR_USB_AUDIO_1_MIC_ENABLE
    if(usb_tx_rx == USB_AUDIO_TX){

        /* Convertor from Alt number to enable/disable */
        alt2en = USB_Audio_TX_SetAlt2Enable(port, alt_num);

        if(alt2en == false) {
            if (USB_Audio[port].setinterface_cb_mic) {
                LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setinterface_cb_mic:0x%X", 2, audio_num, USB_Audio[port].setinterface_cb_mic);
                USB_Audio[port].setinterface_cb_mic(if_num, alt2en);
            } else {
                LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setinterface_cb_mic is NULL", 1, audio_num);
            }
        }

        /* According to ALT num, set up sample size and channel 
           - Set up setting before enabling microphone
           - Clear setting after disabling microphone
        */
        USB_Audio_Set_TX_Alt(port, alt_num);

        if (USB_Audio[port].setsamplesize_cb_mic) {
            LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setsamplesize_cb_mic:0x%X", 2, audio_num, USB_Audio[port].setsamplesize_cb_mic);
            USB_Audio[port].setsamplesize_cb_mic(USB_EP_DIR_IN |g_UsbAudio[port].stream_ep_out_id, USB_Audio_Get_TX_Sample_Size(port));
        } else {
            LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setsamplesize_cb_mic is NULL", 1, audio_num);
        }

        if (USB_Audio[port].setchannel_cb_mic) {
            LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setchannel_cb_mic:0x%X", 2, audio_num, USB_Audio[port].setchannel_cb_mic);
            USB_Audio[port].setchannel_cb_mic(USB_EP_DIR_IN |g_UsbAudio[port].stream_ep_out_id, USB_Audio_Get_TX_Channel(port));
        } else {
            LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setchannel_cb_mic is NULL", 1, audio_num);
        }

        if(alt2en == true) {
            if (USB_Audio[port].setinterface_cb_mic) {
                LOG_MSGID_I(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setinterface_cb_mic:0x%X", 2, audio_num, USB_Audio[port].setinterface_cb_mic);
                USB_Audio[port].setinterface_cb_mic(if_num, alt2en);
            } else {
                LOG_MSGID_E(USBAUDIO_DRV, "USB_Audio_Set_Interface_CB audio%d setinterface_cb_mic is NULL", 1, audio_num);
            }
        }
    }
#endif
}


static int16_t usb_audio_dscr_interface_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_interface_t);

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }

    return size;
}

static int16_t usb_audio_dscr_control_header_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_control_header_t);
    size = size - USB_AUDIO_DSCR_MAX_INTERFACE_NR + ((usb_audio_dscr_control_header_t*)dscr)->bInCollection;

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }

    return size;
}

static int16_t usb_audio_dscr_it_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_it_t);

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }

    return size;
}

static int16_t usb_audio_dscr_ot_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_ot_t);

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }

    return size;
}

static int16_t usb_audio_dscr_mixer_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_mixer_t);
    size = size - USB_DSCR_MIXER_MAX_IN_PINS + ((usb_audio_dscr_mixer_t*)dscr)->bNrInPins;

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }

    return size;
}

static int16_t usb_audio_dscr_selector_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_selector_t);
    size = size - USB_DSCR_SELECTOR_MAX_IN_PINS + ((usb_audio_dscr_selector_t*)dscr)->bNrInPins;

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size - 1 ; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }
    ((uint8_t *)out)[size - 1] = ((usb_audio_dscr_selector_t*)dscr)->iSelector;

    return size;
}

static int16_t usb_audio_dscr_feature_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_feature_t);
    size = size - USB_DSCR_FEATURE_MAX_CONTROLS - 1 + ((usb_audio_dscr_feature_t*)dscr)->control_nr;

    if (size != ((usb_audio_dscr_feature_t*)dscr)->bLength) {
        assert(0);
        return -1;
    }

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size - 1 ; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }
    ((uint8_t *)out)[size - 1] = ((usb_audio_dscr_feature_t*)dscr)->iFeature;

    return size;
}

static int16_t usb_audio_dscr_as_general_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_as_general_t);

    if (size != ((usb_audio_dscr_as_general_t*)dscr)->bLength) {
        assert(0);
        return -1;
    }

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size ; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }

    return size;
}

static int16_t usb_audio_dscr_as_format_type_serialize(void *dscr, void *out, uint16_t ava_size)
{
    uint16_t size = sizeof(usb_audio_dscr_as_format_type_t);
    size -= USB_AUDIO_DSCR_MAX_FREQ_NUM * sizeof(usb_audio_frequency_t);
    size += ((usb_audio_dscr_as_format_type_t*)dscr)->bSamFreqType * sizeof(usb_audio_frequency_t);

    if (size != ((usb_audio_dscr_as_format_type_t*)dscr)->bLength) {
        assert(0);
        return -1;
    }

    if (ava_size < size) {
        assert(0);
        return -1;
    }

    for(int i = 0; i < size ; i++) {
        ((uint8_t *)out)[i] = ((uint8_t *)dscr)[i];
    }

    return size;
}

int16_t dscr_list_serialize(usb_audio_dscr_hdlr_t *dscr_list, uint8_t len, uint8_t *out, uint16_t ava_len)
{
    int16_t tmp;
    int16_t idx = 0;

    for (int i = 0; i < len; i++) {
        tmp = dscr_list[i].serial_func(dscr_list[i].dscr, out + idx, ava_len);
        LOG_MSGID_I(USB_DEBUG, "dscr_list_serialize i[%d] tmp[%d]", 2, i, tmp);

        if (tmp < 0) {
            assert(0);
        }
        idx += tmp;
    }
    return idx;
}

#endif

#endif /* AIR_USB_AUDIO_ENABLE */
