// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadLifecycleBridge.kt
// الوصف: جسر Kotlin لدورة حياة Android (Lifecycle)
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.app.Application
import android.content.Context
import android.os.Bundle
import androidx.lifecycle.*

/**
 * جسر دورة الحياة — SadLifecycleBridge
 */
object SadLifecycleBridge {
    
    private var application: Application? = null
    private val lifecycleObservers = mutableMapOf<String, LifecycleObserver>()
    
    // Callbacks
    private var onActivityCreatedCallback: ((Activity) -> Unit)? = null
    private var onActivityStartedCallback: ((Activity) -> Unit)? = null
    private var onActivityResumedCallback: ((Activity) -> Unit)? = null
    private var onActivityPausedCallback: ((Activity) -> Unit)? = null
    private var onActivityStoppedCallback: ((Activity) -> Unit)? = null
    private var onActivityDestroyedCallback: ((Activity) -> Unit)? = null
    
    private var onAppForegroundCallback: (() -> Unit)? = null
    private var onAppBackgroundCallback: (() -> Unit)? = null
    
    private val activityLifecycleCallbacks = object : Application.ActivityLifecycleCallbacks {
        override fun onActivityCreated(activity: Activity, savedInstanceState: Bundle?) {
            onActivityCreatedCallback?.invoke(activity)
        }
        
        override fun onActivityStarted(activity: Activity) {
            onActivityStartedCallback?.invoke(activity)
        }
        
        override fun onActivityResumed(activity: Activity) {
            onActivityResumedCallback?.invoke(activity)
        }
        
        override fun onActivityPaused(activity: Activity) {
            onActivityPausedCallback?.invoke(activity)
        }
        
        override fun onActivityStopped(activity: Activity) {
            onActivityStoppedCallback?.invoke(activity)
        }
        
        override fun onActivitySaveInstanceState(activity: Activity, outState: Bundle) {}
        
        override fun onActivityDestroyed(activity: Activity) {
            onActivityDestroyedCallback?.invoke(activity)
        }
    }
    
    private val processLifecycleObserver = object : DefaultLifecycleObserver {
        override fun onStart(owner: LifecycleOwner) {
            onAppForegroundCallback?.invoke()
        }
        
        override fun onStop(owner: LifecycleOwner) {
            onAppBackgroundCallback?.invoke()
        }
    }
    
