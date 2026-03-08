# Sad Android SDK — قواعد ProGuard

# Keep native methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# Keep Sad SDK classes
-keep class sad.lang.android.** { *; }

# Keep Kotlin coroutines
-keepnames class kotlinx.coroutines.internal.MainDispatcherFactory {}
-keepnames class kotlinx.coroutines.CoroutineExceptionHandler {}

# Keep OkHttp
-dontwarn okhttp3.**
-dontwarn okio.**
-keep class okhttp3.** { *; }

# Keep Compose
-keep class androidx.compose.** { *; }
