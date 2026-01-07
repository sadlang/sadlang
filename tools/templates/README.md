# بسم الله الرحمن الرحيم
# Sad Language Project Templates
# قوالب مشاريع لغة ص

This directory contains project templates for Sad Language.

## Available Templates / القوالب المتاحة

### 1. CLI Application (`cli`)
Command-line application template with argument parsing.

**Use case:** Tools, utilities, command-line apps  
**Dependencies:** cli_parser (optional), colors (optional)

### 2. Web Application (`web`)
Web application template with HTTP server and routing.

**Use case:** Web backends, HTTP services  
**Dependencies:** http_server, router, json, logger

### 3. Library (`library`)
Reusable library template with tests and documentation.

**Use case:** Shared code, packages for distribution  
**Dependencies:** None (pure library)

### 4. REST API (`rest-api`)
RESTful API template with database integration.

**Use case:** APIs, microservices, backends  
**Dependencies:** http_server, router, database, json, validator, logger

### 5. Game (`game`)
2D game template using Raylib.

**Use case:** 2D games, interactive applications  
**Dependencies:** raylib, game_engine

### 6. Desktop Application (`desktop`)
Desktop GUI application template.

**Use case:** Desktop apps, editors, tools  
**Dependencies:** gui, file_system

### 7. Blank (`blank`)
Minimal blank project template.

**Use case:** Custom projects, starting from scratch  
**Dependencies:** None

## Usage / الاستخدام

### List Templates

```bash
sad-template list
```

### Create New Project

```bash
# Basic usage
sad-template new <template> <project_name>

# With author info
sad-template new cli my_cli --author "Ahmed Ali" --email "ahmed@example.com"

# With custom output directory
sad-template new web my_web_app --output ~/projects
```

### Examples

```bash
# Create CLI application
sad-template new cli my_tool --author "John Doe"

# Create web application
sad-template new web my_web_app --email john@example.com

# Create REST API
sad-template new rest-api my_api --author "Developer"

# Create library
sad-template new library string_utils --author "Utils Team"

# Create game
sad-template new game space_shooter --author "Game Dev"

# Create desktop app
sad-template new desktop text_editor --author "Desktop Dev"

# Create blank project
sad-template new blank my_project
```

## Template Structure / بنية القالب

Each template contains:

```
template_name/
├── sad.toml           # Package configuration
├── src/               # Source code
│   └── main.s (or lib.s)
├── tests/             # Tests (optional)
│   └── test_*.s
├── README.md          # Documentation
└── .gitignore         # Git ignore file
```

## Template Variables / متغيرات القالب

Templates support the following variables:

- `{{PROJECT_NAME}}` - Project name
- `{{AUTHOR_NAME}}` - Author name
- `{{AUTHOR_EMAIL}}` - Author email
- `{{DATE}}` - Current date
- `{{YEAR}}` - Current year

These variables are automatically replaced when generating a project.

## Integration with sad CLI / التكامل مع sad CLI

Templates are integrated with the main `sad` CLI:

```bash
# Create new project
sad new <project_name> --template <template_name>

# Examples
sad new my_cli --template cli
sad new my_api --template rest-api
```

## Adding Custom Templates / إضافة قوالب مخصصة

To add a custom template:

1. Create a new directory in `tools/templates/`
2. Add required files (sad.toml, src/main.s, README.md, etc.)
3. Use template variables where needed
4. Test with `sad-template new`

Example custom template structure:

```
tools/templates/my_custom_template/
├── sad.toml
├── src/
│   └── main.s
└── README.md
```

## Building the Generator / بناء المولد

```bash
# Compile generator
g++ -std=c++17 generator.cpp -o sad-template

# Or with CMake
cmake --build . --target sad-template
```

## License

MIT License

---

**الحمد لله رب العالمين** 🤲

For more information, visit: https://sad-lang.org/docs/templates
