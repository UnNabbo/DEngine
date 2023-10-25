@echo off 

if ["%~1"]==["clean"] (
    rmdir /s /q build
    rmdir /s /q bin
)

if not exist bin\ (
    mkdir bin
)


if not exist build\ (
    mkdir build
)

cd build
if ["%~1"]==["clean"] (
    cmake -G "Ninja" ..
)
cmake -E time cmake --build .

if errorlevel 1 (
    cd ..
    echo:
    echo FAILED COMPILATION
    echo:
) else (
    cd ..
    echo:
    echo SUCCEFUL COMPILATION
    echo:
)


if ["%~1"]==["run"] (
    run.bat
)


if ["%~1"]==["db"] (
    cd bin
    devenv DEngine.exe
    cd ..
)
