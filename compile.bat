@echo off
echo Compiling Stock Market Simulator...
g++ -std=c++17 -O2 -Wall -o StockMarket.exe main.cpp
if %errorlevel% == 0 (
    echo.
    echo [SUCCESS] Compiled successfully! Run with: StockMarket.exe
) else (
    echo.
    echo [ERROR] Compilation failed. Make sure g++ is installed.
    echo         Install MinGW or MSYS2 for Windows.
)
pause
