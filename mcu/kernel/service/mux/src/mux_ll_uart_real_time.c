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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <assert.h>
#include "mux.h"
#include "mux_port.h"
#include "mux_port_device.h"
#include "mux_ll_uart.h"
#include "hal_resource_assignment.h"
#include "hal_pdma_internal.h"
#include "hal_uart_internal.h"

#include "mux_ll_uart_real_time.h"

//#define LLUART_REAL_TIME_MEASUREMENT

#ifdef LLUART_REAL_TIME_MEASUREMENT

#define TEST_ONE_FRAME_TIME 5000 //ms
uint32_t test_one_frame_tx_gpt_trigger_handle;
uint32_t timer_flag = 0;

volatile uint32_t g_lluart_busy_rate_flag = 0;  //0: not start Measurement. 1: UART busy.   2: UART IDLE
uint32_t g_lluart_busy_rate_last_time = 0;
uint32_t g_lluart_busy_rate_busy_time = 0;
uint32_t g_lluart_busy_rate_idle_time = 0;


/**************************************uart busy rate Measurement ****************************/
                        /*LLUART_ACTION_TX_LIMTER*/                          /*LLUART_ACTION_DMA_EMPTY_IRQ*/                    /*LLUART_ACTION_STOP*/
// lluart_busy_rate_table_note_t lluart_lookup_table[3][3] = {
// /*LLUART_STA_NOT_START*/{{LLUART_STA_RUNNING,&g_lluart_busy_rate_idle_time}, {LLUART_STA_EMPTY,NULL},                           {LLUART_STA_NOT_START,NULL}},
// /*LLUART_STA_RUNNING*/  {{LLUART_STA_RUNNING,NULL},                          {LLUART_STA_EMPTY,&g_lluart_busy_rate_busy_time},  {LLUART_STA_NOT_START,&g_lluart_busy_rate_busy_time}},
// /*LLUART_STA_EMPTY*/    {{LLUART_STA_RUNNING,&g_lluart_busy_rate_idle_time}, {LLUART_STA_EMPTY,NULL},                           {LLUART_STA_NOT_START,&g_lluart_busy_rate_idle_time}}
// };

// //lluart_busy_time_measurement_action(LLUART_ACTION_TX_LIMTER);
// //lluart_busy_time_measurement_action(LLUART_ACTION_DMA_EMPTY_IRQ);
// //lluart_busy_time_measurement_action(LLUART_ACTION_STOP);
// void lluart_busy_time_measurement_action(lluart_busy_rate_action_t action)
// {
//     uint32_t during,temp;
//     lluart_busy_rate_table_note_t *p = &lluart_lookup_table[g_lluart_busy_rate_flag][action];
//     g_lluart_busy_rate_flag = p->sta;
//     if(p->p_time != NULL)
//     {
//         hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &temp); 
//         hal_gpt_get_duration_count(g_lluart_busy_rate_last_time,temp,&during);
//         *p->p_time+=during;
//         g_lluart_busy_rate_last_time=temp;
//     }
// }

                      /* g_lluart_busy_rate_idle_time */   /* g_lluart_busy_rate_busy_time */
uint32_t g_lluart_busy_rate_ararry[2] = {0,                           0};

                        /*LLUART_ACTION_TX_LIMTER*/                          /*LLUART_ACTION_DMA_EMPTY_IRQ*/                    /*LLUART_ACTION_STOP*/
lluart_busy_rate_table_note_t lluart_lookup_table[3][3] = {
/*LLUART_STA_NOT_START*/{{LLUART_STA_RUNNING,LLUART_BUSY_RATE_IDLE_TIME_IDX}, {LLUART_STA_EMPTY,0xff},                           {LLUART_STA_NOT_START,0xff}},
/*LLUART_STA_RUNNING*/  {{LLUART_STA_RUNNING,0xff},                           {LLUART_STA_EMPTY,LLUART_BUSY_RATE_BUSY_TIME_IDX}, {LLUART_STA_NOT_START,LLUART_BUSY_RATE_BUSY_TIME_IDX}},
/*LLUART_STA_EMPTY*/    {{LLUART_STA_RUNNING,LLUART_BUSY_RATE_IDLE_TIME_IDX}, {LLUART_STA_EMPTY,0xff},                           {LLUART_STA_NOT_START,LLUART_BUSY_RATE_IDLE_TIME_IDX}}
};

//lluart_busy_time_measurement_action(LLUART_ACTION_TX_LIMTER);
//lluart_busy_time_measurement_action(LLUART_ACTION_DMA_EMPTY_IRQ);
//lluart_busy_time_measurement_action(LLUART_ACTION_STOP);
void lluart_busy_time_measurement_action(lluart_busy_rate_action_t action)
{
    uint32_t during,temp;
    lluart_busy_rate_table_note_t *p = &lluart_lookup_table[g_lluart_busy_rate_flag][action];
    g_lluart_busy_rate_flag = p->sta;
    if(p->time_idx != 0xff)
    {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &temp); 
        hal_gpt_get_duration_count(g_lluart_busy_rate_last_time,temp,&during);
        g_lluart_busy_rate_ararry[p->time_idx]+=during;
        g_lluart_busy_rate_last_time=temp;
    }
}

