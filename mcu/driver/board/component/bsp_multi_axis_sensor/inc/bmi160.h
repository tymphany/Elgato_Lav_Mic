/**
* Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file bmi160.c
* @date 10/01/2020
* @version  3.8.1
*
*/

#ifndef __BMI160_H__
#define __BMI160_H__

/************************************************/
#include <stdint.h>
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"
#include "bsp_multi_axis_sensor_common.h"
#ifdef INCLUDE_AXIS_SENSOR_IIC
#include "hal_i2c_master.h"
#endif
#ifdef INCLUDE_AXIS_SENSOR_SPI
#include "hal_spi_master.h"
#endif
#include "hal_platform.h"

/************************************************/
/* bmi160 sensor debug information*/
#undef BMI_DEBUG
#ifdef BMI_DEBUG
#define BMI160_DEBUG_INF(fmt,...)  printf("[BMI160] "fmt,##__VA_ARGS__)
#else
#define BMI160_DEBUG_INF(fmt,...)
#endif

typedef char      s8;
typedef short     s16;
typedef unsigned int  s32;
typedef unsigned char u8;
typedef unsigned short  u16;
typedef unsigned int  u32;

typedef void (*axis_sensor_dma_callback_t)(void *user_data);



#define BMI160_RD_FUNC_PTR int (*bus_read)(u8,\
      u8, u8 *, u8)

#define BMI160_RD_DMA_FUNC_PTR int (*bus_read_dma)(u8,\
      u8, u8 *, u8,axis_sensor_dma_callback_t ,void *)



#define BMI160_WR_FUNC_PTR int (*bus_write)(u8,u8,\
                              u8 *, u8)

#define BMI160_BUS_WRITE_FUNC(dev_addr, reg_addr, reg_data, wr_len)\
        bus_write(dev_addr, reg_addr, reg_data, wr_len)

#define BMI160_BRD_FUNC_PTR int \
                                        (*burst_read)(u8, u8, u8 *, u32 )

#define BMI160_BUS_READ_FUNC(dev_addr, reg_addr, reg_data, r_len)\
        bus_read(dev_addr, reg_addr, reg_data, r_len)

#define BMI160_BUS_READ_DMA_FUNC(dev_addr, reg_addr, reg_data, r_len, dev_callback,user_data)\
        bus_read_dma(dev_addr, reg_addr, reg_data, r_len, dev_callback,user_data)



#define     DATA_BUFFER_LEN 8
#define     SENSOR_SW_FIFO_LEN 20

#define BSP_MULTI_AXIS_HEAD_SIZE 12
#define BSP_MULTI_AXIS_ACCEL_DATA_SIZE 12
#define BSP_MULTI_AXIS_GYRO_DATA_SIZE 12
#define BSP_MULTI_AXIS_MAG_DATA_SIZE 12
#define BSP_MULTI_AXIS_FRAME_SIZE  48

#define      UINT16_T_MAX 0x8000
/** Soft reset command */
#define      BMI160_SOFT_RESET_CMD                0xb6

/** Self test configurations */
#define     BMI160_ACCEL_SELF_TEST_CONFIG        0x2C
#define     BMI160_ACCEL_SELF_TEST_SIGN_POSITIVE   0x01
#define     BMI160_ACCEL_SELF_TEST_SIGN_NEGATIVE  0x00
#define     BMI160_ACCEL_SELF_TEST_AMP_HIGH   0x01
#define     BMI160_ACCEL_SELF_TEST_AMP_LOW  0x00
#define     BMI160_ACCEL_SELF_TEST_EN       0x01
#define     BMI160_ACCEL_SELF_TEST_LIMIT         8192
#define     BMI160_GYRO_SELF_TEST_OK 0x1
#define     BMI160_GYRO_SELF_TEST_START  0x1

/** temperature about */
#define     BMI160_TEMP_BASE            23
#define     BMI160_TEMP_RESOLUTION 0.0019531     /* (1/2)9 */


/********************************/
/**\name RUNNING MODE DEFINITIONS */
/*******************************/
#define STANDARD_UI_9DOF_FIFO     0
#define STANDARD_UI_IMU_FIFO      1
#define STANDARD_UI_IMU         2
#define STANDARD_UI_ADVANCEPOWERSAVE  3
#define ACCEL_PEDOMETER         4
#define      APPLICATION_HEAD_TRACKING    5
#define      APPLICATION_NAVIGATION     6
#define      APPLICATION_REMOTE_CONTROL   7
#define      APPLICATION_INDOOR_NAVIGATION  8
/********************************/
/**\name MAG INTERFACE */
/*******************************/
#define C_BMI160_BYTE_COUNT    (2)
#define     BMI160_SLEEP_STATE     (0x00)
#define     BMI160_WAKEUP_INTR     (0x00)
#define     BMI160_SLEEP_TRIGGER   (0x04)
#define     BMI160_WAKEUP_TRIGGER  (0x02)
#define     BMI160_ENABLE_FIFO_WM  (0x02)
#define     BMI160_MAG_INTERFACE_OFF_PRIMARY_ON   0x00
#define     BMI160_MAG_INTERFACE_ON_PRIMARY_ON    0x02


#define BMI160_MDELAY_DATA_TYPE                 u32

#define FIFO_FRAME_CNT 8
/***************************************************************/
/**\name  BUS READ AND WRITE FUNCTION POINTERS        */
/***************************************************************/
#define BMI160_I2C_ADDR1                   (0x68)
/**< I2C Address needs to be changed */
#define BMI160_I2C_ADDR2                    (0x69)

/**< I2C address of AKM09912*/
/*******************************************/
/**\name  CONSTANTS        */
/******************************************/
#define  BMI160_GEN_READ_WRITE_DATA_LENGTH  (1)
#define  BMI160_MAXIMUM_TIMEOUT             (10)

/* output data rate condition check*/
#define  BMI160_OUTPUT_DATA_RATE0 (0)
#define  BMI160_OUTPUT_DATA_RATE1 (1)
#define  BMI160_OUTPUT_DATA_RATE2 (2)
#define  BMI160_OUTPUT_DATA_RATE3 (3)
#define  BMI160_OUTPUT_DATA_RATE4 (4)
#define  BMI160_OUTPUT_DATA_RATE5 (5)
#define  BMI160_OUTPUT_DATA_RATE6 (14)
#define  BMI160_OUTPUT_DATA_RATE7 (15)

#define BMI160_GYRO_X_LSB_BYTE              (0)
#define BMI160_GYRO_X_MSB_BYTE              (1)
#define BMI160_GYRO_Y_LSB_BYTE              (0)
#define BMI160_GYRO_Y_MSB_BYTE              (1)
#define BMI160_GYRO_Z_LSB_BYTE              (0)
#define BMI160_GYRO_Z_MSB_BYTE              (1)
#define BMI160_DATA_FRAME_GYRO_X_LSB_BYTE   (0)
#define BMI160_DATA_FRAME_GYRO_X_MSB_BYTE   (1)
#define BMI160_DATA_FRAME_GYRO_Y_LSB_BYTE   (2)
#define BMI160_DATA_FRAME_GYRO_Y_MSB_BYTE   (3)
#define BMI160_DATA_FRAME_GYRO_Z_LSB_BYTE   (4)
#define BMI160_DATA_FRAME_GYRO_Z_MSB_BYTE   (5)

#define BMI160_ACCEL_X_LSB_BYTE              (0)
#define BMI160_ACCEL_X_MSB_BYTE              (1)
#define BMI160_ACCEL_Y_LSB_BYTE              (0)
#define BMI160_ACCEL_Y_MSB_BYTE              (1)
#define BMI160_ACCEL_Z_LSB_BYTE              (0)
#define BMI160_ACCEL_Z_MSB_BYTE              (1)
#define BMI160_DATA_FRAME_ACCEL_X_LSB_BYTE   (0)
#define BMI160_DATA_FRAME_ACCEL_X_MSB_BYTE   (1)
#define BMI160_DATA_FRAME_ACCEL_Y_LSB_BYTE   (2)
#define BMI160_DATA_FRAME_ACCEL_Y_MSB_BYTE   (3)
#define BMI160_DATA_FRAME_ACCEL_Z_LSB_BYTE   (4)
#define BMI160_DATA_FRAME_ACCEL_Z_MSB_BYTE   (5)

/* Delay definitions*/
#define BMI160_GEN_READ_WRITE_DELAY                  (1)
#define BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY                  (4)
#define BMI160_SOFT_RESET__DELAY              (10)
#define BMI160_MEASUREMENT_DELAY              (25)
#define BMI160_ACCEL_SELF_TEST_DELAY        (50)
#define BMI160_GYRO_SWITCH_TO_NORMAL_DELAY                 (60)
/* Error Description - Reg Addr --> (0x02), Bit --> 7 */
#define BMI160_USER_MAG_DADA_RDY_ERR__POS               (7)

/* accel range check*/
#define BMI160_ACCEL_RANGE0  (3)
#define BMI160_ACCEL_RANGE1  (5)
#define BMI160_ACCEL_RANGE3  (8)
#define BMI160_ACCEL_RANGE4  (12)

/* check the status of registers*/
#define  BMI160_FOC_STAT_HIGH     (1)
#define  BMI160_SIG_MOTION_STAT_HIGH  (1)
#define  BMI160_STEP_DET_STAT_HIGH    (1)

/*  INTERRUPT MAPS    */
#define BMI160_INTR1_MAP_LOW_G      (0)
#define BMI160_INTR2_MAP_LOW_G      (1)
#define BMI160_INTR1_MAP_HIGH_G     (0)
#define BMI160_INTR2_MAP_HIGH_G     (1)
#define BMI160_INTR1_MAP_ANY_MOTION   (0)
#define BMI160_INTR2_MAP_ANY_MOTION   (1)
#define BMI160_INTR1_MAP_NOMO     (0)
#define BMI160_INTR2_MAP_NOMO     (1)
#define BMI160_INTR1_MAP_DOUBLE_TAP   (0)
#define BMI160_INTR2_MAP_DOUBLE_TAP   (1)
#define BMI160_INTR1_MAP_SINGLE_TAP   (0)
#define BMI160_INTR2_MAP_SINGLE_TAP   (1)
#define BMI160_INTR1_MAP_ORIENT     (0)
#define BMI160_INTR2_MAP_ORIENT     (1)
#define BMI160_INTR1_MAP_FLAT     (0)
#define BMI160_INTR2_MAP_FLAT     (1)
#define BMI160_INTR1_MAP_DATA_RDY   (0)
#define BMI160_INTR2_MAP_DATA_RDY   (1)
#define BMI160_INTR1_MAP_FIFO_WM    (0)
#define BMI160_INTR2_MAP_FIFO_WM    (1)
#define BMI160_INTR1_MAP_FIFO_FULL      (0)
#define BMI160_INTR2_MAP_FIFO_FULL      (1)
#define BMI160_INTR1_MAP_PMUTRIG        (0)
#define BMI160_INTR2_MAP_PMUTRIG    (1)

/**************************************************************/
/**\name  INTERRUPT1 MAPPIONG OF DATA READY LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->3 */
#define BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__POS               (3)
#define BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__LEN               (1)
#define BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__MSK               (0x08)
#define BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__REG        \
(BMI160_USER_INTR_MAP_1_ADDR)

/*condition check for reading and writing data*/
#define BMI160_MAX_VALUE_SIGNIFICANT_MOTION      (1)
#define BMI160_MAX_VALUE_FIFO_FILTER    (1)
#define BMI160_MAX_VALUE_FIFO_TIME      (1)
#define BMI160_MAX_VALUE_FIFO_INTR      (1)
#define BMI160_MAX_VALUE_FIFO_HEADER    (1)
#define BMI160_MAX_VALUE_FIFO_MAG       (1)
#define BMI160_MAX_VALUE_FIFO_ACCEL     (1)
#define BMI160_MAX_VALUE_FIFO_GYRO      (1)
#define BMI160_MAX_VALUE_SOURCE_INTR    (1)
#define BMI160_MAX_VALUE_LOW_G_MODE     (1)
#define BMI160_MAX_VALUE_NO_MOTION      (1)
#define BMI160_MAX_VALUE_TAP_SHOCK      (1)
#define BMI160_MAX_VALUE_TAP_QUIET      (1)
#define BMI160_MAX_VALUE_ORIENT_UD      (1)
#define BMI160_MAX_VALUE_ORIENT_AXES    (1)
#define BMI160_MAX_VALUE_NVM_PROG       (1)
#define BMI160_MAX_VALUE_SPI3           (1)
#define BMI160_MAX_VALUE_PAGE           (1)
#define BMI160_MAX_VALUE_I2C_WDT        (1)
#define BMI160_MAX_VALUE_SLEEP_STATE    (1)
#define BMI160_MAX_VALUE_WAKEUP_INTR    (1)
#define BMI160_MAX_VALUE_SELFTEST_SIGN  (1)
#define BMI160_MAX_VALUE_SELFTEST_AMP   (1)
#define BMI160_MAX_VALUE_SELFTEST_START (1)

#define BMI160_MAX_GYRO_WAKEUP_TRIGGER    (3)
#define BMI160_MAX_ACCEL_SELFTEST_AXIS      (3)
#define BMI160_MAX_GYRO_STEP_COUNTER        (1)
#define BMI160_MAX_GYRO_BW                  (3)
#define BMI160_MAX_ACCEL_BW                 (7)
#define BMI160_MAX_ORIENT_MODE              (3)
#define BMI160_MAX_ORIENT_BLOCKING          (3)
#define BMI160_MAX_FLAT_HOLD                (3)
#define BMI160_MAX_ACCEL_FOC                (3)
#define BMI160_MAX_IF_MODE                  (3)
#define BMI160_MAX_TARGET_PAGE              (3)
#define BMI160_MAX_GYRO_RANGE               (4)
#define BMI160_MAX_GYRO_SLEEP_TIGGER        (7)
#define BMI160_MAX_TAP_TURN                 (7)
#define BMI160_MAX_UNDER_SAMPLING           (1)
#define BMI160_MAX_UNDER_SIG_MOTION         (3)
#define BMI160_MAX_ACCEL_OUTPUT_DATA_RATE   (12)
#define BMI160_MAX_LATCH_INTR               (15)
#define BMI160_MAX_FLAT_HYST                (15)
#define BMI160_MAX_ORIENT_THETA             (63)
#define BMI160_MAX_FLAT_THETA               (63)

/* bus read and write length for mag, accel and gyro*/
#define BMI160_GYRO_DATA_LENGTH    (2)
#define BMI160_GYRO_XYZ_DATA_LENGTH  (6)
#define BMI160_ACCEL_DATA_LENGTH   (2)
#define BMI160_ACCEL_XYZ_DATA_LENGTH (6)
#define BMI160_TEMP_DATA_LENGTH    (2)
#define BMI160_FIFO_DATA_LENGTH    (2)
#define BMI160_STEP_COUNTER_LENGTH   (2)
#define BMI160_SENSOR_TIME_LENGTH  (3)

/* Delay definitions*/
#define BMI160_SEC_INTERFACE_GEN_READ_WRITE_DELAY    (5)

/****************************************************/
/**\name  ARRAY SIZE DEFINITIONS      */
/***************************************************/
#define BMI160_ACCEL_X_DATA_SIZE   (2)
#define BMI160_ACCEL_Y_DATA_SIZE   (2)
#define BMI160_ACCEL_Z_DATA_SIZE   (2)
#define BMI160_ACCEL_XYZ_DATA_SIZE (6)

