# Chatgaiya++ Compiler

A small educational compiler project for a custom programming language inspired by the **Chatgaiya language style** and implemented in **C++**.

Chatgaiya++ takes source files written in `.cg` and compiles them into **Python 3 source code**. The compiler uses **Flex** for lexical analysis, a **hand-written recursive-descent parser** for syntax analysis, semantic analysis with a symbol table, and a Python code generator.

---

## ✨ Highlights

- 🧩 Custom Chatgaiya++ syntax and keywords
- 🔤 Lexical analysis with **Flex**
- 🌳 Hand-written **recursive-descent parsing**
- 🧠 AST-based compiler representation
- 🔎 Semantic analysis and type checking
- 📚 Symbol table management
- 🐍 Python 3 source-code generation
- ⚠️ Line/column-aware compiler diagnostics
- 🧪 Automated smoke tests
- 🪟 Windows build support with MinGW-w64 / WinFlexBison
- 🐧 Linux / macOS build support with Flex + Make

---

## 🏗️ Compiler Pipeline

```text
             Chatgaiya++ Source (.cg)
                       │
                       ▼
              ┌─────────────────┐
              │   Lexical       │
              │   Analysis      │
              │     (Flex)      │
              └────────┬────────┘
                       │ Tokens
                       ▼
              ┌─────────────────┐
              │    Parsing      │
              │ Recursive       │
              │ Descent Parser  │
              └────────┬────────┘
                       │ AST
                       ▼
              ┌─────────────────┐
              │    Semantic     │
              │    Analysis     │
              │ + Symbol Table  │
              └────────┬────────┘
                       │ Valid AST
                       ▼
              ┌─────────────────┐
              │ Code Generation │
              │   Python 3      │
              └────────┬────────┘
                       │
                       ▼
                Generated .py
```

---

## 👥 Team & Topic Distribution

| Member | Student ID | Topic / Responsibility | Main Files |
|---|---|---|---|
| **Halima Akter Nila** | `0182320012101386` | **Lexical Analysis & Language Design** | `lexer.l`, `tokens.hpp` |
| **Mansurul Islam Abrar** | `0182320012101401` | **Parsing + AST** | `parser.cpp`, `parser.hpp` |
| **Md. Fahmidur Rahman Nafi** | `0182320012101388` | **Semantic Analysis + Code Generation** | `semantic_analysis.cpp`, `semantic_analysis.hpp`, `symbol_table.cpp`, `symbol_table.hpp`, `code_generation.cpp`, `code_generation.hpp` |

### Shared Files

These files can be maintained collaboratively by the team or handled on a rotating basis:

- `compiler.cpp`, `compiler.hpp` — AST node definitions and language type utilities
- `error_reporting.cpp`, `error_reporting.hpp` — compiler diagnostics
- `main.cpp` — compiler driver / command-line interface
- `Makefile` — Unix-like build and test commands
- `build_windows.bat` — Windows build script
- `.gitignore` — repository ignore rules
- `README.md` — project documentation

---

## 📁 Project Structure

```text
ChatgaiyaPlusPlus/
├── lexer.l
├── tokens.hpp
├── parser.cpp
├── parser.hpp
├── compiler.cpp
├── compiler.hpp
├── semantic_analysis.cpp
├── semantic_analysis.hpp
├── symbol_table.cpp
├── symbol_table.hpp
├── code_generation.cpp
├── code_generation.hpp
├── error_reporting.cpp
├── error_reporting.hpp
├── main.cpp
│
├── examples/
│   └── program.cg
│
├── tests/
│   ├── check.cg
│   ├── check.py
│   ├── control_flow.cg
│   ├── precedence.cg
│   ├── run.cg
│   ├── run.py
│   ├── run_tests.py
│   ├── typed_input.cg
│   └── type_error.cg
│
├── Makefile
├── build_windows.bat
├── .gitignore
└── README.md
```

Generated build files are placed in `.build/`.

---

## 🧰 Requirements

### Windows

Install:

- **MinGW-w64** / a C++17 compiler
- **WinFlexBison** (or Flex)
- **Python 3**

Make sure `g++` and `win_flex`/`flex` are available in `PATH`.

### Linux / macOS

Install:

- A **C++17** compiler
- **Flex**
- **Make**
- **Python 3**

> **Bison is not required.** The parser is implemented manually using recursive descent.

---

## 🔨 Build

### Windows

From the project directory:

```bat
build_windows.bat
```

This generates:

```text
chatgaiya.exe
```

### Linux / macOS

```bash
make
```

The generated compiler executable is:

```text
chatgaiya
```

---

## ▶️ Compile a Chatgaiya++ Program

Compile the example program:

### Windows

```bat
chatgaiya.exe examples\program.cg
python examples\program.py
```

### Linux / macOS

```bash
./chatgaiya examples/program.cg
python3 examples/program.py
```

By default, the compiler writes the generated Python file beside the input file.

### Choose an output file

```bash
./chatgaiya examples/program.cg -o output.py
python3 output.py
```

On Windows:

```bat
chatgaiya.exe examples\program.cg -o output.py
python output.py
```

---

## 🧪 Testing

The project includes smoke tests for expression precedence, control flow, typed input, and semantic/type errors.

### Windows

```bat
python tests\run_tests.py
```

### Linux / macOS

```bash
make test
```

A successful run prints:

```text
All compiler smoke tests passed.
```

---

## 📝 Chatgaiya++ Language Overview

### Data Types

| Chatgaiya++ Type | Meaning |
|---|---|
| `ongko` | Integer |
| `dhoshomik` | Floating-point number |
| `kotha` | String |
| `ho` | Boolean |

Boolean literals:

```text
hasa
misa
```

---

### Variable Declaration

