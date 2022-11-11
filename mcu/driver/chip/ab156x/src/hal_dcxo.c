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

#include <stdio.h>
#include <stdlib.h>
#include "hal_feature_config.h"
#include "hal.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#include "nvkey_id_list.h"
#include "nvkey.h"
#endif
//#define DEBUG_MONITOR ((volatile uint32_t*) 0xA2010008)
#ifdef HAL_DCXO_MODULE_ENABLED
#include "hal_clock_internal.h"
#define __EOSC_32K__
/* definition for lock/force DCXO to FPM during CAPID calibration (for G2 chips)
 * lock FPM: in set_capid_rg()
 * to unlock: reset chip for normal dcxo mode control
 */
#define CALIBRATE_CAPID_LOCK_FPM
unsigned int dcxo_capid = 0;

#if !defined(__EXT_BOOTLOADER__) && !defined(__EXT_DA__)
#include "dcxo_capid.h"  /* function only defined in mainbin */
#endif
#define CAPID_MASK    0x1FF    /*  Dcxo capid range : 0 ~ 511. */

typedef struct {
    uint8_t trim_spec;
    uint16_t cap_value;
    uint8_t reserved;
} __attribute__((packed)) xo_info_t;


#if defined(CALIBRATE_CAPID_LOCK_FPM)
/* currently designed for only single user (1: locked, 0: not locked) */
static uint8_t dcxo_lock_fpm = 0;
#endif

void set_capid()
{
    if(dcxo_capid){
        DCXO_PCON_REG.DCXO_CAPID_EFUSE = dcxo_capid;
        DCXO_PCON_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
        log_hal_msgid_info("Load CAPID done, CAPID RG = 0x%x\r\n",1, dcxo_capid);
    }
}

uint32_t get_capid(){
    return dcxo_capid;
}

ATTR_TEXT_IN_TCM void dcxo_lp_mode(dcxo_mode_t mode){
#if defined(CALIBRATE_CAPID_LOCK_FPM)
    if (dcxo_lock_fpm) {
        log_hal_msgid_info("dcxo_lp_mode(), DCXO FPM lock enabled (reset system to unlock)\r\n", 0);
        /* during capid calibration, dcxo is locked to fpm (override request) */
        mode = DCXO_NORMAL_MODE;
    }
#endif

    if(mode == DCXO_LP_MODE){
        *DCXO_CTUNE_LPM = DCXO_PCON_REG.DCXO_CAPID_EFUSE;
        DCXO_CORE_ISEL_LPM_REG.ISEL_LPM = *DCXO_CORE_ISEL;
    }
    DCXO_PCON_REG.EN_26M_FPM = mode;
    DCXO_PCON_REG.BT_26M_EN = mode;
}

uint8_t __get_dcxo_26m_fpm(){
    return DCXO_PCON_REG.EN_26M_FPM;
}

uint8_t __get_dcxo_26m_en(){
    return DCXO_PCON_REG.BT_26M_EN ;
}

/* API that returns current dcxo mode */
dcxo_mode_t dcxo_current_mode(void)
{
    if (DCXO_PCON_REG.EN_26M_FPM == 0 && DCXO_PCON_REG.BT_26M_EN == 0) {
        return DCXO_LP_MODE;
    } else {
        return DCXO_NORMAL_MODE;
    }
}

