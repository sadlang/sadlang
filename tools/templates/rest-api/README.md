# {{PROJECT_NAME}}

A REST API built with Sad Language.

## Features

- ✅ RESTful endpoints (CRUD)
- ✅ Database integration
- ✅ Input validation
- ✅ Error handling
- ✅ Logging
- ✅ Environment configuration

## Installation

```bash
# Clone the repository
git clone https://github.com/{{AUTHOR_NAME}}/{{PROJECT_NAME}}.git
cd {{PROJECT_NAME}}

# Install dependencies
sad-pkg install

# Setup environment
cp .env.example .env
# Edit .env with your database credentials

# Run migrations
sad run migrate

# Build
sad build
```

## Usage

### Development

```bash
sad run dev
```

### Production

```bash
sad build --release
./build/{{PROJECT_NAME}} --env production
```

## API Endpoints

### Items

- `GET /api/items` - List all items
- `GET /api/items/:id` - Get item by ID
- `POST /api/items` - Create new item
- `PUT /api/items/:id` - Update item
- `DELETE /api/items/:id` - Delete item

### Health

- `GET /health` - Health check

## Example Requests

### Create Item

```bash
curl -X POST http://localhost:3000/api/items \
  -H "Content-Type: application/json" \
  -d '{"name":"Example","description":"Example item"}'
```

### Get All Items

```bash
curl http://localhost:3000/api/items
```

### Get Item by ID

```bash
curl http://localhost:3000/api/items/1
```

### Update Item

```bash
curl -X PUT http://localhost:3000/api/items/1 \
  -H "Content-Type: application/json" \
  -d '{"name":"Updated Name"}'
```

### Delete Item

```bash
curl -X DELETE http://localhost:3000/api/items/1
```

## License

MIT License

Copyright (c) {{YEAR}} {{AUTHOR_NAME}}
