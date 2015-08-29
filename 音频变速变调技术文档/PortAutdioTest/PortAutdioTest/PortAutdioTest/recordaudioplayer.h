#pragma once
#include "portaudio.h"
#include "record_struct.h"

class RecordAudioPlayerABC{
private:
    RecordAudioPlayerABC( const RecordAudioPlayerABC& rp ){}
//    RecordAudioPlayerABC& operator=( const RecordAudioPlayerABC& rp ){}
    
    /**
     * @brief : 根据bits获得format
     */
    PaSampleFormat GetSampleFormat();
protected:
    static RecordPlayInfo	m_info;
    RecordData		m_data;
    callbackPointer m_pCallback;
    
public:
    RecordAudioPlayerABC();
    virtual ~RecordAudioPlayerABC();
    
    int StartPlay();
    
    /**
     * @brief : 根据info的数据(秒)来播放，音频实际播放时间要严格>=info的秒数
     */
    virtual int SetData( const void* data )=0;
    /**
     * @brief : 根据buffersize来进行播放，忽略info中的seconds的束缚
     */
    virtual int SetData( const void* data, unsigned long bufferSize )=0;
};

class RecordAudioPlayerFloat32 : public RecordAudioPlayerABC{
private:
    static int playCallback(const void *inputBuffer,
                            void *outputBuffer, unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData);
public:
    RecordAudioPlayerFloat32( const RecordPlayInfo& info );
    ~RecordAudioPlayerFloat32(){}
    
    virtual int SetData( const void* data );
    virtual int SetData( const void* data, unsigned long bufferSize );
};

class RecordAudioPlayerInt16 : public RecordAudioPlayerABC{
private:
    static int playCallback(const void *inputBuffer,
                            void *outputBuffer, unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData);
public:
    RecordAudioPlayerInt16( const RecordPlayInfo& info );
    ~RecordAudioPlayerInt16(){}
    
    virtual int SetData( const void* data );
    virtual int SetData( const void* data, unsigned long bufferSize );
};