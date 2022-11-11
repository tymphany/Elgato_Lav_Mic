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

/*!
 *@file   stream_audio.c
 *@brief  Defines the audio stream
 *
 @verbatim
 @endverbatim
 */

//-
#include "types.h"
#include "dsp_drv_dfe.h"
#include "stream_audio.h"
#include "audio_config.h"
//#include "os_intr_lv3.h"
#include "dsp_buffer.h"
#include "source.h"
#include "sink.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "davt.h"
#include "dsp_memory.h"
#include <string.h>
#ifdef AIR_DCHS_MODE_ENABLE
#include "dsp_mux_uart.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define AUDIO_QUEUE_SIZE(ptr)       (((AUDIO_QUEUE_STRU_PTR)ptr)->length)//AUDIO_SINK_DEFAULT_FRAME_SIZE//
#define AUDIO_QUEUE_DATAPTR(ptr)    (((AUDIO_QUEUE_STRU_PTR)ptr)->data_space)//ptr//

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Type Defintions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
VOID Sink_Audio_Path_Interface_Init(SINK sink);
VOID Source_Audio_Path_Interface_Init(SOURCE source);
VOID Source_Audio_SubPath_Interface_Init(SOURCE source);

U32  SinkSizeAudio(SINK sink);
BOOL SinkWriteBufAudio(SINK sink, U8 *src_addr, U32 length);
BOOL SinkFlushAudio(SINK sink, U32 amount);
BOOL SinkCloseAudio(SINK sink);
BOOL SinkConfigureAudio(SINK sink, stream_config_type type, U32 value);
U32  SinkClaimAudio(SINK sink, U32 extra);
U8  *SinkMapAudio(SINK sink);

U32  SourceSizeAudio(SOURCE source);
BOOL SourceReadBufAudio(SOURCE source, U8 *dst_addr, U32 length);
VOID SourceDropAudio(SOURCE source, U32 amount);
BOOL SourceCloseAudio(SOURCE source);
BOOL SourceConfigureAudio(SOURCE source, stream_config_type type, U32 value);
U8  *SourceMapAudio(SOURCE source);

U32  SourceSize_AudioPattern_VP(SOURCE source);
BOOL SourceReadBuf_AudioPattern_VP(SOURCE source, U8 *dst_addr, U32 length);
VOID SourceDrop_AudioPattern_VP(SOURCE source, U32 amount) ;
U8  *SourceMap_AudioPattern_VP(SOURCE source);
U32  SourceSize_AudioPattern_RT(SOURCE source);
BOOL SourceReadBuf_AudioPattern_RT(SOURCE source, U8 *dst_addr, U32 length);
VOID SourceDrop_AudioPattern_RT(SOURCE source, U32 amount);
BOOL SourceClose_AudioPattern(SOURCE source);




VOID Sink_Audio_Path_Interface_Init(SINK sink)
{
    /* interface init */
    sink->sif.SinkSlack         = SinkSizeAudio;
    sink->sif.SinkFlush         = SinkFlushAudio;
    sink->sif.SinkConfigure     = SinkConfigureAudio;
    sink->sif.SinkClose         = SinkCloseAudio;
    sink->sif.SinkClaim         = SinkClaimAudio;
    sink->sif.SinkMap           = SinkMapAudio;
    sink->sif.SinkWriteBuf      = SinkWriteBufAudio;
}


VOID Source_Audio_Path_Interface_Init(SOURCE source)
{
    /* interface init */
    source->sif.SourceSize         = SourceSizeAudio;
    source->sif.SourceDrop         = SourceDropAudio;
    source->sif.SourceConfigure    = SourceConfigureAudio;
    source->sif.SourceClose        = SourceCloseAudio;
    source->sif.SourceReadBuf      = SourceReadBufAudio;
}


VOID Source_Audio_SubPath_Interface_Init(SOURCE source)
{
    /* interface init */
    if (source->param.VPRT.mode != RT_mode) {
        source->sif.SourceConfigure    = NULL;
        source->sif.SourceClose        = SourceClose_AudioPattern;
        source->sif.SourceSize         = SourceSize_AudioPattern_VP;
        source->sif.SourceDrop         = SourceDrop_AudioPattern_VP;
        source->sif.SourceMap          = SourceMap_AudioPattern_VP;
        source->sif.SourceReadBuf      = SourceReadBuf_AudioPattern_VP;
    } else {
        source->sif.SourceConfigure    = NULL;
        source->sif.SourceClose        = SourceClose_AudioPattern;
        source->sif.SourceSize         = SourceSize_AudioPattern_RT;
        source->sif.SourceDrop         = SourceDrop_AudioPattern_RT;
        source->sif.SourceMap          = NULL;
        source->sif.SourceReadBuf      = SourceReadBuf_AudioPattern_RT;
    }

}


