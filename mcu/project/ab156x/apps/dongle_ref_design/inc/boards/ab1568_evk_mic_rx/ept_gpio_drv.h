/*
 * Generated by Airoha Easy PinMux Tool Version 2.8.1 for AB1568. Copyright Airoha Inc. (C) 2015.
 * 2022-07-26 11:25:06:0500
 * Do Not Modify the File.
 */

/*****************************************************************************
*
* Filename:
* ---------
*    ***.*
*
* Project:
* --------
*
* Description:
* ------------
*
* Author:
* -------
*
*============================================================================
****************************************************************************/

#ifndef _EPT_GPIO_DRV_H
#define _EPT_GPIO_DRV_H

#define MODE_0  0
#define MODE_1  1
#define MODE_2  2
#define MODE_3  3
#define MODE_4  4
#define MODE_5  5
#define MODE_6  6
#define MODE_7  7
#define MODE_8  8
#define MODE_9  9
#define MODE_NC  0

#define PULL_ENABLE  1
#define PULL_DISABLE  0
#define DIR_INPUT  0
#define DIR_OUTPUT  1
#define GPIO_PORTNULL_MODE  0
#define GPIO_PORTNULL_DIR   0
#define GPIO_PORTNULL_OUTPUT_LEVEL  0
#define GPIO_PORTNULL_PU        0
#define GPIO_PORTNULL_PD        0
#define GPIO_PORTNULL_R0    0
#define GPIO_PORTNULL_R1    0
#define GPIO_PORTNULL_PUPD  0
#define GPIO_PORTNULL_DOUT  0
#define GPIO_PORT0_MODE   MODE_1 // 1:UART0_TXD : Used for UART0_TXD
#define GPIO_PORT1_MODE   MODE_1 // 1:UART0_RXD : Used for UART0_RXD
#define GPIO_PORT2_MODE   MODE_9 // 9:EINT2 : Used for EINT2
#define GPIO_PORT3_MODE   MODE_8 // 8:CTP1 : Used for CTP1
#define GPIO_PORT4_MODE   MODE_2 // 2:UART1_TXD : Used for UART1_TXD
#define GPIO_PORT5_MODE   MODE_2 // 2:UART1_RXD : Used for UART1_RXD
#define GPIO_PORT6_MODE   MODE_3 // 3:UART1_CTS : Used for UART1_CTS
#define GPIO_PORT7_MODE   MODE_3 // 3:UART1_RTS : Used for UART1_RTS
#define GPIO_PORT8_MODE   MODE_9 // 9:EINT8 : Used for EINT8
#define GPIO_PORT9_MODE   MODE_9 // 9:EINT9 : Used for EINT9
#define GPIO_PORT10_MODE   MODE_9 // 9:EINT10 : Used for EINT10
#define GPIO_PORT11_MODE   MODE_9 // 9:EINT11 : Used for EINT11
#define GPIO_PORT12_MODE   MODE_0 // 0:GPIO12 : Used for GPIO12
#define GPIO_PORT13_MODE   MODE_9 // 9:EINT13 : Used for EINT13
#define GPIO_PORT14_MODE   MODE_0 // 0:GPIO14 : Used for GPIO14
#define GPIO_PORT15_MODE   MODE_2 // 2:ESC_MOSI : Used for ESC_MOSI
#define GPIO_PORT16_MODE   MODE_2 // 2:ESC_MISO : Used for ESC_MISO
#define GPIO_PORT17_MODE   MODE_2 // 2:ESC_SIO2 : Used for ESC_SIO2
#define GPIO_PORT18_MODE   MODE_8 // 8:UART2_TXD : Used for UART2_TXD
#define GPIO_PORT19_MODE   MODE_8 // 8:UART2_RXD : Used for UART2_RXD
#define GPIO_PORT20_MODE   MODE_7 // 7:AP_JTMS : Used for AP_JTMS
#define GPIO_PORT21_MODE   MODE_7 // 7:AP_JTCK : Used for AP_JTCK
#define GPIO_PORT22_MODE   MODE_7 // 7:DSP_JTMS : Used for DSP_JTMS
#define GPIO_PORT23_MODE   MODE_3 // 3:I2S_SLV2_WS : Used for I2S_SLV2_WS
#define GPIO_PORT24_MODE   MODE_3 // 3:I2S_SLV2_CK : Used for I2S_SLV2_CK
#define GPIO_PORT25_MODE   MODE_3 // 3:I2S_SLV2_RX : Used for I2S_SLV2_RX
#define GPIO_PORT26_MODE   MODE_3 // 3:I2S_SLV2_TX : Used for I2S_SLV2_TX
#define GPIO_PORT27_MODE   MODE_0 // 0:GPIO27 : No Pin
#define GPIO_PORT28_MODE   MODE_0 // 0:GPIO28 : No Pin
#define GPIO_PORT29_MODE   MODE_0 // 0:GPIO29 : No Pin
#define GPIO_PORT30_MODE   MODE_0 // 0:GPIO30 : No Pin
#define GPIO_PORT31_MODE   MODE_0 // 0:GPIO31 : No Pin
#define GPIO_PORT32_MODE   MODE_0 // 0:GPIO32 : No Pin
#define GPIO_PORT33_MODE   MODE_0 // 0:GPIO33 : No Pin
#define GPIO_PORT34_MODE   MODE_0 // 0:GPIO34 : No Pin
#define GPIO_PORT35_MODE   MODE_0 // 0:GPIO35 : No Pin
#define GPIO_PORT36_MODE   MODE_0 // 0:GPIO36 : No Pin
#define GPIO_PORT37_MODE   MODE_0 // 0:GPIO37 : No Pin
#define GPIO_PORT38_MODE   MODE_0 // 0:GPIO38 : No Pin
#define GPIO_PORT39_MODE   MODE_0 // 0:GPIO39 : No Pin
#define RTC_GPIO_PORT0_MODE   MODE_2 // 2:CTP0 : Used for CTP0
#define RTC_GPIO_PORT1_MODE   MODE_NC
#define RTC_GPIO_PORT2_MODE   MODE_NC


