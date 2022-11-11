# Copyright Statement:
#
# (C) 2021  Airoha Technology Corp. All rights reserved.
#
# This software/firmware and related documentation ("Airoha Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
# Without the prior written permission of Airoha and/or its licensors,
# any reproduction, modification, use or disclosure of Airoha Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
# You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
# if you have agreed to and been bound by the applicable license agreement with
# Airoha ("License Agreement") and been granted explicit permission to do so within
# the License Agreement ("Permitted User").  If you are not a Permitted User,
# please cease any access or use of Airoha Software immediately.
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
# ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
# WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
# AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
#

IC_CONFIG                             = ab156x
BOARD_CONFIG                          = ab156x_evb
IC_TYPE                               = ab1565
BOARD_TYPE                            = ab1565_evk_ull2
BL_FEATURE                            = feature_65_evb.mk

##############################################
#           Custom feature option            #
##############################################

## part1: configure y/n

###### Audio peripheral ######
# This option is to enable LINE_IN PEQ feature.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_LINE_IN_PEQ_ENABLE               = n

###### Audio/Voice effects ######

# This option is to enable PEQ feature.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_PEQ_ENABLE                      = n

######  BT/MCSync ######

# This option is to enable/disable LE Audio.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_LE_AUDIO_ENABLE = n

# This option is used to support LE Audio broadcast.
# Dependency: AIR_LE_AUDIO_ENABLE must be enabled when this option is set to y.
AIR_LE_AUDIO_BIS_ENABLE = n

# This option is to enable LE Audio dongle scenario.
# Dependency: AIR_LE_AUDIO_ENABLE must be enabled when this option is set to y.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_LE_AUDIO_DONGLE_ENABLE = n

# This option is to enable LE Audio dongle multi-device scenario.
# Dependency: AIR_LE_AUDIO_DONGLE_ENABLE must be enabled when this option is set to y.
AIR_LE_AUDIO_MULTI_DEVICE_ENABLE = n

# This option is used to enable teams.
# Dependency: AIR_USB_AUDIO_HID_ENABLE must be enabled when this option is set to y.
AIR_MS_TEAMS_ENABLE = n

# This option is to enable MS XBOX GIP feature.
# Dependency: AIR_BLE_ULTRA_LOW_LATENCY_ENABLE must be enabled when this option is set to y.
AIR_MS_GIP_ENABLE = n

######  System ######

# This option is used to enable FOTA basic function.
AIR_FOTA_ENABLE                     = y

# This option is to enable FOTA via race cmd.
# Dependency: AIR_FOTA_ENABLE, AIR_RACE_CMD_ENABLE and AIR_RACE_DUAL_CMD_ENABLE (if it is an earbuds project) must be enabled when this option is set to y.
AIR_FOTA_VIA_RACE_CMD_ENABLE = y

# This option is used to enable component firmware update module.
# Dependency: AIR_USB_AUDIO_HID_ENABLE, AIR_LE_AUDIO_ENABLE, AIR_RACE_CMD_ENABLE, AIR_FOTA_ENABLE must enable, and AIR_USB_AUDIO_VERSION cannot be none.
AIR_CFU_ENABLE = n

# This option is to enable USB module.
AIR_USB_ENABLE                        = y

# This option is to use USB TX as usage of microphone.
# Dependency: AIR_USB_ENABLE  must enable, and AIR_USB_AUDIO_VERSION cannot be none.
AIR_USB_AUDIO_MICROPHONE_ENABLE       = y

# This option is to add more one USB RX to enable second speaker.
# Dependency: AIR_USB_ENABLE  must enable, and AIR_USB_AUDIO_VERSION cannot be none.
AIR_USB_AUDIO_2ND_SPEAKER_ENABLE      = n

# This option is used to support HID (Human Interface Device).
# Dependency: AIR_USB_ENABLE  must enable.
AIR_USB_AUDIO_HID_ENABLE              = y

# This option is used to support XBOX.
# Dependency: AIR_USB_ENABLE must be enabled when this option is set to y.
AIR_USB_XBOX_ENABLE                   = n

# This option is used to enable race cmd.
AIR_RACE_CMD_ENABLE                 = y

# This option is to enable minidump feature.
AIR_MINIDUMP_ENABLE                 = n

# This option is to enable audio dump for CM4 side debug use.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_AUDIO_DUMP_ENABLE               = y

# This option is used to enable/disable key driver.
AIR_AIRO_KEY_ENABLE = y

# This option is used to enable/disable eint key module.
AIR_EINT_KEY_ENABLE                    = y

