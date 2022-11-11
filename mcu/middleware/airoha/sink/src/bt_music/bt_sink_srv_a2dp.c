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

#include "bt_sink_srv_a2dp.h"
#include "bt_sink_srv_avrcp.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_ami.h"
#include "bt_gap.h"
#include "bt_sdp.h"
#include "bt_di.h"
#include "hal_audio.h"
#include "hal_audio_message_struct.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_sink_srv_utils.h"
#include "bt_codec.h"
#include "avm_direct.h"
#ifdef MTK_BT_SPEAKER_ENABLE
#ifdef MTK_BT_SPEAKER_FEC_ENABLE
#include "speaker_fec.h"
#endif
#endif
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
#include "bt_sink_srv_aws_mce_a2dp.h"
#endif
#include "bt_avm.h"
#ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
#include "hal_audio_message_struct.h"
#include "hal_audio_internal.h"
#endif
#include "bt_sink_srv_ami.h"
#include "bt_device_manager.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "bt_device_manager_internal.h"
#include "bt_iot_device_white_list.h"
#include "bt_gap.h"
#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le_cap.h"
#endif

#ifdef BT_SINK_DUAL_ANT_ENABLE
#include "bt_sink_srv_dual_ant.h"
#endif
#include "bt_utils.h"
#ifdef MTK_BT_A2DP_AAC_ENABLE
#define BT_SINK_SRV_A2DP_AAC_SUPPORT
#endif
#include "bt_sink_srv_nvkey_struct.h"

/* currently care sink role */
#define SBC_CODEC_NUM (0x01)

#ifdef BT_SINK_SRV_A2DP_AAC_SUPPORT
#define AAC_CODEC_NUM   (0x01)
#else
#define AAC_CODEC_NUM   (0x00)
#endif

#ifdef MTK_BT_A2DP_VENDOR_CODEC_SUPPORT
#define VENDOR_CODEC_NUM   (0x01)
#else
#define VENDOR_CODEC_NUM   (0x00)
#endif

#define BT_SINK_SRV_A2DP_CODEC_MAX_NUM (SBC_CODEC_NUM+AAC_CODEC_NUM+VENDOR_CODEC_NUM)

bt_sink_srv_a2dp_pseudo_handle_t g_a2dp_pse_hd[BT_SINK_SRV_A2DP_PSEUDO_COUNT + BT_SINK_SRV_MCE_A2DP_PSEUDO_COUNT] = {{0}};

bt_a2dp_codec_capability_t g_bt_sink_srv_a2dp_codec_list[BT_SINK_SRV_A2DP_CODEC_MAX_NUM];

#ifdef BT_SINK_SRV_A2DP_AAC_SUPPORT
static bool g_bt_sink_srv_aac_open = true;
#endif
#if defined(MTK_BT_A2DP_VENDOR_2_ENABLE) || defined(MTK_BT_A2DP_VENDOR_CODEC_SUPPORT)
#ifdef AIR_MULTI_POINT_ENABLE
static bool g_bt_sink_srv_vendor_codec_open = false;
#else
static bool g_bt_sink_srv_vendor_codec_open = true;
#endif
#endif

static bool g_bt_sink_srv_a2dp_init_complete = false;
uint32_t g_a2dp_gpt_run_count_begin = 0;
uint32_t g_a2dp_gpt_run_count_end = 0;
uint32_t g_a2dp_gpt_codec_run_count_begin = 0;
uint32_t g_a2dp_gpt_codec_run_count_end = 0;

uint8_t g_sn_wrap_count = 0;

volatile uint32_t g_Rdebug_a2dp_ltcs_last_time = 0;    /* for A2DP LTCS debug */
#ifdef ENABLE_HWSRC_CLKSKEW
#define BT_SINK_SRV_MUSIC_MAPPING_VALUE     (61)
extern int16_t bt_sink_srv_music_drift_mapping(int16_t origin_drift, int16_t mapping_value);
#endif

static bt_codec_sbc_t sbc_cap[1] = {
    {
        8,  // min_bit_pool
        75,  // max_bit_pool
        0xf, // block_len: all
        0xf, // subband_num: all
        0x3, // both snr/loudness
        0xf, // sample_rate: all
        0xf  // channel_mode: all
    }
};

#ifdef BT_SINK_SRV_A2DP_AAC_SUPPORT
static bt_codec_aac_t aac_cap[1] = {
    {
        true,    /*VBR         */
        false,   /*DRC         */
        0x40,    /*Object type */
        0x0c,    /*Channels    */
        0x0018,  /*Sample_rate */
        0x60000  /*bit_rate, 384 Kbps */
    }
};
#endif

#ifndef MTK_BT_CM_SUPPORT
const static bt_device_manager_db_remote_pnp_info_t g_special_connection_device_list[] = {
    {0x0100, 0x0075}, //GVA-13093, Samsung S8
    {0x00c4, 0x13a1}, //PANABTA-347, GVA-17746
};
#endif
extern void bt_sink_srv_music_a2dp_common_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void
                                                  *param);
extern bt_status_t bt_sink_srv_set_clock_offset_ptr_to_dsp(const bt_bd_addr_t *address);
audio_src_srv_handle_t *bt_sink_srv_a2dp_alloc_pseudo_handle(void);
void bt_sink_srv_a2dp_free_pseudo_handle(audio_src_srv_handle_t *hd);
static bool g_a2dp_codec_aac_support = true;
static int32_t bt_sink_srv_a2dp_handle_start_streaming_ind(bt_a2dp_start_streaming_ind_t *start_ind, bool is_cnf);
static int32_t bt_sink_srv_a2dp_handle_suspend_streaming_ind(bt_a2dp_suspend_streaming_ind_t *suspend_ind, bool need_response);
static void bt_sink_srv_a2dp_handle_media_data_received_ind(uint32_t gap_handle, bt_sink_srv_music_data_info_t *media_info);
static void bt_sink_srv_a2dp_set_sta(bt_sink_srv_music_set_sta_param_t *sta_parameters, bool need_timer);

#ifdef AIR_A2DP_SYNC_STOP_ENABLE
extern void bt_sink_srv_music_sync_stop_done_handler(hal_audio_event_t event, void *data);
#endif

void bt_sink_srv_switch_aac_codec(bool enable)
{
    bt_sink_srv_report_id("[sink][a2dp] aac switch, origin: %d, current: %d", 2,
                          g_a2dp_codec_aac_support, enable);
    g_a2dp_codec_aac_support = enable;
}

#define BT_SINK_SRV_AWS_MCE_SRV_MODE_INVALID 0xff
bt_status_t bt_sink_srv_a2dp_get_params_by_mode(bt_aws_mce_srv_mode_t mode, bt_a2dp_init_params_t *param)
{
    int32_t ret = BT_STATUS_FAIL;
    uint32_t num = 0;

    if (param) {
        /* init sink sep */
        bt_sink_srv_a2dp_sbc_config_parameter_t sbc_config = {
            .min_bitpool = 8,
            .max_bitpool = 75,
        };
        uint32_t read_size = sizeof(sbc_config);
        nvkey_status_t nvdm_ret = nvkey_read_data(NVID_BT_HOST_MUSIC_SBC_CONFIG, (uint8_t *)(&sbc_config), &read_size);
        (void)nvdm_ret;
        sbc_cap[0].min_bit_pool = sbc_config.min_bitpool;
        sbc_cap[0].max_bit_pool = sbc_config.max_bitpool;
        bt_sink_srv_report_id("[sink][a2dp]sbc,max_bit_rate:0x%02x, min_bit_rate:0x%02x, read_size:%d, nvdm_ret:%d", 4, sbc_config.max_bitpool, sbc_config.min_bitpool,
                              read_size, nvdm_ret);
        BT_A2DP_MAKE_SBC_CODEC(g_bt_sink_srv_a2dp_codec_list + num, BT_A2DP_SINK,
                               sbc_cap[0].min_bit_pool, sbc_cap[0].max_bit_pool,
                               sbc_cap[0].block_length, sbc_cap[0].subband_num,
                               sbc_cap[0].alloc_method, sbc_cap[0].sample_rate,
                               sbc_cap[0].channel_mode);
#ifdef MTK_BT_SPEAKER_ENABLE
        /* Limit max bitpool to 35 due to controller bandwidth limitation under SPK broadcast mode */
        if (BT_SINK_SRV_MUSIC_MODE_BROADCAST == mode || BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
            uint32_t max_bp = BT_SINK_SRV_A2DP_SBC_MAX_BP_IN_SPEAKER_MODE;
            bt_a2dp_codec_capability_t *sbc_codec = &g_bt_sink_srv_a2dp_codec_list[num];
            sbc_codec->codec.sbc.max_bitpool = (max_bp & 0xFF);
            bt_sink_srv_report_id("[sink][a2dp]switch to broadcast mode:0x%02x, change max bitpool:0x%02x", 2, mode, sbc_codec->codec.sbc.max_bitpool);
        }
#endif
        num++;
#ifdef BT_SINK_SRV_A2DP_AAC_SUPPORT
        if (g_bt_sink_srv_aac_open) {
            bt_sink_srv_a2dp_aac_config_parameter_t aac_config = {
                .aac_type = 0,
                .bit_rate = 0x60000,
            };
            read_size = sizeof(aac_config);
            nvdm_ret = nvkey_read_data(NVID_BT_HOST_MUSIC_AAC_CONFIG, (uint8_t *)(&aac_config), &read_size);
            aac_cap[0].bit_rate = aac_config.bit_rate;
            bt_sink_srv_report_id("[sink][a2dp]aac,bit_rate:0x%08x, type:0x%02x, read_size:%d, nvdm_ret:%d, object_type:0x%02x", 5, aac_config.bit_rate, aac_config.aac_type,
                                  read_size, nvdm_ret, aac_cap[0].object_type);
            if (aac_config.aac_type == BT_SINK_SRV_A2DP_MPEG_2_LC) {
                aac_cap[0].object_type = 0x40;
            } else if (aac_config.aac_type == BT_SINK_SRV_A2DP_MPEG_2_AND_4_LC) {
                aac_cap[0].object_type = 0x60;
            }
            BT_A2DP_MAKE_AAC_CODEC(g_bt_sink_srv_a2dp_codec_list + num, BT_A2DP_SINK,
                                   aac_cap[0].vbr, aac_cap[0].drc, aac_cap[0].object_type, aac_cap[0].channels,
                                   aac_cap[0].sample_rate, aac_cap[0].bit_rate);
            num++;
        }
#endif

#ifndef MTK_BT_A2DP_VENDOR_2_ENABLE
#ifdef MTK_BT_A2DP_VENDOR_CODEC_SUPPORT
        if (g_bt_sink_srv_vendor_codec_open) {
            bool is_add_vendor_codec = false;
#ifdef MTK_BT_SPEAKER_ENABLE
            if (BT_AWS_MCE_SRV_MODE_NORMAL == mode || BT_AWS_MCE_SRV_MODE_SINGLE == mode) {
                is_add_vendor_codec = true;
            }
            else
#endif
            {
                if (BT_SINK_SRV_AWS_MCE_SRV_MODE_INVALID == mode)
                {
                    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                    if (role == BT_AWS_MCE_ROLE_NONE
#ifndef MTK_BT_SPEAKER_ENABLE
                        || role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_PARTNER
#endif
                       ) {
                        is_add_vendor_codec = true;
                    }
                }
            }
            if (is_add_vendor_codec) {
                BT_A2DP_MAKE_VENDOR_CODEC(g_bt_sink_srv_a2dp_codec_list + num, BT_A2DP_SINK,
                                              0x0000012d, 0x00aa, 0x3c, 0x07);
                num++;
            }
        }
#endif
#else
        if (g_bt_sink_srv_vendor_codec_open) {
            BT_A2DP_MAKE_VENDOR_CODEC_2(g_bt_sink_srv_a2dp_codec_list + num, BT_A2DP_SINK,
                                        0x00000094, 0x4c33, 0x14, 0x61);
            num++;
        }
#endif
        param->sink_feature = 0x0F;
        param->source_feature = 0x00;
        param->codec_number = num;
        param->codec_list = g_bt_sink_srv_a2dp_codec_list;
#ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
        param->sink_delay = BT_SINK_SRV_A2DP_DELAY;
#endif

        ret = BT_STATUS_SUCCESS;
        bt_sink_srv_report_id("[sink][a2dp]get_params_by_mode: %d, num:%d, mode:0x%02x", 3, ret, param->codec_number, mode);
    }
    return ret;
}

#ifdef MTK_BT_SPEAKER_ENABLE
bt_status_t bt_sink_srv_a2dp_mode_changed_handle_callback(bt_cm_event_t event_id, void *params, uint32_t params_len)
{
    int32_t ret = BT_STATUS_FAIL;
    bt_sink_srv_report_id("[sink][a2dp]mode changed callback, event id:0x%x", 1, event_id);
    if (BT_AWS_MCE_SRV_EVENT_MODE_CHANGED_IND == event_id) {
        bt_aws_mce_srv_mode_changed_ind_t *mode_cnf = (bt_aws_mce_srv_mode_changed_ind_t *)params;
        bt_a2dp_init_params_t codec_param;
        ret = bt_sink_srv_a2dp_get_params_by_mode(mode_cnf->mode, &codec_param);
        if (BT_STATUS_SUCCESS == ret) {
            bt_a2dp_update_sep(&codec_param);
        }
        if (BT_AWS_MCE_SRV_MODE_BROADCAST == mode_cnf->mode) {
            bt_a2dp_set_mtu_size(672);
            bt_sink_srv_music_set_sink_latency(BT_SINK_SRV_A2DP_SPEAKER_MODE_DEFAULT_SINK_LATENCY, true);
        } else if (BT_AWS_MCE_SRV_MODE_DOUBLE == mode_cnf->mode) {
            bt_a2dp_set_mtu_size(895);
            bt_sink_srv_music_set_sink_latency(BT_SINK_SRV_A2DP_DEFAULT_SINK_LATENCY, true);
        } else if (BT_AWS_MCE_SRV_MODE_NORMAL == mode_cnf->mode || BT_AWS_MCE_SRV_MODE_SINGLE == mode_cnf->mode) {
            bt_a2dp_set_mtu_size(895);
            bt_sink_srv_music_set_sink_latency(BT_SINK_SRV_A2DP_DEFAULT_SINK_LATENCY, true);
        }
        bt_sink_srv_report_id("[sink][a2dp]mode changed to 0x%02x, ret: %d", 2, mode_cnf->mode, ret);
    }
    return ret;
}
#endif /*MTK_BT_SPEAKER_ENABLE*/
bt_status_t bt_sink_srv_a2dp_get_init_params(bt_a2dp_init_params_t *param)
{
    int32_t ret = BT_STATUS_FAIL;

    if (param) {
        bt_sink_srv_a2dp_get_params_by_mode(BT_SINK_SRV_AWS_MCE_SRV_MODE_INVALID, param);
        ret = BT_STATUS_SUCCESS;
    }
    g_bt_sink_srv_a2dp_init_complete = true;
    
    bt_sink_srv_report_id("[sink][a2dp]get_init_params: %d, param:%d\n", 2, ret, param);
    return ret;
}

#ifdef BT_SINK_SRV_A2DP_AAC_SUPPORT
void bt_sink_srv_a2dp_enable_aac(bool enable_flag)
{
    g_bt_sink_srv_aac_open = enable_flag;
    bt_a2dp_init_params_t init_param;
    bool temp_a2dp_init_flag = g_bt_sink_srv_a2dp_init_complete;
    bt_sink_srv_a2dp_get_init_params(&init_param);
    if (temp_a2dp_init_flag) {
        bt_a2dp_update_sep(&init_param);
    }
    bt_sink_srv_report_id("[sink][a2dp]to open aac : %d", 1, enable_flag);
}
#endif

#if defined(MTK_BT_A2DP_VENDOR_2_ENABLE) || defined(MTK_BT_A2DP_VENDOR_CODEC_SUPPORT)
void bt_sink_srv_a2dp_enable_vendor_codec(bool enable_flag)
{
    bt_sink_srv_report_id("[sink][a2dp]to open vendor codec : %d=>%d, sync:0x%x", 2, g_bt_sink_srv_vendor_codec_open, enable_flag);
    if (enable_flag != g_bt_sink_srv_vendor_codec_open) {
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        if (BT_AWS_MCE_ROLE_AGENT == bt_connection_manager_device_local_info_get_aws_role()) {
            bt_sink_srv_aws_mce_vendor_codec_config_sync(enable_flag);
        }
#endif
        g_bt_sink_srv_vendor_codec_open = enable_flag;
        bt_a2dp_init_params_t init_param;
        bool temp_a2dp_init_flag = g_bt_sink_srv_a2dp_init_complete;
        bt_sink_srv_a2dp_get_init_params(&init_param);
        if (temp_a2dp_init_flag) {
            bt_a2dp_update_sep(&init_param);
        }
    }
}

bool bt_sink_srv_a2dp_get_vendor_codec_config(void)
{
    return g_bt_sink_srv_vendor_codec_open;
}
#endif

