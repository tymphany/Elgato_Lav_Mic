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


/*! file <BMI160 >
    brief <Sensor driver for BMI160> */
#include "hal.h"
#include "hal_clock.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hal_define.h"

#include "bmi160.h"
#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"

#include "airo_platform_bus.h"

/* user defined code to be added here ... */
#define FREE_COUNT_32K_TO_MS(count)  ((uint32_t)(((uint64_t)count * 1000) / 32768))


bmi160_t *p_bmi160;
uint32_t axis_sensor_index;
uint32_t gpt_start_count, gpt_end_count, gpt_duration_count;
uint32_t gpt_int_count;

ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  axis_sensor_send_data[32] = {0x00};
ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  axis_sensor_recv_data[513] = {0x00};
ATTR_RWDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t  bmi160_inf[13] = {0};


uint8_t *g_axis_sensor_recv_data;
uint8_t *g_bmi160_inf;

sw_fifo_t bmi160_sw_fifo;


/* Used to store as accel fifo data */
bmi160_accel_t accel_fifo[FIFO_FRAME_CNT];
/* Used to store as gyro fifo data */
bmi160_gyro_t gyro_fifo[FIFO_FRAME_CNT];

/* Assign the fifo time */
u32 V_fifo_time_U32 = 0;

/* FIFO data read for 1024 bytes of data */
u8 v_fifo_data_u8[FIFO_FRAME] = {0};
uint32_t bmi160_tick_count = 0;

/* add new architect */
bsp_multi_axis_handle_t                    g_bmi160_handle = 0;
bsp_multi_axis_callback_t                   axis_sensor_get_data_callback;
void                                   *axis_sensor_get_data_userdata;
bmi160_t                                s_bmi160_info;
bsp_multi_axis_enable_t                 sensor_enable;


extern int bsp_multi_axis_self_test(bsp_multi_axis_handle_t handle, bsp_multi_axis_enable_t sensor_en);

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
bmi160_sensor_status_t bmi160_init_internal(bmi160_t *bmi160)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    u8 v_pmu_data_u8 = 0;

    /* assign bmi160 ptr */
    p_bmi160 = bmi160;

    /* read Chip Id */
    com_rslt =
        p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                       BMI160_USER_CHIP_ID__REG,
                                       &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);

    if (BMI160_CHIP_ID != v_data_u8) {
        log_axis_sensor_error("[bmi160] bmi160_init_internal() fail: ID not match()(Expect %x, Read %x)", 2, BMI160_CHIP_ID, v_data_u8);
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    }
    log_axis_sensor_warn("[bmi160] Sensor ID Matched(Expect %x, Read %x)", 2, BMI160_CHIP_ID, v_data_u8);
    p_bmi160->chip_id = v_data_u8;
    /* To avoid gyro wakeup it is required to write 0x00 to 0x6C*/
    com_rslt += bmi160_write_reg(BMI160_USER_PMU_TRIGGER_ADDR,
                                 &v_pmu_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
    return com_rslt;
}

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
                                        u8 *v_data_u8, u8 v_len_u8)
{

    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* write data from register*/
        com_rslt =
            p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                            v_addr_u8, v_data_u8, v_len_u8);
    }
    return com_rslt;
}

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
                                       u8 *v_data_u8, u8 v_len_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* Read data from register*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           v_addr_u8, v_data_u8, v_len_u8);
    }

    return com_rslt;
}

bmi160_sensor_status_t bmi160_read_reg_ex(u8 v_addr_u8, u8 *v_data_u8, u8 v_len_u8 ,  axis_sensor_dma_callback_t  dev_callback, void *user_data)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* Read data from register*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_DMA_FUNC(p_bmi160->dev_addr,
                                               v_addr_u8, v_data_u8, v_len_u8, dev_callback, user_data);
    }
    return com_rslt;
}

uint32_t  bmi160_get_one_frame_size(bmi160_sensor_type_t senor_type)
{
    uint32_t size = 0;
    switch (senor_type) {
        case (BMI160_ACCEL_ONLY):
        case (BMI160_GYRO_ONLY):
        case (BMI160_MAG_ONLY):
            size =  14;
            break;
        case (BMI160_ACCEL_ONLY |BMI160_GYRO_ONLY):
        case (BMI160_GYRO_ONLY | BMI160_MAG_ONLY):
        case (BMI160_ACCEL_ONLY | BMI160_MAG_ONLY):
            size = 20;
            break;
        default:
            size = 28;
            break;
    }
    return size;
}

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
bmi160_sensor_status_t bmi160_get_fatal_err(u8 *v_fatal_err_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* reading the fatal error status*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_FATAL_ERR__REG,
                                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_fatal_err_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                              BMI160_USER_FATAL_ERR);
    }
    return com_rslt;
}

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
                                           *v_err_code_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_ERR_CODE__REG,
                                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_err_code_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                             BMI160_USER_ERR_CODE);
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_get_i2c_fail_err(u8 *v_i2c_err_code_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_I2C_FAIL_ERR__REG,
                                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_i2c_err_code_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                                 BMI160_USER_I2C_FAIL_ERR);
    }
    return com_rslt;
}
/*!
*   @brief This API Reads the dropped command error
*   from the register 0x02 bit 6
*
*
*  @param v_drop_cmd_err_u8 : The status of drop command error
*
*
*
*   @return results of bus communication function
*   @retval 0 -> Success
*   @retval -1 -> Error
*
*
*/
bmi160_sensor_status_t bmi160_get_drop_cmd_err(u8 *v_drop_cmd_err_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(
                p_bmi160->dev_addr,
                BMI160_USER_DROP_CMD_ERR__REG,
                &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_drop_cmd_err_u8 = BMI160_GET_BITSLICE(
                                 v_data_u8,
                                 BMI160_USER_DROP_CMD_ERR);
    }
    return com_rslt;
}

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
                                               u8 *v_drop_cmd_err_u8, u8 *v_mag_data_rdy_err_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read the error codes*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(
                p_bmi160->dev_addr,
                BMI160_USER_ERR_STAT__REG,
                &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        /* fatal error*/
        *v_fatal_er_u8r =
            BMI160_GET_BITSLICE(v_data_u8, BMI160_USER_FATAL_ERR);
        /* user error*/
        *v_err_code_u8 =
            BMI160_GET_BITSLICE(v_data_u8, BMI160_USER_ERR_CODE);
        /* i2c fail error*/
        *v_i2c_fail_err_u8 =
            BMI160_GET_BITSLICE(v_data_u8, BMI160_USER_I2C_FAIL_ERR);
        /* drop command error*/
        *v_drop_cmd_err_u8 =
            BMI160_GET_BITSLICE(v_data_u8, BMI160_USER_DROP_CMD_ERR);
        /* mag data ready error*/
        //*v_mag_data_rdy_err_u8 =
        //BMI160_GET_BITSLICE(v_data_u8,BMI160_USER_MAG_DADA_RDY_ERR);
    }
    return com_rslt;
}

/*!
*   @brief This API writes value to the register 0x7E bit 0 to 7
*
*
*  @param  v_command_reg_u8 : The value to write command register
*  value   |  Description
* ---------|--------------------------------------------------------
*   0x00    |   Reserved
*  0x03 |   Starts fast offset calibration for the accel and gyro
*   0x10    |   Sets the PMU mode for the Accelerometer to suspend
*   0x11    |   Sets the PMU mode for the Accelerometer to normal
*   0x12    |   Sets the PMU mode for the Accelerometer Lowpower
*  0x14 |   Sets the PMU mode for the Gyroscope to suspend
*   0x15    |   Sets the PMU mode for the Gyroscope to normal
*   0x16    |   Reserved
*   0x17    |   Sets the PMU mode for the Gyroscope to fast start-up
*  0x18 |   Sets the PMU mode for the Magnetometer to suspend
*   0x19    |   Sets the PMU mode for the Magnetometer to normal
*   0x1A    |   Sets the PMU mode for the Magnetometer to Lowpower
*   0xB0    |   Clears all data in the FIFO
*  0xB1 |   Resets the interrupt engine
*   0xB2    |   step_cnt_clr Clears the step counter
*   0xB6    |   Triggers a reset
*   0x37    |   See extmode_en_last
*   0x9A    |   See extmode_en_last
*   0xC0    |   Enable the extended mode
*  0xC4 |   Erase NVM cell
*   0xC8    |   Load NVM cell
*   0xF0    |   Reset acceleration data path
*
*
*
*   @return results of bus communication function
*   @retval 0 -> Success
*   @retval -1 -> Error
*
*
*/
bmi160_sensor_status_t bmi160_set_command_register(u8 v_command_reg_u8)
{
    bmi160_sensor_status_t com_rslt  = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* write command register */
        com_rslt = p_bmi160->BMI160_BUS_WRITE_FUNC(
                       p_bmi160->dev_addr,
                       BMI160_CMD_COMMANDS__REG,
                       &v_command_reg_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
    }
    return com_rslt;
}

