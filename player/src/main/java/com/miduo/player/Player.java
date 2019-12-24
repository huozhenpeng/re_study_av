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
    private String Tag="Player";
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

    public void callBackJava(int code,String msg)
    {
        if(debug)
        {
            Log.e(Tag,"code:"+code+"------msg:"+msg);
        }
    }
}
