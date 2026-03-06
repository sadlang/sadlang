// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadGoogleSignInBridge.kt
// الوصف: جسر Kotlin لتسجيل الدخول بحساب Google
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.content.Context
import android.content.Intent
import com.google.android.gms.auth.api.signin.GoogleSignIn
import com.google.android.gms.auth.api.signin.GoogleSignInAccount
import com.google.android.gms.auth.api.signin.GoogleSignInClient
import com.google.android.gms.auth.api.signin.GoogleSignInOptions
import com.google.android.gms.common.api.ApiException
import com.google.firebase.auth.FirebaseAuth
import com.google.firebase.auth.GoogleAuthProvider

/**
 * جسر Google Sign-In — SadGoogleSignInBridge
 */
object SadGoogleSignInBridge {
    
    private var context: Context? = null
    private var googleSignInClient: GoogleSignInClient? = null
    private var webClientId: String? = null
    
    const val RC_SIGN_IN = 9001
    
    private var onSignInSuccessCallback: ((Map<String, Any?>) -> Unit)? = null
    private var onSignInErrorCallback: ((String) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context, webClientId: String) {
        context = ctx.applicationContext
        this.webClientId = webClientId
        
        val gso = GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
            .requestIdToken(webClientId)
            .requestEmail()
            .requestProfile()
            .build()
        
        googleSignInClient = GoogleSignIn.getClient(ctx, gso)
    }
    
    @JvmStatic
    fun initializeBasic(ctx: Context) {
        context = ctx.applicationContext
        
        val gso = GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
            .requestEmail()
            .requestProfile()
            .build()
        
        googleSignInClient = GoogleSignIn.getClient(ctx, gso)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Sign In
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getSignInIntent(): Intent? {
        return googleSignInClient?.signInIntent
    }
    
    @JvmStatic
    fun signIn(
        activity: Activity,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        onSignInSuccessCallback = onSuccess
        onSignInErrorCallback = onError
        
        val signInIntent = googleSignInClient?.signInIntent
        if (signInIntent != null) {
            activity.startActivityForResult(signInIntent, RC_SIGN_IN)
        } else {
            onError("لم يتم تهيئة تسجيل الدخول بـ Google")
        }
    }
    
    @JvmStatic
    fun handleSignInResult(data: Intent?) {
        try {
            val task = GoogleSignIn.getSignedInAccountFromIntent(data)
            val account = task.getResult(ApiException::class.java)
            
            if (account != null) {
                onSignInSuccessCallback?.invoke(account.toMap())
            } else {
                onSignInErrorCallback?.invoke("فشل في الحصول على بيانات الحساب")
            }
        } catch (e: ApiException) {
            val errorMessage = when (e.statusCode) {
                12501 -> "تم إلغاء تسجيل الدخول"
                12502 -> "تسجيل الدخول قيد التنفيذ"
                7 -> "خطأ في الشبكة"
                8 -> "خطأ داخلي"
                10 -> "خطأ في المطور - تحقق من SHA-1"
                else -> "خطأ في تسجيل الدخول: ${e.statusCode}"
            }
            onSignInErrorCallback?.invoke(errorMessage)
        }
    }
    
    @JvmStatic
    fun signInWithFirebase(
        activity: Activity,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        signIn(activity, { accountData ->
            val idToken = accountData["idToken"] as? String
            if (idToken != null) {
                firebaseAuthWithGoogle(idToken, onSuccess, onError)
            } else {
                onError("لم يتم الحصول على رمز المصادقة")
            }
        }, onError)
    }
    
    private fun firebaseAuthWithGoogle(
        idToken: String,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        val credential = GoogleAuthProvider.getCredential(idToken, null)
        FirebaseAuth.getInstance().signInWithCredential(credential)
            .addOnSuccessListener { result ->
                val user = result.user
                if (user != null) {
                    onSuccess(mapOf(
                        "uid" to user.uid,
                        "email" to user.email,
                        "displayName" to user.displayName,
                        "photoUrl" to user.photoUrl?.toString(),
                        "isNewUser" to result.additionalUserInfo?.isNewUser,
                        "providerId" to result.additionalUserInfo?.providerId
                    ))
                } else {
                    onError("فشل في الحصول على بيانات المستخدم")
                }
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في المصادقة مع Firebase")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Silent Sign In
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun silentSignIn(
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        googleSignInClient?.silentSignIn()
            ?.addOnSuccessListener { account ->
                onSuccess(account.toMap())
            }
            ?.addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تسجيل الدخول التلقائي")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Current User
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isSignedIn(): Boolean {
        val ctx = context ?: return false
        return GoogleSignIn.getLastSignedInAccount(ctx) != null
    }
    
    @JvmStatic
    fun getCurrentAccount(): Map<String, Any?>? {
        val ctx = context ?: return null
        return GoogleSignIn.getLastSignedInAccount(ctx)?.toMap()
    }
    
    @JvmStatic
    fun getEmail(): String? {
        val ctx = context ?: return null
        return GoogleSignIn.getLastSignedInAccount(ctx)?.email
    }
    
    @JvmStatic
    fun getDisplayName(): String? {
        val ctx = context ?: return null
        return GoogleSignIn.getLastSignedInAccount(ctx)?.displayName
    }
    
    @JvmStatic
    fun getPhotoUrl(): String? {
        val ctx = context ?: return null
        return GoogleSignIn.getLastSignedInAccount(ctx)?.photoUrl?.toString()
    }
    
    @JvmStatic
    fun getId(): String? {
        val ctx = context ?: return null
        return GoogleSignIn.getLastSignedInAccount(ctx)?.id
    }
    
    @JvmStatic
    fun getIdToken(): String? {
        val ctx = context ?: return null
        return GoogleSignIn.getLastSignedInAccount(ctx)?.idToken
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Sign Out
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun signOut(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        googleSignInClient?.signOut()
            ?.addOnSuccessListener { onSuccess() }
            ?.addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تسجيل الخروج")
            }
    }
    
    @JvmStatic
    fun revokeAccess(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        googleSignInClient?.revokeAccess()
            ?.addOnSuccessListener { onSuccess() }
            ?.addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في إلغاء الوصول")
            }
    }
    
    @JvmStatic
    fun signOutFromFirebase() {
        FirebaseAuth.getInstance().signOut()
    }
    
    @JvmStatic
    fun fullSignOut(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        signOutFromFirebase()
        signOut(onSuccess, onError)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun GoogleSignInAccount.toMap(): Map<String, Any?> {
        return mapOf(
            "id" to id,
            "email" to email,
            "displayName" to displayName,
            "givenName" to givenName,
            "familyName" to familyName,
            "photoUrl" to photoUrl?.toString(),
            "idToken" to idToken,
            "serverAuthCode" to serverAuthCode,
            "isExpired" to isExpired
        )
    }
    
    @JvmStatic
    fun hasPermission(scope: String): Boolean {
        val ctx = context ?: return false
        val account = GoogleSignIn.getLastSignedInAccount(ctx) ?: return false
        return GoogleSignIn.hasPermissions(account, com.google.android.gms.common.api.Scope(scope))
    }
    
    @JvmStatic
    fun requestPermissions(activity: Activity, scopes: List<String>) {
        val ctx = context ?: return
        val account = GoogleSignIn.getLastSignedInAccount(ctx) ?: return
        
        val scopeObjects = scopes.map { com.google.android.gms.common.api.Scope(it) }.toTypedArray()
        GoogleSignIn.requestPermissions(activity, RC_SIGN_IN + 1, account, *scopeObjects)
    }
}