/*!
 *  @brief This API reads the accelerometer power mode from
 *  PMU status register 0x03 bit 4 and 5
 *
 *
 *  @param v_accel_power_mode_stat_u8 : The value of accel power mode
 *  accel_powermode  |   value
 * ------------------|----------
 *    SUSPEND        |   0x00
 *    NORMAL         |   0x01
 *  LOW POWER        |   0x02
 *
 * @note The power mode of accel set by the 0x7E command register
 * @note using the function "bmi160_set_command_register()"
 *  value    |   mode
 *  ---------|----------------
 *   0x11    | ACCEL_MODE_NORMAL
 *   0x12    | ACCEL_LOWPOWER
 *   0x10    | ACCEL_SUSPEND
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_accel_power_mode_stat(u8
                                                        *v_accel_power_mode_stat_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_ACCEL_POWER_MODE_STAT__REG,
                                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_accel_power_mode_stat_u8 =
            BMI160_GET_BITSLICE(v_data_u8,
                                BMI160_USER_ACCEL_POWER_MODE_STAT);
    }
    return com_rslt;
}

/*!
 *  @brief This API reads gyro data X values
 *  form the register 0x0C and 0x0D
 *
 *
 *
 *
 *  @param v_gyro_x_s16 : The value of gyro x data
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
*/
bmi160_sensor_status_t bmi160_read_gyro_x(s16 *v_gyro_x_s16)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the gyro X lSB and MSB data
        v_data_u8[0] - LSB
        v_data_u8[MSB_ONE] - MSB*/
    u8 v_data_u8[BMI160_GYRO_X_DATA_SIZE] = {BMI160_SENSOR_RETURN_INIT_VALUE,
                                             BMI160_SENSOR_RETURN_INIT_VALUE
                                            };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_DATA_8_GYRO_X_LSB__REG,
                                           v_data_u8, BMI160_GYRO_DATA_LENGTH);

        *v_gyro_x_s16 = (s16)
                        ((((s32)((s8)v_data_u8[BMI160_GYRO_X_MSB_BYTE]))
                          << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                         | (v_data_u8[BMI160_GYRO_X_LSB_BYTE]));
    }
    return com_rslt;
}
/*!
 *  @brief This API reads gyro data Y values
 *  form the register 0x0E and 0x0F
 *
 *
 *
 *
 *  @param v_gyro_y_s16 : The value of gyro y data
 *
 *  @note Gyro Configuration use the following function
 *  @note bmi160_set_gyro_output_data_rate()
 *  @note bmi160_set_gyro_bw()
 *  @note bmi160_set_gyro_range()
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error result of communication routines
 *
 *
*/
bmi160_sensor_status_t bmi160_read_gyro_y(s16 *v_gyro_y_s16)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the gyro Y lSB and MSB data
        v_data_u8[LSB_ZERO] - LSB
        v_data_u8[MSB_ONE] - MSB*/
    u8 v_data_u8[BMI160_GYRO_Y_DATA_SIZE] = {BMI160_SENSOR_RETURN_INIT_VALUE,
                                             BMI160_SENSOR_RETURN_INIT_VALUE
                                            };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read gyro y data*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_DATA_10_GYRO_Y_LSB__REG,
                                           v_data_u8, BMI160_GYRO_DATA_LENGTH);

        *v_gyro_y_s16 = (s16)
                        ((((s32)((s8)v_data_u8[BMI160_GYRO_Y_MSB_BYTE]))
                          << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                         | (v_data_u8[BMI160_GYRO_Y_LSB_BYTE]));
    }
    return com_rslt;
}
/*!
 *  @brief This API reads gyro data Z values
 *  form the register 0x10 and 0x11
 *
 *
 *
 *
 *  @param v_gyro_z_s16 : The value of gyro z data
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
bmi160_sensor_status_t bmi160_read_gyro_z(s16 *v_gyro_z_s16)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the gyro Z lSB and MSB data
        v_data_u8[LSB_ZERO] - LSB
        v_data_u8[MSB_ONE] - MSB*/
    u8 v_data_u8[BMI160_GYRO_Z_DATA_SIZE] = {BMI160_SENSOR_RETURN_INIT_VALUE,
                                             BMI160_SENSOR_RETURN_INIT_VALUE
                                            };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read gyro z data */
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_DATA_12_GYRO_Z_LSB__REG,
                                           v_data_u8, BMI160_GYRO_DATA_LENGTH);

        *v_gyro_z_s16 = (s16)
                        ((((s32)((s8)v_data_u8[BMI160_GYRO_Z_MSB_BYTE]))
                          << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                         | (v_data_u8[BMI160_GYRO_Z_LSB_BYTE]));
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_read_gyro_xyz(bmi160_gyro_t *gyro)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the mag XYZ lSB and MSB data
        v_data_u8[0] - X-LSB
        v_data_u8[1] - X-MSB
        v_data_u8[0] - Y-LSB
        v_data_u8[1] - Y-MSB
        v_data_u8[0] - Z-LSB
        v_data_u8[1] - Z-MSB
        */
    u8 v_data_u8[BMI160_GYRO_XYZ_DATA_SIZE] = {
        0, 0,
        0, 0,
        0, 0
    };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read the gyro xyz data*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_DATA_8_GYRO_X_LSB__REG,
                                           v_data_u8, BMI160_GYRO_XYZ_DATA_LENGTH);

        /* Data X */
        gyro->x = (s16)
                  ((((s32)((s8)v_data_u8[
                               BMI160_DATA_FRAME_GYRO_X_MSB_BYTE]))
                    << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                   | (v_data_u8[BMI160_DATA_FRAME_GYRO_X_LSB_BYTE]));
        /* Data Y */
        gyro->y = (s16)
                  ((((s32)((s8)v_data_u8[
                               BMI160_DATA_FRAME_GYRO_Y_MSB_BYTE]))
                    << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                   | (v_data_u8[BMI160_DATA_FRAME_GYRO_Y_LSB_BYTE]));

        /* Data Z */
        gyro->z = (s16)
                  ((((s32)((s8)v_data_u8[
                               BMI160_DATA_FRAME_GYRO_Z_MSB_BYTE]))
                    << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                   | (v_data_u8[BMI160_DATA_FRAME_GYRO_Z_LSB_BYTE]));
    }
    return com_rslt;
}
/*!
 *  @brief This API reads accelerometer data X values
 *  form the register 0x12 and 0x13
 *
 *
 *
 *
 *  @param v_accel_x_s16 : The value of accel x
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
bmi160_sensor_status_t bmi160_read_accel_x(s16 *v_accel_x_s16)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the accel X lSB and MSB data
        v_data_u8[0] - LSB
        v_data_u8[1] - MSB*/
    u8 v_data_u8[BMI160_ACCEL_X_DATA_SIZE] = {BMI160_SENSOR_RETURN_INIT_VALUE,
                                              BMI160_SENSOR_RETURN_INIT_VALUE
                                             };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_DATA_14_ACCEL_X_LSB__REG,
                                           v_data_u8, BMI160_ACCEL_DATA_LENGTH);

        *v_accel_x_s16 = (s16)
                         ((((s32)((s8)v_data_u8[BMI160_ACCEL_X_MSB_BYTE]))
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (v_data_u8[BMI160_ACCEL_X_LSB_BYTE]));
    }
    return com_rslt;
}
/*!
 *  @brief This API reads accelerometer data Y values
 *  form the register 0x14 and 0x15
 *
 *
 *
 *
 *  @param v_accel_y_s16 : The value of accel y
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
bmi160_sensor_status_t bmi160_read_accel_y(s16 *v_accel_y_s16)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the accel Y lSB and MSB data
        v_data_u8[0] - LSB
        v_data_u8[1] - MSB*/
    u8 v_data_u8[BMI160_ACCEL_Y_DATA_SIZE] = {BMI160_SENSOR_RETURN_INIT_VALUE,
                                              BMI160_SENSOR_RETURN_INIT_VALUE
                                             };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_DATA_16_ACCEL_Y_LSB__REG,
                                           v_data_u8, BMI160_ACCEL_DATA_LENGTH);

        *v_accel_y_s16 = (s16)
                         ((((s32)((s8)v_data_u8[BMI160_ACCEL_Y_MSB_BYTE]))
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (v_data_u8[BMI160_ACCEL_Y_LSB_BYTE]));
    }
    return com_rslt;
}
/*!
 *  @brief This API reads accelerometer data Z values
 *  form the register 0x16 and 0x17
 *
 *
 *
 *
 *  @param v_accel_z_s16 : The value of accel z
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
bmi160_sensor_status_t bmi160_read_accel_z(s16 *v_accel_z_s16)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the accel Z lSB and MSB data
        a_data_u8r[LSB_ZERO] - LSB
        a_data_u8r[MSB_ONE] - MSB*/
    u8 a_data_u8r[BMI160_ACCEL_Z_DATA_SIZE] = {
        BMI160_SENSOR_RETURN_INIT_VALUE, BMI160_SENSOR_RETURN_INIT_VALUE
    };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_DATA_18_ACCEL_Z_LSB__REG,
                                           a_data_u8r, BMI160_ACCEL_DATA_LENGTH);

        *v_accel_z_s16 = (s16)
                         ((((s32)((s8)a_data_u8r[BMI160_ACCEL_Z_MSB_BYTE]))
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (a_data_u8r[BMI160_ACCEL_Z_LSB_BYTE]));
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_read_accel_xyz(
    bmi160_accel_t *accel)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the accel XYZ lSB and MSB data
    a_data_u8r[0] - X-LSB
    a_data_u8r[1] - X-MSB
    a_data_u8r[0] - Y-LSB
    a_data_u8r[1] - Y-MSB
    a_data_u8r[0] - Z-LSB
    a_data_u8r[1] - Z-MSB
    */
    u8 a_data_u8r[BMI160_ACCEL_XYZ_DATA_SIZE] = {
        0, 0,
        0, 0,
        0, 0
    };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_DATA_14_ACCEL_X_LSB__REG,
                                           a_data_u8r, BMI160_ACCEL_XYZ_DATA_LENGTH);

        /* Data X */
        accel->x = (s16)
                   ((((s32)((s8)a_data_u8r[
                                BMI160_DATA_FRAME_ACCEL_X_MSB_BYTE]))
                     << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                    | (a_data_u8r[BMI160_DATA_FRAME_ACCEL_X_LSB_BYTE]));
        /* Data Y */
        accel->y = (s16)
                   ((((s32)((s8)a_data_u8r[
                                BMI160_DATA_FRAME_ACCEL_Y_MSB_BYTE]))
                     << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                    | (a_data_u8r[BMI160_DATA_FRAME_ACCEL_Y_LSB_BYTE]));

        /* Data Z */
        accel->z = (s16)
                   ((((s32)((s8)a_data_u8r[
                                BMI160_DATA_FRAME_ACCEL_Z_MSB_BYTE]))
                     << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                    | (a_data_u8r[BMI160_DATA_FRAME_ACCEL_Z_LSB_BYTE]));
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_get_sensor_time(bmi160_t *dev)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the sensor time it is 32 bit data
    a_data_u8r[0] - sensor time
    a_data_u8r[1] - sensor time
    a_data_u8r[0] - sensor time
    */
    u8 a_data_u8r[BMI160_SENSOR_TIME_DATA_SIZE] = {0,   0, 0};
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__REG,
                                           a_data_u8r, BMI160_SENSOR_TIME_LENGTH);

        dev->sensor_time = (u32)
                           ((((u32)a_data_u8r[BMI160_SENSOR_TIME_MSB_BYTE])
                             << BMI160_SHIFT_BIT_POSITION_BY_16_BITS)
                            | (((u32)a_data_u8r[BMI160_SENSOR_TIME_XLSB_BYTE])
                               << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                            | (a_data_u8r[BMI160_SENSOR_TIME_LSB_BYTE]));
    }
    return com_rslt;
}

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
                                       *v_temp_s16)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the temperature lSB and MSB data
    v_data_u8[0] - LSB
    v_data_u8[1] - MSB*/
    u8 v_data_u8[BMI160_TEMP_DATA_SIZE] = {BMI160_SENSOR_RETURN_INIT_VALUE,
                                           BMI160_SENSOR_RETURN_INIT_VALUE
                                          };
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read temperature data */
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_TEMP_LSB_VALUE__REG, v_data_u8,
                                           BMI160_TEMP_DATA_LENGTH);
        *v_temp_s16 =
            (s16)(((s32)((s8)(v_data_u8[BMI160_TEMP_MSB_BYTE]) <<
                         BMI160_SHIFT_BIT_POSITION_BY_08_BITS))
                  | v_data_u8[BMI160_TEMP_LSB_BYTE]);
    }
    return com_rslt;
}

/*!
 *  @brief This API reads the status of gyro data ready form the
 *  register 0x1B bit 6
 *  The status get reset when gyro data register read out
 *
 *
 *  @param v_data_rdy_u8 :  The value of gyro data ready
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_gyro_data_rdy(u8 *v_data_rdy_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_STAT_DATA_RDY_GYRO__REG, &v_data_u8,
                                           BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_data_rdy_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                             BMI160_USER_STAT_DATA_RDY_GYRO);
    }
    return com_rslt;
}
/*!
 *  @brief This API reads the status of accel data ready form the
 *  register 0x1B bit 7
 *  The status get reset when accel data register read out
 *
 *
 *  @param v_data_rdy_u8 :  The value of accel data ready status
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t  bmi160_get_accel_data_rdy(u8
                                                  *v_data_rdy_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /*reads the status of accel data ready*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_STAT_DATA_RDY_ACCEL__REG, &v_data_u8,
                                           BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_data_rdy_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                             BMI160_USER_STAT_DATA_RDY_ACCEL);
    }
    return com_rslt;
}

/*!
 *  @brief  This API is used to set
 *  interrupt enable byte1 from the register 0x51 bit 0 to 6
 *  @brief It read the high_g_x,high_g_y,high_g_z,low_g_enable
 *  data ready, fifo full and fifo water mark.
 *
 *
 *
 *  @param  v_enable_u8 :  The value of interrupt enable
 *  @param v_enable_u8 : Value to decided to select interrupt
 *   v_enable_u8   |   interrupt
 *  ---------------|---------------
 *       0         | BMI160_HIGH_G_X_ENABLE
 *       1         | BMI160_HIGH_G_Y_ENABLE
 *       2         | BMI160_HIGH_G_Z_ENABLE
 *       3         | BMI160_LOW_G_ENABLE
 *       4         | BMI160_DATA_RDY_ENABLE
 *       5         | BMI160_FIFO_FULL_ENABLE
 *       6         | BMI160_FIFO_WM_ENABLE
 *
 *  @param v_intr_enable_1_u8 : The interrupt enable value
 *  value    | interrupt enable
 * ----------|-------------------
 *  0x01     |  BMI160_ENABLE
 *  0x00     |  BMI160_DISABLE
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
 */
