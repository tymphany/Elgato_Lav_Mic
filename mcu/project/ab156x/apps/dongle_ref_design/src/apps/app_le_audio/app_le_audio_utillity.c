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
#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_le_audio_def.h"
#include "app_le_audio.h"
#include "app_le_audio_usb.h"
#include "app_le_audio_utillity.h"
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
#include "app_le_audio_ucst.h"
#include "app_le_audio_ucst_utillity.h"
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
#include "app_le_audio_bcst.h"
#include "app_le_audio_bcst_utillity.h"
#endif
#include "apps_events_event_group.h"
#include "ui_shell_manager.h"

#include "audio_transmitter_control.h"

#include "bt_avm.h"
#include "hal_usb.h"

#include "bt_le_audio_msglog.h"
#ifdef AIR_SILENCE_DETECTION_ENABLE
#include "bt_sink_srv_ami.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "apps_common_nvkey_struct.h"
#endif
#include "usbaudio_drv.h"

/**************************************************************************************************
* Define
**************************************************************************************************/
#define APP_LE_AUDIO_AVM_BUFFER_NUM             2

/* Audio transmitter channel mode */
#define APP_LE_AUDIO_TRANSMITTER_CHANNEL_MODE_MONO        0x01
#define APP_LE_AUDIO_TRANSMITTER_CHANNEL_MODE_STEREO      0x02
#define APP_LE_AUDIO_TRANSMITTER_CHANNEL_MODE_DUAL_MONO   0x04

#define APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE     0x00
#define APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_SET_VOL  0x01
#define APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_MIX      0x02
#define APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMIX    0x03
#define APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_MUTE     0x04
#define APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMUTE   0x05
#define APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_CHANNEL  0x06
#define APP_LE_AUDIO_TRANSMITTER_CURRENT_CONFIG_OP_MASK 0x0000000F

/* IN_SAMPLE_RATE */
#define IN_SAMPLE_RATE_8KHZ     8
#define IN_SAMPLE_RATE_16KHZ    16
#define IN_SAMPLE_RATE_24KHZ    24
#define IN_SAMPLE_RATE_32KHZ    32
#define IN_SAMPLE_RATE_44_1KHZ  44
#define IN_SAMPLE_RATE_48KHZ    48

#define APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0    0x00
#define APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1    0x01
#define APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_0  0x02
#define APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_1  0x03
#define APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_MAX                 0x04
typedef uint8_t app_le_audio_transmitter_share_info_t;

#define APP_LE_AUDIO_TRANSMITTER_SHARE_BUFFER_SIZE  (5*1024)

#ifdef AIR_SILENCE_DETECTION_ENABLE
#define AIR_LE_AUDIO_SILENCE_DETECTION_STOP_DELAY_TIMER_PERIOD    (293000) /* 4min53s */
#endif

/**************************************************************************************************
* Structure
**************************************************************************************************/
typedef struct  {
    uint16_t DataOffset;
    uint16_t _reserved_word_02h;
    uint32_t TimeStamp;
    uint16_t ConnEvtCnt;
    uint8_t SampleSeq;
    uint8_t _reserved_byte_0Bh;
    uint8_t PduHdrLo;
    uint8_t _reserved_byte_0Dh;
    uint8_t PduLen ;
    uint8_t _reserved_byte_0Fh;
    uint16_t DataLen;
    uint16_t _reserved_word_12h;
    uint32_t _reserved_long_0;
    uint32_t _reserved_long_1;
} LE_AUDIO_HEADER;

/**************************************************************************************************
* Variable
**************************************************************************************************/
app_le_audio_ctrl_t g_lea_ctrl;

static n9_dsp_share_info_t *g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_MAX];
const uint32_t g_lea_sdu_interval_tbl[] = {
/*  sdu_interval (us)   app_le_audio_sdu_interval_t */
    7500,               /* SDU_INTERVAL_7P5_MS */
    10000,              /* SDU_INTERVAL_10_MS */
};

app_le_audio_timer_info_struct g_lea_timer_info[APP_LE_AUDIO_TIMER_MAX_NUM];


#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
#include "apps_events_i2s_in_event.h"
uint8_t g_streaming_port;
app_i2s_in_det_t g_i2s_in_params;
static void set_audio_transmitter_line_in_config(audio_transmitter_config_t *config, uint8_t port);
static void set_audio_transmitter_i2s_in_config(audio_transmitter_config_t *config, uint8_t port);
#endif

/**************************************************************************************************
* Prototype
**************************************************************************************************/
#ifdef AIR_SILENCE_DETECTION_ENABLE
static void app_le_audio_silence_detection_start_by_port(app_le_audio_stream_port_t port);

static void app_le_audio_silence_detection_stop_by_port(app_le_audio_stream_port_t port);
#endif

/**************************************************************************************************
* Static Functions
**************************************************************************************************/
static void app_le_audio_handle_audio_transmitter_evt(audio_transmitter_event_t event, void *data, void *user_data)
{
    app_le_audio_stream_port_t port = (app_le_audio_stream_port_t)user_data;

    switch (event) {
        case AUDIO_TRANSMITTER_EVENT_START_SUCCESS: {
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] recv START_SUCCESS, port:%x", 1, port);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                APP_LE_AUDIO_EVENT_AUDIO_STREAM_START_SUCCESS_CNF, (void *)port, 0,
                                NULL, 0);
            break;
        }
        case AUDIO_TRANSMITTER_EVENT_START_FAIL: {
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] recv START_FAIL, port:%x", 1, port);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                APP_LE_AUDIO_EVENT_AUDIO_STREAM_START_FAIL_CNF, (void *)port, 0,
                                NULL, 0);
            break;
        }
        case AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS: {
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] recv STOP_SUCCESS, port:%x", 1, port);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                APP_LE_AUDIO_EVENT_AUDIO_STREAM_STOP_CNF, (void *)port, 0,
                                NULL, 0);
            break;
        }
        case AUDIO_TRANSMITTER_EVENT_DATA_NOTIFICATION: {
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] recv DATA_NOTIFICATION, port:%x", 1, port);
            break;
        }
        case AUDIO_TRANSMITTER_EVENT_DATA_DIRECT: {
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] recv DATA_DIRECT, port:%x", 1, port);
            break;
        }
        case AUDIO_TRANSMITTER_EVENT_SET_RUNTIME_CONFIG_SUCCESS: {
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] recv RUNTIME_CONFIG_SUCCESS, port:%x", 1, port);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                APP_LE_AUDIO_EVENT_AUDIO_STREAM_CONFIG_CNF, (void *)port, 0,
                                NULL, 0);
            break;
        }
        default:
            break;
    }
}

static n9_dsp_share_info_t *app_le_audio_get_avm_buffer_info(app_le_audio_transmitter_share_info_t type, uint32_t bit_rate, uint32_t frame_interval, uint32_t frame_size)
{
    n9_dsp_share_info_t *p_dsp_info = NULL;
    uint32_t payload_size = 0;

    /* get codec frame size */
    payload_size = (bit_rate * frame_interval / 8 / 1000 / 1000);

    if ((0 == payload_size) || (payload_size != frame_size)) {
        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] get_avm_buffer_info, type:%x bit_rate:%d frame_interval:%d frame_size:%d payload_size:%x", 5,
                          type,
                          bit_rate,
                          frame_interval,
                          frame_size,
                          payload_size);
        //LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] get_avm_buffer_info, error codec frame size:%d %d", 2, payload_size, frame_size);
        configASSERT(0);
    }

    /* get share buffer info */
    switch (type) {
        case APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0: {
            p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_0);
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1: {
            p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_1);
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_0: {
            p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_0);
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_1: {
            p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_1);
            break;
        }
        default:
            break;
    }

    if (NULL != p_dsp_info) {
        memset((void *)p_dsp_info->start_addr, 0, APP_LE_AUDIO_TRANSMITTER_SHARE_BUFFER_SIZE);
        p_dsp_info->read_offset = 0;
        p_dsp_info->write_offset = 0;
        p_dsp_info->sub_info.block_info.block_size = (payload_size + sizeof(LE_AUDIO_HEADER) + 3) / 4 * 4; //4B align
        p_dsp_info->sub_info.block_info.block_num   = APP_LE_AUDIO_TRANSMITTER_SHARE_BUFFER_SIZE / (p_dsp_info->sub_info.block_info.block_size);
    }

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] get_avm_buffer_info, type:%x info:%x", 2, type, p_dsp_info);

    return p_dsp_info;
}

static uint32_t app_le_audio_set_audio_transmitter_op_queue(uint32_t op_queue, uint8_t op_code)
{
    uint32_t tmp, i = 0;

    for (i = 0; i < 8; i++) {
        tmp = ((op_queue >> (4 * i)) & APP_LE_AUDIO_TRANSMITTER_CURRENT_CONFIG_OP_MASK);
        if (!tmp) {
            break;
        }
        if ((i != 0) && (tmp == op_code)) {
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_op, op_queue:%x op_code:%x i:%x", 3, op_queue, op_code, i);
            return op_queue;
        }
    }

    if (8 != i) {
        tmp = (op_queue | (op_code << (4 * i)));
        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_op, op_queue:%x op_code:%x tmp:%x", 3, op_queue, op_code, tmp);
        return tmp;
    }

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_op, op_queue:%x op_code:%x i:%x", 3, op_queue, op_code, i);
    return op_queue;
}

static bt_status_t app_le_audio_set_audio_transmitter_volume_by_port(app_le_audio_stream_port_t port, app_le_audio_stream_info_t *p_stream_info)
{
    if ((APP_LE_AUDIO_STREAM_PORT_MAX <= port) || (NULL == p_stream_info)) {
        return BT_STATUS_FAIL;
    }

    ble_audio_dongle_runtime_config_operation_t opcode;
    audio_transmitter_runtime_config_t config;
    audio_transmitter_status_t ret;

    if (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) {
        opcode = BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_DUL;
    } else {
        opcode = BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL;
    }

    if (p_stream_info->mute) {
        config.ble_audio_dongle_runtime_config.vol_level.vol_level_l = 0;
        config.ble_audio_dongle_runtime_config.vol_level.vol_level_r = 0;
    } else {
        config.ble_audio_dongle_runtime_config.vol_level.vol_level_l = p_stream_info->vol_level_left;
        config.ble_audio_dongle_runtime_config.vol_level.vol_level_r = p_stream_info->vol_level_right;
    }

    config.ble_audio_dongle_runtime_config.vol_level.vol_ratio = 100;

    ret = audio_transmitter_set_runtime_config(p_stream_info->transmitter_id,
                                               opcode,
                                               &config);

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_vol, port:%x ret:%x mute:%x vol_level:%x %x", 5, port, ret, p_stream_info->mute,
                      config.ble_audio_dongle_runtime_config.vol_level.vol_level_l,
                      config.ble_audio_dongle_runtime_config.vol_level.vol_level_r);

    if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
        return BT_STATUS_FAIL;
    }

    p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG;

    return BT_STATUS_SUCCESS;
}

static bt_status_t app_le_audio_set_audio_transmitter_channel(app_le_audio_stream_port_t port, app_le_audio_stream_info_t *p_stream_info)
{
    if ((APP_LE_AUDIO_STREAM_PORT_MAX <= port) || (NULL == p_stream_info)) {
        return BT_STATUS_FAIL;
    }

    ble_audio_dongle_runtime_config_operation_t opcode;
    audio_transmitter_runtime_config_t config;
    audio_transmitter_status_t ret;

    opcode = BLE_AUDIO_DONGLE_CONFIG_OP_SET_UL_CH1_INPUT_SOURCE;

    config.ble_audio_dongle_runtime_config.channel_enable = p_stream_info->mic_channel;

    ret = audio_transmitter_set_runtime_config(p_stream_info->transmitter_id,
                                               opcode,
                                               &config);

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_channel, port:%x ret:%x channel:%x", 3, port, ret, p_stream_info->mic_channel);

    if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
        return BT_STATUS_FAIL;
    }

    p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG;

    return BT_STATUS_SUCCESS;
}

