package com.yangwenbo.caffemobile;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.File;
import java.nio.ByteBuffer;

import ar.com.hjg.pngj.ImageLineByte;
import ar.com.hjg.pngj.PngReaderByte;

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
    private native float[] predict(byte[] bitmap, int width, int height, int channels);

    public void predictImage(String fileName) {
        CaffeImage image = ReadGrayPngToPixel(fileName);
        float[] result = predict(image.pixels, image.width, image.height, image.channels);
        for (int i = 0; i<result.length; i++) {
            Log.i(TAG, "predictImage:  " + result[i]);
        }
    }

    class CaffeImage {
        int channels;
        int width;
        int height;
        byte[] pixels;
    };

    /**
     * @brief Specified for  MNIST 1 channel grayscale png reading
     * @param file_name
     * @return
     */
    protected CaffeImage ReadGrayPngToPixel(String file_name) {
        PngReaderByte pngr = new PngReaderByte(new File(file_name));
        if (pngr.imgInfo.channels!=1 || pngr.imgInfo.bitDepth != 8 || pngr.imgInfo.indexed) {
            throw new RuntimeException("This method is for gray images");
        }
        CaffeImage image = new CaffeImage();
        image.channels = pngr.imgInfo.channels;
        image.width = pngr.imgInfo.bytesPerRow;
        image.height = pngr.imgInfo.rows;
        image.pixels = new byte[image.width*image.height];
        Log.i(TAG, "ReadGrayscalePngToPixel: {" + image.channels + "," + image.width + ","
                + image.height + "}");
        for (int row = 0; row < pngr.imgInfo.rows; row++) {
            ImageLineByte line = pngr.readRowByte();
            System.arraycopy(line.getScanlineByte(), 0, image.pixels, row*image.width, image.width);
        }
        pngr.end();
        return image;
    }

    /**
     * @brief Read a image from file to BGR pixels buffer (OpenCV)
     * @param file_name
     * @return
     */
    protected CaffeImage ReadColorImageToBGRBuf(String file_name) {
        // Read image file to bitmap (in ARGB format)
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;
        Bitmap bitmap = BitmapFactory.decodeFile(file_name, options);
        // Copy bitmap pixels to buffer
        ByteBuffer argb_buf = ByteBuffer.allocate(bitmap.getByteCount());
        bitmap.copyPixelsToBuffer(argb_buf);
        // Get the underlying array containing the ARGB pixels
        byte[] argb = argb_buf.array();
        CaffeImage image = new CaffeImage();
        image.channels = 3;
        image.width = bitmap.getWidth();
        image.height = bitmap.getHeight();
        // Allocate array for 3 bytes BGR pixels
        image.pixels = new byte[(argb.length / 4) * 3];
        // Copy pixels into place
        for (int i = 0; i < (argb.length / 4); i++) {
            image.pixels[i * 3] = argb[i * 4 + 3];     // B
            image.pixels[i * 3 + 1] = argb[i * 4 + 2]; // G
            image.pixels[i * 3 + 2] = argb[i * 4 + 1]; // R
            // Alpha is discarded
        }
        return image;
    }

}
