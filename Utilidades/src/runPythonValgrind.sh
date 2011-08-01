#!/bin/bash
valgrind.bin --log-file="salidaValgrind.txt" --tool=memcheck --leak-check=full --suppressions=valgrind-python.supp --track-origins=yes python -E -tt $1

