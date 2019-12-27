package com.miduo.restudyav;

import android.app.Service;
import android.content.Intent;
import android.os.Binder;
import android.os.Environment;
import android.os.IBinder;
import android.os.RemoteException;
import android.view.View;

import androidx.annotation.Nullable;

import com.miduo.player.Player;

public class PlayerService extends Service {
    Player player=new Player();
    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return playBinder;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        //player.prepare(Environment.getExternalStorageDirectory()+"/心雨.mp3");
        return super.onStartCommand(intent, flags, startId);
    }



    private Binder  playBinder=new IPlayInterface.Stub()
    {

        @Override
        public void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat, double aDouble, String aString) throws RemoteException {

        }

        @Override
        public void play(String url) throws RemoteException {
            player.prepare(url);
        }

        @Override
        public void pause() throws RemoteException {

            player.pause();
        }

        @Override
        public void resume() throws RemoteException {

            player.resume();
        }

        @Override
        public void stop() throws RemoteException {

            player.stop();
        }

        @Override
        public void seek(int seek) throws RemoteException {

            player.seek(seek);
        }

        @Override
        public void leftVoice() throws RemoteException {

            player.setleftVoice();
        }

        @Override
        public void rightVoice() throws RemoteException {

            player.rightVoice();
        }

        @Override
        public void steroVoice() throws RemoteException {

            player.steroVoice();
        }

        @Override
        public void setPitch(float pitch) throws RemoteException {

            player.setPitch(pitch);
        }

        @Override
        public void setSpeed(float speed) throws RemoteException {

            player.setSpeed(speed);
        }

        @Override
        public void setVolume(int voice) throws RemoteException {
            player.setVolume(voice);
        }
    };

}