bmi160_sensor_status_t bmi160_set_intr_enable_1(
    u8 v_enable_u8, u8 v_intr_enable_1_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        switch (v_enable_u8) {
            case BMI160_HIGH_G_X_ENABLE:
                /* write high_g_x interrupt*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                                          BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE,
                                                    v_intr_enable_1_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                                        BMI160_USER_INTR_ENABLE_1_HIGH_G_X_ENABLE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_HIGH_G_Y_ENABLE:
                /* write high_g_y interrupt*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                                          BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE,
                                                    v_intr_enable_1_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                                        BMI160_USER_INTR_ENABLE_1_HIGH_G_Y_ENABLE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_HIGH_G_Z_ENABLE:
                /* write high_g_z interrupt*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                                          BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE,
                                                    v_intr_enable_1_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                                        BMI160_USER_INTR_ENABLE_1_HIGH_G_Z_ENABLE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_LOW_G_ENABLE:
                /* write low_g interrupt*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                                          BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE,
                                                    v_intr_enable_1_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                                        BMI160_USER_INTR_ENABLE_1_LOW_G_ENABLE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_DATA_RDY_ENABLE:
                /* write data ready interrupt*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                                          BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE,
                                                    v_intr_enable_1_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                                        BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_FIFO_FULL_ENABLE:
                /* write fifo full interrupt*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                                          BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE,
                                                    v_intr_enable_1_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                                        BMI160_USER_INTR_ENABLE_1_FIFO_FULL_ENABLE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_FIFO_WM_ENABLE:
                /* write fifo water mark interrupt*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr, BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE,
                                                    v_intr_enable_1_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                                        BMI160_USER_INTR_ENABLE_1_FIFO_WM_ENABLE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            default:
                com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
                break;
        }
    }
    return com_rslt;
}

/*!
 *  @brief  This API is used to set
 *  interrupt enable byte1 from the register 0x51 bit 0 to 6
 *  @brief It read the high_g_x,high_g_y,high_g_z,low_g_enable
 *  data ready, fifo full and fifo water mark.
 *
 *
 *  @param v_intr_enable_1_u8 : The interrupt enable value
 *  value    | interrupt enable
 * ----------|-------------------
 *  0x01     |  BMI160_ENABLE
 *  0x00     |  BMI160_DISABLE
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
 */
bmi160_sensor_status_t  bmi160_enable_data_ready_intr(
    u8 v_intr_enable_1_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* write data ready interrupt*/
        com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                                  BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__REG,
                                                  &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        if (com_rslt == BMI160_SENSOR_OK) {
            v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                            BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE,
                                            v_intr_enable_1_u8);

            com_rslt +=
                p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                                BMI160_USER_INTR_ENABLE_1_DATA_RDY_ENABLE__REG,
                                                &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        }
    }
    return com_rslt;
}

/*!
 *  @brief This API gets the interrupt status from the sensor.
 */
bmi160_sensor_status_t bmi160_get_int_status(enum bmi160_int_status_sel int_status_sel,
                                             union bmi160_int_status *int_status)
{
    bmi160_sensor_status_t rslt = 0;

    /* To get the status of all interrupts */
    if (int_status_sel == BMI160_INT_STATUS_ALL) {
        rslt = bmi160_read_reg(BMI160_INT_STATUS_ADDR, &int_status->data[0], 4);
    } else {
        if (int_status_sel & BMI160_INT_STATUS_0) {
            rslt = bmi160_read_reg(BMI160_INT_STATUS_ADDR, &int_status->data[0], 1);
        }
        if (int_status_sel & BMI160_INT_STATUS_1) {
            rslt = bmi160_read_reg(BMI160_INT_STATUS_ADDR + 1, &int_status->data[1], 1);
        }
        if (int_status_sel & BMI160_INT_STATUS_2) {
            rslt = bmi160_read_reg(BMI160_INT_STATUS_ADDR + 2, &int_status->data[2], 1);
        }
        if (int_status_sel & BMI160_INT_STATUS_3) {
            rslt = bmi160_read_reg(BMI160_INT_STATUS_ADDR + 3, &int_status->data[3], 1);
        }
    }

    return rslt;
}

/*!
*   @brief This API is used to set the latch duration
*   from the register 0x54 bit 0 to 3
*   @brief This latch selection is not applicable for data ready,
*   orientation and flat interrupts.
*
*
*
*  @param v_latch_intr_u8 : The value of latch duration
*   Latch Duration                      |     value
* --------------------------------------|------------------
*    BMI160_LATCH_DUR_NONE              |      0x00
*    BMI160_LATCH_DUR_312_5_MICRO_SEC   |      0x01
*    BMI160_LATCH_DUR_625_MICRO_SEC     |      0x02
*    BMI160_LATCH_DUR_1_25_MILLI_SEC    |      0x03
*    BMI160_LATCH_DUR_2_5_MILLI_SEC     |      0x04
*    BMI160_LATCH_DUR_5_MILLI_SEC       |      0x05
*    BMI160_LATCH_DUR_10_MILLI_SEC      |      0x06
*    BMI160_LATCH_DUR_20_MILLI_SEC      |      0x07
*    BMI160_LATCH_DUR_40_MILLI_SEC      |      0x08
*    BMI160_LATCH_DUR_80_MILLI_SEC      |      0x09
*    BMI160_LATCH_DUR_160_MILLI_SEC     |      0x0A
*    BMI160_LATCH_DUR_320_MILLI_SEC     |      0x0B
*    BMI160_LATCH_DUR_640_MILLI_SEC     |      0x0C
*    BMI160_LATCH_DUR_1_28_SEC          |      0x0D
*    BMI160_LATCH_DUR_2_56_SEC          |      0x0E
*    BMI160_LATCHED                     |      0x0F
*
*
*
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
*
*
*/
bmi160_sensor_status_t bmi160_set_latch_intr(u8 v_latch_intr_u8)
{
    u8 v_data_u8 = 0;
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_latch_intr_u8 <= BMI160_MAX_LATCH_INTR) {
            /* write the latch duration value */
            com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr, BMI160_USER_INTR_LATCH__REG,
                                                      &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_INTR_LATCH, v_latch_intr_u8);
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr, BMI160_USER_INTR_LATCH__REG,
                                                    &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}

/*!
 *  @brief  API used for set the Configure level condition of interrupt1
 *  and interrupt2 pin form the register 0x53
 *  @brief interrupt1 - bit 1
 *  @brief interrupt2 - bit 5
 *
 *  @param v_channel_u8: The value of level condition selection
 *   v_channel_u8  |   level selection
 *  ---------------|---------------
 *       0         | BMI160_INTR1_LEVEL
 *       1         | BMI160_INTR2_LEVEL
 *
 *  @param v_intr_level_u8 : The value of level of interrupt enable
 *  value    | Behaviour
 * ----------|-------------------
 *  0x01     |  BMI160_LEVEL_HIGH
 *  0x00     |  BMI160_LEVEL_LOW
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_intr_level(
    u8 v_channel_u8, u8 v_intr_level_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        switch (v_channel_u8) {
            case BMI160_INTR1_LEVEL:
                /* write the interrupt1 level*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr, BMI160_USER_INTR1_LEVEL__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR1_LEVEL, v_intr_level_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr, BMI160_USER_INTR1_LEVEL__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_INTR2_LEVEL:
                /* write the interrupt2 level*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr, BMI160_USER_INTR2_LEVEL__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR2_LEVEL, v_intr_level_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr, BMI160_USER_INTR2_LEVEL__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            default:
                com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
                break;
        }
    }
    return com_rslt;
}

/*!
 *  @brief Write Data Ready interrupt mapped to interrupt1
 *  and interrupt2 form the register 0x56
 *  @brief interrupt1 bit 7 in the register 0x56
 *  @brief interrupt2 bit 3 in the register 0x56
 *
 *
 *  @param v_channel_u8: The value of data ready interrupt selection
 *   v_channel_u8  |   interrupt
 *  ---------------|---------------
 *       0         | BMI160_INTR1_MAP_DATA_RDY
 *       1         | BMI160_INTR2_MAP_DATA_RDY
 *
 *  @param v_intr_data_rdy_u8 : The value of data ready interrupt enable
 *  value    | interrupt enable
 * ----------|-------------------
 *  0x01     |  BMI160_ENABLE
 *  0x00     |  BMI160_DISABLE
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_intr_data_rdy(
    u8 v_channel_u8, u8 v_intr_data_rdy_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        switch (v_channel_u8) {
                /*Write Data Ready interrupt*/
            case BMI160_INTR1_MAP_DATA_RDY:
                com_rslt =
                    p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->
                                                   dev_addr, BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__REG,
                                                   &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY,
                                                    v_intr_data_rdy_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr, BMI160_USER_INTR_MAP_1_INTR1_DATA_RDY__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_INTR2_MAP_DATA_RDY:
                com_rslt =
                    p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->
                                                   dev_addr, BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__REG,
                                                   &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY,
                                                    v_intr_data_rdy_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr, BMI160_USER_INTR_MAP_1_INTR2_DATA_RDY__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            default:
                com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
                break;
        }
    }
    return com_rslt;
}

/*!
 *  @brief This API set FIFO frame
 *  header enable from the register 0x47 bit 4
 *
 *  @param v_fifo_header_u8 :The value of fifo header
 *  value    | fifo header
 * ----------|-------------------
 *  0x01     |  BMI160_ENABLE
 *  0x00     |  BMI160_DISABLE
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_fifo_header_enable(
    u8 v_fifo_header_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_fifo_header_u8 <= BMI160_MAX_VALUE_FIFO_HEADER) {
            /* write the fifo header */
            com_rslt =
                p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                               BMI160_USER_FIFO_HEADER_ENABLE__REG, &v_data_u8,
                                               BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_FIFO_HEADER_ENABLE,
                                                v_fifo_header_u8);
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(
                        p_bmi160->dev_addr,
                        BMI160_USER_FIFO_HEADER_ENABLE__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}

/*!
 *  @brief This API is used to read stored
 *  accel data in FIFO (all 3 axes) from the register 0x47 bit 6
 *
 *  @param v_fifo_accel_u8 : The value of fifo accel enble
 *  value    | fifo accel
 * ----------|-------------------
 *  0x00     |  no accel data is stored
 *  0x01     |  accel data is stored
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_fifo_accel_enable(
    u8 *v_fifo_accel_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read the accel fifo enable*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_FIFO_ACCEL_ENABLE__REG, &v_data_u8,
                                           BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_fifo_accel_u8 =
            BMI160_GET_BITSLICE(v_data_u8,
                                BMI160_USER_FIFO_ACCEL_ENABLE);
    }
    return com_rslt;
}
/*!
 *  @brief This API is used to set stored
 *  accel data in FIFO (all 3 axes) from the register 0x47 bit 6
 *
 *  @param v_fifo_accel_u8 : The value of fifo accel enble
 *  value    | fifo accel
 * ----------|-------------------
 *  0x00     |  no accel data is stored
 *  0x01     |  accel data is stored
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_fifo_accel_enable(
    u8 v_fifo_accel_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_fifo_accel_u8 <= BMI160_MAX_VALUE_FIFO_ACCEL) {
            /* write the fifo mag enables*/
            com_rslt =
                p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                               BMI160_USER_FIFO_ACCEL_ENABLE__REG, &v_data_u8,
                                               BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_FIFO_ACCEL_ENABLE, v_fifo_accel_u8);
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(
                        p_bmi160->dev_addr,
                        BMI160_USER_FIFO_ACCEL_ENABLE__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}
/*!
 *  @brief This API is used to read stored
 *   gyro data in FIFO (all 3 axes) from the resister 0x47 bit 7
 *
 *
 *  @param v_fifo_gyro_u8 : The value of fifo gyro enble
 *  value    | fifo gyro
 * ----------|-------------------
 *  0x00     |  no gyro data is stored
 *  0x01     |  gyro data is stored
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_fifo_gyro_enable(
    u8 *v_fifo_gyro_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read fifo gyro enable */
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_FIFO_GYRO_ENABLE__REG, &v_data_u8,
                                           BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_fifo_gyro_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                              BMI160_USER_FIFO_GYRO_ENABLE);
    }
    return com_rslt;
}
/*!
 *  @brief This API is used to set stored
 *  gyro data in FIFO (all 3 axes) from the resister 0x47 bit 7
 *
 *
 *  @param v_fifo_gyro_u8 : The value of fifo gyro enble
 *  value    | fifo gyro
 * ----------|-------------------
 *  0x00     |  no gyro data is stored
 *  0x01     |  gyro data is stored
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_fifo_gyro_enable(
    u8 v_fifo_gyro_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_fifo_gyro_u8 <= BMI160_MAX_VALUE_FIFO_GYRO) {
            /* write fifo gyro enable*/
            com_rslt =
                p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                               BMI160_USER_FIFO_GYRO_ENABLE__REG, &v_data_u8,
                                               BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_FIFO_GYRO_ENABLE, v_fifo_gyro_u8);
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(
                        p_bmi160->dev_addr,
                        BMI160_USER_FIFO_GYRO_ENABLE__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}