U32 SinkSizeAudio(SINK sink)
{
    U32 writeOffset = sink->streamBuffer.BufferInfo.WriteOffset;
    U32 readOffset  = sink->streamBuffer.BufferInfo.ReadOffset;
    U32 length      = sink->streamBuffer.BufferInfo.length;
    if ((sink->streamBuffer.BufferInfo.bBufferIsFull) || (Audio_setting->Audio_sink.Pause_Flag == TRUE)) {
        return 0;
    } else {
        return writeOffset >= readOffset ? (length + readOffset - writeOffset) : (readOffset - writeOffset)  ;
    }
}


BOOL SinkWriteBufAudio(SINK sink, U8 *src_addr, U32 length)
{
    return Sink_Audio_WriteBuffer(sink, src_addr, length);
}


BOOL SinkFlushAudio(SINK sink, U32 amount)
{
    return Sink_Audio_FlushBuffer(sink, amount);
}


BOOL SinkCloseAudio(SINK sink)
{
    return Sink_Audio_CloseProcedure(sink);
}


BOOL SinkConfigureAudio(SINK sink, stream_config_type type, U32 value)
{
    return Sink_Audio_Configuration(sink, type, value);
}


U32 SinkClaimAudio(SINK sink, U32 extra)
{
    if ((sink->param.audio.frame_size == (U16)extra)
        && (sink->streamBuffer.BufferInfo.bBufferIsFull != TRUE)
        && (sink->transform == NULL)) {
        return 0;
    } else {
        return SINK_INVALID_CLAIM;
    }
}


U8 *SinkMapAudio(SINK sink)
{
    return
        (sink->streamBuffer.BufferInfo.startaddr[sink->streamBuffer.BufferInfo.channelSelected]
         + sink->streamBuffer.BufferInfo.WriteOffset);
}


U32 SourceSizeAudio(SOURCE source)
{
    U32 data_size;
    data_size = (source->streamBuffer.BufferInfo.WriteOffset >= source->streamBuffer.BufferInfo.ReadOffset)
                ? (source->streamBuffer.BufferInfo.WriteOffset - source->streamBuffer.BufferInfo.ReadOffset)
                : (source->streamBuffer.BufferInfo.length - source->streamBuffer.BufferInfo.ReadOffset + source->streamBuffer.BufferInfo.WriteOffset);
    if ((source->streamBuffer.BufferInfo.bBufferIsFull)
        || (data_size >= source->param.audio.frame_size)) {
        return (U32)source->param.audio.frame_size;
    } else {
        return 0;
    }
}


BOOL SourceReadBufAudio(SOURCE source, U8 *dst_addr, U32 length)
{
    return Source_Audio_ReadAudioBuffer(source, dst_addr, length);
}


VOID SourceDropAudio(SOURCE source, U32 amount)
{
    if (amount == 0) {
        return;
    } else if (source->streamBuffer.BufferInfo.bBufferIsFull == TRUE) {
        source->streamBuffer.BufferInfo.bBufferIsFull = FALSE;
    }

    source->streamBuffer.BufferInfo.ReadOffset
        = (source->streamBuffer.BufferInfo.ReadOffset + source->param.audio.frame_size)
          % (source->streamBuffer.BufferInfo.length);
}


BOOL SourceCloseAudio(SOURCE source)
{
    return Source_Audio_CloseProcedure(source);
}


BOOL SourceConfigureAudio(SOURCE source, stream_config_type type, U32 value)
{
    return Source_Audio_Configuration(source, type, value);
}


U8 *SourceMapAudio(SOURCE source)
{
    U32 writeOffset = source->streamBuffer.BufferInfo.WriteOffset;
    U32 readOffset  = source->streamBuffer.BufferInfo.ReadOffset;
    U32 length      = source->streamBuffer.BufferInfo.length;

    U32 MoveLength  = writeOffset >= readOffset
                      ? (writeOffset - readOffset)
                      : (length - readOffset + writeOffset);
    DSP_C2D_BufferCopy(MapAddr,
                       source->streamBuffer.BufferInfo.startaddr[source->streamBuffer.BufferInfo.channelSelected] + readOffset,
                       MoveLength,
                       source->streamBuffer.BufferInfo.startaddr[source->streamBuffer.BufferInfo.channelSelected],
                       length);

    return MapAddr;
}


