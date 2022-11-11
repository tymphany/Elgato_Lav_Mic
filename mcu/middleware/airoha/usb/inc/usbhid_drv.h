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

#ifndef USBHID_DRV_H
#define USBHID_DRV_H

#ifdef AIR_USB_HID_ENABLE

/* USB Middleware includes */
#include "usb.h"

/* USB HID Keyboard Enable*/
//#define USB_HID_KEYBOARD_ENABLE

/* USB HID GYROMETER Sensor Enable*/
//#define CUSTOM_GYROMETER

/* USB HID Call Control Enable*/
#define USB_HID_TELEPHONY_CALL_CONTROL_ENABLE

/* Align naming to race and mux  */
#define USB_MUX_PORT1                            0x00
#define USB_MUX_PORT2                            0x01
#define USB_MUX_PORT_UNUSE                       0xFF

#define USB_AUDIO_HID_REPORT_64BYTES             0x40  /* 64-byte for definition and 4-byte algnment*/
#define USB_AUDIO_HID_REPORT_62BYTES             0x3E  /* In/Out HID Report is 62-byte*/
#define USB_AUDIO_HID_REPORT_DATA_LENGTH         0x3B  /* HID Report size of  tool is 62-byte, Available data is 62-3 = 59 (0x3B)(Report ID, Length, Target Device)*/

/* In/Out Report */
#define USB_AUDIO_HID_OUT_REPORT_ID              0x06
#define USB_AUDIO_HID_IN_REPORT_ID               0x07

#define USB_AUDIO_HID_REPORT_ID                  0x00
#define USB_AUDIO_HID_REPORT_LENGTH              0x01
#define USB_AUDIO_HID_REPORT_TARGET              0x02
#define USB_AUDIO_HID_REPORT_DATA_BEGIN          0x03
#define USB_AUDIO_HID_REPORT_DATA_NULL           0x00

#define TARGET_LOCAL_DEVICE                      0x00
#define TARGET_REMOTE_DEVICE                     0x80
#define TARGET_REMOTE_MULTIDEVICE1               0x81
#define TARGET_REMOTE_MULTIDEVICE2               0x82
#define TARGET_REMOTE_MULTIDEVICE3               0x83
#define TARGET_REMOTE_MULTIDEVICE4               0x84
#define TARGET_EXTERNAL_DEVICE                   0x90
#define TARGET_INVALID_DEVICE                    0xFF

typedef enum {
    HID_EVENT_READY_TO_READ = 0,        /**< Ready to read event. */
    HID_EVENT_READY_TO_WRITE,           /**< Ready to write event. */
    HID_EVENT_USB_CONNECTION,           /**< USB connection event. */
    HID_EVENT_USB_DISCONNECTION,        /**< USB disconnection event. */
    HID_EVENT_DROP_RX_DATA,             /**< USB drop rx event. */
} hid_callback_event_t;

typedef void (*mux_usb_hid_callback)(uint8_t usb_port, hid_callback_event_t event, void *parameter);

/*Notifications, interrupt pipe*/
#define USB_AUDIO_HID_NOTIF_NETWORK_CONNECTION      0x00
#define USB_AUDIO_HID_NOTIF_RESPONSE_AVAILABLE      0x01
#define USB_AUDIO_HID_NOTIF_SERIAL_STATE            0x20

/* HID Report of Audio Setting*/
#define USB_HID_AUDIO_REPORT_ID                     0x0C
/* HID Report of Gyroscope Data*/
#define USB_HID_Gyroscope_DATA_REPORT_ID            0x03


#define USB_HID_NONE                                0x00
#define USB_HID_VOLUME_UP                           0x01
#define USB_HID_VOLUME_DOWN                         0x02
#define USB_HID_VOLUME_MUTE                         0x04
#define USB_HID_PLAY_PAUSE                          0x08
#define USB_HID_SCAN_NEXT_TRACK                     0x10
#define USB_HID_SCAN_PREVIOUS_TRACK                 0x20

