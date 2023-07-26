@echo off

if not exist "build" mkdir build
gcc -o build/hangman.exe hangman.c
