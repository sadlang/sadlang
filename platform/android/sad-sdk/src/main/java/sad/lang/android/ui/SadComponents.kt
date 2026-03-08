/**
 * =============================================================================
 * @file SadComponents.kt
 * @brief (AR) مكونات Compose للغة ص
 * @brief (EN) Sad Language Compose Components
 * =============================================================================
 */

package sad.lang.android.ui

import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.text.input.PasswordVisualTransformation
import androidx.compose.ui.text.input.VisualTransformation
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

// ═══════════════════════════════════════════════════════════════════════════════
//  Buttons
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * زر أساسي
 */
@Composable
fun SadButton(
    text: String,
    onClick: () -> Unit,
    modifier: Modifier = Modifier,
    enabled: Boolean = true,
    loading: Boolean = false
) {
    Button(
        onClick = onClick,
        modifier = modifier.height(48.dp),
        enabled = enabled && !loading,
        shape = RoundedCornerShape(12.dp)
    ) {
        if (loading) {
            CircularProgressIndicator(
                modifier = Modifier.size(24.dp),
                color = MaterialTheme.colorScheme.onPrimary,
                strokeWidth = 2.dp
            )
        } else {
            Text(text = text, fontSize = 16.sp)
        }
    }
}

/**
 * زر ثانوي
 */
@Composable
fun SadOutlinedButton(
    text: String,
    onClick: () -> Unit,
    modifier: Modifier = Modifier,
    enabled: Boolean = true
) {
    OutlinedButton(
        onClick = onClick,
        modifier = modifier.height(48.dp),
        enabled = enabled,
        shape = RoundedCornerShape(12.dp)
    ) {
        Text(text = text, fontSize = 16.sp)
    }
}

/**
 * زر نصي
 */