#define GPIO_PORT0_DIR   DIR_INPUT
#define GPIO_PORT1_DIR   DIR_INPUT
#define GPIO_PORT2_DIR   DIR_INPUT
#define GPIO_PORT3_DIR   DIR_INPUT
#define GPIO_PORT4_DIR   DIR_INPUT
#define GPIO_PORT5_DIR   DIR_INPUT
#define GPIO_PORT6_DIR   DIR_INPUT
#define GPIO_PORT7_DIR   DIR_INPUT
#define GPIO_PORT8_DIR   DIR_INPUT
#define GPIO_PORT9_DIR   DIR_INPUT
#define GPIO_PORT10_DIR   DIR_INPUT
#define GPIO_PORT11_DIR   DIR_INPUT
#define GPIO_PORT12_DIR   DIR_INPUT
#define GPIO_PORT13_DIR   DIR_INPUT
#define GPIO_PORT14_DIR   DIR_OUTPUT
#define GPIO_PORT15_DIR   DIR_INPUT
#define GPIO_PORT16_DIR   DIR_INPUT
#define GPIO_PORT17_DIR   DIR_INPUT
#define GPIO_PORT18_DIR   DIR_INPUT
#define GPIO_PORT19_DIR   DIR_INPUT
#define GPIO_PORT20_DIR   DIR_INPUT
#define GPIO_PORT21_DIR   DIR_INPUT
#define GPIO_PORT22_DIR   DIR_INPUT
#define GPIO_PORT23_DIR   DIR_INPUT
#define GPIO_PORT24_DIR   DIR_INPUT
#define GPIO_PORT25_DIR   DIR_INPUT
#define GPIO_PORT26_DIR   DIR_INPUT
#define GPIO_PORT27_DIR   DIR_INPUT
#define GPIO_PORT28_DIR   DIR_INPUT
#define GPIO_PORT29_DIR   DIR_INPUT
#define GPIO_PORT30_DIR   DIR_INPUT
#define GPIO_PORT31_DIR   DIR_INPUT
#define GPIO_PORT32_DIR   DIR_INPUT
#define GPIO_PORT33_DIR   DIR_INPUT
#define GPIO_PORT34_DIR   DIR_INPUT
#define GPIO_PORT35_DIR   DIR_INPUT
#define GPIO_PORT36_DIR   DIR_INPUT
#define GPIO_PORT37_DIR   DIR_INPUT
#define GPIO_PORT38_DIR   DIR_INPUT
#define GPIO_PORT39_DIR   DIR_INPUT
#define RTC_GPIO_PORT0_DIR   DIR_INPUT
#define RTC_GPIO_PORT1_DIR   DIR_INPUT
#define RTC_GPIO_PORT2_DIR   DIR_INPUT