#define BMI160_GYRO_X_DATA_SIZE    (2)
#define BMI160_GYRO_Y_DATA_SIZE    (2)
#define BMI160_GYRO_Z_DATA_SIZE    (2)
#define BMI160_GYRO_XYZ_DATA_SIZE  (6)

/***************************************************/
/**\name INTERRUPT ENABLE REGISTERS*/
/******************************************************/
#define BMI160_USER_INTR_ENABLE_0_ADDR      (0X50)
#define BMI160_USER_INTR_ENABLE_1_ADDR      (0X51)
#define BMI160_USER_INTR_ENABLE_2_ADDR      (0X52)
#define BMI160_USER_INTR_OUT_CTRL_ADDR      (0X53)

/* INT ENABLE 1 */
#define BMI160_HIGH_G_X_ENABLE       (0)
#define BMI160_HIGH_G_Y_ENABLE       (1)
#define BMI160_HIGH_G_Z_ENABLE       (2)
#define BMI160_LOW_G_ENABLE          (3)
#define BMI160_DATA_RDY_ENABLE       (4)
#define BMI160_FIFO_FULL_ENABLE      (5)
#define BMI160_FIFO_WM_ENABLE        (6)

/* IN OUT CONTROL */
#define BMI160_INTR1_EDGE_CTRL      (0)
#define BMI160_INTR2_EDGE_CTRL      (1)
#define BMI160_INTR1_LEVEL        (0)
#define BMI160_INTR2_LEVEL        (1)
#define BMI160_INTR1_OUTPUT_TYPE    (0)
#define BMI160_INTR2_OUTPUT_TYPE    (1)
#define BMI160_INTR1_OUTPUT_ENABLE    (0)
#define BMI160_INTR2_OUTPUT_ENABLE    (1)

/***************************************************/
/**\name MAP INTERRUPT 1 and 2 REGISTERS*/
/******************************************************/
#define BMI160_USER_INTR_MAP_0_ADDR       (0X55)
#define BMI160_USER_INTR_MAP_1_ADDR       (0X56)
#define BMI160_USER_INTR_MAP_2_ADDR       (0X57)
#define BMI160_INT_STATUS_ADDR                                                 (0X1C)

/**************************************************************/
/**\name  INTERRUPT1 MAPPIONG OF DATA READY LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->7 */
#define BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__POS               (7)
#define BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__LEN               (1)
#define BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__MSK               (0x80)
#define BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__REG  \
(BMI160_USER_INTR_MAP_1_ADDR)

/**************************************************************/
/**\name  HIGH_G XYZ ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_1 Description - Reg Addr --> (0x51), Bit -->0 */
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__POS               (0)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__LEN               (1)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__MSK               (0x01)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__REG             \
(BMI160_USER_INTR_ENABLE_1_ADDR)

/* Int_En_1 Description - Reg Addr --> (0x51), Bit -->1 */
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__POS               (1)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__LEN               (1)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__MSK               (0x02)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__REG             \
(BMI160_USER_INTR_ENABLE_1_ADDR)

/* Int_En_1 Description - Reg Addr --> (0x51), Bit -->2 */
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__POS               (2)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__LEN               (1)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__MSK               (0x04)
#define BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__REG             \
(BMI160_USER_INTR_ENABLE_1_ADDR)


/**************************************************************/
/**\name  LOW_G ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_1 Description - Reg Addr --> (0x51), Bit -->3 */
#define BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__POS               (3)
#define BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__LEN               (1)
#define BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__MSK               (0x08)
#define BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__REG           \
(BMI160_USER_INTR_ENABLE_1_ADDR)


/* Int_En_1 Description - Reg Addr --> (0x51), Bit -->4 */
#define BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__POS               (4)
#define BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__LEN               (1)
#define BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__MSK               (0x10)
#define BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__REG         \
(BMI160_USER_INTR_ENABLE_1_ADDR)

/**************************************************************/
/**\name  FIFO FULL AND WATER MARK ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_En_1 Description - Reg Addr --> (0x51), Bit -->5 */
#define BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__POS               (5)
#define BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__LEN               (1)
#define BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__MSK               (0x20)
#define BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__REG               \
(BMI160_USER_INTR_ENABLE_1_ADDR)

/* Int_En_1 Description - Reg Addr --> (0x51), Bit -->6 */
#define BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__POS               (6)
#define BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__LEN               (1)
#define BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__MSK               (0x40)
#define BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__REG            \
(BMI160_USER_INTR_ENABLE_1_ADDR)


/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->2 */
#define BMI160_USER_INTR1_OUTPUT_TYPE__POS               (2)
#define BMI160_USER_INTR1_OUTPUT_TYPE__LEN               (1)
#define BMI160_USER_INTR1_OUTPUT_TYPE__MSK               (0x04)
#define BMI160_USER_INTR1_OUTPUT_TYPE__REG               \
(BMI160_USER_INTR_OUT_CTRL_ADDR)

/**************************************************************/
/**\name  OUTPUT TYPE ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->3 */
#define BMI160_USER_INTR1_OUTPUT_ENABLE__POS               (3)
#define BMI160_USER_INTR1_OUTPUT_ENABLE__LEN               (1)
#define BMI160_USER_INTR1_OUTPUT_ENABLE__MSK               (0x08)
#define BMI160_USER_INTR1_OUTPUT_ENABLE__REG    \
(BMI160_USER_INTR_OUT_CTRL_ADDR)


/**************************************************************/
/**\name    LEVEL CONTROL ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->1 */
#define BMI160_USER_INTR1_LEVEL__POS               (1)
#define BMI160_USER_INTR1_LEVEL__LEN               (1)
#define BMI160_USER_INTR1_LEVEL__MSK               (0x02)
#define BMI160_USER_INTR1_LEVEL__REG               \
(BMI160_USER_INTR_OUT_CTRL_ADDR)


/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->7 */
#define BMI160_USER_INTR2_OUTPUT_EN__POS               (7)
#define BMI160_USER_INTR2_OUTPUT_EN__LEN               (1)
#define BMI160_USER_INTR2_OUTPUT_EN__MSK               (0x80)
#define BMI160_USER_INTR2_OUTPUT_EN__REG    \
(BMI160_USER_INTR_OUT_CTRL_ADDR)

/**************************************************************/
/**\name  LEVEL CONTROL ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->5 */
#define BMI160_USER_INTR2_LEVEL__POS               (5)
#define BMI160_USER_INTR2_LEVEL__LEN               (1)
#define BMI160_USER_INTR2_LEVEL__MSK               (0x20)
#define BMI160_USER_INTR2_LEVEL__REG               \
(BMI160_USER_INTR_OUT_CTRL_ADDR)

/**************************************************************/
/**\name  OUTPUT TYPE ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Out_Ctrl Description - Reg Addr --> 0x53, Bit -->6 */
#define BMI160_USER_INTR2_OUTPUT_TYPE__POS               (6)
#define BMI160_USER_INTR2_OUTPUT_TYPE__LEN               (1)
#define BMI160_USER_INTR2_OUTPUT_TYPE__MSK               (0x40)
#define BMI160_USER_INTR2_OUTPUT_TYPE__REG               \
(BMI160_USER_INTR_OUT_CTRL_ADDR)


#define BMI160_TEMP_DATA_SIZE       (2)

#define BMI160_SENSOR_TIME_DATA_SIZE      (3)

/****************************************************/
/**\name  ARRAY PARAMETER DEFINITIONS      */
/***************************************************/
#define BMI160_SENSOR_TIME_MSB_BYTE   (2)
#define BMI160_SENSOR_TIME_XLSB_BYTE  (1)
#define BMI160_SENSOR_TIME_LSB_BYTE   (0)


#define BMI160_GYRO_X_LSB_BYTE              (0)
#define BMI160_GYRO_X_MSB_BYTE              (1)
#define BMI160_GYRO_Y_LSB_BYTE              (0)
#define BMI160_GYRO_Y_MSB_BYTE              (1)
#define BMI160_GYRO_Z_LSB_BYTE              (0)
#define BMI160_GYRO_Z_MSB_BYTE              (1)


#define BMI160_ACCEL_X_LSB_BYTE              (0)
#define BMI160_ACCEL_X_MSB_BYTE              (1)
#define BMI160_ACCEL_Y_LSB_BYTE              (0)
#define BMI160_ACCEL_Y_MSB_BYTE              (1)
#define BMI160_ACCEL_Z_LSB_BYTE              (0)
#define BMI160_ACCEL_Z_MSB_BYTE              (1)

#define BMI160_TEMP_LSB_BYTE    (0)
#define BMI160_TEMP_MSB_BYTE    (1)

/* Constants */
#define BMI160_NULL           (0)
#define BMI160_DELAY_SETTLING_TIME    (5)

/**************************************************/
/**\name  FIFO CONFIGURATIONS    */
/*************************************************/
#define FIFO_HEADER_ENABLE      (0x01)
#define FIFO_MAG_ENABLE       (0x01)
#define FIFO_ACCEL_ENABLE     (0x01)
#define FIFO_GYRO_ENABLE      (0x01)
#define FIFO_TIME_ENABLE      (0x01)
#define FIFO_STOPONFULL_ENABLE    (0x01)
#define FIFO_WM_INTERRUPT_ENABLE  (0x01)
#define BMI160_FIFO_INDEX_LENGTH  (1)
#define BMI160_FIFO_TAG_INTR_MASK (0xFC)

/***************************************************/
/**\name REGISTER FOR GYRO AND ACCEL DOWNSAMPLING RATES FOR FIFO*/
/******************************************************/
#define BMI160_USER_FIFO_DOWN_ADDR              (0X45)
/***************************************************/
/**\name FIFO CONFIG REGISTERS*/
/******************************************************/
#define BMI160_USER_FIFO_CONFIG_0_ADDR          (0X46)
#define BMI160_USER_FIFO_CONFIG_1_ADDR          (0X47)
#define BMI160_TEMP_LSB_BYTE    (0)
#define BMI160_TEMP_MSB_BYTE    (1)

#define BMI160_FIFO_LENGTH_LSB_BYTE    (0)
#define BMI160_FIFO_LENGTH_MSB_BYTE    (1)

/**************************************************************/
/**\name  FIFO BYTE COUNTER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Length0 Description - Reg Addr --> 0x22, Bit --> 0...7 */
#define BMI160_USER_FIFO_BYTE_COUNTER_LSB__POS           (0)
#define BMI160_USER_FIFO_BYTE_COUNTER_LSB__LEN           (8)
#define BMI160_USER_FIFO_BYTE_COUNTER_LSB__MSK          (0xFF)
#define BMI160_USER_FIFO_BYTE_COUNTER_LSB__REG          \
    (BMI160_USER_FIFO_LENGTH_0_ADDR)

/*Fifo_Length1 Description - Reg Addr --> 0x23, Bit --> 0...2 */
#define BMI160_USER_FIFO_BYTE_COUNTER_MSB__POS           (0)
#define BMI160_USER_FIFO_BYTE_COUNTER_MSB__LEN           3
#define BMI160_USER_FIFO_BYTE_COUNTER_MSB__MSK          (0x07)
#define BMI160_USER_FIFO_BYTE_COUNTER_MSB__REG          \
    (BMI160_USER_FIFO_LENGTH_1_ADDR)

/**************************************************************/
/**\name  FIFO TIME LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 1 */
#define BMI160_USER_FIFO_TIME_ENABLE__POS               (1)
#define BMI160_USER_FIFO_TIME_ENABLE__LEN               (1)
#define BMI160_USER_FIFO_TIME_ENABLE__MSK               (0x02)
#define BMI160_USER_FIFO_TIME_ENABLE__REG (BMI160_USER_FIFO_CONFIG_1_ADDR)

/**************************************************************/
/**\name  FIFO ACCEL ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 6 */
#define BMI160_USER_FIFO_ACCEL_ENABLE__POS               (6)
#define BMI160_USER_FIFO_ACCEL_ENABLE__LEN               (1)
#define BMI160_USER_FIFO_ACCEL_ENABLE__MSK               (0x40)
#define BMI160_USER_FIFO_ACCEL_ENABLE__REG            \
(BMI160_USER_FIFO_CONFIG_1_ADDR)
/**************************************************************/
/**\name  FIFO GYRO ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 7 */
#define BMI160_USER_FIFO_GYRO_ENABLE__POS               (7)
#define BMI160_USER_FIFO_GYRO_ENABLE__LEN               (1)
#define BMI160_USER_FIFO_GYRO_ENABLE__MSK               (0x80)
#define BMI160_USER_FIFO_GYRO_ENABLE__REG          \
(BMI160_USER_FIFO_CONFIG_1_ADDR)

/**************************************************************/
/**\name  FIFO WATER MARK LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_0 Description - Reg Addr --> 0x46, Bit --> 0...7 */
#define BMI160_USER_FIFO_WM__POS               (0)
#define BMI160_USER_FIFO_WM__LEN               (8)
#define BMI160_USER_FIFO_WM__MSK               (0xFF)
#define BMI160_USER_FIFO_WM__REG  (BMI160_USER_FIFO_CONFIG_0_ADDR)
/**************************************************************/
/**\name  FIFO TIME LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 1 */
#define BMI160_USER_FIFO_TIME_ENABLE__POS               (1)
#define BMI160_USER_FIFO_TIME_ENABLE__LEN               (1)
#define BMI160_USER_FIFO_TIME_ENABLE__MSK               (0x02)
#define BMI160_USER_FIFO_TIME_ENABLE__REG (BMI160_USER_FIFO_CONFIG_1_ADDR)

/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->2 */
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__POS               (2)
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__LEN               (1)
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__MSK               (0x04)
#define BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__REG          \
(BMI160_USER_INTR_MAP_1_ADDR)


/**************************************************************/
/**\name  FIFO HEADER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Config_1 Description - Reg Addr --> 0x47, Bit --> 4 */
#define BMI160_USER_FIFO_HEADER_ENABLE__POS               (4)
#define BMI160_USER_FIFO_HEADER_ENABLE__LEN               (1)
#define BMI160_USER_FIFO_HEADER_ENABLE__MSK               (0x10)
#define BMI160_USER_FIFO_HEADER_ENABLE__REG            \
(BMI160_USER_FIFO_CONFIG_1_ADDR)

/* Int_Map_1 Description - Reg Addr --> 0x56, Bit -->6 */
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__POS               (6)
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__LEN               (1)
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__MSK               (0x40)
#define BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__REG \
(BMI160_USER_INTR_MAP_1_ADDR)


/* FIFO definitions*/
#define FIFO_HEAD_A        (0x84)
#define FIFO_HEAD_G        (0x88)
#define FIFO_HEAD_M        (0x90)

#define FIFO_HEAD_G_A (0x8C)
#define FIFO_HEAD_M_A   (0x94)
#define FIFO_HEAD_M_G   (0x98)

#define FIFO_HEAD_M_G_A   (0x9C)

#define FIFO_HEAD_SENSOR_TIME     (0x44)
#define FIFO_HEAD_SKIP_FRAME      (0x40)
#define FIFO_HEAD_OVER_READ_LSB     (0x80)
#define FIFO_HEAD_OVER_READ_MSB     (0x00)


