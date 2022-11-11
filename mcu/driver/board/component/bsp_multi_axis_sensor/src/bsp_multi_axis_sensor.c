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

#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_common.h"
#include "bmi160.h"
#include "lsm6ds3.h"
#include "lsm6dso.h"
#include "lsm6dsox.h"
#include "icm42607.h"
#include "qmi8658.h"


log_create_module(bsp_multi_sensor, PRINT_LEVEL_WARNING);


static void bsp_axis_sensor_eintx_handler(void *user_data)
{
    uint32_t handle = (uint32_t)user_data;

    //log_axis_sensor_error("[bsp][axis] bsp_axis_sensor_eintx_handler()", 0);
    if (BSP_SENSOR_GET_MAGICID(handle) != BSP_SENSOR_MAGIC_ID) {
        log_axis_sensor_error("[bsp][axis] bsp_axis_sensor_eintx_handler() fail: handle invalid(%x)", 1, (unsigned int)handle);
        return;
    }
    switch (BSP_SENSOR_GET_VENDOR(handle)) {
        case LSM6DS3:{
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DS3
                uint8_t  eint_num = 0;
                lsm6ds3_eint_irq_handle(handle);
                eint_num = BSP_SENSOR_GET_EINT_NUM(handle);
                hal_eint_unmask(eint_num);
#endif
            } break;
		case LSM6DSO: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSO
				lsm6dso_eint_irq_handle(handle);
#endif
			} break;
        case BMI160: {
#ifdef BSP_MULTI_AXIS_SENSOR_BMI160
            	bmi160_sensor_interrupt_handler(handle);
#endif
			} break;
        case ICM42607: {
#ifdef BSP_MULTI_AXIS_SENSOR_TDK42607
                icm42607_eint_irq_handle(handle);
#endif
            }break;
        case LSM6DSOX: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSOX
                lsm6dsox_eint_irq_handle(handle);
#endif
            } break;
        case QMI8658: {
#ifdef  BSP_MULTI_AXIS_SENSOR_QMI8658
                qmi8658_eint_irq_handle(handle);
#endif
            }break;

        default: {} break;
    }

}


bsp_multi_axis_handle_t    bsp_multi_axis_init(uint32_t vendor, bsp_multi_axis_init_config_t *config)
{
    uint32_t handle = 0;
	int      ret = BSP_MULTI_AXIS_INVALID_PARA;

    if (config == NULL) {
        return BSP_MULTI_AXIS_INVALID_PARA;
    }
    /*set handle*/
    BSP_SENSOR_SET_MAGICID(handle,  BSP_SENSOR_MAGIC_ID);
    BSP_SENSOR_SET_VENDOR(handle,   vendor);
    BSP_SENSOR_SET_BUS_TYPE(handle, config->bus_type);
    BSP_SENSOR_SET_BUS_PORT(handle, config->bus_port);
    BSP_SENSOR_SET_EINT_NUM(handle, config->eint_num);

    if (config->call_back != 0) {
        hal_eint_init(config->eint_num, &config->eint_config);
        hal_eint_register_callback(config->eint_num, bsp_axis_sensor_eintx_handler, (void *)handle);
        hal_eint_unmask(config->eint_num);
    }
    switch (vendor) {
        case LSM6DS3:
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DS3
            ret = lsm6ds3_init(handle, config);
#endif
            break;
        case BMI160:
#ifdef BSP_MULTI_AXIS_SENSOR_BMI160
            ret = bmi160_init(handle, config);
#endif
            break;
        case LSM6DSO:
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSO
			ret = lsm6dso_init(handle, config);
#endif
			break;
        case ICM42607:
#ifdef BSP_MULTI_AXIS_SENSOR_TDK42607
            ret = icm42607_init(handle, config);
#endif
            break;

        case LSM6DSOX:
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSOX
            ret = lsm6dsox_init(handle, config);
#endif
            break;

        case QMI8658: {
#ifdef  BSP_MULTI_AXIS_SENSOR_QMI8658
                ret = qmi8658_init(handle, config);
#endif
            }break;

        default: {
            log_axis_sensor_error("[bsp][axis] bsp_multi_axis_init() fail: vendor invalid(%d)", 1, (unsigned int)vendor);
        }
        break;
    }
	if(ret != BSP_MULTI_AXIS_OK){
		log_axis_sensor_error("[bsp][axis] bsp_multi_axis_init() fail: sensor init error(%d)", 1, (unsigned int) ret);
		return 0;
	}
    return (bsp_multi_axis_handle_t)handle;
}


int    bsp_multi_axis_deinit(bsp_multi_axis_handle_t handle)
{
    int status = BSP_MULTI_AXIS_INVALID_PARA;

    if (BSP_SENSOR_GET_MAGICID(handle) != BSP_SENSOR_MAGIC_ID) {
        return BSP_MULTI_AXIS_INVALID_HANDLE;
    }

    hal_eint_mask(BSP_SENSOR_GET_EINT_NUM(handle));

    switch (BSP_SENSOR_GET_VENDOR(handle)) {
        case BMI160:
#ifdef BSP_MULTI_AXIS_SENSOR_BMI160
            status = bmi160_deinit(handle);
#endif
            break;

        case BMI088: break;

        case LSM6DS3: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DS3
            status = lsm6ds3_deinit(handle);
#endif
            } break;

        case LSM6DSO: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSO
			status = lsm6dso_deinit(handle);
#endif
			} break;

        case ICM42607: {
#ifdef BSP_MULTI_AXIS_SENSOR_TDK42607
            icm42607_deinit(handle);
#endif
            } break;

        case LSM6DSOX: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSOX
            status = lsm6dsox_deinit(handle);
