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

#ifndef __APP_LE_AUDIO_UTILLITY_H__
#define __APP_LE_AUDIO_UTILLITY_H__

#ifdef AIR_LE_AUDIO_ENABLE
#include "FreeRTOS.h"
#include "timers.h"
#include "bt_type.h"
#include "bt_gap_le_audio.h"

#include "audio_transmitter_control.h"
#include "app_le_audio_usb.h"

/**************************************************************************************************
* Define
**************************************************************************************************/
#define APP_LE_AUDIO_TIMER_LE_CONNECTION_TIME_PERIOD    (60000) /* 1min */

/* Should be more than APP_LE_AUDIO_UCST_LINK_MAX_NUM */
#ifdef AIR_LE_AUDIO_MULTI_DEVICE_ENABLE
#define APP_LE_AUDIO_TIMER_MAX_NUM (4)
#else
#define APP_LE_AUDIO_TIMER_MAX_NUM (3)
#endif

typedef enum {
    APP_LE_AUDIO_EVENT_START_SCAN_NEW_DEVICE,
    APP_LE_AUDIO_EVENT_STOP_SCAN_NEW_DEVICE,
    APP_LE_AUDIO_EVENT_USB_HID_INCOMING_CALL,
    APP_LE_AUDIO_EVENT_USB_HID_CALL_ACTIVATE,
    APP_LE_AUDIO_EVENT_USB_HID_CALL_REMOTE_HOLD,
    APP_LE_AUDIO_EVENT_USB_HID_CALL_HOLD,
    APP_LE_AUDIO_EVENT_USB_HID_CALL_UNHOLD,
    APP_LE_AUDIO_EVENT_USB_HID_CALL_ENDED,
    APP_LE_AUDIO_EVENT_USB_HID_MIC_MUTE,
    APP_LE_AUDIO_EVENT_USB_HID_MIC_UNMUTE,
    APP_LE_AUDIO_EVENT_AUDIO_STREAM_START_SUCCESS_CNF,
    APP_LE_AUDIO_EVENT_AUDIO_STREAM_START_FAIL_CNF,
    APP_LE_AUDIO_EVENT_AUDIO_STREAM_STOP_CNF,
    APP_LE_AUDIO_EVENT_AUDIO_STREAM_CONFIG_CNF,
    APP_LE_AUDIO_EVENT_TIMER_EXPIRED,
} app_le_audio_event_t;

#define APP_LE_AUDIO_MODE_NONE      0
#define APP_LE_AUDIO_MODE_UCST      0x01
#define APP_LE_AUDIO_MODE_BCST      0x02
#define APP_LE_AUDIO_MODE_ASIT      0x03
#define APP_LE_AUDIO_MODE_DISABLE   0x04

/* Enum of detecting status of both port and device */
typedef enum
{
    APP_LE_AUDIO_SILENCE_DETECTION_STATUS_IDLE = 0,

    APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_SILENCE,    /* Data detected */
    APP_LE_AUDIO_SILENCE_DETECTION_STATUS_DETECTING_DATA,   /* Silence detected */

    APP_LE_AUDIO_SILENCE_DETECTION_STATUS_MAX = 0xFF
} app_le_audio_silence_detection_status_enum;


typedef enum
{
    APP_LE_AUDIO_SILENCE_DETECTION_MODE_NONE = 0,

    APP_LE_AUDIO_SILENCE_DETECTION_MODE_NORMAL,     /* Silence detection with at least one CIS connected */
    APP_LE_AUDIO_SILENCE_DETECTION_MODE_SPECIAL,    /* Silence detection with All CISes disconnected */

    APP_LE_AUDIO_SILENCE_DETECTION_MODE_MAX = 0xFF
} app_le_audio_silence_detection_mode_enum;


