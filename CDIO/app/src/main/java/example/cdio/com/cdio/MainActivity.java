package example.cdio.com.cdio;

import android.Manifest;
import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.ColorFilter;
import android.graphics.ColorMatrix;
import android.graphics.ColorMatrixColorFilter;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.media.ExifInterface;
import android.net.Uri;
import android.os.Environment;
import android.provider.MediaStore;
import android.renderscript.Allocation;
import android.renderscript.Element;
import android.renderscript.RenderScript;
import android.renderscript.ScriptIntrinsicBlur;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ImageView;
import android.widget.Toast;


import java.io.File;
import java.io.IOException;
import java.nio.IntBuffer;
import java.text.SimpleDateFormat;
import java.util.Date;

public class MainActivity extends AppCompatActivity {

    private static final int ACTIVITY_START_CAMERA_APP = 0;
    private ImageView mPhotoCapturedImageView;
    private String mImageFileLocation = "";
    public Bitmap minBild;
    public Bitmap minBlurBild;
    public Bitmap minGrayBild;

    // Storage Permissions variables
    private static final int REQUEST_EXTERNAL_STORAGE = 1;
    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.READ_EXTERNAL_STORAGE,
            Manifest.permission.WRITE_EXTERNAL_STORAGE
    };

    //persmission method.
    public static void verifyStoragePermissions(Activity activity) {
        // Check if we have read or write permission
        int writePermission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.WRITE_EXTERNAL_STORAGE);
        int readPermission = ActivityCompat.checkSelfPermission(activity, Manifest.permission.READ_EXTERNAL_STORAGE);

        if (writePermission != PackageManager.PERMISSION_GRANTED || readPermission != PackageManager.PERMISSION_GRANTED) {
            // We don't have permission so prompt the user
            ActivityCompat.requestPermissions(
                    activity,
                    PERMISSIONS_STORAGE,
                    REQUEST_EXTERNAL_STORAGE
            );
        }
    }
    ////BLUR

    ///
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        verifyStoragePermissions(this);
        mPhotoCapturedImageView = (ImageView) findViewById(R.id.capturePhotoImageView);

    }


    public void takePhoto(View view) {

        Intent callCameraApplicationIntent = new Intent();
        callCameraApplicationIntent.setAction(MediaStore.ACTION_IMAGE_CAPTURE); // gör så man kallar på kameran

        File photoFile = null;
        try {
            photoFile = createImageFile();

        } catch (IOException e) {
            e.printStackTrace();
        }
        callCameraApplicationIntent.putExtra(MediaStore.EXTRA_OUTPUT, Uri.fromFile(photoFile));

        startActivityForResult(callCameraApplicationIntent, ACTIVITY_START_CAMERA_APP);

    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        // Check which request we're responding to
        if (requestCode == ACTIVITY_START_CAMERA_APP && resultCode == RESULT_OK) { // Checking if request was succes!
            //Toast.makeText(this, "Picture was taken", Toast.LENGTH_LONG).show();

            // Bundle extras = data.getExtras();
            // Bitmap photoCapturedBitmap = (Bitmap) extras.get("data"); //Getting data.
            //mPhotoCapturedImageView.setImageBitmap(photoCapturedBitmap);

            // Bitmap photoCapturedBitmap = BitmapFactory.decodeFile(mImageFileLocation); //ifall man inte ska ha reduced size
            // mPhotoCapturedImageView.setImageBitmap(photoCapturedBitmap);

            minBild = setReducedImageSize();
            minBild = ConvertToGrayscale(minBild);
            minGrayBild = createInvertedBitmap(minBild);
            for(int i = 0; i < 6; i ++) {
                minGrayBild = blurImage(getApplicationContext(), minGrayBild);
            }

            minBild = ColorDodgeBlend(minGrayBild, minBild);
            rotateImage(minBild);


            //tar upp mindre wjam
            //blackNwhite(setReducedImageSize());

            // Make sure the request was successful
            // The user picked a contact.
            // The Intent's data Uri identifies which contact was selected.
            // Do something with the contact here (bigger example below)

        }
    }

    File createImageFile() throws IOException {

        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date());
        String imageFileName = "IMAGE_" + timeStamp + "_";
        File storageDirectory = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES);

        File image = File.createTempFile(imageFileName, ".jpg", storageDirectory);
        mImageFileLocation = image.getAbsolutePath();

        return image;
    }

    //Reduce image size. För wjam
    private Bitmap setReducedImageSize() {
        //void setReducedImageSize(){
        int targetImageViewWidth = mPhotoCapturedImageView.getWidth();
        int targetImageViewHeight = mPhotoCapturedImageView.getHeight();

        BitmapFactory.Options bmOptions = new BitmapFactory.Options();
        bmOptions.inJustDecodeBounds = true;
        BitmapFactory.decodeFile(mImageFileLocation, bmOptions);
        int cameraImageWidth = bmOptions.outWidth;
        int cameraImageHeight = bmOptions.outHeight;

        int scaleFactor = Math.min(cameraImageWidth / targetImageViewWidth, cameraImageHeight / targetImageViewHeight);
        bmOptions.inSampleSize = scaleFactor;
        bmOptions.inJustDecodeBounds = false;

        // Bitmap photoReducedSizeBitmap = BitmapFactory.decodeFile(mImageFileLocation, bmOptions);
        // mPhotoCapturedImageView.setImageBitmap(photoReducedSizeBitmap);
        return BitmapFactory.decodeFile(mImageFileLocation, bmOptions);

    }

    //Rotera bild!
    private void rotateImage(Bitmap bitmap) {
        ExifInterface exifInterface = null;
        try {
            exifInterface = new ExifInterface(mImageFileLocation);
        } catch (IOException e) {
            e.printStackTrace();
        }
        int orientation = exifInterface.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_UNDEFINED);

        Matrix matrix = new Matrix();
        switch (orientation) {
            case ExifInterface.ORIENTATION_ROTATE_90:
                matrix.setRotate(90);
                break;
            case ExifInterface.ORIENTATION_ROTATE_180:
                matrix.setRotate(180);
                break;
            case ExifInterface.ORIENTATION_ROTATE_270:
                matrix.setRotate(270);
            default:
        }
        Bitmap rotateBitmap = Bitmap.createBitmap(bitmap, 0, 0, bitmap.getWidth(), bitmap.getHeight(), matrix, true);
        mPhotoCapturedImageView.setImageBitmap(rotateBitmap);
    }

    // Testar o göra en bild svart/vit
    public Bitmap ConvertToGrayscale(Bitmap bitmap) {
        int height = bitmap.getHeight();
        int width = bitmap.getWidth();

        float[] arrayForColorMatrix = new float[]{0, 0, 0, 0, 0,
                0, 0, 0, 0, 0,
                0, 0, 1, 0, 0,
                0, 0, 0, 1, 0};

        Bitmap.Config config = bitmap.getConfig();
        Bitmap grayScaleBitmap = Bitmap.createBitmap(width, height, config);

        Canvas c = new Canvas(grayScaleBitmap);
        Paint paint = new Paint();

        ColorMatrix matrix = new ColorMatrix(arrayForColorMatrix);
        matrix.setSaturation(0);

        ColorMatrixColorFilter filter = new ColorMatrixColorFilter(matrix);
        paint.setColorFilter(filter);

        c.drawBitmap(bitmap, 0, 0, paint);


        return grayScaleBitmap;
    }

    //Inverterar bild
    public static Bitmap createInvertedBitmap(Bitmap src) {
        ColorMatrix colorMatrix_Inverted =
                new ColorMatrix(new float[]{
                        -1, 0, 0, 0, 255,
                        0, -1, 0, 0, 255,
                        0, 0, -1, 0, 255,
                        0, 0, 0, 1, 0});

        ColorFilter ColorFilter_Sepia = new ColorMatrixColorFilter(
                colorMatrix_Inverted);

        Bitmap bitmap = Bitmap.createBitmap(src.getWidth(), src.getHeight(),
                Bitmap.Config.ARGB_8888);
        Canvas canvas = new Canvas(bitmap);

        Paint paint = new Paint();

        paint.setColorFilter(ColorFilter_Sepia);
        canvas.drawBitmap(src, 0, 0, paint);


        return bitmap;
    }
    private static final float BLUR_RADIUS = 25f;
    //BLUR
    public static Bitmap blurImage (Context ctx, Bitmap src){

        final RenderScript rs = RenderScript.create(ctx);
        final Allocation input = Allocation.createFromBitmap( rs, src, Allocation.MipmapControl.MIPMAP_NONE, Allocation.USAGE_SCRIPT );
        final Allocation output = Allocation.createTyped( rs, input.getType() );
        final ScriptIntrinsicBlur script = ScriptIntrinsicBlur.create( rs, Element.U8_4( rs ) );
        script.setRadius( BLUR_RADIUS  );
        script.setInput( input );
        script.forEach( output );
        output.copyTo(src);
        return src;
    }

    //// Color dodge blend! merge hetc

    private int colordodge(int in1, int in2) {
        float image = (float)in2;
        float mask = (float)in1;
        return ((int) ((image == 255) ? image:Math.min(255, (((long)mask << 8 ) / (255 - image)))));

    }

    /**
     * Blends 2 bitmaps to one and adds the color dodge blend mode to it.
     */
    public Bitmap ColorDodgeBlend(Bitmap source, Bitmap layer) {
        Bitmap base = source.copy(Bitmap.Config.ARGB_8888, true);
        Bitmap blend = layer.copy(Bitmap.Config.ARGB_8888, false);

        IntBuffer buffBase = IntBuffer.allocate(base.getWidth() * base.getHeight());
        base.copyPixelsToBuffer(buffBase);
        buffBase.rewind();

        IntBuffer buffBlend = IntBuffer.allocate(blend.getWidth() * blend.getHeight());
        blend.copyPixelsToBuffer(buffBlend);
        buffBlend.rewind();

        IntBuffer buffOut = IntBuffer.allocate(base.getWidth() * base.getHeight());
        buffOut.rewind();

        while (buffOut.position() < buffOut.limit()) {
            int filterInt = buffBlend.get();
            int srcInt = buffBase.get();

            int redValueFilter = Color.red(filterInt);
            int greenValueFilter = Color.green(filterInt);
            int blueValueFilter = Color.blue(filterInt);

            int redValueSrc = Color.red(srcInt);
            int greenValueSrc = Color.green(srcInt);
            int blueValueSrc = Color.blue(srcInt);

            int redValueFinal = colordodge(redValueFilter, redValueSrc);
            int greenValueFinal = colordodge(greenValueFilter, greenValueSrc);
            int blueValueFinal = colordodge(blueValueFilter, blueValueSrc);

            int pixel = Color.argb(255, redValueFinal, greenValueFinal, blueValueFinal);

            buffOut.put(pixel);
        }

        buffOut.rewind();

        base.copyPixelsFromBuffer(buffOut);
        blend.recycle();

        return base;
    }


}