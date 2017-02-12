package com.yangwenbo.caffesimple;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;

import java.io.File;
import java.nio.ByteBuffer;
import java.nio.IntBuffer;

import ar.com.hjg.pngj.ImageLineByte;
import ar.com.hjg.pngj.PngReaderByte;

public class MainActivity extends AppCompatActivity {
    static private String TAG = "MainActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        File image_file = new File(Environment.getExternalStorageDirectory(), "test_image.png");
        CaffeImage image = ReadGrayPngToPixel(image_file.getPath());
        // Example of a call to a native method
        TextView tv = (TextView) findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());
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

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }


}