static void app_le_audio_set_audio_transmitter_share_buffer(void)
{
    bt_avm_leaudio_buffer_info_t *leaudio_buf;  /* ul: to controller, dl: from controller */

    if (NULL == (leaudio_buf = pvPortMalloc(APP_LE_AUDIO_AVM_BUFFER_NUM * sizeof(bt_leaudio_buffer_set_t) + sizeof(uint32_t)))) {
        configASSERT(leaudio_buf != NULL);
        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_share_buffer, malloc fail", 0);
        return;
    }

    leaudio_buf->count = APP_LE_AUDIO_AVM_BUFFER_NUM;

    /* share buffer info (SEND_TO_AIR_CH_0) */
    leaudio_buf->buffer[0].ul_address = (uint32_t)g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0];
    leaudio_buf->buffer[0].dl_address = (uint32_t)g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0];
    /* share buffer info (SEND_TO_AIR_CH_1) */
    leaudio_buf->buffer[1].ul_address = (uint32_t)g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1];
    leaudio_buf->buffer[1].dl_address = (uint32_t)g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_share_buffer, (SEND_TO_AIR) CH_0:%x CH_1:%x", 2,
                      leaudio_buf->buffer[0].ul_address,
                      leaudio_buf->buffer[1].ul_address);

    if (g_lea_ctrl.bidirectional) {
        /* share buffer info (FROM_AIR_CH_0) */
        leaudio_buf->buffer[0].dl_address = (uint32_t)g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_0];
        /* share buffer info (FROM_AIR_CH_1) */
        leaudio_buf->buffer[1].dl_address = (uint32_t)g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_1];

        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_share_buffer, (RECV_FROM_AIR) CH_0:%x CH_1:%x", 2,
                          leaudio_buf->buffer[0].dl_address,
                          leaudio_buf->buffer[1].dl_address);
    }

    /* set LE audio share buffer */
    bt_avm_set_leaudio_buffer(leaudio_buf);

    vPortFree(leaudio_buf);
}

bt_status_t app_le_audio_mute_audio_transmitter_by_port(app_le_audio_stream_port_t port, app_le_audio_stream_info_t *p_stream_info)
{
    if ((APP_LE_AUDIO_STREAM_PORT_MAX <= port) || (NULL == p_stream_info)) {
        return BT_STATUS_FAIL;
    }

    ble_audio_dongle_runtime_config_operation_t opcode;
    audio_transmitter_runtime_config_t config;
    audio_transmitter_status_t ret;

    if (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) {
        opcode = BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_DUL;
    } else {
        opcode = BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL;
    }

    config.ble_audio_dongle_runtime_config.vol_level.vol_level_l = 0;
    config.ble_audio_dongle_runtime_config.vol_level.vol_level_r = 0;
    config.ble_audio_dongle_runtime_config.vol_level.vol_ratio = 100;

    ret = audio_transmitter_set_runtime_config(p_stream_info->transmitter_id,
                                               opcode,
                                               &config);

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mute, port:%x ret:%x", 2, port, ret);

    if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
        return BT_STATUS_FAIL;
    }

    p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG;

    return BT_STATUS_SUCCESS;
}

#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
static bool app_le_audio_mix_audio_transmitter_by_port(app_le_audio_stream_port_t port, app_le_audio_stream_info_t *p_stream_info)
{
    audio_transmitter_runtime_config_t config;
    audio_transmitter_status_t ret;
    app_le_audio_stream_info_t *p_stream_info_tmp = NULL;

    if ((APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) || (NULL == p_stream_info)) {
        return false;
    }

    if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTED != p_stream_info->curr_transmitter_state) ||
        (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE != p_stream_info->next_transmitter_state)) {
        return false;
    }

    if (p_stream_info->is_mixed) {
        return true;
    }

    uint8_t streaming_port = app_le_audio_get_streaming_port();

    //mix other streaming port
    for(uint8_t i = 0; i < APP_LE_AUDIO_STREAM_PORT_MAX; i++) {
        if (i == port || i == APP_LE_AUDIO_STREAM_PORT_MIC_0) {
            continue;
        }
        p_stream_info_tmp = &g_lea_ctrl.stream_info[i];
        if (((streaming_port >> i) & 0x01) &&
            ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTED == p_stream_info->curr_transmitter_state) &&
            (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->next_transmitter_state))) {
            //prot is streaming && at STARTED state
            config.ble_audio_dongle_runtime_config.dl_mixer_id = p_stream_info_tmp->transmitter_id;
            ret = audio_transmitter_set_runtime_config(p_stream_info->transmitter_id, BLE_AUDIO_DONGLE_CONFIG_OP_MUSIC_MIX, &config);
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mix port:%d, ret:%x mixer_id:%x", 3, port, ret, config.ble_audio_dongle_runtime_config.dl_mixer_id);
            if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
                LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mix fail! port:%d", 1, port);
                return false;
            }else {
                p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG;
            }
        }
    }
    //mix self
    config.ble_audio_dongle_runtime_config.dl_mixer_id = p_stream_info->transmitter_id;
    ret = audio_transmitter_set_runtime_config(p_stream_info->transmitter_id, BLE_AUDIO_DONGLE_CONFIG_OP_MUSIC_MIX, &config);
    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mix self port:%d, ret:%x mixer_id:%x", 3, port, ret, config.ble_audio_dongle_runtime_config.dl_mixer_id);
    if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mix self fail! port:%d", 1, port);
        return false;
    }else {
        p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG;
    }
    p_stream_info->is_mixed = true;

    return true;
}

#else
static bool app_le_audio_mix_audio_transmitter_by_port(app_le_audio_stream_port_t port, app_le_audio_stream_info_t *p_stream_info)
{
    bool mix_complete = false;

    if ((APP_LE_AUDIO_STREAM_PORT_MIC_0 <= port) || (NULL == p_stream_info)) {
        return false;
    }

    app_le_audio_stream_info_t *p_stream_info_tmp = NULL;
    app_le_audio_stream_port_t port_tmp = APP_LE_AUDIO_STREAM_PORT_SPK_0;

    if (APP_LE_AUDIO_STREAM_PORT_SPK_0 == port) {
        port_tmp = APP_LE_AUDIO_STREAM_PORT_SPK_1;
    }

    p_stream_info_tmp = &g_lea_ctrl.stream_info[port_tmp];
    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] SPK_%x state:%x->%x is_mixed:%x op_queue:%x", 5, port,
                      p_stream_info->curr_transmitter_state,
                      p_stream_info->next_transmitter_state,
                      p_stream_info->is_mixed,
                      p_stream_info->config_op_queue);

    if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTED == p_stream_info->curr_transmitter_state) &&
        (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->next_transmitter_state)) {

        if (p_stream_info->is_mixed) {
            return true;
        }

        audio_transmitter_runtime_config_t config;
        audio_transmitter_status_t ret;


        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] SPK_%x state:%x->%x is_mixed:%x op_queue:%x", 5, port_tmp,
                          p_stream_info_tmp->curr_transmitter_state,
                          p_stream_info_tmp->next_transmitter_state,
                          p_stream_info_tmp->is_mixed,
                          p_stream_info_tmp->config_op_queue);

        if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTED == p_stream_info_tmp->curr_transmitter_state) &&
            (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info_tmp->next_transmitter_state)) {
            p_stream_info->is_mixed = true;
            config.ble_audio_dongle_runtime_config.dl_mixer_id = p_stream_info_tmp->transmitter_id;
            mix_complete = true;
        } else {
            config.ble_audio_dongle_runtime_config.dl_mixer_id = p_stream_info->transmitter_id;
        }

        ret = audio_transmitter_set_runtime_config(p_stream_info->transmitter_id, BLE_AUDIO_DONGLE_CONFIG_OP_MUSIC_MIX, &config);

        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mix SPK_%x, ret:%x mixer_id:%x", 3, port, ret, config.ble_audio_dongle_runtime_config.dl_mixer_id);
        if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
            p_stream_info->is_mixed = false;
        } else {
            p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG;
        }
    }

    if (mix_complete) {
        return true;
    }

    return false;
}
#endif

static bool app_le_audio_unmix_audio_transmitter_by_port(app_le_audio_stream_port_t port, app_le_audio_stream_info_t *p_stream_info)
{
    if ((APP_LE_AUDIO_STREAM_PORT_MAX <= port) || (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) || (NULL == p_stream_info)) {
        return false;
    }

    if (p_stream_info->is_mixed) {
        audio_transmitter_status_t ret;

        p_stream_info->is_mixed = false;
        ret = audio_transmitter_set_runtime_config(p_stream_info->transmitter_id, BLE_AUDIO_DONGLE_CONFIG_OP_MUSIC_UNMIX, NULL);
        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] unmix SPK_%x, ret:%x", 2, port, ret);
        if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
            p_stream_info->is_mixed = true;
            return false;
        }

        p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG;
        return true;
    }

    return false;
}


bt_status_t app_le_audio_init_audio_transmitter(app_le_audio_stream_port_t port)
{
    audio_transmitter_config_t config;
#ifdef AIR_SILENCE_DETECTION_ENABLE
    app_le_audio_silence_detection_mode_enum silence_detection_mode = app_le_audio_silence_detection_get_silence_detection_mode();
#endif
    app_le_audio_stream_info_t *p_stream_info = NULL;
    uint32_t usb_port = 0, usb_sample_rate = 0;
    uint8_t usb_sample_size = 0, usb_channel = 0;
    app_le_audio_usb_port_t lea_usb_port = APP_LE_AUDIO_CONVERT_STREAMING_PORT(port);

#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return BT_STATUS_FAIL;
    }
    if (port != APP_LE_AUDIO_STREAM_PORT_LINE_IN && port != APP_LE_AUDIO_STREAM_PORT_I2S_IN) {
#endif
        if (APP_LE_AUDIO_STREAM_PORT_MAX <= port ||
            APP_LE_AUDIO_USB_PORT_MAX <= lea_usb_port) {
            return BT_STATUS_FAIL;
        }

#ifndef AIR_LE_AUDIO_UNICAST_ENABLE
        if (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) {
            return BT_STATUS_UNSUPPORTED;
        }
#endif

        app_le_audio_usb_convert_port(&usb_port, lea_usb_port);

        if (!app_le_audio_usb_is_port_ready(lea_usb_port)) {
            return BT_STATUS_FAIL;
        }

        if (APP_LE_AUDIO_STREAM_PORT_SPK_0 == port ||
            APP_LE_AUDIO_STREAM_PORT_SPK_1 == port) {
            usb_sample_rate = USB_Audio_Get_RX_Sample_Rate(usb_port);
            usb_sample_size = USB_Audio_Get_RX_Sample_Size(usb_port);
            usb_channel = USB_Audio_Get_RX_Channel(usb_port);
        } else if (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) {
            usb_sample_rate = USB_Audio_Get_TX_Sample_Rate(usb_port);
            usb_sample_size = USB_Audio_Get_TX_Sample_Size(usb_port);
            usb_channel = USB_Audio_Get_TX_Channel(usb_port);
        }

        if ((APP_LE_AUDIO_STREAM_PORT_SPK_0 == port ||
             APP_LE_AUDIO_STREAM_PORT_SPK_1 == port ||
             APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) &&
            (0 == usb_sample_rate ||
             0 == usb_sample_size ||
             0 == usb_channel)) {
            return BT_STATUS_FAIL;
        }

#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
    }
