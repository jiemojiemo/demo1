#include "recordaudioplayerfactory.h"
#include "CAudioTimeSandPitchS.h"
#include "recordfactory.h"
#include <stdio.h>
#include <string>
#define RECORD_TEST 1
#define AUDIO_TEST 0
#define SAVE_RECORD 1
FILE* file;
//Release
//#pragma comment( lib, "portaudio.lib" )

//Debug
//#pragma comment( lib, "portaudio_x86.lib" )


void SaveRecord( const RecordABC* record );



int main()
{
#if RECORD_TEST
    
    RecordFactory factory;
    //设置录音所需要的参数
    RecordeInfo recordInfo;
    recordInfo.channels			 = 1;
    recordInfo.framesPerBuffer	 = 512;
    recordInfo.sampleRate		 = 44100;
    recordInfo.sampleType		 = SAMPLE_TYPE_FLOAT32;
    recordInfo.seconds			 = 5;
    
    RecordABC* recorder = factory.GetRecord( recordInfo );
    
    if( recorder == nullptr )
    {
        fprintf( stderr, "Error:Cannot get record.\n");
        return -1;
    }
    
    recorder->StartRecord();
    //保存录音到文件
    SaveRecord(recorder);
    
    
    CAudioTimeSandPitchS processer;
    
    float scale = 0.0;
    printf( "变速速率( 0.1 -- 5.0 ):" );fflush(stdout);
    cin >> scale;
    int pitch = 0;
    printf( "音高变化( -10 -- 10 ): " ); fflush(stdout);
    cin >> pitch;
    
    //当声道数为 1 时，直接返回录音数据的指针
    //当声道数为 2 时，没有实现
    int channels = 1;
    float *dataIn = processer.WavReadBuffer( (float*)recorder->GetRecordData(), recorder->GetRecordSize(), channels );
    //进行处理
    float *result = processer.TimeScalingAndPitchShifting( pitch, scale, dataIn,1024, 256 );
    unsigned int bufferSize = processer.GetResampleSize();
    
    //设置播放参数，进行播放
    RecordPlayInfo info;
    info.channels = 1;
    info.sampleRate = 44100;
    info.sampleType = SAMPLE_TYPE_FLOAT32;
    info.seconds = 10;
    info.framePerBuffer = 512;
    RecordPlayerFactory recordPlayFactory;
    RecordAudioPlayerABC *audioPlayer = recordPlayFactory.GetRecordPlayer( info );
    audioPlayer->SetData( result, bufferSize );
    //player->SetData( buffer );
    audioPlayer->StartPlay();
    
    
#endif
    
#if AUDIO_TEST
    //open file
    file = fopen( "record.raw", "rb" );
    if( file == NULL )
    {
        fprintf( stderr, "Error:Cannot open the file.\n" );
        return -1;
    }
    
    //get file size
    unsigned long fileSize = 0;
    fseek( file, 0, SEEK_END );
    fileSize = ftell( file );
    fseek( file, 0, SEEK_SET );
    
    //malloc memory
    void* buffer = (void*)malloc( fileSize );
    if( buffer == NULL )
    {
        fprintf( stderr, "Error: malloc failure.\n" );
        return -1;
    }
    
    //copy data from file
    fread( buffer, 1, fileSize, file );
    fclose( file );
    
    //play
    RecordPlayInfo info;
    info.channels = 1;
    info.sampleRate = 44100;
    info.sampleType = SAMPLE_TYPE_FLOAT32;
    info.seconds = 10;
    info.framePerBuffer = 512;
    RecordPlayerFactory recordPlayFactory;
    RecordAudioPlayerABC *audioPlayer = recordPlayFactory.GetRecordPlayer( info );
    audioPlayer->SetData( buffer, fileSize );
    //player->SetData( buffer );
    audioPlayer->StartPlay();
    if( buffer != NULL )
    {
        free( buffer );
        buffer = NULL;
    }
#endif
    
    
    return 0;
}


void SaveRecord( const RecordABC* record )
{
    file = fopen( "record.raw", "wb" );
    
    if( file == NULL )
    {
        fprintf( stderr, "Error:Cannot save record.\n" );
        return;
    }
    fwrite( record->GetRecordData(), 1, record->GetRecordSize(), file );
    fclose( file );
}