#define FIFO_OVER_READ_RETURN   ((s8)-10)
#define FIFO_SENSORTIME_RETURN    ((s8)-9)
#define FIFO_SKIP_OVER_LEN      ((s8)-8)
#define FIFO_M_G_A_OVER_LEN     ((s8)-7)
#define FIFO_M_G_OVER_LEN     ((s8)-6)
#define FIFO_M_A_OVER_LEN     ((s8)-5)
#define FIFO_G_A_OVER_LEN     ((s8)-4)
#define FIFO_M_OVER_LEN       ((s8)-3)
#define FIFO_G_OVER_LEN       ((s8)-2)
#define FIFO_A_OVER_LEN       ((s8)-1)

/*******************/
/**\name FIFO REGISTERS */
/*******************/
#define BMI160_USER_FIFO_LENGTH_0_ADDR      (0X22)
#define BMI160_USER_FIFO_LENGTH_1_ADDR      (0X23)
#define BMI160_USER_FIFO_DATA_ADDR        (0X24)


/**************************************************************/
/**\name  FIFO DATA LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Fifo_Data Description - Reg Addr --> 0x24, Bit --> 0...7 */
#define BMI160_USER_FIFO_DATA__POS           (0)
#define BMI160_USER_FIFO_DATA__LEN           (8)
#define BMI160_USER_FIFO_DATA__MSK          (0xFF)
#define BMI160_USER_FIFO_DATA__REG          (BMI160_USER_FIFO_DATA_ADDR)

/**************************************************************/
/**\name  LATCH INTERRUPT LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Int_Latch Description - Reg Addr --> 0x54, Bit -->0...3 */
#define BMI160_USER_INTR_LATCH__POS               (0)
#define BMI160_USER_INTR_LATCH__LEN               (4)
#define BMI160_USER_INTR_LATCH__MSK               (0x0F)
#define BMI160_USER_INTR_LATCH__REG               (0x54)

/* FIFO length definitions*/
#define BMI160_FIFO_SENSOR_TIME_LSB     (0)
#define BMI160_FIFO_SENSOR_TIME_XLSB    (1)
#define BMI160_FIFO_SENSOR_TIME_MSB     (2)
#define BMI160_FIFO_SENSOR_TIME_LENGTH  (3)
#define BMI160_FIFO_A_LENGTH            (6)
#define BMI160_FIFO_G_LENGTH            (6)
#define BMI160_FIFO_M_LENGTH            (8)
#define BMI160_FIFO_AG_LENGTH           (12)
#define BMI160_FIFO_AMG_LENGTH          (20)
#define BMI160_FIFO_MA_OR_MG_LENGTH     (14)

/* FIFO index definitions*/
#define BMI160_FIFO_X_LSB_DATA      (0)
#define BMI160_FIFO_X_MSB_DATA      (1)
#define BMI160_FIFO_Y_LSB_DATA      (2)
#define BMI160_FIFO_Y_MSB_DATA      (3)
#define BMI160_FIFO_Z_LSB_DATA      (4)
#define BMI160_FIFO_Z_MSB_DATA      (5)
#define BMI160_FIFO_R_LSB_DATA      (6)
#define BMI160_FIFO_R_MSB_DATA      (7)
/* FIFO gyro definition*/
#define BMI160_GA_FIFO_G_X_LSB    (0)
#define BMI160_GA_FIFO_G_X_MSB    (1)
#define BMI160_GA_FIFO_G_Y_LSB    (2)
#define BMI160_GA_FIFO_G_Y_MSB    (3)
#define BMI160_GA_FIFO_G_Z_LSB    (4)
#define BMI160_GA_FIFO_G_Z_MSB    (5)
#define BMI160_GA_FIFO_A_X_LSB    (6)
#define BMI160_GA_FIFO_A_X_MSB    (7)
#define BMI160_GA_FIFO_A_Y_LSB    (8)
#define BMI160_GA_FIFO_A_Y_MSB    (9)
#define BMI160_GA_FIFO_A_Z_LSB    (10)
#define BMI160_GA_FIFO_A_Z_MSB    (11)
/* FIFO mag/gyro/accel definition*/
#define BMI160_MGA_FIFO_M_X_LSB   (0)
#define BMI160_MGA_FIFO_M_X_MSB   (1)
#define BMI160_MGA_FIFO_M_Y_LSB   (2)
#define BMI160_MGA_FIFO_M_Y_MSB   (3)
#define BMI160_MGA_FIFO_M_Z_LSB   (4)
#define BMI160_MGA_FIFO_M_Z_MSB   (5)
#define BMI160_MGA_FIFO_M_R_LSB   (6)
#define BMI160_MGA_FIFO_M_R_MSB   (7)
#define BMI160_MGA_FIFO_G_X_LSB   (8)
#define BMI160_MGA_FIFO_G_X_MSB   (9)
#define BMI160_MGA_FIFO_G_Y_LSB   (10)
#define BMI160_MGA_FIFO_G_Y_MSB   (11)
#define BMI160_MGA_FIFO_G_Z_LSB   (12)
#define BMI160_MGA_FIFO_G_Z_MSB   (13)
#define BMI160_MGA_FIFO_A_X_LSB   (14)
#define BMI160_MGA_FIFO_A_X_MSB   (15)
#define BMI160_MGA_FIFO_A_Y_LSB   (16)
#define BMI160_MGA_FIFO_A_Y_MSB   (17)
#define BMI160_MGA_FIFO_A_Z_LSB   (18)
#define BMI160_MGA_FIFO_A_Z_MSB   (19)
/* FIFO mag definition*/
#define BMI160_MA_FIFO_M_X_LSB    (0)
#define BMI160_MA_FIFO_M_X_MSB    (1)
#define BMI160_MA_FIFO_M_Y_LSB    (2)
#define BMI160_MA_FIFO_M_Y_MSB    (3)
#define BMI160_MA_FIFO_M_Z_LSB    (4)
#define BMI160_MA_FIFO_M_Z_MSB    (5)
#define BMI160_MA_FIFO_M_R_LSB    (6)
#define BMI160_MA_FIFO_M_R_MSB    (7)
#define BMI160_MA_FIFO_A_X_LSB    (8)
#define BMI160_MA_FIFO_A_X_MSB    (9)
#define BMI160_MA_FIFO_A_Y_LSB    (10)
#define BMI160_MA_FIFO_A_Y_MSB    (11)
#define BMI160_MA_FIFO_A_Z_LSB    (12)
#define BMI160_MA_FIFO_A_Z_MSB    (13)
/* FIFO mag/gyro definition*/
#define BMI160_MG_FIFO_M_X_LSB    (0)
#define BMI160_MG_FIFO_M_X_MSB    (1)
#define BMI160_MG_FIFO_M_Y_LSB    (2)
#define BMI160_MG_FIFO_M_Y_MSB    (3)
#define BMI160_MG_FIFO_M_Z_LSB    (4)
#define BMI160_MG_FIFO_M_Z_MSB    (5)
#define BMI160_MG_FIFO_M_R_LSB    (6)
#define BMI160_MG_FIFO_M_R_MSB    (7)
#define BMI160_MG_FIFO_G_X_LSB    (8)
#define BMI160_MG_FIFO_G_X_MSB    (9)
#define BMI160_MG_FIFO_G_Y_LSB    (10)
#define BMI160_MG_FIFO_G_Y_MSB    (11)
#define BMI160_MG_FIFO_G_Z_LSB    (12)
#define BMI160_MG_FIFO_G_Z_MSB    (13)
/* FIFO length definitions*/
#define BMI160_FIFO_SENSOR_TIME_LSB     (0)
#define BMI160_FIFO_SENSOR_TIME_XLSB    (1)
#define BMI160_FIFO_SENSOR_TIME_MSB     (2)
#define BMI160_FIFO_SENSOR_TIME_LENGTH  (3)
#define BMI160_FIFO_A_LENGTH            (6)
#define BMI160_FIFO_G_LENGTH            (6)
#define BMI160_FIFO_M_LENGTH            (8)
#define BMI160_FIFO_AG_LENGTH           (12)
#define BMI160_FIFO_AMG_LENGTH          (20)
#define BMI160_FIFO_MA_OR_MG_LENGTH     (14)
#define BMI160_FIFO_WATER_MARK_UNIT (4)

#define WATER_A_OR_G_MARK_SWITH(x)  ((x* BMI160_FIFO_A_LENGTH+1)/ BMI160_FIFO_WATER_MARK_UNIT)
#define WATER_AG_MARK_SWITH(x)  ((x* BMI160_FIFO_AG_LENGTH+1) / BMI160_FIFO_WATER_MARK_UNIT)
#define ngx_align(d, a)     ((d) & ~((a) - 1))

/****************************************************/
/**\name  REGISTER DEFINITIONS       */
/***************************************************/
/*******************/
/**\name CHIP ID */
/*******************/
#define BMI160_USER_CHIP_ID_ADDR        (0x00)
/*******************/
/**\name ERROR STATUS */
/*******************/
#define BMI160_USER_ERROR_ADDR          (0X02)
/*******************/
/**\name POWER MODE STATUS */
/*******************/
#define BMI160_USER_PMU_STAT_ADDR       (0X03)
/*******************/
/**\name MAG DATA REGISTERS */
/*******************/
#define BMI160_USER_DATA_0_ADDR         (0X04)
#define BMI160_USER_DATA_1_ADDR         (0X05)
#define BMI160_USER_DATA_2_ADDR         (0X06)
#define BMI160_USER_DATA_3_ADDR         (0X07)
#define BMI160_USER_DATA_4_ADDR         (0X08)
#define BMI160_USER_DATA_5_ADDR         (0X09)
#define BMI160_USER_DATA_6_ADDR         (0X0A)
#define BMI160_USER_DATA_7_ADDR         (0X0B)
/*******************/
/**\name GYRO DATA REGISTERS */
/*******************/
#define BMI160_USER_DATA_8_ADDR         (0X0C)
#define BMI160_USER_DATA_9_ADDR         (0X0D)
#define BMI160_USER_DATA_10_ADDR        (0X0E)
#define BMI160_USER_DATA_11_ADDR        (0X0F)
#define BMI160_USER_DATA_12_ADDR        (0X10)
#define BMI160_USER_DATA_13_ADDR        (0X11)
#define BMI160_USER_DATA_14_ADDR        (0X12)
#define BMI160_USER_DATA_15_ADDR        (0X13)
/*******************/
/**\name ACCEL DATA REGISTERS */
/*******************/
#define BMI160_USER_DATA_16_ADDR        (0X14)
#define BMI160_USER_DATA_17_ADDR        (0X15)
#define BMI160_USER_DATA_18_ADDR        (0X16)
#define BMI160_USER_DATA_19_ADDR        (0X17)
/*******************/
/**\name SENSOR TIME REGISTERS */
/*******************/
#define BMI160_USER_SENSORTIME_0_ADDR     (0X18)
#define BMI160_USER_SENSORTIME_1_ADDR     (0X19)
#define BMI160_USER_SENSORTIME_2_ADDR     (0X1A)
/*******************/
/**\name STATUS REGISTER FOR SENSOR STATUS FLAG */
/*******************/
#define BMI160_USER_STAT_ADDR         (0X1B)
/*******************/
/**\name TEMPERATURE REGISTERS */
/*******************/
#define BMI160_USER_TEMPERATURE_0_ADDR      (0X20)
#define BMI160_USER_TEMPERATURE_1_ADDR      (0X21)

/***************************************************/
/**\name ACCEL CONFIG REGISTERS  FOR ODR, BANDWIDTH AND UNDERSAMPLING*/
/******************************************************/
#define BMI160_USER_ACCEL_CONFIG_ADDR     (0X40)
/*******************/
/**\name ACCEL RANGE */
/*******************/
#define BMI160_USER_ACCEL_RANGE_ADDR            (0X41)
/***************************************************/
/**\name GYRO CONFIG REGISTERS  FOR ODR AND BANDWIDTH */
/******************************************************/
#define BMI160_USER_GYRO_CONFIG_ADDR            (0X42)
/*******************/
/**\name GYRO RANGE */
/*******************/
#define BMI160_USER_GYRO_RANGE_ADDR             (0X43)
/***************************************************/
/**\name FAST OFFSET CONFIGURATION REGISTER*/
/******************************************************/
#define BMI160_USER_FOC_CONFIG_ADDR       (0X69)
/***************************************************/
/**\name MISCELLANEOUS CONFIGURATION REGISTER*/
/******************************************************/
#define BMI160_USER_CONFIG_ADDR         (0X6A)
/***************************************************/
/**\name SERIAL INTERFACE SETTINGS REGISTER*/
/******************************************************/
#define BMI160_USER_IF_CONFIG_ADDR        (0X6B)
/***************************************************/
/**\name GYRO POWER MODE TRIGGER REGISTER */
/******************************************************/
#define BMI160_USER_PMU_TRIGGER_ADDR      (0X6C)
/***************************************************/
/**\name SELF_TEST REGISTER*/
/******************************************************/
#define BMI160_USER_SELF_TEST_ADDR        (0X6D)
/***************************************************/
/**\name SPI,I2C SELECTION REGISTER*/
/******************************************************/
#define BMI160_USER_NV_CONFIG_ADDR        (0x70)
/***************************************************/
/**\name ACCEL AND GYRO OFFSET REGISTERS*/
/******************************************************/
#define BMI160_USER_OFFSET_0_ADDR       (0X71)
#define BMI160_USER_OFFSET_1_ADDR       (0X72)
#define BMI160_USER_OFFSET_2_ADDR       (0X73)
#define BMI160_USER_OFFSET_3_ADDR       (0X74)
#define BMI160_USER_OFFSET_4_ADDR       (0X75)
#define BMI160_USER_OFFSET_5_ADDR       (0X76)
#define BMI160_USER_OFFSET_6_ADDR       (0X77)

/***************************************************/
/**\name COMMAND REGISTER*/
/******************************************************/
#define BMI160_CMD_COMMANDS_ADDR        (0X7E)
/***************************************************/
/**\name PAGE REGISTERS*/
/******************************************************/
#define BMI160_CMD_EXT_MODE_ADDR        (0X7F)
#define BMI160_COM_C_TRIM_FIVE_ADDR       (0X05)

/****************************************************/
/**\name  SHIFT VALUE DEFINITION       */
/***************************************************/
#define BMI160_SHIFT_BIT_POSITION_BY_01_BIT      (1)
#define BMI160_SHIFT_BIT_POSITION_BY_02_BITS     (2)
#define BMI160_SHIFT_BIT_POSITION_BY_03_BITS     (3)
#define BMI160_SHIFT_BIT_POSITION_BY_04_BITS     (4)
#define BMI160_SHIFT_BIT_POSITION_BY_05_BITS     (5)
#define BMI160_SHIFT_BIT_POSITION_BY_06_BITS     (6)
#define BMI160_SHIFT_BIT_POSITION_BY_07_BITS     (7)
#define BMI160_SHIFT_BIT_POSITION_BY_08_BITS     (8)
#define BMI160_SHIFT_BIT_POSITION_BY_09_BITS     (9)
#define BMI160_SHIFT_BIT_POSITION_BY_12_BITS     (12)
#define BMI160_SHIFT_BIT_POSITION_BY_13_BITS     (13)
#define BMI160_SHIFT_BIT_POSITION_BY_14_BITS     (14)
#define BMI160_SHIFT_BIT_POSITION_BY_15_BITS     (15)
#define BMI160_SHIFT_BIT_POSITION_BY_16_BITS     (16)

