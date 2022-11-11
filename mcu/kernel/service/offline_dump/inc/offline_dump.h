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

#ifndef __OFFLINE_DUMP_H__
#define __OFFLINE_DUMP_H__

#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

#include "offline_dump_port.h"

#define OFFLINE_FOTA_ADDRESS             offline_dump_query_FOTA_address()
#define OFFLINE_FOTA_RESERVE             offline_dump_query_FOTA_reserve()
#define SERIAL_FLASH_BLOCK_SIZE          4096

#define OFFLINE_BUILD_TIME_COUNT         1
#define OFFLINE_BUILD_INFO_SIZE          48
#define OFFLINE_BUILD_INFO_TOTAL_SIZE    OFFLINE_BUILD_TIME_COUNT * OFFLINE_BUILD_INFO_SIZE

typedef struct {
    uint8_t status;
    uint8_t reserved[3];
    uint16_t header_checksum;
    uint16_t header_size;
    uint8_t dump_version;
    uint8_t payload_type;
    uint8_t sdk_ver_cnt;
    uint8_t build_time_cnt;
    uint8_t magic_number[32];
    uint32_t payload_size;
    uint32_t payload_real_size;
    uint32_t seq_number;
    uint8_t rtc_time[8];
    uint8_t sdk_version[OFFLINE_BUILD_INFO_SIZE];
    uint8_t build_time[OFFLINE_BUILD_INFO_TOTAL_SIZE];
} offline_dump_header_t;

/* Offline dump version */
#define OFFLINE_REGION_VERSION                    0x01

/* syslog dump information */
#define OFFLINE_REGION_TYPE_SYSLOG                0x01
#define SYSLOG_MAGIC_NUMBER                       "OFFLINE_DUMP_V1_SYSLOG"
#ifndef OFFLINE_REGION_SYSLOG_BASE_ADDR
#define OFFLINE_REGION_SYSLOG_BASE_ADDR           (OFFLINE_FOTA_ADDRESS)
#endif
#ifndef OFFLINE_REGION_SYSLOG_CELL_COUNT
#define OFFLINE_REGION_SYSLOG_CELL_COUNT          1
#endif
#ifdef AIR_SYSLOG_BUFFER_EXPAND_ENABLE
#define OFFLINE_REGION_SYSLOG_CELL_SIZE           (8 * SERIAL_FLASH_BLOCK_SIZE)
#else
#define OFFLINE_REGION_SYSLOG_CELL_SIZE           (4 * SERIAL_FLASH_BLOCK_SIZE)
#endif
#define OFFLINE_REGION_SYSLOG_CELL_VALID_SIZE     (OFFLINE_REGION_SYSLOG_CELL_SIZE - sizeof(offline_dump_header_t))

/* mini dump information */
#define OFFLINE_REGION_TYPE_MINIDUMP              0x02
#define MINI_DUMP_MAGIC_NUMBER                    "OFFLINE_DUMP_V2_MINIDUMP"
#ifndef OFFLINE_REGION_MINI_DUMP_BASE_ADDR
#define OFFLINE_REGION_MINI_DUMP_BASE_ADDR        (OFFLINE_REGION_SYSLOG_BASE_ADDR + OFFLINE_REGION_SYSLOG_CELL_COUNT * OFFLINE_REGION_SYSLOG_CELL_SIZE)
#endif
#ifndef OFFLINE_REGION_MINI_DUMP_CELL_COUNT
#define OFFLINE_REGION_MINI_DUMP_CELL_COUNT       1
#endif

#if (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697) || (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 2533) || (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 5932) || (PRODUCT_VERSION == 7698)
#define OFFLINE_REGION_MINI_DUMP_CELL_SIZE        (2 * SERIAL_FLASH_BLOCK_SIZE)
#elif defined(AIR_BTA_IC_PREMIUM_G2) || defined(AIR_BTA_IC_PREMIUM_G3) || defined(AIR_BTA_IC_STEREO_HIGH_G3) 
#define OFFLINE_REGION_MINI_DUMP_CELL_SIZE        ((OFFLINE_FOTA_RESERVE - OFFLINE_REGION_SYSLOG_CELL_COUNT * OFFLINE_REGION_SYSLOG_CELL_SIZE) / OFFLINE_REGION_MINI_DUMP_CELL_COUNT)
#elif (PRODUCT_VERSION == 3335)
#define OFFLINE_REGION_MINI_DUMP_CELL_SIZE        (2 * SERIAL_FLASH_BLOCK_SIZE)
#endif

#define OFFLINE_REGION_MINI_DUMP_CELL_VALID_SIZE  (OFFLINE_REGION_MINI_DUMP_CELL_SIZE - sizeof(offline_dump_header_t))


#ifdef MTK_FOTA_ENABLE

#if (PRODUCT_VERSION == 2523) || (PRODUCT_VERSION == 2533) || (PRODUCT_VERSION == 7686) || (PRODUCT_VERSION == 7698) || (PRODUCT_VERSION == 7682) || (PRODUCT_VERSION == 5932) || (PRODUCT_VERSION == 7687) || (PRODUCT_VERSION == 7697)

#ifndef MTK_FOTA_VIA_RACE_CMD
#include "fota_internal.h"
#define PORT_FOTA_REGION_ACCESS()    fota_write_upgrade_status(FOTA_STATUS_IS_EMPTY)
#else
#define PORT_FOTA_REGION_ACCESS()
#endif

#elif (PRODUCT_VERSION == 1552) || (PRODUCT_VERSION == 2552) || (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)

#ifdef MTK_FOTA_VIA_RACE_CMD
#include "fota_multi_info.h"
#define PORT_FOTA_REGION_ACCESS()    fota_upgrade_flag_clear()
#else
#define PORT_FOTA_REGION_ACCESS()
#endif

#endif

#endif

typedef enum {
    OFFLINE_REGION_SYSLOG = 0,
    OFFLINE_REGION_MINI_DUMP,
    OFFLINE_REGION_MAX,
} offline_dump_region_type_t;

bool offline_dump_region_init(void);

bool offline_dump_region_alloc(offline_dump_region_type_t region_type, uint32_t *p_start_addr);

bool offline_dump_region_write(offline_dump_region_type_t region_type, uint32_t curr_addr, uint8_t *data, uint32_t length);

bool offline_dump_region_write_end(offline_dump_region_type_t region_type, uint32_t total_length);

bool offline_dump_region_read(offline_dump_region_type_t region_type, uint32_t curr_addr, uint8_t *buf, uint32_t length);

bool offline_dump_region_query_seq_range(offline_dump_region_type_t region_type, uint32_t *p_min_seq, uint32_t *p_max_seq);

bool offline_dump_region_query_by_seq(offline_dump_region_type_t region_type, uint32_t seq, uint32_t *p_start_addr, uint32_t *p_total_length);

bool offline_dump_region_query_cell_size(offline_dump_region_type_t region_type, uint32_t *p_cell_size);

bool offline_dump_region_query_cell_valid_size(offline_dump_region_type_t region_type, uint32_t *p_valid_size);

#endif