void uart_busy_rate_timer_cb(void *user_data)
{
    PORT_MUX_UNUSED(user_data);
    lluart_busy_time_measurement_action(LLUART_ACTION_STOP);

    g_lluart_busy_rate_idle_time = g_lluart_busy_rate_ararry[LLUART_BUSY_RATE_IDLE_TIME_IDX];
    g_lluart_busy_rate_busy_time = g_lluart_busy_rate_ararry[LLUART_BUSY_RATE_BUSY_TIME_IDX];

    LOG_MSGID_E(MUX_PORT,"[MCU Real time measurment]uart busy rate:%d,idle time:%dus,busy time:%dus",3,\
                    100*g_lluart_busy_rate_busy_time/(g_lluart_busy_rate_busy_time+g_lluart_busy_rate_idle_time),
                    g_lluart_busy_rate_idle_time,
                    g_lluart_busy_rate_busy_time
                );

    //for next measurement stage start
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &g_lluart_busy_rate_last_time);
    g_lluart_busy_rate_busy_time = 0;
    g_lluart_busy_rate_idle_time = 0;
    g_lluart_busy_rate_ararry[LLUART_BUSY_RATE_IDLE_TIME_IDX] = 0;
    g_lluart_busy_rate_ararry[LLUART_BUSY_RATE_BUSY_TIME_IDX] = 0;


    lluart_busy_time_measurement_action(LLUART_ACTION_TX_LIMTER);
    hal_gpt_sw_start_timer_ms(test_one_frame_tx_gpt_trigger_handle,TEST_ONE_FRAME_TIME,uart_busy_rate_timer_cb,0);
}

void uart_busy_rate_timer_start(void)
{
    if(timer_flag ==0){
        hal_gpt_sw_get_timer(&test_one_frame_tx_gpt_trigger_handle);
        hal_gpt_sw_start_timer_ms(test_one_frame_tx_gpt_trigger_handle,TEST_ONE_FRAME_TIME,uart_busy_rate_timer_cb,0);
        timer_flag = 1;
    }
}

/**************************************cpu mips Measurement ****************************/
volatile uint32_t time_start;
volatile uint32_t g_lluart_tx_cpu_time = 0;
volatile uint32_t g_lluart_rx_cpu_time = 0;
volatile uint32_t g_lluart_test_counter = 0;
volatile uint32_t g_lluart_tx_cpu_time_total = 0;
volatile uint32_t g_lluart_rx_cpu_time_total = 0;
volatile uint32_t g_lluart_tx_cpu_time_average = 0;
volatile uint32_t g_lluart_rx_cpu_time_average = 0;

bool is_tx = false;

void cpu_mips_measurement_start(void)  //tx:sw irq;   rx: uart dma callback;    
{
    time_start = mux_get_tick_count();
}

void cpu_mips_measurement_end(bool is_tx)
{
    if(is_tx == true){
        g_lluart_tx_cpu_time += mux_get_tick_elapse(time_start);
    } else {
        g_lluart_rx_cpu_time += mux_get_tick_elapse(time_start);
    }
    
}

void cpu_mips_measurement_timer_cb(void *user_data)
{
    PORT_MUX_UNUSED(user_data);

    g_lluart_test_counter++;
    g_lluart_tx_cpu_time_total+=g_lluart_tx_cpu_time;
    g_lluart_rx_cpu_time_total+=g_lluart_rx_cpu_time;
    g_lluart_tx_cpu_time_average = g_lluart_tx_cpu_time_total/g_lluart_test_counter;
    g_lluart_rx_cpu_time_average = g_lluart_rx_cpu_time_total/g_lluart_test_counter;

    LOG_MSGID_E(MUX_PORT,"[MCU Real time measurment]cpu tx time:%d(%d)us,cpu rx time:%d(%d)us",4,\
                    g_lluart_tx_cpu_time,
                    g_lluart_tx_cpu_time_average,
                    g_lluart_rx_cpu_time,
                    g_lluart_rx_cpu_time_average
                );

        g_lluart_tx_cpu_time = 0;
        g_lluart_rx_cpu_time = 0;


    hal_gpt_sw_start_timer_ms(test_one_frame_tx_gpt_trigger_handle,TEST_ONE_FRAME_TIME,cpu_mips_measurement_timer_cb,0);
}

void cpu_mips_measurement_timer_start(void)
{
    if(timer_flag ==0){
        hal_gpt_sw_get_timer(&test_one_frame_tx_gpt_trigger_handle);
        hal_gpt_sw_start_timer_ms(test_one_frame_tx_gpt_trigger_handle,TEST_ONE_FRAME_TIME,cpu_mips_measurement_timer_cb,0);
        timer_flag = 1;
    }
}


#endif