@Composable
fun SadTextButton(
    text: String,
    onClick: () -> Unit,
    modifier: Modifier = Modifier
) {
    TextButton(onClick = onClick, modifier = modifier) {
        Text(text = text)
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Text Fields
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * حقل نص
 */
@Composable
fun SadTextField(
    value: String,
    onValueChange: (String) -> Unit,
    modifier: Modifier = Modifier,
    label: String = "",
    placeholder: String = "",
    isError: Boolean = false,
    errorMessage: String = "",
    enabled: Boolean = true,
    singleLine: Boolean = true,
    maxLines: Int = 1
) {
    Column(modifier = modifier) {
        OutlinedTextField(
            value = value,
            onValueChange = onValueChange,
            modifier = Modifier.fillMaxWidth(),
            label = if (label.isNotEmpty()) {{ Text(label) }} else null,
            placeholder = if (placeholder.isNotEmpty()) {{ Text(placeholder) }} else null,
            isError = isError,
            enabled = enabled,
            singleLine = singleLine,
            maxLines = maxLines,
            shape = RoundedCornerShape(12.dp)
        )
        
        if (isError && errorMessage.isNotEmpty()) {
            Text(
                text = errorMessage,
                color = MaterialTheme.colorScheme.error,
                fontSize = 12.sp,
                modifier = Modifier.padding(start = 16.dp, top = 4.dp)
            )
        }
    }
}

/**
 * حقل كلمة مرور
 */
@Composable
fun SadPasswordField(
    value: String,
    onValueChange: (String) -> Unit,
    modifier: Modifier = Modifier,
    label: String = "كلمة المرور",
    isError: Boolean = false,
    errorMessage: String = ""
) {
    var passwordVisible by remember { mutableStateOf(false) }
    
    Column(modifier = modifier) {
        OutlinedTextField(
            value = value,
            onValueChange = onValueChange,
            modifier = Modifier.fillMaxWidth(),
            label = { Text(label) },
            isError = isError,
            singleLine = true,
            visualTransformation = if (passwordVisible) {
                VisualTransformation.None
            } else {
                PasswordVisualTransformation()
            },
            keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Password),
            trailingIcon = {
                IconButton(onClick = { passwordVisible = !passwordVisible }) {
                    Text(if (passwordVisible) "إخفاء" else "إظهار", fontSize = 12.sp)
                }
            },
            shape = RoundedCornerShape(12.dp)
        )
        
        if (isError && errorMessage.isNotEmpty()) {
            Text(
                text = errorMessage,
                color = MaterialTheme.colorScheme.error,
                fontSize = 12.sp,
                modifier = Modifier.padding(start = 16.dp, top = 4.dp)
            )
        }
    }
}

/**
 * حقل رقم
 */
@Composable
fun SadNumberField(
    value: String,
    onValueChange: (String) -> Unit,
    modifier: Modifier = Modifier,
    label: String = "",
    isDecimal: Boolean = false
) {
    OutlinedTextField(
        value = value,
        onValueChange = { newValue ->
            val filtered = if (isDecimal) {
                newValue.filter { it.isDigit() || it == '.' }
            } else {
                newValue.filter { it.isDigit() }
            }
            onValueChange(filtered)
        },
        modifier = modifier.fillMaxWidth(),
        label = if (label.isNotEmpty()) {{ Text(label) }} else null,
        singleLine = true,
        keyboardOptions = KeyboardOptions(
            keyboardType = if (isDecimal) KeyboardType.Decimal else KeyboardType.Number
        ),
        shape = RoundedCornerShape(12.dp)
    )
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Cards
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * بطاقة
 */
@Composable
fun SadCard(
    modifier: Modifier = Modifier,
    onClick: (() -> Unit)? = null,
    content: @Composable ColumnScope.() -> Unit
) {
    if (onClick != null) {
        Card(
            onClick = onClick,
            modifier = modifier,
            shape = RoundedCornerShape(16.dp),
            elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
        ) {
            Column(
                modifier = Modifier.padding(16.dp),
                content = content
            )
        }
    } else {
        Card(
            modifier = modifier,
            shape = RoundedCornerShape(16.dp),
            elevation = CardDefaults.cardElevation(defaultElevation = 4.dp)
        ) {
            Column(
                modifier = Modifier.padding(16.dp),
                content = content
            )
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Loading
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * مؤشر تحميل
 */
@Composable
fun SadLoading(
    modifier: Modifier = Modifier,
    message: String = "جاري التحميل..."
) {
    Column(
        modifier = modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        CircularProgressIndicator()
        Spacer(modifier = Modifier.height(16.dp))
        Text(text = message, color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.6f))
    }
}

/**
 * شريط تحميل خطي
 */
@Composable
fun SadProgressBar(
    progress: Float,
    modifier: Modifier = Modifier,
    label: String = ""
) {
    Column(modifier = modifier) {
        if (label.isNotEmpty()) {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceBetween
            ) {
                Text(text = label, fontSize = 14.sp)
                Text(text = "${(progress * 100).toInt()}%", fontSize = 14.sp)
            }
            Spacer(modifier = Modifier.height(4.dp))
        }
        
        LinearProgressIndicator(
            progress = { progress },
            modifier = Modifier
                .fillMaxWidth()
                .height(8.dp),
        )
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Dialogs
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * حوار تأكيد
 */
@Composable
fun SadConfirmDialog(
    title: String,
    message: String,
    confirmText: String = "تأكيد",
    dismissText: String = "إلغاء",
    onConfirm: () -> Unit,
    onDismiss: () -> Unit
) {
    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text(title) },
        text = { Text(message) },
        confirmButton = {
            SadButton(text = confirmText, onClick = onConfirm)
        },
        dismissButton = {
            SadTextButton(text = dismissText, onClick = onDismiss)
        }
    )
}

/**
 * حوار معلومات
 */
@Composable
fun SadInfoDialog(
    title: String,
    message: String,
    buttonText: String = "حسناً",
    onDismiss: () -> Unit
) {
    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text(title) },
        text = { Text(message) },
        confirmButton = {
            SadButton(text = buttonText, onClick = onDismiss)
        }
    )
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Lists
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * عنصر قائمة
 */
@Composable
fun SadListItem(
    title: String,
    subtitle: String = "",
    onClick: (() -> Unit)? = null,
    trailing: @Composable (() -> Unit)? = null
) {
    Surface(
        modifier = Modifier.fillMaxWidth(),
        onClick = onClick ?: {}
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Column(modifier = Modifier.weight(1f)) {
                Text(
                    text = title,
                    fontSize = 16.sp,
                    color = MaterialTheme.colorScheme.onSurface
                )
                if (subtitle.isNotEmpty()) {
                    Text(
                        text = subtitle,
                        fontSize = 14.sp,
                        color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.6f)
                    )
                }
            }
            
            trailing?.invoke()
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  Empty/Error States
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * حالة فارغة
 */
@Composable
fun SadEmptyState(
    message: String,
    modifier: Modifier = Modifier,
    actionText: String = "",
    onAction: (() -> Unit)? = null
) {
    Column(
        modifier = modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            text = message,
            fontSize = 16.sp,
            color = MaterialTheme.colorScheme.onSurface.copy(alpha = 0.6f),
            textAlign = TextAlign.Center
        )
        
        if (actionText.isNotEmpty() && onAction != null) {
            Spacer(modifier = Modifier.height(16.dp))
            SadButton(text = actionText, onClick = onAction)
        }
    }
}

/**
 * حالة خطأ
 */
@Composable
fun SadErrorState(
    message: String,
    modifier: Modifier = Modifier,
    retryText: String = "إعادة المحاولة",
    onRetry: (() -> Unit)? = null
) {
    Column(
        modifier = modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            text = "⚠️",
            fontSize = 48.sp
        )
        Spacer(modifier = Modifier.height(16.dp))
        Text(
            text = message,
            fontSize = 16.sp,
            color = MaterialTheme.colorScheme.error,
            textAlign = TextAlign.Center,
            modifier = Modifier.padding(horizontal = 32.dp)
        )
        
        if (onRetry != null) {
            Spacer(modifier = Modifier.height(16.dp))
            SadButton(text = retryText, onClick = onRetry)
        }
    }
}