#endif

    p_stream_info = &g_lea_ctrl.stream_info[port];
    if (APP_LE_AUDIO_TRANSMITTER_STATE_INIT <= p_stream_info->curr_transmitter_state) {
        return BT_STATUS_SUCCESS;
    }

    /* Always set downlink AVM if it is not set because currently CIS always enable downlink.
        * Downlink AVM need be ready before CIS creation even when SPK USB port is not enabled or ready.
        */
    if (NULL == g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0]) {
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
        if (APP_LE_AUDIO_MODE_UCST == g_lea_ctrl.curr_mode) {
            g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0] = app_le_audio_get_avm_buffer_info(APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0,
                                                                                                                      (app_le_audio_ucst_get_bitrate(false) * 1000),
                                                                                                                      app_le_audio_ucst_get_sdu_interval(false),
                                                                                                                      app_le_audio_ucst_get_sdu_size(false));
            g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1] = app_le_audio_get_avm_buffer_info(APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1,
                                                                                                                      (app_le_audio_ucst_get_bitrate(false) * 1000),
                                                                                                                      app_le_audio_ucst_get_sdu_interval(false),
                                                                                                                      app_le_audio_ucst_get_sdu_size(false));
        }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
        if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode &&
            APP_LE_AUDIO_STREAM_PORT_MIC_0 != port) {
            g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0] = app_le_audio_get_avm_buffer_info(APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0,
                                                                                                                      (app_le_audio_bcst_get_bitrate() * 1000),
                                                                                                                      app_le_audio_bcst_get_sdu_interval(),
                                                                                                                      app_le_audio_bcst_get_sdu_size());
            g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1] = app_le_audio_get_avm_buffer_info(APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1,
                                                                                                                      (app_le_audio_bcst_get_bitrate() * 1000),
                                                                                                                      app_le_audio_bcst_get_sdu_interval(),
                                                                                                                      app_le_audio_bcst_get_sdu_size());
        }
#endif
    }

#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
    if ((NULL == g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_0]) &&
        (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port)) {
        g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_0] = app_le_audio_get_avm_buffer_info(APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_0,
                                                                                                                    (app_le_audio_ucst_get_bitrate(true) * 1000),
                                                                                                                    app_le_audio_ucst_get_sdu_interval(true),
                                                                                                                    app_le_audio_ucst_get_sdu_size(true));
        g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_1] = app_le_audio_get_avm_buffer_info(APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_1,
                                                                                                                    (app_le_audio_ucst_get_bitrate(true) * 1000),
                                                                                                                    app_le_audio_ucst_get_sdu_interval(true),
                                                                                                                    app_le_audio_ucst_get_sdu_size(true));
    }
#endif

    /* Init audio stream */
    memset(&config, 0x00, sizeof(config));
    config.scenario_type = AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE;

    switch (port) {
#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
        case APP_LE_AUDIO_STREAM_PORT_LINE_IN:
            set_audio_transmitter_line_in_config(&config, port);
            break;
        case APP_LE_AUDIO_STREAM_PORT_I2S_IN:
            set_audio_transmitter_i2s_in_config(&config, port);
            break;
#endif
        case APP_LE_AUDIO_STREAM_PORT_SPK_0:
        case APP_LE_AUDIO_STREAM_PORT_SPK_1: {
            /* Set USB info in use for port. */
            p_stream_info->usb_sample_rate = usb_sample_rate;
            p_stream_info->usb_sample_size = usb_sample_size;
            p_stream_info->usb_channel = usb_channel;

            config.scenario_sub_id = (port == APP_LE_AUDIO_STREAM_PORT_SPK_0) ? AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0 : AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1;

            /* USB setting */
            config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_type = AUDIO_DSP_CODEC_TYPE_PCM;

            config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.sample_rate = p_stream_info->usb_sample_rate;
            config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.format = app_le_audio_usb_convert_sample_size(p_stream_info->usb_sample_size);
            config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.usb_param.pcm.channel_mode = p_stream_info->usb_channel;

            /* BT & Codec setting */
            config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.channel_enable = APP_LE_AUDIO_TRANSMITTER_CHANNEL_DUAL;
            config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_type = AUDIO_DSP_CODEC_TYPE_LC3;
#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
            if (APP_LE_AUDIO_MODE_UCST == g_lea_ctrl.curr_mode) {
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period = app_le_audio_ucst_get_sdu_interval(false);    /* To do: ISO interval */

                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.sample_rate = app_le_audio_ucst_get_sampling_rate(false);
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.bit_rate = (app_le_audio_ucst_get_bitrate(false) * 1000);
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.channel_mode = APP_LE_AUDIO_TRANSMITTER_CHANNEL_MODE_DUAL_MONO;
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_interval = app_le_audio_ucst_get_sdu_interval(false);
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_size = app_le_audio_ucst_get_sdu_size(false);
            }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
            if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) {
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.period = app_le_audio_bcst_get_sdu_interval();

                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.sample_rate = app_le_audio_bcst_get_sampling_rate();
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.bit_rate = (app_le_audio_bcst_get_bitrate() * 1000);
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.channel_mode = APP_LE_AUDIO_TRANSMITTER_CHANNEL_MODE_DUAL_MONO;
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_interval = app_le_audio_bcst_get_sdu_interval();
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.codec_param.lc3.frame_size = app_le_audio_bcst_get_sdu_size();
            }
#endif
            /* Test mode setting */
            if (app_le_audio_get_test_mode() != 0) {
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.test_mode_enable = true;
            } else {
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.test_mode_enable = false;
            }

            /* Volume setting. Mute works only after audio transmitter start. Therefore, do not check mute status at the initial stage. */
            if (p_stream_info->mute) {
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.vol_level.vol_level_l = 0;
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.vol_level.vol_level_r = 0;
            } else {
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.vol_level.vol_level_l = p_stream_info->vol_level_left;
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.vol_level.vol_level_r = p_stream_info->vol_level_right;
            }
            config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.vol_level.vol_ratio = 100;

            /* Silence detection setting */
#ifdef AIR_SILENCE_DETECTION_ENABLE
            if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL == silence_detection_mode) {
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.without_bt_link_mode_enable = TRUE;
            } else
#endif
            {
                config.scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config.without_bt_link_mode_enable = FALSE;
            }

            break;
        }

#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
        case APP_LE_AUDIO_STREAM_PORT_MIC_0: {
            /* Set USB info in use for port. */
            p_stream_info->usb_sample_rate = usb_sample_rate;
            p_stream_info->usb_sample_size = usb_sample_size;
            p_stream_info->usb_channel = usb_channel;

            config.scenario_sub_id = AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT;
            /* BT setting */
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.period = app_le_audio_ucst_get_sdu_interval(true);    /* To do: ISO interval */
#if 0
            uint32_t channel = app_le_audio_ucst_get_available_channel();
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] init MIC_0, available channel %x", 1, channel);

            switch (channel) {
                case AUDIO_LOCATION_FRONT_LEFT:
                    config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.channel_enable = APP_LE_AUDIO_TRANSMITTER_CHANNEL_L;
                    break;
                case AUDIO_LOCATION_FRONT_RIGHT:
                    config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.channel_enable = APP_LE_AUDIO_TRANSMITTER_CHANNEL_R;
                    break;
                default:
                    config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.channel_enable = APP_LE_AUDIO_TRANSMITTER_CHANNEL_DUAL;
                    break;
            }
#else
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.channel_enable = APP_LE_AUDIO_TRANSMITTER_CHANNEL_DUAL;
#endif
            /* USB setting */
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_type = AUDIO_DSP_CODEC_TYPE_PCM;
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.sample_rate = p_stream_info->usb_sample_rate;
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.format = app_le_audio_usb_convert_sample_size(p_stream_info->usb_sample_size);
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.usb_param.pcm.channel_mode = p_stream_info->usb_channel;

            /* Codec setting */
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_type = AUDIO_DSP_CODEC_TYPE_LC3;
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.sample_rate = app_le_audio_ucst_get_sampling_rate(true);
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.bit_rate = (app_le_audio_ucst_get_bitrate(true) * 1000);
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.channel_mode = APP_LE_AUDIO_TRANSMITTER_CHANNEL_MODE_DUAL_MONO;
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.frame_interval = app_le_audio_ucst_get_sdu_interval(true);
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.codec_param.lc3.frame_size = app_le_audio_ucst_get_sdu_size(true);

            /* Test mode setting */
            if (app_le_audio_get_test_mode() != 0) {
                config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.test_mode_enable = true;
            } else {
                config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.test_mode_enable = false;
            }

            /* Volume setting. Mute works only after audio transmitter start. Therefore, do not check mute status at the initial stage. */
            if (p_stream_info->mute) {
                config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.vol_level.vol_level_l = 0;
                config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.vol_level.vol_level_r = 0;
            } else {
                config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.vol_level.vol_level_l = p_stream_info->vol_level_left;
                config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.vol_level.vol_level_r = p_stream_info->vol_level_right;
            }
            config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.vol_level.vol_ratio = 100;

/* Silence detection setting */
#ifdef AIR_SILENCE_DETECTION_ENABLE
            if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL == silence_detection_mode) {
                config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.without_bt_link_mode_enable = TRUE;
            } else
#endif
            {
                config.scenario_config.ble_audio_dongle_config.voice_ble_audio_dongle_config.without_bt_link_mode_enable = FALSE;
            }

            break;
        }
#endif
        default:
            return BT_STATUS_FAIL;
    }

    config.msg_handler = app_le_audio_handle_audio_transmitter_evt;
    config.user_data = (void *)port;

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] init, port:%x port_state:%x->%x", 3, port,
                      p_stream_info->curr_transmitter_state,
                      p_stream_info->next_transmitter_state);

    if ((APP_LE_AUDIO_TRANSMITTER_STATE_IDLE != p_stream_info->curr_transmitter_state) &&
        (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE != p_stream_info->next_transmitter_state)) {
        return BT_STATUS_FAIL;
    }

    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_INIT;

    if (0 > (p_stream_info->transmitter_id = audio_transmitter_init(&config))) {
        LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] init fail, port:%x", 1, port);
        p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
        return BT_STATUS_FAIL;
    }

    p_stream_info->curr_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_INIT;
    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] init, port:%x transmitter_id:%x op_queue:%x wait_cnf:%x", 4,
                      port, p_stream_info->transmitter_id, p_stream_info->config_op_queue, p_stream_info->wait_cnf);

    return BT_STATUS_SUCCESS;
}

static bt_status_t app_le_audio_deinit_audio_transmitter(app_le_audio_stream_port_t port, app_le_audio_stream_info_t *p_stream_info)
{
    audio_transmitter_status_t ret;

    if (NULL == p_stream_info) {
        return BT_STATUS_FAIL;
    }

    ret = audio_transmitter_deinit(p_stream_info->transmitter_id);
    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] deinit, port:%x transmitter_id:%x ret:%x op_queue:%x wait_cnf:%x", 5,
                      port, p_stream_info->transmitter_id, ret, p_stream_info->config_op_queue, p_stream_info->wait_cnf);

    p_stream_info->curr_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
    p_stream_info->is_mixed = false;
    p_stream_info->config_op_queue = 0;
    p_stream_info->transmitter_id = -1;
    p_stream_info->wait_cnf = 0;

    return BT_STATUS_SUCCESS;
}

