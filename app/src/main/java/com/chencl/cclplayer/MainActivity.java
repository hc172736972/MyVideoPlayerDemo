package com.chencl.cclplayer;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
    private boolean isOnlySoft = false;
    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("avutil");
        System.loadLibrary("fdk-aac");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("swscale");
        System.loadLibrary("swresample");
        System.loadLibrary("avfilter");
        System.loadLibrary("ccl-player");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
    }

    public void setOnlySoft(boolean soft)
    {
        this.isOnlySoft = soft;
    }

    public boolean isOnlySoft()
    {
        return isOnlySoft;
    }

    public void mediacodecInit(int mimetype, int width, int height, byte[] csd0, byte[] csd1)
    {

    }
    private void onParpared()
    {

    }
    public void videoComplete() {

    }
    private void onError(int code,String msg) {

    }

    private void  onLoad(boolean load) {

    }




    public native void cclPrepared(String url,boolean isOnlyMusic);

    public native void cclStart();





}