#define USB_AUDIO_HID_TX_EP_NUMBER             1  /*interrupt pipe*/

/* USB HID */
#define USB_HID_INPUT_REPORT_TYPE                0x01
#define USB_HID_OUTPUT_REPORT_TYPE               0x02
#define USB_HID_FEATURE_REPORT_TYPE              0x03

#define USB_AUDIO_TEAMS_FEATURE_REPORT_ID        0x9A
#define USB_AUDIO_TEAMS_IN_REPORT_ID             0x9B
#define USB_HID_CALL_CONTROL_REPORT_ID           0x05

/* CFU report ID*/
#define USB_CFU_DUMMY_IN_REPORT_ID             0x20
#define USB_CFU_CONTENT_IN_REPORT_ID           0x22
#define USB_CFU_OFFER_IN_REPORT_ID             0x25
#define USB_CFU_CONTENT_OUT_REPORT_ID          0x20
#define USB_CFU_OFFER_OUT_REPORT_ID            0x25
#define USB_CFU_VERSION_FEATURE_REPORT_ID      0x20

/* CFU report length, +1 means report ID */
#define USB_CFU_DUMMY_IN_REPORT_LENGTH          1+1
#define USB_CFU_CONTENT_IN_REPORT_LENGTH       1+16
#define USB_CFU_OFFER_IN_REPORT_LENGTH         1+16
#define USB_CFU_CONTENT_OUT_REPORT_LENGTH      1+60
#define USB_CFU_OFFER_OUT_REPORT_LENGTH        1+16
#define USB_CFU_VERSION_FEATURE_REPORT_LENGTH  1+60

/* TEAMS - in report (0x05) for call control */
#define USB_HID_HOOK_SWITCH           0x01
#define USB_HID_LINE_BUSY_TONE        0x02
#define USB_HID_LINE                  0x04
#define USB_HID_PHONE_MUTE            0x08
#define USB_HID_FLASH                 0x10
#define USB_HID_REDIAL                0x20
#define USB_HID_SPEED_DIAL            0x40

#define USB_AUDIO_TEAMS_FEATURE_REPORT_LENGTH      32  /* Include Report ID*/
#define USB_AUDIO_TEAMS_IN_REPORT_LENGTH            2

#define USB_HID_REPOTR_DSCR_SIZE 512

typedef enum {
    AUDIO_HID_INTR_IDLE = 0,
    AUDIO_HID_INTR_SEND_ONE_PKT,
    AUDIO_HID_INTR_SEND_LAST_PKT
} USB_AUDIO_HID_INTR_PIPE_STATE;


/* HID device structure */
typedef struct {
    uint8_t                       data_interface_id;
    USB_AUDIO_HID_INTR_PIPE_STATE intr_state; /* interrupt pipe state machine */
    Usb_EpBIn_Status              *intrpipe;  /* interrupt EP, Communication Interface */
    Usb_Ep_Info                   *comm_ep_intr_info;
    Usb_Interface_Info            *comm_if_info;
    Usb_IAD_Dscr                  *iad_desc;
} UsbHid_Struct;

typedef enum {
    USB_Audio_HID_STATUS_OK                    = 0,     /* Operation completed successfully*/
    USB_Audio_HID_TX_LEN_IS_ZERO               = 1,     /* TX data length is zero */
    USB_Audio_HID_TX_LEN_TOO_LARGE             = 2,     /* TX data length is too large */
    USB_Audio_HID_CHARGER_DETECT_ERROR         = 3,     /* Charger type is wrong */
    USB_Audio_HID_SEND_DATA_ERROR              = 4,     /* HID TX status is error */
    USB_Audio_HID_IS_NOT_READY                 = 5,     /* HID is not ready */
} USB_Audio_HID_t;