static bt_status_t app_le_audio_close_audio_transmitter_by_type(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return BT_STATUS_FAIL;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] close, port:%x port_state:%x->%x op_queue:%x", 4, port,
                      p_stream_info->curr_transmitter_state,
                      p_stream_info->next_transmitter_state,
                      p_stream_info->config_op_queue);

    switch (p_stream_info->curr_transmitter_state) {
        case APP_LE_AUDIO_TRANSMITTER_STATE_IDLE: {
            return BT_STATUS_SUCCESS;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_INIT: {
            if (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->next_transmitter_state) {
                app_le_audio_deinit_audio_transmitter(port, p_stream_info);

            } else if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTING == p_stream_info->next_transmitter_state) ||
                       (APP_LE_AUDIO_TRANSMITTER_STATE_STOP == p_stream_info->next_transmitter_state)) {
                /* wait handle_start_cnf or handle_stop_cnf */
                p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT;
            }
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_STARTING: {
            /* wait handle_config_cnf (set vol) */
            p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT;
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_STARTED: {
            if (APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT == p_stream_info->next_transmitter_state) {
                break;
            }
            if (APP_LE_AUDIO_TRANSMITTER_STATE_STOP == p_stream_info->next_transmitter_state) {
                p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT;
                break;
            }
            p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT;
            if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE != p_stream_info->config_op_queue ||
                (p_stream_info->wait_cnf & APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG)) {
                /* wait handle_config_cnf */
                /* op_queue of NONE does not mean there is no config on-going. */
                break;
            }
            if ((APP_LE_AUDIO_STREAM_PORT_MIC_0 != port) && (p_stream_info->is_mixed)) {
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMIX;
                if (app_le_audio_unmix_audio_transmitter_by_port(port, p_stream_info)) {
                    /* wait handle_config_cnf (unmix) */
                    return BT_STATUS_SUCCESS;
                }
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE;
            }

            if (!(APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP & p_stream_info->wait_cnf)) {
                audio_transmitter_status_t ret = audio_transmitter_stop(p_stream_info->transmitter_id);

                LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] close, stop port:%x transmitter_id:%x ret:%x", 3, port, p_stream_info->transmitter_id, ret);
                if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
                    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
                    return BT_STATUS_FAIL;
                }

                p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP;
            }
            break;
        }
    }
    return BT_STATUS_SUCCESS;
}

/**************************************************************************************************
* Public Functions
**************************************************************************************************/
void app_le_audio_handle_audio_transmitter_start_cnf(bool is_success, app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] handle_start_cnf, port:%x is_success:%x port_state:%x->%x op_queue:%x", 5,
                      port, is_success,
                      p_stream_info->curr_transmitter_state,
                      p_stream_info->next_transmitter_state,
                      p_stream_info->config_op_queue);

    p_stream_info->wait_cnf &= (~APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_START);
    if (!is_success) {
        p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
        return;
    }

    p_stream_info->curr_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STARTING;

    if (!g_lea_ctrl.set_buffer) {
        if (g_lea_ctrl.bidirectional) {
            if (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) {
                g_lea_ctrl.set_buffer = true;
                app_le_audio_set_audio_transmitter_share_buffer();
            }
        } else {
            g_lea_ctrl.set_buffer = true;
            app_le_audio_set_audio_transmitter_share_buffer();
        }
    }

    /* check next transmitter state */
    if ((APP_LE_AUDIO_TRANSMITTER_STATE_STOP == p_stream_info->next_transmitter_state) ||
        (APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT == p_stream_info->next_transmitter_state)) {
        if (!(APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP & p_stream_info->wait_cnf)) {
            p_stream_info->config_op_queue = 0;

            if (!(APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP & p_stream_info->wait_cnf)) {
                audio_transmitter_status_t ret = audio_transmitter_stop(p_stream_info->transmitter_id);

                LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] handle_start_cnf stop, port:%x transmitter_id:%x ret:%x", 3, port, p_stream_info->transmitter_id, ret);
                if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
                    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
                } else {
                    p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP;
                }
            }
        }
        /* return??? */
    } else {
        p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_SET_VOL;
        p_stream_info->config_op_queue = 0;

        if (BT_STATUS_SUCCESS != app_le_audio_set_audio_transmitter_volume_by_port(port, p_stream_info)) {
            p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
        }
    }

#ifdef AIR_SILENCE_DETECTION_ENABLE
    if (APP_LE_AUDIO_TRANSMITTER_STATE_STOP != p_stream_info->next_transmitter_state &&
        APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT != p_stream_info->next_transmitter_state) {
        app_le_audio_silence_detection_mode_enum silence_detection_mode = app_le_audio_silence_detection_get_silence_detection_mode();

        if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_NORMAL == silence_detection_mode ||
            APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL == silence_detection_mode) {
            /* Start silence detetion */
            app_le_audio_silence_detection_start_by_port(port);
        }
    }
#endif

    uint8_t i;
    i = APP_LE_AUDIO_STREAM_PORT_MAX;
    while (i > 0) {
        i--;
        if ((APP_LE_AUDIO_TRANSMITTER_STATE_INIT == g_lea_ctrl.stream_info[i].curr_transmitter_state) &&
            ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTING == g_lea_ctrl.stream_info[i].next_transmitter_state) ||
             (APP_LE_AUDIO_TRANSMITTER_STATE_STOP == g_lea_ctrl.stream_info[i].next_transmitter_state))) {
            return;
        }
    }

#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
    if (APP_LE_AUDIO_MODE_UCST == g_lea_ctrl.curr_mode) {
        app_le_audio_ucst_open_audio_transmitter_cb();
    }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
    if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) {
        app_le_audio_bcst_open_audio_transmitter_cb();
    }
#endif
}

void app_le_audio_handle_audio_transmitter_stop_cnf(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] handle_stop_cnf, port:%x port_state:%x->%x op_queue:%x", 4,
                      port,
                      p_stream_info->curr_transmitter_state,
                      p_stream_info->next_transmitter_state,
                      p_stream_info->config_op_queue);


    p_stream_info->wait_cnf &= (~APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP);
    p_stream_info->curr_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STOP;

    /* check next transmitter state */
    if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTING == p_stream_info->next_transmitter_state) ||
        (APP_LE_AUDIO_TRANSMITTER_STATE_SET_VOL == p_stream_info->next_transmitter_state)) {
        p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STARTING;

        if (!(APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_START & p_stream_info->wait_cnf)) {
            audio_transmitter_status_t ret = audio_transmitter_start(p_stream_info->transmitter_id);

            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] handle_stop_cnf start, port:%x transmitter_id:%x ret:%x", 3, port, p_stream_info->transmitter_id, ret);
            if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
                p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
            } else {
                p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_START;
            }
        }
        return;
    }

    if (APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT == p_stream_info->next_transmitter_state) {
        app_le_audio_deinit_audio_transmitter(port, p_stream_info);
    }

    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
    p_stream_info->config_op_queue = 0;

    if (!g_lea_ctrl.open_audio_transmitter) {
        uint8_t i;
        i = APP_LE_AUDIO_STREAM_PORT_MAX;
        while (i > 0) {
            i--;
            if ((APP_LE_AUDIO_TRANSMITTER_STATE_IDLE != g_lea_ctrl.stream_info[i].curr_transmitter_state) ||
                (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE != g_lea_ctrl.stream_info[i].next_transmitter_state)) {
                return;
            }
        }

#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
        if (APP_LE_AUDIO_MODE_UCST == g_lea_ctrl.curr_mode) {
            app_le_audio_ucst_close_audio_transmitter_cb();
        }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
        if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) {
            app_le_audio_bcst_close_audio_transmitter_cb();
        }
#endif
    }
}

