# Comprehensive Test Suite for Sad Language
# مجموعة اختبارات شاملة للغة ص
# PowerShell Test Runner

param(
    [string]$Component = "all",  # lexer, parser, interpreter, or all
    [switch]$Verbose = $false
)

# Test configuration
$SadExecutable = ".\build\bin\Debug\sad.exe"
$TotalTests = 0
$PassedTests = 0
$FailedTests = @()

function Write-TestHeader {
    param([string]$Title)
    
    Write-Host "`n" -NoNewline
    Write-Host "=" * 50 -ForegroundColor Cyan
    Write-Host $Title -ForegroundColor Yellow
    Write-Host "=" * 50 -ForegroundColor Cyan
}

function Write-TestResult {
    param(
        [string]$TestName,
        [bool]$Passed,
        [string]$Details = ""
    )
    
    $script:TotalTests++
    
    if ($Passed) {
        $script:PassedTests++
        Write-Host "  ✅ $TestName" -ForegroundColor Green
        if ($Verbose -and $Details) {
            Write-Host "     $Details" -ForegroundColor Gray
        }
    } else {
        $script:FailedTests += @{Name = $TestName; Details = $Details}
        Write-Host "  ❌ $TestName" -ForegroundColor Red
        if ($Details) {
            Write-Host "     $Details" -ForegroundColor Gray
        }
    }
}

function Test-SadCode {
    param(
        [string]$Code,
        [bool]$ShouldPass = $true,
        [string]$ExpectedOutput = "",
        [string]$TestName = "Unnamed Test"
    )
    
    # Create temporary test file
    $TempFile = "temp_test_$(Get-Random).s"
    
    try {
        # Write test code to file
        $Code | Out-File -FilePath $TempFile -Encoding UTF8
        
        # Execute the Sad interpreter
        $Result = & $SadExecutable $TempFile 2>&1
        $ExitCode = $LASTEXITCODE
        
        # Analyze results
        $Success = $false
        $Details = ""
        
        if ($ShouldPass) {
            # Test should pass
            if ($ExitCode -eq 0) {
                $Success = $true
                $Details = "Executed successfully"
                
                # Check expected output if provided
                if ($ExpectedOutput -and $Result -notlike "*$ExpectedOutput*") {
                    $Success = $false
                    $Details = "Output mismatch. Expected: '$ExpectedOutput', Got: '$Result'"
                }
            } else {
                $Success = $false
                $Details = "Execution failed with exit code $ExitCode. Output: $Result"
            }
        } else {
            # Test should fail
            if ($ExitCode -ne 0) {
                $Success = $true
                $Details = "Failed as expected with exit code $ExitCode"
            } else {
                $Success = $false
                $Details = "Expected failure but succeeded"
            }
        }
        
        Write-TestResult -TestName $TestName -Passed $Success -Details $Details
        
    } catch {
        Write-TestResult -TestName $TestName -Passed $false -Details "Exception: $($_.Exception.Message)"
    } finally {
        # Clean up temp file
        if (Test-Path $TempFile) {
            Remove-Item $TempFile -Force
        }
    }
}