typedef enum {
    USB_HID_KEYBOARD_KEY_WIN_KEY_ONLY          = 0x00,  /* Press Win Key Only*/
    USB_HID_KEYBOARD_KEY_C                     = 0x06,  /* Press Win Key + C */
    USB_HID_KEYBOARD_KEY_D                     = 0x07,  /* Press Win Key + D */  /* Minimize All Windows */
    USB_HID_KEYBOARD_KEY_E                     = 0x08,  /* Press Win Key + E */  /* Open My Computer */
    USB_HID_KEYBOARD_KEY_L                     = 0x0F,  /* Press Win Key + L */  /* Lock Computer */
} USB_HID_WINDOWS_KEY_MODIFIER_FUNC_t;

void USB_Audio_HID_Ep0_ReadData(uint8_t *data);
void USB_Audio_HID_Ep0_DispatchData(uint8_t *data);

USB_Audio_HID_t USB_Audio_HID_TX(uint8_t *data, uint8_t data_len);
USB_Audio_HID_t USB_Audio_HID_VolumeUp(uint8_t step);
USB_Audio_HID_t USB_Audio_HID_VolumeDown(uint8_t step);
USB_Audio_HID_t USB_Audio_HID_VolumeMute();
USB_Audio_HID_t USB_Audio_HID_PlayPause();
USB_Audio_HID_t USB_Audio_HID_PlayPause_RejectCall();
USB_Audio_HID_t USB_Audio_HID_ScanNextTrack();
USB_Audio_HID_t USB_Audio_HID_ScanPreviousTrack();
USB_Audio_HID_t USB_Audio_HID_Win_Key_Func(USB_HID_WINDOWS_KEY_MODIFIER_FUNC_t key);
USB_Audio_HID_t USB_Audio_HID_Gyroscope_Send_Rotate_Data_and_Reset(int16_t rx, int16_t  ry, int16_t  rz, int16_t vx, int16_t  vy, int16_t  vz, uint8_t frame_count);

void USB_HID_Create(void *ifname);
void USB_HID_Reset(void);
void USB_HID_Enable(void);
void USB_HID_Speed_Reset(bool b_other_speed);

void USB_Init_Hid_Status(void);
void USB_Release_Hid_Status(void);

/* HID data handler */
#define USB_HID_HANDLER_ERROR_OK 0
#define USB_HID_HANDLER_ERROR_FAIL -1
#define USB_HID_HANDLER_ERROR_INITED -2

#define USB_HID_HANDER_NUMS 8
#define USB_HID_INVALID_REPORT_ID 0xFF

typedef void (*usb_hid_handler_callback)(uint8_t *data, uint32_t data_length);

typedef struct {
    uint8_t report_id;
    uint8_t report_length;
    usb_hid_handler_callback handler_callback;
} usb_hid_handler_t;

int32_t usb_hid_handler_rx_register(uint8_t report_id, uint8_t report_length, usb_hid_handler_callback callback);
int32_t usb_hid_handler_tx_register(uint8_t report_id, uint8_t report_length, usb_hid_handler_callback callback);
int32_t usb_hid_rx_handler_unregister(uint8_t report_id);
int32_t usb_hid_tx_handler_unregister(uint8_t report_id);

#endif /* AIR_USB_HID_ENABLE */

/******************************************************************************
 * USB HID 1.11 spec
 *
 * Below is constant or code in USB HID 1.11 spec
 * https://www.usb.org/document-library/device-class-definition-hid-111
 *****************************************************************************/
/**
 * SubClass code of HID device.
 */
typedef enum {
    USB_HID_SUBCLASS_CODE_NONE = 0x00, /* No Subclass */
    USB_HID_SUBCLASS_CODE_BOOT = 0x01, /* Boot Interface Subclass */
} usb_hid_subclass_code_t;

/**
 * Protocol code of No Subclass (0x00).
 */
typedef enum {
    USB_HID_PROTOCOL_CODE_NONE = 0x00,
} usb_hid_protocol_code_t;

/**
 * Protocol code of SubClass Boot Interface (0x01).
 */
typedef enum {
    USB_HID_BOOT_PROTOCOL_CODE_NONE     = 0x00,
    USB_HID_BOOT_PROTOCOL_CODE_KEYBOARD = 0x01,
    USB_HID_BOOT_PROTOCOL_CODE_MOUSE    = 0x02,
} usb_hid_boot_protocol_code_t;