static void bt_sink_srv_a2dp_init()
{
    bt_sink_srv_music_context_t *ctx = NULL;

    ctx = bt_sink_srv_music_get_context();

    if (ctx->a2dp_aid != BT_SINK_SRV_INVALID_AID) {
        bt_sink_srv_ami_audio_close(ctx->a2dp_aid);
        ctx->a2dp_aid = BT_SINK_SRV_INVALID_AID;
    }

    ctx->a2dp_aid = bt_sink_srv_ami_audio_open(AUD_MIDDLE, bt_sink_srv_music_a2dp_common_ami_hdr);
    bt_sink_srv_report_id("[sink][a2dp]init-a2dp_aid: %d", 1, ctx->a2dp_aid);
    avm_direct_clear_drift_parameters();
#ifdef MTK_BT_SPEAKER_ENABLE
    if (BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
#ifdef MTK_BT_SPEAKER_FEC_ENABLE
        speaker_fec_init();
#endif
        bt_sink_srv_music_set_sink_latency(BT_SINK_SRV_A2DP_SPEAKER_MODE_DEFAULT_SINK_LATENCY, true);
    } else {
        bt_sink_srv_music_set_sink_latency(BT_SINK_SRV_A2DP_DEFAULT_SINK_LATENCY, true);
    }
    ctx->context_flag = 0;
#else
    bt_sink_srv_music_set_sink_latency(BT_SINK_SRV_A2DP_DEFAULT_SINK_LATENCY, true);
#endif
#ifdef BT_SINK_SRV_MUSIC_ADD_GPIO
    bt_sink_srv_music_init_gpio(HAL_GPIO_37);
    bt_sink_srv_music_init_gpio(HAL_GPIO_38);
    bt_sink_srv_music_init_gpio(HAL_GPIO_39);
    bt_sink_srv_music_init_gpio(HAL_GPIO_40);
#endif
    //bt_sink_srv_a2dp_3M_enable_config();

#if defined(__GNUC__)
    bt_sink_srv_report("[sink][a2dp]--Version: %s", "__GNUC__");
#endif

#if defined(__ARMCC_VERSION)
    bt_sink_srv_report("[sink][a2dp]--Version: %s", "__ARMCC_VERSION");
#endif

#if defined(__ICCARM__)
    bt_sink_srv_report("[sink][a2dp]--Version: %s", "__ICCARM__");
#endif
}

#ifdef MTK_BT_SPEAKER_ENABLE
void bt_sink_srv_a2dp_update_base_timer(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_report("[sink][a2dp]bt_sink_srv_a2dp_update_base_timer, update_base_count:%d", 1, ctx->update_base_count);
    if (ctx->run_dev) {
        ctx->update_base_count++;
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_UPDATE_BASE, (void *)(&(ctx->run_dev->a2dp_hd)));
        if (ctx->update_base_count < BT_SINK_SRV_MUSIC_MAX_UPDATE_BASE_COUNT)
            bt_timer_ext_start(BT_SINK_SRV_A2DP_SEND_BASE_IND_TIMER_ID, 0,
                               BT_SINK_SRV_A2DP_SEND_BASE_IND_TIMER_DUR, bt_sink_srv_a2dp_update_base_timer);
    }
}
#endif

void bt_sink_srv_a2dp_ami_hdr(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
    bt_sink_srv_music_context_t *ctx = NULL;
    int32_t ret = 0;
    bt_sink_srv_music_device_t *run_dev = NULL;

    ctx = bt_sink_srv_music_get_context();
    run_dev = ctx->run_dev;
    //bt_utils_assert(run_dev);
    if ((ctx->a2dp_aid == aud_id) &&
        (msg_id == AUD_A2DP_PROC_IND) &&
        (sub_msg == AUD_STREAM_EVENT_DATA_REQ ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW ||
         sub_msg == AUD_CODEC_MEDIA_AWS_CHECK_UNDERFLOW)) {
        return;
    }
    bt_sink_srv_report_id("[sink][a2dp]ami_hdr[s]-aid: %d, aud_id: %d, msg_id: %d, sub_msg: %d, 2nd: 0x%x", 5,
                          ctx->a2dp_aid, aud_id, msg_id, sub_msg, sub_msg);
    if (ctx->a2dp_aid == aud_id) {
        switch (msg_id) {
            case AUD_SINK_OPEN_CODEC: {
                if ((run_dev) && (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC)) {
                    /* Save codec handle */
                    bt_sink_srv_memcpy(&(run_dev->med_handle), param, sizeof(bt_sink_srv_am_media_handle_t));

                    BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);

                    /* Set codec open flag */
                    BT_SINK_SRV_SET_FLAG(run_dev->op, BT_SINK_SRV_MUSIC_OP_CODEC_OPEN);

                    // TODO: check aws set R/L channel

                    if (run_dev->flag & BT_SINK_SRV_MUSIC_WAIT_SET_VOLUME) {
                        bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, ctx->vol_lev, STREAM_OUT);
                        BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_WAIT_SET_VOLUME);
                    }

                    if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC ==
                        run_dev->handle->substate || (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_RHO_HAPPEN_DURING_STARTING_PLAY)) {
                        if (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC) {
                            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);
                        }

                        if (run_dev->flag & BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START) {
                            bt_sink_srv_report_id("[sink][a2dp] need to response a2dp start", 0);
                            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
                            bt_a2dp_start_streaming_response(run_dev->a2dp_hd, true);
                        }
                        bt_sink_srv_report_id("[sink][a2dp] to clear codec", 0);
                        if (!(run_dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
                            bt_sink_srv_cm_profile_status_notify(&run_dev->dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, BT_STATUS_SUCCESS);
                        }
                        bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
                        if ((run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_RHO_HAPPEN_DURING_STARTING_PLAY)
                            || (run_dev->op & BT_SINK_SRV_MUSIC_WAITING_REINITIAL_SYNC)) {

                            bt_sink_srv_a2dp_reinitial_sync();
                            BT_SINK_SRV_REMOVE_FLAG(run_dev->op, BT_SINK_SRV_MUSIC_WAITING_REINITIAL_SYNC);
                            BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_RHO_HAPPEN_DURING_STARTING_PLAY);
                        }
                        break;
                    }
#ifdef BT_SINK_SRV_SET_AUDIO_CHANNEL_BY_SINK_MUSIC
                    bool aws_attached = false;

                    uint8_t channel_num = HAL_AUDIO_STEREO;

#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
                    uint32_t aws_handle = 0;
                    aws_handle = bt_sink_srv_music_get_aws_handle_by_addr(&run_dev->dev_addr);
                    bt_sink_srv_music_device_t *aws_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_AWS_A2DP_HD, (void *)(&aws_handle));
                    if (aws_dev) {
                        aws_attached = aws_dev->conn_bit & BT_SINK_SRV_MUSIC_AWS_CONN_BIT ? true : false;
                    }
#endif
                    if (AUDIO_CHANNEL_L == ami_get_audio_channel() && aws_attached) {
                        channel_num = HAL_AUDIO_STEREO_BOTH_L_CHANNEL;
                    } else if (AUDIO_CHANNEL_R == ami_get_audio_channel() && aws_attached) {
                        channel_num = HAL_AUDIO_STEREO_BOTH_R_CHANNEL;
                    } else {
                        channel_num = HAL_AUDIO_STEREO;
                    }

                    bt_sink_srv_report_id("[sink][a2dp]to set channel:%d", 1, channel_num);
#endif
#ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
                    if (run_dev->codec.type == BT_A2DP_CODEC_VENDOR) {
                        bt_sink_srv_music_update_audio_buffer(SHARE_BUFFER_VENDOR_CODEC_SIZE);
                    } else {
                        bt_sink_srv_music_update_audio_buffer(SHARE_BUFFER_BT_AUDIO_DL_SIZE);
                    }
#else
                    bt_sink_srv_music_update_audio_buffer(SHARE_BUFFER_BT_AUDIO_DL_SIZE);
#endif
                    /*set n pkt should before rsp to sp*/
#ifdef __BT_SINK_SRV_A2DP_AVM_DIRECT_SUPPORT__
#if defined (AIR_LE_AUDIO_ENABLE) && defined (AIR_LE_AUDIO_CIS_ENABLE) && !defined(AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE)
                    bt_handle_t le_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
                    if (le_handle != BT_HANDLE_INVALID) {
                        bt_sink_srv_cap_stream_release_autonomously(le_handle, 0xFF, false, 0);
                    }
#endif
                    uint32_t gap_hd = 0;
                    gap_hd = bt_sink_srv_cm_get_gap_handle(&(run_dev->dev_addr));
                    bt_utils_assert(gap_hd);
                    bt_sink_srv_music_set_music_enable(gap_hd, BT_AVM_ROLE_AWS_MCE_AGENT, true);
                    if (run_dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
                        uint16_t n_value = BT_SINK_SRV_A2DP_N_PACKET_NOTIFY;
                        bool ret = bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)&run_dev->dev_addr, BT_IOT_MUSIC_NOTIFY_VENDOR_CODEC_BY_N_PACKET);
                        bt_sink_srv_music_mode_t music_mode = bt_sink_srv_music_get_mode(&run_dev->dev_addr);
                        if (run_dev->codec.type == BT_A2DP_CODEC_VENDOR || ret || music_mode == BT_SINK_SRV_MUSIC_GAME_MODE) {
                            n_value = BT_SINK_SRV_A2DP_N_PACKET_NOTIFY_VENDOR_CODEC;
#ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
                        } else if (run_dev->codec.codec.vendor.codec_id == 0x4c33) {
                            n_value =  BT_SINK_SRV_A2DP_VENDOR_2_N_PACKET_NOTIFY;
#endif
                        }
                        if (run_dev->codec.sec_type) {
                            n_value |= BT_SINK_SRV_MUSIC_CONTENT_PROTECTION_MASK;
                        }
#ifdef AIR_A2DP_REINIT_V2_ENABLE
                        if (run_dev->codec.type == BT_A2DP_CODEC_SBC
                            && bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)&run_dev->dev_addr, BT_IOT_MUSIC_IS_WALKMAN_SET_RATIO_AND_PCDC_OBSERVATION)) {
                            n_value |= BT_SINK_SRV_A2DP_SHORT_PCDC_OBSERVATION;
                            bt_media_handle_t *med_hd = run_dev->med_handle.med_hd;
                            med_hd->set_ts_ratio(med_hd, 0xfffffffe);
                            bt_sink_srv_report_id("[sink][a2dp]:n_value:0x%x", 1, n_value);
                        }
#endif
                        bt_avm_set_a2dp_notify_condition(gap_hd, n_value);
                        BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_N_PKT_DONE);
                    }
#endif
                    if (run_dev->flag & BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START) {
                        BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
                        ret = bt_a2dp_start_streaming_response(run_dev->a2dp_hd, true);
                    } else {
                        ret = BT_STATUS_SUCCESS;
                    }
                    if (BT_STATUS_SUCCESS == ret) {

                        if (BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC != run_dev->handle->substate) {
#ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
                            if (run_dev->codec.delay_report) {
                                if (!(run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC) || (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_ALC_REINITIAL_SYNC)) {
                                    BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_ALC_REINITIAL_SYNC);
                                    int32_t de_ret = 0;
                                    (void)de_ret;
                                    uint32_t delay_report = avm_direct_get_sink_latency();
                                    delay_report = delay_report / 100;
                                    de_ret = bt_a2dp_set_delay_report(run_dev->a2dp_hd, delay_report);
                                    bt_sink_srv_report_id("[sink][a2dp]delay_report--ret: 0x%08x", 1, de_ret);
                                }
                            }

                            if (run_dev->codec.sec_type) {
                                bt_sink_srv_report_id("[sink][a2dp]set content protection", 0);
                                bt_sink_srv_am_media_handle_t *med_handle = &(run_dev->med_handle);
                                med_handle->med_hd->set_content_protection(med_handle->med_hd, true);
                            }
#endif
#ifdef BT_SINK_SRV_SET_AUDIO_CHANNEL_BY_SINK_MUSIC
                            hal_audio_set_stream_out_channel_number(channel_num);
#endif
                        }

                        bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_CODEC_OPEN, NULL);
#ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
                        if (run_dev->codec.type == BT_A2DP_CODEC_VENDOR) {
                            bt_sink_srv_music_drv_play(ctx->run_dev);
#ifdef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
                            bt_clock_t pta;
                            pta.nclk = 0;
                            pta.nclk_intra = 0;
                            bt_avm_set_audio_tracking_time(gap_hd, BT_AVM_TYPE_A2DP, &pta);
#endif
                            bt_sink_srv_music_state_machine_handle(ctx->run_dev, BT_SINK_SRV_MUSIC_EVT_PLAYING, NULL);
                        }
#endif
                    } else {
                        /* Error handle */
                        bt_sink_srv_music_state_machine_handle(run_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
                    }
                }
                break;
            }
            case AUD_A2DP_PROC_IND: {
                if (run_dev) {
                    switch (sub_msg) {
                        case AUD_A2DP_CODEC_RESTART:
                        case AUD_A2DP_AWS_UNDERFLOW: {
                            break;
                        }

                        case AUD_A2DP_DL_REINIT_REQUEST: {
                            BT_SINK_SRV_GPIO_SET_OUTPUT(HAL_GPIO_40, HAL_GPIO_DATA_HIGH);
#ifdef AIR_A2DP_SYNC_STOP_ENABLE
                            if (run_dev && run_dev->handle->substate == BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC) {
                                break;
                            }
#endif
#ifndef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
                            bt_timer_ext_t *timer_ctx = bt_timer_ext_find(BT_SINK_SRV_REINITIAL_SYNC_TIMER_ID);
#endif
                            bt_sink_srv_music_dsp_parameter_t *dsp_parameter = (bt_sink_srv_music_dsp_parameter_t *)param;
                            bt_sink_srv_report_id("[sink][a2dp]reinitial sync para_data:%d", 1, *dsp_parameter);
                            if (*dsp_parameter == BT_SINK_SRV_MUSIC_REINIT_PARTNER_LATER_JOIN && run_dev) {
                                BT_SINK_SRV_SET_FLAG(run_dev->op, BT_SINK_SRV_MUSIC_REINIT_ON_PARTNER_LATER_JOIN_FLAG);
                                break;
                            }
                            if (!ctx->rho_flag
#ifndef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
                                && !timer_ctx
#endif
                               ) {
#ifndef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
                                bt_timer_ext_start(BT_SINK_SRV_REINITIAL_SYNC_TIMER_ID, 0,
                                                   BT_SINK_SRV_REINITIAL_SYNC_TIMER_DUR, NULL);
#endif
                                bt_sink_srv_a2dp_reinitial_sync();
                            } else {
                                bt_sink_srv_am_id_t ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, ctx->vol_lev, STREAM_OUT);
                                (void)ami_ret;
                                bt_sink_srv_report_id("[sink][a2dp]Just to set volume, ami_ret:%d, rho_flag:0x%02x", 2, ami_ret,
                                                      ctx->rho_flag);
                            }
                            BT_SINK_SRV_GPIO_SET_OUTPUT(HAL_GPIO_40, HAL_GPIO_DATA_LOW);
                            break;
                        }

                        case AUD_A2DP_ACTIVE_LATENCY_REQUEST: {
#ifdef AIR_A2DP_SYNC_STOP_ENABLE
                            if (run_dev && run_dev->handle->substate == BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC) {
                                break;
                            }
#endif
                            bool vendor_codec = (run_dev->codec.type == BT_A2DP_CODEC_VENDOR);
                            uint32_t latency_val = bt_sink_srv_get_latency(&run_dev->dev_addr, vendor_codec, true, false);
                            uint32_t original_latency = bt_sink_srv_music_get_sink_latency();
                            if (BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
                                latency_val = bt_sink_srv_get_latency(&run_dev->dev_addr, vendor_codec, true, true);
                            }
                            bt_sink_srv_report_id("[sink][a2dp] DSP trigger ALC, rho_flag:0x%02x, new_latency: %d, ori_latency: %d",
                                                  3, ctx->rho_flag, latency_val, original_latency);

                            if (!ctx->rho_flag) {
                                if (bt_sink_srv_music_get_ALC_enable()) {
                                    if (bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)&run_dev->dev_addr, BT_IOT_MUSIC_SET_LATENCY_TO_ORIGINAL)) {
                                        latency_val = original_latency;
                                        bt_sink_srv_report_id("[sink][a2dp] DSP trigger ALC special deivce, new_latency: %d", 1, latency_val);
                                    }
                                }
                                bt_sink_srv_music_set_sink_latency(latency_val, false);
                                bt_sink_srv_a2dp_reinitial_sync();
                                BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_ALC_REINITIAL_SYNC);
                            } else {
                                bt_sink_srv_am_id_t ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, ctx->vol_lev, STREAM_OUT);
                                (void)ami_ret;
                                bt_sink_srv_report_id("[sink][a2dp] during RHO, just to set volume, ami_ret:%d", 1, ami_ret);
                            }
                            break;
                        }

                        default:
                            break;
                    }
                } else {
                    bt_sink_srv_report_id("[sink][a2dp]ami_hdr--empty run_dev, plase note!!!", 0);
                }
                break;
            }
            default:
                break;
        }
    }

    bt_sink_srv_report_id("[sink][a2dp]ami_hdr[e]-ret: %d", 1, ret);
}

void bt_sink_srv_a2dp_initial_avrcp_timer(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_music_device_t *dev = (bt_sink_srv_music_device_t *)data;
    bt_sink_srv_report_id("[sink][a2dp]initial avrcp timer, conn_bit:0x%04x, hd;0x%08x", 2,
                          dev->conn_bit, dev->avrcp_hd);
    bt_utils_assert(dev && "initial_avrcp_timer");
    if (dev->avrcp_hd == BT_SINK_SRV_MUSIC_INVALID_HD &&
        !(dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT)) {
        uint32_t hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        bt_sink_srv_init_role(BT_AVRCP_ROLE_CT);//[TD-PLAYER]
        bt_status_t ret = bt_avrcp_connect(&hd, (const bt_bd_addr_t *)(&(dev->dev_addr)));
        if (BT_STATUS_SUCCESS == ret) {
            BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AVRCP_CONN_RESULT);
            dev->avrcp_hd = hd;
            dev->avrcp_role = BT_AVRCP_ROLE_CT;//[TD-PLAYER]
        }
        bt_sink_srv_report_id("[sink][a2dp]connect_cnf-ret: %d, avrcp_hd: 0x%x", 2, ret, hd);
    }
}

