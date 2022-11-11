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

#ifndef __NVDM_ROM_BOOK_H__
#define __NVDM_ROM_BOOK_H__

const uint8_t T_NVDM_BEEF[] = {
    0xA5, 0xFF,
};

// g_nvdm_default_table table with nvdm ID, nvdm length and nvdm memory pointer.
const mem_nvdm_info_t g_nvdm_default_table[] = {
#if defined(AIR_3RD_PARTY_NR_ENABLE)
    {0xE10A, 512, NULL },  //sizeof(T_NVDM_E10A)
    {0xE10B, 512, NULL },  //sizeof(T_NVDM_E10B)
#endif
    {0xE101, 264, NULL },  //sizeof(T_NVDM_E101)
    {0xE103, 264, NULL },  //sizeof(T_NVDM_E103)
    {0xE104, 264, NULL },  //sizeof(T_NVDM_E104)
    {0xE105, 264, NULL },  //sizeof(T_NVDM_E105)
    {0xE106, 264, NULL },  //sizeof(T_NVDM_E106)
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE) || defined(AIR_WIRELESS_MIC_RX_ENABLE)
    {0xE441, 944, NULL },  //sizeof(T_NVDM_E441)
    {0xE500, 944, NULL },  //sizeof(T_NVDM_E500)
#endif
    {0xE501, 944, NULL },  //sizeof(T_NVDM_E501)
    {0xE502, 944, NULL },  //sizeof(T_NVDM_E502)
    {0xE503, 944, NULL },  //sizeof(T_NVDM_E503)
    {0xE4C0,  20, NULL },  //sizeof(T_NVDM_E4C0)
#ifdef MTK_LINEIN_INS_ENABLE
    {0xE4C1,  44, NULL },  //sizeof(T_NVDM_E4C1)
#endif
    {0xE4C2,  20, NULL },  //sizeof(T_NVDM_E4C2)
    {0xE140,  46, NULL },  //sizeof(T_NVDM_E140)
    {0xE150, 984, NULL },  //sizeof(T_NVDM_E150)
    {0xE151, 270, NULL },  //sizeof(T_NVDM_E151)
    {0xE152, 180, NULL },  //sizeof(T_NVDM_E152)
    {0xE153, 180, NULL },  //sizeof(T_NVDM_E153)
    {0xE154, 360, NULL },  //sizeof(T_NVDM_E154)
    {0xE155, 460, NULL },  //sizeof(T_NVDM_E155)
    {0xE156, 270, NULL },  //sizeof(T_NVDM_E156)
    {0xE161, 498, NULL },  //sizeof(T_NVDM_E161)
    {0xE162, 498, NULL },  //sizeof(T_NVDM_E162)
    {0xE163, 242, NULL },  //sizeof(T_NVDM_E163)
    {0xE164, 242, NULL },  //sizeof(T_NVDM_E164)
#if defined(AIR_3RD_PARTY_NR_ENABLE)
    {0xE174, 498, NULL },  //share with sizeof(T_NVDM_E162)
#endif
    {0xE190,  76, NULL },  //sizeof(T_NVDM_E190)
    {0xE191,  76, NULL },  //sizeof(T_NVDM_E191)
    {0xE192,  76, NULL },  //sizeof(T_NVDM_E192)
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    {0xE193, 984, NULL },
    {0xE194, 242, NULL },
    {0xE195, 498, NULL },
    {0xE196, 264, NULL },
    {0xE197, 184, NULL },
    {0xE198, 258, NULL },
    {0xE199, 514, NULL },
    {0xE19C, 264, NULL },
    {0xE19D, 264, NULL },
    {0xE19F, 184, NULL },
    {0xE1A0, 184, NULL },
#endif
    {0xE19B,  76, NULL },
#if defined(AIR_ECNR_1MIC_INEAR_ENABLE) || defined(AIR_ECNR_2MIC_INEAR_ENABLE)
    {0xE167, 498, NULL },  //sizeof(T_NVDM_E167)
#endif
    {0xE168, 130, NULL },  //sizeof(T_NVDM_E168)
#if defined(AIR_3RD_PARTY_NR_ENABLE)
    {0xE169, 184, NULL },  //sizeof(T_NVDM_E169)
    {0xE16A, 184, NULL },  //sizeof(T_NVDM_E16A)
#endif
    {0xE16C, 770, NULL },  //sizeof(T_NVDM_E16C)
    {0xE16D, 770, NULL },  //sizeof(T_NVDM_E16D)
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    {0xE3A0, 400, NULL },
    {0xE3A1,   4, NULL },
    {0xE3A2, 400, NULL },
    {0xE3A3, 400, NULL },
    {0xE3A4,  96, NULL },
    {0xE3A5, 960, NULL },
    {0xE3A6,  12, NULL },
#endif
#if defined(AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE)
    {0xE450, 148, NULL },
#endif
#if defined(AIR_SILENCE_DETECTION_ENABLE)
    {0xE711, 148, NULL },
#endif
    {0xBEEF, sizeof(T_NVDM_BEEF), &T_NVDM_BEEF },
    {0xFFFF,                   0,         NULL } // NVKEY_END
};

uint32_t DSP0_NVDM_ITEM_MAX = (sizeof(g_nvdm_default_table) / sizeof(mem_nvdm_info_t));

#endif  /* __NVDM_ROM_BOOK_H__ */
