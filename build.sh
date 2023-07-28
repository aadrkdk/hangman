#!/bin/bash

which gcc &>/dev/null
if [ $? == 0 ]
then
    mkdir -p build
    gcc -o build/hangman hangman.c
else
    echo "gcc not found"
fi
