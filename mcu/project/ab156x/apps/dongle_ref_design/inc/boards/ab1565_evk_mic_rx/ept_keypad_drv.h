/*
 * Generated by Airoha Easy PinMux Tool Version 2.9.3 for AB1565. Copyright Airoha Inc. (C) 2015.
 * 2022-10-12 20:55:26:0788
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

#ifndef  _EPT_KEYPAD_DRV_H
#define  _EPT_KEYPAD_DRV_H


#define  __EPT_CAPTOUCH_KEY__

#if defined(__EPT_CAPTOUCH_KEY__)
#define KEYPAD_MAPPING \
DEVICE_KEY_A, \
DEVICE_KEY_B, \
DEVICE_KEY_C, \
DEVICE_KEY_D 
#endif

#define POWERKEY_POSITION DEVICE_KEY_POWER

#define DRV_KBD_CAPTOUCH_SEL 0xf


#endif /* _EPT_KEYPAD_DRV_H */
