@echo off
echo TSP Solver - Run Tests
echo ====================

REM Build and run tests
call build_test.bat

if %errorlevel% neq 0 (
    echo Error: Failed to build tests.
    exit /b 1
)

REM Run the tests
echo.
echo Running tests...
echo.

cd build
.\Debug\TSPSolverTest.exe

if %errorlevel% equ 0 (
    echo.
    echo All tests passed!
) else (
    echo.
    echo Tests failed!
    exit /b 1
)

pause