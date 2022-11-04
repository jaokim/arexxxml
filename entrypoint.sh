#!/bin/sh -l

echo Hello
echo Doo
whoami
pwd
ls -l
ls -l ..
make CROSS_COMPILE=ppc-amigaos- -f Makefile

