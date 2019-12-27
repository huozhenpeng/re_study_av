// ICallBackInterface.aidl
package com.miduo.restudyav;

// Declare any non-default types here with import statements

interface ICallBackInterface {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat,
            double aDouble, String aString);

    void dbCallBack(int db,int currentTime);

    void totalTimeCallBack(int totalTime);

}
