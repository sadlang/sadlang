package com.sad.app;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.LinearLayout;
import android.widget.FrameLayout;
import android.view.Gravity;
import android.graphics.Color;
import android.graphics.Typeface;
import android.text.method.ScrollingMovementMethod;
import android.view.View;
import java.io.InputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;

/**
 * النشاط الرئيسي لتطبيق ص — يعرض بيئة تنفيذ كود ص
 * Main Activity for Sad Language — shows code execution environment
 */
public class MainActivity extends Activity {
    
    private EditText codeInput;
    private TextView outputView;
    private TextView statusBar;
    private FrameLayout outputContainer;
    private FrameLayout nativeUIContainer;
    private ScrollView textOutputScroll;
    private boolean showingUI = false;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // بناء واجهة المستخدم برمجياً (بدون XML layout)
        LinearLayout root = new LinearLayout(this);
        root.setOrientation(LinearLayout.VERTICAL);
        root.setBackgroundColor(Color.parseColor("#1E1E2E"));
        root.setPadding(24, 48, 24, 24);
        
        // --- العنوان ---
        TextView title = new TextView(this);
        title.setText("🔤 لغة ص — Sad Language");
        title.setTextSize(22);
        title.setTextColor(Color.WHITE);
        title.setGravity(Gravity.CENTER);
        title.setPadding(0, 0, 0, 16);
        root.addView(title);
        
        // --- شريط الإصدار ---
        statusBar = new TextView(this);
        statusBar.setTextSize(12);
        statusBar.setTextColor(Color.parseColor("#89B4FA"));
        statusBar.setGravity(Gravity.CENTER);
        statusBar.setPadding(0, 0, 0, 16);
        root.addView(statusBar);
        
        // --- محرر الكود ---
        codeInput = new EditText(this);
        codeInput.setHint("اكتب كود ص هنا...");
        codeInput.setHintTextColor(Color.parseColor("#6C7086"));
        codeInput.setTextColor(Color.parseColor("#CDD6F4"));
        codeInput.setBackgroundColor(Color.parseColor("#313244"));
        codeInput.setTextSize(14);
        codeInput.setTypeface(Typeface.MONOSPACE);
        codeInput.setGravity(Gravity.TOP | Gravity.START);
        codeInput.setPadding(16, 16, 16, 16);
        codeInput.setMinLines(8);
        codeInput.setMaxLines(12);
        