void bt_sink_srv_a2dp_disconnect_avrcp_timer(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_music_device_t *dev = (bt_sink_srv_music_device_t *)data;
    bt_sink_srv_report_id("[sink][a2dp]deinitial avrcp timer, conn_bit:0x%04x, hd;0x%08x", 2,
                          dev->conn_bit, dev->avrcp_hd);
    if ((dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT) && !(dev->conn_bit &
                                                                BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
        bt_status_t ret = bt_avrcp_disconnect(dev->avrcp_hd);
        (void)ret;
        bt_sink_srv_report_id("[sink][a2dp]ret:0x%08x", 1, ret);
    }
}

#ifdef MTK_BT_SPEAKER_ENABLE
void bt_sink_srv_a2dp_send_play_ind_timer(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_report_id("[sink][a2dp]run_dev:0x%08x", 1, ctx->run_dev);
    if (ctx->run_dev) {
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_PLAY_IND, (void *)(&(ctx->run_dev->a2dp_hd)));
        bt_timer_ext_start(BT_SINK_SRV_A2DP_SEND_PLAY_IND_TIMER_ID, 0,
                           BT_SINK_SRV_A2DP_SEND_PLAY_IND_TIMER_DUR, bt_sink_srv_a2dp_send_play_ind_timer);
    } else {
        bt_timer_ext_stop(BT_SINK_SRV_A2DP_SEND_PLAY_IND_TIMER_ID);
    }
}
#endif

int32_t bt_sink_srv_a2dp_handle_connect_cnf(bt_a2dp_connect_cnf_t *conn_cnf)
{
    bt_sink_srv_music_device_t *dev = NULL;
    //bt_sink_srv_music_context_t *ctx = NULL;
    int32_t ret = 0;
    bool connected = false;
    bt_bd_addr_t dev_addr;
    uint32_t hd = 0;

    //ctx = bt_sink_srv_music_get_context();

    bt_sink_srv_report_id("[sink][a2dp]connect_cnf-a2dp_hd: 0x%x", 1, conn_cnf->handle);

    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(conn_cnf->handle)));

    if (!dev) {
        return ret;
    }

    bt_sink_srv_memcpy(&dev_addr, &(dev->dev_addr), sizeof(bt_bd_addr_t));
    dev->a2dp_status = BT_SINK_SRV_A2DP_STATUS_SUSPEND;
    if (conn_cnf->status == BT_STATUS_SUCCESS) {
        /* Save codec capability */
        memcpy(&(dev->codec), (conn_cnf->codec_cap), sizeof(bt_a2dp_codec_capability_t));

        bt_sink_srv_report_id("[sink][a2dp]connect_cnf, codec_type:%d, codec_type: %d", 2,
                              dev->codec.type, conn_cnf->codec_cap->type);

        /* As slave */

        //bt_sink_srv_cm_set_role(&(dev->dev_addr), BT_ROLE_SLAVE);
        BT_SINK_SRV_SET_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);

        bt_sink_srv_report_id("[sink][a2dp]connect_cnf-con_bit: 0x%04x, flag:0x%08x", 2,
                              dev->conn_bit, dev->flag);
        if (BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP & dev->flag) {
            if (dev->avrcp_hd == BT_SINK_SRV_MUSIC_INVALID_HD) {
                bt_sink_srv_init_role(BT_AVRCP_ROLE_CT);//[TD-PLAYER]
                ret = bt_avrcp_connect(&hd, (const bt_bd_addr_t *)(&(dev->dev_addr)));
                if (BT_STATUS_SUCCESS == ret) {
                    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AVRCP_CONN_RESULT);
                    dev->avrcp_hd = hd;
                    dev->avrcp_role = BT_AVRCP_ROLE_CT;//[TD-PLAYER]
                }
            }
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP);

            bt_sink_srv_report_id("[sink][a2dp]connect_cnf-ret: %d, avrcp_hd: 0x%x", 2, ret, hd);
        } else {
            if ((!(dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT))
                && (0 == bt_sink_srv_music_get_context()->rho_flag)) {
                uint32_t data = (uint32_t)dev;
                bt_timer_ext_start(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID, data,
                                   BT_SINK_SRV_AVRCP_CONNECTION_TIMER_DUR, bt_sink_srv_a2dp_initial_avrcp_timer);
            }
        }
        connected = true;
        /* State machine handle */
        bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_READY, NULL);
    } else {
        /* State machine handle */
        bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_CONNECT_CNF, conn_cnf);
        BT_SINK_SRV_REMOVE_FLAG(dev->conn_bit, BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_INITIAL_A2DP_BY_DEVICE);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP);
        if (!(dev->conn_bit) && dev->avrcp_hd == BT_SINK_SRV_MUSIC_INVALID_HD) {
            bt_sink_srv_a2dp_free_pseudo_handle(dev->handle);
            dev->handle = NULL;
            bt_sink_srv_music_reset_device(dev);
        } else {
            dev->a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        }
    }
    if (connected) {
        //void bt_sink_srv_aws_mce_init_eir(bt_bd_addr_t *remote_address);
        bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED, conn_cnf->status);

#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_CONNECTION, (void *)(&(conn_cnf->handle)));
#endif
    } else {
        bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, conn_cnf->status);
    }
    return ret;
}


int32_t bt_sink_srv_a2dp_handle_connect_ind(bt_a2dp_connect_ind_t *conn_ind)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;
    uint8_t *addr = (uint8_t *)conn_ind->address;
    (void)addr;

    bt_sink_srv_report_id("[sink][a2dp]connect_ind-a2dp_hd: 0x%x, addr--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 7, conn_ind->handle, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    /* A2dp connected */
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP, (void *)(conn_ind->address));
    if (!dev) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)(conn_ind->address));
    }
    if (dev == NULL) {
        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP);
        if (dev) {
            dev->handle = bt_sink_srv_a2dp_alloc_pseudo_handle();
            dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP;
            dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(conn_ind->address);
            dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
        } else {
            bt_sink_srv_report_id("[sink][a2dp]Error, dev NULL", 0);
            bt_utils_assert(0 && "dev NULL");
        }
    }

    bt_utils_assert(dev && "Error: a2dp dev NULL");
    /* Init pse handle */
    dev->a2dp_hd = conn_ind->handle;
    dev->role = conn_ind->role;
    memcpy(&(dev->dev_addr), conn_ind->address, sizeof(bt_bd_addr_t));
    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_CON_IND);

    ret = bt_a2dp_connect_response(conn_ind->handle, true);
    bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_CONNECT_IND, conn_ind);

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_disconnect_cnf(bt_a2dp_disconnect_cnf_t *disconn_cnf)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_bd_addr_t dev_addr = {0};
    int32_t ret = 0;

    bt_sink_srv_report_id("[sink][a2dp]disconnect_cnf-hd: 0x%x", 1, disconn_cnf->handle);

#ifdef MTK_BT_SPEAKER_ENABLE
    if (BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
        bt_timer_ext_stop(BT_SINK_SRV_A2DP_SEND_BASE_IND_TIMER_ID);
        bt_timer_ext_stop(BT_SINK_SRV_A2DP_SEND_PLAY_IND_TIMER_ID);
    }
#endif
    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(disconn_cnf->handle)));
    if (a2dp_dev && a2dp_dev->handle) {
        /* force set a2dp suspend when disconnect */
        a2dp_dev->a2dp_status = BT_SINK_SRV_A2DP_STATUS_SUSPEND;
        bt_sink_srv_report_id("[sink_a2dp_flow]sp_disconnect_cnf delete waiting list handle: 0x%08x", 1, a2dp_dev->handle);
        audio_src_srv_del_waiting_list(a2dp_dev->handle);
        bt_sink_srv_set_must_play_tone_flag(&a2dp_dev->dev_addr, BT_SINK_SRV_INIT_NOTIFICATION_VOICE, false);

        if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_LIST_SINK_PLAY) {
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_LIST_SINK_PLAY);
        }
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_STOP_MUSIC_PENDING);
        bt_sink_srv_memcpy(&dev_addr, &(a2dp_dev->dev_addr), sizeof(bt_bd_addr_t));
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->conn_bit, BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_INITIAL_A2DP_BY_DEVICE);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAITING_START);
        /* Clear recover flag */
#ifndef AIR_BT_MUSIC_DISABLE_NOTIFICATION_VOICE_OPTIMIZATION
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_GAME_FLAG);
#endif

        if ((a2dp_dev->op & BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT)) {
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
            //audio_src_srv_del_waiting_list(a2dp_dev->handle);
        }
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);
        a2dp_dev->a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
        /* Music state machine handle first */
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_A2DP_DISCONNECT_CNF, NULL);
    } else {
        bt_sink_srv_report_id("[sink][a2dp]disconnect_cnf, no device.", 0);
        return 0;
    }

    /* Deinit pse handle */
    if (a2dp_dev->handle && !(a2dp_dev->conn_bit) &&
        BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC != a2dp_dev->handle->substate
        && a2dp_dev->avrcp_hd == BT_SINK_SRV_MUSIC_INVALID_HD) {
        bt_sink_srv_a2dp_free_pseudo_handle(a2dp_dev->handle);
        a2dp_dev->handle = NULL;
        bt_sink_srv_music_reset_device(a2dp_dev);
    }
    bool notify_n9_dis = true;
    if (a2dp_dev->handle && BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == a2dp_dev->handle->substate) {
        notify_n9_dis = false;
    }
    if (notify_n9_dis) {
        bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, disconn_cnf->status);
    }


#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    if (a2dp_dev) {
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_DISCONNECTION, (void *)(&dev_addr));
    }
#endif

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_disconnect_ind(bt_a2dp_disconnect_ind_t *disconn_ind)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    bt_bd_addr_t dev_addr = {0};
    int32_t ret = 0;

    bt_sink_srv_report_id("[sink][a2dp]disconnect_ind-hd: 0x%x", 1, disconn_ind->handle);

    bt_timer_ext_stop(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID);
#ifdef MTK_BT_SPEAKER_ENABLE
    if (BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
        bt_timer_ext_stop(BT_SINK_SRV_A2DP_SEND_BASE_IND_TIMER_ID);
        bt_timer_ext_stop(BT_SINK_SRV_A2DP_SEND_PLAY_IND_TIMER_ID);
    }
#endif
    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(disconn_ind->handle)));
    if (!a2dp_dev) {
        bt_sink_srv_report_id("[sink][a2dp]It has been disconnected!", 0);
        return ret;
    }
    /* force set a2dp suspend when disconnect */
    a2dp_dev->a2dp_status = BT_SINK_SRV_A2DP_STATUS_SUSPEND;
    if (a2dp_dev && a2dp_dev->handle) {
        bt_sink_srv_report_id("[sink_flow] sp_disconnect_ind, delete waiting list hanlde : 0x%08x", 1, a2dp_dev->handle);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_ADD_WAITING_LIST);
        audio_src_srv_del_waiting_list(a2dp_dev->handle);
    }

    if (a2dp_dev && (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_LIST_SINK_PLAY)) {
        //audio_src_srv_del_waiting_list(a2dp_dev->handle);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_LIST_SINK_PLAY);
    }

    if (a2dp_dev && (a2dp_dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
        bt_sink_srv_memcpy(&dev_addr, &(a2dp_dev->dev_addr), sizeof(bt_bd_addr_t));
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->conn_bit, BT_SINK_SRV_MUSIC_A2DP_CONN_BIT);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_INITIAL_A2DP_BY_DEVICE);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_STOP_MUSIC_PENDING);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAITING_START);
        /* Clear recover flag */
#ifndef AIR_BT_MUSIC_DISABLE_NOTIFICATION_VOICE_OPTIMIZATION
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_GAME_FLAG);
#endif
        bt_sink_srv_set_must_play_tone_flag(&a2dp_dev->dev_addr, BT_SINK_SRV_INIT_NOTIFICATION_VOICE, false);

        if ((a2dp_dev->op & BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT)) {
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
            //audio_src_srv_del_waiting_list(a2dp_dev->handle);
        }

        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);
        /* Music state machine handle first */
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_A2DP_DISCONNECT_IND, NULL);
        a2dp_dev->a2dp_hd = BT_SINK_SRV_MUSIC_INVALID_HD;
    }
    uint32_t data = (uint32_t)a2dp_dev;
    if (a2dp_dev->conn_bit & BT_SINK_SRV_MUSIC_AVRCP_CONN_BIT) {
        if (a2dp_dev->avrcp_flag & BT_SINK_SRV_INIT_AVRCP_BY_REMOTE_DEVICE) {
            bt_timer_ext_start(BT_SINK_SRV_AVRCP_DISCONNECT_TIMER_ID, data,
                               BT_SINK_SRV_AVRCP_DISCONNECT_TIMER_DUR, bt_sink_srv_a2dp_disconnect_avrcp_timer);
        } else {
            ret = bt_avrcp_disconnect(a2dp_dev->avrcp_hd);
        }
    } else if ((a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AVRCP_CONN_RESULT)) {
        BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_INIT_DISCONNNECT_AVRCP_TIMER_FLAG);
    }
    /* Deinit pse handle */
    if (a2dp_dev->handle && !(a2dp_dev->conn_bit) &&
        BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC != a2dp_dev->handle->substate
        && a2dp_dev->avrcp_hd == BT_SINK_SRV_MUSIC_INVALID_HD) {

        bt_sink_srv_a2dp_free_pseudo_handle(a2dp_dev->handle);
        a2dp_dev->handle = NULL;
        bt_sink_srv_music_reset_device(a2dp_dev);
    }
    bool notify_n9_dis = true;
    if (a2dp_dev->handle && BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == a2dp_dev->handle->substate) {
        notify_n9_dis = false;
    }
    if (notify_n9_dis) {
        bt_sink_srv_cm_profile_status_notify(&dev_addr, BT_SINK_SRV_PROFILE_A2DP_SINK, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, BT_STATUS_SUCCESS);
    }

#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    if (a2dp_dev) {
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_DISCONNECTION, (void *)(&dev_addr));
    }
#endif

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_start_streaming_cnf(bt_a2dp_start_streaming_cnf_t *start_cnf)
{
    int32_t ret = 0;
    bt_a2dp_start_streaming_ind_t start_ind;
    bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_cnf->handle)));
    if (dev && start_cnf->status == BT_STATUS_SUCCESS) {
        start_ind.handle = start_cnf->handle;
        start_ind.codec_cap = start_cnf->codec_cap;
        bt_sink_srv_a2dp_handle_start_streaming_ind(&start_ind, true);
    }
    bt_sink_srv_report_id("[sink][a2dp]start_cnf, dev:0x%08x, status:0x%08x", 2, dev, start_cnf->status);
    return ret;
}

static void bt_sink_srv_a2dp_avrcp_pause_music(bt_sink_srv_music_device_t *dev)
{
    bt_device_manager_db_remote_pnp_info_t dev_id_p = {0,0};
    bt_status_t pnp_ret = bt_device_manager_remote_find_pnp_info(dev->dev_addr, &dev_id_p);

    if(BT_STATUS_SUCCESS == pnp_ret && dev_id_p.product_id == 0x1200 && dev_id_p.vender_id == 0x038f && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
        bt_sink_srv_avrcp_force_pause_music(dev);
    } else {
        bt_sink_srv_avrcp_stop_music(dev);
    }
}

#ifdef AIR_BT_FAST_PAIR_SASS_ENABLE
static void bt_sink_srv_a2dp_int_user_conf_timer(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_music_device_t *dev = (bt_sink_srv_music_device_t*)data;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    if (dev) {
        bt_sink_srv_report_id("[sink][a2dp]int_user_conf_timer-dev:0x%08x,a2dp:0x%08x,avrcp:0x%08x,op:0x%08x,flag:0x%08x,role:0x%x", 6,
                               dev, dev->a2dp_status, dev->avrcp_status, dev->op, dev->flag, role);
        if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
            if (BT_AVRCP_STATUS_PLAY_PLAYING == dev->avrcp_status) {
                bt_sink_srv_a2dp_avrcp_pause_music(dev);
            }
        }
    } else {
        bt_sink_srv_report_id("[sink][a2dp]int_user_conf dev==null", 0);
    }
}

bt_status_t bt_sink_srv_a2dp_int_user_conf_set(uint8_t val, bt_bd_addr_ptr_t addr)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_device_t *run_dev = bt_sink_srv_music_get_context()->run_dev;
    bt_utils_assert(addr);
    bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)addr);

    bt_timer_ext_stop(BT_SINK_SRV_A2DP_INT_USER_CONF_TIMER_ID);
    if (dev) {
        if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
            if (run_dev != dev) {
                if (val) {
                    if (BT_SINK_SRV_A2DP_STATUS_STREAMING == dev->a2dp_status) {
                        bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_START_STREAMING_IND, NULL);
                        ret = BT_STATUS_SUCCESS;
                    }
                } else {
                    if (BT_AVRCP_STATUS_PLAY_PLAYING == dev->avrcp_status) {
                        bt_sink_srv_a2dp_avrcp_pause_music(dev);
                        ret = BT_STATUS_SUCCESS;
                    }
                }
            }
        }
    }
    bt_sink_srv_report_id("[sink][a2dp]int_user_conf_set-dev:0x%x, ret:0x%x, val: 0x%x, addr--0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 9,
        dev, ret, val, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    return ret;
}

extern int32_t bt_sink_srv_a2dp_int_user_conf_get(bt_bd_addr_ptr_t coming_addr, bt_bd_addr_ptr_t cur_addr);
static int32_t default_bt_sink_srv_a2dp_int_user_conf_get(bt_bd_addr_ptr_t coming_addr, bt_bd_addr_ptr_t cur_addr)
{
    return BT_SINK_SRV_A2DP_INT_USER_CONF_ALLOW;
}

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:bt_sink_srv_a2dp_int_user_conf_get=default_bt_sink_srv_a2dp_int_user_conf_get")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_a2dp_int_user_conf_get = default_bt_sink_srv_a2dp_int_user_conf_get
#else
#error "Unsupported Platform"
#endif

