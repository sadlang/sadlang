# Test LLVM Pipeline
# Simple test program to verify LLVM compilation works

رئيسي()
{
    # Test 1: Basic arithmetic
    س = 10 + 20
    اطبع("Test 1: Basic arithmetic")
    اطبع("10 + 20 = ")
    اطبع(س)
    اطبع("\n")
    
    # Test 2: Function call
    نتيجة = فيبوناتشي(10)
    اطبع("Test 2: Function call")
    اطبع("fibonacci(10) = ")
    اطبع(نتيجة)
    اطبع("\n")
    
    # Test 3: Loop
    اطبع("Test 3: Loop (1 to 5)\n")
    ي = 1
    بينما ي <= 5
    {
        اطبع(ي)
        اطبع(" ")
        ي = ي + 1
    }
    اطبع("\n")
    
    # Test 4: Conditional
    اطبع("Test 4: Conditional\n")
    اذا س > 25
    {
        اطبع("10 + 20 is greater than 25\n")
    }
    والا
    {
        اطبع("10 + 20 is NOT greater than 25\n")
    }
    
    ارجع 0
}

# Fibonacci function for testing recursion
فيبوناتشي(ن)
{
    اذا ن <= 1
    {
        ارجع ن
    }
    ارجع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
}
