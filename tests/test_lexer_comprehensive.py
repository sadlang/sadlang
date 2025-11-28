# Test Unit للـ Lexer - Comprehensive Tests
# اختبارات شاملة للمحلل المعجمي

import subprocess
import sys
import os

def run_sad_lexer_test(code, expected_tokens=None):
    """Test the Sad language lexer with given code"""
    
    # Write test file
    test_file = "temp_lexer_test.s"
    with open(test_file, 'w', encoding='utf-8') as f:
        f.write(code)
    
    try:
        # Run the interpreter to check lexer
        result = subprocess.run([
            "./build/bin/Debug/sad.exe", 
            test_file
        ], 
        capture_output=True, 
        text=True, 
        encoding='utf-8'
        )
        
        return {
            'exit_code': result.returncode,
            'stdout': result.stdout,
            'stderr': result.stderr
        }
    finally:
        # Clean up
        if os.path.exists(test_file):
            os.remove(test_file)

def test_basic_tokens():
    """Test basic token recognition"""
    print("🔤 Testing Basic Token Recognition...")
    
    tests = [
        {
            'name': 'Arabic Variable Declaration',
            'code': 'رقم عمر = 25 ;',
            'should_pass': True
        },
        {
            'name': 'String Literal',
            'code': 'نص اسم = "أحمد محمد" ;',
            'should_pass': True
        },
        {
            'name': 'Boolean Literals',
            'code': 'منطق صحيح_أم_لا = صحيح ;',
            'should_pass': True
        },
        {
            'name': 'Float Numbers',
            'code': 'عشري درجة = 98.5 ;',
            'should_pass': True
        },
    ]
    
    passed = 0
    total = len(tests)
    
    for test in tests:
        result = run_sad_lexer_test(test['code'])
        
        # Check if lexer succeeded (parser may fail, that's ok)
        success = True
        if "[1/4] التحليل المعجمي" in result['stdout']:
            if "Lexer created" in result['stdout']:
                success = True
            else:
                success = False
        else:
            success = False
            
        if success == test['should_pass']:
            print(f"  ✅ {test['name']}")
            passed += 1
        else:
            print(f"  ❌ {test['name']}")
            print(f"     Expected: {test['should_pass']}, Got: {success}")
    
    print(f"📊 Basic Tokens: {passed}/{total} passed ({100*passed/total:.1f}%)\n")
    return passed, total

def test_arabic_keywords():
    """Test Arabic keyword recognition"""
    print("🕌 Testing Arabic Keywords...")
    
    keywords_tests = [
        'إذا ( صحيح ) اطبع("test") ; نهاية',
        'لكل ( عنصر في قائمة ) اطبع(عنصر) ; نهاية', 
        'بينما ( عداد < 10 ) عداد = عداد + 1 ; نهاية',
        'حاول رقم ع = 5 ; نهاية امسك ( استثناء ) اطبع("خطأ") ; نهاية',
        'دالة احسب() أرجع 5 ; نهاية',
        'صنف شخص رقم العمر ; نهاية'
    ]
    
    passed = 0
    total = len(keywords_tests)
    
    for i, code in enumerate(keywords_tests):
        result = run_sad_lexer_test(code)
        
        # Check if lexer recognized keywords
        success = "Lexer created" in result['stdout']
        
        if success:
            print(f"  ✅ Keywords Test {i+1}")
            passed += 1
        else:
            print(f"  ❌ Keywords Test {i+1}")
    
    print(f"📊 Arabic Keywords: {passed}/{total} passed ({100*passed/total:.1f}%)\n")
    return passed, total