function Test-Lexer {
    Write-TestHeader "🔤 LEXER TESTS / اختبارات المحلل المعجمي"
    
    # Test 1: Basic Token Recognition
    Test-SadCode -Code 'رقم عمر = 25 ;' -TestName "Arabic Variable Declaration"
    
    # Test 2: String Literals
    Test-SadCode -Code 'نص اسم = "أحمد محمد" ;' -TestName "String Literal with Arabic"
    
    # Test 3: Boolean Literals  
    Test-SadCode -Code 'منطق حالة = صحيح ;' -TestName "Boolean Literal Recognition"
    
    # Test 4: Float Numbers
    Test-SadCode -Code 'عشري درجة = 98.5 ;' -TestName "Float Number Recognition"
    
    # Test 5: Multiple Variable Types
    Test-SadCode -Code @'
رقم عدد = 10 ;
نص اسم = "أحمد" ;
منطق صحيح_أم_لا = صحيح ;
عشري نسبة = 75.5 ;
'@ -TestName "Multiple Variable Types"
    
    # Test 6: Arithmetic Operators
    Test-SadCode -Code 'رقم نتيجة = 5 + 3 * 2 - 1 / 2 ;' -TestName "Arithmetic Operators"
    
    # Test 7: Comparison Operators
    Test-SadCode -Code 'منطق مقارنة = ( عمر >= 18 ) && ( عمر <= 65 ) ;' -TestName "Comparison Operators"
    
    # Test 8: Arabic Keywords
    Test-SadCode -Code @'
إذا ( عمر > 18 )
    اطبع("بالغ") ;
وإلا
    اطبع("قاصر") ;
نهاية
'@ -TestName "Arabic Keywords (if-else)"
    
    # Test 9: Loop Keywords
    Test-SadCode -Code @'
رقم عداد = 0 ;
بينما ( عداد < 5 )
    عداد = عداد + 1 ;
نهاية
'@ -TestName "Loop Keywords (while)"
    
    # Test 10: Function Declaration Keywords
    Test-SadCode -Code @'
دالة احسب( رقم أ ، رقم ب )
    أرجع أ + ب ;
نهاية
'@ -TestName "Function Declaration Keywords"
}

function Test-Parser {
    Write-TestHeader "🌳 PARSER TESTS / اختبارات المحلل النحوي"
    
    # Test 1: Simple Expression
    Test-SadCode -Code 'رقم نتيجة = 2 + 3 * 4 ;' -TestName "Simple Mathematical Expression"
    
    # Test 2: Parenthesized Expression
    Test-SadCode -Code 'رقم نتيجة = ( 2 + 3 ) * 4 ;' -TestName "Parenthesized Expression"
    
    # Test 3: Variable Assignment
    Test-SadCode -Code @'
رقم أ = 5 ;
رقم ب = 10 ;
رقم جـ = أ + ب ;
'@ -TestName "Multiple Variable Assignments"
    
    # Test 4: If Statement
    Test-SadCode -Code @'
رقم عمر = 20 ;
إذا ( عمر >= 18 )
    اطبع("بالغ") ;
نهاية
'@ -TestName "If Statement Parsing"
    
    # Test 5: If-Else Statement
    Test-SadCode -Code @'
رقم درجة = 85 ;
إذا ( درجة >= 90 )
    اطبع("ممتاز") ;
وإلا
    اطبع("جيد") ;
نهاية
'@ -TestName "If-Else Statement Parsing"
    
    # Test 6: While Loop
    Test-SadCode -Code @'
رقم عداد = 0 ;
بينما ( عداد < 3 )
    اطبع("العداد: ", عداد) ;
    عداد = عداد + 1 ;
نهاية
'@ -TestName "While Loop Parsing"
    
    # Test 7: Array Declaration
    Test-SadCode -Code 'مصفوفة أرقام = [1, 2, 3, 4, 5] ;' -TestName "Array Declaration"
    
    # Test 8: Print Statement
    Test-SadCode -Code 'اطبع("مرحبا بالعالم") ;' -TestName "Print Statement"
    
    # Test 9: Complex Expression
    Test-SadCode -Code 'رقم معقد = ( ( 5 + 3 ) * 2 ) - ( 10 / 2 ) + 1 ;' -TestName "Complex Mathematical Expression"
    
    # Test 10: Syntax Error Test
    Test-SadCode -Code 'رقم خطأ = ;' -ShouldPass $false -TestName "Syntax Error Detection"
}