    @JvmStatic
    fun initialize(ctx: Context) {
        application = ctx.applicationContext as? Application
        application?.registerActivityLifecycleCallbacks(activityLifecycleCallbacks)
        ProcessLifecycleOwner.get().lifecycle.addObserver(processLifecycleObserver)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Activity Lifecycle Callbacks
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnActivityCreatedListener(callback: (Activity) -> Unit) {
        onActivityCreatedCallback = callback
    }
    
    @JvmStatic
    fun setOnActivityStartedListener(callback: (Activity) -> Unit) {
        onActivityStartedCallback = callback
    }
    
    @JvmStatic
    fun setOnActivityResumedListener(callback: (Activity) -> Unit) {
        onActivityResumedCallback = callback
    }
    
    @JvmStatic
    fun setOnActivityPausedListener(callback: (Activity) -> Unit) {
        onActivityPausedCallback = callback
    }
    
    @JvmStatic
    fun setOnActivityStoppedListener(callback: (Activity) -> Unit) {
        onActivityStoppedCallback = callback
    }
    
    @JvmStatic
    fun setOnActivityDestroyedListener(callback: (Activity) -> Unit) {
        onActivityDestroyedCallback = callback
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // App State Callbacks
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnAppForegroundListener(callback: () -> Unit) {
        onAppForegroundCallback = callback
    }
    
    @JvmStatic
    fun setOnAppBackgroundListener(callback: () -> Unit) {
        onAppBackgroundCallback = callback
    }
    
    @JvmStatic
    fun isAppInForeground(): Boolean {
        return ProcessLifecycleOwner.get().lifecycle.currentState.isAtLeast(Lifecycle.State.STARTED)
    }
    
    @JvmStatic
    fun getAppLifecycleState(): String {
        val state = ProcessLifecycleOwner.get().lifecycle.currentState
        return when (state) {
            Lifecycle.State.DESTROYED -> "مدمر"
            Lifecycle.State.INITIALIZED -> "مهيأ"
            Lifecycle.State.CREATED -> "منشأ"
            Lifecycle.State.STARTED -> "مبدوء"
            Lifecycle.State.RESUMED -> "مستأنف"
        }
    }
    
    @JvmStatic
    fun getAppLifecycleStateEnglish(): String {
        return ProcessLifecycleOwner.get().lifecycle.currentState.name
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // LifecycleOwner Observer
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun observeLifecycle(
        owner: LifecycleOwner,
        observerId: String,
        onStart: (() -> Unit)? = null,
        onStop: (() -> Unit)? = null,
        onResume: (() -> Unit)? = null,
        onPause: (() -> Unit)? = null,
        onCreate: (() -> Unit)? = null,
        onDestroy: (() -> Unit)? = null
    ) {
        val observer = object : DefaultLifecycleObserver {
            override fun onCreate(owner: LifecycleOwner) {
                onCreate?.invoke()
            }
            
            override fun onStart(owner: LifecycleOwner) {
                onStart?.invoke()
            }
            
            override fun onResume(owner: LifecycleOwner) {
                onResume?.invoke()
            }
            
            override fun onPause(owner: LifecycleOwner) {
                onPause?.invoke()
            }
            
            override fun onStop(owner: LifecycleOwner) {
                onStop?.invoke()
            }
            
            override fun onDestroy(owner: LifecycleOwner) {
                onDestroy?.invoke()
                lifecycleObservers.remove(observerId)
            }
        }
        
        lifecycleObservers[observerId] = observer
        owner.lifecycle.addObserver(observer)
    }
    
    @JvmStatic
    fun removeObserver(owner: LifecycleOwner, observerId: String) {
        lifecycleObservers[observerId]?.let { observer ->
            owner.lifecycle.removeObserver(observer)
            lifecycleObservers.remove(observerId)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Lifecycle State Queries
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getState(owner: LifecycleOwner): String {
        val state = owner.lifecycle.currentState
        return when (state) {
            Lifecycle.State.DESTROYED -> "مدمر"
            Lifecycle.State.INITIALIZED -> "مهيأ"
            Lifecycle.State.CREATED -> "منشأ"
            Lifecycle.State.STARTED -> "مبدوء"
            Lifecycle.State.RESUMED -> "مستأنف"
        }
    }
    
    @JvmStatic
    fun isAtLeast(owner: LifecycleOwner, state: String): Boolean {
        val lifecycleState = when (state.lowercase()) {
            "destroyed", "مدمر" -> Lifecycle.State.DESTROYED
            "initialized", "مهيأ" -> Lifecycle.State.INITIALIZED
            "created", "منشأ" -> Lifecycle.State.CREATED
            "started", "مبدوء" -> Lifecycle.State.STARTED
            "resumed", "مستأنف" -> Lifecycle.State.RESUMED
            else -> return false
        }
        return owner.lifecycle.currentState.isAtLeast(lifecycleState)
    }
    
    @JvmStatic
    fun isCreated(owner: LifecycleOwner): Boolean {
        return owner.lifecycle.currentState.isAtLeast(Lifecycle.State.CREATED)
    }
    
    @JvmStatic
    fun isStarted(owner: LifecycleOwner): Boolean {
        return owner.lifecycle.currentState.isAtLeast(Lifecycle.State.STARTED)
    }
    
    @JvmStatic
    fun isResumed(owner: LifecycleOwner): Boolean {
        return owner.lifecycle.currentState.isAtLeast(Lifecycle.State.RESUMED)
    }
    
    @JvmStatic
    fun isDestroyed(owner: LifecycleOwner): Boolean {
        return owner.lifecycle.currentState == Lifecycle.State.DESTROYED
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Lifecycle-aware Coroutines Support
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun runWhenStarted(owner: LifecycleOwner, action: () -> Unit) {
        if (owner.lifecycle.currentState.isAtLeast(Lifecycle.State.STARTED)) {
            action()
        } else {
            owner.lifecycle.addObserver(object : DefaultLifecycleObserver {
                override fun onStart(owner: LifecycleOwner) {
                    action()
                    owner.lifecycle.removeObserver(this)
                }
            })
        }
    }
    
    @JvmStatic
    fun runWhenResumed(owner: LifecycleOwner, action: () -> Unit) {
        if (owner.lifecycle.currentState.isAtLeast(Lifecycle.State.RESUMED)) {
            action()
        } else {
            owner.lifecycle.addObserver(object : DefaultLifecycleObserver {
                override fun onResume(owner: LifecycleOwner) {
                    action()
                    owner.lifecycle.removeObserver(this)
                }
            })
        }
    }
    
    @JvmStatic
    fun runWhenCreated(owner: LifecycleOwner, action: () -> Unit) {
        if (owner.lifecycle.currentState.isAtLeast(Lifecycle.State.CREATED)) {
            action()
        } else {
            owner.lifecycle.addObserver(object : DefaultLifecycleObserver {
                override fun onCreate(owner: LifecycleOwner) {
                    action()
                    owner.lifecycle.removeObserver(this)
                }
            })
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // LifecycleScope Alternative
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun doOnStart(owner: LifecycleOwner, repeatOnLifecycle: Boolean = false, action: () -> Unit) {
        owner.lifecycle.addObserver(object : DefaultLifecycleObserver {
            override fun onStart(owner: LifecycleOwner) {
                action()
                if (!repeatOnLifecycle) {
                    owner.lifecycle.removeObserver(this)
                }
            }
        })
    }
    
    @JvmStatic
    fun doOnStop(owner: LifecycleOwner, repeatOnLifecycle: Boolean = false, action: () -> Unit) {
        owner.lifecycle.addObserver(object : DefaultLifecycleObserver {
            override fun onStop(owner: LifecycleOwner) {
                action()
                if (!repeatOnLifecycle) {
                    owner.lifecycle.removeObserver(this)
                }
            }
        })
    }
    
    @JvmStatic
    fun doOnResume(owner: LifecycleOwner, repeatOnLifecycle: Boolean = false, action: () -> Unit) {
        owner.lifecycle.addObserver(object : DefaultLifecycleObserver {
            override fun onResume(owner: LifecycleOwner) {
                action()
                if (!repeatOnLifecycle) {
                    owner.lifecycle.removeObserver(this)
                }
            }
        })
    }
    
    @JvmStatic
    fun doOnPause(owner: LifecycleOwner, repeatOnLifecycle: Boolean = false, action: () -> Unit) {
        owner.lifecycle.addObserver(object : DefaultLifecycleObserver {
            override fun onPause(owner: LifecycleOwner) {
                action()
                if (!repeatOnLifecycle) {
                    owner.lifecycle.removeObserver(this)
                }
            }
        })
    }
    
    @JvmStatic
    fun doOnDestroy(owner: LifecycleOwner, action: () -> Unit) {
        owner.lifecycle.addObserver(object : DefaultLifecycleObserver {
            override fun onDestroy(owner: LifecycleOwner) {
                action()
            }
        })
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Cleanup
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun clearAllObservers() {
        lifecycleObservers.clear()
    }
    
    @JvmStatic
    fun removeAllActivityCallbacks() {
        onActivityCreatedCallback = null
        onActivityStartedCallback = null
        onActivityResumedCallback = null
        onActivityPausedCallback = null
        onActivityStoppedCallback = null
        onActivityDestroyedCallback = null
    }
    
    @JvmStatic
    fun cleanup() {
        application?.unregisterActivityLifecycleCallbacks(activityLifecycleCallbacks)
        ProcessLifecycleOwner.get().lifecycle.removeObserver(processLifecycleObserver)
        clearAllObservers()
        removeAllActivityCallbacks()
        onAppForegroundCallback = null
        onAppBackgroundCallback = null
    }
}
