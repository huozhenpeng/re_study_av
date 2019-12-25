package com.miduo.player;

import android.util.Log;
import android.widget.Toast;

public class Player {
    static {
        System.loadLibrary("native-lib");
        System.loadLibrary("avutil-55");
        System.loadLibrary("swresample-2");
        System.loadLibrary("avcodec-57");
        System.loadLibrary("avformat-57");
        System.loadLibrary("swscale-4");
    }

    private boolean debug=true;
    private String Tag="JavaPlayer";
    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    public native void prepare(String url);

    /**
     * 打印ffmpeg支持的解码器
     */
    public native void printCodec();

    /**
     * 开始解码操作
     */
    public native void start();

    public void callBackJava(int code,String msg)
    {
        if(debug)
        {
            //Log.e(Tag,"code:"+code+"------msg:"+msg);
        }
        if(code==200)
        {
            if(debug)
            {
                //Log.e(Tag,"解码器已经准备好了，开始解码了");
            }
            start();
        }
    }

    public void onShowTime(int code,int total,int current)
    {
        Log.e(Tag,"total==>"+total+"current===>"+current);

    }

    public native void pause();

    public native void resume() ;

    public native void stop();
}