#endif
            } break;
        case QMI8658: {
#ifdef  BSP_MULTI_AXIS_SENSOR_QMI8658
            status = qmi8658_deinit(handle);
#endif
            }break;
        default: {
            log_axis_sensor_error("[bsp][axis] bsp_multi_axis_get_frame_cnt() fail: vendor invalid(%d)", 1, (unsigned int)BSP_SENSOR_GET_VENDOR(handle));
        }
        break;
    }
    return status;
}


int    bsp_multi_axis_config(bsp_multi_axis_handle_t handle, bsp_multi_axis_config_t *config)
{
	int ret = BSP_MULTI_AXIS_INVALID_PARA;
	uint8_t eint_num;

    if (BSP_SENSOR_GET_MAGICID(handle) != BSP_SENSOR_MAGIC_ID) {
        return BSP_MULTI_AXIS_INVALID_HANDLE;
    }
    if (config == NULL || config->enable == 0 || (config->enable & BSP_MULTI_AXIS_DISABLE_ALL) ||
        config->frame_threshold == 0) {
		log_axis_sensor_error("[bsp][axis] bsp_multi_axis_config() fail: para invalid(%d)", 1, (unsigned int) ret);
        return BSP_MULTI_AXIS_INVALID_PARA;
    }
	eint_num = BSP_SENSOR_GET_EINT_NUM(handle);
    hal_eint_unmask(eint_num);
	log_axis_sensor_warn("[bsp][axis] bsp_multi_axis_config(): unmask eint(%d)\r\n",1,  eint_num);
    
    switch (BSP_SENSOR_GET_VENDOR(handle)) {
        case LSM6DS3: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DS3
                if (config->enable & BSP_MULTI_AXIS_ACCEL_ENABLE) {
                    ret = lsm6ds3_set_xl_full_scale(config->accel_range);
                }
                if (config->enable & BSP_MULTI_AXIS_GYRO_ENABLE) {
                    ret = lsm6ds3_set_gy_full_scale(config->gyro_range);
                }
                if (config->enable != 0) {
                    ret = lsm6ds3_set_data_output_rate(config->sensor_odr, config->frame_threshold);
                }
#endif
            } break;

        case BMI160: {
#ifdef BSP_MULTI_AXIS_SENSOR_BMI160
            ret = bmi160_config(handle, config);
#endif
            } break;

        case BMI088: {
            } break;
        
		case LSM6DSO:{
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSO
			ret = lsm6dso_config(handle, config);
#endif
		    } break;

        case ICM42607: {
#ifdef BSP_MULTI_AXIS_SENSOR_TDK42607
                ret = icm42607_config(handle, config);
#endif
            }break;

        case LSM6DSOX:{
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSOX
            ret = lsm6dsox_config(handle, config);
#endif
            } break;
        case QMI8658: {
#ifdef  BSP_MULTI_AXIS_SENSOR_QMI8658
                ret = qmi8658_config(handle, config);
#endif
            }break;

        default: {
            log_axis_sensor_error("[bsp][axis] bsp_multi_axis_config() fail: vendor invalid(%d)", 1, (unsigned int)BSP_SENSOR_GET_VENDOR(handle));
        }
        break;
    }
	if(ret != BSP_MULTI_AXIS_OK) {
		log_axis_sensor_error("[bsp][axis] bsp_multi_axis_config() fail: config error(%d)", 1, (unsigned int) ret);
		return ret;
	}    

    return ret;
}


int bsp_multi_axis_get_frame_cnt(bsp_multi_axis_handle_t handle)
{
    int frame_cnt = 0;

    if (BSP_SENSOR_GET_MAGICID(handle) != BSP_SENSOR_MAGIC_ID) {
        return BSP_MULTI_AXIS_INVALID_HANDLE;
    }
    switch (BSP_SENSOR_GET_VENDOR(handle)) {
        case BMI160: {
#ifdef BSP_MULTI_AXIS_SENSOR_BMI160
                frame_cnt = bmi160_get_sw_fifo_available_frame_cnt();
#endif
            } break;
        case BMI088: {
            } break;
        case LSM6DS3: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DS3
                frame_cnt = lsm6ds3_get_frame_cnt();
#endif
            } break;
        case LSM6DSO: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSO
                frame_cnt = lsm6dso_get_frame_cnt();
#endif
            } break;
        case ICM42607: {
#ifdef BSP_MULTI_AXIS_SENSOR_TDK42607
                frame_cnt = icm42607_get_frame_cnt(handle);
#endif
            }break;
        
        case LSM6DSOX: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSOX
                frame_cnt = lsm6dsox_get_frame_cnt();