/*!
 *  @brief This API set fifo sensor time
 *  frame after the last valid data frame form the register  0x47 bit 1
 *
 *
 *
 *
 *  @param v_fifo_time_enable_u8 : The value of sensor time
 *  value      |  fifo sensor time
 * ------------|-------------------------
 *    0x00     |  do not return sensortime frame
 *    0x01     |  return sensortime frame
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
 */
bmi160_sensor_status_t bmi160_set_fifo_time_enable(
    u8 v_fifo_time_enable_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_fifo_time_enable_u8 <= BMI160_MAX_VALUE_FIFO_TIME) {
            /* write the fifo sensor time*/
            com_rslt =
                p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                               BMI160_USER_FIFO_TIME_ENABLE__REG, &v_data_u8,
                                               BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_FIFO_TIME_ENABLE,
                                                v_fifo_time_enable_u8);
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(
                        p_bmi160->dev_addr,
                        BMI160_USER_FIFO_TIME_ENABLE__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}

/*!
 *  @brief Write FIFO Watermark interrupt mapped to interrupt1
 *  and interrupt2 form the register 0x56 bit 6 and 2
 *  @brief interrupt1 bit 6 in the register 0x56
 *  @brief interrupt2 bit 2 in the register 0x56
 *
 *
 *  @param v_channel_u8: The value of fifo Watermark interrupt selection
 *   v_channel_u8  |   interrupt
 *  ---------------|---------------
 *       0         | BMI160_INTR1_MAP_FIFO_WM
 *       1         | BMI160_INTR2_MAP_FIFO_WM
 *
 *  @param v_intr_fifo_wm_u8 : The value of fifo Watermark interrupt enable
 *  value    | interrupt enable
 * ----------|-------------------
 *  0x01     |  BMI160_ENABLE
 *  0x00     |  BMI160_DISABLE
 *
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_intr_fifo_wm(
    u8 v_channel_u8, u8 v_intr_fifo_wm_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        switch (v_channel_u8) {
                /* write the fifo water mark interrupt */
            case BMI160_INTR1_MAP_FIFO_WM:
                com_rslt =
                    p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->
                                                   dev_addr, BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__REG,
                                                   &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM,
                                                    v_intr_fifo_wm_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr,
                                                        BMI160_USER_INTR_MAP_1_INTR1_FIFO_WM__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_INTR2_MAP_FIFO_WM:
                com_rslt =
                    p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->
                                                   dev_addr, BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__REG,
                                                   &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM,
                                                    v_intr_fifo_wm_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr,
                                                        BMI160_USER_INTR_MAP_1_INTR2_FIFO_WM__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            default:
                com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
                break;
        }
    }
    return com_rslt;
}

/*!
 *  @brief This API is used to Trigger an interrupt
 *  when FIFO contains water mark level from the register 0x46 bit 0 to 7
 *
 *
 *
 *  @param  v_fifo_wm_u8 : The value of fifo water mark level
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_fifo_wm(
    u8 v_fifo_wm_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* write the fifo water mark level*/
        com_rslt =
            p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->dev_addr,
                                            BMI160_USER_FIFO_WM__REG,
                                            &v_fifo_wm_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
    }
    return com_rslt;
}

/*!
 *  @brief This function used for reading the
 *  fifo data of  header mode
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
bmi160_sensor_status_t bmi160_read_fifo_header_data(void  *data_t, bmi160_t *dev)
{
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* read the whole fifo data*/
    com_rslt =
        bmi160_read_fifo_header_data_user_defined_length(data_t, dev->fifo_len);
    return com_rslt;
}

/*!
 *  @brief  API used to set output enable of interrupt1
 *  and interrupt2 from the register 0x53
 *  @brief interrupt1 - bit 2
 *  @brief interrupt2 - bit 6
 *
 *
 *  @param v_channel_u8: The value of output type enable selection
 *   v_channel_u8  |   level selection
 *  ---------------|---------------
 *       0         | BMI160_INTR1_OUTPUT_TYPE
 *       1         | BMI160_INTR2_OUTPUT_TYPE
 *
 *  @param v_intr_output_type_u8 :
 *  The value of output type of interrupt enable
 *  value    | Behaviour
 * ----------|-------------------
 *  0x01     |  BMI160_OPEN_DRAIN
 *  0x00     |  BMI160_PUSH_PULL
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_intr_output_type(
    u8 v_channel_u8, u8 v_intr_output_type_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        switch (v_channel_u8) {
            case BMI160_INTR1_OUTPUT_TYPE:
                /* write the output type of interrupt1*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->
                                                          dev_addr, BMI160_USER_INTR1_OUTPUT_TYPE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR1_OUTPUT_TYPE,
                                                    v_intr_output_type_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr, BMI160_USER_INTR1_OUTPUT_TYPE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_INTR2_OUTPUT_TYPE:
                /* write the output type of interrupt2*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->
                                                          dev_addr, BMI160_USER_INTR2_OUTPUT_TYPE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR2_OUTPUT_TYPE,
                                                    v_intr_output_type_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr, BMI160_USER_INTR2_OUTPUT_TYPE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            default:
                com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
                break;
        }
    }
    return com_rslt;
}

/*!
 *  @brief API used to set the Output enable for interrupt1
 *  and interrupt1 pin from the register 0x53
 *  @brief interrupt1 - bit 3
 *  @brief interrupt2 - bit 7
 *
 *  @param v_channel_u8: The value of output enable selection
 *   v_channel_u8  |   level selection
 *  ---------------|---------------
 *       0         | BMI160_INTR1_OUTPUT_TYPE
 *       1         | BMI160_INTR2_OUTPUT_TYPE
 *
 *  @param v_output_enable_u8 :
 *  The value of output enable of interrupt enable
 *  value    | Behaviour
 * ----------|-------------------
 *  0x00     |  BMI160_INPUT
 *  0x01     |  BMI160_OUTPUT
 *
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_set_output_enable(
    u8 v_channel_u8, u8 v_output_enable_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = 0;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        switch (v_channel_u8) {
            case BMI160_INTR1_OUTPUT_ENABLE:
                /* write the output enable of interrupt1*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->
                                                          dev_addr, BMI160_USER_INTR1_OUTPUT_ENABLE__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR1_OUTPUT_ENABLE,
                                                    v_output_enable_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr, BMI160_USER_INTR1_OUTPUT_ENABLE__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            case BMI160_INTR2_OUTPUT_ENABLE:
                /* write the output enable of interrupt2*/
                com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->
                                                          dev_addr, BMI160_USER_INTR2_OUTPUT_EN__REG,
                                                          &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                if (com_rslt == BMI160_SENSOR_OK) {
                    v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                    BMI160_USER_INTR2_OUTPUT_EN,
                                                    v_output_enable_u8);
                    com_rslt +=
                        p_bmi160->BMI160_BUS_WRITE_FUNC(p_bmi160->
                                                        dev_addr, BMI160_USER_INTR2_OUTPUT_EN__REG,
                                                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                }
                break;
            default:
                com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
                break;
        }
    }
    return com_rslt;
}

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
    u8 *v_output_data_rate_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read the accel output data rate*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__REG,
                                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_output_data_rate_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                                     BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE);
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_set_accel_output_data_rate(
    u8 v_output_data_rate_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* accel output data rate selection */
        if ((v_output_data_rate_u8 != BMI160_SENSOR_RETURN_INIT_VALUE) &&
            (v_output_data_rate_u8 <= BMI160_MAX_ACCEL_OUTPUT_DATA_RATE)) {
            /* write accel output data rate */
            com_rslt =
                p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                               BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__REG,
                                               &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE,
                                                v_output_data_rate_u8);
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(
                        p_bmi160->dev_addr,
                        BMI160_USER_ACCEL_CONFIG_OUTPUT_DATA_RATE__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_get_accel_bw(u8 *v_bw_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read the accel bandwidth */
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_ACCEL_CONFIG_ACCEL_BW__REG, &v_data_u8,
                                           BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_bw_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                       BMI160_USER_ACCEL_CONFIG_ACCEL_BW);
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_set_accel_bw(u8 v_bw_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* select accel bandwidth*/
        if (v_bw_u8 <= BMI160_MAX_ACCEL_BW) {
            /* write accel bandwidth*/
            com_rslt =
                p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                               BMI160_USER_ACCEL_CONFIG_ACCEL_BW__REG, &v_data_u8,
                                               BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_ACCEL_CONFIG_ACCEL_BW,
                                                v_bw_u8);
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(
                        p_bmi160->dev_addr,
                        BMI160_USER_ACCEL_CONFIG_ACCEL_BW__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}

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
    u8 *v_range_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read the accel range*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_ACCEL_RANGE__REG, &v_data_u8,
                                           BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_range_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                          BMI160_USER_ACCEL_RANGE);
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_set_accel_range(u8 v_range_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if ((v_range_u8 == BMI160_ACCEL_RANGE0) ||
            (v_range_u8 == BMI160_ACCEL_RANGE1) ||
            (v_range_u8 == BMI160_ACCEL_RANGE3) ||
            (v_range_u8 == BMI160_ACCEL_RANGE4)) {
            com_rslt =
                p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                               BMI160_USER_ACCEL_RANGE__REG,
                                               &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8  = BMI160_SET_BITSLICE(
                                 v_data_u8, BMI160_USER_ACCEL_RANGE,
                                 v_range_u8);
                /* write the accel range*/
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(
                        p_bmi160->dev_addr,
                        BMI160_USER_ACCEL_RANGE__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}
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
    u8 *v_output_data_rate_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read the gyro output data rate*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__REG,
                                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_output_data_rate_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                                     BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE);
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_set_gyro_output_data_rate(
    u8 v_output_data_rate_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* select the gyro output data rate*/
        if ((v_output_data_rate_u8 <  BMI160_OUTPUT_DATA_RATE6) &&
            (v_output_data_rate_u8 != BMI160_SENSOR_RETURN_INIT_VALUE)
            && (v_output_data_rate_u8 !=  BMI160_OUTPUT_DATA_RATE1)
            && (v_output_data_rate_u8 !=  BMI160_OUTPUT_DATA_RATE2)
            && (v_output_data_rate_u8 !=  BMI160_OUTPUT_DATA_RATE3)
            && (v_output_data_rate_u8 !=  BMI160_OUTPUT_DATA_RATE4)
            && (v_output_data_rate_u8 !=  BMI160_OUTPUT_DATA_RATE5)
            && (v_output_data_rate_u8 !=  BMI160_OUTPUT_DATA_RATE6)
            && (v_output_data_rate_u8 !=  BMI160_OUTPUT_DATA_RATE7)) {
            /* write the gyro output data rate */
            com_rslt = p_bmi160->BMI160_BUS_READ_FUNC
                       (p_bmi160->dev_addr,
                        BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE,
                                                v_output_data_rate_u8);
                com_rslt += p_bmi160->BMI160_BUS_WRITE_FUNC
                            (p_bmi160->dev_addr,
                             BMI160_USER_GYRO_CONFIG_OUTPUT_DATA_RATE__REG,
                             &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
                p_bmi160->odr = v_output_data_rate_u8;
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_get_gyro_bw(u8 *v_bw_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read gyro bandwidth*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_GYRO_CONFIG_BW__REG, &v_data_u8,
                                           BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_bw_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                       BMI160_USER_GYRO_CONFIG_BW);
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_set_gyro_bw(u8 v_bw_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_bw_u8 <= BMI160_MAX_GYRO_BW) {
            /* write the gyro bandwidth*/
            com_rslt =
                p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                               BMI160_USER_GYRO_CONFIG_BW__REG,
                                               &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_GYRO_CONFIG_BW, v_bw_u8);
                com_rslt +=
                    p_bmi160->BMI160_BUS_WRITE_FUNC(
                        p_bmi160->dev_addr,
                        BMI160_USER_GYRO_CONFIG_BW__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}
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
bmi160_sensor_status_t bmi160_get_gyro_range(u8 *v_range_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read the gyro range */
        com_rslt = p_bmi160->BMI160_BUS_READ_FUNC
                   (p_bmi160->dev_addr,
                    BMI160_USER_GYRO_RANGE__REG,
                    &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_range_u8 =
            BMI160_GET_BITSLICE(v_data_u8,
                                BMI160_USER_GYRO_RANGE);
    }
    return com_rslt;
}
/**
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
bmi160_sensor_status_t bmi160_set_gyro_range(u8 v_range_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_range_u8 <= BMI160_MAX_GYRO_RANGE) {
            /* write the gyro range value */
            com_rslt = p_bmi160->BMI160_BUS_READ_FUNC
                       (p_bmi160->dev_addr,
                        BMI160_USER_GYRO_RANGE__REG,
                        &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_GYRO_RANGE,
                                                v_range_u8);
                com_rslt += p_bmi160->BMI160_BUS_WRITE_FUNC
                            (p_bmi160->dev_addr,
                             BMI160_USER_GYRO_RANGE__REG,
                             &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}


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
    u8 v_gyro_selftest_start_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_gyro_selftest_start_u8 <=
            BMI160_MAX_VALUE_SELFTEST_START) {
            /* write gyro self test start */
            com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(
                           p_bmi160->dev_addr,
                           BMI160_USER_GYRO_SELFTEST_START__REG,
                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_GYRO_SELFTEST_START,
                                                v_gyro_selftest_start_u8);
                com_rslt += p_bmi160->BMI160_BUS_WRITE_FUNC(
                                p_bmi160->dev_addr,
                                BMI160_USER_GYRO_SELFTEST_START__REG,
                                &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}

/*!
*   @brief This API reads the Gyroscope self test
*   status from the register 0x1B bit 1
*
*
*  @param v_gyro_selftest_u8 : The value of gyro self test status
*  value    |   status
*  ---------|----------------
*   0       | Gyroscope self test is running or failed
*   1       | Gyroscope self test completed successfully
*
*
*   @return results of bus communication function
*   @retval 0 -> Success
*   @retval -1 -> Error
*
*
*/
bmi160_sensor_status_t bmi160_get_gyro_selftest(u8
                                                *v_gyro_selftest_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_STAT_GYRO_SELFTEST_OK__REG,
                                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_gyro_selftest_u8 = BMI160_GET_BITSLICE(v_data_u8,
                                                  BMI160_USER_STAT_GYRO_SELFTEST_OK);
    }
    return com_rslt;
}

/*!
* @brief This API read accel select axis to be self-test
*
*  @param v_accel_selftest_axis_u8 :
*   The value of accel self test axis selection
*  Value  |  Description
* --------|-------------
*   0x00  | disabled
*   0x01  | x-axis
*   0x02  | y-axis
*   0x03  | z-axis
*
*
*   @return results of bus communication function
*   @retval 0 -> Success
*   @retval -1 -> Error
*
*
*/
bmi160_sensor_status_t bmi160_get_accel_selftest_axis(
    u8 *v_accel_selftest_axis_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt  = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read accel self test axis*/
        com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(
                       p_bmi160->dev_addr,
                       BMI160_USER_ACCEL_SELFTEST_AXIS__REG,
                       &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_accel_selftest_axis_u8 = BMI160_GET_BITSLICE(
                                        v_data_u8,
                                        BMI160_USER_ACCEL_SELFTEST_AXIS);
    }
    return com_rslt;
}
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
    u8 v_accel_selftest_axis_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_accel_selftest_axis_u8
            <= BMI160_MAX_ACCEL_SELFTEST_AXIS) {
            /* write accel self test axis*/
            com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(
                           p_bmi160->dev_addr,
                           BMI160_USER_ACCEL_SELFTEST_AXIS__REG,
                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_ACCEL_SELFTEST_AXIS,
                                                v_accel_selftest_axis_u8);
                com_rslt += p_bmi160->BMI160_BUS_WRITE_FUNC(
                                p_bmi160->dev_addr,
                                BMI160_USER_ACCEL_SELFTEST_AXIS__REG,
                                &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}
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
    u8 *v_accel_selftest_sign_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt  = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read accel self test axis sign*/
        com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(
                       p_bmi160->dev_addr,
                       BMI160_USER_ACCEL_SELFTEST_SIGN__REG,
                       &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_accel_selftest_sign_u8 = BMI160_GET_BITSLICE(
                                        v_data_u8,
                                        BMI160_USER_ACCEL_SELFTEST_SIGN);
    }
    return com_rslt;
}
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
    u8 v_accel_selftest_sign_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_accel_selftest_sign_u8 <=
            BMI160_MAX_VALUE_SELFTEST_SIGN) {
            /* write accel self test axis sign*/
            com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(
                           p_bmi160->dev_addr,
                           BMI160_USER_ACCEL_SELFTEST_SIGN__REG,
                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_ACCEL_SELFTEST_SIGN,
                                                v_accel_selftest_sign_u8);
                com_rslt += p_bmi160->BMI160_BUS_WRITE_FUNC(
                                p_bmi160->dev_addr,
                                BMI160_USER_ACCEL_SELFTEST_SIGN__REG,
                                &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}
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
    u8 *v_accel_selftest_amp_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt  = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read  self test amplitude*/
        com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(
                       p_bmi160->dev_addr,
                       BMI160_USER_SELFTEST_AMP__REG,
                       &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_accel_selftest_amp_u8 = BMI160_GET_BITSLICE(
                                       v_data_u8,
                                       BMI160_USER_SELFTEST_AMP);
    }
    return com_rslt;
}
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
    u8 v_accel_selftest_amp_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        if (v_accel_selftest_amp_u8 <=
            BMI160_MAX_VALUE_SELFTEST_AMP) {
            /* write  self test amplitude*/
            com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(
                           p_bmi160->dev_addr,
                           BMI160_USER_SELFTEST_AMP__REG,
                           &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            if (com_rslt == BMI160_SENSOR_OK) {
                v_data_u8 = BMI160_SET_BITSLICE(v_data_u8,
                                                BMI160_USER_SELFTEST_AMP,
                                                v_accel_selftest_amp_u8);
                com_rslt += p_bmi160->BMI160_BUS_WRITE_FUNC(
                                p_bmi160->dev_addr,
                                BMI160_USER_SELFTEST_AMP__REG,
                                &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
            }
        } else {
            com_rslt = BMI160_SENSOR_OUT_OF_RANGE_ERROR;
        }
    }
    return com_rslt;
}
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
    u8 *v_gyro_selftest_start_u8)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt  = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_data_u8 = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read gyro self test start */
        com_rslt = p_bmi160->BMI160_BUS_READ_FUNC(
                       p_bmi160->dev_addr,
                       BMI160_USER_GYRO_SELFTEST_START__REG,
                       &v_data_u8, BMI160_GEN_READ_WRITE_DATA_LENGTH);
        *v_gyro_selftest_start_u8 = BMI160_GET_BITSLICE(
                                        v_data_u8,
                                        BMI160_USER_GYRO_SELFTEST_START);
    }
    return com_rslt;
}
/**
* @brief  bmi160_enable_accel_selftest
* @param  None
* @return  type of communication routine
* @note acc_odr=1600Hz;  acc_bwp=2(default) ;acc_us=0(default); acc_range = 8g
*/
bmi160_sensor_status_t  bmi160_enable_accel_selftest(void)
{
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;

    /* check argument */
    if (NULL == p_bmi160->delay_msec) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    }

    /*Set the accel mode as Normal write in the register 0x7E*/
    com_rslt = bmi160_set_command_register(ACCEL_MODE_NORMAL);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);

    /* set accel rang as 8G */
    com_rslt +=  bmi160_set_accel_range(BMI160_ACCEL_RANGE_8G);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set accel data rate as 1600Hz*/
    com_rslt += bmi160_set_accel_output_data_rate(BMI160_GYRO_OUTPUT_DATA_RATE_1600HZ);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    return com_rslt;
}