typedef enum
{
    APP_LE_AUDIO_SILENCE_DETECTION_EVENT_START_OTHER_MODE, /* Start non-special silence detection mode */
    APP_LE_AUDIO_SILENCE_DETECTION_EVENT_STOP_ANY_MODE,
    APP_LE_AUDIO_SILENCE_DETECTION_EVENT_START_SPECIAL_SILENCE_DETECTION,
    APP_LE_AUDIO_SILENCE_DETECTION_EVENT_SPECIAL_SILENCE_DETECTION_STOPPED,
    APP_LE_AUDIO_SILENCE_DETECTION_EVENT_REMOTE_DEVICE_BREDR_STATUS_UPDATE,
    APP_LE_AUDIO_SILENCE_DETECTION_EVENT_DISCONNECT_CIS_FOR_SILENCE_UPDATE,
    APP_LE_AUDIO_SILENCE_DETECTION_EVENT_PORT_DISABLED,

    APP_LE_AUDIO_SILENCE_DETECTION_EVENT_MAX = 0xFF
} app_le_audio_silence_detection_event_enum;

typedef uint8_t app_le_audio_mode_t;

/* Config releated */
#define APP_LE_AUDIO_SAMPLING_FREQ_INVALID      0

/* SDU interval (ms) */
#define SDU_INTERVAL_7P5_MS                     0x00    /* 7.5 ms */
#define SDU_INTERVAL_10_MS                      0x01    /* 10 ms */
#define SDU_INTERVAL_INVALID                    0xFF
typedef uint8_t app_le_audio_sdu_interval_t;

#define APP_LE_AUDIO_VOL_LEVEL_MIN      0
#define APP_LE_AUDIO_VOL_LEVEL_MAX      15
#define APP_LE_AUDIO_VOL_LEVEL_DEFAULT  12

/* Audio stream releated */
#define APP_LE_AUDIO_STREAM_PORT_SPK_0     0
#define APP_LE_AUDIO_STREAM_PORT_SPK_1     1
#define APP_LE_AUDIO_STREAM_PORT_MIC_0     2
#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
#define APP_LE_AUDIO_STREAM_PORT_LINE_IN   3
#define APP_LE_AUDIO_STREAM_PORT_I2S_IN    4
#define APP_LE_AUDIO_STREAM_PORT_MAX       5
#else
#define APP_LE_AUDIO_STREAM_PORT_MAX       3
#endif
typedef uint32_t app_le_audio_stream_port_t;

#define APP_LE_AUDIO_STREAM_PORT_MASK_SPK_0     0x01
#define APP_LE_AUDIO_STREAM_PORT_MASK_SPK_1     0x02
#define APP_LE_AUDIO_STREAM_PORT_MASK_MIC_0     0x04
#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
#define APP_LE_AUDIO_STREAM_PORT_MASK_LINE_IN   0x08
#define APP_LE_AUDIO_STREAM_PORT_MASK_I2S_IN    0x10
#endif
typedef uint8_t app_le_audio_stream_port_mask_t;

#define APP_LE_AUDIO_TRANSMITTER_STATE_IDLE         0x00
#define APP_LE_AUDIO_TRANSMITTER_STATE_INIT         0x01
#define APP_LE_AUDIO_TRANSMITTER_STATE_STOP         APP_LE_AUDIO_TRANSMITTER_STATE_INIT
#define APP_LE_AUDIO_TRANSMITTER_STATE_STARTING     0x02
#define APP_LE_AUDIO_TRANSMITTER_STATE_SET_VOL      0x03
#define APP_LE_AUDIO_TRANSMITTER_STATE_STARTED      APP_LE_AUDIO_TRANSMITTER_STATE_SET_VOL
#define APP_LE_AUDIO_TRANSMITTER_STATE_DEINIT       0x04
typedef uint8_t app_le_audio_transmitter_state_t;

/* Audio transmitter channel */
#define APP_LE_AUDIO_TRANSMITTER_CHANNEL_L          0x01
#define APP_LE_AUDIO_TRANSMITTER_CHANNEL_R          0x02
#define APP_LE_AUDIO_TRANSMITTER_CHANNEL_DUAL       0x03    /* (APP_LE_AUDIO_TRANSMITTER_CHANNEL_L | APP_LE_AUDIO_TRANSMITTER_CHANNEL_R) */