uint32_t bt_sink_srv_a2dp_int_user_conf_check(bt_sink_srv_music_device_t *dev)
{
    uint32_t ret = BT_SINK_SRV_A2DP_INT_USER_CONF_ALLOW;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    if (ctx->run_dev && dev && ctx->run_dev != dev) {
        if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
            ret = bt_sink_srv_a2dp_int_user_conf_get((bt_bd_addr_ptr_t)(dev->dev_addr), (bt_bd_addr_ptr_t)(ctx->run_dev->dev_addr));
            if (BT_SINK_SRV_A2DP_INT_USER_CONF_PENDING == ret) {
                bt_timer_ext_t *timer_ctx = bt_timer_ext_find(BT_SINK_SRV_A2DP_INT_USER_CONF_TIMER_ID);

                if (!timer_ctx) {
                    bt_timer_ext_start(BT_SINK_SRV_A2DP_INT_USER_CONF_TIMER_ID, (uint32_t)dev,
                        BT_SINK_SRV_A2DP_INT_WAIT_USER_TIMER_DUR, bt_sink_srv_a2dp_int_user_conf_timer);
                } else if (timer_ctx->data != (uint32_t)dev){
                    bt_timer_ext_stop(BT_SINK_SRV_A2DP_INT_USER_CONF_TIMER_ID);
                }
            } else if (BT_SINK_SRV_A2DP_INT_USER_CONF_DISALLOW == ret) {
                if (BT_AVRCP_STATUS_PLAY_PLAYING == dev->avrcp_status)
                bt_sink_srv_a2dp_avrcp_pause_music(dev);
            }
        }
    }
    bt_sink_srv_report_id("[sink][a2dp]int_user_conf_check-run_dev:0x%x, dev:0x%x, role:0x%x, ret:0x%x", 4,
                           ctx->run_dev, dev, role, ret);
    return ret;
}
#endif
static int32_t bt_sink_srv_a2dp_handle_start_streaming_ind(bt_a2dp_start_streaming_ind_t *start_ind, bool is_cnf)
{
    bt_sink_srv_music_device_t *dev = NULL;
    int32_t ret = 0;
    const audio_src_srv_handle_t *audio_src_running = audio_src_srv_get_runing_pseudo_device();
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
    bt_timer_ext_status_t timer_ret = BT_TIMER_EXT_STATUS_SUCCESS;
#endif
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(start_ind->handle)));
    if (!dev) {
        bt_utils_assert(dev);
        return ret;
    }
    bt_sink_srv_memcpy(&(dev->codec), start_ind->codec_cap, sizeof(bt_a2dp_codec_capability_t));
    dev->a2dp_status = BT_SINK_SRV_A2DP_STATUS_STREAMING;
    if (!is_cnf) {
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
    }
    if ((!(dev->avrcp_flag & BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG))
        && (ctx->context_flag & BT_SINK_SRV_CNTX_FLAG_MUST_PLAY_RING_TONE_FLAG)) {
        BT_SINK_SRV_SET_FLAG(dev->avrcp_flag, BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG);
        BT_SINK_SRV_REMOVE_FLAG(ctx->context_flag, BT_SINK_SRV_CNTX_FLAG_MUST_PLAY_RING_TONE_FLAG);
    }
#ifdef MTK_BT_A2DP_FPGA_IT
    ret = bt_a2dp_start_streaming_response(dev->a2dp_hd, true);
    return ret;
#endif
    if (audio_src_running) {
        bt_sink_srv_report_id("[sink][a2dp]start_ind(noodle)-running: 0x%08x, type: %d, flag: 0x%08x, op: 0x%08x", 4,
                              audio_src_running, audio_src_running->type, dev->flag, dev->op);
    }
#ifdef __BT_SINK_SRV_VOICE_PROMPT__
    if (dev->op & BT_SINK_SRV_MUSIC_WAIT_VAP_RESUME) {
        /* Ignore it */
        return ret;
    }

    if (dev && dev->handle) {
        bt_sink_srv_report_id("[sink_a2dp_flow]sp_streaming_start_ind, media handle: 0x%08x", 1, dev->handle);
    }

    if (audio_src_running && audio_src_running->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_VP) {
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
        timer_ret = bt_timer_ext_start(BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_ID, (uint32_t)dev, BT_SINK_SRV_A2DP_WAIT_VPA_TIMER_DUR, bt_sink_srv_a2dp_wait_vpa_timeout);
        bt_utils_assert(timer_ret == BT_TIMER_EXT_STATUS_SUCCESS && "start wait VPA timer fail ");
        bt_sink_srv_report_id("[sink][a2dp]start_ind(noodle)--start wait VPA timer", 0);
#endif
        audio_src_srv_add_waiting_list(dev->handle);
        BT_SINK_SRV_SET_FLAG(dev->op, BT_SINK_SRV_MUSIC_WAIT_VAP_RESUME);
        return ret;
    }
#endif
    if (!(ctx->rho_flag)) {
        bool need_start_streaming = false;
        bool need_response = true;
#ifdef FEATURE_NEW_MULTIPOINT_TRIAL
        bool need_check_avrcp = false;

        if (audio_src_running) {
            if (AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP == audio_src_running->type) {
                need_check_avrcp = true;
            }

            if (AUDIO_SRC_SRV_PSEUDO_DEVICE_ULL_A2DP == audio_src_running->type) {
                if (bt_sink_srv_music_get_context()->context_flag & BT_SINK_SRV_CNTX_FLAG_A2DP_INTER_ULL_BY_AVRCP) {
                    need_check_avrcp = true;
                }
            }
        }
        if (need_check_avrcp) {
            if (audio_src_running != dev->handle
                && ((dev->avrcp_status == BT_AVRCP_STATUS_PLAY_PLAYING) || (dev->avrcp_flag & BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG))) {
                need_response = false;
                need_start_streaming = true;
            } else if (ctx->run_dev == dev && dev->handle->substate == BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC) {
                BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAITING_START);
            }
        } else {
            need_response = false;
            need_start_streaming = true;

#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
            if (ctx->resume_dev || (ctx->context_flag & BT_SINK_SRV_CNTX_FLAG_SWITCH_AUDIO_OFF)) {
                need_start_streaming = false;
                ctx->resume_dev = dev;
                need_response = true;
            }
#endif
        }

        bt_sink_srv_report_id("[sink][a2dp]start_ind, ret:0x%08x, avrcp_status:0x%02x, response:%d", 3,
                              ret, dev->avrcp_status, need_response);
#else
        need_response = false;
        need_start_streaming = true;
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
        if (ctx->resume_dev || (ctx->context_flag & BT_SINK_SRV_CNTX_FLAG_SWITCH_AUDIO_OFF)) {
            need_start_streaming = false;
            need_response = true;
            ctx->resume_dev = dev;
        }
#endif
#endif
#ifdef AIR_BT_FAST_PAIR_SASS_ENABLE
        if (need_start_streaming) {
            if (BT_SINK_SRV_A2DP_INT_USER_CONF_ALLOW != bt_sink_srv_a2dp_int_user_conf_check(dev)) {
                need_start_streaming = false;
                need_response = true;
            }
        }
#endif
        if (need_response && (dev->flag & BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START)) {
            BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
            uint32_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(dev->dev_addr));
            bt_utils_assert(gap_hd);
            bt_avm_set_music_enable(gap_hd, BT_AVM_ROLE_AWS_MCE_AGENT, false);
            ret = bt_a2dp_start_streaming_response(dev->a2dp_hd, true);
        }
        if (need_start_streaming) {
            g_sn_wrap_count = 0;
            bt_sink_srv_music_state_machine_handle(dev, BT_A2DP_START_STREAMING_IND, NULL);
        }
        bt_sink_srv_report_id("[sink][a2dp]start_ind, ret:0x%08x, avrcp_status:0x%02x, response:%d, need_start_streaming:0x%x", 4,
            ret,dev->avrcp_status,need_response, need_start_streaming);
    } else {
        uint32_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(dev->dev_addr));
        bt_utils_assert(gap_hd);
        bt_avm_set_music_enable(gap_hd, BT_AVM_ROLE_AWS_MCE_AGENT, false);
        ret = bt_a2dp_start_streaming_response(dev->a2dp_hd, true);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
        bt_sink_srv_report_id("[sink][a2dp] A2dp response received during RHO", 0);
        BT_SINK_SRV_SET_FLAG(dev->op, BT_SINK_SRV_MUSIC_START_PLAY_ON_NEW_AGENT_FLAG);
    }

    return ret;
}

static int32_t bt_sink_srv_a2dp_handle_suspend_streaming_cnf(bt_a2dp_suspend_streaming_cnf_t *suspend_cnf)
{
    bt_a2dp_suspend_streaming_ind_t suspend_streaming_ind;
    suspend_streaming_ind.handle = suspend_cnf->handle;
    int32_t ret = bt_sink_srv_a2dp_handle_suspend_streaming_ind(&suspend_streaming_ind, false);

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_suspend_streaming_ind(bt_a2dp_suspend_streaming_ind_t *suspend_ind, bool need_response)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    int32_t ret = 0;

    bt_sink_srv_report_id("[sink][a2dp]suspend_streaming_ind, latency:%d", 1, ctx->init_sink_latency);

    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(suspend_ind->handle)));


#ifdef MTK_BT_A2DP_FPGA_IT
    ret = bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
    return ret;
#endif
    if (a2dp_dev) {
        a2dp_dev->a2dp_status = BT_SINK_SRV_A2DP_STATUS_SUSPEND;
        if (!(a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT)) {
            audio_src_srv_del_waiting_list(a2dp_dev->handle);
        }
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_STOP_MUSIC_PENDING);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAITING_START);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_ALC_REINITIAL_SYNC);
#ifndef AIR_BT_MUSIC_DISABLE_NOTIFICATION_VOICE_OPTIMIZATION
        if (a2dp_dev->op & BT_SINK_SRV_MUSIC_GAME_FLAG) {
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_GAME_FLAG);
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT);
            audio_src_srv_del_waiting_list(a2dp_dev->handle);
        }
#endif
        if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_LIST_SINK_PLAY) {
            BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_LIST_SINK_PLAY);
            bt_sink_srv_report_id("[sink][a2dp]flag wait list sink play", 0);
        }
    }
    if ((a2dp_dev) &&
        (ctx->run_dev) &&
        (a2dp_dev == ctx->run_dev)) {
        /* SP suspend */
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);
        /* check other device is playing or not */
        audio_src_srv_del_waiting_list(a2dp_dev->handle);

        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_A2DP_SUSPEND_STREAMING_IND, NULL);
    } else {
        ret = bt_a2dp_suspend_streaming_response(suspend_ind->handle, true);
    }

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_reconfigure_cnf(bt_a2dp_reconfigure_cnf_t *reconfigure_cnf)
{
    int32_t ret = 0;

    return ret;
}


static int32_t bt_sink_srv_a2dp_handle_reconfigure_ind(bt_a2dp_reconfigure_ind_t *reconfigure_ind)
{
    bt_sink_srv_music_device_t *a2dp_dev = NULL;
    int32_t ret = 0;

    a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_A2DP_HD, (void *)(&(reconfigure_ind->handle)));
    if (a2dp_dev) {
        memcpy(&(a2dp_dev->codec), reconfigure_ind->codec_cap, sizeof(bt_a2dp_codec_capability_t));
    }

    ret = bt_a2dp_reconfigure_response(reconfigure_ind->handle, true);

    return ret;
}


#ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
static int32_t bt_sink_srv_a2dp_handle_delay_report_cnf(bt_a2dp_delay_report_cnf_t *delay_cnf)
{
    int32_t ret = 0;

    bt_sink_srv_report_id("[sink][a2dp]delay_report_cnf-hd: 0x%08x, status: 0x%08x", 2,
                          delay_cnf->handle, delay_cnf->status);

    return ret;
}
#endif


audio_src_srv_handle_t *bt_sink_srv_a2dp_alloc_pseudo_handle(void)
{
    int32_t i = 0;
    audio_src_srv_handle_t *hd = NULL;

    for (i = 0; i < BT_SINK_SRV_A2DP_PSEUDO_COUNT + BT_SINK_SRV_MCE_A2DP_PSEUDO_COUNT; ++i) {
        if (!(g_a2dp_pse_hd[i].flag & BT_SINK_SRV_A2DP_PSEUDO_FLAG_USEED)) {
            hd = g_a2dp_pse_hd[i].hd;
            BT_SINK_SRV_SET_FLAG(g_a2dp_pse_hd[i].flag, BT_SINK_SRV_A2DP_PSEUDO_FLAG_USEED);
            bt_sink_srv_music_fill_audio_src_callback(hd);
            break;
        }
    }

    bt_sink_srv_report_id("[sink][a2dp]alloc_pseudo_handle--hd: 0x%x", 1, hd);

    bt_utils_assert(hd);
    return hd;
}


void bt_sink_srv_a2dp_free_pseudo_handle(audio_src_srv_handle_t *hd)
{
    int32_t i = 0;

    for (i = 0; i < BT_SINK_SRV_A2DP_PSEUDO_COUNT + BT_SINK_SRV_MCE_A2DP_PSEUDO_COUNT; ++i) {
        if ((hd) &&
            (g_a2dp_pse_hd[i].flag & BT_SINK_SRV_A2DP_PSEUDO_FLAG_USEED) &&
            (g_a2dp_pse_hd[i].hd == hd)) {
            BT_SINK_SRV_REMOVE_FLAG(g_a2dp_pse_hd[i].flag, BT_SINK_SRV_A2DP_PSEUDO_FLAG_USEED);
            break;
        }
    }

    bt_sink_srv_report_id("[sink][a2dp]free_pseudo_handle--hd: 0x%x", 1, hd);
}


bt_status_t bt_sink_srv_a2dp_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_status_t ret = 0;
    int32_t err = 0;
#ifndef MTK_BT_CM_SUPPORT
    bt_bd_addr_t *dev_addr = NULL;
    uint32_t hd = 0;
    bt_sink_srv_profile_connection_action_t *conn_info = NULL;
#endif
    (void)ret;
    (void)err;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_context_t *cntx = bt_sink_srv_music_get_context();

    bt_sink_srv_report_id("[sink][a2dp]process_a2dp_action[s]-action: 0x%x, base: 0x%x\n", 2, action, BT_SINK_MODULE_A2DP_ACTION);
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (action) {
#ifndef MTK_BT_CM_SUPPORT
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_a2dp_init();
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
            bt_sink_srv_aws_mce_a2dp_init();
#endif
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            if (cntx->a2dp_aid != BT_SINK_SRV_INVALID_AID) {
                bt_sink_srv_ami_audio_close(cntx->a2dp_aid);
                cntx->a2dp_aid = BT_SINK_SRV_INVALID_AID;
            }
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
            if (cntx->aws_aid != BT_SINK_SRV_INVALID_AID) {
                bt_sink_srv_ami_audio_close(cntx->aws_aid);
                cntx->aws_aid = BT_SINK_SRV_INVALID_AID;
            }
#endif /* defined(__BT_AWS_A2DP_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__) */
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            uint8_t special_ratio_dev_count = sizeof(g_special_connection_device_list) / sizeof(bt_device_manager_db_remote_pnp_info_t);
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->address);
            bt_device_manager_db_remote_pnp_info_t dev_id_p;
            bt_device_manager_remote_find_pnp_info((void *)dev_addr, &dev_id_p);
            uint8_t i = 0;

            for (i = 0; i < special_ratio_dev_count; i++) {
                if (dev_id_p.vender_id == g_special_connection_device_list[i].vender_id
                    && dev_id_p.product_id == g_special_connection_device_list[i].product_id) {
                    break;
                }
            }
            if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
                bt_sink_srv_report_id("[sink][a2dp]Action is on client!", 0);
                break;
            }
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_A2DP_SINK) {
                ret = bt_a2dp_connect(&hd, (const bt_bd_addr_t *)dev_addr, BT_A2DP_SINK);
                if (BT_STATUS_SUCCESS == ret) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)dev_addr);
                    if (!dev) {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP, (void *)dev_addr);
                    }

                    if (!dev) {
                        dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
                        bt_utils_assert(dev && "Error: a2dp dev NULL");

                        dev->handle = bt_sink_srv_a2dp_alloc_pseudo_handle();
                        dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP;
                        dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid(dev_addr);
                        dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
                    }
                    bt_utils_assert(dev && "Error1: a2dp dev NULL");
                    dev->a2dp_hd = hd;
                    dev->role = BT_A2DP_SINK;
                    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_INITIAL_A2DP_BY_DEVICE);
                    BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP);

                    bt_sink_srv_memcpy(&(dev->dev_addr), dev_addr, sizeof(bt_bd_addr_t));

                }
            }
            break;
        }

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            conn_info = (bt_sink_srv_profile_connection_action_t *)param;
            dev_addr = &(conn_info->address);

            if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
                bt_sink_srv_report_id("[sink][a2dp]Action is on client!", 0);
                break;
            }
            if (conn_info->profile_connection_mask & BT_SINK_SRV_PROFILE_A2DP_SINK) {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)dev_addr);
                if (dev) {
                    BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP);
                    if (dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT)) {
                        bt_timer_ext_stop(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID);
                        ret = bt_a2dp_disconnect(dev->a2dp_hd);
                    }
                } else {
                    err = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
                }
            }
            break;
        }
#endif
        case BT_SINK_SRV_ACTION_VOLUME_UP: {

            if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
                bt_sink_srv_report_id("[sink][a2dp]Action is on client!", 0);
                break;
            }
            dev = cntx->run_dev;
            if (param) {
                bt_sink_srv_action_volume_up_t *sink_srv_dev = (bt_sink_srv_action_volume_up_t*)param;
                if (BT_SINK_SRV_DEVICE_EDR == sink_srv_dev->type) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)(&sink_srv_dev->address));
                }
            }
            if (dev) {
                bt_sink_srv_a2dp_change_volume(VOLUME_UP, 1, 0, dev);
            } else {
                ret = BT_STATUS_FAIL;
            }
            break;
        }

        case BT_SINK_SRV_ACTION_VOLUME_DOWN: {

            if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
                bt_sink_srv_report_id("[sink][a2dp]Action is on client!", 0);
                break;
            }

            dev = cntx->run_dev;
            if (param) {
                bt_sink_srv_action_volume_down_t *sink_srv_dev = (bt_sink_srv_action_volume_up_t*)param;
                if (BT_SINK_SRV_DEVICE_EDR == sink_srv_dev->type) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)(&sink_srv_dev->address));
                }
            }
            if (dev) {
                bt_sink_srv_a2dp_change_volume(VOLUME_DOWN, 1, 0, dev);
            } else {
                ret = BT_STATUS_FAIL;
            }
            break;
        }

        case BT_SINK_SRV_ACTION_SET_VOLUME: {
            if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
                bt_sink_srv_report_id("[sink][a2dp]Action is on client!", 0);
                break;
            }
            uint8_t *volume_value = (uint8_t *)param;
            dev = cntx->run_dev;
            if (dev) {
                bt_sink_srv_a2dp_change_volume(VOLUME_VALUE, 1, *volume_value, dev);
            } else {
                ret = BT_STATUS_FAIL;
            }
            break;
        }

        case BT_SINK_SRV_ACTION_SET_LATENCY: {
            bt_sink_srv_report_id("[sink][a2dp]Action is set sink latency.", 0);
            if (param) {
                uint32_t *latency_val = (uint32_t *)param;
                ret = bt_sink_srv_music_set_sink_latency(*latency_val, true);
            } else {
                bt_sink_srv_report_id("[sink][a2dp]sink latency is null!", 0);
            }
            break;
        }

        default:
            break;
    }

    bt_sink_srv_report_id("[sink][a2dp]process_a2dp_action[e]-ret: %d, err: %d", 2, ret, err);

    return ret;
}


