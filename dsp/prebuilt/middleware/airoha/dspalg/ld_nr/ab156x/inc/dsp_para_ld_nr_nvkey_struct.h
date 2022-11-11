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
#ifndef _DSP_PARA_LD_H_
#define _DSP_PARA_LD_H_

#include "types.h"

/**
 * @brief Parameter for DSP WB RX EQ
 * @KeyID 0xE1XX
 */

#ifdef WIN32
#pragma pack(1)
#endif

typedef struct stru_dsp_at_ld_para_s {
    U8  ENABLE;                     /**< @Value 0x00       @Desc 1 */
    U8  REVISION;                   /**< @Value 0x00       @Desc 1 */
    U16 padding1;                   /**< @Value 0x0000     @Desc 1 */

    S16 sup_par_type;               /**< @Value 0x0002     @Desc 1 */
    S16 sup_par_alpha;              /**< @Value 0x799A     @Desc 1 */
    S16 sup_par_alpha_1;            /**< @Value 0x0665     @Desc 1 */
    S16 sup_par_gain_limiter;       /**< @Value 0x101D     @Desc 1 */

    S16 ld_alpha_peak_up;           /**< @Value 0x747B     @Desc 1 */
    S16 ld_alpha_peak_dn;           /**< @Value 0x7C29     @Desc 1 */
    S16 ld_alpha_valley_dn;         /**< @Value 0x747B     @Desc 1 */
    S16 ld_alpha_valley_up;         /**< @Value 0x7C29     @Desc 1 */
    S16 ld_pow_diff_thr;            /**< @Value 0x0348     @Desc 1 */
    S16 ld_pow_credit;              /**< @Value 0x03E8     @Desc 1 */
    S16 ld_alpha_sm_fast;           /**< @Value 0x4CCD     @Desc 1 */
    S16 ld_alpha_sm_slow;           /**< @Value 0x6CCD     @Desc 1 */
    S16 ld_noise_update_thr0;       /**< @Value 0x0A80     @Desc 1 */
    S16 ld_noise_update_thr1;       /**< @Value 0x0780     @Desc 1 */
    S16 ld_noise_update_thr2;       /**< @Value 0x0300     @Desc 1 */
    S16 ld_min_period;              /**< @Value 0x000D     @Desc 1 */
    S32 ld_noise_alpha_fast;        /**< @Value 0x66666600 @Desc 1 */
    S32 ld_noise_alpha_slow;        /**< @Value 0x7c28f600 @Desc 1 */
    S32 ld_noise_alpha_ultra_slow;  /**< @Value 0x7f7cee00 @Desc 1 */
    S32 ld_over_est;                /**< @Value 0x19999a00 @Desc 1 */
    S32 ld_noise_floor;             /**< @Value 0x00001000 @Desc 1 */
    S32 pitch_thr;                  /**< @Value 0x003C0000 @Desc 1 */
    S32 pitch_thr2;                 /**< @Value 0x00230000 @Desc 1 */
    S16 pitch_idx_st;               /**< @Value         10 @Desc 1 */
    S16 pitch_idx_end;              /**< @Value        200 @Desc 1 */
    S16 max_dis;                    /**< @Value         10 @Desc 1 */
    S16 pitch_bin_end;              /**< @Value        256 @Desc 1 */
    S16 pitch_en;                   /**< @Value     0x0000 @Desc 1 */
    S16 Pitch_Structure_Reserve;    /**< @Value     0x0000 @Desc 1 */
    //Reserve
    U32 RESERVE_1;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_2;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_3;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_4;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_5;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_6;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_7;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_8;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_9;                  /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_10;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_11;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_12;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_13;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_14;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_15;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_16;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_17;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_18;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_19;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_20;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_21;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_22;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_23;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_24;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_25;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_26;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_27;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_28;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_29;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_30;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_31;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_32;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_33;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_34;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_35;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_36;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_37;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_38;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_39;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_40;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_41;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_42;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_43;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_44;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_45;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_46;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_47;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_48;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_49;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_50;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_51;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_52;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_53;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_54;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_55;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_56;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_57;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_58;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_59;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_60;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_61;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_62;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_63;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_64;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_65;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_66;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_67;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_68;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_69;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_70;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_71;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_72;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_73;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_74;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_75;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_76;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_77;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_78;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_79;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_80;                 /**< @Value 0x00000000 @Desc 1 */
    U32 RESERVE_81;                 /**< @Value 0x00000000 @Desc 1 */
} PACKED DSP_PARA_AT_LD_STRU;

#ifdef WIN32
#pragma pack()
#endif

#endif /* _DSP_PARA_LD_H_ */
