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

#include <string.h>
#include <xtensa/tie/xt_hifi2.h>
#include <xtensa/hal.h>
#include "assert.h"

#include "xt_library_loader.h"
#include "preloader_pisplit.h"
#include "cpd_portable.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "dsp_feature_interface.h"


/***********************************************************************/
SemaphoreHandle_t cpd_load_lib_xSemaphore = NULL;


/***********************************************************************/



extern void cpd_default_function_parameter();
static uint32_t cpd_lib_loaded_counter = 0;

#ifndef MTK_BT_HFP_SPE_ALG_V2
#define CPD_EXPORT_PARAM_NUM 8
void *cpd_decoder_export_parameters[CPD_EXPORT_PARAM_NUM] = {cpd_default_function_parameter, cpd_default_function_parameter, cpd_default_function_parameter, cpd_default_function_parameter, cpd_default_function_parameter, cpd_default_function_parameter, cpd_default_function_parameter, cpd_default_function_parameter};
#else
#define CPD_EXPORT_PARAM_NUM 12
void *cpd_decoder_export_parameters[CPD_EXPORT_PARAM_NUM] = {
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter,
    cpd_default_function_parameter
};
#endif

#if PIC_LOGPRINT
void *cpd_decoder_import_parameters[1] = {printf};//
#else
EXTERN uint32_t fake_printf(const char *format, ...);
void *cpd_decoder_import_parameters[1] = {fake_printf};//
#endif


void cpd_default_function_parameter()
{
    DSP_MW_LOG_E("function point is NULL!!! cpd_decoder library not load or had been unload!!!", 0);
    AUDIO_ASSERT(0);
}


preloader_pisplit_handle_t p_cpd_pisplit_library_handle = NULL;

void cpd_load_library_callback(preloader_pisplit_handle_t handle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t *p_export_parameters;
    p_export_parameters = preloader_pisplit_get_export_parameter(handle, cpd_decoder_import_parameters);

    if (p_export_parameters == NULL) {
       DSP_MW_LOG_E("p_export_parameters is NULL, please check!!!", 0);
    } else {
        for (uint32_t i = 0; i < CPD_EXPORT_PARAM_NUM; i++) {
            cpd_decoder_export_parameters[i] = (void *)p_export_parameters[i];
        }

        xSemaphoreGiveFromISR(cpd_load_lib_xSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

uint32_t cpd_library_load(void *code_address, void *data_address, uint32_t *dram_pic_usage)
{
    if (cpd_load_lib_xSemaphore == NULL) {
        cpd_load_lib_xSemaphore = xSemaphoreCreateBinary();

        if (cpd_load_lib_xSemaphore == NULL) {
            AUDIO_ASSERT(0);
        }
    }
    if (p_cpd_pisplit_library_handle == NULL) {
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_get_handle(&p_cpd_pisplit_library_handle, &pisplit_cpd, cpd_load_library_callback)) {
            DSP_MW_LOG_E("pisplit_cpd preloader_pisplit_get_handle() error!!!!", 0);
            AUDIO_ASSERT(0);
        }
    }

    if(cpd_lib_loaded_counter == 0) {
#ifdef AIR_DSP_MEMORY_REGION_ENABLE
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load_v1(p_cpd_pisplit_library_handle, code_address, data_address))
#else
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_load(p_cpd_pisplit_library_handle, PRELOADER_I_HIGH_PERFORMANCE, PRELOADER_D_HIGH_PERFORMANCE))
#endif
        {
            DSP_MW_LOG_E("pisplit_cpd preloader_pisplit_load() error!!!!", 0);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_I_LOW_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_HIGH_PERFORMANCE);
            preloader_pisplit_dump_pic_memory_heap_info(PRELOADER_D_LOW_PERFORMANCE);
            AUDIO_ASSERT(0);
        }
        if (xSemaphoreTake(cpd_load_lib_xSemaphore, portMAX_DELAY) == pdTRUE) {

        } else {
            AUDIO_ASSERT(0);
        }
    }
    vSemaphoreDelete(cpd_load_lib_xSemaphore);
    cpd_load_lib_xSemaphore = NULL;
    cpd_lib_loaded_counter ++;
#ifdef AIR_DSP_MEMORY_REGION_ENABLE
    preloader_pisplit_get_library_code_and_data_size(p_cpd_pisplit_library_handle, NULL, dram_pic_usage);
#else
    UNUSED(code_address);
    UNUSED(data_address);
    UNUSED(dram_pic_usage);
#endif
    return cpd_lib_loaded_counter;
}

uint32_t cpd_library_unload()
{
    if (cpd_lib_loaded_counter == 1) {
#ifdef AIR_DSP_MEMORY_REGION_ENABLE
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload_v1(p_cpd_pisplit_library_handle))
#else
        if(PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_unload(p_cpd_pisplit_library_handle))
#endif
        {
            DSP_MW_LOG_E("pisplit_cpd preloader_pisplit_unload() error!!!!", 0);
            AUDIO_ASSERT(0);
        }
        if (PRELOADER_PISPLIT_XTLIB_NO_ERR != preloader_pisplit_release_handle(p_cpd_pisplit_library_handle)) {
            DSP_MW_LOG_E("pisplit_cpd preloader_pisplit_release_handle() error!!!!", 0);
            AUDIO_ASSERT(0);
        }

        for (uint32_t i = 0; i < CPD_EXPORT_PARAM_NUM; i++) {
            cpd_decoder_export_parameters[i] = cpd_default_function_parameter;
        }
        p_cpd_pisplit_library_handle = NULL;
    }
    cpd_lib_loaded_counter -- ;
    return cpd_lib_loaded_counter;
}


