package com.yangwenbo.caffesimple;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.yangwenbo.caffemobile.CaffeMobile;

import java.io.File;
import java.util.Arrays;

public class MainActivity extends AppCompatActivity {
    static private String TAG = "MainActivity";
    private CaffeMobile _cm;

    protected class Cate implements Comparable<Cate> {
        public final int    idx;
        public final float  prob;

        public Cate(int idx, float prob) {
            this.idx = idx;
            this.prob = prob;
        }

        @Override
        public int compareTo(Cate other) {
            // need descending sort order
            return Float.compare(other.prob, this.prob);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        TextView tv = (TextView) findViewById(R.id.Console);
        tv.append("Loading caffe model...");
        tv.setMovementMethod(new ScrollingMovementMethod());
        // Show test image
        final File imageFile = new File(Environment.getExternalStorageDirectory(), "test_image.jpg");
        Bitmap bmp = BitmapFactory.decodeFile(imageFile.getPath());
        ImageView img = (ImageView) findViewById(R.id.testImage);
        img.setImageBitmap(bmp);

        // Load caffe model
        long start_time = System.nanoTime();
        File modelFile = new File(Environment.getExternalStorageDirectory(), "net.protobin");
        File weightFile = new File(Environment.getExternalStorageDirectory(), "weight.caffemodel");
        Log.d(TAG, "onCreate: modelFile:" + modelFile.getPath());
        Log.d(TAG, "onCreate: weightFIle:" + weightFile.getPath());
        _cm = new CaffeMobile();
        boolean res = _cm.loadModel(modelFile.getPath(), weightFile.getPath());
        long end_time = System.nanoTime();
        double difference = (end_time - start_time)/1e6;
        Log.d(TAG, "onCreate: loadmodel:" + res);
        tv.append(String.valueOf(difference) + "ms\n");

        //_cm.setBlasThreadNum(2);
        Button btn = (Button) findViewById(R.id.button);
        btn.setOnClickListener(new View.OnClickListener() {
            // Run test
            @Override
            public void onClick(View view) {
                final TextView tv = (TextView) findViewById(R.id.Console);
                tv.append("\nCaffe inferring...\n");
                final Handler myHandler = new Handler(){
                    @Override
                    public void handleMessage(Message msg) {
                        tv.append((String)msg.obj);
                    }
                };
                (new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Message msg = myHandler.obtainMessage();
                        long start_time = System.nanoTime();
                        float mean[] = {81.3f, 107.3f, 105.3f};
                        float[] result = _cm.predictImage(imageFile.getPath(), mean);
                        long end_time = System.nanoTime();
                        if (null != result) {
                            double difference = (end_time - start_time) / 1e6;
                            // Top 10
                            int topN = 10;
                            Cate[] cates = new Cate[result.length];
                            for (int i = 0; i < result.length; i++) {
                                cates[i] = new Cate(i, result[i]);
                            }
                            Arrays.sort(cates);
                            msg.obj = "Top" + topN + " Results (" + String.valueOf(difference) + "ms):\n";
                            for (int i = 0; i < topN; i++) {
                                msg.obj += "output[" + cates[i].idx + "]\t=" + cates[i].prob + "\n";
                            }
                        } else {
                            msg.obj = "output=null (some error happens)";
                        }
                        myHandler.sendMessage(msg);
                    }
                })).start();
            }
        });
    }

    // Used to load the 'caffe-jni' library on application startup.
    static {
        System.loadLibrary("caffe-jni");
    }
}
