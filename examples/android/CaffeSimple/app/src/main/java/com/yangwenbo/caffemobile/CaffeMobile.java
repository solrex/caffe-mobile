package com.yangwenbo.caffemobile;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Created by solrex on 2017/2/18.
 */

public class CaffeMobile {
    private static String TAG = "CaffeMobile";

    /**
     * A native method that is implemented by the 'caffe-jni' native library,
     * which is packaged with this application.
     */
    public native boolean loadModel(String modelPath, String weightPath);

    /**
     * A native method that is implemented by the 'caffe-jni' native library,
     * which is packaged with this application.
     */
    public native void setBlasThreadNum(int numThreads);

    /**
     * A native method that is implemented by the 'caffe-jni' native library,
     * which is packaged with this application.
     */
    public native int inputChannels();

    /**
     * A native method that is implemented by the 'caffe-jni' native library,
     * which is packaged with this application.
     */
    public native int inputWidth();

    /**
     * A native method that is implemented by the 'caffe-jni' native library,
     * which is packaged with this application.
     */
    public native int inputHeight();

    /**
     * A native method that is implemented by the 'caffe-jni' native library,
     * which is packaged with this application.
     */
    private native float[] predict(byte[] bitmap, int channels, float[]mean);

    public float[] predictImage(String fileName, float[] mean) {
        CaffeImage image = readImage(fileName);
        float[] result = predict(image.pixels, image.channels, mean);
        return result;
    }

    class CaffeImage {
        int channels;
        int width;
        int height;
        byte[] pixels;
    };

    /**
     * @brief Read a image from file to BGR pixels buffer (OpenCV)
     * @param file_name
     * @param channels
     * @return
     */
    protected CaffeImage readImage(String file_name) {
        Log.i(TAG, "readImage: reading: " + file_name);
        // Read image file to bitmap (in ARGB format)
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        options.inPremultiplied = false;
        Bitmap bitmap = BitmapFactory.decodeFile(file_name, options);

        // Copy bitmap pixels to buffer
        ByteBuffer argb_buf = ByteBuffer.allocate(bitmap.getByteCount());
        bitmap.copyPixelsToBuffer(argb_buf);

        // Generate CaffeImage to classification
        CaffeImage image = new CaffeImage();
        image.width = bitmap.getWidth();
        image.height = bitmap.getHeight();
        image.channels = 4;
        Log.i(TAG, "readImage: image CxWxH: " + image.channels + "x" + image.width + "x" + image.height);
        // Get the underlying array containing the ARGB pixels
        image.pixels = argb_buf.array();
        Log.d(TAG, "readImage: bitmap(0,0)="
                + Integer.toHexString(bitmap.getPixel(0, 0))
                + ", rgba[0,0]="
                + Integer.toHexString((image.pixels[0] << 24 & 0xff000000) | (image.pixels[1] << 16 & 0xff0000)
                                      | (image.pixels[2] << 8 & 0xff00) | (image.pixels[3] & 0xff) ));
        return image;
    }
}