typedef enum {
    APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_START = 0x01,
    APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_CONFIG = 0x02,
    APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_STOP = 0x04,

    APP_LE_AUDIO_TRANSMITTER_WAIT_CNF_MAX = 0xFF
} app_le_audio_transmitter_wait_cnf_enum;

/* Function works only when app_le_audio_usb_port_t and app_le_audio_stream_port_t are aligned. */
// TODO: phase-out app_le_audio_stream_port_t and use app_le_audio_usb_port_t only?
#define APP_LE_AUDIO_CONVERT_STREAMING_PORT(streaming_port) ((app_le_audio_usb_port_t)streaming_port)
#define APP_LE_AUDIO_CONVERT_USB_PORT(usb_port) ((app_le_audio_stream_port_t)usb_port)

/**************************************************************************************************
* Structure
**************************************************************************************************/
typedef struct {
    uint8_t sampling_freq;                      /* Sampling frequency */
    uint16_t sdu_size;                          /* Maximum SDU size (octets) */
    app_le_audio_sdu_interval_t sdu_interval;   /* SDU interval */
    float bitrate;                              /* Bitrate (kbps) */
    uint8_t rtn;                                /* Retransmission number */
    uint8_t latency;                            /* Max transport latency (ms) */
} app_le_audio_qos_params_t;

typedef struct {
    audio_transmitter_id_t transmitter_id;                      /**< Audio transmitter ID */
    app_le_audio_transmitter_state_t curr_transmitter_state;    /**< Current audio transmitter state */
    app_le_audio_transmitter_state_t next_transmitter_state;    /**< Next audio transmitter state */
    uint8_t is_mixed;                                           /**< Audio stream is mixed or not, for SPK_0 and SPK_1 only */
    uint32_t config_op_queue;
    uint8_t mute;                           /**< mute state: MUTE or UNMUTE */
    uint8_t vol_level_left;                 /**< volume level L */
    uint8_t vol_level_right;                /**< volume level R */
    uint8_t mic_channel;                    /**< mic channel mode */
#ifdef AIR_SILENCE_DETECTION_ENABLE
    app_le_audio_silence_detection_status_enum silence_detection_status; /* Silence detection status of each port */
#endif
    app_le_audio_transmitter_wait_cnf_enum wait_cnf;
    uint32_t usb_sample_rate;               /**< USB sample rate in use. E.g. 16000, 32000, 44100, 48000, ... */
    uint8_t usb_sample_size;                /**< USB sample size in use. E.g. 1, 2, 3, 4 */
    uint8_t usb_channel;                    /**< USB channel in use. E.g. 1, 2 */
} app_le_audio_stream_info_t;

typedef struct {
    /* Silence detection status of device. The data is for lazy update use. Use app_le_audio_silence_detection_get_status() to get real status. */
    app_le_audio_silence_detection_status_enum silence_detection_status;
    TimerHandle_t delay_stop_timer_handle;  /* Delay stopping non-special detection mode when silence is detected. */
    uint32_t delay_stop_timer_period;    /* The timer period of delay stop timer. */
} app_le_audio_silence_detection_struct;

typedef struct {
    app_le_audio_mode_t curr_mode;
    app_le_audio_mode_t next_mode;
    app_le_audio_stream_info_t stream_info[APP_LE_AUDIO_STREAM_PORT_MAX];
    uint8_t open_audio_transmitter;
    uint8_t bidirectional;
    uint8_t set_buffer;
#ifdef AIR_SILENCE_DETECTION_ENABLE
    app_le_audio_silence_detection_struct silence_detection;
#endif
} app_le_audio_ctrl_t;

