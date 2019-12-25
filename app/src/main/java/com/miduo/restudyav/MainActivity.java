package com.miduo.restudyav;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.miduo.player.Player;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP) {
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, PERMISSIONS_STORAGE, REQUEST_PERMISSION_CODE);
            }
        }
        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        final Player player=new Player();
        tv.setText(player.stringFromJNI());
        //player.printCodec();

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
                player.prepare(Environment.getExternalStorageDirectory()+"/心雨.mp3");
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
                player.pause();
            }
        });

        final Button resume=findViewById(R.id.resume);
        resume.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                player.resume();
            }
        });

        final Button stop=findViewById(R.id.stop);
        stop.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                player.stop();
            }
        });

        final Button seek=findViewById(R.id.seek);
        seek.setOnClickListener(new View.OnClickListener(){

            @Override
            public void onClick(View v) {
                player.seek(270);
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
}