/**
 * @brief  bmi160_set_accel_selftest_start
 * @param  None
 * @retval type of communication routine
 */
bmi160_sensor_status_t    bmi160_set_accel_selftest_start(u8 v_bmi160_accel_selftest_mode, bmi160_accel_t  *accel_data)
{
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;

    /* check argument */
    if ((NULL == accel_data) || (NULL == p_bmi160->delay_msec)) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    }

    /* set bmi160 acc_self_test_amp */
    com_rslt  = bmi160_set_accel_selftest_sign(v_bmi160_accel_selftest_mode);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set bmi160 acc_self_test_sign */
    com_rslt += bmi160_set_accel_selftest_amp(BMI160_ACCEL_SELF_TEST_AMP_HIGH);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set bmi160 acc_self_test_enable */
    com_rslt += bmi160_set_accel_selftest_axis(BMI160_ACCEL_SELF_TEST_EN);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    if (com_rslt == BMI160_SENSOR_OK) {
        /* Read the data after a delay of 50ms */
        p_bmi160->delay_msec(BMI160_ACCEL_SELF_TEST_DELAY);

        /* read accel data*/
        com_rslt += bmi160_read_accel_xyz(accel_data);
    }

    return com_rslt;

}


/**
* @brief  bmi160_validate_accel_self_test
* @param accel_pos struct of postive sign
* @param accel_neg  struct of negative sign
* @retval type of communication routine
*/
bmi160_sensor_status_t bmi160_validate_accel_self_test(const  bmi160_accel_t *accel_pos,
                                                       const  bmi160_accel_t *accel_neg)
{
    /* Check argument */
    if ((NULL == accel_pos)  || (NULL ==  accel_neg)) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    }
    BMI160_DEBUG_INF("neg:x=%d,y=%d,z=%d", accel_neg->x, accel_neg->y, accel_neg->z);
    BMI160_DEBUG_INF("pos:x=%d,y=%d,z=%d", accel_pos->x, accel_pos->y, accel_pos->z);
    /* Validate the results of self test */
    if (((accel_neg->x - accel_pos->x) > BMI160_ACCEL_SELF_TEST_LIMIT) &&
        ((accel_neg->y - accel_pos->y) > BMI160_ACCEL_SELF_TEST_LIMIT) &&
        ((accel_neg->z - accel_pos->z) > BMI160_ACCEL_SELF_TEST_LIMIT)) {
        /* Self test pass condition */
        return BMI160_SENSOR_OK;
    } else {
        /* Self test fail condition */
        return  BMI160_SENSOR_ACCEL_SELF_TEST_ERROR;
    }
}

/**
* @brief This API makes the device with default set
* @param  none
* @retval noe
* @note when soft reset the device,all user configuration settings are overwritten
*     with their default state or the setting stored in the NVM,so here we make the device
*     work in default mode
*/
bmi160_sensor_status_t bmi160_default_param_set(void)
{
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;

    /*Set the accel mode as Normal write in the register 0x7E*/
    com_rslt = bmi160_set_command_register(ACCEL_MODE_NORMAL);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);

    /*Set the gyro mode as Normal write in the register 0x7E*/
    com_rslt += bmi160_set_command_register(GYRO_MODE_NORMAL);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GYRO_SWITCH_TO_NORMAL_DELAY);

    /* Set the accel bandwidth as Normal */
    com_rslt += bmi160_set_accel_bw(BMI160_ACCEL_NORMAL_AVG4);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* Set the gryo bandwidth as Normal */
    com_rslt += bmi160_set_gyro_bw(BMI160_GYRO_NORMAL_MODE);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set gyro data rate as 100Hz*/
    com_rslt += bmi160_set_gyro_output_data_rate(BMI160_GYRO_OUTPUT_DATA_RATE_100HZ);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set accel data rate as 100Hz*/
    com_rslt += bmi160_set_accel_output_data_rate(BMI160_ACCEL_OUTPUT_DATA_RATE_100HZ);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set accel rang as 2G */
    com_rslt +=  bmi160_set_accel_range(BMI160_ACCEL_RANGE_2G);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set gyro rang as 2000d */
    com_rslt += bmi160_set_gyro_range(BMI160_GYRO_RANGE_2000_DEG_SEC);

    /* bmi160_delay_ms in ms*/
    p_bmi160->delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    return com_rslt;

}

/**
  * @brief This API resets and restarts the device
  * @param  none
  * @retval type of communication routine
  * @note All register values are overwritten with default parameters.
  */