/**
 * Descriptor Type of HID class
 */
typedef enum {
    USB_HID_DESC_TYPE_HID    = 0x21,
    USB_HID_DESC_TYPE_REPORT = 0x22,
    USB_HID_DESC_TYPE_PHY    = 0x23,
} usb_hid_desc_type_t;

/**
 * HID Class Specification release number
 */
typedef enum {
    USB_HID_BCD_101 = 0x0101,
    USB_HID_BCD_110 = 0x0110,
    USB_HID_BCD_111 = 0x0111,
} usb_hid_bcd_t;

/**
 * USB HID Country Code
 *
 * Most device is zero, not supported.
 * Keyboards may use the field to indicate the language of the key caps.
 */
typedef enum {
    USB_HID_CONTRY_NONE = 0,
} usb_hid_contry_t;

typedef enum {
    USB_HID_BREQUEST_GET_REPORT   = 0x01,
    USB_HID_BREQUEST_GET_IDLE     = 0x02,
    USB_HID_BREQUEST_GET_PROTOCOL = 0x03,
    USB_HID_BREQUEST_SET_REPORT   = 0x09,
    USB_HID_BREQUEST_SET_IDLE     = 0x0A,
    USB_HID_BREQUEST_SET_PROTOCOL = 0x0B,
} usb_hid_brequest_t;

/******************************************************************************
 * USB HID Driver Structures
 *****************************************************************************/
typedef struct __attribute__((packed)) {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} usb_hid_dscr_interface_t;

typedef struct __attribute__((packed)) {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bEndpointAddress;
    uint8_t bmAttributes;
    uint16_t wMaxPacketSize;
    uint8_t bInterval;
} usb_hid_dscr_endpoint_t;

/**
 * The continued descriptor follow HID Descriptor
 *
 * The num of continued descriptors is 1 in most condiction.
 */
typedef struct __attribute__((packed)) {
    uint8_t bDescriptorType;
    uint16_t wDescriptorLength;
} usb_hid_cont_dscr_t;

/**
 * HID Descriptor
 *
 * bNumDescriptors is 1 in most condiction.
 */
#define USB_DSCR_HID_MAX_CONT_DSCRS 5
typedef struct __attribute__((packed)) {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdHID;
    uint8_t bCountryCode;
    uint8_t bNumDescriptors;
    usb_hid_cont_dscr_t cont_dscrs[USB_DSCR_HID_MAX_CONT_DSCRS];
} usb_hid_dscr_hid_t;

/**
 * USB hid descriptor serialize function
 */
typedef int16_t (*usb_hid_serial_func_t)(void *, void *, uint16_t);

typedef struct {
    void *dscr;
    usb_hid_serial_func_t serial_func;
} usb_hid_dscr_hdlr_t;

typedef enum {
    USB_REPORT_DSCR_TYPE_MUX,
    USB_REPORT_DSCR_TYPE_AC,
    USB_REPORT_DSCR_TYPE_TEAMS,
    USB_REPORT_DSCR_TYPE_TELEPHONY,
    USB_REPORT_DSCR_TYPE_KEYBOARD,
    USB_REPORT_DSCR_TYPE_GYROMETER,
    USB_REPORT_DSCR_TYPE_SENSOR,
    USB_REPORT_DSCR_TYPE_CFU,
    USB_REPORT_DSCR_TYPE_CUSTOMER,
    USB_REPORT_DSCR_TYPE_NUM, /* The available num of hid report dscrs */
} usb_hid_report_dscr_type_t;

typedef struct {
    usb_hid_report_dscr_type_t type;
    const char *name;
    const uint8_t *dscr;
    uint8_t length;
    bool enable;
} usb_hid_report_dscr_hdlr_t;

void usb_hid_set_dscr_enable(usb_hid_report_dscr_type_t* list, uint8_t len);

#endif /* USBHID_DRV_H */