U32 SourceSize_AudioPattern_VP(SOURCE source)
{
    return Source_Audio_GetVoicePromptSize(source);
}


BOOL SourceReadBuf_AudioPattern_VP(SOURCE source, U8 *dst_addr, U32 length)
{
    return Source_Audio_ReadVoicePromptBuffer(source, dst_addr, length);
}


VOID SourceDrop_AudioPattern_VP(SOURCE source, U32 amount)
{
    Source_Audio_DropVoicePrompt(source, amount);
}


U8 *SourceMap_AudioPattern_VP(SOURCE source)
{
    U32 readOffset = source->streamBuffer.BufferInfo.ReadOffset;
    U32 remainlength = SourceSize_AudioPattern_VP(source);

    if (remainlength) {
        remainlength = (remainlength > 4) ? 4 : remainlength;
        memcpy(MapAddr, source->streamBuffer.BufferInfo.startaddr[0] + readOffset, remainlength);
    }
    return MapAddr;
}


U32 SourceSize_AudioPattern_RT(SOURCE source)
{
    return Source_Audio_GetRingtoneSize(source);
}


BOOL SourceReadBuf_AudioPattern_RT(SOURCE source, U8 *dst_addr, U32 length)
{
    return Source_Audio_ReadRingtoneBuf(source, dst_addr, length);
}


VOID SourceDrop_AudioPattern_RT(SOURCE source, U32 amount)
{
    Source_Audio_DropRingtone(source, amount);
}


BOOL SourceClose_AudioPattern(SOURCE source)
{
    if (source) {
        return TRUE;
    }
    return FALSE;
}


BOOL SinkFlushVirtualSink(SINK sink, U32 amount)
{

    if (sink->param.virtual_para.entry != NULL) {
        sink->param.virtual_para.entry((VOID *)sink->streamBuffer.BufferInfo.startaddr[0], amount);
    }
    if (sink->param.virtual_para.handler != NULL) {
        //MSG_MessageSend(sink->param.virtual_para.handler, MESSAGE_SINK_NEW_DATA_IN, 0);
    }
    return TRUE;

}

BOOL SinkCloseVirtualSink(SINK sink)
{
    sink->param.virtual_para.user_count--;
    if(!sink->param.virtual_para.user_count) {
        if (sink->streamBuffer.BufferInfo.startaddr[0]) {
            vPortFree(sink->streamBuffer.BufferInfo.startaddr[0]);
            sink->streamBuffer.BufferInfo.startaddr[0] = NULL;
        }
    }
    return (sink->param.virtual_para.user_count) ? false : true;
}

U32 SinkSlackVirtualSink(SINK sink)
{

    return (sink->param.virtual_para.mem_size) ? sink->param.virtual_para.mem_size : 0xffffffff ;
}

U32 SinkClaimVirtualSink(SINK sink, U32 len)
{
    return (len > sink->param.virtual_para.mem_size)
           ? 0xffffffff
           : 0;
}

U8 *SinkMapVirtualSink(SINK sink)
{
    return (U8 *)sink->streamBuffer.BufferInfo.startaddr[0];
}