/****************************************************/
/**\name  YAMAHA-YAS532 OFFSET DEFINITION */
/***************************************************/
static const int8_t INVALID_OFFSET[] = {0x7f, 0x7f, 0x7f};
#define set_vector(to, from) \
  {int _l; for (_l = 0; _l < 3; _l++) (to)[_l] = (from)[_l]; }
#define is_valid_offset(a) \
  (((a)[0] <= 31) && ((a)[1] <= 31) && ((a)[2] <= 31) \
    && (-31 <= (a)[0]) && (-31 <= (a)[1]) && (-31 <= (a)[2]))

/**************************************************************/
/**\name  CHIP ID LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Chip ID Description - Reg Addr --> (0x00), Bit --> 0...7 */
#define BMI160_CHIP_ID                                      (0xD1)
#define BMI160_USER_CHIP_ID__POS             (0)
#define BMI160_USER_CHIP_ID__MSK            (0xFF)
#define BMI160_USER_CHIP_ID__LEN             (8)
#define BMI160_USER_CHIP_ID__REG             (BMI160_USER_CHIP_ID_ADDR)
/**************************************************************/
/**\name  ERROR STATUS LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Error Description - Reg Addr --> (0x02), Bit --> 0 */
#define BMI160_USER_ERR_STAT__POS               (0)
#define BMI160_USER_ERR_STAT__LEN               (8)
#define BMI160_USER_ERR_STAT__MSK               (0xFF)
#define BMI160_USER_ERR_STAT__REG               (BMI160_USER_ERROR_ADDR)

#define BMI160_USER_FATAL_ERR__POS               (0)
#define BMI160_USER_FATAL_ERR__LEN               (1)
#define BMI160_USER_FATAL_ERR__MSK               (0x01)
#define BMI160_USER_FATAL_ERR__REG               (BMI160_USER_ERROR_ADDR)

/* Error Description - Reg Addr --> (0x02), Bit --> 1...4 */
#define BMI160_USER_ERR_CODE__POS               (1)
#define BMI160_USER_ERR_CODE__LEN               (4)
#define BMI160_USER_ERR_CODE__MSK               (0x1E)
#define BMI160_USER_ERR_CODE__REG               (BMI160_USER_ERROR_ADDR)

/* Error Description - Reg Addr --> (0x02), Bit --> 5 */
#define BMI160_USER_I2C_FAIL_ERR__POS               (5)
#define BMI160_USER_I2C_FAIL_ERR__LEN               (1)
#define BMI160_USER_I2C_FAIL_ERR__MSK               (0x20)
#define BMI160_USER_I2C_FAIL_ERR__REG               (BMI160_USER_ERROR_ADDR)

/* Error Description - Reg Addr --> (0x02), Bit --> 6 */
#define BMI160_USER_DROP_CMD_ERR__POS              (6)
#define BMI160_USER_DROP_CMD_ERR__LEN              (1)
#define BMI160_USER_DROP_CMD_ERR__MSK              (0x40)
#define BMI160_USER_DROP_CMD_ERR__REG              (BMI160_USER_ERROR_ADDR)

/**************************************************************/
/**\name  GYRO POWER MODE LENGTH, POSITION AND MASK    */
/**************************************************************/
/* PMU_Status Description of GYRO - Reg Addr --> (0x03), Bit --> 3...2 */
#define BMI160_USER_GYRO_POWER_MODE_STAT__POS               (2)
#define BMI160_USER_GYRO_POWER_MODE_STAT__LEN               (2)
#define BMI160_USER_GYRO_POWER_MODE_STAT__MSK               (0x0C)
#define BMI160_USER_GYRO_POWER_MODE_STAT__REG         \
(BMI160_USER_PMU_STAT_ADDR)
/**************************************************************/
/**\name  ACCEL POWER MODE LENGTH, POSITION AND MASK    */
/**************************************************************/
/* PMU_Status Description of ACCEL - Reg Addr --> (0x03), Bit --> 5...4 */
#define BMI160_USER_ACCEL_POWER_MODE_STAT__POS               (4)
#define BMI160_USER_ACCEL_POWER_MODE_STAT__LEN               (2)
#define BMI160_USER_ACCEL_POWER_MODE_STAT__MSK               (0x30)
#define BMI160_USER_ACCEL_POWER_MODE_STAT__REG        \
(BMI160_USER_PMU_STAT_ADDR)
/**************************************************************/
/**\name  MAG DATA XYZ LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Mag_X(LSB) Description - Reg Addr --> (0x04), Bit --> 0...7 */
#define BMI160_USER_DATA_0_MAG_X_LSB__POS           (0)
#define BMI160_USER_DATA_0_MAG_X_LSB__LEN           (8)
#define BMI160_USER_DATA_0_MAG_X_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_0_MAG_X_LSB__REG          (BMI160_USER_DATA_0_ADDR)

/* Mag_X(LSB) Description - Reg Addr --> (0x04), Bit --> 3...7 */
#define BMI160_USER_DATA_MAG_X_LSB__POS           (3)
#define BMI160_USER_DATA_MAG_X_LSB__LEN           (5)
#define BMI160_USER_DATA_MAG_X_LSB__MSK          (0xF8)
#define BMI160_USER_DATA_MAG_X_LSB__REG          (BMI160_USER_DATA_0_ADDR)

/* Mag_X(MSB) Description - Reg Addr --> (0x05), Bit --> 0...7 */
#define BMI160_USER_DATA_1_MAG_X_MSB__POS           (0)
#define BMI160_USER_DATA_1_MAG_X_MSB__LEN           (8)
#define BMI160_USER_DATA_1_MAG_X_MSB__MSK          (0xFF)
#define BMI160_USER_DATA_1_MAG_X_MSB__REG          (BMI160_USER_DATA_1_ADDR)

/* Mag_Y(LSB) Description - Reg Addr --> (0x06), Bit --> 0...7 */
#define BMI160_USER_DATA_2_MAG_Y_LSB__POS           (0)
#define BMI160_USER_DATA_2_MAG_Y_LSB__LEN           (8)
#define BMI160_USER_DATA_2_MAG_Y_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_2_MAG_Y_LSB__REG          (BMI160_USER_DATA_2_ADDR)


/* RHALL(LSB) Description - Reg Addr --> (0x0A), Bit --> 0...7 */
#define BMI160_USER_DATA_6_RHALL_LSB__POS           (0)
#define BMI160_USER_DATA_6_RHALL_LSB__LEN           (8)
#define BMI160_USER_DATA_6_RHALL_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_6_RHALL_LSB__REG          (BMI160_USER_DATA_6_ADDR)

/* RHALL(MSB) Description - Reg Addr --> (0x0B), Bit --> 0...7 */
#define BMI160_USER_DATA_7_RHALL_MSB__POS           (0)
#define BMI160_USER_DATA_7_RHALL_MSB__LEN           (8)
#define BMI160_USER_DATA_7_RHALL_MSB__MSK          (0xFF)
#define BMI160_USER_DATA_7_RHALL_MSB__REG          (BMI160_USER_DATA_7_ADDR)
/**************************************************************/
/**\name  GYRO DATA XYZ LENGTH, POSITION AND MASK    */
/**************************************************************/
/* GYR_X (LSB) Description - Reg Addr --> (0x0C), Bit --> 0...7 */
#define BMI160_USER_DATA_8_GYRO_X_LSB__POS           (0)
#define BMI160_USER_DATA_8_GYRO_X_LSB__LEN           (8)
#define BMI160_USER_DATA_8_GYRO_X_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_8_GYRO_X_LSB__REG          (BMI160_USER_DATA_8_ADDR)

/* GYR_X (MSB) Description - Reg Addr --> (0x0D), Bit --> 0...7 */
#define BMI160_USER_DATA_9_GYRO_X_MSB__POS           (0)
#define BMI160_USER_DATA_9_GYRO_X_MSB__LEN           (8)
#define BMI160_USER_DATA_9_GYRO_X_MSB__MSK          (0xFF)
#define BMI160_USER_DATA_9_GYRO_X_MSB__REG          (BMI160_USER_DATA_9_ADDR)

/* GYR_Y (LSB) Description - Reg Addr --> 0x0E, Bit --> 0...7 */
#define BMI160_USER_DATA_10_GYRO_Y_LSB__POS           (0)
#define BMI160_USER_DATA_10_GYRO_Y_LSB__LEN           (8)
#define BMI160_USER_DATA_10_GYRO_Y_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_10_GYRO_Y_LSB__REG          (BMI160_USER_DATA_10_ADDR)

/* GYR_Y (MSB) Description - Reg Addr --> (0x0F), Bit --> 0...7 */
#define BMI160_USER_DATA_11_GYRO_Y_MSB__POS           (0)
#define BMI160_USER_DATA_11_GYRO_Y_MSB__LEN           (8)
#define BMI160_USER_DATA_11_GYRO_Y_MSB__MSK          (0xFF)
#define BMI160_USER_DATA_11_GYRO_Y_MSB__REG          (BMI160_USER_DATA_11_ADDR)

/* GYR_Z (LSB) Description - Reg Addr --> (0x10), Bit --> 0...7 */
#define BMI160_USER_DATA_12_GYRO_Z_LSB__POS           (0)
#define BMI160_USER_DATA_12_GYRO_Z_LSB__LEN           (8)
#define BMI160_USER_DATA_12_GYRO_Z_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_12_GYRO_Z_LSB__REG          (BMI160_USER_DATA_12_ADDR)

/* GYR_Z (MSB) Description - Reg Addr --> (0x11), Bit --> 0...7 */
#define BMI160_USER_DATA_13_GYRO_Z_MSB__POS           (0)
#define BMI160_USER_DATA_13_GYRO_Z_MSB__LEN           (8)
#define BMI160_USER_DATA_13_GYRO_Z_MSB__MSK          (0xFF)
#define BMI160_USER_DATA_13_GYRO_Z_MSB__REG          (BMI160_USER_DATA_13_ADDR)
/**************************************************************/
/**\name  ACCEL DATA XYZ LENGTH, POSITION AND MASK    */
/**************************************************************/
/* ACC_X (LSB) Description - Reg Addr --> (0x12), Bit --> 0...7 */
#define BMI160_USER_DATA_14_ACCEL_X_LSB__POS           (0)
#define BMI160_USER_DATA_14_ACCEL_X_LSB__LEN           (8)
#define BMI160_USER_DATA_14_ACCEL_X_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_14_ACCEL_X_LSB__REG          (BMI160_USER_DATA_14_ADDR)

/* ACC_X (MSB) Description - Reg Addr --> 0x13, Bit --> 0...7 */
#define BMI160_USER_DATA_15_ACCEL_X_MSB__POS           (0)
#define BMI160_USER_DATA_15_ACCEL_X_MSB__LEN           (8)
#define BMI160_USER_DATA_15_ACCEL_X_MSB__MSK          (0xFF)
#define BMI160_USER_DATA_15_ACCEL_X_MSB__REG          (BMI160_USER_DATA_15_ADDR)

/* ACC_Y (LSB) Description - Reg Addr --> (0x14), Bit --> 0...7 */
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__POS           (0)
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__LEN           (8)
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_16_ACCEL_Y_LSB__REG          (BMI160_USER_DATA_16_ADDR)

/* ACC_Y (MSB) Description - Reg Addr --> (0x15), Bit --> 0...7 */
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__POS           (0)
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__LEN           (8)
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__MSK          (0xFF)
#define BMI160_USER_DATA_17_ACCEL_Y_MSB__REG          (BMI160_USER_DATA_17_ADDR)

/* ACC_Z (LSB) Description - Reg Addr --> 0x16, Bit --> 0...7 */
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__POS           (0)
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__LEN           (8)
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__MSK          (0xFF)
#define BMI160_USER_DATA_18_ACCEL_Z_LSB__REG          (BMI160_USER_DATA_18_ADDR)

/* ACC_Z (MSB) Description - Reg Addr --> (0x17), Bit --> 0...7 */
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__POS           (0)
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__LEN           (8)
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__MSK          (0xFF)
#define BMI160_USER_DATA_19_ACCEL_Z_MSB__REG          (BMI160_USER_DATA_19_ADDR)
/**************************************************************/
/**\name  SENSOR TIME LENGTH, POSITION AND MASK    */
/**************************************************************/
/* SENSORTIME_0 (LSB) Description - Reg Addr --> (0x18), Bit --> 0...7 */
#define BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__POS           (0)
#define BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__LEN           (8)
#define BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__MSK          (0xFF)
#define BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__REG          \
    (BMI160_USER_SENSORTIME_0_ADDR)

/* SENSORTIME_1 (MSB) Description - Reg Addr --> (0x19), Bit --> 0...7 */
#define BMI160_USER_SENSORTIME_1_SENSOR_TIME_MSB__POS           (0)
#define BMI160_USER_SENSORTIME_1_SENSOR_TIME_MSB__LEN           (8)
#define BMI160_USER_SENSORTIME_1_SENSOR_TIME_MSB__MSK          (0xFF)
#define BMI160_USER_SENSORTIME_1_SENSOR_TIME_MSB__REG          \
    (BMI160_USER_SENSORTIME_1_ADDR)

/* SENSORTIME_2 (MSB) Description - Reg Addr --> (0x1A), Bit --> 0...7 */
#define BMI160_USER_SENSORTIME_2_SENSOR_TIME_MSB__POS           (0)
#define BMI160_USER_SENSORTIME_2_SENSOR_TIME_MSB__LEN           (8)
#define BMI160_USER_SENSORTIME_2_SENSOR_TIME_MSB__MSK          (0xFF)
#define BMI160_USER_SENSORTIME_2_SENSOR_TIME_MSB__REG          \
    (BMI160_USER_SENSORTIME_2_ADDR)
/**************************************************************/
/**\name  GYRO SELF TEST LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 1 */
#define BMI160_USER_STAT_GYRO_SELFTEST_OK__POS          (1)
#define BMI160_USER_STAT_GYRO_SELFTEST_OK__LEN          (1)
#define BMI160_USER_STAT_GYRO_SELFTEST_OK__MSK          (0x02)
#define BMI160_USER_STAT_GYRO_SELFTEST_OK__REG         \
    (BMI160_USER_STAT_ADDR)

/**************************************************************/
/**\name  FOC STATUS LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 3 */
#define BMI160_USER_STAT_FOC_RDY__POS          (3)
#define BMI160_USER_STAT_FOC_RDY__LEN          (1)
#define BMI160_USER_STAT_FOC_RDY__MSK          (0x08)
#define BMI160_USER_STAT_FOC_RDY__REG          (BMI160_USER_STAT_ADDR)
/**************************************************************/
/**\name  NVM READY LENGTH, POSITION AND MASK    */
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 4 */
#define BMI160_USER_STAT_NVM_RDY__POS           (4)
#define BMI160_USER_STAT_NVM_RDY__LEN           (1)
#define BMI160_USER_STAT_NVM_RDY__MSK           (0x10)
#define BMI160_USER_STAT_NVM_RDY__REG           (BMI160_USER_STAT_ADDR)
/**************************************************************/
/**\name  DATA READY LENGTH, POSITION AND MASK FOR ACCEL, MAG AND GYRO*/
/**************************************************************/
/* Status Description - Reg Addr --> 0x1B, Bit --> 5 */
#define BMI160_USER_STAT_DATA_RDY_MAG__POS           (5)
#define BMI160_USER_STAT_DATA_RDY_MAG__LEN           (1)
#define BMI160_USER_STAT_DATA_RDY_MAG__MSK           (0x20)
#define BMI160_USER_STAT_DATA_RDY_MAG__REG           (BMI160_USER_STAT_ADDR)