#endif
            } break;

        case QMI8658: {
#ifdef  BSP_MULTI_AXIS_SENSOR_QMI8658
                frame_cnt = qmi8658_get_frame_cnt();
#endif
            }break;
                
        default: {
            log_axis_sensor_error("[bsp][axis] bsp_multi_axis_get_frame_cnt() fail: vendor invalid(%d)", 1, (unsigned int)BSP_SENSOR_GET_VENDOR(handle));
        }
        break;
    }
    return frame_cnt;
}





int bsp_multi_axis_self_test(bsp_multi_axis_handle_t handle, bsp_multi_axis_enable_t sensor_en)
{
    bsp_multi_axis_status_t ret = BSP_MULTI_AXIS_INVALID_PARA;

    if (BSP_SENSOR_GET_MAGICID(handle) != BSP_SENSOR_MAGIC_ID) {
        return BSP_MULTI_AXIS_INVALID_HANDLE;
    }
    switch (BSP_SENSOR_GET_VENDOR(handle)) {
        case BMI160:
#ifdef BSP_MULTI_AXIS_SENSOR_BMI160
            if (sensor_en & BSP_MULTI_AXIS_ACCEL_ENABLE) {
                /* Start accel self test and get the result */
                ret = bmi160_accel_selftest();
                if (ret != BSP_MULTI_AXIS_OK) {
                    return ret;
                }
            }
            if (sensor_en & BSP_MULTI_AXIS_GYRO_ENABLE) {
                /* Start gyro test and get the result */
                ret = bmi160_gyro_selftest();
                if (ret != BSP_MULTI_AXIS_OK) {
                    return ret;
                }
            }
#endif            
        break;
        case BMI088: {
        } break;

        case LSM6DS3: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DS3
            ret = lsm6ds3_self_test(sensor_en);
#endif
        } break;

        case LSM6DSO: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSO
            ret = lsm6dso_self_test(sensor_en);
#endif
        } break;

        case ICM42607: {
#ifdef BSP_MULTI_AXIS_SENSOR_TDK42607
#endif
        } break;

        case LSM6DSOX: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSOX
            ret = lsm6dsox_self_test(sensor_en);
#endif
        } break;

        case QMI8658: {
#ifdef  BSP_MULTI_AXIS_SENSOR_QMI8658

#endif
            }break;
        default: {
            log_axis_sensor_error("[bsp][axis] bsp_multi_axis_self_test() fail: vendor invalid(%d)", 1, (unsigned int)BSP_SENSOR_GET_VENDOR(handle));
        }
        break;
    }
    return ret;

}

uint32_t bsp_multi_axis_get_data(bsp_multi_axis_handle_t handle, bsp_multi_axis_data_t *frame_buff, uint32_t frame_cnt)
{
	uint8_t eint_num;

    if (BSP_SENSOR_GET_MAGICID(handle) != BSP_SENSOR_MAGIC_ID) {
		log_axis_sensor_error("[bsp][axis] bsp_multi_axis_get_data() fail: invalid handle", 0);
        return 0;
    }
    if (frame_buff == NULL || frame_cnt == 0) {
		log_axis_sensor_error("[bsp][axis] bsp_multi_axis_get_data() fail: invalid para", 0);
        return 0;
    }

    eint_num = BSP_SENSOR_GET_EINT_NUM(handle);
    switch (BSP_SENSOR_GET_VENDOR(handle)) {
        case BMI160: {            
#ifdef BSP_MULTI_AXIS_SENSOR_BMI160
                frame_cnt = bmi160_read_format_data(frame_buff, frame_cnt);
#endif
            } break;
        case BMI088: {
            } break;
        case LSM6DS3: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DS3
                frame_cnt = lsm6ds3_read_format_data(frame_buff, frame_cnt);
#endif
            } break;

        case LSM6DSO: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSO
                frame_cnt = lsm6dso_read_format_data(frame_buff, frame_cnt);
#endif
            } break;

        case ICM42607: {
#ifdef BSP_MULTI_AXIS_SENSOR_TDK42607
                frame_cnt = icm42607_read_format_data(handle, frame_buff, frame_cnt);
#endif
            }break;

        case LSM6DSOX: {
#ifdef BSP_MULTI_AXIS_SENSOR_LSM6DSOX
                frame_cnt = lsm6dsox_read_format_data(frame_buff, frame_cnt);
#endif
            } break;

/* handle sensor: QMI8658 */
        case QMI8658: {
#ifdef  BSP_MULTI_AXIS_SENSOR_QMI8658
                frame_cnt = qmi8658_read_format_data(frame_buff, frame_cnt);
#endif
            }break;

        default: {
            log_axis_sensor_error("[bsp][axis] bsp_multi_axis_get_data() fail: vendor invalid(%d)", 1, (unsigned int)BSP_SENSOR_GET_VENDOR(handle));
        } break;
    }
    hal_eint_unmask(eint_num);
	//log_axis_sensor_error("[bsp][axis] bsp_multi_axis_get_data(): unmask eint(%d)\r\n",1,  eint_num);
    return frame_cnt;
}


