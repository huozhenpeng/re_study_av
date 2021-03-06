package com.miduo.restudyav;

import android.Manifest;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import com.miduo.restudyav.Widget.DbLineView;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    Intent intent;
    DbLineView lineview;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, PERMISSIONS_STORAGE, REQUEST_PERMISSION_CODE);
            }
        }

        intent=new Intent(MainActivity.this,PlayerService.class);
        startService(intent);
        bindService(intent,serviceConnection,BIND_WAIVE_PRIORITY);

        lineview=findViewById(R.id.lineview);



        final Button  button=findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                button.setText(button.getText()+":"+getBestSampleRate());

            }
        });

        final Button decode=findViewById(R.id.decode);
        decode.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {

                try {
                    player.play(Environment.getExternalStorageDirectory()+"/心雨.mp3");
                } catch (RemoteException e) {
                    e.printStackTrace();
                }

            }
        });

        final Button native_play=findViewById(R.id.native_play);
        native_play.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                MediaPlayer mp = new MediaPlayer();
                try {
                    mp.setDataSource(Environment.getExternalStorageDirectory()+"/心雨.mp3");
                    mp.prepare();
                    mp.start();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        });

        final Button pause=findViewById(R.id.pause);
        pause.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                try {
                    player.pause();
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });

        final Button resume=findViewById(R.id.resume);
        resume.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                try {
                    player.resume();
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });

        final Button stop=findViewById(R.id.stop);
        stop.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                try {
                    player.stop();
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });

        final Button seek=findViewById(R.id.seek);
        seek.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                try {
                    player.seek(270);
                } catch (RemoteException e) {
                    e.printStackTrace();
                }
            }
        });

        final Button volume=findViewById(R.id.volume);
        volume.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                try {
                    player.setVolume(50);
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

    public void leftVoice(View view) {
        try {
            player.leftVoice();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void rightVoice(View view) {
        try {
            player.rightVoice();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void steroVoice(View view) {
        try {
            player.steroVoice();
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }


    public void setPitch(View view) {
        try {
            player.setPitch(1.5f);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void setSpeed(View view) {
        try {
            player.setSpeed(1.5f);
        } catch (RemoteException e) {
            e.printStackTrace();
        }

    }
    @Override
    protected void onPause() {
        super.onPause();
    }

    private IPlayInterface player;
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
                    lineview.setCurrentDb(msg.arg2,msg.arg1);
                    break;
                case 0x02:
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


}