#ifdef MTK_AWS_MCE_ENABLE
static void bt_sink_srv_a2dp_set_special_dev_mtu(bt_device_manager_db_remote_pnp_info_t *device_id, bt_bd_addr_t addr, bool is_acl_connected)
{
    uint32_t mtu_size = BT_SINK_SRV_A2DP_SPECIAL_DEVICE_MTU_SIZE;
    bool ret;
    ret = bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)addr, BT_IOT_MUSIC_SET_SPECIAL_MTU);
    if (ret) {
        if (!is_acl_connected) {
            mtu_size = BT_SINK_SRV_A2DP_NORMAL_MTU_SIZE;
        }
        bt_a2dp_set_mtu_size(mtu_size);
        bt_sink_srv_report_id("[sink][a2dp] set special MTU size: %d", 1, mtu_size);
    }
}

static bt_status_t bt_sink_srv_a2dp_link_status_change_handler(bt_gap_link_status_updated_ind_t *link_data)
{
    bt_sink_srv_report_id("[sink][a2dp] link status change handle, link_status: 0x%x", 1, link_data->link_status);
    bt_device_manager_db_remote_pnp_info_t dev_id_p = {0, 0};

    bt_device_manager_remote_find_pnp_info((void *)(link_data->address), &dev_id_p);
    if (link_data->link_status == BT_GAP_LINK_STATUS_CONNECTED_0
        && dev_id_p.product_id != 0 && dev_id_p.vender_id != 0) {
        bt_sink_srv_a2dp_set_special_dev_mtu(&dev_id_p, (void *)(link_data->address), true);
    } else if (link_data->link_status == BT_GAP_LINK_STATUS_DISCONNECTED
               && dev_id_p.product_id != 0 && dev_id_p.vender_id != 0) {
        bt_sink_srv_a2dp_set_special_dev_mtu(&dev_id_p, (void *)(link_data->address), false);
    }
    return BT_STATUS_SUCCESS;
}

static void bt_sink_srv_a2dp_sdp_attribute_handler(void *buffer)
{
    bt_sdpc_attribute_cnf_t *attr_result = (bt_sdpc_attribute_cnf_t *)buffer;
    bt_device_manager_db_remote_pnp_info_t device_id = {0, 0};
    uint8_t *parse_result = NULL;
    uint8_t *vendor_id = NULL;
    uint16_t vendor_id_len = 0;
    uint16_t result_len = 0;
    bt_sink_srv_report_id("[sink][a2dp] BT_SDPC_SEARCH_ATTRIBUTE_CNF", 0);

    bt_sdpc_parse_attribute(&parse_result, &result_len, BT_DI_SDP_ATTRIBUTE_VENDOR_ID, 0, attr_result->length, attr_result->attribute_data);
    if (!parse_result) {
        bt_sink_srv_report_id("[sink][a2dp] sdp error: parse_result is null!", 0);
        return;
    }
    bt_sink_srv_report_id("[sink][a2dp]***result len is %d, parse_result: 0x%04x", 2, result_len, parse_result[0] << 8 | parse_result[1]);
    bt_sdpc_parse_next_value(&vendor_id, &vendor_id_len, parse_result, result_len);

    bt_sink_srv_report_id("[sink][a2dp]***result vendor len is %d", 1, vendor_id_len);
    if (vendor_id_len) {
        bt_sink_srv_report_id("[sink][a2dp]vender_id:0x%04x", 1, vendor_id[0] << 8 | vendor_id[1]);
        device_id.vender_id = (vendor_id[0] << 8 | vendor_id[1]);
    }
    bt_sdpc_parse_attribute(&parse_result, &result_len, BT_DI_SDP_ATTRIBUTE_PRODUCT_ID, 0, attr_result->length, attr_result->attribute_data);
    bt_sink_srv_report_id("[sink][a2dp]***result len is %d, parse_result: 0x%04x", 2, result_len, parse_result[0] << 8 | parse_result[1]);
    bt_sdpc_parse_next_value(&vendor_id, &vendor_id_len, parse_result, result_len);
    if (vendor_id_len) {
        bt_sink_srv_report_id("[sink][a2dp]product_id:0x%04x", 1, vendor_id[0] << 8 | vendor_id[1]);
        device_id.product_id = (vendor_id[0] << 8 | vendor_id[1]);
    }
    if (device_id.product_id && device_id.vender_id) {
        bt_sink_srv_a2dp_set_special_dev_mtu(&device_id, (void *)(attr_result->user_data), true);
    }
}
#endif

bt_status_t bt_sink_srv_a2dp_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    int32_t ret = 0;
    static uint32_t s_a2dp_data_log_loop_cnt = BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT;

    if (((BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT == s_a2dp_data_log_loop_cnt) && (msg == BT_A2DP_STREAMING_RECEIVED_IND))
        || (msg != BT_A2DP_STREAMING_RECEIVED_IND && msg != BT_MEMORY_FREE_GARBAGE_IND)) {
        bt_sink_srv_report_id("[sink][a2dp]common_hdr[s]-msg: 0x%x, status: %d", 2, msg, status);
        if (BT_SINK_SRV_A2DP_DATA_RCE_LOOP_COUNT == s_a2dp_data_log_loop_cnt) {
            s_a2dp_data_log_loop_cnt = 0;
            /* Update info */
            g_a2dp_gpt_run_count_begin = g_a2dp_gpt_run_count_end;
        }
    }
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if ((role != BT_AWS_MCE_ROLE_AGENT) && (role != BT_AWS_MCE_ROLE_NONE)) {
        return ret;
    }
    switch (msg) {
        case BT_A2DP_CONNECT_CNF: {
            bt_a2dp_connect_cnf_t *conn_cnf = (bt_a2dp_connect_cnf_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_connect_cnf(conn_cnf);
            break;
        }

        case BT_A2DP_CONNECT_IND: {
            bt_a2dp_connect_ind_t *conn_ind = (bt_a2dp_connect_ind_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_connect_ind(conn_ind);
            break;
        }

        case BT_A2DP_DISCONNECT_CNF: {
            bt_a2dp_disconnect_cnf_t *disconn_cnf = (bt_a2dp_disconnect_cnf_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_disconnect_cnf(disconn_cnf);
            break;
        }

        case BT_A2DP_DISCONNECT_IND: {
            bt_a2dp_disconnect_ind_t *disconn_ind = (bt_a2dp_disconnect_ind_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_disconnect_ind(disconn_ind);
            break;
        }

        case BT_A2DP_START_STREAMING_CNF: {
            bt_a2dp_start_streaming_cnf_t *start_cnf = (bt_a2dp_start_streaming_cnf_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_start_streaming_cnf(start_cnf);
            break;
        }

        case BT_A2DP_START_STREAMING_IND: {
            bt_a2dp_start_streaming_ind_t *start_ind = (bt_a2dp_start_streaming_ind_t *)buffer;
            bt_sink_srv_report_id("[sink][a2dp]start_ind--buff: 0x%x, cap: 0x%x", 2, buffer, start_ind->codec_cap);

#ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
            bt_sink_srv_report_id("[sink][a2dp]start_ind--sec_type: 0x%04x, delay: %d", 2, start_ind->codec_cap->sec_type, start_ind->codec_cap->delay_report);
#endif

            if (start_ind->codec_cap->type == BT_A2DP_CODEC_SBC) {
                bt_sink_srv_report_id("[sink][a2dp]start_ind(sbc)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d", 11,
                                      start_ind->handle,
                                      start_ind->codec_cap->type,
                                      start_ind->codec_cap->sep_type,
                                      start_ind->codec_cap->length,
                                      start_ind->codec_cap->codec.sbc.channel_mode,
                                      start_ind->codec_cap->codec.sbc.sample_freq,
                                      start_ind->codec_cap->codec.sbc.alloc_method,
                                      start_ind->codec_cap->codec.sbc.subbands,
                                      start_ind->codec_cap->codec.sbc.block_len,
                                      start_ind->codec_cap->codec.sbc.min_bitpool,
                                      start_ind->codec_cap->codec.sbc.max_bitpool);
            } else if (start_ind->codec_cap->type == BT_A2DP_CODEC_AAC) {
                bt_sink_srv_report_id("[sink][a2dp]start_ind(aac)--hd: 0x%x, type: %d, sep_type: %d, len: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d, 8: %d, 9: %d", 13,
                                      start_ind->handle,
                                      start_ind->codec_cap->type,
                                      start_ind->codec_cap->sep_type,
                                      start_ind->codec_cap->length,
                                      start_ind->codec_cap->codec.aac.object_type,
                                      start_ind->codec_cap->codec.aac.drc,
                                      start_ind->codec_cap->codec.aac.freq_h,
                                      start_ind->codec_cap->codec.aac.channels,
                                      start_ind->codec_cap->codec.aac.freq_l,
                                      start_ind->codec_cap->codec.aac.br_h,
                                      start_ind->codec_cap->codec.aac.vbr,
                                      start_ind->codec_cap->codec.aac.br_m,
                                      start_ind->codec_cap->codec.aac.br_l);
            }

            ret = bt_sink_srv_a2dp_handle_start_streaming_ind(start_ind, false);
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_CNF: {
            bt_a2dp_suspend_streaming_cnf_t *suspend_cnf = (bt_a2dp_suspend_streaming_cnf_t *)buffer;

            ret = bt_sink_srv_a2dp_handle_suspend_streaming_cnf(suspend_cnf);
            break;
        }

        case BT_A2DP_SUSPEND_STREAMING_IND: {
            bt_a2dp_suspend_streaming_ind_t *suspend_ind = (bt_a2dp_suspend_streaming_ind_t *)buffer;

            ret = bt_sink_srv_a2dp_handle_suspend_streaming_ind(suspend_ind, true);
            break;
        }

        case BT_A2DP_RECONFIGURE_CNF: {
            bt_a2dp_reconfigure_cnf_t *reconfigure_cnf = (bt_a2dp_reconfigure_cnf_t *)buffer;

            ret = bt_sink_srv_a2dp_handle_reconfigure_cnf(reconfigure_cnf);
            break;
        }

        case BT_A2DP_RECONFIGURE_IND: {
            bt_a2dp_reconfigure_ind_t *reconfigure_ind = (bt_a2dp_reconfigure_ind_t *)buffer;

            ret = bt_sink_srv_a2dp_handle_reconfigure_ind(reconfigure_ind);
            break;
        }

        case BT_AVM_MEDIA_DATA_RECEIVED_IND: {
            bt_avm_a2dp_media_info_t *media_info = (bt_avm_a2dp_media_info_t *)buffer;
            bt_sink_srv_music_data_info_t media_data_info;
            media_data_info.asi = media_info->asi;
            media_data_info.clock.nclk = media_info->clock.nclk;
            media_data_info.clock.nclk_intra = media_info->clock.nclk_intra;
            media_data_info.ratio = (media_info->ratio == 0xff ? 0xffffffff : media_info->ratio);
            media_data_info.samples = media_info->samples;
            bt_sink_srv_a2dp_handle_media_data_received_ind(media_info->gap_handle, &media_data_info);
            break;
        }
        case BT_AVM_SEQUENCE_NUM_WRAP_COUNT_IND: {
            bt_avm_a2dp_sn_wrap_count_t *wrap_count_info = (bt_avm_a2dp_sn_wrap_count_t *)buffer;
            bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
            uint32_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(ctx->run_dev->dev_addr));
            if (gap_hd == wrap_count_info->gap_handle) {
                g_sn_wrap_count = wrap_count_info->wrap_count;
                bt_sink_srv_report_id("[sink][a2dp]sequence num wrap count:%d, handle:0x%08x", 1, g_sn_wrap_count, gap_hd);
            } else {
                bt_sink_srv_report_id("[sink][a2dp][ERR]connection handle is 0x%08x, gap handle is 0x%08x", 2, wrap_count_info->gap_handle, gap_hd);
            }
            break;
        }

        case BT_AVM_SET_LOCAL_ASI_FLAG: {
            bt_avm_a2dp_local_asi_ind_t *media_info = (bt_avm_a2dp_local_asi_ind_t *)buffer;
            bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
            bt_sink_srv_music_set_nvdm_data(&ctx->run_dev->dev_addr, BT_SINK_SRV_MUSIC_DATA_LOCAL_ASI_FLAG, (void *)(&media_info->local_asi_flag));
            break;
        }

#ifdef MTK_AWS_MCE_ENABLE
        case BT_GAP_LINK_STATUS_UPDATED_IND: {
            bt_gap_link_status_updated_ind_t *link_data = (bt_gap_link_status_updated_ind_t *)buffer;
            bt_sink_srv_a2dp_link_status_change_handler(link_data);
            break;
        }
        case BT_SDPC_SEARCH_ATTRIBUTE_CNF: {
            bt_sink_srv_a2dp_sdp_attribute_handler(buffer);
            break;
        }
#endif
#ifdef __BT_SINK_SRV_A2DP_V13_SUPPORT__
        case BT_A2DP_DELAY_REPORT_CNF: {
            bt_a2dp_delay_report_cnf_t *delay_cnf = (bt_a2dp_delay_report_cnf_t *)buffer;
            ret = bt_sink_srv_a2dp_handle_delay_report_cnf(delay_cnf);
            break;
        }
#endif
        default:
            break;
    }

    if (msg != BT_A2DP_STREAMING_RECEIVED_IND && msg != BT_MEMORY_FREE_GARBAGE_IND) {
        bt_sink_srv_report_id("[sink][a2dp]common_hdr[e]-ret: %d", 1, ret);
    }

    return ret;
}



void bt_sink_srv_a2dp_drv_play(void *param)
{
    bt_sink_srv_music_device_t *a2dp_dev = (bt_sink_srv_music_device_t *)param;
    bt_sink_srv_music_context_t *ctx = NULL;
    bt_sink_srv_am_media_handle_t *med_hd = NULL;
    int32_t ret = 0;

    ctx = bt_sink_srv_music_get_context();
    if (ctx->run_dev == a2dp_dev) {
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_DRV_PLAY);
        med_hd = &(a2dp_dev->med_handle);
        BT_SINK_SRV_GPIO_SET_OUTPUT(HAL_GPIO_39, HAL_GPIO_DATA_HIGH);
        ret = med_hd->play(ctx->a2dp_aid);
        BT_SINK_SRV_GPIO_SET_OUTPUT(HAL_GPIO_39, HAL_GPIO_DATA_LOW);
        bt_sink_srv_report_id("[sink][a2dp]drv_play--ret: %d", 1, ret);
        if (BT_CODEC_MEDIA_STATUS_OK == ret) {
            BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_OP_DRV_PLAY);
            if (!(a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC)) {
                bt_sink_srv_state_set(BT_SINK_SRV_STATE_STREAMING);
            }
#ifdef MTK_BT_SPEAKER_ENABLE
            if (BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
                bt_timer_ext_start(BT_SINK_SRV_A2DP_SEND_PLAY_IND_TIMER_ID, 0,
                                   BT_SINK_SRV_A2DP_SEND_PLAY_IND_TIMER_DUR, bt_sink_srv_a2dp_send_play_ind_timer);
            }
            bt_sink_srv_report_id("get_type:0x%02x, type:0x%02x", 2, bt_sink_srv_music_get_spk_mode(), BT_SINK_SRV_MUSIC_MODE_BROADCAST);
#endif
#ifdef BT_SINK_DUAL_ANT_ENABLE
            bt_sink_srv_dual_ant_data_t notify;
            notify.type = BT_SINK_DUAL_ANT_TYPE_MUSIC;
            notify.music_info.a2dp_state = true;
            bt_sink_srv_dual_ant_notify(false, &notify);
#endif
            //bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PLAYING, NULL);
        } else {
            /* Error handle */
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, (uint32_t *)(&g_Rdebug_a2dp_ltcs_last_time));
            bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
        }
    }
}


#ifdef MTK_AUDIO_SYNC_ENABLE
bt_status_t bt_sink_srv_a2dp_change_sync_volume(uint8_t type, uint8_t notify_avrcp, uint32_t volume_value)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    uint8_t vol = ctx->vol_lev;

    if ((VOLUME_UP == type) && (vol < BT_SINK_SRV_A2DP_MAX_VOL_LEV)) {
        vol++;
    } else if ((VOLUME_DOWN == type) && (vol > BT_SINK_SRV_A2DP_MIN_VOL_LEV)) {
        vol--;
    } else if ((VOLUME_VALUE == type) && (volume_value >= BT_SINK_SRV_A2DP_MIN_VOL_LEV)
               && (volume_value <= BT_SINK_SRV_A2DP_MAX_VOL_LEV)) {
        vol = volume_value;
    }

    if ((vol != ctx->vol_lev) && run_dev && (run_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
        bt_sink_srv_audio_sync_music_data_t music_data;
        bt_sink_srv_get_sync_data_parameter_t sync_data;
        music_data.volume = vol;
        music_data.volume_type = 0;
        if (notify_avrcp) {
            music_data.volume_type = type;
        }
        sync_data.type = BT_SINK_SRV_A2DP_VOLUME_TYPE;
        sync_data.length = sizeof(bt_sink_srv_audio_sync_music_data_t);
        sync_data.duration = 200000;
        sync_data.timeout_duration = 0xffffffff;
        sync_data.data = &music_data;
        ctx->vol_lev = vol;
        bt_sink_srv_music_set_nvdm_data(&(run_dev->dev_addr), BT_SINK_SRV_MUSIC_DATA_VOLUME, &ctx->vol_lev);
        bt_sink_srv_request_sync_gpt(&sync_data);
    }

    return ret;
}
#endif

int32_t bt_sink_srv_a2dp_change_volume(uint8_t type, uint8_t sync, uint32_t volume_value, bt_sink_srv_music_device_t *dev)
{
    bt_sink_srv_mutex_lock();

    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_am_id_t ami_ret = 0;
    uint8_t vol = ctx->vol_lev;
    (void)ami_ret;

    bt_utils_assert(dev && "please input valid dev!");
#ifdef MTK_AUDIO_SYNC_ENABLE
    if (run_dev == dev && (run_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY) && sync) {
        bt_aws_mce_agent_state_type_t aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
        uint32_t aws_handle = bt_sink_srv_aws_mce_get_handle(&(run_dev->dev_addr));
        if (aws_handle) {
            aws_state = bt_sink_srv_aws_mce_get_aws_state_by_handle(aws_handle);
        }

        if (aws_handle && (aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
            bt_sink_srv_a2dp_change_sync_volume(type, 1, volume_value);
            bt_sink_srv_mutex_unlock();
            return ret;
        }
    }
#endif

    /* volume up */
    if (VOLUME_UP == type) {
        if (vol < BT_SINK_SRV_A2DP_MAX_VOL_LEV) {
            vol = vol + 1;
        } else {
            ret = BT_SINK_SRV_MUSIC_ERR_FAIL_1ST;
        }
#ifdef MTK_BT_SPEAKER_ENABLE
        if (run_dev == dev && sync && (BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode())) {
            bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_UP, (void *)(&(run_dev->a2dp_hd)));
        }
#endif
    } else if (VOLUME_DOWN == type) {
        if (vol > BT_SINK_SRV_A2DP_MIN_VOL_LEV) {
            vol = vol - 1;
        } else {
            ret = BT_SINK_SRV_MUSIC_ERR_FAIL_2ND;
        }
#ifdef MTK_BT_SPEAKER_ENABLE
        if (run_dev == dev && sync && (BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode())) {
            bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_DOWN, (void *)(&(run_dev->a2dp_hd)));
        }
#endif
    } else if (VOLUME_VALUE == type) {
        if (volume_value <= BT_SINK_SRV_A2DP_MAX_VOL_LEV) {
            vol = volume_value;
        }
    }

    if ((vol != ctx->vol_lev) && dev) {
        if (run_dev == dev) {
            if (sync) {
                ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->a2dp_aid, vol, STREAM_OUT);
            } else {
                ami_ret = bt_sink_srv_ami_audio_set_volume(ctx->aws_aid, vol, STREAM_OUT);
            }
        }
        ctx->vol_lev = vol;
        ctx->last_volume = vol;
        if (sync) {
            bt_sink_srv_music_set_nvdm_data(&(dev->dev_addr), BT_SINK_SRV_MUSIC_DATA_VOLUME, &ctx->vol_lev);
        }
#ifdef MTK_BT_SPEAKER_ENABLE
        else {
            uint16_t music_volume;
            music_volume = BT_SINK_SRV_A2DP_MAGIC_CODE | ctx->vol_lev;
            bt_connection_manager_device_local_info_set_local_music_volume(&music_volume);
        }
#endif
        bt_sink_srv_avrcp_volume_notification(dev->avrcp_hd, ctx->vol_lev, type);

#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        if (sync && (run_dev == dev)
#ifdef MTK_BT_SPEAKER_ENABLE
            && BT_SINK_SRV_MUSIC_MODE_BROADCAST != bt_sink_srv_music_get_spk_mode()
#endif
           ) {
            bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_SYNC, (void *)(&(ctx->run_dev->a2dp_hd)));
        }