bmi160_sensor_status_t bmi160_soft_reset(void)
{
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 data = BMI160_SOFT_RESET_CMD;

    /* Null-pointer check */
    if (NULL ==   p_bmi160->delay_msec) {
        com_rslt = BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* Reset the device */
        com_rslt = bmi160_write_reg(BMI160_CMD_COMMANDS__REG, &data, 1);

        /* bmi160_delay_ms in ms*/
        p_bmi160->delay_msec(BMI160_SOFT_RESET__DELAY);

        if (BMI160_SENSOR_OK == com_rslt) {
            /* Update the default parameters */
            bmi160_default_param_set();
        }
    }

    return com_rslt;
}
/*!
 *  @brief This function used for reading
 *  bmi160_t structure
 *
 *  @return the reference and values of bmi160_t
 *
 *
*/
bmi160_t *bmi160_get_ptr(void)
{
    return  p_bmi160;
}

/*!
 *  @brief This API reads the fifo data of the sensor
 *  from the register 0x24
 *  @brief Data format depends on the setting of register FIFO_CONFIG
 *
 *
 *
 *  @param v_fifodata_u8 : Pointer holding the fifo data
 *  @param fifo_length_u16 : The value of fifo length maximum
 *  1024
 *
 *  @note For reading FIFO data use the following functions
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_fifo_data(
    u8 *v_fifodata_u8, u16 v_fifo_length_u16)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    } else {
        /* read fifo data*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_FIFO_DATA__REG,
                                           v_fifodata_u8, v_fifo_length_u16);

    }
    return com_rslt;
}

/*!
 *  @brief This API reads the  of the sensor
 *  form the register 0x23 and 0x24 bit 0 to 7 and 0 to 2
 *  @brief this byte counter is updated each time a complete frame
 *  was read or writtern
 *
 *
 *  @param v_fifo_length_u32 : The value of fifo byte counter
 *
 *
 *  @return results of bus communication function
 *  @retval 0 -> Success
 *  @retval -1 -> Error
 *
 *
*/
bmi160_sensor_status_t bmi160_get_fifo_length(bmi160_t *dev)
{
    /* variable used for return the status of communication result*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    /* Array contains the fifo length data
    v_data_u8[0] - fifo length
    v_data_u8[1] - fifo length*/
    u8 a_data_u8r[2] = {0, 0};
    /* check the p_bmi160 structure as NULL*/
    if (p_bmi160 == BMI160_NULL) {
        return BMI160_SENSOR_NULL_POINT_ERROR;
    } else {
        /* read fifo length*/
        com_rslt =
            p_bmi160->BMI160_BUS_READ_FUNC(p_bmi160->dev_addr,
                                           BMI160_USER_FIFO_BYTE_COUNTER_LSB__REG, a_data_u8r,
                                           BMI160_FIFO_DATA_LENGTH);

        a_data_u8r[BMI160_FIFO_LENGTH_MSB_BYTE] =
            BMI160_GET_BITSLICE(
                a_data_u8r[BMI160_FIFO_LENGTH_MSB_BYTE],
                BMI160_USER_FIFO_BYTE_COUNTER_MSB);

        dev->fifo_len =
            (u32)(((u32)((u8)(
                             a_data_u8r[BMI160_FIFO_LENGTH_MSB_BYTE]) <<
                         BMI160_SHIFT_BIT_POSITION_BY_08_BITS))
                  | a_data_u8r[BMI160_FIFO_LENGTH_LSB_BYTE]);
    }
    return com_rslt;
}

unsigned int   bmi160_switch_range(bmi160_sensor_type_t  sensor_type, u8 range)
{
    unsigned int  ret_range = 0;

    if (BMI160_ACCEL_ONLY == sensor_type) {
        switch (range) {
            case 0x5:
                ret_range = 4;
                break;
            case 0x8:
                ret_range = 8;
                break;
            case 0xC:
                ret_range = 16;
                break;
            default:
                ret_range = 2;
                break;
        }

    } else if (BMI160_GYRO_ONLY == sensor_type) {
        switch (range) {
            case 0x0:
                ret_range = 2000;
                break;
            case 0x1:
                ret_range = 1000;
                break;
            case 0x2:
                ret_range = 500;
                break;
            case 0x3:
                ret_range = 250;
                break;
            case 0x4:
                ret_range = 125;
                break;
            default:
                break;
        }

    } else {
        return 0;
    }
    return ret_range;
}


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
bmi160_sensor_status_t bmi160_pack_and_push_sw_fifo(
    u16 v_fifo_user_length_u16)
{

    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    u8 v_accel_index_u8 = 0;
    u8 v_gyro_index_u8 = 0;
    u8 v_count = 0;
    s8 v_last_return_stat_s8 = 0;
    u16 v_fifo_index_u16 = 0;
    u8 v_frame_head_u8 = 0;
    u16 v_fifo_length_u16 = 0;
    uint32_t delay_count = 0;
    bsp_axis_sensor_raw_data_t  data_t;
    float system_time_ms;
    v_fifo_length_u16 = v_fifo_user_length_u16;

    for (v_fifo_index_u16 = 0; v_fifo_index_u16 < v_fifo_length_u16;) {
        v_frame_head_u8 = g_axis_sensor_recv_data[v_fifo_index_u16];
        v_frame_head_u8 = v_frame_head_u8 & BMI160_FIFO_TAG_INTR_MASK;
        /* Get delay time */
        delay_count = p_bmi160->sensor_time - ngx_align(p_bmi160->sensor_time, 1 << (16 - p_bmi160->odr));

        /* Parse system time,every count 30us */
        system_time_ms = FREE_COUNT_32K_TO_MS(bmi160_tick_count) - delay_count * 0.039;

        switch (v_frame_head_u8) {
                /* Header frame of accel */
            case FIFO_HEAD_A: {
                /*fifo v_data_u8 frame index + 1*/
                v_fifo_index_u16 = v_fifo_index_u16 + BMI160_FIFO_INDEX_LENGTH;
                if ((v_fifo_index_u16 + BMI160_FIFO_A_LENGTH) > v_fifo_length_u16) {
                    v_last_return_stat_s8 = FIFO_A_OVER_LEN;
                    break;
                }
                /* Accel raw x v_data_u8 */
                data_t.accel_xyz.x =
                    (float)(s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                           BMI160_FIFO_X_MSB_DATA])
                                  << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                                 | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                            BMI160_FIFO_X_LSB_DATA]));

                /* Accel raw y v_data_u8 */
                data_t.accel_xyz.y =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_FIFO_Y_MSB_DATA])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_FIFO_Y_LSB_DATA]));

                /* Accel raw z v_data_u8 */
                data_t.accel_xyz.z =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_FIFO_Z_MSB_DATA])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_FIFO_Z_LSB_DATA]));

                /* index adde to 6 accel alone*/
                v_fifo_index_u16 = v_fifo_index_u16 +
                                   BMI160_FIFO_A_LENGTH;
                v_accel_index_u8++;

                /* Get fifo frame count */
                v_count = v_fifo_user_length_u16 / BMI160_FIFO_A_LENGTH;

                /* Parse sensor time */
                data_t.sensor_time = ngx_align(p_bmi160->sensor_time, 1 << (16 - p_bmi160->odr)) -
                                     (0x1 << (16 - p_bmi160->odr)) * (v_count - v_accel_index_u8) ;

                data_t.system_time_ms  = system_time_ms - (float)(0.039 * (1 << (16 - p_bmi160->odr))) * (v_count - v_accel_index_u8) ;

                data_t.temperature = p_bmi160->temperature;

                bmi160_push_sw_fifo(&data_t, 1);
                break;
            }
            /* Header frame of gyro */
            case FIFO_HEAD_G: {
                /*fifo v_data_u8 frame index + 1*/
                v_fifo_index_u16 = v_fifo_index_u16
                                   + BMI160_FIFO_INDEX_LENGTH;

                if ((v_fifo_index_u16 + BMI160_FIFO_G_LENGTH) >
                    v_fifo_length_u16) {
                    v_last_return_stat_s8 = FIFO_G_OVER_LEN;
                    break;
                }
                /* Gyro raw x v_data_u8 */
                data_t.gyro_xyz.x  =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_FIFO_X_MSB_DATA])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_FIFO_X_LSB_DATA]));

                /* Gyro raw y v_data_u8 */
                data_t.gyro_xyz.y =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_FIFO_Y_MSB_DATA])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_FIFO_Y_LSB_DATA]));

                /* Gyro raw z v_data_u8 */
                data_t.gyro_xyz.z  =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_FIFO_Z_MSB_DATA])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_FIFO_Z_LSB_DATA]));

                /*fifo G v_data_u8 frame index + 6*/
                v_fifo_index_u16 = v_fifo_index_u16 +
                                   BMI160_FIFO_G_LENGTH;
                v_gyro_index_u8++;

                /* Get fifo frame count */
                v_count = v_fifo_user_length_u16 / BMI160_FIFO_G_LENGTH;

                /* Parse sensor time */
                data_t.sensor_time = ngx_align(p_bmi160->sensor_time, 1 << (16 - p_bmi160->odr)) -
                                     (0x1 << (16 - p_bmi160->odr)) * (v_count - v_accel_index_u8) ;

                data_t.system_time_ms  = system_time_ms - (float)(0.039 * (1 << (16 - p_bmi160->odr))) * (v_count - v_accel_index_u8) ;

                data_t.temperature = p_bmi160->temperature;

                bmi160_push_sw_fifo(&data_t, 1);
                break;
            }
            /* Header frame of gyro and accel */
            case FIFO_HEAD_G_A:
                v_fifo_index_u16 = v_fifo_index_u16 + BMI160_FIFO_INDEX_LENGTH;
                if ((v_fifo_index_u16 + BMI160_FIFO_AG_LENGTH) > v_fifo_length_u16) {
                    v_last_return_stat_s8 = FIFO_G_A_OVER_LEN;
                    break;
                }

                /* Raw gyro x */
                data_t.gyro_xyz.x   =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_GA_FIFO_G_X_MSB])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_GA_FIFO_G_X_LSB]));

                /* Raw gyro y */
                data_t.gyro_xyz.y  =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_GA_FIFO_G_Y_MSB])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_GA_FIFO_G_Y_LSB]));

                /* Raw gyro z */
                data_t.gyro_xyz.z  =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_GA_FIFO_G_Z_MSB])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_GA_FIFO_G_Z_LSB]));

                /* Raw accel x */
                data_t.accel_xyz.x =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_GA_FIFO_A_X_MSB])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_GA_FIFO_A_X_LSB]));

                /* Raw accel y */
                data_t.accel_xyz.y =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_GA_FIFO_A_Y_MSB])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                     BMI160_GA_FIFO_A_Y_LSB]));

                /* Raw accel z */
                data_t.accel_xyz.z =
                    (s16)(((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                    BMI160_GA_FIFO_A_Z_MSB])
                           << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)
                          | (g_axis_sensor_recv_data[v_fifo_index_u16
                                                     + BMI160_GA_FIFO_A_Z_LSB]));

                /* Index added to 12 for gyro and accel*/
                v_fifo_index_u16 = v_fifo_index_u16 +
                                   BMI160_FIFO_AG_LENGTH;
                v_gyro_index_u8++;
                v_accel_index_u8++;

                /* Get fifo frame count */
                v_count = v_fifo_user_length_u16 / BMI160_FIFO_AG_LENGTH;

                /* Parse sensor time */
                data_t.sensor_time = ngx_align(p_bmi160->sensor_time, 1 << (16 - p_bmi160->odr)) -
                                     (1 << (16 - p_bmi160->odr)) * (v_count - v_accel_index_u8) ;

                data_t.system_time_ms  = system_time_ms - (float)(0.039 * (1 << (16 - p_bmi160->odr))) * (v_count - v_accel_index_u8) ;

                /* Parse sensor temperature */
                data_t.temperature = p_bmi160->temperature;

                bmi160_push_sw_fifo(&data_t, 1);
                break;
                /* Header frame of sensor time */
            case FIFO_HEAD_SENSOR_TIME: {
                v_fifo_index_u16 = v_fifo_index_u16 +
                                   BMI160_GEN_READ_WRITE_DATA_LENGTH;

                if ((v_fifo_index_u16
                     + BMI160_FIFO_SENSOR_TIME_LENGTH) >
                    (v_fifo_length_u16)) {
                    v_last_return_stat_s8 = FIFO_SENSORTIME_RETURN;
                    break;
                }
                /* Sensor time */
                V_fifo_time_U32 = (u32)
                                  ((g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                            BMI160_FIFO_SENSOR_TIME_MSB]
                                    << BMI160_SHIFT_BIT_POSITION_BY_16_BITS) |
                                   (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                            BMI160_FIFO_SENSOR_TIME_XLSB]
                                    << BMI160_SHIFT_BIT_POSITION_BY_08_BITS) |
                                   (g_axis_sensor_recv_data[v_fifo_index_u16 +
                                                            BMI160_FIFO_SENSOR_TIME_LSB]));

                v_fifo_index_u16 = v_fifo_index_u16 +
                                   BMI160_FIFO_SENSOR_TIME_LENGTH;
                break;
            }
            /* Header frame of skip frame */
            case FIFO_HEAD_SKIP_FRAME: {
                /*fifo v_data_u8 frame index + 1*/
                v_fifo_index_u16 = v_fifo_index_u16 +
                                   BMI160_FIFO_INDEX_LENGTH;
                if (v_fifo_index_u16 + BMI160_FIFO_INDEX_LENGTH
                    > v_fifo_length_u16) {
                    v_last_return_stat_s8 =
                        FIFO_SKIP_OVER_LEN;
                    break;
                }
                v_fifo_index_u16 = v_fifo_index_u16 +
                                   BMI160_FIFO_INDEX_LENGTH;
                break;
            }
            /* Header frame of over read fifo v_data_u8 */
            case FIFO_HEAD_OVER_READ_LSB: {
                /*fifo v_data_u8 frame index + 1*/
                v_fifo_index_u16 = v_fifo_index_u16 +
                                   BMI160_FIFO_INDEX_LENGTH;

                if ((v_fifo_index_u16 + BMI160_FIFO_INDEX_LENGTH)
                    > (v_fifo_length_u16)) {
                    v_last_return_stat_s8 = FIFO_OVER_READ_RETURN;
                    break;
                }
                if (g_axis_sensor_recv_data[v_fifo_index_u16] ==
                    FIFO_HEAD_OVER_READ_MSB) {
                    /*fifo over read frame index + 1*/
                    v_fifo_index_u16 = v_fifo_index_u16 +
                                       BMI160_FIFO_INDEX_LENGTH;
                    break;
                } else {
                    v_last_return_stat_s8 = FIFO_OVER_READ_RETURN;
                    break;
                }
            }

            default:
                v_last_return_stat_s8 = BMI160_FIFO_INDEX_LENGTH;
                break;
        }

        if (v_last_return_stat_s8) {
            break;
        }
    }

    return com_rslt;
}