# This option is used to enable/disable HWSRC CLK SKEW.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_HWSRC_CLKSKEW_ENABLE = n

# This option is used to enable/disable secure boot module.
# NOTICE:
#   - This option can only be enabled if the secure-boot-addon-repo exists.
#   - secure-boot-addon-repo path: mcu\prebuilt\middleware\airoha\secure_boot
#   - If this option enabled without secure boot addon repo, the code will build fail.
AIR_SECURITY_SECURE_BOOT_ENABLE = n

# This option is used to enable/disable ultra low latency version1.0 module.
AIR_BT_ULTRA_LOW_LATENCY_ENABLE = y

# This option is used to enable/disable ultra low latency version2.0 module.
AIR_BLE_ULTRA_LOW_LATENCY_ENABLE = n

# This option is to enable ultra low latency version1.0 dongle scenario.
# Dependency: AIR_ULL_GAMING_DONGLE_ENABLE must be enabled when this option is set to y.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_ULL_GAMING_DONGLE_ENABLE = y

# This option is used to enable/disable LC3PLUS codec support. It should be disable by default.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_AUDIO_LC3PLUS_CODEC_ENABLE = n

# This option is used to enable/disable vendor codec support. It should be disable by default.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_AUDIO_VEND_CODEC_ENABLE = n

# This option is used to enable/disable silence detection feature. It should be disable by default.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_AUDIO_SILENCE_DETECTION_ENABLE = n

# This option is to enable ultra low latency version2.0 dongle scenario.
# Dependency: AIR_ULL_AUDIO_V2_DONGLE_ENABLE must be enabled when this option is set to y.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_ULL_AUDIO_V2_DONGLE_ENABLE = n

# This option is used to reduce game audio when chat audio existed.
# It must be set to the same value for both DSP and MCU, otherwise, it will not work.
# Dependency: AIR_BT_ULTRA_LOW_LATENCY_ENABLE be enabled.
AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE = y

# This option is used to enable ULL voice uplink low latency (AFE buffering 7.5ms)
# Dependency: AIR_BT_ULTRA_LOW_LATENCY_ENABLE be enabled.
AIR_ULL_VOICE_LOW_LATENCY_ENABLE = y

# This option is to enable wireless microphone receiver scenario.
# Dependency: AIR_BLE_ULTRA_LOW_LATENCY_ENABLE be enabled.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work.
AIR_WIRELESS_MIC_ENABLE = n

# This option is to enable wired audio in dongle side, support line in / usb in.
# It must be turned on/off for both DSP and MCU side, otherwise, it will not work.
AIR_WIRED_AUDIO_ENABLE = n

# This option is used to enable usb audio in.
# It must be turned on/off for both DSP and MCU, otherwise, it will not work. No support for earbuds project.
# Dependency: Dongle USB in will be disable when this option is set to y.
AIR_USB_AUDIO_IN_ENABLE = n

# This option is used to enable usb audio in(wired audio).
# It must be turned on/off for both DSP and MCU, otherwise, it will not work. No support for earbuds project.
AIR_USB_AUDIO_IN_MIX_ENABLE = n

# This option is used to enable line in (wired audio).
# It must be turned on/off for both DSP and MCU, otherwise, it will not work. No support for earbuds project.
# Dependency: AIR_DONGLE_AFE_IN_TYPE can't select line in when this option is set to y.
AIR_LINE_IN_ENABLE = n

# This option is used to enable line in (wired audio).
# It must be turned on/off for both DSP and MCU, otherwise, it will not work. No support for earbuds project.
AIR_LINE_IN_MIX_ENABLE = n

## part2: need to set specified value

######  System ######

# Using specific linker script during linking process.
AIR_MCU_LINKER_SCRIPT_FILE = ab156x_flash.ld


# This option is to configure mbedTLS features.
AIR_MBEDTLS_CONFIG_FILE = config-vendor-fota-race-cmd.h

# This option is to configure system log debug level.
# debug level: none, error, warning, info and debug
#              empty   : All debug logs are compiled.
#              error   : Only error logs are compiled.
#              warning : Only warning and error logs are compiled.
#              info    : Only info, warning, and error logs are compiled.
#              debug   : All debug logs are compiled.
#              none    : All debugs are disabled.
AIR_DEBUG_LEVEL                     = info

# Choose the version of UAC (USB Audio Class). Default setting is version 1.
# Dependency: AIR_USB_ENABLE must be enabled when this option is set to v1.
# USB Audio Version : v1, none
#                     none  : means disable USB Audio.
AIR_USB_AUDIO_VERSION                 = v1


