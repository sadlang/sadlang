// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadFirestoreBridge.kt
// الوصف: جسر Kotlin لقاعدة بيانات Firestore
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.content.Context
import com.google.firebase.firestore.*
import com.google.firebase.firestore.ktx.firestore
import com.google.firebase.ktx.Firebase

/**
 * جسر Firestore — SadFirestoreBridge
 */
object SadFirestoreBridge {
    
    private var context: Context? = null
    private val db: FirebaseFirestore by lazy { Firebase.firestore }
    private val listeners = mutableMapOf<String, ListenerRegistration>()
    
    @JvmStatic
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Document Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun addDocument(
        collection: String,
        data: Map<String, Any>,
        onSuccess: (String) -> Unit,
        onError: (String) -> Unit
    ) {
        db.collection(collection)
            .add(data)
            .addOnSuccessListener { docRef ->
                onSuccess(docRef.id)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في إضافة المستند")
            }
    }
    
    @JvmStatic
    fun setDocument(
        collection: String,
        documentId: String,
        data: Map<String, Any>,
        merge: Boolean = false,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val docRef = db.collection(collection).document(documentId)
        val task = if (merge) {
            docRef.set(data, SetOptions.merge())
        } else {
            docRef.set(data)
        }
        
        task.addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في حفظ المستند")
            }
    }
    
    @JvmStatic
    fun getDocument(
        collection: String,
        documentId: String,
        onSuccess: (Map<String, Any>?) -> Unit,
        onError: (String) -> Unit
    ) {
        db.collection(collection).document(documentId)
            .get()
            .addOnSuccessListener { document ->
                if (document.exists()) {
                    val data = document.data?.toMutableMap() ?: mutableMapOf()
                    data["_id"] = document.id
                    onSuccess(data)
                } else {
                    onSuccess(null)
                }
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في جلب المستند")
            }
    }
    
    @JvmStatic
    fun updateDocument(
        collection: String,
        documentId: String,
        data: Map<String, Any>,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        db.collection(collection).document(documentId)
            .update(data)
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تحديث المستند")
            }
    }
    
    @JvmStatic
    fun deleteDocument(
        collection: String,
        documentId: String,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        db.collection(collection).document(documentId)
            .delete()
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في حذف المستند")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Collection Queries
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getCollection(
        collection: String,
        onSuccess: (List<Map<String, Any>>) -> Unit,
        onError: (String) -> Unit
    ) {
        db.collection(collection)
            .get()
            .addOnSuccessListener { result ->
                val documents = result.documents.mapNotNull { doc ->
                    val data = doc.data?.toMutableMap() ?: return@mapNotNull null
                    data["_id"] = doc.id
                    data
                }
                onSuccess(documents)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في جلب المجموعة")
            }
    }
    
    @JvmStatic
    fun queryWhere(
        collection: String,
        field: String,
        operator: String,
        value: Any,
        onSuccess: (List<Map<String, Any>>) -> Unit,
        onError: (String) -> Unit
    ) {
        var query = db.collection(collection) as Query
        
        query = when (operator.lowercase()) {
            "==" -> query.whereEqualTo(field, value)
            "!=" -> query.whereNotEqualTo(field, value)
            "<" -> query.whereLessThan(field, value)
            "<=" -> query.whereLessThanOrEqualTo(field, value)
            ">" -> query.whereGreaterThan(field, value)
            ">=" -> query.whereGreaterThanOrEqualTo(field, value)
            "in" -> query.whereIn(field, value as List<*>)
            "not-in" -> query.whereNotIn(field, value as List<*>)
            "array-contains" -> query.whereArrayContains(field, value)
            "array-contains-any" -> query.whereArrayContainsAny(field, value as List<*>)
            else -> {
                onError("عامل غير مدعوم: $operator")
                return
            }
        }
        
        query.get()
            .addOnSuccessListener { result ->
                val documents = result.documents.mapNotNull { doc ->
                    val data = doc.data?.toMutableMap() ?: return@mapNotNull null
                    data["_id"] = doc.id
                    data
                }
                onSuccess(documents)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في الاستعلام")
            }
    }
    
    @JvmStatic
    fun queryOrdered(
        collection: String,
        orderByField: String,
        descending: Boolean = false,
        limit: Long = 0,
        onSuccess: (List<Map<String, Any>>) -> Unit,
        onError: (String) -> Unit
    ) {
        var query = db.collection(collection) as Query
        
        query = if (descending) {
            query.orderBy(orderByField, Query.Direction.DESCENDING)
        } else {
            query.orderBy(orderByField, Query.Direction.ASCENDING)
        }
        
        if (limit > 0) {
            query = query.limit(limit)
        }
        
        query.get()
            .addOnSuccessListener { result ->
                val documents = result.documents.mapNotNull { doc ->
                    val data = doc.data?.toMutableMap() ?: return@mapNotNull null
                    data["_id"] = doc.id
                    data
                }
                onSuccess(documents)
            }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في الاستعلام")
            }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Real-time Listeners
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun listenToDocument(
        listenerId: String,
        collection: String,
        documentId: String,
        onUpdate: (Map<String, Any>?) -> Unit,
        onError: (String) -> Unit
    ) {
        removeListener(listenerId)
        
        val registration = db.collection(collection).document(documentId)
            .addSnapshotListener { snapshot, error ->
                if (error != null) {
                    onError(error.localizedMessage ?: "خطأ في المراقبة")
                    return@addSnapshotListener
                }
                
                if (snapshot != null && snapshot.exists()) {
                    val data = snapshot.data?.toMutableMap() ?: mutableMapOf()
                    data["_id"] = snapshot.id
                    onUpdate(data)
                } else {
                    onUpdate(null)
                }
            }
        
        listeners[listenerId] = registration
    }
    
    @JvmStatic
    fun listenToCollection(
        listenerId: String,
        collection: String,
        onUpdate: (List<Map<String, Any>>) -> Unit,
        onError: (String) -> Unit
    ) {
        removeListener(listenerId)
        
        val registration = db.collection(collection)
            .addSnapshotListener { snapshot, error ->
                if (error != null) {
                    onError(error.localizedMessage ?: "خطأ في المراقبة")
                    return@addSnapshotListener
                }
                
                val documents = snapshot?.documents?.mapNotNull { doc ->
                    val data = doc.data?.toMutableMap() ?: return@mapNotNull null
                    data["_id"] = doc.id
                    data
                } ?: emptyList()
                
                onUpdate(documents)
            }
        
        listeners[listenerId] = registration
    }
    
    @JvmStatic
    fun removeListener(listenerId: String): Boolean {
        val registration = listeners.remove(listenerId)
        registration?.remove()
        return registration != null
    }
    
    @JvmStatic
    fun removeAllListeners() {
        listeners.values.forEach { it.remove() }
        listeners.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Batch Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun batchWrite(
        operations: List<Map<String, Any>>,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val batch = db.batch()
        
        try {
            for (op in operations) {
                val type = op["type"] as? String ?: continue
                val collection = op["collection"] as? String ?: continue
                val documentId = op["documentId"] as? String
                val data = op["data"] as? Map<String, Any>
                
                when (type) {
                    "set" -> {
                        val docRef = if (documentId != null) {
                            db.collection(collection).document(documentId)
                        } else {
                            db.collection(collection).document()
                        }
                        data?.let { batch.set(docRef, it) }
                    }
                    "update" -> {
                        if (documentId != null && data != null) {
                            val docRef = db.collection(collection).document(documentId)
                            batch.update(docRef, data)
                        }
                    }
                    "delete" -> {
                        if (documentId != null) {
                            val docRef = db.collection(collection).document(documentId)
                            batch.delete(docRef)
                        }
                    }
                }
            }
            
            batch.commit()
                .addOnSuccessListener { onSuccess() }
                .addOnFailureListener { e ->
                    onError(e.localizedMessage ?: "فشل في تنفيذ الدفعة")
                }
        } catch (e: Exception) {
            onError(e.localizedMessage ?: "خطأ في إعداد الدفعة")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Transactions
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun runTransaction(
        collection: String,
        documentId: String,
        updateFunction: (Map<String, Any>?) -> Map<String, Any>?,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val docRef = db.collection(collection).document(documentId)
        
        db.runTransaction { transaction ->
            val snapshot = transaction.get(docRef)
            val currentData = snapshot.data
            val newData = updateFunction(currentData)
            
            if (newData != null) {
                transaction.set(docRef, newData)
            }
            
            null
        }
        .addOnSuccessListener { onSuccess() }
        .addOnFailureListener { e ->
            onError(e.localizedMessage ?: "فشل في تنفيذ المعاملة")
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Field Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun incrementField(
        collection: String,
        documentId: String,
        field: String,
        incrementBy: Long = 1,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        db.collection(collection).document(documentId)
            .update(field, FieldValue.increment(incrementBy))
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في زيادة القيمة")
            }
    }
    
    @JvmStatic
    fun arrayUnion(
        collection: String,
        documentId: String,
        field: String,
        elements: List<Any>,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        db.collection(collection).document(documentId)
            .update(field, FieldValue.arrayUnion(*elements.toTypedArray()))
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في إضافة العناصر")
            }
    }
    
    @JvmStatic
    fun arrayRemove(
        collection: String,
        documentId: String,
        field: String,
        elements: List<Any>,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        db.collection(collection).document(documentId)
            .update(field, FieldValue.arrayRemove(*elements.toTypedArray()))
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في حذف العناصر")
            }
    }
    
    @JvmStatic
    fun serverTimestamp(): FieldValue = FieldValue.serverTimestamp()
    
    @JvmStatic
    fun deleteField(): FieldValue = FieldValue.delete()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Utilities
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun generateDocumentId(collection: String): String {
        return db.collection(collection).document().id
    }
    
    @JvmStatic
    fun enableOfflinePersistence() {
        val settings = FirebaseFirestoreSettings.Builder()
            .setPersistenceEnabled(true)
            .build()
        db.firestoreSettings = settings
    }
    
    @JvmStatic
    fun disableNetwork(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        db.disableNetwork()
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تعطيل الشبكة")
            }
    }
    
    @JvmStatic
    fun enableNetwork(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        db.enableNetwork()
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في تفعيل الشبكة")
            }
    }
    
    @JvmStatic
    fun clearPersistence(
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        db.clearPersistence()
            .addOnSuccessListener { onSuccess() }
            .addOnFailureListener { e ->
                onError(e.localizedMessage ?: "فشل في مسح البيانات المحلية")
            }
    }
}
