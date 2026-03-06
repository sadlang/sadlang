// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadFirebaseAuthBridge.kt
// الوصف: جسر Kotlin لمصادقة Firebase
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import com.google.firebase.auth.*
import com.google.firebase.auth.ktx.auth
import com.google.firebase.ktx.Firebase

/**
 * جسر Firebase Auth — SadFirebaseAuthBridge
 */
object SadFirebaseAuthBridge {
    
    private var context: Context? = null
    private val auth: FirebaseAuth by lazy { Firebase.auth }
    
    private var onAuthStateChangedCallback: ((Boolean, Map<String, Any?>?) -> Unit)? = null
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
        
        auth.addAuthStateListener { firebaseAuth ->
            val user = firebaseAuth.currentUser
            onAuthStateChangedCallback?.invoke(user != null, user?.toMap())
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Email/Password Authentication
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun signUpWithEmail(
        email: String,
        password: String,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        auth.createUserWithEmailAndPassword(email, password)
            .addOnSuccessListener { result ->
                onSuccess(result.user?.toMap() ?: emptyMap())
            }
            .addOnFailureListener { e ->
                onError(getArabicErrorMessage(e))
            }
    }
    
    @JvmStatic
    fun signInWithEmail(
        email: String,
        password: String,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        auth.signInWithEmailAndPassword(email, password)
            .addOnSuccessListener { result ->
                onSuccess(result.user?.toMap() ?: emptyMap())
            }
            .addOnFailureListener { e ->
                onError(getArabicErrorMessage(e))
            }
    }
    
    @JvmStatic
    fun sendPasswordResetEmail(
        email: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        auth.sendPasswordResetEmail(email)
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e -> onError(getArabicErrorMessage(e)) }
    }
    
    @JvmStatic
    fun sendEmailVerification(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        auth.currentUser?.sendEmailVerification()
            ?.addOnSuccessListener { onSuccess() }
            ?.addOnFailureListener { e -> onError(getArabicErrorMessage(e)) }
            ?: onError("لا يوجد مستخدم مسجل")
    }
    
    @JvmStatic
    fun updatePassword(
        newPassword: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        auth.currentUser?.updatePassword(newPassword)
            ?.addOnSuccessListener { onSuccess() }
            ?.addOnFailureListener { e -> onError(getArabicErrorMessage(e)) }
            ?: onError("لا يوجد مستخدم مسجل")
    }
    
