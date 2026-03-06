// بسم الله الرحمن الرحيم
// ═══════════════════════════════════════════════════════════════════════════════
// ملف: SadDialogBridge.kt
// الوصف: جسر Kotlin لمربعات الحوار
// ═══════════════════════════════════════════════════════════════════════════════

package com.sad.bridges

import android.app.Activity
import android.app.AlertDialog
import android.content.DialogInterface
import android.widget.EditText

/**
 * جسر مربعات الحوار — SadDialogBridge
 */
object SadDialogBridge {
    
    private var activity: Activity? = null
    private var currentDialog: AlertDialog? = null
    
    @JvmStatic
    fun initialize(act: Activity) {
        activity = act
    }
    
    @JvmStatic
    fun alert(
        title: String,
        message: String,
        buttonText: String = "حسناً",
        onDismiss: (() -> Unit)? = null
    ) {
        val act = activity ?: return
        
        act.runOnUiThread {
            currentDialog?.dismiss()
            currentDialog = AlertDialog.Builder(act)
                .setTitle(title)
                .setMessage(message)
                .setPositiveButton(buttonText) { dialog, _ ->
                    dialog.dismiss()
                    onDismiss?.invoke()
                }
                .setCancelable(true)
                .create()
            currentDialog?.show()
        }
    }
    
    @JvmStatic
    fun confirm(
        title: String,
        message: String,
        positiveText: String = "نعم",
        negativeText: String = "لا",
        onConfirm: () -> Unit,
        onCancel: (() -> Unit)? = null
    ) {
        val act = activity ?: return
        
        act.runOnUiThread {
            currentDialog?.dismiss()
            currentDialog = AlertDialog.Builder(act)
                .setTitle(title)
                .setMessage(message)
                .setPositiveButton(positiveText) { _, _ -> onConfirm() }
                .setNegativeButton(negativeText) { _, _ -> onCancel?.invoke() }
                .setCancelable(true)
                .create()
            currentDialog?.show()
        }
    }
    
    @JvmStatic
    fun prompt(
        title: String,
        message: String = "",
        hint: String = "",
        defaultValue: String = "",
        positiveText: String = "موافق",
        negativeText: String = "إلغاء",
        onSubmit: (String) -> Unit,
        onCancel: (() -> Unit)? = null
    ) {
        val act = activity ?: return
        
        act.runOnUiThread {
            val input = EditText(act).apply {
                hint = hint
                setText(defaultValue)
            }
            
            currentDialog?.dismiss()
            currentDialog = AlertDialog.Builder(act)
                .setTitle(title)
                .setMessage(message.ifEmpty { null })
                .setView(input)
                .setPositiveButton(positiveText) { _, _ ->
                    onSubmit(input.text.toString())
                }
                .setNegativeButton(negativeText) { _, _ ->
                    onCancel?.invoke()
                }
                .setCancelable(true)
                .create()
            currentDialog?.show()
        }
    }
    
    @JvmStatic
    fun select(
        title: String,
        items: Array<String>,
        onSelect: (Int, String) -> Unit
    ) {
        val act = activity ?: return
        
        act.runOnUiThread {
            currentDialog?.dismiss()
            currentDialog = AlertDialog.Builder(act)
                .setTitle(title)
                .setItems(items) { _, which ->
                    onSelect(which, items[which])
                }
                .setCancelable(true)
                .create()
            currentDialog?.show()
        }
    }
    
    @JvmStatic
    fun multiSelect(
        title: String,
        items: Array<String>,
        checkedItems: BooleanArray? = null,
        positiveText: String = "موافق",
        onConfirm: (List<Int>, List<String>) -> Unit
    ) {
        val act = activity ?: return
        val checked = checkedItems ?: BooleanArray(items.size) { false }
        
        act.runOnUiThread {
            currentDialog?.dismiss()
            currentDialog = AlertDialog.Builder(act)
                .setTitle(title)
                .setMultiChoiceItems(items, checked) { _, which, isChecked ->
                    checked[which] = isChecked
                }
                .setPositiveButton(positiveText) { _, _ ->
                    val selectedIndices = checked.mapIndexedNotNull { i, b -> if (b) i else null }
                    val selectedItems = selectedIndices.map { items[it] }
                    onConfirm(selectedIndices, selectedItems)
                }
                .setCancelable(true)
                .create()
            currentDialog?.show()
        }
    }
    
    @JvmStatic
    fun dismiss() {
        activity?.runOnUiThread {
            currentDialog?.dismiss()
            currentDialog = null
        }
    }
    
    @JvmStatic
    fun isShowing(): Boolean = currentDialog?.isShowing == true
}
