#include "recordaudioplayer.h"
#include <stdio.h>

RecordPlayInfo	RecordAudioPlayerABC::m_info;

//RecordPlayerABC
RecordAudioPlayerABC::RecordAudioPlayerABC()
{
    
}

RecordAudioPlayerABC::~RecordAudioPlayerABC()
{
    
}

/*******************************
 ** FunctionName：GetSampleFormat
 ** Comment:     根据bits获得format
 ** return：     format
 ** Creator：    HW
 ** Date：		2015-8-14
 ** Modifier：
 ** ModifyDate：
 ** Version：    1.0.0
 *******************************/
PaSampleFormat RecordAudioPlayerABC::GetSampleFormat()
{
    switch( m_info.sampleType )
    {
        case SAMPLE_TYPE_FLOAT32:
            return paFloat32;
        case SAMPLE_TYPE_INT16:
            return paInt16;
        case SAMPLE_TYPE_INT8:
            return paInt8;
        case SAMPLE_TYPE_UINT8:
            return paUInt8;
        default:
            return paFloat32;
    }
}

int RecordAudioPlayerABC::StartPlay()
{
    PaError					err = paNoError;
    PaStream*				stream;
    PaStreamParameters		outputParameters;
    //portaudio环境初始化
    err = Pa_Initialize();
    if( err != paNoError )
        return err;
    
    outputParameters.device = Pa_GetDefaultOutputDevice();
    if( outputParameters.device == paNoDevice )
    {
        fprintf( stderr, "Error: No default output device.\n" );
        return -1;
    }
    
    outputParameters.channelCount = m_info.channels;
    outputParameters.sampleFormat = this->GetSampleFormat();
    outputParameters.suggestedLatency =
    Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    
    printf("\n=== Now playing back. ===\n"); fflush(stdout);
    
    //打开音频流
    err = Pa_OpenStream( &stream,
                        NULL, //no input
                        &outputParameters,
                        m_info.sampleRate,
                        m_info.framePerBuffer,
                        paClipOff,
                        m_pCallback,
                        &m_data
                        );
    if( err != paNoError )
        return err;
    if( stream )
    {
        //开始播放
        err = Pa_StartStream( stream );
        if( err != paNoError )
            return err;
        
        printf("Waiting for playback to finish.\n"); fflush(stdout);
        
        //等待播放结束
        while( ( err = Pa_IsStreamActive( stream ) ) == 1 ) Pa_Sleep(100);
        if( err < 0 )
            return err;
        
        printf("Done.\n"); fflush(stdout);
    }
    return err;
}

//RecordPlayerFloat32

RecordAudioPlayerFloat32::RecordAudioPlayerFloat32( const RecordPlayInfo& info )
{
    m_info = info;
    m_pCallback = playCallback;
    m_data.frameIndex = 0;
    m_data.maxFrameIndex = info.sampleRate * info.seconds;
    m_data.totalBytes = m_data.maxFrameIndex * info.channels * sizeof(float);
    m_data.recordedSamples = NULL;
}

int RecordAudioPlayerFloat32::SetData( const void* data )
{
    if( data != NULL )
        m_data.recordedSamples = (float*)data;
    else
    {
        fprintf( stderr, "Error: data is empty.\n" );
        return -1;
    }
    return 0;
}
int RecordAudioPlayerFloat32::SetData( const void* data, unsigned long bufferSize )
{
    if( data != NULL )
        m_data.recordedSamples = (float*)data;
    else
    {
        fprintf( stderr, "Error: data is empty.\n" );
        return -1;
    }
    m_data.totalBytes = bufferSize;
    m_data.maxFrameIndex = bufferSize / m_info.channels / sizeof(float);
    return 0;
}
int RecordAudioPlayerFloat32::playCallback(const void *inputBuffer,
                                           void *outputBuffer, unsigned long framesPerBuffer,
                                           const PaStreamCallbackTimeInfo* timeInfo,
                                           PaStreamCallbackFlags statusFlags,
                                           void *userData)
{
    
    RecordData *data = (RecordData*)userData;
    float* buffer = (float*)data->recordedSamples;
    float *rptr = &buffer[data->frameIndex * m_info.channels];
    buffer = rptr;
    float *wptr = (float*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;
    
    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;
    
    if( framesLeft < framesPerBuffer )
    {
        /* final buffer... */
        for( i=0; i<framesLeft; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( m_info.channels == 2 )
                *wptr++ = *rptr++;  /* right */
        }
        for( ; i<framesPerBuffer; i++ )
        {
            *wptr++ = 0;  /* left */
            if( m_info.channels == 2 )
                *wptr++ = 0;  /* right */
        }
        data->frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            
            *wptr++ = *rptr++;  /* left */
            if( m_info.channels == 2 )
                *wptr++ = *rptr++;  /* right */
            
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    
    return finished;
}

//RecordPlayerInt16
RecordAudioPlayerInt16::RecordAudioPlayerInt16( const RecordPlayInfo& info )
{
    m_info = info;
    m_pCallback = playCallback;
    m_data.frameIndex = 0;
    m_data.maxFrameIndex = info.sampleRate * info.seconds;
    m_data.totalBytes = m_data.maxFrameIndex * info.channels * sizeof(short);
    m_data.recordedSamples = NULL;
}

int RecordAudioPlayerInt16::SetData( const void* data )
{
    if( data != NULL )
        m_data.recordedSamples = (short*)data;
    else
    {
        fprintf( stderr, "Error: data is empty.\n" );
        return -1;
    }
    return 0;
}

int RecordAudioPlayerInt16::SetData( const void* data, unsigned long bufferSize )
{
    if( data != NULL )
        m_data.recordedSamples = (short*)data;
    else
    {
        fprintf( stderr, "Error: data is empty.\n" );
        return -1;
    }
    m_data.totalBytes = bufferSize;
    m_data.maxFrameIndex = bufferSize / m_info.channels / sizeof(short);
    return 0;
}

int RecordAudioPlayerInt16::playCallback(const void *inputBuffer,
                                         void *outputBuffer, unsigned long framesPerBuffer,
                                         const PaStreamCallbackTimeInfo* timeInfo,
                                         PaStreamCallbackFlags statusFlags, 
                                         void *userData)
{
    
    RecordData *data = (RecordData*)userData;
    short* buffer = (short*)data->recordedSamples;
    short *rptr = &buffer[data->frameIndex * m_info.channels];
    short *wptr = (short*)outputBuffer;
    unsigned int i;
    int finished;
    unsigned int framesLeft = data->maxFrameIndex - data->frameIndex;
    
    (void) inputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;
    
    if( framesLeft < framesPerBuffer )
    {
        /* final buffer... */
        for( i=0; i<framesLeft; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( m_info.channels == 2 ) 
                *wptr++ = *rptr++;  /* right */
        }
        for( ; i<framesPerBuffer; i++ )
        {
            *wptr++ = 0;  /* left */
            if( m_info.channels == 2 ) 
                *wptr++ = 0;  /* right */
        }
        data->frameIndex += framesLeft;
        finished = paComplete;
    }
    else
    {
        for( i=0; i<framesPerBuffer; i++ )
        {
            
            *wptr++ = *rptr++;  /* left */
            if( m_info.channels == 2 ) 
                *wptr++ = *rptr++;  /* right */
            
        }
        data->frameIndex += framesPerBuffer;
        finished = paContinue;
    }
    //.....
    return finished;
}