typedef struct {
    uint16_t sdu_size;                          /* Maximum SDU size (octets) */
    app_le_audio_sdu_interval_t sdu_interval;   /* SDU interval */
    float bitrate;                              /* Bitrate (kbps) */
    uint8_t low_rtn;                            /* Low latency retransmission number */
    uint8_t low_latency;                        /* Low latency Max transport latency (ms) */
    uint8_t high_rtn;                           /* High reliability retransmission number */
    uint8_t high_latency;                       /* High reliability max transport latency (ms) */
} app_le_audio_qos_params_tbl_t;

typedef void (*app_le_audio_timer_callback_t)(TimerHandle_t timer_handle, void *user_data);

typedef struct {
    TimerHandle_t timer_handle;
    app_le_audio_timer_callback_t callback;
    void *user_data;
} app_le_audio_timer_info_struct;

/**************************************************************************************************
* Public function
**************************************************************************************************/
bt_status_t app_le_audio_mute_audio_transmitter(app_le_audio_stream_port_t port);

bt_status_t app_le_audio_unmute_audio_transmitter(app_le_audio_stream_port_t port);

bt_status_t app_le_audio_set_audio_transmitter_volume_level(app_le_audio_stream_port_t port, uint8_t vol_level_left, uint8_t vol_level_right);

bt_status_t app_le_audio_set_audio_transmitter_mic_channel(uint32_t channel);

bt_status_t app_le_audio_init_audio_transmitter(app_le_audio_stream_port_t port);

bt_status_t app_le_audio_start_audio_transmitter(app_le_audio_stream_port_t port);

bt_status_t app_le_audio_stop_audio_transmitter(app_le_audio_stream_port_t port);

bt_status_t app_le_audio_open_audio_transmitter(bool bidirectional, app_le_audio_stream_port_mask_t streaming_port);

bt_status_t app_le_audio_close_audio_transmitter(void);

uint8_t app_le_audio_get_sample_freq(uint8_t in_smaple_rate);

app_le_audio_mode_t app_le_audio_get_current_mode(void);

bt_status_t app_le_audio_setup_iso_data_path(bt_handle_t handle, bt_gap_le_iso_data_path_direction_t direction, bt_gap_le_iso_data_path_id_t id);

bt_status_t app_le_audio_remove_iso_data_path(bt_handle_t handle, uint8_t direction);

#ifdef AIR_SILENCE_DETECTION_ENABLE
bool app_le_audio_silence_detection_is_speical_silence_detection_ongoing(void);

void app_le_audio_silence_detection_callback(audio_scenario_type_t scenario_type, bool silence_flag);

app_le_audio_silence_detection_mode_enum app_le_audio_silence_detection_get_silence_detection_mode(void);

void app_le_audio_silence_detection_stop_delay_stop_timer(void);

void app_le_audio_silence_detection_init(void);

void app_le_audio_silence_detection_handle_bt_off(void);

void app_le_audio_silence_detection_handle_event(app_le_audio_silence_detection_event_enum event_id, void *parameter);

void app_le_audio_silence_detection_set_status(app_le_audio_silence_detection_status_enum status);
#endif

bt_status_t app_le_audio_timer_start(TimerHandle_t *timer_handle, uint32_t timer_period, app_le_audio_timer_callback_t callback, void *user_data);

void app_le_audio_timer_stop(TimerHandle_t timer_handle);

void app_le_audio_timer_handle_timer_expired_event(TimerHandle_t timer_handle);

uint32_t app_le_audio_get_usb_sample_rate_in_use(app_le_audio_stream_port_t port);

uint8_t app_le_audio_get_usb_sample_size_in_use(app_le_audio_stream_port_t port);

uint8_t app_le_audio_get_usb_channel_in_use(app_le_audio_stream_port_t port);

#if defined AIR_BLE_AUDIO_DONGLE_LINE_IN_ENABLE || defined AIR_BLE_AUDIO_DONGLE_I2S_IN_ENABLE
uint8_t app_le_audio_get_streaming_port(void);
#endif

#endif /* AIR_LE_AUDIO_ENABLE */
#endif /* __APP_LE_AUDIO_UTILLITY_H__ */