#define GPIO_PORT0_OUTPUT_LEVEL   0
#define GPIO_PORT1_OUTPUT_LEVEL   0
#define GPIO_PORT2_OUTPUT_LEVEL   0
#define GPIO_PORT3_OUTPUT_LEVEL   0
#define GPIO_PORT4_OUTPUT_LEVEL   0
#define GPIO_PORT5_OUTPUT_LEVEL   0
#define GPIO_PORT6_OUTPUT_LEVEL   0
#define GPIO_PORT7_OUTPUT_LEVEL   0
#define GPIO_PORT8_OUTPUT_LEVEL   0
#define GPIO_PORT9_OUTPUT_LEVEL   0
#define GPIO_PORT10_OUTPUT_LEVEL   0
#define GPIO_PORT11_OUTPUT_LEVEL   0
#define GPIO_PORT12_OUTPUT_LEVEL   0
#define GPIO_PORT13_OUTPUT_LEVEL   0
#define GPIO_PORT14_OUTPUT_LEVEL   1
#define GPIO_PORT15_OUTPUT_LEVEL   0
#define GPIO_PORT16_OUTPUT_LEVEL   0
#define GPIO_PORT17_OUTPUT_LEVEL   0
#define GPIO_PORT18_OUTPUT_LEVEL   0
#define GPIO_PORT19_OUTPUT_LEVEL   0
#define GPIO_PORT20_OUTPUT_LEVEL   0
#define GPIO_PORT21_OUTPUT_LEVEL   0
#define GPIO_PORT22_OUTPUT_LEVEL   0
#define GPIO_PORT23_OUTPUT_LEVEL   0
#define GPIO_PORT24_OUTPUT_LEVEL   0
#define GPIO_PORT25_OUTPUT_LEVEL   0
#define GPIO_PORT26_OUTPUT_LEVEL   0
#define GPIO_PORT27_OUTPUT_LEVEL   0
#define GPIO_PORT28_OUTPUT_LEVEL   0
#define GPIO_PORT29_OUTPUT_LEVEL   0
#define GPIO_PORT30_OUTPUT_LEVEL   0
#define GPIO_PORT31_OUTPUT_LEVEL   0
#define GPIO_PORT32_OUTPUT_LEVEL   0
#define GPIO_PORT33_OUTPUT_LEVEL   0
#define GPIO_PORT34_OUTPUT_LEVEL   0
#define GPIO_PORT35_OUTPUT_LEVEL   0
#define GPIO_PORT36_OUTPUT_LEVEL   0
#define GPIO_PORT37_OUTPUT_LEVEL   0
#define GPIO_PORT38_OUTPUT_LEVEL   0
#define GPIO_PORT39_OUTPUT_LEVEL   0
#define RTC_GPIO_PORT0_OUTPUT_LEVEL   0
#define RTC_GPIO_PORT1_OUTPUT_LEVEL   0
#define RTC_GPIO_PORT2_OUTPUT_LEVEL   0


#define GPIO_PORT0_PU   0
#define GPIO_PORT1_PU   1
#define GPIO_PORT2_PU   0
#define GPIO_PORT3_PU   0
#define GPIO_PORT4_PU   0
#define GPIO_PORT5_PU   1
#define GPIO_PORT6_PU   1
#define GPIO_PORT7_PU   0
#define GPIO_PORT8_PU   1
#define GPIO_PORT9_PU   1
#define GPIO_PORT10_PU   1
#define GPIO_PORT11_PU   1
#define GPIO_PORT12_PU   1
#define GPIO_PORT13_PU   0
#define GPIO_PORT14_PU   0
#define GPIO_PORT15_PU   0
#define GPIO_PORT16_PU   0
#define GPIO_PORT17_PU   0
#define GPIO_PORT18_PU   0
#define GPIO_PORT19_PU   0
#define GPIO_PORT20_PU   1
#define GPIO_PORT21_PU   1
#define GPIO_PORT22_PU   1
#define GPIO_PORT23_PU   0
#define GPIO_PORT24_PU   0
#define GPIO_PORT25_PU   0
#define GPIO_PORT26_PU   0
#define GPIO_PORT27_PU   0
#define GPIO_PORT28_PU   0
#define GPIO_PORT29_PU   0
#define GPIO_PORT30_PU   0
#define GPIO_PORT31_PU   0
#define GPIO_PORT32_PU   0
#define GPIO_PORT33_PU   0
#define GPIO_PORT34_PU   0
#define GPIO_PORT35_PU   0
#define GPIO_PORT36_PU   0
#define GPIO_PORT37_PU   0
#define GPIO_PORT38_PU   0
#define GPIO_PORT39_PU   0
#define RTC_GPIO_PORT0_PU   0
#define RTC_GPIO_PORT1_PU   0
#define RTC_GPIO_PORT2_PU   0


