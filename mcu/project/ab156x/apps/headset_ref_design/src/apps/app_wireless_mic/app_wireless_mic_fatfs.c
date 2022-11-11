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

#ifdef AIR_WIRELESS_MIC_ENABLE

#include "app_wireless_mic_fatfs.h"
#include "app_wireless_mic_idle_activity.h"
#include "apps_config_event_list.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_events_key_event.h"
#include "apps_debug.h"

#include "bt_sink_srv_ami.h"

#include "hal_gpio.h"
#include "hal_gpt.h"
#include "hal_platform.h"
#include "hal_audio_message_struct_common.h"

#include "memory_attribute.h"

#ifdef MTK_NVDM_ENABLE
#include "nvkey.h"
#include "nvkey_id_list.h"
#endif

#define LOG_TAG "[APP_WIRELESS_MIC][FATFS]"

#define APP_DEFAULT_MAX_FILE_LEN            (_MAX_LFN + 1)
#define APP_DEFAULT_FS                      _T("SD:")
#define APP_DEFAULT_DIR                     _T("SD:/recorder")
#define APP_DEFAULT_DIR_SEP                 _T("SD:/recorder/")
#define APP_WAV_DATA_CHACHE_MAX_LENGTH      (4*1024) //4k

char filename[sizeof("SD:/recorder/xxxx.pcm")];

FIL fdst;                        /* file target which must be a global variable name if it would be accessed with global scope. */
FATFS fatfs;                     /* fs target which must be a global variable name if it would be accessed with global scope. */
BYTE test_work[1024];

typedef struct {
    FATFS                              *fatfs;
    FIL                                 file;
    DWORD                               total_size;
    bool                                is_file_open;
    bool                                is_create_file;
    uint32_t                            wav_size;
    uint32_t                            file_num;
    app_wm_wav_header_t                 wav_header;
    uint8_t                            *wav_cache_data;
    uint32_t                            wav_cache_size;
} app_wireless_mic_fatfs_t;

ATTR_ZIDATA_IN_NONCACHED_RAM static app_wireless_mic_fatfs_t s_app_wm_fs_context;
bool g_wireless_mic_fs_init_done = FALSE;

#define  WAV_HEADER_LENGTH   sizeof(app_wm_wav_header_t)
ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t s_wav_header[WAV_HEADER_LENGTH] = {0};

void app_wireless_mic_fatfs_save_file_num(void)
{
 #ifdef MTK_NVDM_ENABLE
     uint32_t size = sizeof(uint32_t);
     nvkey_status_t status = nvkey_write_data(NVID_APP_WM_FATFS_FILE_NUM,
                                              (const uint8_t *)&(s_app_wm_fs_context.file_num), size);
     APPS_LOG_MSGID_I(LOG_TAG" save_file_num, num=%d status=%d",
                      2, s_app_wm_fs_context.file_num, status);
 #endif
}


void app_wireless_mic_fatfs_get_file_num(void)
{
 #ifdef MTK_NVDM_ENABLE
     uint32_t size = sizeof(uint32_t);
     nvkey_status_t status = nvkey_read_data(NVID_APP_WM_FATFS_FILE_NUM, (uint8_t *)&(s_app_wm_fs_context.file_num), &size);
     if (status == NVKEY_STATUS_ITEM_NOT_FOUND) {
         size = sizeof(uint32_t);
         status = nvkey_write_data(NVID_APP_WM_FATFS_FILE_NUM, (const uint8_t *)&(s_app_wm_fs_context.file_num), size);
     }
     APPS_LOG_MSGID_I(LOG_TAG" get_file_num, read status=%d, file_num=%d", 2, status, s_app_wm_fs_context.file_num);
 #endif
}

bool app_wireless_mic_fatfs_get_wav_file_status(void)
{
    APPS_LOG_MSGID_I(LOG_TAG" get_wav_file_status:%d", 1, s_app_wm_fs_context.is_create_file);
    return s_app_wm_fs_context.is_create_file;
}

