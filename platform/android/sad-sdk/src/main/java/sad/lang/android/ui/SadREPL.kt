/**
 * =============================================================================
 * @file SadREPL.kt
 * @brief (AR) واجهة REPL تفاعلية للغة ص
 * @brief (EN) Interactive REPL Interface for Sad Language
 * =============================================================================
 */

package sad.lang.android.ui

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontFamily
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import kotlinx.coroutines.launch
import sad.lang.android.SadResult
import sad.lang.android.SadSdk

/**
 * عنصر في سجل REPL
 */
data class ReplEntry(
    val input: String,
    val output: String,
    val isError: Boolean = false,
    val timestamp: Long = System.currentTimeMillis()
)

/**
 * واجهة REPL
 */
@Composable
fun SadREPL(
    modifier: Modifier = Modifier,
    isDarkTheme: Boolean = false
) {
    var inputText by remember { mutableStateOf("") }
    var history by remember { mutableStateOf(listOf<ReplEntry>()) }
    var isProcessing by remember { mutableStateOf(false) }
    
    val listState = rememberLazyListState()
    val scope = rememberCoroutineScope()
    
    val backgroundColor = if (isDarkTheme) Color(0xFF1E1E1E) else Color(0xFFF5F5F5)
    val inputBackgroundColor = if (isDarkTheme) Color(0xFF2D2D2D) else Color.White
    
    Column(
        modifier = modifier
            .fillMaxSize()
            .background(backgroundColor)
    ) {
        // History
        LazyColumn(
            state = listState,
            modifier = Modifier
                .weight(1f)
                .fillMaxWidth()
                .padding(8.dp)
        ) {
            // Welcome message
            item {
                Text(
                    text = "مرحباً بك في REPL لغة ص!\nاكتب كود واضغط Enter للتنفيذ.\n",
                    fontFamily = FontFamily.Monospace,
                    fontSize = 14.sp,
                    color = if (isDarkTheme) Color.Gray else Color.DarkGray
                )
            }
            
            items(history) { entry ->
                ReplEntryView(entry, isDarkTheme)
                Spacer(modifier = Modifier.height(8.dp))
            }
        }
        
        // Input
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .background(inputBackgroundColor)
                .padding(8.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Text(
                text = ">>> ",
                fontFamily = FontFamily.Monospace,
                fontSize = 16.sp,
                color = if (isDarkTheme) Color.Green else Color(0xFF2E7D32)
            )
            
            OutlinedTextField(
                value = inputText,
                onValueChange = { inputText = it },
                modifier = Modifier.weight(1f),
                enabled = !isProcessing,
                singleLine = true,
                placeholder = { Text("اكتب كود...") },
                keyboardOptions = KeyboardOptions(imeAction = ImeAction.Go),
                keyboardActions = KeyboardActions(
                    onGo = {
                        if (inputText.isNotBlank()) {
                            isProcessing = true
                            val code = inputText
                            inputText = ""
                            
                            scope.launch {
                                val result = SadSdk.interpreter.evalAsync(code)
                                val entry = when (result) {
                                    is SadResult.Success -> ReplEntry(code, result.value)
                                    is SadResult.Error -> ReplEntry(code, result.message, isError = true)
                                }
                                history = history + entry
                                isProcessing = false
                                
                                // Scroll to bottom
                                listState.animateScrollToItem(history.size)
                            }
                        }
                    }
                )
            )
            
            IconButton(
                onClick = {
                    if (inputText.isNotBlank()) {
                        isProcessing = true
                        val code = inputText
                        inputText = ""
                        
                        scope.launch {
                            val result = SadSdk.interpreter.evalAsync(code)
                            val entry = when (result) {
                                is SadResult.Success -> ReplEntry(code, result.value)
                                is SadResult.Error -> ReplEntry(code, result.message, isError = true)
                            }
                            history = history + entry
                            isProcessing = false
                            listState.animateScrollToItem(history.size)
                        }
                    }
                },
                enabled = !isProcessing && inputText.isNotBlank()
            ) {
                if (isProcessing) {
                    CircularProgressIndicator(
                        modifier = Modifier.size(24.dp),
                        strokeWidth = 2.dp
                    )
                } else {
                    Text("▶", fontSize = 20.sp)
                }
            }
        }
    }
}

@Composable
private fun ReplEntryView(entry: ReplEntry, isDarkTheme: Boolean) {
    Column(modifier = Modifier.fillMaxWidth()) {
        // Input
        Row {
            Text(
                text = ">>> ",
                fontFamily = FontFamily.Monospace,
                fontSize = 14.sp,
                color = if (isDarkTheme) Color.Green else Color(0xFF2E7D32)
            )
            Text(
                text = entry.input,
                fontFamily = FontFamily.Monospace,
                fontSize = 14.sp,
                color = if (isDarkTheme) Color.White else Color.Black
            )
        }
        
        // Output
        if (entry.output.isNotEmpty()) {
            Text(
                text = entry.output,
                fontFamily = FontFamily.Monospace,
                fontSize = 14.sp,
                color = if (entry.isError) {
                    Color.Red
                } else if (isDarkTheme) {
                    Color(0xFF9CDCFE)
                } else {
                    Color(0xFF0066CC)
                },
                modifier = Modifier.padding(start = 28.dp)
            )
        }
    }
}

/**
 * شاشة REPL كاملة
 */
@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun SadReplScreen(
    onNavigateBack: () -> Unit = {},
    isDarkTheme: Boolean = false
) {
    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("REPL لغة ص") },
                navigationIcon = {
                    IconButton(onClick = onNavigateBack) {
                        Text("←", fontSize = 24.sp)
                    }
                },
                actions = {
                    // Clear button
                    IconButton(onClick = { /* Clear history */ }) {
                        Text("🗑️")
                    }
                }
            )
        }
    ) { padding ->
        SadREPL(
            modifier = Modifier.padding(padding),
            isDarkTheme = isDarkTheme
        )
    }
}
