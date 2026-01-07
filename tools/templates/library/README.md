# {{PROJECT_NAME}}

A reusable library built with Sad Language.

## Installation

```bash
# Add to your project
sad-pkg add {{PROJECT_NAME}} ^0.1.0
```

## Usage

```sad
استورد "{{PROJECT_NAME}}" كـ مكتبة;

// Use functions
نتيجة = مكتبة.جمع(5, 3);
اطبع(نتيجة);  // 8

// Use classes
آلة = مكتبة.آلة_حاسبة.جديد();
آلة.جمع(10).طرح(3);
اطبع(آلة.احصل_النتيجة());  // 7
```

## API Documentation

### Functions

- `جمع(أ: عدد، ب: عدد) -> عدد` - Add two numbers
- `طرح(أ: عدد، ب: عدد) -> عدد` - Subtract two numbers
- `ضرب(أ: عدد، ب: عدد) -> عدد` - Multiply two numbers
- `قسمة(أ: عدد، ب: عدد) -> عدد` - Divide two numbers

### Classes

- `آلة_حاسبة` - Calculator with chainable operations

## Development

```bash
# Run tests
sad test

# Run benchmarks
sad bench

# Generate documentation
sad doc
```

## License

MIT License

Copyright (c) {{YEAR}} {{AUTHOR_NAME}}