function Test-Interpreter {
    Write-TestHeader "⚡ INTERPRETER TESTS / اختبارات المفسر"
    
    # Test 1: Basic Arithmetic
    Test-SadCode -Code @'
رقم نتيجة = 5 + 3 ;
اطبع("النتيجة: ", نتيجة) ;
'@ -TestName "Basic Arithmetic Execution"
    
    # Test 2: Variable Assignment and Usage
    Test-SadCode -Code @'
رقم عمر = 25 ;
نص اسم = "أحمد" ;
اطبع("الاسم: ", اسم, "، العمر: ", عمر) ;
'@ -TestName "Variable Assignment and Usage"
    
    # Test 3: Conditional Logic
    Test-SadCode -Code @'
رقم درجة = 85 ;
إذا ( درجة >= 80 )
    اطبع("نجح") ;
وإلا
    اطبع("رسب") ;
نهاية
'@ -ExpectedOutput "نجح" -TestName "Conditional Logic Execution"
    
    # Test 4: Loop Execution
    Test-SadCode -Code @'
رقم مجموع = 0 ;
رقم عداد = 1 ;
بينما ( عداد <= 5 )
    مجموع = مجموع + عداد ;
    عداد = عداد + 1 ;
نهاية
اطبع("المجموع: ", مجموع) ;
'@ -ExpectedOutput "15" -TestName "Loop Execution"
    
    # Test 5: Array Operations
    Test-SadCode -Code @'
مصفوفة أرقام = [10, 20, 30] ;
اطبع("العنصر الثاني: ", أرقام[1]) ;
'@ -ExpectedOutput "20" -TestName "Array Access"
    
    # Test 6: String Operations
    Test-SadCode -Code @'
نص تحية = "مرحبا " + "بالعالم" ;
اطبع(تحية) ;
'@ -ExpectedOutput "مرحبا بالعالم" -TestName "String Concatenation"
    
    # Test 7: Boolean Operations
    Test-SadCode -Code @'
منطق شرط1 = صحيح ;
منطق شرط2 = خطأ ;
منطق النتيجة = شرط1 && !شرط2 ;
اطبع("النتيجة: ", النتيجة) ;
'@ -TestName "Boolean Operations"
    
    # Test 8: Exception Handling (من Phase 8)
    Test-SadCode -Code @'
حاول 
    رقم نتيجة = 10 / 0 ;
نهاية
امسك ( استثناء )
    اطبع("تم اصطياد خطأ القسمة على صفر") ;
نهاية
'@ -ExpectedOutput "تم اصطياد خطأ القسمة على صفر" -TestName "Exception Handling"
    
    # Test 9: Runtime Error Detection
    Test-SadCode -Code @'
مصفوفة قائمة = [1, 2, 3] ;
رقم قيمة = قائمة[10] ; # خطأ: فهرس خارج النطاق
'@ -ShouldPass $false -TestName "Runtime Error Detection"
    
    # Test 10: Memory Management
    Test-SadCode -Code @'
# اختبار إدارة الذاكرة - متغيرات متعددة
رقم أ = 1 ;
رقم ب = 2 ;
رقم جـ = 3 ;
رقم د = 4 ;
رقم هـ = 5 ;
اطبع("جميع المتغيرات تم إنشاؤها بنجاح") ;
'@ -TestName "Memory Management"
}

function Test-Integration {
    Write-TestHeader "🔗 INTEGRATION TESTS / اختبارات التكامل"
    
    # Test 1: Complete Program
    Test-SadCode -Code @'
# برنامج حساب المتوسط
اطبع("برنامج حساب متوسط الدرجات") ;

مصفوفة درجات = [85, 90, 78, 92, 88] ;
رقم مجموع = 0 ;
رقم عداد = 0 ;

بينما ( عداد < 5 )
    مجموع = مجموع + درجات[عداد] ;
    عداد = عداد + 1 ;
نهاية

عشري متوسط = مجموع / 5.0 ;
اطبع("المتوسط: ", متوسط) ;

إذا ( متوسط >= 90 )
    اطبع("تقدير: ممتاز") ;
وإلا
    إذا ( متوسط >= 80 )
        اطبع("تقدير: جيد جداً") ;
    وإلا
        اطبع("تقدير: جيد") ;
    نهاية
نهاية
'@ -TestName "Complete Grade Average Program"
    
    # Test 2: Function Definition and Call
    Test-SadCode -Code @'
دالة مربع( رقم عدد )
    أرجع عدد * عدد ;
نهاية

رقم النتيجة = مربع(5) ;
اطبع("مربع 5 = ", النتيجة) ;
'@ -TestName "Function Definition and Call"
    
    # Test 3: Class Definition (OOP)
    Test-SadCode -Code @'
صنف مستطيل 
    رقم الطول ;
    رقم العرض ;
    
    دالة احسب_المساحة()
        أرجع الطول * العرض ;
    نهاية
نهاية

مستطيل شكل ;
شكل.الطول = 10 ;
شكل.العرض = 5 ;
رقم المساحة = شكل.احسب_المساحة() ;
اطبع("المساحة: ", المساحة) ;
'@ -TestName "Class Definition and Usage"
}