FRESULT app_wireless_mic_fatfs_creat_wav_file(void)
{
    FRESULT ret = FR_OK;
    DWORD nclst = 0;
    UINT length_written ;

    APPS_LOG_MSGID_I(LOG_TAG" creat_file: is_init=%d, file_num=%d",
            2, g_wireless_mic_fs_init_done, s_app_wm_fs_context.file_num);

    if (!g_wireless_mic_fs_init_done) {
        return FR_INT_ERR;
    }

    f_getfree("SD:", &nclst, &s_app_wm_fs_context.fatfs);
#if FF_MAX_SS != FF_MIN_SS
    WORD    ssize = s_app_wm_fs_context.fatfs->ssize;
#else
    WORD    ssize = FF_MIN_SS;
#endif
    DWORD fre_size = s_app_wm_fs_context.fatfs->free_clst*s_app_wm_fs_context.fatfs->csize*ssize;

    APPS_LOG_MSGID_I(LOG_TAG" creat_file: nclst=%d, free_size=0x%x-byte", 2, nclst, fre_size);
    if (fre_size < sizeof(app_wm_wav_header_t)) {
        return FR_NOT_ENABLED;
    }

    if (s_app_wm_fs_context.wav_cache_data == NULL) {
        s_app_wm_fs_context.wav_cache_data = (uint8_t *)pvPortMallocNC(sizeof(uint8_t)*APP_WAV_DATA_CHACHE_MAX_LENGTH);
        if (s_app_wm_fs_context.wav_cache_data == NULL) {
            return FR_DISK_ERR;
        }
        memset(s_app_wm_fs_context.wav_cache_data, 0, APP_WAV_DATA_CHACHE_MAX_LENGTH);
        s_app_wm_fs_context.wav_cache_size = 0;
    }


    memset(filename, 0, sizeof(filename));
    sprintf(filename, "%s/%ld%s", "SD:/recorder", s_app_wm_fs_context.file_num, ".wav");

    DIR fdir;
    ret = f_opendir(&fdir, APP_DEFAULT_DIR);
    if (FR_OK != ret) {
        APPS_LOG_MSGID_I(LOG_TAG" creat_file: open dir fail-ret=%d", 1, ret);
        return ret;
    }

    ret = f_open(&s_app_wm_fs_context.file, filename, FA_CREATE_ALWAYS | FA_WRITE);
    if (FR_OK != ret) {
        APPS_LOG_MSGID_I(LOG_TAG" creat_file: create fail-ret=%d", 1, ret);
        return ret;
    }

    app_wireless_mic_fatfs_create_wav_header(&s_app_wm_fs_context.wav_header);
    ret = f_write(&s_app_wm_fs_context.file, &s_app_wm_fs_context.wav_header, sizeof(app_wm_wav_header_t), &length_written);
    if (FR_OK != ret) {
        APPS_LOG_MSGID_I(LOG_TAG" creat_file: write wav header fail-ret=%d", 1, ret);
        ret = f_close(&s_app_wm_fs_context.file);
        return ret;
    } else {
        s_app_wm_fs_context.is_create_file = TRUE;
        s_app_wm_fs_context.wav_size       = 0;
        s_app_wm_fs_context.file_num++;
        APPS_LOG_MSGID_I(LOG_TAG" creat_file: write wav header success-ret=%d", 1, ret);
    }

    APPS_LOG_MSGID_I(LOG_TAG" creat_file: create end-ret=%d, file_num=%d", 2, ret, s_app_wm_fs_context.file_num);
    return ret;
}

