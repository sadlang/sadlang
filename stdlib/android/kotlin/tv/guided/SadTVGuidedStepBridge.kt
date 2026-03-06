package sad.android.tv.guided

import android.content.Context
import android.os.Handler
import android.os.Looper

/**
 * جسر الخطوات الموجهة التلفزيونية - GuidedStepSupportFragment
 * TV Guided Step Bridge - GuidedStepSupportFragment
 * 
 * @author فريق لغة ص
 */
object SadTVGuidedStepBridge {
    
    private var context: Context? = null
    private val mainHandler = Handler(Looper.getMainLooper())
    
    /**
     * إجراء موجه / Guided action
     */
    data class GuidedAction(
        val id: Long,
        val title: String,
        val description: String = "",
        val iconResId: Int? = null,
        val isEditable: Boolean = false,
        val editTitle: String = "",
        val editDescription: String = "",
        val inputType: InputType = InputType.NORMAL,
        val subActions: List<GuidedAction> = emptyList(),
        val isChecked: Boolean = false,
        val checkSetId: Int = NO_CHECK_SET,
        val isEnabled: Boolean = true,
        val isFocusable: Boolean = true
    ) {
        companion object {
            const val NO_CHECK_SET = -1
            const val CHECKBOX_CHECK_SET_ID = 0
        }
    }
    
    /**
     * نوع الإدخال / Input type
     */
    enum class InputType {
        NORMAL,
        PASSWORD,
        EMAIL,
        NUMERIC,
        DATE
    }
    
    /**
     * معلومات التوجيه / Guidance info
     */
    data class GuidanceInfo(
        val title: String,
        val description: String = "",
        val breadcrumb: String = "",
        val iconResId: Int? = null
    )
    
    /**
     * تكوين الخطوة / Step config
     */
    data class StepConfig(
        val hasContinueButton: Boolean = true,
        val continueButtonTitle: String = "التالي",
        val hasCancelButton: Boolean = true,
        val cancelButtonTitle: String = "إلغاء",
        val focusedActionIndex: Int = 0,
        val theme: StepTheme = StepTheme.DEFAULT
    )
    
    /**
     * سمة الخطوة / Step theme
     */
    enum class StepTheme {
        DEFAULT,
        DARK,
        LIGHT,
        TRANSLUCENT
    }
    
    /**
     * مستمع الخطوة / Step listener
     */
    interface GuidedStepListener {
        fun onActionClicked(action: GuidedAction)
        fun onSubActionClicked(action: GuidedAction)
        fun onActionEdited(action: GuidedAction, newValue: String)
        fun onContinue()
        fun onCancel()
    }
    
    private var guidedStepListener: GuidedStepListener? = null
    private var guidanceInfo: GuidanceInfo? = null
    private var config = StepConfig()
    private val actions = mutableListOf<GuidedAction>()
    private var selectedActionIndex = 0
    
    // ═══════════════════════════════════════════════════════════════
    //                         التهيئة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تهيئة / Initialize
     */
    fun initialize(ctx: Context) {
        context = ctx.applicationContext
    }
    
    /**
     * تعيين المستمع / Set listener
     */
    fun setGuidedStepListener(listener: GuidedStepListener?) {
        guidedStepListener = listener
    }
    
    /**
     * تعيين معلومات التوجيه / Set guidance info
     */
    fun setGuidanceInfo(info: GuidanceInfo) {
        guidanceInfo = info
    }
    