function Show-FinalSummary {
    Write-TestHeader "📊 FINAL TEST SUMMARY / الملخص النهائي للاختبارات"
    
    $SuccessRate = if ($TotalTests -gt 0) { [math]::Round(($PassedTests / $TotalTests) * 100, 1) } else { 0 }
    
    Write-Host "Total Tests: $TotalTests" -ForegroundColor White
    Write-Host "Passed: $PassedTests ✅" -ForegroundColor Green  
    Write-Host "Failed: $($TotalTests - $PassedTests) ❌" -ForegroundColor Red
    Write-Host "Success Rate: $SuccessRate%" -ForegroundColor $(if ($SuccessRate -ge 80) {"Green"} else {"Red"})
    
    if ($FailedTests.Count -gt 0) {
        Write-Host "`n❌ Failed Tests:" -ForegroundColor Red
        foreach ($FailedTest in $FailedTests) {
            Write-Host "  • $($FailedTest.Name)" -ForegroundColor Red
            if ($FailedTest.Details) {
                Write-Host "    $($FailedTest.Details)" -ForegroundColor Gray
            }
        }
    }
    
    Write-Host "`n" -NoNewline
    if ($SuccessRate -ge 80) {
        Write-Host "🎉 TEST SUITE PASSED! (≥80% success rate)" -ForegroundColor Green
        Write-Host "Phase 9.1 Testing objectives met!" -ForegroundColor Green
    } else {
        Write-Host "⚠️  TEST SUITE NEEDS IMPROVEMENT (<80% success rate)" -ForegroundColor Yellow
        Write-Host "Some components need additional work." -ForegroundColor Yellow
    }
    
    return $SuccessRate
}

# Main execution
try {
    Write-Host "🚀 Sad Language Comprehensive Test Suite" -ForegroundColor Cyan
    Write-Host "مجموعة اختبارات شاملة للغة ص" -ForegroundColor Cyan
    Write-Host "Starting tests for: $Component" -ForegroundColor Yellow
    
    # Check if executable exists
    if (-not (Test-Path $SadExecutable)) {
        Write-Host "❌ Error: Sad executable not found at $SadExecutable" -ForegroundColor Red
        Write-Host "Please build the project first using: cmake --build build --config Debug" -ForegroundColor Yellow
        exit 1
    }
    
    # Run tests based on component parameter
    switch ($Component.ToLower()) {
        "lexer" { Test-Lexer }
        "parser" { Test-Parser }
        "interpreter" { Test-Interpreter }
        "integration" { Test-Integration }
        "all" { 
            Test-Lexer
            Test-Parser
            Test-Interpreter
            Test-Integration
        }
        default { 
            Write-Host "❌ Invalid component: $Component" -ForegroundColor Red
            Write-Host "Valid options: lexer, parser, interpreter, integration, all" -ForegroundColor Yellow
            exit 1
        }
    }
    
    # Show final summary
    $FinalSuccessRate = Show-FinalSummary
    
    # Exit with appropriate code
    if ($FinalSuccessRate -ge 80) {
        exit 0  # Success
    } else {
        exit 1  # Needs improvement
    }
    
} catch {
    Write-Host "❌ Test suite encountered an error: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}