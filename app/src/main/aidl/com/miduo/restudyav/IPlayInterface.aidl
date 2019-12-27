// IPlayInterface.aidl
package com.miduo.restudyav;

import com.miduo.restudyav.ICallBackInterface;

interface IPlayInterface {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat,
            double aDouble, String aString);


    void play(String url);

    void pause();

    void resume();

    void stop();

    void seek(int seek);

    void leftVoice();

    void rightVoice();

    void steroVoice();


    void setPitch(float pitch);

    void setSpeed(float speed);

    void setVolume(int voice);

    void registerListener(ICallBackInterface listener);

    void unregisterListener(ICallBackInterface listener);




}