#endif
    }
    bt_sink_srv_report_id("[sink][a2dp]change_volume-ami_ret: %d, ret: %d, vol: %d, dev:0x%x, run_dev:0x%x", 5,
        ami_ret, ret, ctx->vol_lev, dev, run_dev);
    bt_sink_srv_mutex_unlock();

    return ret;
}


void bt_sink_srv_a2dp_play(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);
    bt_sink_srv_am_audio_capability_t aud_cap = {0};
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_am_result_t am_ret;
    uint32_t latency_val = 0;
    int32_t ret = 0;

    bt_utils_assert(a2dp_dev);
    BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_LIST_SINK_PLAY);
    bt_sink_srv_report_id("[sink][a2dp]play(s)--hd: 0x%08x, type: %d, flag: 0x%08x, op: 0x%08x", 4,
                          handle, handle->type, a2dp_dev->flag, a2dp_dev->op);

    if (a2dp_dev->op & BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT) {
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
        a2dp_dev->handle->substate = BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC;
    }

#ifdef AIR_LE_AUDIO_ENABLE
    if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC) {
        bt_sink_srv_cap_am_enable_waiting_list();
    }
#endif
    
    bt_sink_srv_music_stop_vp_detection(a2dp_dev->handle);
    if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT) {
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT);
#ifdef AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE
        bt_sink_srv_config_t ue_config = {0};
        bt_sink_srv_state_manager_get_resume_config_t get_resume_config = {0};
        get_resume_config.type = BT_SINK_SRV_DEVICE_EDR;
        bt_sink_srv_memcpy(&(get_resume_config.address),&(a2dp_dev->dev_addr),sizeof(bt_bd_addr_t));
        if (BT_STATUS_SUCCESS == bt_sink_srv_state_manager_get_config(BT_SINK_SRV_GET_RESUME_CONFIG, 
                                                                        (void*)(&get_resume_config),
                                                                        &ue_config)) {
            if (BT_SINK_SRV_INTERRUPT_OPERATION_PLAY_MUSIC == ue_config.resume_config.resume_operation) {
#ifndef AIR_BT_MUSIC_DISABLE_NOTIFICATION_VOICE_OPTIMIZATION
                if (!(a2dp_dev->op & BT_SINK_SRV_MUSIC_GAME_FLAG)) {
                    ret = bt_sink_srv_avrcp_play_music(a2dp_dev);
                }
                BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_GAME_FLAG);
#else
                ret = bt_sink_srv_avrcp_play_music(a2dp_dev);
#endif
            }
        }
#else
#ifndef AIR_BT_MUSIC_DISABLE_NOTIFICATION_VOICE_OPTIMIZATION
        if (!(a2dp_dev->op & BT_SINK_SRV_MUSIC_GAME_FLAG)) {
            ret = bt_sink_srv_avrcp_play_music(a2dp_dev);
        }
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_GAME_FLAG);
#else
        ret = bt_sink_srv_avrcp_play_music(a2dp_dev);
#endif

#endif
        if (a2dp_dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_SUSPEND) {
            bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_RESUME, NULL);
            return ;
        }
    }

#ifdef AIR_MULTI_POINT_ENABLE
#ifdef MTK_BT_CM_SUPPORT
    bt_cm_connect_t conn_req;
    bt_sink_srv_memcpy(&(conn_req.address), &(a2dp_dev->dev_addr), sizeof(bt_bd_addr_t));
    conn_req.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
    bt_cm_connect(&conn_req);
    bt_sink_srv_report_id("[sink][a2dp]switch link", 0);

#endif
#endif
    /* Audio source accept play request */
    BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);
    /* Update run device */
    bt_sink_srv_music_update_run_device(a2dp_dev);
    /* 1. Open A2DP codec */
    bt_sink_srv_music_get_nvdm_data(&(a2dp_dev->dev_addr), BT_SINK_SRV_MUSIC_DATA_VOLUME, &ctx->vol_lev);

    uint16_t a2dp_mtu = bt_a2dp_get_mtu_size(a2dp_dev->a2dp_hd);
    bt_sink_srv_music_fill_am_aud_param(&aud_cap, &a2dp_dev->codec, BT_A2DP_SINK, a2dp_mtu);
#ifdef AIR_A2DP_SYNC_STOP_ENABLE
    bt_sink_srv_ami_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BT_A2DP_DL, bt_sink_srv_music_sync_stop_done_handler, NULL);
#endif

    if (!(a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC)) {
        bool is_initial = true;
        if (BT_SINK_SRV_MUSIC_MODE_BROADCAST != bt_sink_srv_music_get_spk_mode()) {
            if (a2dp_dev->codec.type != BT_A2DP_CODEC_VENDOR) {
                bt_sink_srv_music_mode_t music_mode = bt_sink_srv_music_get_mode(&a2dp_dev->dev_addr);
                latency_val = bt_sink_srv_get_latency(&a2dp_dev->dev_addr, false, false, false);
                if (BT_STATUS_SUCCESS == ret && latency_val < BT_SINK_SRV_A2DP_SPECIAL_DEVICE_LATENCY
                    && (music_mode != BT_SINK_SRV_MUSIC_GAME_MODE)
                    && bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)&ctx->run_dev->dev_addr, BT_IOT_MUSIC_SET_LATENCY_TO_250)) {
                    latency_val = BT_SINK_SRV_A2DP_SPECIAL_DEVICE_LATENCY;
                    is_initial = false;
                }
            } else {
                latency_val = bt_sink_srv_get_latency(&a2dp_dev->dev_addr, true, false, false);
                is_initial = false;
            }
        } else {
            latency_val = bt_sink_srv_get_latency(&a2dp_dev->dev_addr, false, false, true);
        }
        bt_sink_srv_music_set_sink_latency(latency_val, is_initial);
    }
#if defined (AIR_LE_AUDIO_ENABLE) && defined (__BT_AWS_MCE_A2DP_SUPPORT__) && defined (AIR_LE_AUDIO_CIS_ENABLE)
    bt_handle_t le_handle = bt_sink_srv_cap_get_link_handle(0xFF);
    if (BT_HANDLE_INVALID != le_handle) {
        bt_sink_srv_cap_update_connection_interval(le_handle, 0x30);
    }
#endif
    int32_t gpt_ret = 0;
    (void)gpt_ret;
    gpt_ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &g_a2dp_gpt_codec_run_count_begin);
    bt_sink_srv_report_id("[sink][a2dp]drv_play--begin: %d, gpt_ret: %d", 2, g_a2dp_gpt_codec_run_count_begin, gpt_ret);

    bt_sink_srv_set_clock_offset_ptr_to_dsp((const bt_bd_addr_t *)(&(a2dp_dev->dev_addr)));
    am_ret = bt_sink_srv_ami_audio_play(ctx->a2dp_aid, &aud_cap);
    bt_sink_srv_update_last_device(&(a2dp_dev->dev_addr), BT_SINK_SRV_PROFILE_A2DP_SINK);

#ifdef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
    if (a2dp_dev->flag & BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START) {
        bt_sink_srv_report_id("[sink][a2dp] response a2dp start during opening codec", 0);
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
        bt_a2dp_start_streaming_response(a2dp_dev->a2dp_hd, true);
    }
#endif

    if (AUD_EXECUTION_SUCCESS != am_ret) {
        /* Exception: play fail */
        BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC);
        bt_sink_srv_music_update_run_device(NULL);
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_FAIL, NULL);
    }
    if (a2dp_dev->handle->substate != BT_SINK_SRV_MUSIC_TRANSIENT_STATE_PREPARE_CODEC) {
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_CODEC, NULL);
    }
    bt_gap_reset_sniff_timer(BT_SINK_SRV_A2DP_MIN_SNIFF_DUR);
    bt_sink_srv_report_id("[sink][a2dp]to set sniff timer to 1s", 0);

#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    if (a2dp_dev && a2dp_dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_START_STREAMING, (void *)(&(a2dp_dev->a2dp_hd)));
    }
#endif
    BT_SINK_SRV_REMOVE_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_AVRCP_PLAY_TRIGGER);
}
static void bt_sink_srv_a2dp_imp_stop(bt_sink_srv_music_device_t *a2dp_dev)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_utils_assert(a2dp_dev);
    bt_sink_srv_report_id("[sink][a2dp]imp stop(s)--a2dp_dev: 0x%x, flag: 0x%x, op: 0x%x", 3,
                      a2dp_dev, a2dp_dev->flag, a2dp_dev->op);
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    if (a2dp_dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) {
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_SUSPEND_STREAMING, (void *)(&(a2dp_dev->a2dp_hd)));
    }
#endif

    /* Clear codec */
#ifdef AIR_A2DP_SYNC_STOP_ENABLE
    uint32_t aws_handle = bt_sink_srv_aws_mce_get_handle(&(a2dp_dev->dev_addr));
    bt_aws_mce_agent_state_type_t aws_state = bt_sink_srv_aws_mce_get_aws_state_by_handle(aws_handle);
    if ((a2dp_dev->state == AUDIO_SRC_SRV_STATE_PLAYING) && (aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
        if (BT_STATUS_SUCCESS != bt_sink_srv_music_request_sync_stop()) {
            bt_sink_srv_music_set_force_stop(true);
            bt_sink_srv_music_stop(a2dp_dev, ctx->a2dp_aid);
        } else {
            bt_sink_srv_music_set_force_stop(false);
        }
    } else {
        bt_sink_srv_music_stop(a2dp_dev, ctx->a2dp_aid);
    }
#else
    bt_sink_srv_music_stop(a2dp_dev, ctx->a2dp_aid);
#endif
}

void bt_sink_srv_a2dp_stop(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_music_device_t *a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();

    bt_utils_assert(a2dp_dev);

    bt_sink_srv_report_id("[sink][a2dp]stop(s)--hd: 0x%x, type: %d, flag: 0x%x, op: 0x%x", 4,
                          handle, handle->type, a2dp_dev->flag, a2dp_dev->op);

    BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_NORMAL_STOP_FLAG);

#if defined (AIR_LE_AUDIO_ENABLE) && defined (__BT_AWS_MCE_A2DP_SUPPORT__) && defined (AIR_LE_AUDIO_CIS_ENABLE)
    bt_handle_t le_handle = bt_sink_srv_cap_get_link_handle(0xFF);
    if (BT_HANDLE_INVALID != le_handle) {
        bt_sink_srv_cap_update_connection_interval(le_handle, 0x30);
    }
#endif
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
    if (a2dp_dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) {
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_SUSPEND_STREAMING, (void *)(&(a2dp_dev->a2dp_hd)));
    }
#endif

    /* Clear codec */
#ifdef AIR_A2DP_SYNC_STOP_ENABLE
    uint32_t aws_handle = bt_sink_srv_aws_mce_get_handle(&(a2dp_dev->dev_addr));
    bt_aws_mce_agent_state_type_t aws_state = bt_sink_srv_aws_mce_get_aws_state_by_handle(aws_handle);
    if ((a2dp_dev->state == AUDIO_SRC_SRV_STATE_PLAYING) && (aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
        if (BT_STATUS_SUCCESS != bt_sink_srv_music_request_sync_stop()) {
            bt_sink_srv_music_set_force_stop(true);
            bt_sink_srv_music_stop(a2dp_dev, ctx->a2dp_aid);
        } else {
            bt_sink_srv_music_set_force_stop(false);
        }
    } else {
        bt_sink_srv_music_stop(a2dp_dev, ctx->a2dp_aid);
    }
#else
    bt_sink_srv_music_stop(a2dp_dev, ctx->a2dp_aid);
#endif
    /* When interrupt is comming during prepare codec, need to switch to int-dev after codec done, the running dev
       is still this dev in suspend function, so can not add running dev to waiting list at AM side. */
    if ((a2dp_dev->flag & BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT) && a2dp_dev->handle) {
        if (!(a2dp_dev->handle->flag & AUDIO_SRC_SRV_FLAG_WAITING)) {
            bt_sink_srv_a2dp_add_waitinglist(a2dp_dev->handle);
        }
    }
}


void bt_sink_srv_a2dp_suspend(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    bt_sink_srv_mutex_lock();
    bt_sink_srv_music_device_t *a2dp_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);
    bt_sink_srv_music_device_t *int_dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)int_hd);

    bt_utils_assert(a2dp_dev && handle && int_hd);
    /*GVA-13775, if a2dp suspend come before open codec done and esco need to suspend a2dp, just do nothing.*/
    if (a2dp_dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_SUSPEND
        && BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC == a2dp_dev->handle->substate) {
        bt_sink_srv_mutex_unlock();
        return;
    }

    /* Clear codec */
    if (a2dp_dev->handle->substate != BT_SINK_SRV_MUSIC_TRANSIENT_STATE_CLEAR_CODEC) {
        bt_sink_srv_music_state_machine_handle(a2dp_dev, BT_SINK_SRV_MUSIC_EVT_PREPARE_CLEAR, NULL);
    }
    bt_sink_srv_a2dp_imp_stop(a2dp_dev);
#ifdef AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE
    bt_sink_srv_config_t ue_config = {0};
    bt_sink_srv_state_manager_get_suspend_config_t get_suspend_config = {0};
    get_suspend_config.type = BT_SINK_SRV_DEVICE_EDR;
    bt_sink_srv_memcpy(&(get_suspend_config.address),&(a2dp_dev->dev_addr),sizeof(bt_bd_addr_t));
    get_suspend_config.suspend_handle = int_hd;
    bt_sink_srv_state_manager_get_config(BT_SINK_SRV_GET_SUSPEND_CONFIG, 
                                        (void*)(&get_suspend_config),
                                        &ue_config);