/* Status Description - Reg Addr --> 0x1B, Bit --> 6 */
#define BMI160_USER_STAT_DATA_RDY_GYRO__POS           (6)
#define BMI160_USER_STAT_DATA_RDY_GYRO__LEN           (1)
#define BMI160_USER_STAT_DATA_RDY_GYRO__MSK           (0x40)
#define BMI160_USER_STAT_DATA_RDY_GYRO__REG           (BMI160_USER_STAT_ADDR)

/* Status Description - Reg Addr --> 0x1B, Bit --> 7 */
#define BMI160_USER_STAT_DATA_RDY_ACCEL__POS           (7)
#define BMI160_USER_STAT_DATA_RDY_ACCEL__LEN           (1)
#define BMI160_USER_STAT_DATA_RDY_ACCEL__MSK           (0x80)
#define BMI160_USER_STAT_DATA_RDY_ACCEL__REG           (BMI160_USER_STAT_ADDR)



/**************************************************************/
/**\name  TEMPERATURE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Temperature Description - LSB Reg Addr --> (0x20), Bit --> 0...7 */
#define BMI160_USER_TEMP_LSB_VALUE__POS               (0)
#define BMI160_USER_TEMP_LSB_VALUE__LEN               (8)
#define BMI160_USER_TEMP_LSB_VALUE__MSK               (0xFF)
#define BMI160_USER_TEMP_LSB_VALUE__REG               \
    (BMI160_USER_TEMPERATURE_0_ADDR)

/* Temperature Description - LSB Reg Addr --> 0x21, Bit --> 0...7 */
#define BMI160_USER_TEMP_MSB_VALUE__POS               (0)
#define BMI160_USER_TEMP_MSB_VALUE__LEN               (8)
#define BMI160_USER_TEMP_MSB_VALUE__MSK               (0xFF)
#define BMI160_USER_TEMP_MSB_VALUE__REG               \
    (BMI160_USER_TEMPERATURE_1_ADDR)

/**************************************************************/
/**\name  ACCEL CONFIGURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Acc_Conf Description - Reg Addr --> (0x40), Bit --> 0...3 */
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__POS               (0)
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__LEN               (4)
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__MSK               (0x0F)
#define BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__REG           \
(BMI160_USER_ACCEL_CONFIG_ADDR)

/* Acc_Conf Description - Reg Addr --> (0x40), Bit --> 4...6 */
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__POS               (4)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__LEN               (3)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__MSK               (0x70)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_BW__REG  (BMI160_USER_ACCEL_CONFIG_ADDR)

/* Acc_Conf Description - Reg Addr --> (0x40), Bit --> 7 */
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__POS           (7)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__LEN           (1)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__MSK           (0x80)
#define BMI160_USER_ACCEL_CONFIG_ACCEL_UNDER_SAMPLING__REG  \
(BMI160_USER_ACCEL_CONFIG_ADDR)

/* Acc_Range Description - Reg Addr --> 0x41, Bit --> 0...3 */
#define BMI160_USER_ACCEL_RANGE__POS               (0)
#define BMI160_USER_ACCEL_RANGE__LEN               (4)
#define BMI160_USER_ACCEL_RANGE__MSK               (0x0F)
#define BMI160_USER_ACCEL_RANGE__REG              \
(BMI160_USER_ACCEL_RANGE_ADDR)
/**************************************************************/
/**\name  GYRO CONFIGURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Gyro_Conf Description - Reg Addr --> (0x42), Bit --> 0...3 */
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__POS               (0)
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__LEN               (4)
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__MSK               (0x0F)
#define BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__REG               \
(BMI160_USER_GYRO_CONFIG_ADDR)

/* Gyro_Conf Description - Reg Addr --> (0x42), Bit --> 4...5 */
#define BMI160_USER_GYRO_CONFIG_BW__POS               (4)
#define BMI160_USER_GYRO_CONFIG_BW__LEN               (2)
#define BMI160_USER_GYRO_CONFIG_BW__MSK               (0x30)
#define BMI160_USER_GYRO_CONFIG_BW__REG               \
(BMI160_USER_GYRO_CONFIG_ADDR)

/* Gyr_Range Description - Reg Addr --> 0x43, Bit --> 0...2 */
#define BMI160_USER_GYRO_RANGE__POS               (0)
#define BMI160_USER_GYRO_RANGE__LEN               (3)
#define BMI160_USER_GYRO_RANGE__MSK               (0x07)
#define BMI160_USER_GYRO_RANGE__REG               (BMI160_USER_GYRO_RANGE_ADDR)


/**************************************************************/
/**\name  MAG I2C ADDRESS SELECTION LENGTH, POSITION AND MASK*/
/**************************************************************/

/* Mag_IF_0 Description - Reg Addr --> 0x4b, Bit --> 1...7 */
#define BMI160_USER_I2C_DEVICE_ADDR__POS               (1)
#define BMI160_USER_I2C_DEVICE_ADDR__LEN               (7)
#define BMI160_USER_I2C_DEVICE_ADDR__MSK               (0xFE)
#define BMI160_USER_I2C_DEVICE_ADDR__REG  (BMI160_USER_MAG_IF_0_ADDR)

/**************************************************************/
/**\name  NVM PROGRAM LENGTH, POSITION AND MASK*/
/**************************************************************/
/* CONF Description - Reg Addr --> (0x6A), Bit --> 1 */
#define BMI160_USER_CONFIG_NVM_PROG_ENABLE__POS               (1)
#define BMI160_USER_CONFIG_NVM_PROG_ENABLE__LEN               (1)
#define BMI160_USER_CONFIG_NVM_PROG_ENABLE__MSK               (0x02)
#define BMI160_USER_CONFIG_NVM_PROG_ENABLE__REG               \
(BMI160_USER_CONFIG_ADDR)

/*IF_CONF Description - Reg Addr --> (0x6B), Bit --> 0 */

#define BMI160_USER_IF_CONFIG_SPI3__POS               (0)
#define BMI160_USER_IF_CONFIG_SPI3__LEN               (1)
#define BMI160_USER_IF_CONFIG_SPI3__MSK               (0x01)
#define BMI160_USER_IF_CONFIG_SPI3__REG               \
(BMI160_USER_IF_CONFIG_ADDR)

/*IF_CONF Description - Reg Addr --> (0x6B), Bit --> 5..4 */
#define BMI160_USER_IF_CONFIG_IF_MODE__POS               (4)
#define BMI160_USER_IF_CONFIG_IF_MODE__LEN               (2)
#define BMI160_USER_IF_CONFIG_IF_MODE__MSK               (0x30)
#define BMI160_USER_IF_CONFIG_IF_MODE__REG    \
(BMI160_USER_IF_CONFIG_ADDR)
/**************************************************************/
/**\name  GYRO SLEEP CONFIGURATION LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Pmu_Trigger Description - Reg Addr --> 0x6c, Bit --> 0...2 */
#define BMI160_USER_GYRO_SLEEP_TRIGGER__POS               (0)
#define BMI160_USER_GYRO_SLEEP_TRIGGER__LEN               (3)
#define BMI160_USER_GYRO_SLEEP_TRIGGER__MSK               (0x07)
#define BMI160_USER_GYRO_SLEEP_TRIGGER__REG (BMI160_USER_PMU_TRIGGER_ADDR)

/* Pmu_Trigger Description - Reg Addr --> 0x6c, Bit --> 3...4 */
#define BMI160_USER_GYRO_WAKEUP_TRIGGER__POS               (3)
#define BMI160_USER_GYRO_WAKEUP_TRIGGER__LEN               (2)
#define BMI160_USER_GYRO_WAKEUP_TRIGGER__MSK               (0x18)
#define BMI160_USER_GYRO_WAKEUP_TRIGGER__REG  (BMI160_USER_PMU_TRIGGER_ADDR)

/* Pmu_Trigger Description - Reg Addr --> 0x6c, Bit --> 5 */
#define BMI160_USER_GYRO_SLEEP_STATE__POS               (5)
#define BMI160_USER_GYRO_SLEEP_STATE__LEN               (1)
#define BMI160_USER_GYRO_SLEEP_STATE__MSK               (0x20)
#define BMI160_USER_GYRO_SLEEP_STATE__REG (BMI160_USER_PMU_TRIGGER_ADDR)

/* Pmu_Trigger Description - Reg Addr --> 0x6c, Bit --> 6 */
#define BMI160_USER_GYRO_WAKEUP_INTR__POS               (6)
#define BMI160_USER_GYRO_WAKEUP_INTR__LEN               (1)
#define BMI160_USER_GYRO_WAKEUP_INTR__MSK               (0x40)
#define BMI160_USER_GYRO_WAKEUP_INTR__REG (BMI160_USER_PMU_TRIGGER_ADDR)
/**************************************************************/
/**\name  ACCEL SELF TEST LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Self_Test Description - Reg Addr --> 0x6d, Bit --> 0...1 */
#define BMI160_USER_ACCEL_SELFTEST_AXIS__POS               (0)
#define BMI160_USER_ACCEL_SELFTEST_AXIS__LEN               (2)
#define BMI160_USER_ACCEL_SELFTEST_AXIS__MSK               (0x03)
#define BMI160_USER_ACCEL_SELFTEST_AXIS__REG  (BMI160_USER_SELF_TEST_ADDR)

/* Self_Test Description - Reg Addr --> 0x6d, Bit --> 2 */
#define BMI160_USER_ACCEL_SELFTEST_SIGN__POS               (2)
#define BMI160_USER_ACCEL_SELFTEST_SIGN__LEN               (1)
#define BMI160_USER_ACCEL_SELFTEST_SIGN__MSK               (0x04)
#define BMI160_USER_ACCEL_SELFTEST_SIGN__REG  (BMI160_USER_SELF_TEST_ADDR)

/* Self_Test Description - Reg Addr --> 0x6d, Bit --> 3 */
#define BMI160_USER_SELFTEST_AMP__POS               (3)
#define BMI160_USER_SELFTEST_AMP__LEN               (1)
#define BMI160_USER_SELFTEST_AMP__MSK               (0x08)
#define BMI160_USER_SELFTEST_AMP__REG   (BMI160_USER_SELF_TEST_ADDR)
/**************************************************************/
/**\name  GYRO SELF TEST LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Self_Test Description - Reg Addr --> 0x6d, Bit --> 4 */
#define BMI160_USER_GYRO_SELFTEST_START__POS               (4)
#define BMI160_USER_GYRO_SELFTEST_START__LEN               (1)
#define BMI160_USER_GYRO_SELFTEST_START__MSK               (0x10)
#define BMI160_USER_GYRO_SELFTEST_START__REG        \
(BMI160_USER_SELF_TEST_ADDR)
/**************************************************************/
/**\name  NV_CONFIG LENGTH, POSITION AND MASK*/
/**************************************************************/
/* NV_CONF Description - Reg Addr --> (0x70), Bit --> 0 */
#define BMI160_USER_NV_CONFIG_SPI_ENABLE__POS               (0)
#define BMI160_USER_NV_CONFIG_SPI_ENABLE__LEN               (1)
#define BMI160_USER_NV_CONFIG_SPI_ENABLE__MSK               (0x01)
#define BMI160_USER_NV_CONFIG_SPI_ENABLE__REG  (BMI160_USER_NV_CONFIG_ADDR)

/*IF_CONF Description - Reg Addr --> (0x70), Bit --> 1 */
#define BMI160_USER_IF_CONFIG_I2C_WDT_SELECT__POS               (1)
#define BMI160_USER_IF_CONFIG_I2C_WDT_SELECT__LEN               (1)
#define BMI160_USER_IF_CONFIG_I2C_WDT_SELECT__MSK               (0x02)
#define BMI160_USER_IF_CONFIG_I2C_WDT_SELECT__REG   \
(BMI160_USER_NV_CONFIG_ADDR)

/*IF_CONF Description - Reg Addr --> (0x70), Bit --> 2 */
#define BMI160_USER_IF_CONFIG_I2C_WDT_ENABLE__POS               (2)
#define BMI160_USER_IF_CONFIG_I2C_WDT_ENABLE__LEN               (1)
#define BMI160_USER_IF_CONFIG_I2C_WDT_ENABLE__MSK               (0x04)
#define BMI160_USER_IF_CONFIG_I2C_WDT_ENABLE__REG   \
(BMI160_USER_NV_CONFIG_ADDR)

/* NV_CONF Description - Reg Addr --> (0x70), Bit --> 3 */
#define BMI160_USER_NV_CONFIG_SPARE0__POS               (3)
#define BMI160_USER_NV_CONFIG_SPARE0__LEN               (1)
#define BMI160_USER_NV_CONFIG_SPARE0__MSK               (0x08)
#define BMI160_USER_NV_CONFIG_SPARE0__REG (BMI160_USER_NV_CONFIG_ADDR)

/* NV_CONF Description - Reg Addr --> (0x70), Bit --> 4...7 */
#define BMI160_USER_NV_CONFIG_NVM_COUNTER__POS               (4)
#define BMI160_USER_NV_CONFIG_NVM_COUNTER__LEN               (4)
#define BMI160_USER_NV_CONFIG_NVM_COUNTER__MSK               (0xF0)
#define BMI160_USER_NV_CONFIG_NVM_COUNTER__REG  (BMI160_USER_NV_CONFIG_ADDR)
/**************************************************************/
/**\name  ACCEL MANUAL OFFSET LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Offset_0 Description - Reg Addr --> (0x71), Bit --> 0...7 */
#define BMI160_USER_OFFSET_0_ACCEL_OFF_X__POS               (0)
#define BMI160_USER_OFFSET_0_ACCEL_OFF_X__LEN               (8)
#define BMI160_USER_OFFSET_0_ACCEL_OFF_X__MSK               (0xFF)
#define BMI160_USER_OFFSET_0_ACCEL_OFF_X__REG (BMI160_USER_OFFSET_0_ADDR)

/* Offset_1 Description - Reg Addr --> 0x72, Bit --> 0...7 */
#define BMI160_USER_OFFSET_1_ACCEL_OFF_Y__POS               (0)
#define BMI160_USER_OFFSET_1_ACCEL_OFF_Y__LEN               (8)
#define BMI160_USER_OFFSET_1_ACCEL_OFF_Y__MSK               (0xFF)
#define BMI160_USER_OFFSET_1_ACCEL_OFF_Y__REG (BMI160_USER_OFFSET_1_ADDR)

/* Offset_2 Description - Reg Addr --> 0x73, Bit --> 0...7 */
#define BMI160_USER_OFFSET_2_ACCEL_OFF_Z__POS               (0)
#define BMI160_USER_OFFSET_2_ACCEL_OFF_Z__LEN               (8)
#define BMI160_USER_OFFSET_2_ACCEL_OFF_Z__MSK               (0xFF)
#define BMI160_USER_OFFSET_2_ACCEL_OFF_Z__REG (BMI160_USER_OFFSET_2_ADDR)
/**************************************************************/
/**\name  GYRO MANUAL OFFSET LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Offset_3 Description - Reg Addr --> 0x74, Bit --> 0...7 */
#define BMI160_USER_OFFSET_3_GYRO_OFF_X__POS               (0)
#define BMI160_USER_OFFSET_3_GYRO_OFF_X__LEN               (8)
#define BMI160_USER_OFFSET_3_GYRO_OFF_X__MSK               (0xFF)
#define BMI160_USER_OFFSET_3_GYRO_OFF_X__REG  (BMI160_USER_OFFSET_3_ADDR)