void app_le_audio_handle_audio_transmitter_config_cnf(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] handle_config_cnf, port:%x port_state:%x->%x op_queue:%x", 4, port,
                      p_stream_info->curr_transmitter_state,
                      p_stream_info->next_transmitter_state,
                      p_stream_info->config_op_queue);

    p_stream_info->wait_cnf &= (~APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG);
    if (APP_LE_AUDIO_TRANSMITTER_STATE_STARTING == p_stream_info->curr_transmitter_state) {
        p_stream_info->curr_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STARTED;
    }
    switch (p_stream_info->next_transmitter_state) {
        case APP_LE_AUDIO_TRANSMITTER_STATE_STOP:
        case APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT: {
            p_stream_info->config_op_queue = 0;
            if (p_stream_info->is_mixed) {
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMIX;
                if (app_le_audio_unmix_audio_transmitter_by_port(port, p_stream_info)) {
                    return;
                }
                p_stream_info->config_op_queue = 0;
            }
            if (!(APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP & p_stream_info->wait_cnf)) {
                audio_transmitter_status_t ret = audio_transmitter_stop(p_stream_info->transmitter_id);

                LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] handle_config_cnf stop, port:%x transmitter_id:%x ret:%x", 3, port, p_stream_info->transmitter_id, ret);
                if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
                    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
                    return;
                }
                p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP;
            }
            return;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_STARTING: {
            return;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_SET_VOL: {
            p_stream_info->curr_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STARTED;
            p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
            if (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) {
                if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE == p_stream_info->config_op_queue) {
                    p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_CHANNEL;
                    if (BT_STATUS_SUCCESS != app_le_audio_set_audio_transmitter_channel(APP_LE_AUDIO_STREAM_PORT_MIC_0, p_stream_info)) {
                        p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE;
                    }
                } else {
                    p_stream_info->config_op_queue = app_le_audio_set_audio_transmitter_op_queue(p_stream_info->config_op_queue, APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_CHANNEL);
                }
                //app_le_audio_set_audio_transmitter_mic_channel(p_stream_info->mic_channel);
            } else {
                if (app_le_audio_mix_audio_transmitter_by_port(port, p_stream_info)) {
#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
                    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mix port fail! port: %d", 1, port);
#else
                    if (APP_LE_AUDIO_STREAM_PORT_SPK_0 == port) {
                        app_le_audio_mix_audio_transmitter_by_port(APP_LE_AUDIO_STREAM_PORT_SPK_1, &g_lea_ctrl.stream_info[APP_LE_AUDIO_STREAM_PORT_SPK_1]);
                    } else {
                        app_le_audio_mix_audio_transmitter_by_port(APP_LE_AUDIO_STREAM_PORT_SPK_0, &g_lea_ctrl.stream_info[APP_LE_AUDIO_STREAM_PORT_SPK_0]);
                    }
#endif
                }
            }
            return;
        }
        default:
            break;
    }

    if (!p_stream_info->config_op_queue) {
        return;
    }

    uint8_t op;
    p_stream_info->config_op_queue = (p_stream_info->config_op_queue >> 4);                     /* remove curr op */
    op = (p_stream_info->config_op_queue & APP_LE_AUDIO_TRANSMITTER_CURRENT_CONFIG_OP_MASK);    /* get next op */

    switch (op) {
        case APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_SET_VOL:
        case APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMUTE: {
            app_le_audio_set_audio_transmitter_volume_by_port(port, p_stream_info);
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_MIX: {
            app_le_audio_mix_audio_transmitter_by_port(port, p_stream_info);
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_MUTE: {
            app_le_audio_mute_audio_transmitter_by_port(port, p_stream_info);
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_CHANNEL: {
            app_le_audio_set_audio_transmitter_channel(port, p_stream_info);
            break;
        }
        default:
            break;
    }

}

bt_status_t app_le_audio_mute_audio_transmitter(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return BT_STATUS_FAIL;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];
    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mute, port:%x %x", 2, port, p_stream_info->mute);
    p_stream_info->mute = true;

    if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTED == p_stream_info->curr_transmitter_state) &&
        (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->next_transmitter_state)) {

        if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE == p_stream_info->config_op_queue &&
            0 == (APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG & p_stream_info->wait_cnf)) {
            ble_audio_dongle_runtime_config_operation_t opcode;
            audio_transmitter_runtime_config_t config;
            audio_transmitter_status_t ret;

            p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_MUTE;
            if (APP_LE_AUDIO_STREAM_PORT_MIC_0 == port) {
                opcode = BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_DUL;
            } else {
                opcode = BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL;
            }

            config.ble_audio_dongle_runtime_config.vol_level.vol_level_l = 0;
            config.ble_audio_dongle_runtime_config.vol_level.vol_level_r = 0;
            config.ble_audio_dongle_runtime_config.vol_level.vol_ratio = 100;

            ret = audio_transmitter_set_runtime_config(p_stream_info->transmitter_id,
                                                       opcode,
                                                       &config);
            LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] mute, port:%x ret:%x", 2, port, ret);
            if (BT_STATUS_SUCCESS != ret) {
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE;
            } else {
                p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG;
            }
        } else {
            p_stream_info->config_op_queue = app_le_audio_set_audio_transmitter_op_queue(p_stream_info->config_op_queue, APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_MUTE);
        }
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t app_le_audio_unmute_audio_transmitter(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return BT_STATUS_FAIL;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];
    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] unmute, port:%x %x", 2, port, p_stream_info->mute);
    p_stream_info->mute = false;

    if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTED == p_stream_info->curr_transmitter_state) &&
        (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->next_transmitter_state)) {

        if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE == p_stream_info->config_op_queue &&
            0 == (APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG & p_stream_info->wait_cnf)) {
            p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMUTE;
            if (BT_STATUS_SUCCESS != app_le_audio_set_audio_transmitter_volume_by_port(port, p_stream_info)) {
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE;
            }
        } else {
            p_stream_info->config_op_queue = app_le_audio_set_audio_transmitter_op_queue(p_stream_info->config_op_queue, APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMUTE);
        }
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t app_le_audio_set_audio_transmitter_volume_level(app_le_audio_stream_port_t port, uint8_t vol_level_left, uint8_t vol_level_right)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return BT_STATUS_FAIL;
    }

    if ((vol_level_left > APP_LE_AUDIO_VOL_LEVEL_MAX) || (vol_level_right > APP_LE_AUDIO_VOL_LEVEL_MAX)) {
        return BT_STATUS_FAIL;
    }
    p_stream_info = &g_lea_ctrl.stream_info[port];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_vol_level, port:%x mute:%x vol_level:%d %d", 4, port, p_stream_info->mute, vol_level_left, vol_level_right);

    p_stream_info->vol_level_left = vol_level_left;
    p_stream_info->vol_level_right = vol_level_right;

    if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTED == p_stream_info->curr_transmitter_state) &&
        (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->next_transmitter_state) &&
        (false == p_stream_info->mute)) {

        if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE == p_stream_info->config_op_queue &&
            0 == (APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG & p_stream_info->wait_cnf)) {
            p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_SET_VOL;
            if (BT_STATUS_SUCCESS != app_le_audio_set_audio_transmitter_volume_by_port(port, p_stream_info)) {
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE;
            }
        } else {
            p_stream_info->config_op_queue = app_le_audio_set_audio_transmitter_op_queue(p_stream_info->config_op_queue, APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_SET_VOL);
        }
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t app_le_audio_set_audio_transmitter_mic_channel(uint32_t channel)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;
    p_stream_info = &g_lea_ctrl.stream_info[APP_LE_AUDIO_STREAM_PORT_MIC_0];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] set_mic_channel, %x->%x", 2, p_stream_info->mic_channel, channel);

    if (p_stream_info->mic_channel == channel) {
        return BT_STATUS_SUCCESS;
    }

    p_stream_info->mic_channel = channel;

    if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTED == p_stream_info->curr_transmitter_state) &&
        (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->next_transmitter_state)) {

        if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE == p_stream_info->config_op_queue &&
            0 == (APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG & p_stream_info->wait_cnf)) {
            p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_CHANNEL;
            if (BT_STATUS_SUCCESS != app_le_audio_set_audio_transmitter_channel(APP_LE_AUDIO_STREAM_PORT_MIC_0, p_stream_info)) {
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE;
            }
        } else {
            p_stream_info->config_op_queue = app_le_audio_set_audio_transmitter_op_queue(p_stream_info->config_op_queue, APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_CHANNEL);
        }
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t app_le_audio_start_audio_transmitter(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return BT_STATUS_FAIL;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] start, port:%x port_state:%x->%x", 3, port,
                      p_stream_info->curr_transmitter_state,
                      p_stream_info->next_transmitter_state);

    switch (p_stream_info->curr_transmitter_state) {
        case APP_LE_AUDIO_TRANSMITTER_STATE_IDLE: {
            return BT_STATUS_FAIL;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_INIT: {
            if (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->next_transmitter_state) {
                p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STARTING;

                if (!(APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_START & p_stream_info->wait_cnf)) {
                    audio_transmitter_status_t ret = audio_transmitter_start(p_stream_info->transmitter_id);
                    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] start, port:%x transmitter_id:%x ret:%x", 3, port, p_stream_info->transmitter_id, ret);
                    if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
                        p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
                        return BT_STATUS_FAIL;
                    }

                    p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_START;
                }
            } else {
                /* wait handle_start_cnf */
                p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STARTING;
            }
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_STARTING: {
            if ((APP_LE_AUDIO_TRANSMITTER_STATE_STOP == p_stream_info->next_transmitter_state) ||
                (APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT == p_stream_info->next_transmitter_state)) {
                /* wait handle_stop_cnf */
                p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_SET_VOL;
            }
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_STARTED: {
            if ((APP_LE_AUDIO_TRANSMITTER_STATE_STOP == p_stream_info->next_transmitter_state) ||
                (APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT == p_stream_info->next_transmitter_state)) {
                if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE != p_stream_info->config_op_queue ||
                    p_stream_info->wait_cnf & APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG) {
                    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
                    if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMIX == p_stream_info->config_op_queue) {
                        p_stream_info->config_op_queue = app_le_audio_set_audio_transmitter_op_queue(p_stream_info->config_op_queue, APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_MIX);
                    }
                } else {
                    /* wait handle_stop_cnf */
                    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STARTING;
                }
            }
            break;
        }
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

uint32_t app_le_audio_get_usb_sample_rate_in_use(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return 0;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];
    if (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->curr_transmitter_state) {
        return 0;
    }

    return p_stream_info->usb_sample_rate;
}


uint8_t app_le_audio_get_usb_sample_size_in_use(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return 0;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];
    if (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->curr_transmitter_state) {
        return 0;
    }

    return p_stream_info->usb_sample_size;
}

uint8_t app_le_audio_get_usb_channel_in_use(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return 0;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];
    if (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE == p_stream_info->curr_transmitter_state) {
        return 0;
    }

    return p_stream_info->usb_channel;
}


/* Only stop port and do not deinit port */
bt_status_t app_le_audio_stop_audio_transmitter(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return BT_STATUS_FAIL;
    }
    p_stream_info = &g_lea_ctrl.stream_info[port];

    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] stop, port:%x port_state:%x->%x", 3, port,
                      p_stream_info->curr_transmitter_state,
                      p_stream_info->next_transmitter_state);

    switch (p_stream_info->curr_transmitter_state) {
        case APP_LE_AUDIO_TRANSMITTER_STATE_IDLE: {
            return BT_STATUS_FAIL;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_INIT: {
            if ((APP_LE_AUDIO_TRANSMITTER_STATE_STARTING == p_stream_info->next_transmitter_state) ||
                (APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT == p_stream_info->next_transmitter_state)) {
                /* wait handle_start_cnf */
                p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STOP;
            }
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_STARTING: {
            /* wait handle_config_cnf */
            p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STOP;
            break;
        }
        case APP_LE_AUDIO_TRANSMITTER_STATE_STARTED: {
            if (APP_LE_AUDIO_TRANSMITTER_STATE_STOP == p_stream_info->next_transmitter_state) {
                break;
            }
            if (APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT == p_stream_info->next_transmitter_state) {
                p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STOP;
                break;
            }
            p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_STOP;
            if (APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE != p_stream_info->config_op_queue ||
                p_stream_info->wait_cnf & APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG) {
                /* wait handle_config_cnf */
                break;
            }
            if ((APP_LE_AUDIO_STREAM_PORT_MIC_0 != port) && (p_stream_info->is_mixed)) {
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_UNMIX;
                if (app_le_audio_unmix_audio_transmitter_by_port(port, p_stream_info)) {
                    return BT_STATUS_SUCCESS;
                }
                p_stream_info->config_op_queue = APP_LE_AUDIO_TRANSMITTER_CONFIG_OP_NONE;
            }

            if (!(APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP & p_stream_info->wait_cnf)) {
                audio_transmitter_status_t ret = audio_transmitter_stop(p_stream_info->transmitter_id);
                LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS] stop, port:%x transmitter_id:%x ret:%x", 3, port, p_stream_info->transmitter_id, ret);
                if (AUDIO_TRANSMITTER_STATUS_SUCCESS != ret) {
                    p_stream_info->next_transmitter_state = APP_LE_AUDIO_TRANSMITTER_STATE_IDLE;
                    return BT_STATUS_FAIL;
                }
                p_stream_info->wait_cnf |= APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP;
            }
            break;
        }
    }
    return BT_STATUS_SUCCESS;
}


/* Return success as long as at least one port is started successfully. */
bt_status_t app_le_audio_open_audio_transmitter(bool bidirectional, app_le_audio_stream_port_mask_t streaming_port)
{
    bt_status_t ret = BT_STATUS_FAIL, start_ret = BT_STATUS_FAIL;
    LE_AUDIO_MSGLOG_I("[APP][AUDIO_TRANS]open audio transmitter, streaming_port:%x ", 1, streaming_port);
    g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0] = NULL;
    g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1] = NULL;
    g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_0] = NULL;
    g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_1] = NULL;

    /* SPK_0 */
    if (streaming_port & APP_LE_AUDIO_STREAM_PORT_MASK_SPK_0) {
        if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_SPK_0)) {
            ret = app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_SPK_0);
            if (BT_STATUS_SUCCESS != start_ret) {
                start_ret = ret;
            }
        }
    }

    /* SPK_1 */
    if (streaming_port & APP_LE_AUDIO_STREAM_PORT_MASK_SPK_1) {
        if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_SPK_1)) {
            ret = app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_SPK_1);
            if (BT_STATUS_SUCCESS != start_ret) {
                start_ret = ret;
            }
        }
    }

    /* MIC_0 */
    if (bidirectional) {
        if (streaming_port & APP_LE_AUDIO_STREAM_PORT_MASK_MIC_0) {
            if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_MIC_0)) {
                ret = app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_MIC_0);
                if (BT_STATUS_SUCCESS != start_ret) {
                    start_ret = ret;
                }
            }
        }
    }

#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
    /* LINE_IN */
    if (streaming_port & APP_LE_AUDIO_STREAM_PORT_MASK_LINE_IN) {
        if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_LINE_IN)) {
            ret = app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_LINE_IN);
            if (BT_STATUS_SUCCESS != start_ret) {
                start_ret = ret;
            }
        }
    }
    /* I2S_IN */
    if (streaming_port & APP_LE_AUDIO_STREAM_PORT_MASK_I2S_IN) {
        if (BT_STATUS_SUCCESS == app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_I2S_IN)) {
            ret = app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_I2S_IN);
            if (BT_STATUS_SUCCESS != start_ret) {
                start_ret = ret;
            }
        }
    }
#endif

    if (BT_STATUS_SUCCESS != start_ret) {
        return start_ret;
    }
    g_lea_ctrl.open_audio_transmitter = true;
    g_lea_ctrl.bidirectional = bidirectional;
    g_lea_ctrl.set_buffer = false;
    return BT_STATUS_SUCCESS;
}