#define GPIO_PORT0_PD   0
#define GPIO_PORT1_PD   0
#define GPIO_PORT2_PD   1
#define GPIO_PORT3_PD   0
#define GPIO_PORT4_PD   0
#define GPIO_PORT5_PD   0
#define GPIO_PORT6_PD   0
#define GPIO_PORT7_PD   0
#define GPIO_PORT8_PD   0
#define GPIO_PORT9_PD   0
#define GPIO_PORT10_PD   0
#define GPIO_PORT11_PD   0
#define GPIO_PORT12_PD   0
#define GPIO_PORT13_PD   0
#define GPIO_PORT14_PD   0
#define GPIO_PORT15_PD   0
#define GPIO_PORT16_PD   0
#define GPIO_PORT17_PD   0
#define GPIO_PORT18_PD   0
#define GPIO_PORT19_PD   0
#define GPIO_PORT20_PD   0
#define GPIO_PORT21_PD   0
#define GPIO_PORT22_PD   0
#define GPIO_PORT23_PD   0
#define GPIO_PORT24_PD   0
#define GPIO_PORT25_PD   0
#define GPIO_PORT26_PD   0
#define GPIO_PORT27_PD   1
#define GPIO_PORT28_PD   1
#define GPIO_PORT29_PD   1
#define GPIO_PORT30_PD   1
#define GPIO_PORT31_PD   1
#define GPIO_PORT32_PD   1
#define GPIO_PORT33_PD   1
#define GPIO_PORT34_PD   1
#define GPIO_PORT35_PD   1
#define GPIO_PORT36_PD   1
#define GPIO_PORT37_PD   0
#define GPIO_PORT38_PD   0
#define GPIO_PORT39_PD   1
#define RTC_GPIO_PORT0_PD   1
#define RTC_GPIO_PORT1_PD   1
#define RTC_GPIO_PORT2_PD   1


// PUPD1  PU-100K_ohms
#define GPIO_PORT13_PUPD  0
#define GPIO_PORT13_R1  0
#define GPIO_PORT13_R0  1
// PUPD1  Disable_pull
#define GPIO_PORT14_PUPD  0
#define GPIO_PORT14_R1  0
#define GPIO_PORT14_R0  0
// PUPD1  PD-120K_ohms
#define GPIO_PORT15_PUPD  1
#define GPIO_PORT15_R1  0
#define GPIO_PORT15_R0  1
// PUPD1  PD-120K_ohms
#define GPIO_PORT16_PUPD  1
#define GPIO_PORT16_R1  0
#define GPIO_PORT16_R0  1
// PUPD1  PD-120K_ohms
#define GPIO_PORT17_PUPD  1
#define GPIO_PORT17_R1  0
#define GPIO_PORT17_R0  1
// PUPD1  Disable_pull
#define GPIO_PORT18_PUPD  0
#define GPIO_PORT18_R1  0
#define GPIO_PORT18_R0  0
// PUPD1  PU-100K_ohms
#define GPIO_PORT19_PUPD  0
#define GPIO_PORT19_R1  0
#define GPIO_PORT19_R0  1
// PUPD1  PD-120K_ohms
#define GPIO_PORT37_PUPD  1
#define GPIO_PORT37_R1  0
#define GPIO_PORT37_R0  1
// PUPD1  PD-120K_ohms
#define GPIO_PORT38_PUPD  1
#define GPIO_PORT38_R1  0
#define GPIO_PORT38_R0  1


#define EPT_GPIO_PIN_MASK_1   0xff
#define EPT_GPIO_PIN_MASK_0   0xffffffff


#define GPIO_VOLTAGE_TYPE_0 0x00000000
#define GPIO_VOLTAGE_TYPE_1 0x00000000
#define GPIO_VOLTAGE_TYPE_2 0x00000000
#define GPIO_VOLTAGE_TYPE_3 0x00000000
#define GPIO_VOLTAGE_TYPE_4 0x00000000
#define GPIO_VOLTAGE_TYPE_5 0x00000000

#endif /* _EPT_GPIO_DRV_H */
