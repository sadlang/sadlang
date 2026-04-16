package com.sad.app;

import android.view.ViewGroup;

/**
 * محرك ص الأصلي — واجهة JNI للتواصل مع libsad_app.so
 * Sad Engine — JNI interface to native libsad_app.so
 */
public class SadEngine {
    
    static {
        System.loadLibrary("sad_app");
    }
    
    // Native methods
    public static native boolean nativeInit(Object activity);
    public static native boolean nativeExecute(String code);
    public static native String nativeGetLastError();
    public static native String nativeGetLastOutput();
    public static native void nativeShutdown();
    public static native String nativeGetVersion();
    public static native String nativeGetUIHtml();
    
    // واجهة أصلية (بدون WebView)
    public static native boolean nativeHasNativeUI();
    public static native boolean nativeBuildNativeUI(ViewGroup container);
}