/* Only stop port and deinit port */
bt_status_t app_le_audio_close_audio_transmitter(void)
{
    uint8_t i;

#ifdef AIR_SILENCE_DETECTION_ENABLE
    app_le_audio_silence_detection_stop_by_port(APP_LE_AUDIO_STREAM_PORT_SPK_0);
    app_le_audio_silence_detection_stop_by_port(APP_LE_AUDIO_STREAM_PORT_SPK_1);
#endif

    /* SPK_0 */
    app_le_audio_close_audio_transmitter_by_type(APP_LE_AUDIO_STREAM_PORT_SPK_0);
    /* SPK_1 */
    app_le_audio_close_audio_transmitter_by_type(APP_LE_AUDIO_STREAM_PORT_SPK_1);
    /* MIC_0 */
    app_le_audio_close_audio_transmitter_by_type(APP_LE_AUDIO_STREAM_PORT_MIC_0);
    g_lea_ctrl.open_audio_transmitter = false;
    g_lea_ctrl.set_buffer = false;
    g_lea_ctrl.bidirectional = false;

    i = APP_LE_AUDIO_STREAM_PORT_MAX;
    while (0 < i) {
        i--;
        if (APP_LE_AUDIO_TRANSMITTER_STATE_IDLE != g_lea_ctrl.stream_info[i].curr_transmitter_state) {
            return BT_STATUS_SUCCESS;
        }
    }

    g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_0] = NULL;
    g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_SEND_TO_AIR_CH_1] = NULL;
    g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_0] = NULL;
    g_lea_share_info[APP_LE_AUDIO_TRANSMITTER_SHARE_INFO_RECV_FROM_AIR_CH_1] = NULL;

#ifdef AIR_LE_AUDIO_UNICAST_ENABLE
    if (APP_LE_AUDIO_MODE_UCST == g_lea_ctrl.curr_mode) {
        app_le_audio_ucst_close_audio_transmitter_cb();
    }
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
    if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) {
        app_le_audio_bcst_close_audio_transmitter_cb();
    }
#endif
    return BT_STATUS_SUCCESS;
}

uint8_t app_le_audio_get_sample_freq(uint8_t in_smaple_rate)
{
    switch (in_smaple_rate) {
        case IN_SAMPLE_RATE_8KHZ:
            return CODEC_CONFIGURATION_SAMPLING_FREQ_8KHZ;
        case IN_SAMPLE_RATE_16KHZ:
            return CODEC_CONFIGURATION_SAMPLING_FREQ_16KHZ;
        case IN_SAMPLE_RATE_24KHZ:
            return CODEC_CONFIGURATION_SAMPLING_FREQ_24KHZ;
        case IN_SAMPLE_RATE_32KHZ:
            return CODEC_CONFIGURATION_SAMPLING_FREQ_32KHZ;
        case IN_SAMPLE_RATE_44_1KHZ:
            return CODEC_CONFIGURATION_SAMPLING_FREQ_44_1KHZ;
        case IN_SAMPLE_RATE_48KHZ:
            return CODEC_CONFIGURATION_SAMPLING_FREQ_48KHZ;
    }
    return APP_LE_AUDIO_SAMPLING_FREQ_INVALID;
}

app_le_audio_mode_t app_le_audio_get_current_mode(void)
{
    return g_lea_ctrl.curr_mode;
}

bt_status_t app_le_audio_setup_iso_data_path(bt_handle_t handle, bt_gap_le_iso_data_path_direction_t direction, bt_gap_le_iso_data_path_id_t id)
{
    bt_status_t ret;
    bt_gap_le_setup_iso_data_path_t param = {
        .handle = handle,
        .direction = direction,
        .data_path_id = id, /* should use AVM buffer#1 instead of BT_GAP_LE_ISO_DATA_PATH_ID_HCI */
        .codec_format = 0x00,
        .company_id = 0x0000,
        .vendor_codec_id = 0x0000,
        .controller_delay = 0,
        .codec_configuration_length = 0,
        .codec_configuration = NULL
    };

    ret = bt_gap_le_setup_iso_data_path(&param);

    LE_AUDIO_MSGLOG_I("[APP] setup_iso_data_path, handle:%x direction:%x id:%x ret:%x", 4, handle, direction, id, ret);

    return ret;
}

bt_status_t app_le_audio_remove_iso_data_path(bt_handle_t handle, uint8_t direction)
{
    bt_status_t ret;
    bt_gap_le_remove_iso_data_path_t param = {
        .handle = handle,
        .data_path_direction = direction,
    };

    ret = bt_gap_le_remove_iso_data_path(&param);

    LE_AUDIO_MSGLOG_I("[APP] remove_iso_data_path, handle:%x direction:%x ret:%x", 3, handle, direction, ret);

    return ret;
}


#ifdef AIR_SILENCE_DETECTION_ENABLE
app_le_audio_silence_detection_mode_enum app_le_audio_silence_detection_get_silence_detection_mode(void)
{
    app_le_audio_ucst_target_t curr_target = app_le_audio_ucst_get_curr_target();

    if (APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == curr_target ||
        APP_LE_AUDIO_UCST_TARGET_STOP_MEDIA_MODE == curr_target) {
        return APP_LE_AUDIO_SILENCE_DETECTION_MODE_NORMAL;
    } else if (APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE == curr_target ||
               APP_LE_AUDIO_UCST_TARGET_STOP_SPECIAL_SILENCE_DETECTION_MODE == curr_target) {
        return APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL;
    } else {
        return APP_LE_AUDIO_SILENCE_DETECTION_MODE_NONE;
    }
}


/* Set the silence detecting status of the device. No need to set status to IDLE. It will be updated automatically when getting status. */
void app_le_audio_silence_detection_set_status(app_le_audio_silence_detection_status_enum status)
{
    g_lea_ctrl.silence_detection.silence_detection_status = status;
}


app_le_audio_silence_detection_status_enum app_le_audio_silence_detection_get_status(void)
{
    app_le_audio_silence_detection_mode_enum silence_detection_mode = app_le_audio_silence_detection_get_silence_detection_mode();

    /* There is no time slot for normal silence detection changing to special silence detection. */
    if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_NONE == silence_detection_mode &&
        APP_LE_AUDIO_SILENCE_DETECTION_STATUS_IDLE != g_lea_ctrl.silence_detection.silence_detection_status) {
        app_le_audio_silence_detection_set_status(APP_LE_AUDIO_SILENCE_DETECTION_STATUS_IDLE);
    }

    return g_lea_ctrl.silence_detection.silence_detection_status;
}


TimerHandle_t app_le_audio_silence_detection_get_delay_stop_timer(void)
{
    return g_lea_ctrl.silence_detection.delay_stop_timer_handle;
}


static uint32_t app_le_audio_silence_detection_get_delay_stop_timer_period(void)
{
    return g_lea_ctrl.silence_detection.delay_stop_timer_period;
}


void app_le_audio_silence_detection_stop_delay_stop_timer(void)
{
    if (g_lea_ctrl.silence_detection.delay_stop_timer_handle) {
        app_le_audio_timer_stop(g_lea_ctrl.silence_detection.delay_stop_timer_handle);
        g_lea_ctrl.silence_detection.delay_stop_timer_handle = NULL;
    }
}


void app_le_audio_silence_detection_stop_delay_timer_callback(TimerHandle_t timer_handle, void *user_data)
{
    app_le_audio_silence_detection_mode_enum silence_detection_mode = app_le_audio_silence_detection_get_silence_detection_mode();

    LE_AUDIO_MSGLOG_I("[APP] sd timer expired handle:%x sd_mode:%x timer:%x", 3, timer_handle, silence_detection_mode, g_lea_ctrl.silence_detection.delay_stop_timer_handle);

    if (g_lea_ctrl.silence_detection.delay_stop_timer_handle) {
        /* The timer will be deleted after callback returns. */
        g_lea_ctrl.silence_detection.delay_stop_timer_handle = NULL;

        if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_NORMAL == silence_detection_mode) {
            app_le_audio_ucst_stop(FALSE);
        }
    }
}


void app_le_audio_silence_detection_handle_remote_device_bredr_connection_update(void *parameter)
{
    app_le_audio_silence_detection_status_enum silence_detection_status = app_le_audio_silence_detection_get_status();
    bool disconnect_cis = app_le_audio_silence_detection_disconnect_cis_for_silence();
    app_le_audio_ucst_ctrl_t *ucst_ctrl = app_le_audio_ucst_get_ctrl();
    app_le_audio_silence_detection_mode_enum silence_detection_mode = app_le_audio_silence_detection_get_silence_detection_mode();

    /* Only process the event when silence detection is on-going.
        * When currently data is detected, do nothing.
        * When currently silcence is detected, if disconnecting CIS is needed, disconnect CIS if CIS is not disconnected.
        * If disconnecting CIS is not needed, reconnect CIS if CIS is disconnected.
        */
    if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_NONE != silence_detection_mode &&
        APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA == silence_detection_status) {
        if (disconnect_cis &&
            !app_le_audio_silence_detection_is_speical_silence_detection_ongoing()) {
            app_le_audio_ucst_stop(FALSE);
        } else if (!disconnect_cis &&
                   (app_le_audio_silence_detection_is_speical_silence_detection_ongoing() ||
                    APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE == ucst_ctrl->next_target)) {
            app_le_audio_ucst_start();
        }
    }
}


void app_le_audio_silence_detection_handle_disconnect_cis_for_silence_update(void *parameter)
{
    app_le_audio_silence_detection_handle_remote_device_bredr_connection_update(parameter);
}


void app_le_audio_silence_detection_handle_port_disabled(void *parameter)
{
    app_le_audio_ucst_ctrl_t *ucst_ctrl = app_le_audio_ucst_get_ctrl();
    uint8_t streaming_port = app_le_audio_ucst_get_streaming_port();
    app_le_audio_usb_port_t port = (app_le_audio_usb_port_t)parameter;

    if (APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE == ucst_ctrl->curr_target) {
        if (!streaming_port) {
            app_le_audio_ucst_stop(FALSE);
        } else {
            app_le_audio_silence_detection_stop_by_port(port);
            app_le_audio_close_audio_transmitter_by_type(port);
        }
    }
}


