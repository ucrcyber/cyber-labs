# Buffer Overflow

A buffer overflow exists when a program attempts to write more data into a 
buffer than the buffer has space to handle. In the case of programs written 
in C/C++ or Fortran, where there is no implicit protections against such a 
case, the data is written into the subsequent memory locations. This 
overwrites any data previously in that location even if it is being used by 
other parts of the program.

This is a well known, and long lived security vulnerability. As such, there 
is a lot of information to cover about it. So we will be splitting this into 
three different labs.

1. Hidden Function
	- In this lab we will learn the basics of the vulnerability, and a 
	simple method of exploiting it to call a hidden function.

2. Shellcode Injection
	- We continue our exploration of the buffer overflow vulnerability by 
	leveraging a buffer overflow to call code that does not exist in the 
	program.

3. Return-to-libc
	- We further explore the exploitation of buffer overflows by learning how 
	to create a stackframe that frees us from the need to inject shellcode 
	onto the stack.
