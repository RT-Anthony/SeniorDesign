@echo off
start mdb -hard -digilent -off=prefer_soft_bp -off=download -on=reset_upon_restart -cmd="read ../../cmake/memorymap.txt" -chipinit=chipinit.txt ..\..\rom\blank.elf
