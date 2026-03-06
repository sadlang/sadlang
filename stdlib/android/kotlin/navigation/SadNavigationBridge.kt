// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadNavigationBridge.kt
// الوصف: جسر Kotlin للتنقل بين الشاشات Navigation Component
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import android.os.Bundle
import androidx.navigation.NavController
import androidx.navigation.NavGraph
import androidx.navigation.NavOptions
import androidx.navigation.Navigator

/**
 * جسر التنقل — SadNavigationBridge
 */
object SadNavigationBridge {
    
    private var context: Context? = null
    private var navController: NavController? = null
    private var onDestinationChangedCallback: ((Int, String?, Bundle?) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    @JvmStatic
    fun setNavController(controller: NavController) {
        navController = controller
        setupDestinationListener()
    }
    
    private fun setupDestinationListener() {
        navController?.addOnDestinationChangedListener { _, destination, args ->
            onDestinationChangedCallback?.invoke(
                destination.id,
                destination.label?.toString(),
                args
            )
        }
    }
    
    @JvmStatic
    fun getNavController(): NavController? = navController
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Navigation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun navigate(destinationId: Int, args: Bundle? = null): Boolean {
        return try {
            navController?.navigate(destinationId, args)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun navigateWithOptions(
        destinationId: Int,
        args: Bundle? = null,
        popUpTo: Int? = null,
        popUpToInclusive: Boolean = false,
        launchSingleTop: Boolean = false,
        restoreState: Boolean = false
    ): Boolean {
        return try {
            val navOptions = NavOptions.Builder().apply {
                setLaunchSingleTop(launchSingleTop)
                setRestoreState(restoreState)
                popUpTo?.let { setPopUpTo(it, popUpToInclusive) }
            }.build()
            
            navController?.navigate(destinationId, args, navOptions)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun navigateWithAnimation(
        destinationId: Int,
        args: Bundle? = null,
        enterAnim: Int,
        exitAnim: Int,
        popEnterAnim: Int,
        popExitAnim: Int
    ): Boolean {
        return try {
            val navOptions = NavOptions.Builder().apply {
                setEnterAnim(enterAnim)
                setExitAnim(exitAnim)
                setPopEnterAnim(popEnterAnim)
                setPopExitAnim(popExitAnim)
            }.build()
            
            navController?.navigate(destinationId, args, navOptions)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Deep Link
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun navigateDeepLink(deepLink: String): Boolean {
        return try {
            val uri = android.net.Uri.parse(deepLink)
            navController?.navigate(uri)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun handleDeepLink(deepLink: String): Boolean {
        return try {
            val uri = android.net.Uri.parse(deepLink)
            navController?.handleDeepLink(
                android.content.Intent().apply { data = uri }
            ) ?: false
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Back Navigation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun navigateUp(): Boolean {
        return try {
            navController?.navigateUp() ?: false
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun popBackStack(): Boolean {
        return try {
            navController?.popBackStack() ?: false
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun popBackStackTo(destinationId: Int, inclusive: Boolean = false): Boolean {
        return try {
            navController?.popBackStack(destinationId, inclusive) ?: false
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun clearBackStack(destinationId: Int): Boolean {
        return try {
            navController?.clearBackStack(destinationId) ?: false
        } catch (e: Exception) {
            false
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Current Destination
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getCurrentDestination(): Map<String, Any?>? {
        val destination = navController?.currentDestination ?: return null
        
        return mapOf(
            "id" to destination.id,
            "label" to destination.label?.toString(),
            "navigatorName" to destination.navigatorName,
            "route" to destination.route
        )
    }
    
    @JvmStatic
    fun getCurrentDestinationId(): Int? {
        return navController?.currentDestination?.id
    }
    
    @JvmStatic
    fun getCurrentDestinationLabel(): String? {
        return navController?.currentDestination?.label?.toString()
    }
    
    @JvmStatic
    fun getCurrentBackStackEntry(): Map<String, Any?>? {
        val entry = navController?.currentBackStackEntry ?: return null
        
        return mapOf(
            "id" to entry.destination.id,
            "label" to entry.destination.label?.toString(),
            "arguments" to entry.arguments?.keySet()?.associateWith { entry.arguments?.get(it) }
        )
    }
    
    @JvmStatic
    fun getPreviousBackStackEntry(): Map<String, Any?>? {
        val entry = navController?.previousBackStackEntry ?: return null
        
        return mapOf(
            "id" to entry.destination.id,
            "label" to entry.destination.label?.toString()
        )
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Back Stack
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getBackStackDepth(): Int {
        return navController?.backQueue?.size ?: 0
    }
    
    @JvmStatic
    fun hasBackStack(): Boolean {
        return getBackStackDepth() > 1
    }
    
    @JvmStatic
    fun getBackStackDestinations(): List<Map<String, Any?>> {
        return navController?.backQueue?.map { entry ->
            mapOf(
                "id" to entry.destination.id,
                "label" to entry.destination.label?.toString(),
                "route" to entry.destination.route
            )
        } ?: emptyList()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Saved State
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun saveState(): Bundle? {
        return navController?.saveState()
    }
    
    @JvmStatic
    fun restoreState(state: Bundle?) {
        state?.let { navController?.restoreState(it) }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Graph
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setGraph(graphResId: Int, startDestinationArgs: Bundle? = null): Boolean {
        return try {
            navController?.setGraph(graphResId, startDestinationArgs)
            true
        } catch (e: Exception) {
            false
        }
    }
    
    @JvmStatic
    fun getGraphStartDestination(): Int? {
        return navController?.graph?.startDestinationId
    }
    
    @JvmStatic
    fun getGraphDestinations(): List<Map<String, Any?>> {
        val graph = navController?.graph ?: return emptyList()
        
        val destinations = mutableListOf<Map<String, Any?>>()
        for (i in 0 until graph.childCount) {
            val node = graph.getChildAt(i)
            node?.let {
                destinations.add(mapOf(
                    "id" to it.id,
                    "label" to it.label?.toString(),
                    "route" to it.route
                ))
            }
        }
        return destinations
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Listener
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnDestinationChangedListener(callback: (Int, String?, Bundle?) -> Unit) {
        onDestinationChangedCallback = callback
    }
    
    @JvmStatic
    fun removeOnDestinationChangedListener() {
        onDestinationChangedCallback = null
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utility
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun bundleOf(vararg pairs: Pair<String, Any?>): Bundle {
        return Bundle().apply {
            pairs.forEach { (key, value) ->
                when (value) {
                    null -> putString(key, null)
                    is String -> putString(key, value)
                    is Int -> putInt(key, value)
                    is Long -> putLong(key, value)
                    is Float -> putFloat(key, value)
                    is Double -> putDouble(key, value)
                    is Boolean -> putBoolean(key, value)
                    is Char -> putChar(key, value)
                    is CharSequence -> putCharSequence(key, value)
                    is Bundle -> putBundle(key, value)
                    is IntArray -> putIntArray(key, value)
                    is LongArray -> putLongArray(key, value)
                    is FloatArray -> putFloatArray(key, value)
                    is DoubleArray -> putDoubleArray(key, value)
                    is BooleanArray -> putBooleanArray(key, value)
                    is CharArray -> putCharArray(key, value)
                    is java.io.Serializable -> putSerializable(key, value)
                    is android.os.Parcelable -> putParcelable(key, value)
                }
            }
        }
    }
    
    @JvmStatic
    fun getNavigationInfo(): Map<String, Any?> {
        return mapOf(
            "hasController" to (navController != null),
            "currentDestination" to getCurrentDestination(),
            "backStackDepth" to getBackStackDepth(),
            "canNavigateUp" to hasBackStack()
        )
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// Helper Extension for NavGraph
// ═══════════════════════════════════════════════════════════════════════════════

private val NavGraph.childCount: Int
    get() {
        var count = 0
        val iterator = this.iterator()
        while (iterator.hasNext()) {
            iterator.next()
            count++
        }
        return count
    }

private fun NavGraph.getChildAt(index: Int): androidx.navigation.NavDestination? {
    var i = 0
    val iterator = this.iterator()
    while (iterator.hasNext()) {
        val dest = iterator.next()
        if (i == index) return dest
        i++
    }
    return null
}