FRESULT app_wireless_mic_fatfs_close_wav_file(void)
{
    FRESULT ret = FR_OK;
    UINT length_written ;

    if (!s_app_wm_fs_context.is_create_file) {
        return ret;
    }

#if 1
    if (s_app_wm_fs_context.wav_cache_data != NULL && s_app_wm_fs_context.wav_cache_size > 0) {
        if (s_app_wm_fs_context.wav_cache_size > 0) {
            f_write(&s_app_wm_fs_context.file, s_app_wm_fs_context.wav_cache_data, s_app_wm_fs_context.wav_cache_size, &length_written);
        }
        vPortFreeNC(s_app_wm_fs_context.wav_cache_data);
        s_app_wm_fs_context.wav_cache_data = NULL;
        s_app_wm_fs_context.wav_cache_size = 0;
    }
#endif
    s_app_wm_fs_context.wav_header.riff.ChunkSize = s_app_wm_fs_context.wav_size + 36;
    s_app_wm_fs_context.wav_header.data.ChunkSize = s_app_wm_fs_context.wav_size;

    f_lseek(&s_app_wm_fs_context.file, 0);

    ret = f_write(&s_app_wm_fs_context.file, &s_app_wm_fs_context.wav_header, sizeof(app_wm_wav_header_t), &length_written);
    if (FR_OK != ret) {
        APPS_LOG_MSGID_I(LOG_TAG" close_file: write file fail-ret=%d", 1, ret);
    }

    ret = f_close(&s_app_wm_fs_context.file);
    if (FR_OK != ret) {
        APPS_LOG_MSGID_I(LOG_TAG" close_file: close file fail-ret=%d", 1, ret);
        ret = f_close(&s_app_wm_fs_context.file);
        return ret;
    }

    s_app_wm_fs_context.is_create_file = FALSE;
    s_app_wm_fs_context.wav_size       = 0;

    APPS_LOG_MSGID_I(LOG_TAG" close_file: close file end-ret=%d", 1, ret);

    return ret;
}


FRESULT app_wireless_mic_fatfs_write_wav_data(uint8_t *data, uint32_t len)
{
    FRESULT ret = FR_OK;
    UINT length_written ;

    if (!s_app_wm_fs_context.is_create_file) {
        APPS_LOG_MSGID_W(LOG_TAG" write_data: wav file was not created.", 0);
        return FR_INT_ERR;
    }

    DWORD nclst = 0;
    f_getfree("SD:", &nclst, &s_app_wm_fs_context.fatfs);
#if FF_MAX_SS != FF_MIN_SS
    WORD    ssize = s_app_wm_fs_context.fatfs->ssize;
#else
    WORD    ssize = FF_MIN_SS;
#endif
    uint32_t fre_size = (s_app_wm_fs_context.fatfs->free_clst*s_app_wm_fs_context.fatfs->csize)*ssize;
    APPS_LOG_MSGID_I(LOG_TAG" write_data: nclst=%d,free_clst=%d, csize=%d, ssize=%d,free_size=0x%x-byte",
                     5, nclst, s_app_wm_fs_context.fatfs->free_clst, s_app_wm_fs_context.fatfs->csize, ssize, fre_size);
    if (fre_size  < len) {
        return FR_NOT_ENABLED;
    }


#if 1
    if (s_app_wm_fs_context.wav_cache_size + len < APP_WAV_DATA_CHACHE_MAX_LENGTH) {
        memcpy(s_app_wm_fs_context.wav_cache_data+s_app_wm_fs_context.wav_cache_size, data, len);
        s_app_wm_fs_context.wav_cache_size += len;
    } else {
        uint32_t remaining_lenth = APP_WAV_DATA_CHACHE_MAX_LENGTH-s_app_wm_fs_context.wav_cache_size;
        memcpy(s_app_wm_fs_context.wav_cache_data+s_app_wm_fs_context.wav_cache_size, data, remaining_lenth);
#ifdef APP_WIRELESS_MIC_DEBUG
        uint32_t start_count = 0;
        uint32_t end_count   = 0;
        uint32_t count   = 0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &start_count);
#endif
        ret = f_write(&s_app_wm_fs_context.file, s_app_wm_fs_context.wav_cache_data, APP_WAV_DATA_CHACHE_MAX_LENGTH, &length_written);
#ifdef APP_WIRELESS_MIC_DEBUG
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_count);
        if (end_count > start_count) {
            count = end_count - start_count;
            } else {
               count = 0xffffffff - (end_count - start_count);
            }
        APPS_LOG_MSGID_I(LOG_TAG" write_data: speed = %dKByte/s=%d", 1, (4096 * 1000) / count);
#endif
        if (FR_OK != ret) {
            APPS_LOG_MSGID_I(LOG_TAG" write_data: write file fail-ret=%d", 1, ret);
            return ret;
        } else {
            s_app_wm_fs_context.wav_cache_size = 0;
            memset(s_app_wm_fs_context.wav_cache_data, 0, APP_WAV_DATA_CHACHE_MAX_LENGTH);
            memcpy(s_app_wm_fs_context.wav_cache_data, data+remaining_lenth, len-remaining_lenth);
            s_app_wm_fs_context.wav_cache_size += len-remaining_lenth;
        }
    }

    s_app_wm_fs_context.wav_size += len;