void app_le_audio_silence_detection_handle_event(app_le_audio_silence_detection_event_enum event_id, void *parameter)
{
    app_le_audio_ucst_ctrl_t *ucst_ctrl = app_le_audio_ucst_get_ctrl();
    uint8_t streaming_port = app_le_audio_ucst_get_streaming_port();

    LE_AUDIO_MSGLOG_I("[APP][SD] event_id:%x streaming_port:%x target:%x->%x", 4,
                      event_id,
                      streaming_port,
                      ucst_ctrl->curr_target,
                      ucst_ctrl->next_target);

    /* Process events using their handlers. */
    if (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_REMOTE_DEVICE_BREDR_STATUS_UPDATE == event_id) {
        app_le_audio_silence_detection_handle_remote_device_bredr_connection_update(parameter);
        return;
    } else if (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_DISCONNECT_CIS_FOR_SILENCE_UPDATE == event_id) {
        app_le_audio_silence_detection_handle_disconnect_cis_for_silence_update(parameter);
        return;
    } else if (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_PORT_DISABLED == event_id) {
        app_le_audio_silence_detection_handle_port_disabled(parameter);
        return;
    }

    /* Process events which update curr_target and/or next_target and continue to process based on curr_target/next_target updated.  */
    if (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_START_OTHER_MODE == event_id ||
        APP_LE_AUDIO_SILENCE_DETECTION_EVENT_STOP_ANY_MODE == event_id) {
        bool restart = FALSE;

        if (APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE == ucst_ctrl->curr_target) {
            ucst_ctrl->curr_target = APP_LE_AUDIO_UCST_TARGET_STOP_SPECIAL_SILENCE_DETECTION_MODE;
        }

        if (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_STOP_ANY_MODE == event_id) {
            restart = (bool)parameter;
        }

        ucst_ctrl->next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        if (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_START_OTHER_MODE == event_id ||
            (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_STOP_ANY_MODE == event_id && restart)) {
            if (APP_LE_AUDIO_USB_PORT_MASK_MIC_0 & streaming_port) {
                ucst_ctrl->next_target = APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE;
            }
            else {
                ucst_ctrl->next_target = APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE;
            }
        }
    }
    else if (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_START_SPECIAL_SILENCE_DETECTION == event_id) {
        /* Only start the special silence mode if there is any speaker USB port enabled and any LE link connected. */
        if (app_le_audio_ucst_get_link_num() &&
            (APP_LE_AUDIO_USB_PORT_MASK_SPK_0 & streaming_port ||
             APP_LE_AUDIO_USB_PORT_MASK_SPK_1 & streaming_port)) {
            ucst_ctrl->curr_target = APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE;
        } else {
            ucst_ctrl->curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        }
        ucst_ctrl->next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
    } else if (APP_LE_AUDIO_SILENCE_DETECTION_EVENT_SPECIAL_SILENCE_DETECTION_STOPPED == event_id) {
        if (APP_LE_AUDIO_UCST_TARGET_NONE == ucst_ctrl->next_target ||
            APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == ucst_ctrl->next_target ||
            APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == ucst_ctrl->next_target) {
            ucst_ctrl->curr_target = ucst_ctrl->next_target;
        } else {
            /* Unexpected next_target. */
            ucst_ctrl->curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        }
        ucst_ctrl->next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
    }

    LE_AUDIO_MSGLOG_I("[APP][SD] target:%x->%x", 2, ucst_ctrl->curr_target, ucst_ctrl->next_target);

    /* Process based on curr_target updated above */
    if (APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE == ucst_ctrl->curr_target) {
        /* Open audio transmitter if needed */
        if (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == ucst_ctrl->curr_stream_state &&
            APP_LE_AUDIO_UCST_STREAM_STATE_IDLE == ucst_ctrl->next_stream_state) {
            /* Set silence detection mode first. Open audio transmitter will check the silence detection mode. */
            ucst_ctrl->next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_START_AUDIO_STREAM;
            if (BT_STATUS_SUCCESS != app_le_audio_open_audio_transmitter(FALSE, streaming_port)) {
                ucst_ctrl->next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
                ucst_ctrl->curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
                return;
            }
        }
    } else if (APP_LE_AUDIO_UCST_TARGET_STOP_SPECIAL_SILENCE_DETECTION_MODE == ucst_ctrl->curr_target) {
        /* Close audio transmitter if needed */
        if (APP_LE_AUDIO_UCST_STREAM_STATE_IDLE < ucst_ctrl->curr_stream_state &&
            APP_LE_AUDIO_UCST_STREAM_STATE_STREAMING >= ucst_ctrl->curr_stream_state &&
            APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM != ucst_ctrl->next_stream_state) {
            ucst_ctrl->next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_STOP_AUDIO_STREAM;
            if (BT_STATUS_SUCCESS != app_le_audio_close_audio_transmitter()) {
                ucst_ctrl->next_stream_state = APP_LE_AUDIO_UCST_STREAM_STATE_IDLE;
                ucst_ctrl->curr_target = APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE;
                return;
            }
        }
    } else if (APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == ucst_ctrl->curr_target ||
               APP_LE_AUDIO_UCST_TARGET_START_CALL_MODE == ucst_ctrl->curr_target) {
        ucst_ctrl->curr_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        ucst_ctrl->next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) {
            g_lea_ctrl.curr_mode = APP_LE_AUDIO_MODE_BCST;
            g_lea_ctrl.next_mode = APP_LE_AUDIO_MODE_NONE;
            app_le_audio_start_broadcast();
        } else {
            if (app_le_audio_ucst_check_pause_stream() ||
                ucst_ctrl->release) {
                return;
            }

            app_le_audio_ucst_start();
        }
    } else if (APP_LE_AUDIO_UCST_TARGET_NONE == ucst_ctrl->curr_target) {
        /* There should be no case of NONE curr_target with non-NONE next_target. */
        ucst_ctrl->next_target = APP_LE_AUDIO_UCST_TARGET_NONE;
        if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.next_mode) {
            g_lea_ctrl.curr_mode = APP_LE_AUDIO_MODE_BCST;
            g_lea_ctrl.next_mode = APP_LE_AUDIO_MODE_NONE;
            app_le_audio_start_broadcast();
        }
    }
}


/* silence detection mode is set after curr_target is set to APP_LE_AUDIO_UCST_TARGET_START_SPECIAL_SILENCE_DETECTION_MODE. */
bool app_le_audio_silence_detection_is_speical_silence_detection_ongoing(void)
{
    app_le_audio_silence_detection_mode_enum silence_detection_mode = app_le_audio_silence_detection_get_silence_detection_mode();

    return silence_detection_mode == APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL;
}


bt_status_t app_le_audio_silence_detection_set_status_by_port(app_le_audio_stream_port_t port, app_le_audio_silence_detection_status_enum status)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return BT_STATUS_FAIL;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];
    p_stream_info->silence_detection_status = status;
    return BT_STATUS_SUCCESS;
}


/* Get detecting status */
app_le_audio_silence_detection_status_enum app_le_audio_silence_detection_get_status_by_port(app_le_audio_stream_port_t port)
{
    app_le_audio_stream_info_t *p_stream_info = NULL;

    if (APP_LE_AUDIO_STREAM_PORT_MAX <= port) {
        return APP_LE_AUDIO_SILENCE_DETECTION_STATUS_MAX;
    }

    p_stream_info = &g_lea_ctrl.stream_info[port];
    return p_stream_info->silence_detection_status;
}


bool app_le_audio_silence_detection_is_detecting_by_port(app_le_audio_stream_port_t port)
{
    app_le_audio_silence_detection_status_enum port_status = app_le_audio_silence_detection_get_status_by_port(port);

    if (APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_SILENCE == port_status ||
        APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA == port_status) {
        return TRUE;
    }

   return FALSE;
}


void app_le_audio_silence_detection_start_by_port(app_le_audio_stream_port_t port)
{
    app_le_audio_ucst_target_t curr_target = app_le_audio_ucst_get_curr_target();
    app_le_audio_silence_detection_mode_enum silence_detection_mode = app_le_audio_silence_detection_get_silence_detection_mode();

    if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_NORMAL == silence_detection_mode ||
        APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL == silence_detection_mode) {
        LE_AUDIO_MSGLOG_I("[APP][SD] start silence detection port:%x sd_mode:%x", 2, port, silence_detection_mode);
        if (APP_LE_AUDIO_STREAM_PORT_SPK_0 == port) {
            audio_silence_detection_scenario_start(AUDIO_SCENARIO_TYPE_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0, app_le_audio_silence_detection_callback);
        } else if (APP_LE_AUDIO_STREAM_PORT_SPK_1 == port) {
            audio_silence_detection_scenario_start(AUDIO_SCENARIO_TYPE_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1, app_le_audio_silence_detection_callback);
        }

        //if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_NORMAL == silence_detection_mode) {
        if (APP_LE_AUDIO_UCST_TARGET_START_MEDIA_MODE == curr_target) {
            app_le_audio_silence_detection_set_status_by_port(port, APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_SILENCE);
        } else {
            app_le_audio_silence_detection_set_status_by_port(port, APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA);
        }
    }
}


static void app_le_audio_silence_detection_stop_by_port(app_le_audio_stream_port_t port)
{
    if (app_le_audio_silence_detection_is_detecting_by_port(port)) {
        LE_AUDIO_MSGLOG_I("[APP][SD] stop silence detection port:%x", 1, port);
        if (APP_LE_AUDIO_STREAM_PORT_SPK_0 == port) {
            audio_silence_detection_scenario_stop(AUDIO_SCENARIO_TYPE_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0);
        } else if (APP_LE_AUDIO_STREAM_PORT_SPK_1 == port) {
            audio_silence_detection_scenario_stop(AUDIO_SCENARIO_TYPE_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1);
        }

        app_le_audio_silence_detection_set_status_by_port(port, APP_LE_AUDIO_SILENCE_DETECTION_STATUS_IDLE);
    }
}


void app_le_audio_silence_detection_callback(audio_scenario_type_t scenario_type, bool silence_flag)
{
    app_le_audio_stream_port_t port = APP_LE_AUDIO_STREAM_PORT_SPK_0, other_port = APP_LE_AUDIO_STREAM_PORT_SPK_1;
    app_le_audio_silence_detection_status_enum port_status = APP_LE_AUDIO_SILENCE_DETECTION_STATUS_MAX;
    app_le_audio_silence_detection_status_enum other_port_status = APP_LE_AUDIO_SILENCE_DETECTION_STATUS_MAX;
    app_le_audio_silence_detection_status_enum old_device_status = app_le_audio_silence_detection_get_status();
    app_le_audio_silence_detection_status_enum new_device_status = APP_LE_AUDIO_SILENCE_DETECTION_STATUS_MAX;
    app_le_audio_silence_detection_mode_enum silence_detection_mode = app_le_audio_silence_detection_get_silence_detection_mode();

    if (AUDIO_SCENARIO_TYPE_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0 != scenario_type &&
        AUDIO_SCENARIO_TYPE_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1 != scenario_type) {
        LE_AUDIO_MSGLOG_W("[APP][SD] wrong scenario_type:%x", 1, scenario_type);
        return;
    }

    if (AUDIO_SCENARIO_TYPE_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1 == scenario_type) {
        port = APP_LE_AUDIO_STREAM_PORT_SPK_1;
        other_port = APP_LE_AUDIO_STREAM_PORT_SPK_0;
    }

    if (!app_le_audio_silence_detection_is_detecting_by_port(port)) {
        LE_AUDIO_MSGLOG_W("[APP][SD] Not detecting:%x", 1, port);
        return;
    }

    /* Update silence detection status of port */
    if (silence_flag) {
        /* Silence detected */
        port_status = APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA;
    } else {
        /* Data detected */
        port_status = APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_SILENCE;
    }
    app_le_audio_silence_detection_set_status_by_port(port, port_status);

    other_port_status = app_le_audio_silence_detection_get_status_by_port(other_port);

    /* Update silence detection status of device */
    if (APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA != other_port_status &&
        APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_SILENCE != other_port_status) {
        /* Only one port starts silence detection. */
        app_le_audio_silence_detection_set_status(port_status);
    } else {
        if (APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA == port_status &&
            APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA == other_port_status) {
            /* Only when both ports detected silence, will the status of device be slience detected. */
            app_le_audio_silence_detection_set_status(APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA);
        } else {
            /* As long as at least one port detected data, the status of device will be data detected. */
            app_le_audio_silence_detection_set_status(APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_SILENCE);
        }
    }

    new_device_status = app_le_audio_silence_detection_get_status();

    LE_AUDIO_MSGLOG_I("[APP][SD] s_flag:%x p:%x ps:%x o_p:%x o_ps:%x ds:%x->%x sd_mode:%x handle:%x", 9,
                      silence_flag, port, port_status, other_port, other_port_status,
                      old_device_status, new_device_status, silence_detection_mode,
                      g_lea_ctrl.silence_detection.delay_stop_timer_handle);

    // if (old_device_status != new_device_status)
    {
        if (APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA == new_device_status) {
            /* Silence detected */
            if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_NORMAL == silence_detection_mode) {
                if (app_le_audio_silence_detection_disconnect_cis_for_silence()) {
                    app_le_audio_ucst_stop(FALSE);
                } else {
                    uint32_t timer_period = app_le_audio_silence_detection_get_delay_stop_timer_period();

                    if (timer_period) {
                        /* Start a timer to stop non-special silence detection mode latter. */
                        if (!g_lea_ctrl.silence_detection.delay_stop_timer_handle) {
                            app_le_audio_timer_start(&g_lea_ctrl.silence_detection.delay_stop_timer_handle,
                                                     timer_period,
                                                     app_le_audio_silence_detection_stop_delay_timer_callback,
                                                     NULL);
                        }
                    } else {
                        /* Stop immediately. */
                        app_le_audio_ucst_stop(FALSE);
                    }
                }
            }
        } else if (APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_SILENCE == new_device_status) {
            /* Data detected */
            app_le_audio_silence_detection_stop_delay_stop_timer();
            if (APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL == silence_detection_mode) {
                app_le_audio_ucst_start();
            }
        }
    }
}