    /**
     * تعيين التكوين / Set config
     */
    fun setConfig(newConfig: StepConfig) {
        config = newConfig
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         إدارة الإجراءات
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة إجراء / Add action
     */
    fun addAction(action: GuidedAction) {
        actions.add(action)
    }
    
    /**
     * إضافة إجراء بسيط / Add simple action
     */
    fun addAction(id: Long, title: String, description: String = "") {
        addAction(GuidedAction(id, title, description))
    }
    
    /**
     * إضافة خيار تبديل / Add checkbox action
     */
    fun addCheckboxAction(id: Long, title: String, isChecked: Boolean = false) {
        addAction(GuidedAction(
            id = id,
            title = title,
            isChecked = isChecked,
            checkSetId = GuidedAction.CHECKBOX_CHECK_SET_ID
        ))
    }
    
    /**
     * إضافة حقل إدخال / Add editable action
     */
    fun addEditableAction(
        id: Long,
        title: String,
        description: String = "",
        inputType: InputType = InputType.NORMAL
    ) {
        addAction(GuidedAction(
            id = id,
            title = title,
            description = description,
            isEditable = true,
            editTitle = title,
            inputType = inputType
        ))
    }
    
    /**
     * تعيين الإجراءات / Set actions
     */
    fun setActions(newActions: List<GuidedAction>) {
        actions.clear()
        actions.addAll(newActions)
    }
    
    /**
     * الحصول على الإجراءات / Get actions
     */
    fun getActions(): List<GuidedAction> = actions.toList()
    
    /**
     * الحصول على إجراء / Get action
     */
    fun getAction(id: Long): GuidedAction? = actions.find { it.id == id }
    
    /**
     * تحديث إجراء / Update action
     */
    fun updateAction(id: Long, update: (GuidedAction) -> GuidedAction): Boolean {
        val index = actions.indexOfFirst { it.id == id }
        if (index < 0) return false
        
        actions[index] = update(actions[index])
        return true
    }
    
    /**
     * مسح الإجراءات / Clear actions
     */
    fun clearActions() {
        actions.clear()
        selectedActionIndex = 0
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         التنقل
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * تحديد إجراء / Select action
     */
    fun selectAction(index: Int): Boolean {
        if (index in 0 until actions.size && actions[index].isFocusable) {
            selectedActionIndex = index
            return true
        }
        return false
    }
    
    /**
     * الإجراء التالي / Next action
     */
    fun nextAction(): Boolean {
        for (i in (selectedActionIndex + 1) until actions.size) {
            if (actions[i].isFocusable) {
                selectedActionIndex = i
                return true
            }
        }
        return false
    }
    
    /**
     * الإجراء السابق / Previous action
     */
    fun previousAction(): Boolean {
        for (i in (selectedActionIndex - 1) downTo 0) {
            if (actions[i].isFocusable) {
                selectedActionIndex = i
                return true
            }
        }
        return false
    }
    
    /**
     * الحصول على الإجراء المحدد / Get selected action
     */
    fun getSelectedAction(): GuidedAction? = actions.getOrNull(selectedActionIndex)
    
    /**
     * النقر على المحدد / Click selected
     */
    fun clickSelectedAction() {
        val action = getSelectedAction() ?: return
        
        if (!action.isEnabled) return
        
        // تبديل checkbox
        if (action.checkSetId != GuidedAction.NO_CHECK_SET) {
            updateAction(action.id) { it.copy(isChecked = !it.isChecked) }
        }
        
        mainHandler.post { guidedStepListener?.onActionClicked(action) }
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         الأزرار
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إضافة أزرار افتراضية / Add default buttons
     */
    fun addDefaultButtons() {
        if (config.hasContinueButton) {
            addAction(GuidedAction(
                id = ACTION_CONTINUE,
                title = config.continueButtonTitle
            ))
        }
        
        if (config.hasCancelButton) {
            addAction(GuidedAction(
                id = ACTION_CANCEL,
                title = config.cancelButtonTitle
            ))
        }
    }
    
    /**
     * معالجة نقر الزر / Handle button click
     */
    fun handleButtonClick(actionId: Long) {
        when (actionId) {
            ACTION_CONTINUE -> mainHandler.post { guidedStepListener?.onContinue() }
            ACTION_CANCEL -> mainHandler.post { guidedStepListener?.onCancel() }
        }
    }
    
    companion object {
        const val ACTION_CONTINUE = -1000L
        const val ACTION_CANCEL = -1001L
    }
    
    // ═══════════════════════════════════════════════════════════════
    //                         نماذج جاهزة
    // ═══════════════════════════════════════════════════════════════
    
    /**
     * إنشاء نموذج تسجيل دخول / Create login form
     */
    fun createLoginForm(): List<GuidedAction> {
        return listOf(
            GuidedAction(
                id = 1,
                title = "البريد الإلكتروني",
                description = "أدخل بريدك الإلكتروني",
                isEditable = true,
                inputType = InputType.EMAIL
            ),
            GuidedAction(
                id = 2,
                title = "كلمة المرور",
                description = "أدخل كلمة المرور",
                isEditable = true,
                inputType = InputType.PASSWORD
            ),
            GuidedAction(
                id = 3,
                title = "تذكرني",
                isChecked = false,
                checkSetId = GuidedAction.CHECKBOX_CHECK_SET_ID
            ),
            GuidedAction(
                id = ACTION_CONTINUE,
                title = "تسجيل الدخول"
            ),
            GuidedAction(
                id = ACTION_CANCEL,
                title = "إلغاء"
            )
        )
    }
    
    /**
     * إنشاء نموذج اختيار / Create selection form
     */
    fun createSelectionForm(title: String, options: List<Pair<Long, String>>, selectedId: Long? = null): List<GuidedAction> {
        setGuidanceInfo(GuidanceInfo(title = title))
        
        return options.map { (id, label) ->
            GuidedAction(
                id = id,
                title = label,
                isChecked = id == selectedId,
                checkSetId = 1 // نفس المجموعة = radio buttons
            )
        }
    }
    
    /**
     * تحرير الموارد / Release resources
     */
    fun release() {
        actions.clear()
        guidanceInfo = null
        guidedStepListener = null
        context = null
    }
}