    @JvmStatic
    fun updateEmail(
        newEmail: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        auth.currentUser?.updateEmail(newEmail)
            ?.addOnSuccessListener { onSuccess() }
            ?.addOnFailureListener { e -> onError(getArabicErrorMessage(e)) }
            ?: onError("لا يوجد مستخدم مسجل")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Anonymous Authentication
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun signInAnonymously(
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        auth.signInAnonymously()
            .addOnSuccessListener { result ->
                onSuccess(result.user?.toMap() ?: emptyMap())
            }
            .addOnFailureListener { e ->
                onError(getArabicErrorMessage(e))
            }
    }
    
    @JvmStatic
    fun linkWithEmail(
        email: String,
        password: String,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        val credential = EmailAuthProvider.getCredential(email, password)
        auth.currentUser?.linkWithCredential(credential)
            ?.addOnSuccessListener { result ->
                onSuccess(result.user?.toMap() ?: emptyMap())
            }
            ?.addOnFailureListener { e ->
                onError(getArabicErrorMessage(e))
            }
            ?: onError("لا يوجد مستخدم مسجل")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Phone Authentication
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun sendPhoneVerificationCode(
        phoneNumber: String,
        activity: android.app.Activity,
        onCodeSent: (String) -> Unit,
        onVerificationCompleted: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        val callbacks = object : PhoneAuthProvider.OnVerificationStateChangedCallbacks() {
            override fun onVerificationCompleted(credential: PhoneAuthCredential) {
                signInWithPhoneCredential(credential, onVerificationCompleted, onError)
            }
            
            override fun onVerificationFailed(e: FirebaseException) {
                onError(getArabicErrorMessage(e))
            }
            
            override fun onCodeSent(verificationId: String, token: PhoneAuthProvider.ForceResendingToken) {
                onCodeSent(verificationId)
            }
        }
        
        val options = PhoneAuthOptions.newBuilder(auth)
            .setPhoneNumber(phoneNumber)
            .setTimeout(60L, java.util.concurrent.TimeUnit.SECONDS)
            .setActivity(activity)
            .setCallbacks(callbacks)
            .build()
        
        PhoneAuthProvider.verifyPhoneNumber(options)
    }
    
    @JvmStatic
    fun verifyPhoneCode(
        verificationId: String,
        code: String,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        val credential = PhoneAuthProvider.getCredential(verificationId, code)
        signInWithPhoneCredential(credential, onSuccess, onError)
    }
    
    private fun signInWithPhoneCredential(
        credential: PhoneAuthCredential,
        onSuccess: (Map<String, Any?>) -> Unit,
        onError: (String) -> Unit
    ) {
        auth.signInWithCredential(credential)
            .addOnSuccessListener { result ->
                onSuccess(result.user?.toMap() ?: emptyMap())
            }
            .addOnFailureListener { e ->
                onError(getArabicErrorMessage(e))
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // User State
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun isSignedIn(): Boolean = auth.currentUser != null
    
    @JvmStatic
    fun getCurrentUser(): Map<String, Any?>? = auth.currentUser?.toMap()
    
    @JvmStatic
    fun getUserId(): String? = auth.currentUser?.uid
    
    @JvmStatic
    fun getUserEmail(): String? = auth.currentUser?.email
    
    @JvmStatic
    fun getUserPhone(): String? = auth.currentUser?.phoneNumber
    
    @JvmStatic
    fun getUserDisplayName(): String? = auth.currentUser?.displayName
    
    @JvmStatic
    fun getUserPhotoUrl(): String? = auth.currentUser?.photoUrl?.toString()
    
    @JvmStatic
    fun isEmailVerified(): Boolean = auth.currentUser?.isEmailVerified == true
    
    @JvmStatic
    fun isAnonymous(): Boolean = auth.currentUser?.isAnonymous == true
    
    @JvmStatic
    fun getProviders(): List<String> {
        return auth.currentUser?.providerData?.map { it.providerId } ?: emptyList()
    }
    
    @JvmStatic
    fun updateProfile(
        displayName: String? = null,
        photoUri: String? = null,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val profileUpdates = UserProfileChangeRequest.Builder().apply {
            displayName?.let { setDisplayName(it) }
            photoUri?.let { setPhotoUri(android.net.Uri.parse(it)) }
        }.build()
        
        auth.currentUser?.updateProfile(profileUpdates)
            ?.addOnSuccessListener { onSuccess() }
            ?.addOnFailureListener { e -> onError(getArabicErrorMessage(e)) }
            ?: onError("لا يوجد مستخدم مسجل")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Sign Out & Delete
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun signOut() {
        auth.signOut()
    }
    
    @JvmStatic
    fun deleteAccount(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        auth.currentUser?.delete()
            ?.addOnSuccessListener { onSuccess() }
            ?.addOnFailureListener { e -> onError(getArabicErrorMessage(e)) }
            ?: onError("لا يوجد مستخدم مسجل")
    }
    
    @JvmStatic
    fun reauthenticate(
        email: String,
        password: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val credential = EmailAuthProvider.getCredential(email, password)
        auth.currentUser?.reauthenticate(credential)
            ?.addOnSuccessListener { onSuccess() }
            ?.addOnFailureListener { e -> onError(getArabicErrorMessage(e)) }
            ?: onError("لا يوجد مستخدم مسجل")
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Listeners
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setOnAuthStateChangedListener(callback: (Boolean, Map<String, Any?>?) -> Unit) {
        onAuthStateChangedCallback = callback
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    private fun FirebaseUser.toMap(): Map<String, Any?> {
        return mapOf(
            "uid" to uid,
            "email" to email,
            "phone" to phoneNumber,
            "displayName" to displayName,
            "photoUrl" to photoUrl?.toString(),
            "isEmailVerified" to isEmailVerified,
            "isAnonymous" to isAnonymous,
            "creationTime" to metadata?.creationTimestamp,
            "lastSignInTime" to metadata?.lastSignInTimestamp,
            "providers" to providerData.map { it.providerId }
        )
    }
    
    private fun getArabicErrorMessage(e: Exception): String {
        return when (e) {
            is FirebaseAuthInvalidCredentialsException -> "بيانات الاعتماد غير صالحة"
            is FirebaseAuthInvalidUserException -> "المستخدم غير موجود أو معطل"
            is FirebaseAuthUserCollisionException -> "هذا البريد مستخدم بالفعل"
            is FirebaseAuthWeakPasswordException -> "كلمة المرور ضعيفة جداً"
            is FirebaseAuthEmailException -> "خطأ في البريد الإلكتروني"
            is FirebaseAuthRecentLoginRequiredException -> "يرجى تسجيل الدخول مجدداً"
            is FirebaseAuthMultiFactorException -> "مطلوب التحقق متعدد العوامل"
            else -> e.localizedMessage ?: "حدث خطأ غير متوقع"
        }
    }
}
