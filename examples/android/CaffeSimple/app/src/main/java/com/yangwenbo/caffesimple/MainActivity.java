package com.yangwenbo.caffesimple;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import com.yangwenbo.caffemobile.CaffeMobile;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    static private String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        File imageFile = new File(Environment.getExternalStorageDirectory(), "test_image.png");
        File modelFile = new File(Environment.getExternalStorageDirectory(), "net.prototxt");
        File weightFile = new File(Environment.getExternalStorageDirectory(), "weight.caffemodel");

        Log.i(TAG, "onCreate: modelFile:" + modelFile.getPath());
        Log.i(TAG, "onCreate: weightFIle:" + weightFile.getPath());
        CaffeMobile cm = new CaffeMobile();
        boolean res = cm.loadModel(modelFile.getPath(), weightFile.getPath());
        Log.i(TAG, "onCreate: loadmodel:" + res);
        cm.predictImage(imageFile.getPath());


        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText("onCreate: loadmodel:" + res);


    }

    // Used to load the 'caffe-jni' library on application startup.
    static {
        System.loadLibrary("caffe-jni");
    }
}