        LinearLayout.LayoutParams codeParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT, 0, 3);
        codeParams.setMargins(0, 0, 0, 12);
        root.addView(codeInput, codeParams);
        
        // --- أزرار التحكم ---
        LinearLayout btnRow = new LinearLayout(this);
        btnRow.setOrientation(LinearLayout.HORIZONTAL);
        btnRow.setGravity(Gravity.CENTER);
        
        Button runBtn = createButton("▶ تنفيذ", "#A6E3A1");
        runBtn.setOnClickListener(v -> executeCode());
        btnRow.addView(runBtn);
        
        Button clearBtn = createButton("🗑 مسح", "#F38BA8");
        clearBtn.setOnClickListener(v -> {
            codeInput.setText("");
            outputView.setText("");
        });
        btnRow.addView(clearBtn);
        
        Button demoBtn = createButton("📝 مثال", "#89B4FA");
        demoBtn.setOnClickListener(v -> loadDemo());
        btnRow.addView(demoBtn);
        
        Button calcBtn = createButton("🧮 آلة حاسبة", "#F9E2AF");
        calcBtn.setOnClickListener(v -> loadCalculator());
        btnRow.addView(calcBtn);
        
        Button uiBtn = createButton("🖼 واجهة", "#CBA6F7");
        uiBtn.setOnClickListener(v -> loadUIDemo());
        btnRow.addView(uiBtn);
        
        root.addView(btnRow);
        
        // --- المخرجات ---
        TextView outputLabel = new TextView(this);
        outputLabel.setText("📤 المخرجات:");
        outputLabel.setTextSize(14);
        outputLabel.setTextColor(Color.parseColor("#A6ADC8"));
        outputLabel.setPadding(0, 16, 0, 8);
        root.addView(outputLabel);
        
        // حاوية المخرجات (نص + واجهة أصلية)
        outputContainer = new FrameLayout(this);
        
        textOutputScroll = new ScrollView(this);
        outputView = new TextView(this);
        outputView.setTextColor(Color.parseColor("#A6E3A1"));
        outputView.setBackgroundColor(Color.parseColor("#181825"));
        outputView.setTextSize(13);
        outputView.setTypeface(Typeface.MONOSPACE);
        outputView.setPadding(16, 16, 16, 16);
        outputView.setText("مرحباً بك في لغة ص! 🎉\nاضغط 'مثال' أو اكتب كوداً واضغط 'تنفيذ'");
        textOutputScroll.addView(outputView);
        outputContainer.addView(textOutputScroll, new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));
        
        // حاوية الواجهة الأصلية (بدلاً من WebView)
        nativeUIContainer = new FrameLayout(this);
        nativeUIContainer.setBackgroundColor(Color.parseColor("#F5F5F0"));
        nativeUIContainer.setVisibility(View.GONE);
        outputContainer.addView(nativeUIContainer, new FrameLayout.LayoutParams(
            FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.MATCH_PARENT));
        
        LinearLayout.LayoutParams outputParams = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.MATCH_PARENT, 0, 4);
        outputParams.setMargins(0, 0, 0, 0);
        root.addView(outputContainer, outputParams);
        
        setContentView(root);
        
        // تهيئة المحرك
        initEngine();
    }
    
    private Button createButton(String text, String color) {
        Button btn = new Button(this);
        btn.setText(text);
        btn.setTextSize(12);
        btn.setTextColor(Color.parseColor("#1E1E2E"));
        btn.setBackgroundColor(Color.parseColor(color));
        btn.setPadding(20, 8, 20, 8);
        LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(
            LinearLayout.LayoutParams.WRAP_CONTENT,
            LinearLayout.LayoutParams.WRAP_CONTENT);
        params.setMargins(6, 8, 6, 8);
        btn.setLayoutParams(params);
        return btn;
    }
    
    private void initEngine() {
        try {
            boolean ok = SadEngine.nativeInit(this);
            String ver = SadEngine.nativeGetVersion();
            statusBar.setText("الإصدار: " + ver + " | الحالة: " + (ok ? "✅ جاهز" : "❌ خطأ"));
        } catch (Exception e) {
            statusBar.setText("❌ خطأ في التهيئة: " + e.getMessage());
        }
    }
    
    private void executeCode() {
        String code = codeInput.getText().toString().trim();
        if (code.isEmpty()) {
            outputView.setText("⚠ لا يوجد كود للتنفيذ");
            return;
        }
        
        outputView.setText("⏳ جاري التنفيذ...\n");
        showTextOutput();
        
        try {
            boolean success = SadEngine.nativeExecute(code);
            
            if (success) {
                // التحقق من وجود واجهة أصلية مولّدة (JNI مباشر)
                if (SadEngine.nativeHasNativeUI()) {
                    showNativeUI();
                    return;
                }
                
                String output = SadEngine.nativeGetLastOutput();
                outputView.setText("✅ نجاح!\n\n" + 
                    (output != null && !output.isEmpty() ? output : "(لا مخرجات)"));
            } else {
                String error = SadEngine.nativeGetLastError();
                outputView.setText("❌ خطأ:\n" + 
                    (error != null ? error : "خطأ غير معروف"));
            }
        } catch (Exception e) {
            outputView.setText("💥 استثناء: " + e.getMessage());
        }
    }
    
    private void showTextOutput() {
        showingUI = false;
        nativeUIContainer.setVisibility(View.GONE);
        nativeUIContainer.removeAllViews();
        textOutputScroll.setVisibility(View.VISIBLE);
    }
    
    private void showNativeUI() {
        showingUI = true;
        textOutputScroll.setVisibility(View.GONE);
        nativeUIContainer.setVisibility(View.VISIBLE);
        nativeUIContainer.removeAllViews();
        try {
            boolean built = SadEngine.nativeBuildNativeUI(nativeUIContainer);
            if (!built) {
                showTextOutput();
                outputView.setText("❌ فشل بناء الواجهة الأصلية");
            }
        } catch (Exception e) {
            showTextOutput();
            outputView.setText("💥 استثناء في بناء الواجهة: " + e.getMessage());
        }
    }
    
    private void loadDemo() {
        String demo = 
            "# مثال بسيط بلغة ص\n" +
            "متغير الاسم = \"أندرويد\"\n" +
            "اطبع_سطر(\"مرحباً من لغة ص على \" + الاسم + \"! 🎉\")\n\n" +
            "# حلقة بسيطة\n" +
            "لكل ع في [1، 2، 3، 4، 5]\n" +
            "    اطبع_سطر(\"العدد: \" + ع)\n" +
            "نهاية\n\n" +
            "# دالة\n" +
            "دالة مضروب(ن)\n" +
            "    إذا (ن <= 1)\n" +
            "        ارجع 1\n" +
            "    نهاية\n" +
            "    ارجع ن * مضروب(ن - 1)\n" +
            "نهاية\n\n" +
            "اطبع_سطر(\"مضروب 10 = \" + مضروب(10))";
        
        codeInput.setText(demo);
    }
    
    private void loadCalculator() {
        String calc = 
            "# آلة حاسبة بسيطة بلغة ص\n" +
            "دالة حاسبة(أ، عملية، ب)\n" +
            "    طابق (عملية)\n" +
            "        عندما \"+\":\n" +
            "            ارجع أ + ب\n" +
            "        عندما \"-\":\n" +
            "            ارجع أ - ب\n" +
            "        عندما \"*\":\n" +
            "            ارجع أ * ب\n" +
            "        عندما \"/\":\n" +
            "            إذا (ب == 0)\n" +
            "                ارجع \"خطأ: قسمة على صفر!\"\n" +
            "            نهاية\n" +
            "            ارجع أ / ب\n" +
            "        افتراضي:\n" +
            "            ارجع \"عملية غير معروفة\"\n" +
            "    نهاية\n" +
            "نهاية\n\n" +
            "اطبع_سطر(\"🧮 آلة حاسبة بلغة ص\")\n" +
            "اطبع_سطر(\"═══════════════════\")\n" +
            "اطبع_سطر(\"10 + 5 = \" + حاسبة(10، \"+\"، 5))\n" +
            "اطبع_سطر(\"20 - 8 = \" + حاسبة(20، \"-\"، 8))\n" +
            "اطبع_سطر(\"7 × 6 = \" + حاسبة(7، \"*\"، 6))\n" +
            "اطبع_سطر(\"100 ÷ 4 = \" + حاسبة(100، \"/\"، 4))\n" +
            "اطبع_سطر(\"القسمة على صفر: \" + حاسبة(5، \"/\"، 0))";
        
        codeInput.setText(calc);
    }
    
    private void loadUIDemo() {
        String uiDemo =
            "# مثال واجهة رسومية بلغة ص على أندرويد\n" +
            "صنف عنصر_واجهة\n" +
            "    باني(النوع)\n" +
            "        هذا._نوع = النوع\n" +
            "        هذا._أبناء = []\n" +
            "    نهاية\n" +
            "    دالة اضف(ابن)\n" +
            "        هذا._أبناء = هذا._أبناء + [ابن]\n" +
            "        ارجع هذا\n" +
            "    نهاية\n" +
            "نهاية\n\n" +
            "صنف نص_واجهة يرث عنصر_واجهة\n" +
            "    باني(محتوى)\n" +
            "        الأساس(\"نص\")\n" +
            "        هذا._محتوى = محتوى\n" +
            "    نهاية\n" +
            "نهاية\n\n" +
            "صنف زر_واجهة يرث عنصر_واجهة\n" +
            "    باني(عنوان)\n" +
            "        الأساس(\"زر\")\n" +
            "        هذا._عنوان = عنوان\n" +
            "    نهاية\n" +
            "نهاية\n\n" +
            "صنف عمود_واجهة يرث عنصر_واجهة\n" +
            "    باني()\n" +
            "        الأساس(\"عمود\")\n" +
            "    نهاية\n" +
            "نهاية\n\n" +
            "صنف صف_واجهة يرث عنصر_واجهة\n" +
            "    باني()\n" +
            "        الأساس(\"صف\")\n" +
            "    نهاية\n" +
            "نهاية\n\n" +
            "# بناء الواجهة\n" +
            "متغير التطبيق = جديد عمود_واجهة()\n" +
            "التطبيق.اضف(جديد نص_واجهة(\"مرحبا من لغة ص!\"))\n" +
            "التطبيق.اضف(جديد نص_واجهة(\"هذه واجهة مولدة بالكود\"))\n\n" +
            "متغير ازرار = جديد صف_واجهة()\n" +
            "ازرار.اضف(جديد زر_واجهة(\"زر 1\"))\n" +
            "ازرار.اضف(جديد زر_واجهة(\"زر 2\"))\n" +
            "ازرار.اضف(جديد زر_واجهة(\"زر 3\"))\n" +
            "التطبيق.اضف(ازرار)\n\n" +
            "التطبيق.اضف(جديد نص_واجهة(\"واجهة عربية بالكامل\"))\n\n" +
            "_محرك_واجهات(التطبيق)\n";
        
        codeInput.setText(uiDemo);
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        SadEngine.nativeShutdown();
    }
}