def test_operators_punctuation():
    """Test operators and punctuation"""
    print("⚡ Testing Operators & Punctuation...")
    
    operator_tests = [
        'رقم ع = 1 + 2 - 3 * 4 / 5 % 6 ;',
        'منطق نتيجة = ( أ == ب ) && ( ج != د ) ;',
        'منطق مقارنة = ( x <= y ) || ( x >= z ) ;',
        'رقم زيادة = ++عداد ;',
        'رقم نقصان = --عداد ;',
        'مصفوفة أرقام = [1, 2, 3, 4, 5] ;'
    ]
    
    passed = 0
    total = len(operator_tests)
    
    for i, code in enumerate(operator_tests):
        result = run_sad_lexer_test(code)
        
        # Check if lexer processed operators
        success = "Lexer created" in result['stdout'] and "ERROR" not in result['stdout']
        
        if success:
            print(f"  ✅ Operators Test {i+1}")
            passed += 1
        else:
            print(f"  ❌ Operators Test {i+1}")
    
    print(f"📊 Operators: {passed}/{total} passed ({100*passed/total:.1f}%)\n")
    return passed, total

def test_complex_expressions():
    """Test complex expressions"""
    print("🧮 Testing Complex Expressions...")
    
    complex_tests = [
        '''
        رقم نتيجة = ( 5 + 3 ) * ( 10 - 2 ) / 4 ;
        ''',
        '''
        نص رسالة = "مرحبا " + اسم + "، عمرك " + عمر + " سنة" ;
        ''',
        '''
        منطق شرط = ( عمر >= 18 ) && ( العمر < 65 ) && صالح ;
        ''',
        '''
        مصفوفة قائمة = [1, 2.5, "نص", صحيح, [1, 2, 3]] ;
        '''
    ]
    
    passed = 0
    total = len(complex_tests)
    
    for i, code in enumerate(complex_tests):
        result = run_sad_lexer_test(code.strip())
        
        success = "Lexer created" in result['stdout']
        
        if success:
            print(f"  ✅ Complex Expression {i+1}")
            passed += 1
        else:
            print(f"  ❌ Complex Expression {i+1}")
    
    print(f"📊 Complex Expressions: {passed}/{total} passed ({100*passed/total:.1f}%)\n")
    return passed, total

def test_error_handling():
    """Test lexer error handling"""
    print("🚨 Testing Error Handling...")
    
    error_tests = [
        {
            'name': 'Invalid Characters',
            'code': 'رقم ع = @ # $ % ^ ;',
            'should_fail': True
        },
        {
            'name': 'Unterminated String',
            'code': 'نص اسم = "غير مكتمل',
            'should_fail': True
        },
        {
            'name': 'Invalid Number Format', 
            'code': 'عشري رقم = 12.34.56 ;',
            'should_fail': True
        }
    ]
    
    passed = 0
    total = len(error_tests)
    
    for test in error_tests:
        result = run_sad_lexer_test(test['code'])
        
        # For error tests, we expect them to fail gracefully
        failed = result['exit_code'] != 0 or "error" in result['stdout'].lower()
        
        if failed == test['should_fail']:
            print(f"  ✅ {test['name']}")
            passed += 1
        else:
            print(f"  ❌ {test['name']}")
    
    print(f"📊 Error Handling: {passed}/{total} passed ({100*passed/total:.1f}%)\n")
    return passed, total

def main():
    """Main test runner"""
    print("🚀 Comprehensive Lexer Test Suite")
    print("مجموعة اختبارات شاملة للمحلل المعجمي")
    print("=" * 50)
    
    total_passed = 0
    total_tests = 0
    
    # Run all test categories
    p, t = test_basic_tokens()
    total_passed += p
    total_tests += t
    
    p, t = test_arabic_keywords()
    total_passed += p
    total_tests += t
    
    p, t = test_operators_punctuation()
    total_passed += p
    total_tests += t
    
    p, t = test_complex_expressions()
    total_passed += p
    total_tests += t
    
    p, t = test_error_handling()
    total_passed += p
    total_tests += t
    
    # Final summary
    success_rate = 100 * total_passed / total_tests
    print("=" * 50)
    print("📊 LEXER TEST SUMMARY")
    print("=" * 50)
    print(f"Total Tests: {total_tests}")
    print(f"Passed: {total_passed} ✅")
    print(f"Failed: {total_tests - total_passed} ❌")
    print(f"Success Rate: {success_rate:.1f}%")
    
    if success_rate >= 80:
        print("🎉 LEXER TESTS PASSED! (≥80% success rate)")
        return 0
    else:
        print("❌ LEXER TESTS NEED IMPROVEMENT (<80% success rate)")
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)