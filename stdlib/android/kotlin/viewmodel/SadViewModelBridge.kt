// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadViewModelBridge.kt
// الوصف: جسر Kotlin لـ ViewModel و LiveData
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import androidx.lifecycle.*

/**
 * جسر ViewModel — SadViewModelBridge
 */
object SadViewModelBridge {
    
    // Store for ViewModels created via bridge
    private val viewModels = mutableMapOf<String, SadViewModel>()
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // ViewModel Creation
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun createViewModel(key: String): SadViewModel {
        val viewModel = viewModels.getOrPut(key) { SadViewModel() }
        return viewModel
    }
    
    @JvmStatic
    fun getViewModel(key: String): SadViewModel? {
        return viewModels[key]
    }
    
    @JvmStatic
    fun removeViewModel(key: String) {
        viewModels.remove(key)?.clear()
    }
    
    @JvmStatic
    fun hasViewModel(key: String): Boolean {
        return viewModels.containsKey(key)
    }
    
    @JvmStatic
    fun clearAllViewModels() {
        viewModels.values.forEach { it.clear() }
        viewModels.clear()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Data Access
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setValue(vmKey: String, dataKey: String, value: Any?) {
        viewModels[vmKey]?.setValue(dataKey, value)
    }
    
    @JvmStatic
    fun getValue(vmKey: String, dataKey: String): Any? {
        return viewModels[vmKey]?.getValue(dataKey)
    }
    
    @JvmStatic
    fun postValue(vmKey: String, dataKey: String, value: Any?) {
        viewModels[vmKey]?.postValue(dataKey, value)
    }
    
    @JvmStatic
    fun observe(
        vmKey: String,
        dataKey: String,
        owner: LifecycleOwner,
        callback: (Any?) -> Unit
    ) {
        viewModels[vmKey]?.observe(dataKey, owner, callback)
    }
    
    @JvmStatic
    fun observeForever(vmKey: String, dataKey: String, callback: (Any?) -> Unit): String {
        return viewModels[vmKey]?.observeForever(dataKey, callback) ?: ""
    }
    
    @JvmStatic
    fun removeObserver(vmKey: String, observerId: String) {
        viewModels[vmKey]?.removeObserver(observerId)
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Typed Access
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun setString(vmKey: String, dataKey: String, value: String) {
        setValue(vmKey, dataKey, value)
    }
    
    @JvmStatic
    fun getString(vmKey: String, dataKey: String, default: String = ""): String {
        return getValue(vmKey, dataKey) as? String ?: default
    }
    
    @JvmStatic
    fun setInt(vmKey: String, dataKey: String, value: Int) {
        setValue(vmKey, dataKey, value)
    }
    
    @JvmStatic
    fun getInt(vmKey: String, dataKey: String, default: Int = 0): Int {
        return (getValue(vmKey, dataKey) as? Number)?.toInt() ?: default
    }
    
    @JvmStatic
    fun setDouble(vmKey: String, dataKey: String, value: Double) {
        setValue(vmKey, dataKey, value)
    }
    
    @JvmStatic
    fun getDouble(vmKey: String, dataKey: String, default: Double = 0.0): Double {
        return (getValue(vmKey, dataKey) as? Number)?.toDouble() ?: default
    }
    
    @JvmStatic
    fun setBoolean(vmKey: String, dataKey: String, value: Boolean) {
        setValue(vmKey, dataKey, value)
    }
    
    @JvmStatic
    fun getBoolean(vmKey: String, dataKey: String, default: Boolean = false): Boolean {
        return getValue(vmKey, dataKey) as? Boolean ?: default
    }
    
    @JvmStatic
    fun setList(vmKey: String, dataKey: String, value: List<Any?>) {
        setValue(vmKey, dataKey, value)
    }
    
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun getList(vmKey: String, dataKey: String): List<Any?> {
        return getValue(vmKey, dataKey) as? List<Any?> ?: emptyList()
    }
    
    @JvmStatic
    fun setMap(vmKey: String, dataKey: String, value: Map<String, Any?>) {
        setValue(vmKey, dataKey, value)
    }
    
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun getMap(vmKey: String, dataKey: String): Map<String, Any?> {
        return getValue(vmKey, dataKey) as? Map<String, Any?> ?: emptyMap()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // List Operations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun appendToList(vmKey: String, dataKey: String, item: Any?) {
        val current = getValue(vmKey, dataKey) as? MutableList<Any?>
            ?: mutableListOf()
        current.add(item)
        setValue(vmKey, dataKey, current)
    }
    
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun removeFromList(vmKey: String, dataKey: String, index: Int) {
        val current = getValue(vmKey, dataKey) as? MutableList<Any?> ?: return
        if (index in current.indices) {
            current.removeAt(index)
            setValue(vmKey, dataKey, current)
        }
    }
    
    @JvmStatic
    @Suppress("UNCHECKED_CAST")
    fun updateListItem(vmKey: String, dataKey: String, index: Int, newValue: Any?) {
        val current = getValue(vmKey, dataKey) as? MutableList<Any?> ?: return
        if (index in current.indices) {
            current[index] = newValue
            setValue(vmKey, dataKey, current)
        }
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // State Management
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun getDataKeys(vmKey: String): List<String> {
        return viewModels[vmKey]?.getDataKeys() ?: emptyList()
    }
    
    @JvmStatic
    fun hasData(vmKey: String, dataKey: String): Boolean {
        return viewModels[vmKey]?.hasData(dataKey) ?: false
    }
    
    @JvmStatic
    fun removeData(vmKey: String, dataKey: String) {
        viewModels[vmKey]?.removeData(dataKey)
    }
    
    @JvmStatic
    fun clearData(vmKey: String) {
        viewModels[vmKey]?.clear()
    }
    
    @JvmStatic
    fun getAllData(vmKey: String): Map<String, Any?> {
        return viewModels[vmKey]?.getAllData() ?: emptyMap()
    }
    
    // ═══════════════════════════════════════════════════════════════════════════════
    // Transformations
    // ═══════════════════════════════════════════════════════════════════════════════
    
    @JvmStatic
    fun map(
        vmKey: String,
        sourceKey: String,
        destKey: String,
        transform: (Any?) -> Any?
    ) {
        viewModels[vmKey]?.map(sourceKey, destKey, transform)
    }
    
    @JvmStatic
    fun combine(
        vmKey: String,
        key1: String,
        key2: String,
        destKey: String,
        combiner: (Any?, Any?) -> Any?
    ) {
        viewModels[vmKey]?.combine(key1, key2, destKey, combiner)
    }
}

/**
 * Custom ViewModel class for Sad bridges
 */
class SadViewModel : ViewModel() {
    
    private val liveDataMap = mutableMapOf<String, MutableLiveData<Any?>>()
    private val foreverObservers = mutableMapOf<String, Observer<Any?>>()
    private var observerIdCounter = 0L
    
    fun setValue(key: String, value: Any?) {
        getLiveData(key).value = value
    }
    
    fun getValue(key: String): Any? {
        return liveDataMap[key]?.value
    }
    
    fun postValue(key: String, value: Any?) {
        getLiveData(key).postValue(value)
    }
    
    fun observe(key: String, owner: LifecycleOwner, callback: (Any?) -> Unit) {
        getLiveData(key).observe(owner) { value ->
            callback(value)
        }
    }
    
    fun observeForever(key: String, callback: (Any?) -> Unit): String {
        val observerId = "observer_${++observerIdCounter}"
        val observer = Observer<Any?> { value ->
            callback(value)
        }
        foreverObservers[observerId] = observer
        getLiveData(key).observeForever(observer)
        return observerId
    }
    
    fun removeObserver(observerId: String) {
        foreverObservers.remove(observerId)?.let { observer ->
            liveDataMap.values.forEach { liveData ->
                liveData.removeObserver(observer)
            }
        }
    }
    
    fun getDataKeys(): List<String> {
        return liveDataMap.keys.toList()
    }
    
    fun hasData(key: String): Boolean {
        return liveDataMap.containsKey(key)
    }
    
    fun removeData(key: String) {
        liveDataMap.remove(key)
    }
    
    fun clear() {
        foreverObservers.forEach { (_, observer) ->
            liveDataMap.values.forEach { liveData ->
                liveData.removeObserver(observer)
            }
        }
        foreverObservers.clear()
        liveDataMap.clear()
    }
    
    fun getAllData(): Map<String, Any?> {
        return liveDataMap.mapValues { it.value.value }
    }
    
    fun map(sourceKey: String, destKey: String, transform: (Any?) -> Any?) {
        val source = getLiveData(sourceKey)
        val dest = getLiveData(destKey)
        
        source.observeForever { value ->
            dest.value = transform(value)
        }
    }
    
    fun combine(key1: String, key2: String, destKey: String, combiner: (Any?, Any?) -> Any?) {
        val source1 = getLiveData(key1)
        val source2 = getLiveData(key2)
        val dest = getLiveData(destKey)
        
        val updateDest = {
            dest.value = combiner(source1.value, source2.value)
        }
        
        source1.observeForever { updateDest() }
        source2.observeForever { updateDest() }
    }
    
    private fun getLiveData(key: String): MutableLiveData<Any?> {
        return liveDataMap.getOrPut(key) { MutableLiveData() }
    }
    
    override fun onCleared() {
        super.onCleared()
        clear()
    }
}