#else
    ret = f_write(&s_app_wm_fs_context.file, data, len, &length_written);
    if (FR_OK != ret) {
        APPS_LOG_MSGID_I(LOG_TAG" write_data: write file fail-ret=%d", 1, ret);
        return ret;
    } else {
        s_app_wm_fs_context.wav_size += len;
    }
#endif

    APPS_LOG_MSGID_I(LOG_TAG" write_data end ret=%d, cache_size=%d", 2, ret, s_app_wm_fs_context.wav_cache_size);
    return ret;
}

void app_wireless_mic_fatfs_create_wav_header(app_wm_wav_header_t* wavhead)
{
    wavhead->riff.ChunkID      = 0X46464952;
    wavhead->riff.ChunkSize    = 0;           // calculate it when end the recorder
    wavhead->riff.Format       = 0X45564157;

    wavhead->fmt.ChunkID       = 0X20746D66;
    wavhead->fmt.ChunkSize     = 16;
    wavhead->fmt.AudioFormat   = 0X01;
    wavhead->fmt.NumOfChannels = (uint16_t)ami_get_stream_in_channel_num(AUDIO_SCENARIO_TYPE_ADVANCED_RECORD_N_MIC);
    wavhead->fmt.SampleRate    = 48000;
    wavhead->fmt.BlockAlign    = 4;
    wavhead->fmt.BitsPerSample = 32;
    wavhead->fmt.ByteRate      = (wavhead->fmt.SampleRate)*(wavhead->fmt.NumOfChannels)*(wavhead->fmt.BitsPerSample)/8; //SampleRate * NumChannels * BitsPerSample/8

    wavhead->data.ChunkID      = 0X61746164;
    wavhead->data.ChunkSize    = 0;          // calculate it when end the recorder
}


void app_wireless_mic_fatfs_init(void)
{
    s_app_wm_fs_context.fatfs = (FATFS *)pvPortMallocNC(sizeof(FATFS));
    if (s_app_wm_fs_context.fatfs == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG" init, malloc fatfs fail", 0);
        return;
    }

    FRESULT result = f_mount(s_app_wm_fs_context.fatfs, APP_DEFAULT_FS, 1);
    APPS_LOG_MSGID_I(LOG_TAG" init, f_mount result=%d", 1, result);
    if (result == FR_NO_FILESYSTEM) {
        uint8_t *buff = (uint8_t *)pvPortMallocNC(FF_MAX_SS);
        if (buff == NULL) {
            APPS_LOG_MSGID_E(LOG_TAG" init, malloc fail", 0);
            return;
        }
        result = f_mkfs(APP_DEFAULT_FS, NULL, buff, FF_MAX_SS);
        APPS_LOG_MSGID_I(LOG_TAG" init, f_mkfs result=%d", 1, result);
        if (result == FR_OK) {
            result = f_mount(s_app_wm_fs_context.fatfs, APP_DEFAULT_FS, 1);
            APPS_LOG_MSGID_I(LOG_TAG" init, fs_mount again result=%d", 1, result);
        }
        vPortFreeNC(buff);
    }

    if (result == FR_OK) {
        DIR fdir;
        result = f_opendir(&fdir, APP_DEFAULT_DIR);
        APPS_LOG_MSGID_I(LOG_TAG" init, opendir=%d", 1, result);
        if (result == FR_NO_PATH) {
            result = f_mkdir(APP_DEFAULT_DIR);
            APPS_LOG_MSGID_I(LOG_TAG" init, mkdir=%d", 1, result);
        }

        g_wireless_mic_fs_init_done = TRUE;
    }

    app_wireless_mic_fatfs_get_file_num();

    s_app_wm_fs_context.total_size = (s_app_wm_fs_context.fatfs->n_fatent - 2) * s_app_wm_fs_context.fatfs->csize/2;

    APPS_LOG_MSGID_I(LOG_TAG" init result=%d, total_size=%d", 2, result, s_app_wm_fs_context.total_size);
}

#endif //AIR_WIRELESS_MIC_ENABLE