######  Audio ######

# This option is to choose the uplink rate. Default setting is none.
# It must be set to the same value for both DSP and MCU, otherwise, it will not work.
# Up Link Rate : none, 48k
#                none : uplink rate will be handled by scenario itself.
#                48k  : uplink rate will be fixed in 48k Hz.
AIR_UPLINK_RATE   = none

# This option is to fix the uplink DMA resolution. Default setting is none.
# It must be set to the same value for both DSP and MCU, otherwise, it will not work.
# Up Link Resolution : none, 32bit
#                      none : uplink resolution will be handled by scenario itself.
#                      32bit: uplink resolution will be fixed at 32-bit.
AIR_UPLINK_RESOLUTION   = none

# This option is to choose the downlink rate. Default setting is none.
# It must be set to the same value for both DSP and MCU, otherwise, it will not work.
# Down Link Rate : none, 48k, 96k
#                  none  : downlink rate will be handled by scenario itself.
#                  48k   : downlink rate will be fixed in 48k Hz.
#                  96k   : downlink rate will be fixed in 96k Hz.
AIR_DOWNLINK_RATE = none

# This option is to choose the type of ECNR. Default setting is none.
# It must be set to the same value for both DSP and MCU, otherwise, it will not work.
#                  none             : disable ECNR
#                  ECNR_1_OR_2_MIC  : Inhouse ECNR to support 1 or 2 MIC.
#                  ECNR_1MIC_INEAR  : Inhouse ECNR to support 1 + 1 MIC.
#                  ECNR_2MIC_INEAR  : Inhouse ECNR to support 1 + 2 MIC.
#                  3RD_PARTY_AI_NR  : 3rd party AINR to support 1/2 MIC.
#                  3RD_PARTY_AI_NR_OFFLOAD  : 3rd party AINR to support 1 MIC with offload.
#                  3RD_PARTY_AI_NR_OFFLOAD_POST_EC  : 3rd party AINR to support 1 MIC with offload(dongle).
#                  3RD_PARTY_AI_NR_PRO_BROADSIDE_SEPARATE_MODE  : 3rd party AINR to support 1 MIC with separate EC/NR_PostEC.
#                  3RD_PARTY_AI_NR_SEPARATE_MODE_EC  : 3rd party AINR to support 1 MIC with separate EC only.
#                  3RD_PARTY_AI_NR_INEAR : 3rd party AINR to support 1/2 + 1 MIC.
#                  3RD_PARTY_AI_NR_SHORT_BOOM_OO : 3rd party AINR to support short boom mic for open office.
#                  3RD_PARTY_AI_NR_PRO_DISTRACTOR : 3rd party AINR to support pro distractor.
#                  3RD_PARTY_AI_NR_PRO_TWS_OO : 3rd party AINR to support pro tws for open office.
#                  3RD_PARTY_CUSTOMIZED : Customized NR.
#                  LD_NR : Inhouse LDNR(Low Delay NR)to support 1/2 MIC.
AIR_ECNR_CONFIG_TYPE = none

# This option is to choose the type of dongle afe in type. Default setting is line in + i2s slv in.
# It must be set to the same value for both DSP and MCU, otherwise, it will not work.
#                  NONE                : not support afe in.
#                  LINE_IN             : Only support line in.
#                  I2S_MST_IN          : Only support i2s master in.
#                  I2S_SLV_IN          : Only support i2s slave in.
#                  LINE_IN_I2S_MST_IN  : Support line in and i2s master in, but cann't playback at the same time.
#                  LINE_IN_I2S_SLV_IN  : Support line in and i2s slave in, but cann't playback at the same time.
AIR_DONGLE_AFE_IN_TYPE = NONE

# This option is to choose the type of dongle afe out type. Default setting is line out.
# It must be set to the same value for both DSP and MCU, otherwise, it will not work.
#                  NONE                : not support afe out.
#                  LINE_OUT             : Only support line out.
#                  I2S_MST_OUT          : Only support i2s master out.
#                  I2S_SLV_OUT          : Only support i2s slave out.
#                  LINE_OUT_I2S_MST_OUT  : Support line out and i2s master out at the same time.
#                  LINE_OUT_I2S_SLV_OUT  : Support line out and i2s slave out at the same time.
AIR_DONGLE_AFE_OUT_TYPE = NONE

# This option is to define product category.
AIR_PRODUCT_CATEGORY = AB1565_ULL2_DONGLE
