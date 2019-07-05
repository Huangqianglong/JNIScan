package com.hql.jniscan;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
    TextView tv;
    JNI jni;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tv = findViewById(R.id.sample_text);
        jni = new JNI();
        tv.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d("hql","点击事件"+encryptStr("1233213"));
                jni.clearCatch();
                long start = System.currentTimeMillis();
                Log.d("hql","点击事件开始："+start);
                jni.scanDir("/storage/005B-0D07/");
                Log.d("hql","点击事件结束 耗时："+(System.currentTimeMillis()-start)
                        +"\n  音频数量:"+jni.getCacheSize());
            }
        });
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
    public native String encryptStr(String msg);
}