BOOL SinkWriteBufVirtualSink(SINK sink, U8 *src_addr, U32 length)
{
    TRANSFORM transform = sink->transform;
    DSP_CALLBACK_PTR callback_ptr;

    if (!sink->param.virtual_para.mem_size) {
        return FALSE;
    }

    if (transform != NULL && src_addr == NULL) {
        callback_ptr = DSP_Callback_Get(transform->source, sink);
        src_addr = callback_ptr->EntryPara.out_ptr[0];
    }

    if (length <= sink->param.virtual_para.mem_size) {
        memcpy(sink->streamBuffer.BufferInfo.startaddr[0],
               src_addr,
               length);
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL SinkConfigureVirtualSink(SINK sink, stream_config_type type, U32 value)
{
    return Sink_VirtualSink_Configuration(sink, type, value);
}


VOID Sink_VirtualSink_Interface_Init(SINK sink)
{
    /* interface init */
    sink->sif.SinkSlack         = SinkSlackVirtualSink;
    sink->sif.SinkFlush         = SinkFlushVirtualSink;
    sink->sif.SinkConfigure     = SinkConfigureVirtualSink;
    sink->sif.SinkClose         = SinkCloseVirtualSink;
    sink->sif.SinkClaim         = SinkClaimVirtualSink;
    sink->sif.SinkMap           = SinkMapVirtualSink;
    sink->sif.SinkWriteBuf      = SinkWriteBufVirtualSink;
}

U32 SourceSizeVirtualSource(SOURCE source)
{
    if(source->param.virtual_para.is_processed == false){
        return (source->param.virtual_para.mem_size);
    } else {
        return 0;
    }
}

#include "dsp_dump.h"
BOOL SourceReadBufVirtualSource(SOURCE source, U8 *dst_addr, U32 length)
{
    TRANSFORM transform = source->transform;
    DSP_CALLBACK_PTR callback_ptr;
    // DSP_MW_LOG_I("SourceReadBufVirtualSource :%d, is_processed %d, is_dummy_data %d", 3,
    //                 source->param.virtual_para.mem_size,
    //                 source->param.virtual_para.is_processed,
    //                 source->param.virtual_para.is_dummy_data);
    if (!source->param.virtual_para.mem_size) {
        return FALSE;
    }

    if (transform != NULL) {
        callback_ptr = DSP_Callback_Get(source, transform->sink);
        if (length <= source->param.virtual_para.mem_size) {
            for(uint8_t i=0; i<callback_ptr->EntryPara.in_channel_num; i++){
                dst_addr = callback_ptr->EntryPara.out_ptr[i];
                //DSP_MW_LOG_I("SourceReadBufVirtualSource :0x%x, 0x%x, %d", 3, dst_addr, source->streamBuffer.BufferInfo.startaddr[i],length);
                if (dst_addr != NULL) {
                    if ((source->streamBuffer.BufferInfo.startaddr[i] == NULL) || (source->param.virtual_para.is_dummy_data == true)) {
                        memset(dst_addr, 0, length);
                    } else {
                        memcpy(dst_addr,
                        source->streamBuffer.BufferInfo.startaddr[i],
                        length);
                    }
                }
                if(i == 0){
                    //LOG_AUDIO_DUMP(source->streamBuffer.BufferInfo.startaddr[i], length, AUDIO_DUMP_TEST_ID_2);
                }
            }
            if(source->scenario_type == AUDIO_SCENARIO_TYPE_WIRED_AUDIO_USB_IN_OUT_IEM){
                memcpy(callback_ptr->EntryPara.out_ptr[0],
                        source->streamBuffer.BufferInfo.startaddr[0],
                        length);
            }
            source->param.virtual_para.is_processed = true;  //data used
            source->param.virtual_para.is_dummy_data = true; //data used, if no data coming, next data will be dummy 0.
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        return FALSE;
    }
}

U8 *SourceMapVirtualSource(SOURCE source)
{
    return (U8 *)source->streamBuffer.BufferInfo.startaddr[0];
}

BOOL SourceConfigureVirtualSource(SOURCE source, stream_config_type type, U32 value)
{
    UNUSED(source);
    UNUSED(type);
    UNUSED(value);
    return TRUE;
}

VOID SourceDropVirtualSource(SOURCE source, U32 amount)
{
    UNUSED(source);
    UNUSED(amount);
}

BOOL SourceCloseVirtualSource(SOURCE source)
{
    source->param.virtual_para.user_count--;
    if(!source->param.virtual_para.user_count) {
        for(uint8_t i=0; i<BUFFER_INFO_CH_NUM; i++){
            if (source->streamBuffer.BufferInfo.startaddr[i]) {
                vPortFree(source->streamBuffer.BufferInfo.startaddr[i]);
                source->streamBuffer.BufferInfo.startaddr[i] = NULL;
            }
        }
    }
#if defined(AIR_WIRED_AUDIO_ENABLE)
    if (source->scenario_type == AUDIO_SCENARIO_TYPE_WIRED_AUDIO_USB_IN_OUT_IEM){
        extern void wired_audio_usb_in_out_source_close(SOURCE source);
        wired_audio_usb_in_out_source_close(source);
    }
#endif
    return (source->param.virtual_para.user_count) ? false : true;
}
VOID Source_VirtualSource_Interface_Init(SOURCE source)
{
    /* interface init */
    source->sif.SourceSize        = SourceSizeVirtualSource;
    source->sif.SourceReadBuf     = SourceReadBufVirtualSource;
    source->sif.SourceMap         = SourceMapVirtualSource;
    source->sif.SourceConfigure   = SourceConfigureVirtualSource;
    source->sif.SourceDrop        = SourceDropVirtualSource;
    source->sif.SourceClose       = SourceCloseVirtualSource;
}