/* Offset_4 Description - Reg Addr --> 0x75, Bit --> 0...7 */
#define BMI160_USER_OFFSET_4_GYRO_OFF_Y__POS               (0)
#define BMI160_USER_OFFSET_4_GYRO_OFF_Y__LEN               (8)
#define BMI160_USER_OFFSET_4_GYRO_OFF_Y__MSK               (0xFF)
#define BMI160_USER_OFFSET_4_GYRO_OFF_Y__REG  (BMI160_USER_OFFSET_4_ADDR)

/* Offset_5 Description - Reg Addr --> 0x76, Bit --> 0...7 */
#define BMI160_USER_OFFSET_5_GYRO_OFF_Z__POS               (0)
#define BMI160_USER_OFFSET_5_GYRO_OFF_Z__LEN               (8)
#define BMI160_USER_OFFSET_5_GYRO_OFF_Z__MSK               (0xFF)
#define BMI160_USER_OFFSET_5_GYRO_OFF_Z__REG  (BMI160_USER_OFFSET_5_ADDR)


/* Offset_6 Description - Reg Addr --> 0x77, Bit --> 0..1 */
#define BMI160_USER_OFFSET_6_GYRO_OFF_X__POS               (0)
#define BMI160_USER_OFFSET_6_GYRO_OFF_X__LEN               (2)
#define BMI160_USER_OFFSET_6_GYRO_OFF_X__MSK               (0x03)
#define BMI160_USER_OFFSET_6_GYRO_OFF_X__REG  (BMI160_USER_OFFSET_6_ADDR)

/* Offset_6 Description - Reg Addr --> 0x77, Bit --> 2...3 */
#define BMI160_USER_OFFSET_6_GYRO_OFF_Y__POS               (2)
#define BMI160_USER_OFFSET_6_GYRO_OFF_Y__LEN               (2)
#define BMI160_USER_OFFSET_6_GYRO_OFF_Y__MSK               (0x0C)
#define BMI160_USER_OFFSET_6_GYRO_OFF_Y__REG  (BMI160_USER_OFFSET_6_ADDR)

/* Offset_6 Description - Reg Addr --> 0x77, Bit --> 4...5 */
#define BMI160_USER_OFFSET_6_GYRO_OFF_Z__POS               (4)
#define BMI160_USER_OFFSET_6_GYRO_OFF_Z__LEN               (2)
#define BMI160_USER_OFFSET_6_GYRO_OFF_Z__MSK               (0x30)
#define BMI160_USER_OFFSET_6_GYRO_OFF_Z__REG   (BMI160_USER_OFFSET_6_ADDR)
/**************************************************************/
/**\name  ACCEL OFFSET  ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Offset_6 Description - Reg Addr --> 0x77, Bit --> 6 */
#define BMI160_USER_OFFSET_6_ACCEL_OFF_ENABLE__POS               (6)
#define BMI160_USER_OFFSET_6_ACCEL_OFF_ENABLE__LEN               (1)
#define BMI160_USER_OFFSET_6_ACCEL_OFF_ENABLE__MSK               (0x40)
#define BMI160_USER_OFFSET_6_ACCEL_OFF_ENABLE__REG   \
(BMI160_USER_OFFSET_6_ADDR)
/**************************************************************/
/**\name  GYRO OFFSET  ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Offset_6 Description - Reg Addr --> 0x77, Bit -->  7 */
#define BMI160_USER_OFFSET_6_GYRO_OFF_EN__POS               (7)
#define BMI160_USER_OFFSET_6_GYRO_OFF_EN__LEN               (1)
#define BMI160_USER_OFFSET_6_GYRO_OFF_EN__MSK               (0x80)
#define BMI160_USER_OFFSET_6_GYRO_OFF_EN__REG  (BMI160_USER_OFFSET_6_ADDR)


/* USER REGISTERS DEFINITION END */
/**************************************************************************/
/* CMD REGISTERS DEFINITION START */
/**************************************************************/
/**\name  COMMAND REGISTER LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Command description address - Reg Addr --> 0x7E, Bit -->  0....7 */
#define BMI160_CMD_COMMANDS__POS              (0)
#define BMI160_CMD_COMMANDS__LEN              (8)
#define BMI160_CMD_COMMANDS__MSK              (0xFF)
#define BMI160_CMD_COMMANDS__REG   (BMI160_CMD_COMMANDS_ADDR)
/**************************************************************/
/**\name  PAGE ENABLE LENGTH, POSITION AND MASK*/
/**************************************************************/
/* Target page address - Reg Addr --> 0x7F, Bit -->  4....5 */
#define BMI160_CMD_TARGET_PAGE__POS           (4)
#define BMI160_CMD_TARGET_PAGE__LEN           (2)
#define BMI160_CMD_TARGET_PAGE__MSK           (0x30)
#define BMI160_CMD_TARGET_PAGE__REG  (BMI160_CMD_EXT_MODE_ADDR)

/* Target page address - Reg Addr --> 0x7F, Bit -->  4....5 */
#define BMI160_CMD_PAGING_EN__POS           (7)
#define BMI160_CMD_PAGING_EN__LEN           (1)
#define BMI160_CMD_PAGING_EN__MSK           (0x80)
#define BMI160_CMD_PAGING_EN__REG   (BMI160_CMD_EXT_MODE_ADDR)

/* Target page address - Reg Addr --> 0x7F, Bit -->  4....5 */
#define BMI160_COM_C_TRIM_FIVE__POS           (0)
#define BMI160_COM_C_TRIM_FIVE__LEN           (8)
#define BMI160_COM_C_TRIM_FIVE__MSK           (0xFF)
#define BMI160_COM_C_TRIM_FIVE__REG   (BMI160_COM_C_TRIM_FIVE_ADDR)

/**************************************************************************/
/* CMD REGISTERS DEFINITION END */

/**************************************************/
/**\name  FIFO FRAME COUNT DEFINITION           */
/*************************************************/
#define FIFO_FRAME        (1024)
#define FIFO_CONFIG_CHECK1    (0x00)
#define FIFO_CONFIG_CHECK2    (0x80)
/**************************************************/
/**\name  MAG SENSOR SELECT          */
/*************************************************/
#define BST_BMM   (0)
#define BST_AKM   (1)
#define BMI160_YAS537_I2C_ADDRESS (0x2E)
/**************************************************/
/**\name  ACCEL RANGE          */
/*************************************************/
#define BMI160_ACCEL_RANGE_2G           (0X03)
#define BMI160_ACCEL_RANGE_4G           (0X05)
#define BMI160_ACCEL_RANGE_8G           (0X08)
#define BMI160_ACCEL_RANGE_16G          (0X0C)
/**************************************************/
/**\name  ACCEL ODR          */
/*************************************************/
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED       (0x00)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_0_78HZ         (0x01)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ         (0x02)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_3_12HZ         (0x03)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_6_25HZ         (0x04)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ         (0x05)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ           (0x06)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ           (0x07)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ          (0x08)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ          (0x09)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ          (0x0A)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ          (0x0B)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ         (0x0C)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED0      (0x0D)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED1      (0x0E)
#define BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED2      (0x0F)
/**************************************************/
/**\name  ACCEL BANDWIDTH PARAMETER         */
/*************************************************/
#define BMI160_ACCEL_OSR4_AVG1      (0x00)
#define BMI160_ACCEL_OSR2_AVG2      (0x01)
#define BMI160_ACCEL_NORMAL_AVG4    (0x02)
#define BMI160_ACCEL_CIC_AVG8     (0x03)
#define BMI160_ACCEL_RES_AVG16      (0x04)
#define BMI160_ACCEL_RES_AVG32      (0x05)
#define BMI160_ACCEL_RES_AVG64      (0x06)
#define BMI160_ACCEL_RES_AVG128     (0x07)
/**************************************************/
/**\name  GYRO ODR         */
/*************************************************/
#define BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED   (0x00)
#define BMI160_GYRO_OUTPUT_DATA_RATE_25HZ     (0x06)
#define BMI160_GYRO_OUTPUT_DATA_RATE_50HZ     (0x07)
#define BMI160_GYRO_OUTPUT_DATA_RATE_100HZ      (0x08)
#define BMI160_GYRO_OUTPUT_DATA_RATE_200HZ      (0x09)
#define BMI160_GYRO_OUTPUT_DATA_RATE_400HZ      (0x0A)
#define BMI160_GYRO_OUTPUT_DATA_RATE_800HZ      (0x0B)
#define BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ     (0x0C)
#define BMI160_GYRO_OUTPUT_DATA_RATE_3200HZ     (0x0D)
/**************************************************/
/**\name  GYRO BANDWIDTH PARAMETER         */
/*************************************************/
#define BMI160_GYRO_OSR4_MODE   (0x00)
#define BMI160_GYRO_OSR2_MODE   (0x01)
#define BMI160_GYRO_NORMAL_MODE   (0x02)
#define BMI160_GYRO_CIC_MODE    (0x03)
/**************************************************/
/**\name  GYROSCOPE RANGE PARAMETER         */
/*************************************************/
#define BMI160_GYRO_RANGE_2000_DEG_SEC  (0x00)
#define BMI160_GYRO_RANGE_1000_DEG_SEC  (0x01)
#define BMI160_GYRO_RANGE_500_DEG_SEC (0x02)
#define BMI160_GYRO_RANGE_250_DEG_SEC (0x03)
#define BMI160_GYRO_RANGE_125_DEG_SEC (0x04)

/**************************************************/
/**\name  ACCEL POWER MODE    */
/*************************************************/
#define ACCEL_MODE_NORMAL (0x11)
#define ACCEL_LOWPOWER    (0X12)
#define ACCEL_SUSPEND   (0X10)
/**************************************************/
/**\name  GYRO POWER MODE    */
/*************************************************/
#define GYRO_MODE_SUSPEND   (0x14)
#define GYRO_MODE_NORMAL    (0x15)
#define GYRO_MODE_FASTSTARTUP (0x17)
/**************************************************/
/**\name  MAG POWER MODE    */
/*************************************************/
#define MAG_MODE_SUSPEND  (0x18)
#define MAG_MODE_NORMAL   (0x19)
#define MAG_MODE_LOWPOWER (0x1A)
/**************************************************/
/**\name  ENABLE/DISABLE BIT VALUES    */
/*************************************************/
#define BMI160_ENABLE (0x01)
#define BMI160_DISABLE  (0x00)
/**************************************************/
/**\name  INTERRUPT EDGE TRIGGER ENABLE    */
/*************************************************/
#define BMI160_EDGE   (0x01)
#define BMI160_LEVEL  (0x00)
/**************************************************/
/**\name  INTERRUPT LEVEL ENABLE    */
/*************************************************/
#define BMI160_LEVEL_LOW    (0x00)
#define BMI160_LEVEL_HIGH   (0x01)
/**************************************************/
/**\name  INTERRUPT OUTPUT ENABLE    */
/*************************************************/
#define BMI160_OPEN_DRAIN (0x01)
#define BMI160_PUSH_PULL  (0x00)

/* interrupt output enable*/
#define BMI160_INPUT  (0x00)
#define BMI160_OUTPUT (0x01)


/**************************************************/
/**\name  GYRO OFFSET MASK DEFINITION   */
/*************************************************/
#define BMI160_GYRO_MANUAL_OFFSET_0_7 (0x00FF)
#define BMI160_GYRO_MANUAL_OFFSET_8_9 (0x0300)


