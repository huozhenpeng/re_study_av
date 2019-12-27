package com.miduo.player;

import android.media.MediaCodec;
import android.media.MediaCodecInfo;
import android.media.MediaFormat;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;

public class MediaCodecRecord {

    private MediaFormat mediaFormat;
    private MediaCodec mediaCodec;
    private MediaCodec.BufferInfo bufferInfo;
    private FileOutputStream outputStream;

    private int aacSampleRate=4;

    public void initMediaCodec(int samplerate, File outfile)
    {
        try {
            aacSampleRate=getADTSsamplerate(samplerate);
            mediaFormat=MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC,samplerate,2);
            mediaFormat.setInteger(MediaFormat.KEY_BIT_RATE,96000);//码率也叫比特率
            mediaFormat.setInteger(MediaFormat.KEY_AAC_PROFILE, MediaCodecInfo.CodecProfileLevel.AACObjectLC);
            mediaFormat.setInteger(MediaFormat.KEY_MAX_INPUT_SIZE,1024*8);
            mediaCodec=MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC);

            bufferInfo=new MediaCodec.BufferInfo();
            if(mediaCodec==null)
            {
                Log.e("VoicePlayer","[Java]创建编码器失败");
            }
            mediaCodec.configure(mediaFormat,null,null,MediaCodec.CONFIGURE_FLAG_ENCODE);
            outputStream=new FileOutputStream(outfile);
            mediaCodec.start();

        }
        catch (Exception e)
        {
            e.printStackTrace();
        }

    }

    private int perpcmsize=0;
    private byte[] outByteBuffer;
    public void encodePcm2AAC(int size,byte[] buffer)
    {
        if(buffer!=null&&mediaCodec!=null)
        {
            int inputBufferIndex=mediaCodec.dequeueInputBuffer(0);
            if(inputBufferIndex>=0)
            {
                ByteBuffer byteBuffer=mediaCodec.getInputBuffers()[inputBufferIndex];
                byteBuffer.clear();
                byteBuffer.put(buffer);
                mediaCodec.queueInputBuffer(inputBufferIndex,0,size,0,0);

            }
            int index=mediaCodec.dequeueOutputBuffer(bufferInfo,0);
            while (index>0)
            {
                try {
                    perpcmsize=bufferInfo.size+7;
                    outByteBuffer=new byte[perpcmsize];

                    ByteBuffer byteBuffer=mediaCodec.getOutputBuffers()[index];
                    byteBuffer.position(bufferInfo.offset);
                    byteBuffer.limit(bufferInfo.offset+bufferInfo.size);

                    addADtsHeader(outByteBuffer,perpcmsize,aacSampleRate);

                    //outByteBuffer偏移7个字节然后将byteBuffer放入outByteBuffer
                    byteBuffer.get(outByteBuffer,7,bufferInfo.size);
                    byteBuffer.position(bufferInfo.offset);
                    outputStream.write(outByteBuffer,0,perpcmsize);

                    mediaCodec.releaseOutputBuffer(index,false);
                    index=mediaCodec.dequeueOutputBuffer(bufferInfo,0);
                    outByteBuffer=null;

                }
                catch (Exception e)
                {

                }
            }
        }


    }



    private void addADtsHeader(byte[] packet, int packetLen, int samplerate)
    {
        int profile = 2; // AAC LC
        int freqIdx = samplerate; // samplerate
        int chanCfg = 2; // CPE

        packet[0] = (byte) 0xFF; // 0xFFF(12bit) 这里只取了8位，所以还差4位放到下一个里面
        packet[1] = (byte) 0xF9; // 第一个t位放F
        packet[2] = (byte) (((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2));
        packet[3] = (byte) (((chanCfg & 3) << 6) + (packetLen >> 11));
        packet[4] = (byte) ((packetLen & 0x7FF) >> 3);
        packet[5] = (byte) (((packetLen & 7) << 5) + 0x1F);
        packet[6] = (byte) 0xFC;
    }

    private int getADTSsamplerate(int samplerate)
    {
        int rate = 4;
        switch (samplerate)
        {
            case 96000:
                rate = 0;
                break;
            case 88200:
                rate = 1;
                break;
            case 64000:
                rate = 2;
                break;
            case 48000:
                rate = 3;
                break;
            case 44100:
                rate = 4;
                break;
            case 32000:
                rate = 5;
                break;
            case 24000:
                rate = 6;
                break;
            case 22050:
                rate = 7;
                break;
            case 16000:
                rate = 8;
                break;
            case 12000:
                rate = 9;
                break;
            case 11025:
                rate = 10;
                break;
            case 8000:
                rate = 11;
                break;
            case 7350:
                rate = 12;
                break;
        }
        return rate;
    }
}
