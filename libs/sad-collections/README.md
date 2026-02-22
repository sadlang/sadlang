# sad-collections — مكتبة هياكل البيانات
# Data Structures Library for لغة ص

## الوصف / Description
مكتبة هياكل بيانات قوية تحتوي على 5 أصناف أساسية: المكدس، الطابور، العداد، النطاق، والقائمة مع خوارزميات الترتيب والبحث.

A robust data structures library with 5 core classes: Stack, Queue, Accumulator, Range, and Array with sorting/searching algorithms.

## الأصناف المتوفرة / Available Classes

### صنف مكدس (Stack — LIFO)
```
مكدس م = مكدس()
م.ادفع(10)        # Push
م.اسحب()          # Pop → returns value
م.القمة()          # Peek
م.فارغ()           # Is empty?
م.الطول()          # Size
م.اعرض()          # Display
```

### صنف طابور (Queue — FIFO)
```
طابور ط = طابور()
ط.أدخل(100)       # Enqueue
ط.أخرج()          # Dequeue → returns value
ط.الأول()          # Front
ط.فارغ()           # Is empty?
ط.الطول()          # Size
```

### صنف عداد (Accumulator)
```
عداد ع = عداد(0)
ع.أضف(50)         # Add
ع.اطرح(10)        # Subtract
ع.اضرب(3)         # Multiply
ع.الحالي()         # Current value
ع.كم_عملية()       # Operation count
```

### صنف نطاق (Range)
```
نطاق ر = نطاق(1، 100، 2)  # start, end, step
ر.يحتوي(50)       # Contains?
ر.الطول()          # Length
ر.المجموع()        # Sum
```

### صنف قائمة_أعداد (Array with Sort/Search)
```
قائمة_أعداد ق = قائمة_أعداد()
ق.أضف(64)         # Add
ق.اجلب(0)         # Get at index
ق.ضع(0، 99)      # Set at index
ق.ابحث(64)        # Linear search → index
ق.يحتوي(64)       # Contains?
ق.رتب_تصاعدي()    # Bubble sort ascending
ق.رتب_تنازلي()    # Bubble sort descending
ق.اعكس()          # Reverse
ق.الأقصى()        # Max
ق.الأدنى()        # Min
ق.المجموع()        # Sum
ق.المتوسط()        # Average
```

## التثبيت / Installation
```
sad-pkg add sad-collections ^1.0.0
```

## الترخيص / License
MIT