```text
ongko age = 21;
dhoshomik cgpa = 3.75;
kotha name = "Nafi";
ho passed = hasa;
```

Declarations may also be written without an initializer:

```text
ongko count;
kotha message;
```

---

### Output

Use `ko(...)`:

```text
ko("Hello Chatgaiya++");
ko(name);
ko("Name: " + name);
```

---

### Input

Use `lo()`:

```text
ongko age = lo();
dhoshomik cgpa = lo();
kotha name = lo();
ho passed = lo();
```

The generated Python code converts typed input appropriately.

---

### Conditions

```text
zodi (age >= 18) {
    ko("Adult");
} tokon {
    ko("Minor");
}
```

`noile` is also recognized as an `else` keyword.

---

### Loops

```text
zotokkhon (count < 10) {
    ko(count);
    count++;
}
```

Loop control:

```text
tham;
chol;
```

- `tham` → `break`
- `chol` → `continue`

---

### Increment / Decrement

```text
count++;
count--;
```

---

### Expressions

The parser supports:

- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
- Logical: `&&`, `||`, `!`
- Bitwise: `&`, `|`, `^`
- Shift: `<<`, `>>`
- Unary `+` and `-`
- Parenthesized expressions
- String indexing

Example:

```text
ongko result = (10 + 4) * 2;
ho valid = (result >= 20) && (result != 30);
```

---

### String Indexing

```text
kotha word = "Chatgaiya";
ko(word[0]);
ko(word[1]);
```

The generated Python uses a small helper for safe indexing.

---

### Type Inspection

`ki_type(expr)` produces the type name of an expression:

```text
kotha name = "Nila";
ko(ki_type(name));
```

The compiler represents the result using the Chatgaiya++ type names.

---

## 🧠 Semantic Analysis

Before generating Python, the compiler performs semantic checks including:

- Use of undeclared variables
- Duplicate declarations
- Assignment type compatibility
- Invalid numeric operations
- Invalid boolean operations
- Invalid relational comparisons
- Invalid string indexing
- `tham` / `chol` outside loops
- Variable names that conflict with generated Python syntax
- Other type-related errors

When compilation fails, the compiler reports the source location and does **not** generate the output file.

Example diagnostic style:

```text
program.cg:4:12: error: use of undeclared variable 'age'
```

---

## 🧩 Main Components

### `lexer.l`
Defines the Flex rules that convert Chatgaiya++ source code into tokens while tracking line and column information.

### `tokens.hpp`
Contains token kinds shared by the lexer and parser.

### `parser.cpp` / `parser.hpp`
Implements the recursive-descent parser, including operator precedence, statements, expressions, conditionals, loops, and blocks.

### `compiler.cpp` / `compiler.hpp`
Defines the compiler's core AST structures:

- `Expr`
- `Stmt`
- `ValueType`
- `ExprKind`
- `StmtKind`

### `semantic_analysis.cpp` / `semantic_analysis.hpp`
Performs type checking and other semantic validation before code generation.

### `symbol_table.cpp` / `symbol_table.hpp`
Stores declared variables and their types.

### `code_generation.cpp` / `code_generation.hpp`
Traverses the validated AST and emits Python 3 code.

### `error_reporting.cpp` / `error_reporting.hpp`
Collects and prints compiler diagnostics with line and column information.

### `main.cpp`
Coordinates the complete compilation process:

```text
Read source
   ↓
Lex + Parse
   ↓
Semantic Analysis
   ↓
Generate Python
   ↓
Write output.py
```

---

## 🔍 Example Program

```text
shuru_kor;

kotha name = "Nafi";
kotha language = "Chatgaiya++";

ko("Name: " + name);
ko("Language: " + language);

ongko length = 10;
ko("Length: " + length);

kotha word = "Chatgaiya";
ko("First character: " + word[0]);

zodi (name == "Nafi") {
    ko("Name matched successfully.");
} tokon {
    ko("Name did not match.");
}
```

This program is compiled into Python 3 source by the compiler.

---

## 📚 Learning Goals

This project demonstrates the main stages of a small compiler:

1. **Lexical Analysis**
2. **Syntax Analysis / Parsing**
3. **Abstract Syntax Tree (AST) Construction**
4. **Semantic Analysis**
5. **Symbol Table Management**
6. **Intermediate Compiler Representation**
7. **Target Code Generation**
8. **Error Reporting**
9. **Testing**

The project is intended as a practical demonstration of how a source program moves through a compiler pipeline and becomes executable target-language code.

---

## 🚀 Quick Start

```bash
# Clone the repository
git clone <your-repository-url>

# Enter the project
cd ChatgaiyaPlusPlus

# Build
make

# Compile example
./chatgaiya examples/program.cg

# Run generated Python
python3 examples/program.py

# Run tests
make test
```

For Windows, use `build_windows.bat` and the corresponding Windows commands shown above.

---

## 👨‍💻 Contributors

### Halima Akter Nila
**Student ID:** `0182320012101386`  
**Focus:** Lexical Analysis & Language Design

### Mansurul Islam Abrar
**Student ID:** `0182320012101401`  
**Focus:** Parsing + AST

### Md. Fahmidur Rahman Nafi
**Student ID:** `0182320012101388`  
**Focus:** Semantic Analysis + Code Generation

---

## 📌 Project Status

This repository contains the current implementation of the Chatgaiya++ compiler, including the lexer, recursive-descent parser, semantic analysis, symbol table, Python code generation, build scripts, example programs, and automated tests.

---

## 📄 License

Add your preferred license here before publishing the repository publicly, for example **MIT**, **Apache-2.0**, or your university/project-specific license.

---

<p align="center">
  <b>Chatgaiya++ Compiler</b><br>
  Built as a collaborative compiler-design project using C++, Flex, and Python 3.
</p>