void app_le_audio_silence_detection_init(void)
{
    nvkey_status_t nvkey_status = NVKEY_STATUS_OK;
    app_silence_detection_nvkey_struct silence_detection_nvkey = {0};
    uint32_t size = sizeof(app_silence_detection_nvkey_struct);

    nvkey_status = nvkey_read_data(NVID_APP_SILENCE_DETECTION, (uint8_t *)&silence_detection_nvkey, &size);
    if (NVKEY_STATUS_OK == nvkey_status) {
        g_lea_ctrl.silence_detection.delay_stop_timer_period = silence_detection_nvkey.delay_stop_timer_period;
    } else {
        g_lea_ctrl.silence_detection.delay_stop_timer_period = AIR_LE_AUDIO_SILENCE_DETECTION_STOP_DELAY_TIMER_PERIOD;
    }

    LE_AUDIO_MSGLOG_I("[APP][SD] period:%x status:%x size:%x", 3,
                      g_lea_ctrl.silence_detection.delay_stop_timer_period,
                      nvkey_status,
                      size);
}


void app_le_audio_silence_detection_handle_bt_off(void)
{
    if (g_lea_ctrl.silence_detection.delay_stop_timer_handle) {
        app_le_audio_timer_stop(g_lea_ctrl.silence_detection.delay_stop_timer_handle);
        g_lea_ctrl.silence_detection.delay_stop_timer_handle = NULL;
    }
    g_lea_ctrl.silence_detection.silence_detection_status = APP_LE_AUDIO_SILENCE_DETECTION_STATUS_IDLE;
}
#endif


void app_le_audio_timer_reset_timer_info(app_le_audio_timer_info_struct *timer_info)
{
    if (timer_info) {
        memset(timer_info, 0, sizeof(app_le_audio_timer_info_struct));
    }
}


app_le_audio_timer_info_struct *app_le_audio_timer_get_timer_info_by_timer_handle(TimerHandle_t timer_handle)
{
    uint32_t i = 0;

    if (!timer_handle) {
        return NULL;
    }

    for (; APP_LE_AUDIO_TIMER_MAX_NUM > i; i++) {
        if (timer_handle == g_lea_timer_info[i].timer_handle) {
            return &g_lea_timer_info[i];
        }
    }

    return NULL;
}


app_le_audio_timer_info_struct *app_le_audio_timer_get_empty_timer_info(void)
{
    uint32_t i = 0;

    for (; APP_LE_AUDIO_TIMER_MAX_NUM > i; i++) {
        if (NULL == g_lea_timer_info[i].timer_handle) {
            return &g_lea_timer_info[i];
        }
    }

    return NULL;
}


void app_le_audio_timer_handle_timer_expired_event(TimerHandle_t timer_handle)
{
    app_le_audio_timer_info_struct *timer_info = app_le_audio_timer_get_timer_info_by_timer_handle(timer_handle);

    if (timer_info) {
        if (timer_info->callback) {
            timer_info->callback(timer_handle, timer_info->user_data);
        }

        xTimerDelete(timer_info->timer_handle, 0);
        app_le_audio_timer_reset_timer_info(timer_info);
    }
}


void app_le_audio_timer_callback(TimerHandle_t timer_handle)
{
    ui_shell_send_event(TRUE,
                        EVENT_PRIORITY_HIGH,
                        EVENT_GROUP_UI_SHELL_LE_AUDIO,
                        APP_LE_AUDIO_EVENT_TIMER_EXPIRED,
                        timer_handle,
                        0,
                        NULL,
                        0);
}


/* timer_period is in ms. The timer is identified by timer_handle only. The user should manage the allocating and the freeing of user_data. */
bt_status_t app_le_audio_timer_start(TimerHandle_t *timer_handle, uint32_t timer_period, app_le_audio_timer_callback_t callback, void *user_data)
{
    app_le_audio_timer_info_struct *timer_info = NULL;

    if (!timer_handle || !timer_period || !callback) {
        return BT_STATUS_FAIL;
    }

    *timer_handle = NULL;

    timer_info = app_le_audio_timer_get_empty_timer_info();
    if (!timer_info) {
        /* No empty timer info. */
        return BT_STATUS_TIMER_FULL;
    }

    timer_info->timer_handle = xTimerCreate("g_lea_timer",
                                            (timer_period / portTICK_PERIOD_MS),
                                            pdFALSE,
                                            NULL,
                                            app_le_audio_timer_callback);

    if (timer_info->timer_handle) {
        *timer_handle = timer_info->timer_handle;
        timer_info->callback = callback;
        timer_info->user_data = user_data;
        xTimerStart(timer_info->timer_handle, 0);
        LE_AUDIO_MSGLOG_I("[APP] start timer. handle:%x period:%x", 2, timer_info->timer_handle, timer_period);
        return BT_STATUS_SUCCESS;
    }

    return BT_STATUS_FAIL;
}


void app_le_audio_timer_stop(TimerHandle_t timer_handle)
{
    app_le_audio_timer_info_struct *timer_info = app_le_audio_timer_get_timer_info_by_timer_handle(timer_handle);

    if (timer_info) {
        LE_AUDIO_MSGLOG_I("[APP] stop timer. handle:%x", 1, timer_handle);
        xTimerStop(timer_info->timer_handle, 0);
        xTimerDelete(timer_info->timer_handle, 0);
        app_le_audio_timer_reset_timer_info(timer_info);
    }
}

#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
static void set_audio_transmitter_line_in_config(audio_transmitter_config_t *config, uint8_t port)
{
    music_ble_audio_dongle_config_t *line_in_at_config = &config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config;
    config->scenario_sub_id = AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_LINE_IN;
    /* BT setting */
    if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) {
        line_in_at_config->period = app_le_audio_bcst_get_sdu_interval();
    }
    line_in_at_config->channel_enable = APP_LE_AUDIO_TRANSMITTER_CHANNEL_DUAL;
    /* Codec setting */
    line_in_at_config->codec_type = AUDIO_DSP_CODEC_TYPE_LC3;
    if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) {
        line_in_at_config->codec_param.lc3.sample_rate = app_le_audio_bcst_get_sampling_rate();
        line_in_at_config->codec_param.lc3.bit_rate = (app_le_audio_bcst_get_bitrate() * 1000);
        line_in_at_config->codec_param.lc3.channel_mode = APP_LE_AUDIO_TRANSMITTER_CHANNEL_MODE_DUAL_MONO;
        line_in_at_config->codec_param.lc3.frame_interval = app_le_audio_bcst_get_sdu_interval();
        line_in_at_config->codec_param.lc3.frame_size = app_le_audio_bcst_get_sdu_size();
    }

    line_in_at_config->vol_level.vol_level_l = g_lea_ctrl.stream_info[port].vol_level_left;
    line_in_at_config->vol_level.vol_level_r = g_lea_ctrl.stream_info[port].vol_level_right;
    line_in_at_config->vol_level.vol_ratio = 70;
}

static void set_audio_transmitter_i2s_in_config(audio_transmitter_config_t *config, uint8_t port)
{
    music_ble_audio_dongle_config_t *i2s_in_at_config = &config->scenario_config.ble_audio_dongle_config.music_ble_audio_dongle_config;
    config->scenario_sub_id = AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_I2S_IN;
    /* BT setting */
    if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) {
        i2s_in_at_config->period = app_le_audio_bcst_get_sdu_interval();
    }
    i2s_in_at_config->channel_enable = APP_LE_AUDIO_TRANSMITTER_CHANNEL_DUAL;
    /* Codec setting */
    i2s_in_at_config->codec_type = AUDIO_DSP_CODEC_TYPE_LC3;
    if (APP_LE_AUDIO_MODE_BCST == g_lea_ctrl.curr_mode) {
        i2s_in_at_config->codec_param.lc3.sample_rate = app_le_audio_bcst_get_sampling_rate();
        i2s_in_at_config->codec_param.lc3.bit_rate = (app_le_audio_bcst_get_bitrate() * 1000);
        i2s_in_at_config->codec_param.lc3.channel_mode = APP_LE_AUDIO_TRANSMITTER_CHANNEL_MODE_DUAL_MONO;
        i2s_in_at_config->codec_param.lc3.frame_interval = app_le_audio_bcst_get_sdu_interval();
        i2s_in_at_config->codec_param.lc3.frame_size = app_le_audio_bcst_get_sdu_size();
    }

    i2s_in_at_config->vol_level.vol_level_l = g_lea_ctrl.stream_info[port].vol_level_left;
    i2s_in_at_config->vol_level.vol_level_r = g_lea_ctrl.stream_info[port].vol_level_right;
    i2s_in_at_config->vol_level.vol_ratio = 70;
}

uint8_t app_le_audio_get_streaming_port(void)
{
    uint8_t usb_straming_port;
    usb_straming_port = app_le_audio_usb_get_streaming_port();
    g_streaming_port = (g_streaming_port & 0xf8) | usb_straming_port;

    return g_streaming_port;
}

void app_le_audio_line_in_start(void)
{
    g_streaming_port |= APP_LE_AUDIO_STREAM_PORT_MASK_LINE_IN;

    if (app_le_audio_bcst_gat_curr_state() == APP_LE_AUDIO_BCST_STATE_IDLE) {
        app_le_audio_bcst_start();
        return;
    }

    app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_LINE_IN);
    app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_LINE_IN);
}

void app_le_audio_line_in_stop(void)
{
    g_streaming_port &= !APP_LE_AUDIO_STREAM_PORT_MASK_LINE_IN;
    app_le_audio_close_audio_transmitter_by_type(APP_LE_AUDIO_STREAM_PORT_LINE_IN);
}

void app_le_audio_i2s_in_start(app_i2s_in_det_t *i2s_in_param)
{
    g_streaming_port |= APP_LE_AUDIO_STREAM_PORT_MASK_I2S_IN;
    memcpy(&g_i2s_in_params, i2s_in_param, sizeof(app_i2s_in_det_t));

    if (app_le_audio_bcst_gat_curr_state() == APP_LE_AUDIO_BCST_STATE_IDLE) {
        app_le_audio_bcst_start();
        return;
    }

    app_le_audio_init_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_I2S_IN);
    app_le_audio_start_audio_transmitter(APP_LE_AUDIO_STREAM_PORT_I2S_IN);
}

void app_le_audio_i2s_in_stop(void)
{
    g_streaming_port &= !APP_LE_AUDIO_STREAM_PORT_MASK_I2S_IN;
    app_le_audio_close_audio_transmitter_by_type(APP_LE_AUDIO_STREAM_PORT_I2S_IN);
}
#endif

#endif  /* AIR_LE_AUDIO_ENABLE */