#endif
    int32_t ret = 0, err = 0;
    (void)err;
    (void)ret;
    if ((handle->dev_id == int_hd->dev_id) &&
        (int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP)) {
        if (a2dp_dev->avrcp_status == BT_AVRCP_STATUS_PLAY_PLAYING) {
            /* WSAP00041710 Nexus 5, HF don't interrupt A2DP */
            /* Add self in waiting list */
            bt_sink_srv_a2dp_add_waitinglist(handle);
            /* Set interrupt flag */
            BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_A2DP_HF_INTERRUPT);
            err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_7TH;
        } else {
            /* The same device & HF interrupt */
            err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_1ST;
        }
    } else if ((int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP) || ((int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE) && (int_hd->priority == AUDIO_SRC_SRV_PRIORITY_NORMAL))) {
        /* PartyMode interrupt */
        if ((int_dev && (!(int_dev->avrcp_flag & BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG)))
            || (int_hd->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE)) {
#ifdef AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE
            if (BT_SINK_SRV_INTERRUPT_OPERATION_PAUSE_MUSIC == ue_config.suspend_config.suspend_operation) {
                bt_sink_srv_a2dp_avrcp_pause_music(a2dp_dev);
            }
            if (ue_config.suspend_config.will_suspend_resume) {
                if (ue_config.suspend_config.suspend_resume_timeout) {
                    bt_sink_srv_music_start_vp_detection(a2dp_dev->handle, ue_config.suspend_config.suspend_resume_timeout); 
                }
                BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT);
                bt_sink_srv_a2dp_add_waitinglist(handle);
            }
#else
            bt_sink_srv_a2dp_avrcp_pause_music(a2dp_dev);
#endif
        }
        err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_2ND;
    } else {
        /* Add self in waiting list */
        if (!(a2dp_dev->avrcp_flag & BT_SINK_SRV_AVRCP_MUST_PLAY_RING_TONE_FLAG)) {
            BT_SINK_SRV_SET_FLAG(a2dp_dev->flag, BT_SINK_SRV_MUSIC_FLAG_A2DP_INTERRUPT);
#ifdef AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE
            if (ue_config.suspend_config.will_suspend_resume) {
                bt_sink_srv_a2dp_add_waitinglist(handle);
            }
            if (BT_SINK_SRV_INTERRUPT_OPERATION_PAUSE_MUSIC == ue_config.suspend_config.suspend_operation) {
                if (bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)&a2dp_dev->dev_addr, BT_IOT_MUSIC_SEND_AVRCP_FORCE_PAUSE)
                    && a2dp_dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
                    ret = bt_sink_srv_avrcp_force_pause_music(a2dp_dev);
                } else {
#ifndef AIR_BT_MUSIC_DISABLE_NOTIFICATION_VOICE_OPTIMIZATION
                    if (a2dp_dev->avrcp_status == BT_AVRCP_STATUS_PLAY_PLAYING) {
                        ret = bt_sink_srv_avrcp_stop_music(a2dp_dev);
                    } else {
                        BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_GAME_FLAG);
                    }
#else
                    ret = bt_sink_srv_avrcp_stop_music(a2dp_dev);
#endif
                }
            }
#else
            if (bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)&a2dp_dev->dev_addr, BT_IOT_MUSIC_SEND_AVRCP_FORCE_PAUSE)
                    && a2dp_dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
                    ret = bt_sink_srv_avrcp_force_pause_music(a2dp_dev);
            } else {
#ifndef AIR_BT_MUSIC_DISABLE_NOTIFICATION_VOICE_OPTIMIZATION
                if (a2dp_dev->avrcp_status == BT_AVRCP_STATUS_PLAY_PLAYING) {
                    ret = bt_sink_srv_avrcp_stop_music(a2dp_dev);
                } else {
                    BT_SINK_SRV_SET_FLAG(a2dp_dev->op, BT_SINK_SRV_MUSIC_GAME_FLAG);
                }
#else
                ret = bt_sink_srv_avrcp_stop_music(a2dp_dev);
#endif
            }
#endif
        }
        err = BT_SINK_SRV_MUSIC_ERR_SUCCESS_3RD;
    }
    bt_sink_srv_mutex_unlock();
#ifdef AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE
    bt_sink_srv_report_id("[sink][a2dp]suspend--hd: 0x%x,type:%d,int:0x%x,type:%d,err:0x%x,ret:0x%x, ue_config:0x%x-0x%x", 8,
                          handle, handle->type, int_hd, int_hd->type, err, ret,
                          ue_config.suspend_config.will_suspend_resume, ue_config.suspend_config.suspend_operation);
#else
    bt_sink_srv_report_id("[sink][a2dp]suspend--hd: 0x%x,type:%d,int:0x%x,type:%d,err:0x%x,ret:0x%x", 6,
                      handle, handle->type, int_hd, int_hd->type, err, ret);

#endif
}


void bt_sink_srv_a2dp_reject(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_mutex_lock();
    bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_PSE_HD, (void *)handle);
    const audio_src_srv_handle_t *rej_handle = audio_src_srv_get_runing_pseudo_device();

    bt_utils_assert(dev && rej_handle && handle);
    bt_sink_srv_report_id("[sink][a2dp]reject(s)--hd: 0x%x, type: %d, dev_id: 0x%x-0x%x", 4,
                          handle, handle->type, rej_handle->dev_id, handle->dev_id);
    
    if (dev->flag & BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START) {
        bt_a2dp_start_streaming_response(dev->a2dp_hd, true);
        BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_NEED_TO_RESPONSE_A2DP_START);
    }

    bt_gap_connection_handle_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(dev->dev_addr));
    bt_utils_assert(gap_hd);
    bt_sink_srv_music_set_music_enable(gap_hd, BT_AVM_ROLE_AWS_MCE_AGENT, false);
    bt_sink_srv_music_state_machine_handle(dev, BT_SINK_SRV_MUSIC_EVT_REJECT, NULL);

    if (rej_handle->dev_id == handle->dev_id) {
        BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_WAIT_LIST_SINK_PLAY);
        audio_src_srv_add_waiting_list(handle);
    } else {
#ifdef AIR_BT_SINK_SRV_STATE_MANAGER_ENABLE
        bt_sink_srv_config_t ue_config = {0};
        bt_sink_srv_state_manager_get_reject_config_t get_reject_config = {0};
        get_reject_config.type = BT_SINK_SRV_DEVICE_EDR;
        bt_sink_srv_memcpy(&(get_reject_config.address),&(dev->dev_addr),sizeof(bt_bd_addr_t));
        if (BT_STATUS_SUCCESS == bt_sink_srv_state_manager_get_config(BT_SINK_SRV_GET_REJECT_CONFIG, 
                                            (void*)(&get_reject_config),
                                            &ue_config)) {
            if (BT_SINK_SRV_INTERRUPT_OPERATION_PAUSE_MUSIC == ue_config.reject_config.reject_operation) {
                if (bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)&dev->dev_addr, BT_IOT_MUSIC_SEND_AVRCP_FORCE_PAUSE)
                    && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
                    bt_sink_srv_avrcp_force_pause_music(dev);
                } else {
                    bt_sink_srv_avrcp_stop_music(dev);
                }
            }
            if (ue_config.reject_config.will_reject_resume) {
                bt_sink_srv_a2dp_add_waitinglist(dev->handle);
            }
        }
#else
    if (bt_iot_device_white_list_check_iot_case((bt_bd_addr_t *)&dev->dev_addr, BT_IOT_MUSIC_SEND_AVRCP_FORCE_PAUSE)
        && dev->a2dp_status == BT_SINK_SRV_A2DP_STATUS_STREAMING) {
        bt_sink_srv_avrcp_force_pause_music(dev);
    } else {
        bt_sink_srv_avrcp_stop_music(dev);
    }
#endif
    }

    /* Notify state machine reject reason */
    bt_sink_srv_mutex_unlock();
}

void bt_sink_srv_a2dp_create_pse_handle(void)
{
    int32_t i = 0;

    for (i = 0; i < BT_SINK_SRV_A2DP_PSEUDO_COUNT + BT_SINK_SRV_MCE_A2DP_PSEUDO_COUNT; ++i) {
        g_a2dp_pse_hd[i].hd = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP);
    }
}


void bt_sink_srv_a2dp_destroy_pse_handle(void)
{
    int32_t i = 0;

    for (i = 0; i < BT_SINK_SRV_A2DP_PSEUDO_COUNT + BT_SINK_SRV_MCE_A2DP_PSEUDO_COUNT; ++i) {
        audio_src_srv_destruct_handle(g_a2dp_pse_hd[i].hd);
    }
}


int32_t bt_sink_srv_a2dp_set_volume(uint8_t volume, bt_sink_srv_music_device_t *sp_dev)
{
    bt_sink_srv_music_context_t *cntx = bt_sink_srv_music_get_context();;
    bt_sink_srv_music_device_t *run_dev = cntx->run_dev;
    uint8_t new_vol = bt_sink_srv_avrcp_get_volume_level(volume);
    uint8_t old_vol = cntx->vol_lev;
    int32_t ret = BT_SINK_SRV_MUSIC_ERR_SUCCESS_OK;
    bt_sink_srv_am_id_t ami_ret = 0;
    (void)ami_ret;

    bt_sink_srv_report_id("[sink][a2dp]set_volume[s]-new_vol: %d, old_vol: %d", 2, new_vol, old_vol);
    bt_utils_assert(sp_dev && "sp_dev is NULL why?");

#ifdef MTK_AUDIO_SYNC_ENABLE
    if (run_dev && (run_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {

        bt_aws_mce_agent_state_type_t aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
        uint32_t aws_handle = bt_sink_srv_aws_mce_get_handle(&(run_dev->dev_addr));
        if (aws_handle) {
            aws_state = bt_sink_srv_aws_mce_get_aws_state_by_handle(aws_handle);
        }
        if (aws_handle && (aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
            bt_sink_srv_a2dp_change_sync_volume(VOLUME_VALUE, 0, new_vol);
            return ret;
        }
    }
#endif

    ret = bt_sink_srv_music_set_nvdm_data(&(sp_dev->dev_addr), BT_SINK_SRV_MUSIC_DATA_VOLUME, &new_vol);

    if ((old_vol != new_vol) && run_dev && run_dev == sp_dev) {
        cntx->vol_lev = new_vol;
        cntx->last_volume = new_vol;
        if (run_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN) {
            ami_ret = bt_sink_srv_ami_audio_set_volume(cntx->a2dp_aid, new_vol, STREAM_OUT);
        } else {
            BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_WAIT_SET_VOLUME);
        }
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        if (run_dev && (run_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY)) {
#ifdef MTK_BT_SPEAKER_ENABLE
            if (BT_SINK_SRV_MUSIC_MODE_BROADCAST == bt_sink_srv_music_get_spk_mode()) {
                if (new_vol > old_vol) {
                    bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_UP, (void *)(&(run_dev->a2dp_hd)));
                } else {
                    bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_DOWN, (void *)(&(run_dev->a2dp_hd)));
                }
            } else {
                bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_SYNC, (void *)(&(run_dev->a2dp_hd)));
            }
#else
            bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_VOL_SYNC, (void *)(&(run_dev->a2dp_hd)));
#endif
        }
#endif
    }

    bt_sink_srv_report_id("[sink][a2dp]set_volume-ami_ret: %d, ret: %d, vol: %d", 3,
                          ami_ret, ret, cntx->vol_lev);
    return ret;
}

static bool bt_sink_srv_a2dp_optimize_base_pta_by_asi(bt_sink_srv_music_optimize_pta_param_t *optimize_parameter)
{
    avm_direct_optimize_pta_param_t optimize_para;
    bt_clock_t rec_clk_cur = {0};
    bool ret = true;

    bt_sink_srv_memcpy(&rec_clk_cur, &optimize_parameter->cur_recv_clock, sizeof(bt_clock_t));
    avm_direct_bt_clock_normalize(&rec_clk_cur, &rec_clk_cur, BT_ROLE_SLAVE);
    optimize_para.base_asi = optimize_parameter->base_asi;
    optimize_para.frequence = optimize_parameter->frequence;
    optimize_para.cur_asi = optimize_parameter->cur_asi;
    optimize_para.latency_clock = optimize_parameter->latency_clock;
    optimize_para.cur_recv_clock = optimize_parameter->cur_recv_clock;
    optimize_para.pta = optimize_parameter->pta;
    avm_direct_optimize_base_pta(&optimize_para);
    return ret;
}


static void bt_sink_srv_a2dp_sta_timer(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_set_sta_param_t sta_parameters;
    sta_parameters.frequence = ctx->freq;
    sta_parameters.gap_handle = data;
    sta_parameters.packet_count = ctx->packet_count;
    sta_parameters.packet_list = ctx->media_data_list;
    sta_parameters.pta = &ctx->bt_clk;
    bt_sink_srv_a2dp_set_sta(&sta_parameters, false);
}

#define BT_INTRA_PRECISE_4_TICK (1250)
static bool bt_sink_srv_a2dp_get_duration(uint32_t nclk_base, uint32_t nclk_target, uint32_t *duration)
{
    uint32_t dur_nclk;
    dur_nclk = nclk_target - nclk_base;
    bt_sink_srv_report_id("[sink][a2dp]get_duration, base:0x%08x, target:0x%08x", 2, nclk_base, nclk_target);
    if (dur_nclk & 0x80000000) {
        return false;
    }

    *duration = (dur_nclk * BT_INTRA_PRECISE_4_TICK) >> 2;
    return true;
}

void bt_sink_srv_a2dp_stop_timer(uint32_t timer_id)
{
    bt_timer_ext_t *stop_timer = bt_timer_ext_find(timer_id);
    if (stop_timer) {
        bt_timer_ext_stop(timer_id);
    }
}
#ifdef AIR_DCHS_MODE_ENABLE
extern void DCHS_bt_pka_get_native_clk(uint32_t *NativeClk, uint16_t *NativePhase);
#endif
static void bt_sink_srv_a2dp_set_sta(bt_sink_srv_music_set_sta_param_t *sta_parameters, bool need_timer)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    //uint32_t packet_count = sta_parameters->packet_count;
    //uint32_t freq = sta_parameters->frequence;
    bt_clock_t *pta = sta_parameters->pta;
    bt_clock_t current_clock;
    bt_clock_t min_sta_clock;
    bt_clock_t max_sta_clock;
    bt_clock_t min_dur_clock;
    bt_clock_t max_dur_clock;

    //Just workround, set current bt clock to be packet rec time + 12 ticks.
    //bt_get_bt_clock(sta_parameters->gap_handle, &current_clock);
#ifdef AIR_DCHS_MODE_ENABLE
    // N-done is using native clock for DCHS products
    if(dchs_get_device_mode() != DCHS_MODE_SINGLE) {
        DCHS_bt_pka_get_native_clk(&current_clock.nclk, &current_clock.nclk_intra);
        bt_sink_srv_report_id("[sink][a2dp]DCHS get native clock:0x%x, cur_intra:0x%x",
                              2, current_clock.nclk, current_clock.nclk_intra);
    } else {
        bt_get_bt_clock(sta_parameters->gap_handle, &current_clock);
    }

#else
    bt_get_bt_clock(sta_parameters->gap_handle, &current_clock);
#endif
#if 0
    current_clock.nclk = packet_list[ctx->packet_count - 1].clock.nclk + 16;
    current_clock.nclk_intra = 0;
#endif
    max_dur_clock.nclk = BT_SINK_SRV_A2DP_AVM_TIMER_MIN_DUR_TO_PLAY_BY_TICK;
    max_dur_clock.nclk_intra = 0;
    min_dur_clock.nclk = BT_SINK_SRV_A2DP_MIN_STA_DUR_TO_PLAY;
    min_dur_clock.nclk_intra = 0;
    avm_direct_bt_clock_add_duration(&max_sta_clock, &current_clock, &min_dur_clock, BT_ROLE_SLAVE);
    avm_direct_bt_clock_add_duration(&min_sta_clock, &current_clock, &max_dur_clock, BT_ROLE_SLAVE);
    uint32_t duration = 0;
    bool ret = bt_sink_srv_a2dp_get_duration(min_sta_clock.nclk, pta->nclk, &duration);
    duration = duration / 1000;
    bt_sink_srv_report_id("[sink][a2dp]ret:%d, duration:0x%08x, pta-nclk:0x%08x, pta-intra:0x%04x, packet_count:%d", 5, ret, duration,
                          sta_parameters->pta->nclk, sta_parameters->pta->nclk_intra, sta_parameters->packet_count);
    if (ret && need_timer) {
        bt_sink_srv_a2dp_stop_timer(BT_SINK_SRV_STA_TIMER_ID);
        bt_timer_ext_start(BT_SINK_SRV_STA_TIMER_ID, sta_parameters->gap_handle,
                           duration, bt_sink_srv_a2dp_sta_timer);
    } else {
        if (ret) {
            return;
        }
        bt_sink_srv_a2dp_stop_timer(BT_SINK_SRV_STA_TIMER_ID);
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
        bt_sink_srv_aws_mce_a2dp_send_eir(BT_SINK_SRV_AWS_MCE_A2DP_EVT_PLAY_IND, (void *)(&(ctx->run_dev->a2dp_hd)));
#endif
        bt_sink_srv_report_id("[sink][a2dp]play_info, cur_nclk:0x%08x, cur_intra:0x%04x, pta-nclk:0x%08x, pta-intra:0x%04x, base_asi:0x%08x",
                              5, current_clock.nclk, current_clock.nclk_intra, pta->nclk, pta->nclk_intra, ctx->ts);
        bt_sink_srv_music_trigger_play(sta_parameters->gap_handle, ctx->run_dev, pta, ctx->ts, ctx->ts);
        ctx->packet_count = 0;
    }
}

