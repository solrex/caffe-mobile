package com.yangwenbo.caffesimple;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import com.yangwenbo.caffemobile.CaffeMobile;

import java.io.File;

public class MainActivity extends AppCompatActivity {
    static private String TAG = "MainActivity";
    private CaffeMobile _cm;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tv = (TextView) findViewById(R.id.Console);
        tv.append("Loading caffe model...");
        tv.setMovementMethod(new ScrollingMovementMethod());

        // Load caffe model
        long start_time = System.nanoTime();
        File modelFile = new File(Environment.getExternalStorageDirectory(), "net.prototxt");
        File weightFile = new File(Environment.getExternalStorageDirectory(), "weight.caffemodel");
        Log.d(TAG, "onCreate: modelFile:" + modelFile.getPath());
        Log.d(TAG, "onCreate: weightFIle:" + weightFile.getPath());
        _cm = new CaffeMobile();
        boolean res = _cm.loadModel(modelFile.getPath(), weightFile.getPath());
        long end_time = System.nanoTime();
        double difference = (end_time - start_time)/1e6;
        Log.d(TAG, "onCreate: loadmodel:" + res);
        tv.append(String.valueOf(difference) + "ms\n");

        Button btn = (Button) findViewById(R.id.button);
        btn.setOnClickListener(new View.OnClickListener() {
            // Run test
            @Override
            public void onClick(View view) {
                TextView tv = (TextView) findViewById(R.id.Console);
                tv.append("\nCaffe inferring...");
                long start_time = System.nanoTime();
                File imageFile = new File(Environment.getExternalStorageDirectory(), "test_image.png");
                float[] result = _cm.predictImage(imageFile.getPath());
                long end_time = System.nanoTime();
                double difference = (end_time - start_time)/1e6;
                tv.append(String.valueOf(difference) + "ms\n");
                for (int i = 0; i < result.length; i++) {
                    tv.append("result[" + i + "]\t=" + result[i] + "\n");
                }
            }
        });
    }

    // Used to load the 'caffe-jni' library on application startup.
    static {
        System.loadLibrary("caffe-jni");
    }
}