void hal_dcxo_init(void)
{
#if !defined(__EXT_BOOTLOADER__) && !defined(__EXT_DA__)
    dcxo_load_capid(); /* function only defined in mainbin */
#endif

    set_capid();
    DCXO_PCON_REG.GSM_DCXO_CTL_EN=0x1;
    DCXO_PCON_REG.EXT_DCXO_CTL_EN = 0x1;
#ifdef __EOSC_32K__
    DCXO_PCON_REG.DCXO_PWR_EN_TD = 0x1;
    DCXO_PCON_REG.DCXO_EN_TD=0x1;
    DCXO_PCON_REG.DCXO_BUF_EN_TD=0x52;
#else //32kless
    DCXO_PCON_REG.DCXO32K_EN = 0x1;
    DCXO_PCON_REG.DCXO_PWR_EN_TD = 0x1;
    DCXO_PCON_REG.DCXO_EN_TD = 0x1;
    DCXO_PCON_REG.DCXO_BUF_EN_TD=0x14;

#endif
    DCXO_PCON_REG.DCXO_ISO_EN_TD=0x6;
    DCXO_PCON_REG.DCXO_SLEEP_TD=0xA;

    /* Since dcxo_init() is currently the last clock-related init
     * -> call system freq debug log
     */
    hal_clock_get_freq_meter(FQMTR_SYS_DBG, 1000);
}

/* common API to set FPM capid RG */
/* for G2 chips, this API is only used during CAPID calibration
 * it will currently lock DCXO to FPM
 * (reset chip for normal dcxo mode control)
 */
void set_capid_rg(uint32_t capid_val)
{
#if defined(CALIBRATE_CAPID_LOCK_FPM)
    /* lock and set DCXO to FPM (reset system to unlock) */
    log_hal_msgid_info("DCXO FPM lock set\r\n", 0);
    dcxo_lock_fpm = 1;
    dcxo_lp_mode(DCXO_NORMAL_MODE);
#endif

    DCXO_PCON_REG.DCXO_CAPID_EFUSE = capid_val;
    DCXO_PCON_REG.DCXO_CAPID_EFUSE_SEL = 0x1;
    log_hal_msgid_info("[DCXO] Load FPM CAPID done, CAPID RG = 0x%x\r\n", 1, DCXO_PCON_REG.DCXO_CAPID_EFUSE);

}

/* common API to get FPM capid RG (returns capid value) */
uint32_t get_capid_rg(void)
{
    return DCXO_PCON_REG.DCXO_CAPID_EFUSE;
}

/* common API to set capid value stored in NVDM (returns nvkey write api returned status) */
int set_capid_nvdm(uint32_t capid_val)
{
    int result = 0;

    capid_val = capid_val & CAPID_MASK;
#ifdef MTK_NVDM_ENABLE
    uint8_t nvkey[4] = {0};
    uint32_t size = sizeof(xo_info_t);

    if (((nvkey_status_t)nvkey_read_data(NVID_CAL_XO_26M_CRTSTAL_TRIM, (uint8_t *)nvkey, &size)) == NVKEY_STATUS_OK) {
        (((xo_info_t *)nvkey) -> cap_value) = capid_val;
        result = nvkey_write_data(NVID_CAL_XO_26M_CRTSTAL_TRIM, (uint8_t *)nvkey, size);
        LOG_MSGID_I(common, "CAPID item in NVDM(nvkey) is set(0x%x).\r\n", 1, (((xo_info_t *)nvkey) -> cap_value));
    } else {
        LOG_MSGID_E(common, "CAPID item in  NVDM(nvkey) is empty, use default value.\r\n", 0);
    }
#endif
    return result;
}


/* common API to get capid value stored in NVDM (returns retrieved capid value) */
uint32_t get_capid_nvdm(void)
{
    uint32_t dcxo_capid = 0;

#ifdef MTK_NVDM_ENABLE
    uint32_t size = 0;
    uint8_t nvkey[4] = {0};
    size = sizeof(xo_info_t);
    if (nvkey_read_data(NVID_CAL_XO_26M_CRTSTAL_TRIM, nvkey, &size) == NVKEY_STATUS_OK) {
        dcxo_capid = (((xo_info_t *)nvkey) -> cap_value) & CAPID_MASK;
        LOG_MSGID_I(common, "CAPID item in NVDM(nvkey) is available(0x%x).\r\n", 1, dcxo_capid);
    } else {
        LOG_MSGID_E(common, "CAPID item in  NVDM(nvkey) is empty, use default value.\r\n", 0);
    }
#endif

    return dcxo_capid;
}


#endif /*HAL_DCXO_MODULE_ENABLED*/
