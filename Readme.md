# MiniDB Server

The backend API for MiniDB, connecting the web client to the custom C++ database engine.

## Overview

The server is built with NestJS and exposes REST APIs for executing SQL queries and retrieving available tables.

The backend invokes the compiled MiniDB C++ engine to process SQL commands.

## Architecture

```text
Client
   │
   │ HTTP
   ▼
NestJS API
   │
   │ executes
   ▼
MiniDB C++ Engine
   │
   ▼
data/*.tbl
```

## Tech Stack

- Node.js
- NestJS
- TypeScript
- C++
- CMake

## Project Structure

```text
Mini-DB-Server/
├── server/
│   ├── src/
│   │   ├── database/
│   │   ├── app.module.ts
│   │   └── main.ts
│   ├── package.json
│   └── ...
│
├── src/
│   └── C++ MiniDB engine
│
├── data/
│   └── database table files
│
└── CMakeLists.txt
```

## API

### Execute Query

```http
POST /api/query
```

Request:

```json
{
  "query": "SELECT * FROM users;"
}
```

### Get Tables

```http
GET /api/tables
```

Example response:

```json
{
  "tables": ["users"]
}
```

## Running Locally

### Build the C++ engine

Create a build directory and configure CMake:

```bash
cmake -S . -B build
```

Build the engine:

```bash
cmake --build build
```

### Start the NestJS server

Go to the server directory:

```bash
cd server
```

Install dependencies:

```bash
npm install
```

Start the development server:

```bash
npm run start:dev
```

The API will run on:

```text
http://localhost:3000
```

## CORS

The server allows the frontend origin through the `CLIENT_URL` environment variable.

Example:

```env
CLIENT_URL=http://localhost:5173
```

For production, set this to the deployed frontend URL.

## Deployment

The backend requires a Linux-compatible C++ build environment because the NestJS server executes the MiniDB engine.

The recommended deployment setup is:

```text
Frontend → Vercel
Backend  → Render
```

The C++ engine is built as part of the backend deployment.

## Supported SQL

MiniDB currently supports operations including:

- `CREATE TABLE`
- `INSERT`
- `SELECT`
- `UPDATE`
- `DELETE`
- Equality-based `WHERE` filters# Mini-DB-server