/**
  * @brief This function is an example for delay
  * @param msek: delay in milli seconds
  * @return : communication result
  */
void bmi160_delay_ms(u32 msek)
{
    /* user delay*/
    hal_gpt_delay_ms(msek);
}

/**
 * @brief  bmi160_accel_selftest
 * @param  None
 * @return  If the function succeeds, the return value is BMI160_SENSOR_OK.
 *  Otherwise, an error code is returned
 */
bmi160_sensor_status_t  bmi160_accel_selftest()
{
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;
    bmi160_accel_t accel_pos, accel_neg;

    /* Enable Gyro self test bit */
    com_rslt = bmi160_enable_accel_selftest();
    if (com_rslt == BMI160_SENSOR_OK) {
        /* Perform accel self test with positive excitation */
        com_rslt = bmi160_set_accel_selftest_start(BMI160_ACCEL_SELF_TEST_SIGN_POSITIVE, &accel_pos);
        if (com_rslt == BMI160_SENSOR_OK) {
            /* Perform accel self test with negative excitation */
            com_rslt = bmi160_set_accel_selftest_start(BMI160_ACCEL_SELF_TEST_SIGN_NEGATIVE, &accel_neg);
            if (com_rslt == BMI160_SENSOR_OK) {
                /* Validate the self test result */
                com_rslt = bmi160_validate_accel_self_test(&accel_pos, &accel_neg);
            }
        }
    }
    return com_rslt;
}

/**
  * @brief  start bmi160_gyro_selftest
  * @param  None
  * @return   If the function succeeds, the return value is BMI160_SENSOR_OK.
  *  Otherwise, an error code is returned
  */
bmi160_sensor_status_t  bmi160_gyro_selftest()
{
    u8 v_gyro_selftest_u8;
    bmi160_sensor_status_t  com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;

    /* check argument */
    if (NULL == s_bmi160_info.delay_msec) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    }

    /*Set the gyro mode as Normal write in the register 0x7E*/
    com_rslt = bmi160_set_command_register(GYRO_MODE_NORMAL);

    /* bmi160_delay_ms in ms*/
    s_bmi160_info.delay_msec(BMI160_GYRO_SWITCH_TO_NORMAL_DELAY);

    /* Set the gryo bandwidth as Normal */
    com_rslt += bmi160_set_gyro_bw(BMI160_GYRO_NORMAL_MODE);

    /* bmi160_delay_ms in ms*/
    s_bmi160_info.delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set gyro data rate as 100Hz*/
    com_rslt += bmi160_set_gyro_output_data_rate(BMI160_GYRO_OUTPUT_DATA_RATE_100HZ);

    /* bmi160_delay_ms in ms*/
    s_bmi160_info.delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* start gyro selftest */
    com_rslt +=  bmi160_set_gyro_selftest_start(BMI160_GYRO_SELF_TEST_START);

    /* bmi160_delay_ms in ms,here we must delay at least 25ms to wait gyro start selftest*/
    s_bmi160_info.delay_msec(BMI160_MEASUREMENT_DELAY);

    if (BMI160_SENSOR_OK == com_rslt) {
        /* get gyro selfstart result */
        com_rslt +=  bmi160_get_gyro_selftest(&v_gyro_selftest_u8);

        /* bmi160_delay_ms in ms*/
        s_bmi160_info.delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    }
    return com_rslt;

}

/**
  * @brief This function used for interrupt configuration
  *
  *
  * @return results of bus communication function
  * @retval 0 -> Success
  * @retval 1 -> Error
  */
bmi160_sensor_status_t bmi160_interrupt_configuration(u8  fifo_watermark)
{
    /* This variable used for provide the communication results*/
    bmi160_sensor_status_t com_rslt = BMI160_SENSOR_RETURN_INIT_VALUE;

    s_bmi160_info.delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* Configure the in/out control of interrupt1*/
    com_rslt = bmi160_set_output_enable(BMI160_INTR1_OUTPUT_TYPE, BMI160_OUTPUT);

    s_bmi160_info.delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* Configure the interrupt1 active HIGH*/
    com_rslt += bmi160_set_intr_level(BMI160_INTR1_LEVEL, BMI160_LEVEL_HIGH);

    s_bmi160_info.delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* set the pint1 push poll */
    com_rslt  += bmi160_set_intr_output_type(BMI160_INTR1_OUTPUT_TYPE, BMI160_PUSH_PULL);

    s_bmi160_info.delay_msec(BMI160_GEN_READ_WRITE_DELAY);

    /* Enable the FIFO header*/
    com_rslt += bmi160_set_fifo_header_enable(FIFO_HEADER_ENABLE);
    s_bmi160_info.delay_msec(
        BMI160_GEN_READ_WRITE_DELAY);/* bmi160_delay_ms in ms*/

    /* Enable the FIFO accel*/
    com_rslt += bmi160_set_fifo_accel_enable(FIFO_ACCEL_ENABLE);
    s_bmi160_info.delay_msec(
        BMI160_GEN_READ_WRITE_DELAY);/* bmi160_delay_ms in ms*/

    /* Enable the FIFO gyro*/
    com_rslt += bmi160_set_fifo_gyro_enable(FIFO_GYRO_ENABLE);
    s_bmi160_info.delay_msec(
        BMI160_GEN_READ_WRITE_DELAY);/* bmi160_delay_ms in ms*/

    /* Enable the FIFO time*/
    com_rslt += bmi160_set_fifo_time_enable(BMI160_ENABLE);
    s_bmi160_info.delay_msec(
        BMI160_GEN_READ_WRITE_DELAY);/* bmi160_delay_ms in ms*/

    /*Map the FIFO water mark to interrupt1*/
    com_rslt += bmi160_set_intr_fifo_wm(BMI160_INTR1_MAP_FIFO_WM,
                                        BMI160_ENABLE);
    s_bmi160_info.delay_msec(
        BMI160_GEN_READ_WRITE_DELAY);/* bmi160_delay_ms in ms*/

    /* set the fifo water mark as fifo_watermark*/
    com_rslt += bmi160_set_fifo_wm(fifo_watermark);
    s_bmi160_info.delay_msec(
        BMI160_GEN_READ_WRITE_DELAY);/* bmi160_delay_ms in ms*/

    /* Enable fifo wm intr */
    com_rslt += bmi160_set_intr_enable_1(BMI160_FIFO_WM_ENABLE, BMI160_ENABLE);
    s_bmi160_info.delay_msec(
        BMI160_GEN_READ_WRITE_DELAY);/* bmi160_delay_ms in ms*/

    return com_rslt;
}

/**
  *  @brief This API reads the temperature of the sensor
  *  from the register 0x21 bit 0 to 7
  *  @param temperature : The value of temperature (unit:Degrees Celsius)
  *  @return If the function succeeds, the return value is BMI160_SENSOR_OK.
  *   Otherwise, an error code is returned
  */
bmi160_sensor_status_t bmi160_get_temperature(bmi160_t *dev)
{
    bmi160_sensor_status_t ret = BMI160_SENSOR_RETURN_INIT_VALUE;
    s16 temp = 0;

    /* null point  check */
    if (NULL == dev) {
        return BMI160_SENSOR_GET_CHIP_ID_ERROR;
    }

    /*read temperature */
    ret = bmi160_get_temp(&temp);
    if (BMI160_SENSOR_OK == ret) {
        dev->temperature = BMI160_TEMP_BASE + temp * BMI160_TEMP_RESOLUTION;
    }

    return ret;
}






//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int    platform_bmi160_bus_init(platform_bus_init_config_t *config)
{
    //printf("platform_bmi160_bus_init():handle %x, frequency %d", g_bmi160_handle, config->i2c_config.frequency);
    return platform_bus_init(g_bmi160_handle, config);
}

int platform_bmi160_bus_deinit()
{
    return platform_bus_deinit(g_bmi160_handle);
}

int    platform_bmi160_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
    platform_transfer_config_t config;

    config.buff     = reg_data;
    config.dev_addr = dev_addr;
    config.reg_addr = reg_addr;
    config.size     = cnt;
    return platform_bus_read(g_bmi160_handle, &config);
}

int platform_bmi160_bus_read_dma(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt, axis_sensor_dma_callback_t callback, void *user_data)
{
    platform_transfer_dma_config_t config;

    config.dev_addr  = BMI160_I2C_ADDR1;
    config.reg_addr  = reg_addr;

    config.call_back = callback;
    config.user_data = user_data;

    config.buff      = reg_data;
    config.size      = cnt;

    return platform_bus_read_dma(g_bmi160_handle, &config);
}

int platform_bmi160_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{

    platform_transfer_config_t config;

    config.buff     = reg_data;
    config.dev_addr = dev_addr;
    config.reg_addr = reg_addr;
    config.size     = cnt;
    return platform_bus_write(g_bmi160_handle, &config);
}


void    platform_bmi160_bus_delayms(unsigned int ms)
{
    hal_gpt_delay_ms(ms);
}








static bsp_multi_axis_status_t    bmi160_config_accel(bsp_multi_axis_gyro_range_t range, bsp_multi_axis_gyro_bandwidth_t bandwidth)
{
    bsp_multi_axis_status_t ret = BSP_MULTI_AXIS_OK;

    log_axis_sensor_info("bmi160_config_accel()----------------------------", 0);
    ret = bmi160_set_accel_range(range);
    s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);
    s_bmi160_info.accel_range = range;
    if (ret == BSP_MULTI_AXIS_OK) {
        ret = bmi160_set_accel_bw(bandwidth);
        s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);
    }
    return ret;
}