/**\name  BIT SLICE GET AND SET FUNCTIONS  */
/*************************************************/
#define BMI160_GET_BITSLICE(regvar, bitname)\
    ((regvar & bitname##__MSK) >> bitname##__POS)


#define BMI160_SET_BITSLICE(regvar, bitname, val)\
    ((regvar & ~bitname##__MSK) | \
    ((val<<bitname##__POS)&bitname##__MSK))

#define BMI160_INF_LEN 12

/**************************************************************/
/**\name  STRUCTURE DEFINITIONS                         */
/**************************************************************/

/*!
 * @brief struct used for assign the value for
 *  gyro sleep configuration
 */
typedef struct  {
    uint8_t sleep_trigger;/**< gyro sleep trigger configuration*/
    uint8_t wakeup_trigger;/**< gyro wakeup trigger configuration*/
    uint8_t sleep_state;/**< gyro sleep state configuration*/
    uint8_t wakeup_int;/**< gyro wakeup interrupt configuration*/
} gyro_sleep_setting_t;

/*!
 * @brief bmi160 interrupt status selection enum.
 */
enum bmi160_int_status_sel {
    BMI160_INT_STATUS_0 = 1,
    BMI160_INT_STATUS_1 = 2,
    BMI160_INT_STATUS_2 = 4,
    BMI160_INT_STATUS_3 = 8,
    BMI160_INT_STATUS_ALL = 15
};

typedef enum {
    BMI160_ACCEL_ONLY = 1,
    BMI160_GYRO_ONLY = 1 << 1,
    BMI160_MAG_ONLY = 1 << 2,
    BMI160_ALL = BMI160_ACCEL_ONLY | BMI160_GYRO_ONLY | BMI160_MAG_ONLY
} bmi160_sensor_type_t;


/*!
 * @brief bmi160 interrupt status bits structure
 */
struct bmi160_int_status_bits {
    uint32_t step : 1;
    uint32_t sigmot : 1;
    uint32_t anym : 1;

    /* pmu trigger will be handled later */
    uint32_t pmu_trigger_reserved : 1;
    uint32_t d_tap : 1;
    uint32_t s_tap : 1;
    uint32_t orient : 1;
    uint32_t flat_int : 1;
    uint32_t reserved : 2;
    uint32_t high_g : 1;
    uint32_t low_g : 1;
    uint32_t drdy : 1;
    uint32_t ffull : 1;
    uint32_t fwm : 1;
    uint32_t nomo : 1;
    uint32_t anym_first_x : 1;
    uint32_t anym_first_y : 1;
    uint32_t anym_first_z : 1;
    uint32_t anym_sign : 1;
    uint32_t tap_first_x : 1;
    uint32_t tap_first_y : 1;
    uint32_t tap_first_z : 1;
    uint32_t tap_sign : 1;
    uint32_t high_first_x : 1;
    uint32_t high_first_y : 1;
    uint32_t high_first_z : 1;
    uint32_t high_sign : 1;
    uint32_t orient_1_0 : 2;
    uint32_t orient_2 : 1;
    uint32_t flat : 1;
};


/*!
 * @brief bmi160 interrupt status structure
 */
union bmi160_int_status {
    uint8_t data[4];
    struct bmi160_int_status_bits bit;
};


/*!
* @brief bmi160 structure
* This structure holds all relevant information about bmi160
*/
typedef void (*delay_msec_t)(unsigned int  msek);/**< delay function pointer */

typedef struct  {
    uint8_t chip_id;/**< chip id of BMI160 */
    uint8_t dev_addr;/**< device address of BMI160 */
    int8_t mag_manual_enable;/**< used for check the mag manual/auto mode status */
    BMI160_WR_FUNC_PTR;/**< bus write function pointer */
    BMI160_RD_FUNC_PTR;/**< bus read function pointer */
    BMI160_BRD_FUNC_PTR;/**< burst write function pointer */
    BMI160_RD_DMA_FUNC_PTR;/**< bus dma read function pointer */
    delay_msec_t delay_msec;
    uint16_t fifo_len;
    uint32_t sensor_time;
    float temperature;
    uint32_t accel_range;
    uint32_t gyro_range;
    uint8_t odr;
    union bmi160_int_status int_status;
} bmi160_t;

typedef uint32_t (*multi_axis_notify_get_data_callback_t)(bsp_multi_axis_handle_t handle, uint32_t frame_cnt, void *user_data);


typedef enum {
    BMI160_SENSOR_GET_CHIP_ID_ERROR       = -8,
    BMI160_SENSOR_GYRO_SELF_TEST_ERROR = -7,
    BMI160_SENSOR_ACCEL_SELF_TEST_ERROR = -6,
    BMI160_SENSOR_OUT_OF_RANGE_ERROR   = -5,
    BMI160_SENSOR_BUSY_ERROR                         = -4,
    BMI160_SENSOR_TIME_OUT_ERROR              = -3,
    BMI160_SENSOR_NULL_POINT_ERROR          = -2,
    BMI160_SENSOR_RETURN_INIT_VALUE         = -1,
    BMI160_SENSOR_OK                                             = 0,
} bmi160_sensor_status_t;

/*!
 * @brief Structure containing gyro xyz data
 */
typedef sensor_gyro_t  bmi160_gyro_t;

/*!
 * @brief Structure containing gyro xyz data
 */
typedef sensor_gyro_float_t bmi160_gyro_float_t;

/*!
 * @brief Structure containing accel xyz data
 */
typedef sensor_accel_t bmi160_accel_t;

/*!
 * @brief Structure containing accel xyz data
 */
typedef sensor_accel_float_t bmi160_accel_float_t;

typedef sensor_mag_xyzr_t bmi160_mag_xyzr_t;


/**************************************************/
/**\name   FUNCTION DECLARATIONS  */
/*************************************************/
extern sw_fifo_t bmi160_sw_fifo;
#define bmi160_sw_fifo_init()                     sw_fifo_init(&bmi160_sw_fifo, SENSOR_SW_FIFO_LEN, sizeof(bsp_axis_sensor_raw_data_t))
#define bmi160_push_sw_fifo(data, len)            sw_fifo_push(&bmi160_sw_fifo, data, len)
#define bmi160_pop_sw_fifo(data, len)             sw_fifo_pop(&bmi160_sw_fifo, data, len)
#define bmi160_get_sw_fifo_remain_frame_cnt()     sw_fifo_get_remain_frame_cnt(&bmi160_sw_fifo)
#define bmi160_get_sw_fifo_available_frame_cnt()  sw_fifo_get_available_frame_cnt(&bmi160_sw_fifo)

/**************************************************/
/**\name   FUNCTION FOR BMI160 INITIALIZE  */
/*************************************************/
/*!
 *  @brief
 *  This function is used for initialize
 *  bus read and bus write functions
 *  assign the chip id and device address
 *  chip id is read in the register 0x00 bit from 0 to 7
 *
 *  @param bmi160 : structure pointer
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *  @note
 *  While changing the parameter of the bmi160_t
 *  consider the following point:
 *  Changing the reference value of the parameter
 *  will changes the local copy or local reference
 *  make sure your changes will not
 *  affect the reference value of the parameter
 *  (Better case don't change the reference value of the parameter)
 *
*/


/**************************************************/
/**\name   FUNCTION FOR READ AND WRITE REGISTERS  */
/*************************************************/
/*!
 * @brief
 *  This API write the data to
 *  the given register
 *
 *
 *  @param v_addr_u8 -> Address of the register
 *  @param v_data_u8 -> The data from the register
 *  @param v_len_u8 -> no of bytes to read
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
 */
bmi160_sensor_status_t bmi160_write_reg(u8 v_addr_u8,
                                        u8 *v_data_u8, u8 v_len_u8);
/*!
 * @brief
 *  This API reads the data from
 *  the given register
 *
 *
 *  @param v_addr_u8 -> Address of the register
 *  @param v_data_u8 -> The data from the register
 *  @param v_len_u8 -> no of bytes to read
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
 */
bmi160_sensor_status_t bmi160_read_reg(u8 v_addr_u8,
                                       u8 *v_data_u8, u8 v_len_u8);
/**************************************************/
/**\name   FUNCTION FOR ERROR CODES  */
/*************************************************/
/*!
 *  @brief This API used to reads the fatal error
 *  from the Register 0x02 bit 0
 *  This flag will be reset only by power-on-reset and soft reset
 *
 *
 *  @param v_fatal_err_u8 : The status of fatal error
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_fatal_err(u8
                                            *v_fatal_err_u8);
/*!
 *  @brief This API used to read the error code
 *  from register 0x02 bit 1 to 4
 *
 *
 *  @param v_err_code_u8 : The status of error codes
 *  error_code  |    description
 *  ------------|---------------
 *  0x00        |no error
 *  0x01        |ACC_CONF error (accel ODR and bandwidth not compatible)
 *  0x02        |GYR_CONF error (Gyroscope ODR and bandwidth not compatible)
 *  0x03        |Under sampling mode and interrupt uses pre filtered data
 *  0x04        |reserved
 *  0x05        |Selected trigger-readout offset in
 *    -         |MAG_IF greater than selected ODR
 *  0x06        |FIFO configuration error for header less mode
 *  0x07        |Under sampling mode and pre filtered data as FIFO source
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_err_code(u8
                                           *v_error_code_u8);
/*!
 *  @brief This API Reads the i2c error code from the
 *  Register 0x02 bit 5.
 *  This error occurred in I2C master detected
 *
 *  @param v_i2c_err_code_u8 : The status of i2c fail error
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_i2c_fail_err(u8
                                               *v_i2c_error_code_u8);
/*!
* @brief This API Reads the dropped command error
* from the register 0x02 bit 6
*
*
*  @param v_drop_cmd_err_u8 : The status of drop command error
*
*
*
* @return results of bus communication function
* @retval 0 -> Success
* @retval -1 -> Error
*
*
*/
bmi160_sensor_status_t bmi160_get_drop_cmd_err(u8
                                               *v_drop_cmd_err_u8);

/*!
 *  @brief This function used for reading the
 *  fifo data of  header less mode for using user defined length
 *
 *
 *  @param v_fifo_user_length_u16: The value of length of fifo read data
 *
 *  @note Configure the below functions for FIFO header less mode
 *  @note 1. bmi160_set_fifo_down_gyro
 *  @note 2. bmi160_set_gyro_fifo_filter_data
 *  @note 3. bmi160_set_fifo_down_accel
 *  @note 4. bmi160_set_accel_fifo_filter_dat
 *  @note 5. bmi160_set_fifo_mag_enable
 *  @note 6. bmi160_set_fifo_accel_enable
 *  @note 7. bmi160_set_fifo_gyro_enable
 *  @note For interrupt configuration
 *  @note 1. bmi160_set_intr_fifo_full
 *  @note 2. bmi160_set_intr_fifo_wm
 *  @note 3. bmi160_set_fifo_tag_intr2_enable
 *  @note 4. bmi160_set_fifo_tag_intr1_enable
 *
 *  @note The fifo reads the whole 1024 bytes
 *  and processing the data
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
 */
bmi160_sensor_status_t
bmi160_read_fifo_headerless_mode_user_defined_length(
    u16 v_fifo_user_length_u16);
/*!
 *  @brief This function used for reading the
 *  fifo data of  header mode for using user defined length
 *
 *
 *  @note Configure the below functions for FIFO header mode
 *  @note 1. bmi160_set_fifo_down_gyro()
 *  @note 2. bmi160_set_gyro_fifo_filter_data()
 *  @note 3. bmi160_set_fifo_down_accel()
 *  @note 4. bmi160_set_accel_fifo_filter_dat()
 *  @note 5. bmi160_set_fifo_mag_enable()
 *  @note 6. bmi160_set_fifo_accel_enable()
 *  @note 7. bmi160_set_fifo_gyro_enable()
 *  @note 8. bmi160_set_fifo_header_enable()
 *  @note For interrupt configuration
 *  @note 1. bmi160_set_intr_fifo_full()
 *  @note 2. bmi160_set_intr_fifo_wm()
 *  @note 3. bmi160_set_fifo_tag_intr2_enable()
 *  @note 4. bmi160_set_fifo_tag_intr1_enable()
 *
 *  @note The fifo reads the whole 1024 bytes
 *  and processing the data
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
 */
bmi160_sensor_status_t bmi160_read_fifo_header_data_user_defined_length(
    void  *data, uint16_t  v_fifo_user_length_u16);


/*!
 *  @brief This API reads the error status
 *  from the error register 0x02 bit 0 to 7
 *
 *  @param v_mag_data_rdy_err_u8 : The status of mag data ready interrupt
 *  @param v_fatal_er_u8r : The status of fatal error
 *  @param v_err_code_u8 : The status of error code
 *  @param v_i2c_fail_err_u8 : The status of I2C fail error
 *  @param v_drop_cmd_err_u8 : The status of drop command error
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_error_status(u8 *v_fatal_er_u8r,
                                               u8 *v_err_code_u8, u8 *v_i2c_fail_err_u8,
                                               u8 *v_drop_cmd_err_u8, u8 *v_mag_data_rdy_err_u8);

/***************************************************/
/**\name  FUNCTION FOR ACCEL SELF TEST */
/***************************************************/
/*!
 * @brief This API read accel select axis to be self-test
 *
 *  @param v_accel_selftest_axis_u8 :
 *  The value of accel self test axis selection
 *  Value  |  Description
 * --------|-------------
 *   0x00  | disabled
 *   0x01  | x-axis
 *   0x02  | y-axis
 *   0x03  | z-axis
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_accel_selftest_axis(
    u8 *acc_selftest_axis);
/*!
 * @brief This API write accel select axis to be self-test
 *
 *  @param v_accel_selftest_axis_u8 :
 *  The value of accel self test axis selection
 *  Value  |  Description
 * --------|-------------
 *   0x00  | disabled
 *   0x01  | x-axis
 *   0x02  | y-axis
 *   0x03  | z-axis
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_accel_selftest_axis(
    u8 acc_selftest_axis);
/*!
 *  @brief This API read accel self test axis sign
 *  from the register 0x6D bit 2
 *
 *  @param v_accel_selftest_sign_u8: The value of accel self test axis sign
 *  Value  |  Description
 * --------|-------------
 *   0x00  | negative
 *   0x01  | positive
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_accel_selftest_sign(
    u8 *acc_selftest_sign);
/*!
 *  @brief This API write accel self test axis sign
 *  from the register 0x6D bit 2
 *
 *  @param v_accel_selftest_sign_u8: The value of accel self test axis sign
 *  Value  |  Description
 * --------|-------------
 *   0x00  | negative
 *   0x01  | positive
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_accel_selftest_sign(
    u8 acc_selftest_sign);
/*!
 *  @brief This API read accel self test amplitude
 *  from the register 0x6D bit 3
 *        select amplitude of the selftest deflection:
 *
 *  @param v_accel_selftest_amp_u8 : The value of accel self test amplitude
 *  Value  |  Description
 * --------|-------------
 *   0x00  | LOW
 *   0x01  | HIGH
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_accel_selftest_amp(
    u8 *acc_selftest_amp);
/*!
 *  @brief This API write accel self test amplitude
 *  from the register 0x6D bit 3
 *        select amplitude of the selftest deflection:
 *
 *  @param v_accel_selftest_amp_u8 : The value of accel self test amplitude
 *  Value  |  Description
 * --------|-------------
 *   0x00  | LOW
 *   0x01  | HIGH
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_accel_selftest_amp(
    u8 acc_selftest_amp);
/***************************************************/
/**\name  FUNCTION FOR GYRO SELF TEST */
/***************************************************/
/*!
 *  @brief This API read gyro self test trigger
 *
 *  @param v_gyro_selftest_start_u8: The value of gyro self test start
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_gyro_selftest_start(
    u8 *v_gyro_selftest_start_u8);
/*!
 *  @brief This API write gyro self test trigger
 *
 *  @param v_gyro_selftest_start_u8: The value of gyro self test start
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_gyro_selftest_start(
    u8 v_gyro_selftest_start_u8);


/*!
* @brief This API reads the Gyroscope self test
* status from the register 0x1B bit 1
*
*
*  @param v_gyro_selftest_u8 : The value of gyro self test status
*  value    |   status
*  ---------|----------------
*   0       | Gyroscope self test is running or failed
*   1       | Gyroscope self test completed successfully
*
*
* @return results of bus communication function
* @retval 0 -> Success
* @retval -1 -> Error
*
*
*/
bmi160_sensor_status_t bmi160_get_gyro_selftest(u8 *v_gyro_selftest_u8);

/**
* @brief  bmi160_enable_accel_selftest
* @param  None
* @return  type of communication routine
* @note acc_odr=1600Hz;  acc_bwp=2(default) ;acc_us=0(default); acc_range = 8g
*/
bmi160_sensor_status_t  bmi160_enable_accel_selftest(void);

/**
 * @brief  bmi160_set_accel_selftest_start
 * @param  None
 * @retval type of communication routine
 */
bmi160_sensor_status_t    bmi160_set_accel_selftest_start(u8 v_bmi160_accel_selftest_mode, bmi160_accel_t  *accel_data);

/*!
* @brief This API writes value to the register 0x7E bit 0 to 7
*
*
*  @param  v_command_reg_u8 : The value to write command register
*  value   |  Description
* ---------|--------------------------------------------------------
* 0x00  | Reserved
*  0x03 | Starts fast offset calibration for the accel and gyro
* 0x10  | Sets the PMU mode for the Accelerometer to suspend
* 0x11  | Sets the PMU mode for the Accelerometer to normal
* 0x12  | Sets the PMU mode for the Accelerometer Lowpower
*  0x14 | Sets the PMU mode for the Gyroscope to suspend
* 0x15  | Sets the PMU mode for the Gyroscope to normal
* 0x16  | Reserved
* 0x17  | Sets the PMU mode for the Gyroscope to fast start-up
*  0x18 | Sets the PMU mode for the Magnetometer to suspend
* 0x19  | Sets the PMU mode for the Magnetometer to normal
* 0x1A  | Sets the PMU mode for the Magnetometer to Lowpower
* 0xB0  | Clears all data in the FIFO
*  0xB1 | Resets the interrupt engine
* 0xB2  | step_cnt_clr Clears the step counter
* 0xB6  | Triggers a reset
* 0x37  | See extmode_en_last
* 0x9A  | See extmode_en_last
* 0xC0  | Enable the extended mode
*  0xC4 | Erase NVM cell
* 0xC8  | Load NVM cell
* 0xF0  | Reset acceleration data path
*
*
*
* @return results of bus communication function
* @retval 0 -> Success
* @retval -1 -> Error
*
*
*/
bmi160_sensor_status_t bmi160_set_command_register(u8 v_command_reg_u8);

/*!
 *  @brief This API is used to set the
 *  accel bandwidth from the register 0x40 bit 4 to 6
 *  @brief bandwidth parameter determines filter configuration(acc_us=0)
 *  and averaging for under sampling mode(acc_us=1)
 *
 *
 *  @param  v_bw_u8 : The value of accel bandwidth
 *
 *  @note accel bandwidth depends on under sampling parameter
 *  @note under sampling parameter cab be set by the function
 *  "BMI160_SET_ACCEL_UNDER_SAMPLING_PARAMETER"
 *
 *  @note Filter configuration
 *  accel_us  | Filter configuration
 * -----------|---------------------
 *    0x00    |  OSR4 mode
 *    0x01    |  OSR2 mode
 *    0x02    |  normal mode
 *    0x03    |  CIC mode
 *    0x04    |  Reserved
 *    0x05    |  Reserved
 *    0x06    |  Reserved
 *    0x07    |  Reserved
 *
 *  @note accel under sampling mode
 *  accel_us  | Under sampling mode
 * -----------|---------------------
 *    0x00    |  no averaging
 *    0x01    |  average 2 samples
 *    0x02    |  average 4 samples
 *    0x03    |  average 8 samples
 *    0x04    |  average 16 samples
 *    0x05    |  average 32 samples
 *    0x06    |  average 64 samples
 *    0x07    |  average 128 samples
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_accel_bw(u8 v_bw_u8);
/*!
 *  @brief This API is used to set the
 *  data of gyro from the register 0x42 bit 4 to 5
 *
 *
 *
 *
 *  @param  v_bw_u8 : The value of gyro bandwidth
 *  value     | gyro bandwidth
 *  ----------|----------------
 *   0x00     | BMI160_GYRO_OSR4_MODE
 *   0x01     | BMI160_GYRO_OSR2_MODE
 *   0x02     | BMI160_GYRO_NORMAL_MODE
 *   0x03     | BMI160_GYRO_CIC_MODE
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_gyro_bw(u8 v_bw_u8);

/*!
 *  @brief This API is used to set the
 *  gyroscope output data rate from the register 0x42 bit 0 to 3
 *
 *
 *
 *
 *  @param  v_output_data_rate_u8 :The value of gyro output data rate
 *  value     |      gyro output data rate
 * -----------|-----------------------------
 *   0x00     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x01     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x02     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x03     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x04     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x05     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x06     | BMI160_GYRO_OUTPUT_DATA_RATE_25HZ
 *   0x07     | BMI160_GYRO_OUTPUT_DATA_RATE_50HZ
 *   0x08     | BMI160_GYRO_OUTPUT_DATA_RATE_100HZ
 *   0x09     | BMI160_GYRO_OUTPUT_DATA_RATE_200HZ
 *   0x0A     | BMI160_GYRO_OUTPUT_DATA_RATE_400HZ
 *   0x0B     | BMI160_GYRO_OUTPUT_DATA_RATE_800HZ
 *   0x0C     | BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ
 *   0x0D     | BMI160_GYRO_OUTPUT_DATA_RATE_3200HZ
 *   0x0E     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x0F     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_gyro_output_data_rate(u8 v_output_data_rate_u8);

/*!
 *  @brief This API is used to set the
 *  accel output date rate form the register 0x40 bit 0 to 3
 *
 *
 *  @param  v_output_data_rate_u8 :The value of accel output date rate
 *  value |  output data rate
 * -------|--------------------------
 *   0    | BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED
 *   1    | BMI160_ACCEL_OUTPUT_DATA_RATE_0_78HZ
 *   2    | BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ
 *   3    | BMI160_ACCEL_OUTPUT_DATA_RATE_3_12HZ
 *   4    | BMI160_ACCEL_OUTPUT_DATA_RATE_6_25HZ
 *   5    | BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ
 *   6    | BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ
 *   7    | BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ
 *   8    | BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ
 *   9    | BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ
 *   10   | BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ
 *   11   | BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ
 *   12   | BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_accel_output_data_rate(u8 v_output_data_rate_u8);

/**
* @brief  bmi160_validate_accel_self_test
* @param accel_pos struct of postive sign
* @param accel_neg  struct of negative sign
* @retval type of communication routine
*/
bmi160_sensor_status_t bmi160_validate_accel_self_test(const  bmi160_accel_t *accel_pos,
                                                       const  bmi160_accel_t *accel_neg);


/*!
 *  @brief This API is used to get the
 *  accel output date rate form the register 0x40 bit 0 to 3
 *
 *
 *  @param  v_output_data_rate_u8 :The value of accel output date rate
 *  value |  output data rate
 * -------|--------------------------
 *   0    | BMI160_ACCEL_OUTPUT_DATA_RATE_RESERVED
 *   1    | BMI160_ACCEL_OUTPUT_DATA_RATE_0_78HZ
 *   2    | BMI160_ACCEL_OUTPUT_DATA_RATE_1_56HZ
 *   3    | BMI160_ACCEL_OUTPUT_DATA_RATE_3_12HZ
 *   4    | BMI160_ACCEL_OUTPUT_DATA_RATE_6_25HZ
 *   5    | BMI160_ACCEL_OUTPUT_DATA_RATE_12_5HZ
 *   6    | BMI160_ACCEL_OUTPUT_DATA_RATE_25HZ
 *   7    | BMI160_ACCEL_OUTPUT_DATA_RATE_50HZ
 *   8    | BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ
 *   9    | BMI160_ACCEL_OUTPUT_DATA_RATE_200HZ
 *   10   | BMI160_ACCEL_OUTPUT_DATA_RATE_400HZ
 *   11   | BMI160_ACCEL_OUTPUT_DATA_RATE_800HZ
 *   12   | BMI160_ACCEL_OUTPUT_DATA_RATE_1600HZ
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_accel_output_data_rate(
    u8 *v_output_data_rate_u8);

/*!
 *  @brief This API is used to set the ranges
 *  (g values) of the accel from the register 0x41 bit 0 to 3
 *
 *
 *
 *
 *  @param v_range_u8 : The value of accel g range
 *  value    | g_range
 * ----------|-----------
 *   0x03    | BMI160_ACCEL_RANGE_2G
 *   0x05    | BMI160_ACCEL_RANGE_4G
 *   0x08    | BMI160_ACCEL_RANGE_8G
 *   0x0C    | BMI160_ACCEL_RANGE_16G
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_accel_range(u8 v_range_u8);
/*!
 *  @brief This API is used to get the ranges
 *  (g values) of the accel from the register 0x41 bit 0 to 3
 *
 *
 *
 *
 *  @param v_range_u8 : The value of accel g range
 *  value    | g_range
 * ----------|-----------
 *   0x03    | BMI160_ACCEL_RANGE_2G
 *   0x05    | BMI160_ACCEL_RANGE_4G
 *   0x08    | BMI160_ACCEL_RANGE_8G
 *   0x0C    | BMI160_ACCEL_RANGE_16G
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_accel_range(
    u8 *v_range_u8);



/**
  * @brief This API resets and restarts the device
  * @param  none
  * @retval type of communication routine
  * @note All register values are overwritten with default parameters.
  */
bmi160_sensor_status_t bmi160_soft_reset(void);
/*!
 *  @brief This API is used to get the
 *  accel bandwidth from the register 0x40 bit 4 to 6
 *  @brief bandwidth parameter determines filter configuration(acc_us=0)
 *  and averaging for under sampling mode(acc_us=1)
 *
 *
 *  @param  v_bw_u8 : The value of accel bandwidth
 *
 *  @note accel bandwidth depends on under sampling parameter
 *  @note under sampling parameter cab be set by the function
 *  "BMI160_SET_ACCEL_UNDER_SAMPLING_PARAMETER"
 *
 *  @note Filter configuration
 *  accel_us  | Filter configuration
 * -----------|---------------------
 *    0x00    |  OSR4 mode
 *    0x01    |  OSR2 mode
 *    0x02    |  normal mode
 *    0x03    |  CIC mode
 *    0x04    |  Reserved
 *    0x05    |  Reserved
 *    0x06    |  Reserved
 *    0x07    |  Reserved
 *
 *  @note accel under sampling mode
 *  accel_us  | Under sampling mode
 * -----------|---------------------
 *    0x00    |  no averaging
 *    0x01    |  average 2 samples
 *    0x02    |  average 4 samples
 *    0x03    |  average 8 samples
 *    0x04    |  average 16 samples
 *    0x05    |  average 32 samples
 *    0x06    |  average 64 samples
 *    0x07    |  average 128 samples
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_accel_bw(u8 *v_bw_u8);

/*!
 *  @brief This API is used to get the
 *  gyroscope output data rate from the register 0x42 bit 0 to 3
 *
 *
 *
 *
 *  @param  v_output_data_rate_u8 :The value of gyro output data rate
 *  value     |      gyro output data rate
 * -----------|-----------------------------
 *   0x00     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x01     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x02     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x03     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x04     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x05     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x06     | BMI160_GYRO_OUTPUT_DATA_RATE_25HZ
 *   0x07     | BMI160_GYRO_OUTPUT_DATA_RATE_50HZ
 *   0x08     | BMI160_GYRO_OUTPUT_DATA_RATE_100HZ
 *   0x09     | BMI160_GYRO_OUTPUT_DATA_RATE_200HZ
 *   0x0A     | BMI160_GYRO_OUTPUT_DATA_RATE_400HZ
 *   0x0B     | BMI160_GYRO_OUTPUT_DATA_RATE_800HZ
 *   0x0C     | BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ
 *   0x0D     | BMI160_GYRO_OUTPUT_DATA_RATE_3200HZ
 *   0x0E     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *   0x0F     | BMI160_GYRO_OUTPUT_DATA_RATE_RESERVED
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_gyro_output_data_rate(
    u8 *v_output_data_rate_u8);

/*!
 *  @brief This API set the range
 *  of gyro from the register 0x43 bit 0 to 2
 *
 *  @param  v_range_u8 : The value of gyro range
 *   value    |    range
 *  ----------|-------------------------------
 *    0x00    | BMI160_GYRO_RANGE_2000_DEG_SEC
 *    0x01    | BMI160_GYRO_RANGE_1000_DEG_SEC
 *    0x02    | BMI160_GYRO_RANGE_500_DEG_SEC
 *    0x03    | BMI160_GYRO_RANGE_250_DEG_SEC
 *    0x04    | BMI160_GYRO_RANGE_125_DEG_SEC
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_gyro_range(u8 v_range_u8);

/*!
 *  @brief This API reads the range
 *  of gyro from the register 0x43 bit 0 to 2
 *
 *  @param  v_range_u8 : The value of gyro range
 *   value    |    range
 *  ----------|-------------------------------
 *    0x00    | BMI160_GYRO_RANGE_2000_DEG_SEC
 *    0x01    | BMI160_GYRO_RANGE_1000_DEG_SEC
 *    0x02    | BMI160_GYRO_RANGE_500_DEG_SEC
 *    0x03    | BMI160_GYRO_RANGE_250_DEG_SEC
 *    0x04    | BMI160_GYRO_RANGE_125_DEG_SEC
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_gyro_range(u8 *v_range_u8);

/*!
 *  @brief This API is used to get the
 *  data of gyro from the register 0x42 bit 4 to 5
 *
 *
 *
 *
 *  @param  v_bw_u8 : The value of gyro bandwidth
 *  value     | gyro bandwidth
 *  ----------|----------------
 *   0x00     | BMI160_GYRO_OSR4_MODE
 *   0x01     | BMI160_GYRO_OSR2_MODE
 *   0x02     | BMI160_GYRO_NORMAL_MODE
 *   0x03     | BMI160_GYRO_CIC_MODE
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_gyro_bw(u8 *v_bw_u8);

/*!
 *  @brief This API reads gyro data X,Y,Z values
 *  from the register 0x0C to 0x11
 *
 *
 *
 *
 *  @param gyro : The value of gyro xyz
 *
 *  @note Gyro Configuration use the following function
 *  @note bmi160_set_gyro_output_data_rate()
 *  @note bmi160_set_gyro_bw()
 *  @note bmi160_set_gyro_range()
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_read_gyro_xyz(bmi160_gyro_t *gyro);

unsigned int   bmi160_switch_range(bmi160_sensor_type_t  sensor_type, u8 range);
/*!
 *  @brief This API reads accelerometer data X,Y,Z values
 *  from the register 0x12 to 0x17
 *
 *
 *
 *
 *  @param accel :The value of accel xyz
 *
 *  @note For accel configuration use the following functions
 *  @note bmi160_set_accel_output_data_rate()
 *  @note bmi160_set_accel_bw()
 *  @note bmi160_set_accel_under_sampling_parameter()
 *  @note bmi160_set_accel_range()
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/


/**
  * @brief This function used for interrupt configuration
  *
  *
  * @return results of bus communication function
  * @retval 0 -> Success
  * @retval 1 -> Error
  */
bmi160_sensor_status_t bmi160_interrupt_configuration(u8  fifo_watermark);

/**
 * @brief  bmi160_accel_selftest
 * @param  None
 * @return  If the function succeeds, the return value is BMI160_SENSOR_OK.
 *  Otherwise, an error code is returned
 */
bmi160_sensor_status_t  bmi160_accel_selftest();

/**
  * @brief  start bmi160_gyro_selftest
  * @param  None
  * @return   If the function succeeds, the return value is BMI160_SENSOR_OK.
  *  Otherwise, an error code is returned
  */
bmi160_sensor_status_t  bmi160_gyro_selftest();

bmi160_sensor_status_t bmi160_read_accel_xyz(
    bmi160_accel_t *accel);
/*!
 *  @brief This API reads sensor_time from the register
 *  0x18 to 0x1A
 *
 *
 *  @param v_sensor_time_u32 : The value of sensor time
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_sensor_time(bmi160_t *dev);

/*!
 *  @brief This API reads the temperature of the sensor
 *  from the register 0x21 bit 0 to 7
 *
 *
 *
 *  @param v_temp_s16 : The value of temperature
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_temp(s16
                                       *v_temp_s16);


bmi160_t *bmi160_get_ptr(void);

/**
  * @brief This function is an example for delay
  * @param msek: delay in milli seconds
  * @return : communication result
  */
void bmi160_delay_ms(u32 msek);

#ifdef INCLUDE_AXIS_SENSOR_IIC
/**
 *  @brief Used for I2C initialization
 *  @note
 *  The following function is used to map the
 *  I2C bus read, write, bmi160_delay_ms and
 *  device address with global structure bmi160_t
*/
void  bmi160_i2c_routine(void);

/*!
* @brief : The function is used as I2C bus read
* @return : Status of the I2C read
* @param dev_addr : The device address of the sensor
* @param reg_addr : Address of the first register,
* will data is going to be read
* @param reg_data : This data read from the sensor,
* which is hold in an array
* @param cnt : The no of byte of data to be read
*/
s8 bmi160_i2c_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
/*!
* @brief : The function is used as I2C bus write
* @return : Status of the I2C write
* @param dev_addr : The device address of the sensor
* @param reg_addr : Address of the first register,
* will data is going to be written
* @param reg_data : It is a value hold in the array,
*   will be used for write the value into the register
* @param cnt : The no of byte of data to be write
*/
s8 bmi160_i2c_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

void axis_sensor_i2c_dma_callback(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data);


void bsp_axis_sensor_interrupt_handler(void *eint_index);
#endif
#ifdef INCLUDE_AXIS_SENSOR_SPI
void  bmi160_spi_routine(void);
void axis_sensor_spi_dma_callback(hal_spi_master_callback_event_t event, void *user_data);
#endif

uint32_t  bmi160_get_one_frame_size(bmi160_sensor_type_t senor_type);


void bmi160_sensor_interrupt_handler(bsp_multi_axis_handle_t handle);

bsp_multi_axis_status_t bmi160_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config);
bsp_multi_axis_status_t bmi160_deinit(bsp_multi_axis_handle_t handle);
uint32_t                bmi160_read_format_data(bsp_multi_axis_data_t *addr, uint32_t frame_cnt);
bsp_multi_axis_status_t bmi160_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config);


#endif

