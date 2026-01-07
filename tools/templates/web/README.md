# {{PROJECT_NAME}}

A web application built with Sad Language.

## Features

- ✅ HTTP Server
- ✅ Routing
- ✅ JSON API
- ✅ Logging
- ✅ Hot reload (development)

## Installation

```bash
# Clone the repository
git clone https://github.com/{{AUTHOR_NAME}}/{{PROJECT_NAME}}.git
cd {{PROJECT_NAME}}

# Install dependencies
sad-pkg install

# Build
sad build
```

## Usage

### Development

```bash
# Run with hot reload on port 3000
sad run dev
```

### Production

```bash
# Build optimized version
sad build --release

# Run on port 80
./build/{{PROJECT_NAME}} --port 80
```

## API Endpoints

- `GET /` - Home page
- `GET /api/data` - JSON API endpoint
- `GET /health` - Health check

## License

MIT License

Copyright (c) {{YEAR}} {{AUTHOR_NAME}}
