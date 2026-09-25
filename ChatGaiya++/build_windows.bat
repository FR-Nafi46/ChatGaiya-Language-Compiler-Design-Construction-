@echo off
setlocal
cd /d "%~dp0"
where g++ >nul 2>nul
if errorlevel 1 (
  echo ERROR: g++ was not found. Install MinGW-w64 or MSYS2 UCRT64 and add it to PATH.
  exit /b 1
)
set FLEX_CMD=
where flex >nul 2>nul
if not errorlevel 1 set FLEX_CMD=flex
if "%FLEX_CMD%"=="" (
  where win_flex >nul 2>nul
  if not errorlevel 1 set FLEX_CMD=win_flex
)
if "%FLEX_CMD%"=="" (
  echo ERROR: Flex was not found. Install Flex or WinFlexBison and add it to PATH.
  exit /b 1
)
if not exist .build mkdir .build
%FLEX_CMD% -o .build\lexer.generated.cpp lexer.l
if errorlevel 1 exit /b 1
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -I. -o chatgaiya.exe main.cpp parser.cpp compiler.cpp error_reporting.cpp symbol_table.cpp semantic_analysis.cpp code_generation.cpp .build\lexer.generated.cpp
if errorlevel 1 exit /b 1
echo Build successful: chatgaiya.exe