static void bt_sink_srv_a2dp_handle_media_data_received_ind(uint32_t gap_handle, bt_sink_srv_music_data_info_t *media_info)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_data_info_t *packet_list = ctx->media_data_list;
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_clock_t recv_clk = {0, 0};
    bt_clock_t original_pta = {0, 0};
    if (!run_dev || run_dev->op & BT_SINK_SRV_MUSIC_OP_DRV_PLAY || !(run_dev->op & BT_SINK_SRV_MUSIC_OP_CODEC_OPEN)) {
        return;
    }
    uint32_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(run_dev->dev_addr));
    if (gap_hd != gap_handle) {
        return;
    }

    bt_media_handle_t *med_hd = run_dev->med_handle.med_hd;
    media_info->asi = media_info->asi & 0x03ffffff;
    bt_sink_srv_music_fill_recevied_media_data(media_info);
    bt_sink_srv_memcpy(&recv_clk, &media_info->clock, sizeof(bt_clock_t));
    bt_clock_t *clk_latency = avm_direct_get_sink_latency_by_tick();
    avm_direct_bt_clock_normalize(&recv_clk, &recv_clk, BT_ROLE_SLAVE);
    if (ctx->packet_count == 1) {
        avm_direct_bt_clock_normalize(&recv_clk, &recv_clk, BT_ROLE_SLAVE);
        avm_direct_bt_clock_add_duration(&ctx->bt_clk, &recv_clk, clk_latency, BT_ROLE_SLAVE);
        ctx->freq = med_hd->get_sampling_rate(med_hd);
        ctx->ts = media_info->asi;
        ctx->ratio = media_info->ratio;
        ctx->samples = media_info->samples;
    } else {
        bt_sink_srv_music_optimize_pta_param_t optimize_parameters;
        optimize_parameters.frequence = ctx->freq;
        optimize_parameters.base_asi = packet_list[0].asi;
        optimize_parameters.cur_asi = media_info->asi;
        optimize_parameters.latency_clock = clk_latency;
        optimize_parameters.cur_recv_clock = &media_info->clock;
        optimize_parameters.pta = &ctx->bt_clk;
        bt_sink_srv_memcpy(&original_pta, &ctx->bt_clk, sizeof(bt_clock_t));
        bt_sink_srv_a2dp_optimize_base_pta_by_asi(&optimize_parameters);
    }
    bt_sink_srv_music_set_sta_param_t sta_parameters;

    sta_parameters.frequence = ctx->freq;
    sta_parameters.gap_handle = gap_hd;
    sta_parameters.packet_count = ctx->packet_count;
    sta_parameters.packet_list = ctx->media_data_list;
    sta_parameters.pta = &ctx->bt_clk;
    bool need_timer = false;
    bt_sink_srv_report_id("[sink][a2dp]play_info, base_asi:0x%08x, base_nclk:0x%08x, base_intra:0x%04x", 3,
                          ctx->ts, sta_parameters.pta->nclk, sta_parameters.pta->nclk_intra);
    if (ctx->bt_clk.nclk != original_pta.nclk) {
        need_timer = true;
    }
    bt_sink_srv_a2dp_set_sta(&sta_parameters, need_timer);
}

#if 0
static void bt_sink_srv_a2dp_handle_media_data_ind(uint32_t gap_handle, bt_avm_a2dp_packet_info_t *media_data)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_avm_a2dp_packet_info_t *packet_list = ctx->packet_list;
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_media_handle_t *med_hd = run_dev->med_handle;
    bt_clock_t recv_clk = {0, 0};

    bt_sink_srv_a2dp_fill_media_data(media_data);

    bt_sink_srv_memcpy(&recv_clk, media_data->clock, sizeof(bt_clock_t));
    bt_clock_t *clk_latency = avm_direct_get_sink_latency_by_tick();
    avm_direct_bt_clock_normalize(&recv_clk, &recv_clk, BT_ROLE_SLAVE);
    if (ctx->packet_count == 1) {
        avm_direct_bt_clock_normalize(&recv_clk, &recv_clk, BT_ROLE_SLAVE);
        avm_direct_bt_clock_add_duration(&ctx->bt_clk, &recv_clk, clk_latency, BT_ROLE_SLAVE);
        ctx->freq = med_hd->get_sampling_rate(med_hd);
    } else {
        if (packet_list[0].sequence_num + 1 != packet_list[1].sequence_num) {
            bt_sink_srv_music_clear_media_packet_list();
            bt_avm_set_a2dp_notify_condition(gap_handle, BT_SINK_SRV_A2DP_N_PACKET_NOTIFY);
            return;
        }
        ctx->ratio = med_hd->get_ts_ratio(med_hd,
                                          packet_list[0].timestamp, packet_list[1].timestamp);

        if (ctx->ratio > BT_SINK_SRV_MAX_RATIO) {
            ctx->ts = packet_list[0].sequence_num * ctx->ratio; //rtp_header->seqno << 10 == rtp_header->seqno *1024
            ctx->samples = ctx->ratio;
        } else {
            ctx->ts = packet_list[0].timestamp / ctx->ratio;
        }
        bt_sink_srv_music_optimize_pta_parameter_t optimize_parameter;
        optimize_parameter.frequence = ctx->freq;
        optimize_parameter.base_clock = &ctx->bt_clk;
        optimize_parameter.ratio = ctx->ratio;
        optimize_parameter.latency_clock = clk_latency;
        optimize_parameter.base_packet = &packet_list[0];
        optimize_parameter.cur_packet = media_data;
        bt_sink_srv_a2dp_optimize_base_pta(&optimize_parameter);
    }
}
#endif

extern void bt_driver_set_no_retransmission_mode(uint16_t seq_num, uint16_t length);
void bt_sink_srv_a2dp_set_no_retransmission_mode(uint16_t seq_num, uint16_t length)
{
    bt_driver_set_no_retransmission_mode(seq_num, length);
}

#ifdef __BT_SINK_SRV_A2DP_AVM_DIRECT_SUPPORT__
bt_status_t bt_sink_srv_a2dp_reinitial_sync()
{
    bt_sink_srv_mutex_lock();
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    uint32_t flag = 0;
    (void)flag;

    if (run_dev && (!(run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC)) && (!(run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC))) {
        uint32_t device_list[BT_SINK_SRV_MUISC_DEV_COUNT];
        uint32_t device_count = 0;
        BT_SINK_SRV_REMOVE_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_ALC_REINITIAL_SYNC);
        bt_sink_srv_music_get_waiting_list_devices(device_list, &device_count);
        bt_sink_srv_music_device_waiting_list_operation(device_list, device_count, false);

        flag = run_dev->flag;
        BT_SINK_SRV_SET_FLAG(run_dev->flag, BT_SINK_SRV_MUSIC_FLAG_REINITIAL_SYNC);
#ifdef AIR_LE_AUDIO_ENABLE
        bt_sink_srv_cap_am_disable_waiting_list();
#endif
        bt_sink_srv_music_state_machine_handle(run_dev, BT_A2DP_SUSPEND_STREAMING_IND, NULL);

        bt_sink_srv_music_device_waiting_list_operation(device_list, device_count, true);
    } else if (run_dev && (run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC)) {
        BT_SINK_SRV_SET_FLAG(run_dev->op, BT_SINK_SRV_MUSIC_WAITING_REINITIAL_SYNC);
        ret = BT_STATUS_FAIL;
    } else {
        bt_sink_srv_report_id("[a2dp]It is not on playing or on reinitial sync", 0);
    }
    bt_sink_srv_report_id("[a2dp]To reinitial sync, run_dev:0x%08x, flag:0x%08x", 2, run_dev, flag);
    bt_sink_srv_mutex_unlock();
    return ret;
}

bt_status_t bt_sink_srv_a2dp_get_volume(bt_bd_addr_t *bd_addr, uint32_t *volume)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_bd_addr_t *remote_addr = bd_addr;
    bt_status_t ret = BT_STATUS_FAIL;

    if (!remote_addr) {
        if (run_dev) {
            remote_addr = &(run_dev->dev_addr);
        }
    }

    if (remote_addr) {
        if (role == BT_AWS_MCE_ROLE_NONE || role == BT_AWS_MCE_ROLE_AGENT) {
            ret = bt_sink_srv_music_get_nvdm_data(remote_addr, BT_SINK_SRV_MUSIC_DATA_VOLUME, volume);
        } else {
            ret = BT_STATUS_SUCCESS;
            *volume = ctx->vol_lev;
        }
    }

    bt_sink_srv_report_id("[a2dp]get_volume, volume:0x%08x, role:0x%02x, remote_addr:0x%08x, run_dev:0x%08x", 4, *volume, role, remote_addr, run_dev);

    return ret;
}



#endif

int32_t bt_sink_srv_a2dp_add_waitinglist(audio_src_srv_handle_t *handle)
{
    int32_t ret = 0;
    uint8_t idx = 0;
    bt_sink_srv_music_device_t *dev = NULL;
    bt_sink_srv_music_device_list_t *device_list = bt_sink_srv_music_get_played_device_list(true);

    bt_utils_assert(handle);
    if (device_list && device_list->number) {
        for (idx = 0; idx < device_list->number; idx++) {
            dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP, (void *)(&(device_list->device_list[idx])));
            if (!dev) {
                dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)(&(device_list->device_list[idx])));
            }

            if (dev) {
                if (dev->handle->flag & AUDIO_SRC_SRV_FLAG_WAITING) {
                    bt_sink_srv_report_id("[sink_a2dp_flow] waiting list exist, dev->handle: 0x%08x, handle: 0x%08x", 2, dev->handle, handle);
                    if (dev->handle != handle) {
                        ret = -2;
                    }
                    break;
                }
            } else {
                ret = -1;
                bt_sink_srv_report_id("[sink_a2dp_flow][error] no device find, handle: 0x%08x", 1, handle);
            }
        }
    } else {
        ret = -1;
        bt_sink_srv_report_id("[sink_a2dp_flow][error] no device connected, handle: 0x%08x", 1, handle);
    }
    if (0 == ret) {
        bt_sink_srv_report_id("[sink_a2dp_flow]add to waiting list, handle: 0x%08x", 1, handle);
        audio_src_srv_add_waiting_list(handle);
    }

    return ret;
}

bt_status_t bt_sink_srv_a2dp_get_codec_parameters(bt_sink_srv_a2dp_basic_config_t *config_data)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_sink_srv_music_device_t *run_dev = ctx->run_dev;
    bt_status_t ret = BT_STATUS_FAIL;
    bt_utils_assert(config_data);
    config_data->status = 0x01;
    if (run_dev) {
#ifdef MTK_INITIAL_SYNC_BY_SAMPLE_INDEX_SUPPORT
        bt_gap_connection_handle_t gap_hd = bt_sink_srv_cm_get_gap_handle(&(run_dev->dev_addr));
        extern uint8_t bt_avm_get_rate(uint32_t handle);
        config_data->enable_3M = bt_avm_get_rate(gap_hd);
#endif
        config_data->latency = avm_direct_get_sink_latency();
        config_data->codec_type = run_dev->codec.type;
        config_data->min_bit_pool = 0xff;
        config_data->max_bit_pool = 0xff;
        config_data->bit_rate = 0xffffffff;
        if (config_data->codec_type == BT_A2DP_CODEC_SBC) {
            config_data->min_bit_pool = run_dev->codec.codec.sbc.min_bitpool;
            config_data->max_bit_pool = run_dev->codec.codec.sbc.max_bitpool;
        } else if (config_data->codec_type == BT_A2DP_CODEC_AAC) {
            bt_a2dp_aac_codec_t *aac_codec = &(run_dev->codec.codec.aac);
            config_data->bit_rate = (aac_codec->br_h << 16 | aac_codec->br_m << 8 | aac_codec->br_l);
        }
        config_data->status = 0x00;
        ret = BT_STATUS_SUCCESS;
    }
    bt_sink_srv_report_id("[sink_a2dp]get_codec_parameters status:0x%02x, 3M:0x%02x, latency:%d, codec_type:0x%02x, min:0x%02x, max:0x%02x, bit_rate:0x%08x", 7,
                          config_data->status, config_data->enable_3M, config_data->latency, config_data->codec_type, config_data->min_bit_pool, config_data->max_bit_pool, config_data->bit_rate);

    return ret;
}


#ifdef MTK_BT_CM_SUPPORT
bt_status_t  bt_sink_srv_a2dp_cm_callback_handler(bt_cm_profile_service_handle_t type, void *data)
{
    bt_sink_srv_music_context_t *ctx = bt_sink_srv_music_get_context();
    bt_status_t status = BT_STATUS_SUCCESS;
    uint8_t *address = NULL;
    bt_sink_srv_report_id("[a2dp] cm_callback_handler type:0x%02x", 1, type);
    switch (type) {
        case BT_CM_PROFILE_SERVICE_HANDLE_POWER_ON: {
            //firstly init context
            bt_sink_srv_music_init_context();
            //secondary init a2dp profile
            bt_sink_srv_a2dp_init();
#ifdef MTK_AUDIO_SYNC_ENABLE
            bt_sink_srv_register_sync_callback(BT_SINK_SRV_A2DP_VOLUME_TYPE, bt_sink_srv_music_volume_sync_callback);
#ifdef AIR_A2DP_SYNC_STOP_ENABLE
            extern void bt_sink_srv_music_stop_sync_callback(bt_sink_srv_sync_status_t status, bt_sink_srv_sync_callback_data_t *sync_data);
            bt_sink_srv_register_sync_callback(BT_SINK_SRV_MUSIC_STOP_TYPE, bt_sink_srv_music_stop_sync_callback);
#endif
#endif
#ifdef __BT_AWS_MCE_A2DP_SUPPORT__
            bt_sink_srv_aws_mce_a2dp_init();
#endif
            break;
        }
        case BT_CM_PROFILE_SERVICE_HANDLE_POWER_OFF: {
            bt_sink_srv_music_context_t *cntx = bt_sink_srv_music_get_context();
            if (cntx->a2dp_aid != BT_SINK_SRV_INVALID_AID) {
                if (ctx->run_dev && (ctx->run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC)) {
                    BT_SINK_SRV_SET_FLAG(ctx->context_flag, BT_SINK_SRV_CNTX_FLAG_POWER_OFF);
                } else {
                    bt_sink_srv_ami_audio_close(cntx->a2dp_aid);
                    cntx->a2dp_aid = BT_SINK_SRV_INVALID_AID;
                }
            }
#ifdef MTK_AUDIO_SYNC_ENABLE
            bt_sink_srv_deregister_sync_callback(BT_SINK_SRV_A2DP_VOLUME_TYPE);
#endif
#if defined(__BT_AWS_MCE_A2DP_SUPPORT__)
            if (cntx->aws_aid != BT_SINK_SRV_INVALID_AID) {
                if (ctx->run_dev && (ctx->run_dev->flag & BT_SINK_SRV_MUSIC_FLAG_WAIT_AMI_OPEN_CODEC)) {
                    BT_SINK_SRV_SET_FLAG(ctx->context_flag, BT_SINK_SRV_CNTX_FLAG_POWER_OFF);
                } else {
                    bt_sink_srv_ami_audio_close(cntx->aws_aid);
                    cntx->aws_aid = BT_SINK_SRV_INVALID_AID;
                }
            }
#endif /* defined(__BT_AWS_A2DP_SUPPORT__) || defined(__BT_AWS_MCE_A2DP_SUPPORT__) */
            break;
        }
        case BT_CM_PROFILE_SERVICE_HANDLE_CONNECT: {
            bt_sink_srv_mutex_lock();
            address = (uint8_t *)data;
            bt_utils_assert(address);
            bt_sink_srv_report_id("[sink][a2dp] connect a2dp addr :0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", 6, address[0], address[1], address[2],
                                  address[3], address[4], address[5]);
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
                bt_sink_srv_report_id("[sink][a2dp]Action is on client!", 0);
                status = BT_STATUS_UNSUPPORTED;
                bt_sink_srv_mutex_unlock();
                break;
            }
            if (ctx->run_dev && bt_sink_srv_memcmp(ctx->run_dev->dev_addr, address, sizeof(bt_bd_addr_t)) == 0) {
                status = BT_STATUS_FAIL;
                bt_sink_srv_mutex_unlock();
                break;
            }
            uint32_t hd = 0;
            status = bt_a2dp_connect(&hd, (const bt_bd_addr_t *)address, BT_A2DP_SINK);
            if (BT_STATUS_SUCCESS == status || BT_STATUS_OUT_OF_MEMORY == status) {
                bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)address);
                if (!dev) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_AVRCP, (void *)address);
                }

                if (!dev) {
                    dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_UNUSED, NULL);
                    bt_utils_assert(dev && "Error: a2dp dev NULL");

                    dev->handle = bt_sink_srv_a2dp_alloc_pseudo_handle();
                    dev->handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP;
                    dev->handle->dev_id = bt_sink_srv_music_convert_btaddr_to_devid((bt_bd_addr_t *)address);
                    dev->handle->priority = AUDIO_SRC_SRV_PRIORITY_NORMAL;
                }
                bt_utils_assert(dev && "Error1: a2dp dev NULL");
                dev->a2dp_hd = hd;
                dev->role = BT_A2DP_SINK;
                BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_INITIAL_A2DP_BY_DEVICE);
                BT_SINK_SRV_SET_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP);
                bt_sink_srv_memcpy(&(dev->dev_addr), address, sizeof(bt_bd_addr_t));
            }
            bt_sink_srv_mutex_unlock();
            break;
        }
        case BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT: {
            bt_sink_srv_mutex_lock();
            address = (uint8_t *)data;
            bt_utils_assert(address);
            bt_sink_srv_report_id("[sink][a2dp] disconnect a2dp addr :0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", 6, address[0], address[1], address[2],
                                  address[3], address[4], address[5]);
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
                bt_sink_srv_report_id("[sink][a2dp]Action is on client!", 0);
                bt_sink_srv_mutex_unlock();
                break;
            }
            bt_sink_srv_music_device_t *dev = bt_sink_srv_music_get_device(BT_SINK_SRV_MUSIC_DEVICE_ADDR_A2DP, (void *)address);
            if (dev) {
                BT_SINK_SRV_REMOVE_FLAG(dev->flag, BT_SINK_SRV_MUSIC_FLAG_DEV_TO_CON_AVRCP);
                if (dev->conn_bit & BT_SINK_SRV_MUSIC_A2DP_CONN_BIT) {
                    bt_timer_ext_stop(BT_SINK_SRV_AVRCP_CONNECTION_TIMER_ID);
                    status = bt_a2dp_disconnect(dev->a2dp_hd);
                    if (BT_SINK_SRV_MUSIC_FLAG_WAIT_AVRCP_CONN_RESULT & dev->flag) {
                        BT_SINK_SRV_SET_FLAG(dev->op, BT_SINK_SRV_MUSIC_INIT_DISCONNNECT_AVRCP_TIMER_FLAG);
                    }
                } else {
                    status = BT_STATUS_CONNECTION_NOT_FOUND;
                }
            } else {
                status = BT_STATUS_CONNECTION_NOT_FOUND;
            }
            bt_sink_srv_mutex_unlock();
            break;
        }
        default:
            break;
    }
    return status;
}
#endif
