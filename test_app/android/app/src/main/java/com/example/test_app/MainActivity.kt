// ╔════════════════════════════════════════════════════════════════════╗
// ║    test_app - مُولَّد تلقائياً بواسطة لغة ص             ║
// ╚════════════════════════════════════════════════════════════════════╝

package com.example.test_app

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                MainScreen()
            }
        }
    }
}

@Composable
fun MainScreen() {
    var counter by remember { mutableIntStateOf(0) }

    Surface(
        modifier = Modifier.fillMaxSize(),
        color = MaterialTheme.colorScheme.background
    ) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(24.dp),
            verticalArrangement = Arrangement.Center,
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                text = "\uD83C\uDF1F مرحباً من لغة ص!",
                fontSize = 28.sp,
                textAlign = TextAlign.Center,
                color = MaterialTheme.colorScheme.primary
            )
            Spacer(modifier = Modifier.height(16.dp))
            Text(
                text = "العداد: $counter",
                fontSize = 22.sp
            )
            Spacer(modifier = Modifier.height(24.dp))
            Button(onClick = { counter++ }) {
                Text(text = "+1", fontSize = 18.sp)
            }
            Spacer(modifier = Modifier.height(12.dp))
            OutlinedButton(onClick = { counter = 0 }) {
                Text(text = "إعادة تعيين", fontSize = 16.sp)
            }
        }
    }
}