static bsp_multi_axis_status_t    bmi160_config_gyro(bsp_multi_axis_gyro_range_t range, bsp_multi_axis_gyro_bandwidth_t bandwidth)
{
    bsp_multi_axis_status_t ret = BSP_MULTI_AXIS_OK;

    log_axis_sensor_info("bmi160_config_gyro()----------------------------", 0);
    ret = bmi160_set_gyro_range(range);
    s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);
    s_bmi160_info.gyro_range = range;
    if (ret == BSP_MULTI_AXIS_OK) {
        /* Set gyro bandwidth */
        bmi160_set_gyro_bw(bandwidth);
        /* bmi160_delay_ms in ms*/
        s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);
    }
    return ret;
}


static bsp_multi_axis_status_t    bmi160_config_odr(bsp_multi_axis_enable_t  enable, bsp_multi_axis_output_data_rate_t output_data_rate, uint32_t ready_to_read_len)
{
    bsp_multi_axis_status_t ret = BSP_MULTI_AXIS_OK;

    log_axis_sensor_info("bmi160_config_odr()----------------------------", 0);
    if (enable & BSP_MULTI_AXIS_ACCEL_ENABLE) {
        /* Set accel output data rate */
        ret = bmi160_set_accel_output_data_rate(output_data_rate);
        /* bmi160_delay_ms in ms*/
        s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);
    }

    if ((enable & BSP_MULTI_AXIS_GYRO_ENABLE) && (ret == BSP_MULTI_AXIS_OK)) {
        /* set gyro sensor output data rate */
        ret += bmi160_set_gyro_output_data_rate(output_data_rate);
        /* bmi160_delay_ms in ms*/
        s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);
    }
    if ((enable & BSP_MULTI_AXIS_GYRO_ENABLE)  && (enable & BSP_MULTI_AXIS_ACCEL_ENABLE)) {
        /* config interrupt and fifo watermark*/
        ret += bmi160_interrupt_configuration(WATER_AG_MARK_SWITH(ready_to_read_len));
    } else if ((enable & BSP_MULTI_AXIS_GYRO_ENABLE)  || (enable & BSP_MULTI_AXIS_ACCEL_ENABLE)) {
        /* config interrupt and fifo watermark*/
        ret += bmi160_interrupt_configuration(WATER_A_OR_G_MARK_SWITH(ready_to_read_len));
    }
    return ret;
}




bsp_multi_axis_status_t    bmi160_init(bsp_multi_axis_handle_t handle, bsp_multi_axis_init_config_t *config)
{
    bsp_multi_axis_status_t ret = BSP_MULTI_AXIS_RETURN_INIT_VALUE;

    if (NULL == config) {
        return BSP_MULTI_AXIS_NULL_POINT_ERROR;
    }

    if (BSP_SENSOR_GET_BUS_TYPE(handle) == BSP_MULTI_AXIS_BUS_TYPE_SPI) {
        g_axis_sensor_recv_data = &axis_sensor_recv_data[1];
        g_bmi160_inf = &bmi160_inf[1];
    } else {
        g_axis_sensor_recv_data = &axis_sensor_recv_data[0];
        g_bmi160_inf = &bmi160_inf[0];
    }
    log_axis_sensor_info("bmi160_init()..................................", 0);
    g_bmi160_handle = handle;
    s_bmi160_info.bus_write    = platform_bmi160_bus_write;
    s_bmi160_info.bus_read     = platform_bmi160_bus_read;
    s_bmi160_info.bus_read_dma = platform_bmi160_bus_read_dma;
    s_bmi160_info.delay_msec   = platform_bmi160_bus_delayms;
    s_bmi160_info.dev_addr     = BMI160_I2C_ADDR1;
    platform_bmi160_bus_init((platform_bus_init_config_t *) & (config->bus_config));

    /* Read chipID and avoid  gyro wake up */
    ret = bmi160_init_internal(&s_bmi160_info);
    if (ret != BSP_MULTI_AXIS_OK) {
        log_axis_sensor_error("[bmi160] bmi160_init() fail: ID not match\r\n", 0);
        return ret;
    }
    /* Bmi160_delay_ms in ms*/
    s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);

#ifdef AXIS_SENSOR_SELFTEST_ON_INIT_STAGE
    ret = bsp_multi_axis_self_test(handle, BSP_MULTI_AXIS_GYRO_ENABLE | BSP_MULTI_AXIS_ACCEL_ENABLE);
    if (BSP_MULTI_AXIS_OK != ret) {
        log_axis_sensor_error("[bmi160] bmi160_init() fail: self test err(%d)\r\n", 1, ret);
    }
    /* Reset bmi160 */
    bmi160_soft_reset();
    s_bmi160_info.delay_msec(200);
#endif

    /* Software fifo init */
    bmi160_sw_fifo_init();
    axis_sensor_get_data_callback = config->call_back;
    axis_sensor_get_data_userdata = config->user_data;
    return BSP_MULTI_AXIS_OK;
}

bsp_multi_axis_status_t bmi160_deinit(bsp_multi_axis_handle_t handle)
{
    //todo: add deinit sensor code

    return platform_bmi160_bus_deinit();
}



bsp_multi_axis_status_t    bmi160_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config)
{
    bsp_multi_axis_status_t ret = BSP_MULTI_AXIS_OK;

    sensor_enable = config->enable;

    log_axis_sensor_info("[bmi160] bmi160_config():enable(%x).................", (unsigned int)sensor_enable);
    /* Enable accel sensor */
    if (config->enable & BSP_MULTI_AXIS_ACCEL_ENABLE) {
        ret = bmi160_set_command_register(ACCEL_MODE_NORMAL);
        if (BSP_MULTI_AXIS_OK != ret) {
            log_axis_sensor_error("[bmi160] bmi160_config() fail: set cmd for accel\r\n", 0);
        }
        s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);
    }
    if (config->enable & BSP_MULTI_AXIS_GYRO_ENABLE) {
        ret = bmi160_set_command_register(GYRO_MODE_NORMAL);
        if (BSP_MULTI_AXIS_OK != ret) {
            log_axis_sensor_error("[bmi160] bmi160_config() fail: set cmd for gyro\r\n", 0);
        }
        s_bmi160_info.delay_msec(BMI160_ACCEL_SWITCH_TO_NORMAL_DELAY);
    }

    /* config sensor*/
    if (config->enable & BSP_MULTI_AXIS_GYRO_ENABLE) {
        bmi160_config_gyro(config->gyro_range, config->gyro_bandwidth);
    }
    if (config->enable & BSP_MULTI_AXIS_ACCEL_ENABLE) {
        bmi160_config_accel(config->accel_range, config->accel_bandwidth);
    }

    /*config sensor output data rate*/
    bmi160_config_odr(config->enable, config->sensor_odr, config->frame_threshold);

    return BSP_MULTI_AXIS_OK;
}




uint32_t  bmi160_read_format_data(bsp_multi_axis_data_t *addr, uint32_t frame_cnt)
{
    int i = 0;
    uint32_t  accel_range = 0;
    uint32_t  gyro_range = 0;

    bsp_axis_sensor_raw_data_t raw_data_with_head;
    uint32_t cnt = 0;

    /* Read accel range */
    accel_range =  bmi160_switch_range(BMI160_ACCEL_ONLY, p_bmi160->accel_range);

    /* Get gyro range */
    gyro_range =  bmi160_switch_range(BMI160_GYRO_ONLY, p_bmi160->gyro_range);

    for (i = 0; i < frame_cnt; i++) {
        cnt = bmi160_pop_sw_fifo(&raw_data_with_head, 1);
        if (cnt == 0) {
            return i;
        }

        /*Parse header include system timer,sensor_time and temperature */
        addr->head.system_time_ms = raw_data_with_head.system_time_ms;
        addr->head.sensor_time =  raw_data_with_head.sensor_time;
        addr->head.temperature = raw_data_with_head.temperature;
        /* Parse sensor xyz axis */
        addr->axis_sensor_data_t.accel_data.x = (float)raw_data_with_head.accel_xyz.x / (UINT16_T_MAX / accel_range);
        addr->axis_sensor_data_t.accel_data.y = (float)raw_data_with_head.accel_xyz.y / (UINT16_T_MAX / accel_range);
        addr->axis_sensor_data_t.accel_data.z = (float)raw_data_with_head.accel_xyz.z / (UINT16_T_MAX / accel_range);
        addr->axis_sensor_data_t.gyro_data.x = (float)raw_data_with_head.gyro_xyz.x * gyro_range / UINT16_T_MAX;
        addr->axis_sensor_data_t.gyro_data.y = (float)raw_data_with_head.gyro_xyz.y * gyro_range / UINT16_T_MAX;
        addr->axis_sensor_data_t.gyro_data.z = (float)raw_data_with_head.gyro_xyz.z * gyro_range / UINT16_T_MAX;
        addr++;
    }
    return frame_cnt;
}
/*!
 * @brief This API will be called when iic get sensor information dma transfer finish
 */

void axis_sensor_dma_callback(void *user_data)

{
    /* Get fifo len and frame count */
    uint32_t  sensor_fifo_len =  p_bmi160->fifo_len / BMI160_FIFO_AG_LENGTH;

    /* Pack fifo frame and push it into sw fifo */
    bmi160_pack_and_push_sw_fifo(p_bmi160->fifo_len);

    /* Callback user function */
    (*axis_sensor_get_data_callback)(g_bmi160_handle, sensor_fifo_len, axis_sensor_get_data_userdata);

    /* Note:Here we must set debounce time again,otherwise when we unmask intr,it will be fired
      * immediately once meeting debounce time condition during mask intr.
      */
    hal_eint_set_debounce_time((hal_eint_number_t)user_data, 1);

    /* Umask the index interrupt */
    hal_eint_unmask((hal_eint_number_t)user_data);
}



void  bmi160_get_fifo_len_callback(void *user_data)
{
    s16 temp = 0;

    /* Get sensor time */
    p_bmi160->sensor_time = (u32)((((u32)g_bmi160_inf[BMI160_SENSOR_TIME_MSB_BYTE]) <<
                                   BMI160_SHIFT_BIT_POSITION_BY_16_BITS) |
                                  (((u32)g_bmi160_inf[BMI160_SENSOR_TIME_XLSB_BYTE]) <<
                                   BMI160_SHIFT_BIT_POSITION_BY_08_BITS) |
                                  (g_bmi160_inf[BMI160_SENSOR_TIME_LSB_BYTE]));

    /* Get system tick count */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bmi160_tick_count);

    temp = (s16)(((s32)((s8)(g_bmi160_inf[9]) << BMI160_SHIFT_BIT_POSITION_BY_08_BITS)) | g_bmi160_inf[8]);

    /* Get sensor temperature */
    p_bmi160->temperature = BMI160_TEMP_BASE + temp * BMI160_TEMP_RESOLUTION;

    g_bmi160_inf[11] = BMI160_GET_BITSLICE(g_bmi160_inf[11], BMI160_USER_FIFO_BYTE_COUNTER_MSB);

    /* Get sensor fifo len */
    p_bmi160->fifo_len = (u32)(((u32)((u8)(g_bmi160_inf[11]) <<
                                      BMI160_SHIFT_BIT_POSITION_BY_08_BITS)) | g_bmi160_inf[10]);

    /* If fifo len 0,then unmask intr */
    if (0 == p_bmi160->fifo_len) {
        log_axis_sensor_error("[bmi160] bmi160_get_fifo_len_callback() fail:fifo len is 0\r\n", 0);
        hal_eint_unmask((hal_eint_number_t)user_data);
        return;
    }
    /* If get fifo data fail,the unmask intr */
    if (BMI160_SENSOR_OK != bmi160_read_reg_ex(BMI160_USER_FIFO_DATA__REG, axis_sensor_recv_data,
                                               p_bmi160->fifo_len, axis_sensor_dma_callback, user_data)) {
    }
}

void bmi160_sensor_interrupt_handler(bsp_multi_axis_handle_t handle)
{
    /* If get sensor information fail,unmask intr */
    bmi160_sensor_status_t sensor_status;

    sensor_status = bmi160_read_reg_ex(BMI160_USER_SENSORTIME_0_SENSOR_TIME_LSB__REG, bmi160_inf,
                                       BMI160_INF_LEN, bmi160_get_fifo_len_callback, (void *)BSP_SENSOR_GET_EINT_NUM(handle));
    if (BMI160_SENSOR_OK != sensor_status) {
        log_axis_sensor_error("[bmi160] bmi160_sensor_interrupt_handler()fail: read INF_LEN err(%d)\r\n", 1, sensor_status);
    }
}
