/**
 * =============================================================================
 * @file SadCodeEditor.kt
 * @brief (AR) محرر كود لغة ص لأندرويد
 * @brief (EN) Sad Language Code Editor for Android
 * =============================================================================
 */

package sad.lang.android.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.horizontalScroll
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.SolidColor
import androidx.compose.ui.text.*
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// ═══════════════════════════════════════════════════════════════════════════════
//  Syntax Colors
// ═══════════════════════════════════════════════════════════════════════════════

object SadSyntaxColors {
    // Light theme colors
    val Keyword = Color(0xFF0033B3)
    val String = Color(0xFF067D17)
    val Number = Color(0xFF1750EB)
    val Comment = Color(0xFF8C8C8C)
    val Function = Color(0xFF00627A)
    val Variable = Color(0xFF871094)
    val Operator = Color(0xFF000000)
    val Type = Color(0xFF00677C)
    
    // Dark theme colors
    val KeywordDark = Color(0xFFCC7832)
    val StringDark = Color(0xFF6A8759)
    val NumberDark = Color(0xFF6897BB)
    val CommentDark = Color(0xFF808080)
    val FunctionDark = Color(0xFFFFC66D)
    val VariableDark = Color(0xFF9876AA)
    val OperatorDark = Color(0xFFA9B7C6)
    val TypeDark = Color(0xFF6897BB)
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Sad Keywords
// ═══════════════════════════════════════════════════════════════════════════════

private val SadKeywords = setOf(
    // Reserved keywords (40)
    "دالة", "ارجع", "صنف", "بنية", "تعداد", "يرث", "نهاية", "جديد", "هذا", "باني", "الأساس",
    "إذا", "اذا", "وإلا", "والا", "بينما", "لكل", "في", "توقف", "استمر",
    "طابق", "عندما", "افتراضي",
    "حاول", "امسك", "ارمي", "أخيراً", "اخيرا",
    "عام", "خاص", "محمي", "مجرد",
    "استورد", "من", "كـ", "صدّر", "صدر",
    "متغير", "ثابت", "ساكن", "خارجي",
    "صحيح", "خطأ", "لاشيء",
    
    // Logical operators
    "و", "أو", "او", "ليس"
)

private val SadContextualKeywords = setOf(
    "غير_متزامن", "انتظر", "لامدا", "أنتج", "انتج", "مولد", "باستخدام", "نهاية_استخدام",
    "سمة", "واجهة", "نفّذ", "نفذ", "قالب", "فضاء", "نهاية_فضاء",
    "اختبر", "خاصية", "احصل", "عيّن", "عين", "هدم", "عامل", "رئيسية", "حالة"
)

private val SadTypes = setOf(
    "رقم", "عشري", "نص", "منطقي", "فراغ", "عدم", "مصفوفة", "خريطة", "أي"
)

private val SadBuiltins = setOf(
    "اطبع", "اطبع_سطر", "اقرأ", "اقرا", "اقرأ_سطر", "طول", "نوع"
)

// ═══════════════════════════════════════════════════════════════════════════════
//  Syntax Highlighter
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * تلوين الكود
 */
fun highlightSadCode(code: String, isDarkTheme: Boolean = false): AnnotatedString {
    return buildAnnotatedString {
        val colors = if (isDarkTheme) {
            object {
                val keyword = SadSyntaxColors.KeywordDark
                val string = SadSyntaxColors.StringDark
                val number = SadSyntaxColors.NumberDark
                val comment = SadSyntaxColors.CommentDark
                val function = SadSyntaxColors.FunctionDark
                val variable = SadSyntaxColors.VariableDark
                val type = SadSyntaxColors.TypeDark
                val normal = Color.White
            }
        } else {
            object {
                val keyword = SadSyntaxColors.Keyword
                val string = SadSyntaxColors.String
                val number = SadSyntaxColors.Number
                val comment = SadSyntaxColors.Comment
                val function = SadSyntaxColors.Function
                val variable = SadSyntaxColors.Variable
                val type = SadSyntaxColors.Type
                val normal = Color.Black
            }
        }
        
        var i = 0
        while (i < code.length) {
            when {
                // Single-line comment
                code.startsWith("#", i) && !code.startsWith("#*", i) -> {
                    val end = code.indexOf('\n', i).let { if (it == -1) code.length else it }
                    withStyle(SpanStyle(color = colors.comment)) {
                        append(code.substring(i, end))
                    }
                    i = end
                }
                
                // Multi-line comment
                code.startsWith("#*", i) -> {
                    val end = code.indexOf("*#", i + 2).let { if (it == -1) code.length else it + 2 }
                    withStyle(SpanStyle(color = colors.comment)) {
                        append(code.substring(i, end))
                    }
                    i = end
                }
                
                // String
                code[i] == '"' -> {
                    val end = findStringEnd(code, i)
                    withStyle(SpanStyle(color = colors.string)) {
                        append(code.substring(i, end))
                    }
                    i = end
                }
                
                // Number
                code[i].isDigit() -> {
                    val end = findNumberEnd(code, i)
                    withStyle(SpanStyle(color = colors.number)) {
                        append(code.substring(i, end))
                    }
                    i = end
                }
                
                // Identifier/Keyword
                code[i].isLetter() || code[i] == '_' || code[i].code > 127 -> {
                    val end = findIdentifierEnd(code, i)
                    val word = code.substring(i, end)
                    
                    val style = when {
                        word in SadKeywords -> SpanStyle(color = colors.keyword, fontWeight = FontWeight.Bold)
                        word in SadContextualKeywords -> SpanStyle(color = colors.keyword)
                        word in SadTypes -> SpanStyle(color = colors.type)
                        word in SadBuiltins -> SpanStyle(color = colors.function)
                        else -> SpanStyle(color = colors.normal)
                    }
                    
                    withStyle(style) {
                        append(word)
                    }
                    i = end
                }
                
                // Other characters
                else -> {
                    withStyle(SpanStyle(color = colors.normal)) {
                        append(code[i])
                    }
                    i++
                }
            }
        }
    }
}

private fun findStringEnd(code: String, start: Int): Int {
    var i = start + 1
    while (i < code.length) {
        if (code[i] == '"' && (i == 0 || code[i - 1] != '\\')) {
            return i + 1
        }
        i++
    }
    return code.length
}

private fun findNumberEnd(code: String, start: Int): Int {
    var i = start
    var hasDecimal = false
    while (i < code.length) {
        when {
            code[i].isDigit() -> i++
            code[i] == '.' && !hasDecimal -> {
                hasDecimal = true
                i++
            }
            else -> break
        }
    }
    return i
}

private fun findIdentifierEnd(code: String, start: Int): Int {
    var i = start
    while (i < code.length && (code[i].isLetterOrDigit() || code[i] == '_' || code[i].code > 127)) {
        i++
    }
    return i
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Code Editor Composable
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * محرر كود لغة ص
 */
@Composable
fun SadCodeEditor(
    code: String,
    onCodeChange: (String) -> Unit,
    modifier: Modifier = Modifier,
    isDarkTheme: Boolean = false,
    showLineNumbers: Boolean = true,
    readOnly: Boolean = false
) {
    val backgroundColor = if (isDarkTheme) Color(0xFF1E1E1E) else Color(0xFFF5F5F5)
    val lineNumberColor = if (isDarkTheme) Color(0xFF606366) else Color(0xFF999999)
    
    val lines = code.split("\n")
    val highlightedCode = remember(code, isDarkTheme) {
        highlightSadCode(code, isDarkTheme)
    }
    
    Row(
        modifier = modifier
            .background(backgroundColor)
            .horizontalScroll(rememberScrollState())
    ) {
        // Line numbers
        if (showLineNumbers) {
            Column(
                modifier = Modifier
                    .verticalScroll(rememberScrollState())
                    .padding(horizontal = 8.dp, vertical = 8.dp)
            ) {
                lines.forEachIndexed { index, _ ->
                    Text(
                        text = "${index + 1}",
                        fontFamily = FontFamily.Monospace,
                        fontSize = 14.sp,
                        color = lineNumberColor,
                        modifier = Modifier.padding(vertical = 1.dp)
                    )
                }
            }
            
            Divider(
                modifier = Modifier
                    .fillMaxHeight()
                    .width(1.dp),
                color = lineNumberColor.copy(alpha = 0.3f)
            )
        }
        
        // Code editor
        if (readOnly) {
            Text(
                text = highlightedCode,
                fontFamily = FontFamily.Monospace,
                fontSize = 14.sp,
                modifier = Modifier
                    .verticalScroll(rememberScrollState())
                    .padding(8.dp)
            )
        } else {
            BasicTextField(
                value = code,
                onValueChange = onCodeChange,
                modifier = Modifier
                    .fillMaxWidth()
                    .verticalScroll(rememberScrollState())
                    .padding(8.dp),
                textStyle = TextStyle(
                    fontFamily = FontFamily.Monospace,
                    fontSize = 14.sp,
                    color = if (isDarkTheme) Color.White else Color.Black
                ),
                cursorBrush = SolidColor(if (isDarkTheme) Color.White else Color.Black),
                decorationBox = { innerTextField ->
                    Box {
                        if (code.isEmpty()) {
                            Text(
                                text = "# اكتب كود لغة ص هنا...",
                                fontFamily = FontFamily.Monospace,
                                fontSize = 14.sp,
                                color = if (isDarkTheme) Color.Gray else Color.LightGray
                            )
                        }
                        innerTextField()
                    }
                }
            )
        }
    }
}

/**
 * عارض كود (للقراءة فقط)
 */
@Composable
fun SadCodeViewer(
    code: String,
    modifier: Modifier = Modifier,
    isDarkTheme: Boolean = false,
    showLineNumbers: Boolean = true
) {
    SadCodeEditor(
        code = code,
        onCodeChange = {},
        modifier = modifier,
        isDarkTheme = isDarkTheme,
        showLineNumbers = showLineNumbers,
        readOnly = true
    )
}
