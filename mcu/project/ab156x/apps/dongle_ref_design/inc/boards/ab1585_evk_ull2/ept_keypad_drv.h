/*
 * Generated by Airoha Easy PinMux Tool Version 2.8.1 for AB1585. Copyright Airoha Inc. (C) 2015.
 * 2022-06-01 09:42:36:0734
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
DEVICE_KEY_NONE, \
DEVICE_KEY_NONE, \
DEVICE_KEY_NONE 
#endif

#define POWERKEY_POSITION DEVICE_KEY_POWER

#define DRV_KBD_CAPTOUCH_SEL 0x1


#endif /* _EPT_KEYPAD_DRV_H */
