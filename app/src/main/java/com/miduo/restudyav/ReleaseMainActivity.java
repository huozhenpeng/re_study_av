package com.miduo.restudyav;

import android.Manifest;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ImageView;
import android.widget.SeekBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.AppCompatSeekBar;
import androidx.core.app.ActivityCompat;

import com.miduo.restudyav.Widget.DbLineView;


public class ReleaseMainActivity extends AppCompatActivity {

    Intent intent;
    AppCompatSeekBar progress_seek;
    ImageView iv_play;
    boolean isSeek=false;
    int position=0;

    TextView tv_start;
    TextView tv_end;

    DbLineView lineview;

    TextView tv_cut;

    @RequiresApi(api = Build.VERSION_CODES.M)
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main_release);
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, PERMISSIONS_STORAGE, REQUEST_PERMISSION_CODE);
            }
        }
        lineview=findViewById(R.id.lineview);

        progress_seek=findViewById(R.id.progress_seek);
        iv_play=findViewById(R.id.iv_play);

        tv_start=findViewById(R.id.tv_start);
        tv_end=findViewById(R.id.tv_end);

        tv_cut=findViewById(R.id.tv_cut);
        intent=new Intent(ReleaseMainActivity.this,PlayerService.class);
        startService(intent);
        bindService(intent,serviceConnection,BIND_WAIVE_PRIORITY);



        progress_seek.setMax(100);
       progress_seek.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
           @Override
           public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
               if(isSeek&&fromUser)
               {
                   position=progress*time/100;
               }
           }

           @Override
           public void onStartTrackingTouch(SeekBar seekBar) {

               isSeek=true;
           }

           @Override
           public void onStopTrackingTouch(SeekBar seekBar) {

               isSeek=false;
               try {
                   player.seek(position);
               } catch (RemoteException e) {
                   e.printStackTrace();
               }
           }
       });

    }

    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE};
    //请求状态码
    private static int REQUEST_PERMISSION_CODE = 1;

    public int getBestSampleRate()
    {
        if (Build.VERSION.SDK_INT >= 17) {
            AudioManager am = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
            String sampleRateString = am.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
            int sampleRate = sampleRateString == null ? 44100 : Integer.parseInt(sampleRateString);
            return sampleRate;
        } else {
            return 44100;
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    private IPlayInterface player;
    private int time;
    private ICallBackInterface callBackInterface=new ICallBackInterface.Stub() {

        @Override
        public void basicTypes(int anInt, long aLong, boolean aBoolean, float aFloat, double aDouble, String aString) throws RemoteException {

        }

        @Override
        public void dbCallBack(int db, int currentTime) throws RemoteException {

            Message message=Message.obtain();
            message.what=0x01;
            message.arg1=db;
            message.arg2=currentTime;
            handler.sendMessage(message);
        }

        @Override
        public void totalTimeCallBack(int totalTime) throws RemoteException {
            time=totalTime;
            Message message=Message.obtain();
            message.what=0x02;
            message.arg1=totalTime;
            handler.sendMessage(message);
        }

    };

    private Handler handler=new Handler(){

        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            switch (msg.what)
            {
                case 0x01:
                    progress_seek.setProgress(msg.arg2*100/time);
                    tv_start.setText(Utils.secToTime(msg.arg2));
                    lineview.setCurrentDb(msg.arg2,msg.arg1);
                    break;
                case 0x02:
                    tv_end.setText(Utils.secToTime(msg.arg1));
                    lineview.setTotalTime(msg.arg1);
                    break;
            }
        }
    };

    private ServiceConnection serviceConnection=new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            player=IPlayInterface.Stub.asInterface(service);
            try {
                player.registerListener(callBackInterface);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    boolean isPlay=false;
    boolean isStart=false;
    /**
     * 播放 暂停 恢复
     * @param view
     */
    public void play(View view) throws RemoteException {
        if(isStart)
        {
            if(isPlay)
            {
                isPlay=false;
                player.pause();
                iv_play.setImageResource(R.mipmap.pause);

            }
            else
            {
                isPlay=true;
                player.resume();
                iv_play.setImageResource(R.mipmap.play);
            }
        }
        else
        {
            isStart=true;
            isPlay=true;
            iv_play.setImageResource(R.mipmap.play);
            player.play(Environment.getExternalStorageDirectory()+"/心雨.mp3");
        }
    }


    boolean isCut=false;
    public void special(View view) throws RemoteException {

        if(isCut)
        {
            isCut=false;
            tv_cut.setText("开始裁剪");
            player.startRecord(Environment.getExternalStorageDirectory().getPath()+"/xinyu.aac",false);
        }
        else
        {
            isCut=true;
            tv_cut.setText("结束裁剪");
            player.startRecord(Environment.getExternalStorageDirectory().getPath()+"/xinyu.aac",true);

        }
    }


    public void leftVoice(View view) throws RemoteException {
        player.leftVoice();
    }

    public void rightVoice(View view) throws RemoteException {
        player.rightVoice();
    }

    public void reverse(View view) throws RemoteException {
        player.setPitch(1.0f);
        player.setSpeed(1.0f);
    }

    public void setSpeed(View view) throws RemoteException {
        player.setSpeed(1.5f);
    }

    public void setPitch(View view) throws RemoteException {

        player.setPitch(1.5f);
    }

    public void sterao(View view) throws RemoteException {
        player.steroVoice();
    }
}
