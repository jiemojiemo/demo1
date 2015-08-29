#pragma once

#include "recordaudioplayer.h"

class RecordPlayerFactory{
private:
    RecordPlayerFactory( const RecordPlayerFactory& rf ){}
//    RecordPlayerFactory& operator=( const RecordPlayerFactory& rf ){}
public:
    RecordPlayerFactory(){}
    static RecordAudioPlayerABC* GetRecordPlayer( const RecordPlayInfo& info )
    {
        
        switch( info.sampleType )
        {
            case SAMPLE_TYPE_FLOAT32:
                return new RecordAudioPlayerFloat32( info );
                break;
            case SAMPLE_TYPE_INT16:
                break;
            case SAMPLE_TYPE_INT8:
                break;
            case SAMPLE_TYPE_UINT8:
                break;
            default:
                return nullptr;
                break;
        }
        return nullptr;
    }
};