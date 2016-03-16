#!/bin/bash
find . -name "*.c" -o -name "*.S" -o -name "*.h" > cscope.files
cscope -q -R -b -i cscope.files
