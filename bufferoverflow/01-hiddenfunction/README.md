# Buffer Overflow

A buffer overflow exists when a program attempts to write more data into a 
buffer than the buffer has space to handle. In the case of programs written 
in C/C++ or Fortran, where there is no implicit protections against such a 
case, the data is written into the subsequent memory locations. This 
overwrites any data previously in that location even if it is being used by 
other parts of the program.

## Overview

In this lesson, we will leverage the buffer overflow condition to overwrite 
the data placed on the stack by function calls that is used to return from 
said function. By overwriting the return address, we can redirect the program 
control flow to execute code found anywhere within the program that we choose.

Before we can get into that however, we need to understand how the stack 
figures into the execution of a program.

## Memory Layout of a C/C++ Program

For the purpose of this tutorial, we will examine a simple memory layout 
example, consisting of three practical parts: Text, Heap, and Stack. While 
realistic memory layout is more intricate than described here, this simplified 
explanation helps us focus on details that are more pertinent here. In future 
lessons, we will examine details which we choose to ignore here.

![alt text](img/MemLayout.png "C/C++ Memory Layout")

**Text Segment**

The Text section of the memory resides in the lowest memory addresses and is 
populated with the compiled program at execution time. Instructions for the 
execution of the program are located here, and are executed in mostly 
sequential order as the instuction pointer moves through them. More on the 
instruction pointer later.

**Heap**

The Heap is populated dynamically during execution by memory allocation 
functions. We will not be utilizing this memory section during this lesson, 
but it is a space in memory where buffers can overflow and is important enough 
to make note of regardless.

**Stack**

The Stack is, for the purposes of this lesson, the most important part of the 
memory to understand. The stack is used to store variables, and status 
register data during function calls.

## CPU Registers

Registers are memory locations on the CPU used to store information that as 
the CPU is executing instructions. For the purpose of this lesson there are 
three that we need to know about: The Instruction Pointer, The Stack Pointer, 
and the Base Pointer.

**Instruction Pointer**

The Instrcution Pointer contains the memory address of the next instruction 
to be executed.

**Stack Pointer**

The Stack Pointer contains the address of the top of the stack. Remember from 
earlier that the stack grows towards lower addresses. A smaller stack pointer 
means a larger stack.

**Base Pointer**

The Base Pointer contains the address of the base of the current stack frame. 
The stack frame contains data that is pushed to the stack when a function call is made.

## How a Function is Called

**To call a function**

1. The parameter is pushed onto the stack.
2. The current value of the instruction pointer is pushed onto the stack, this 
is referred to as the return address
3. The address of the function is written to the instruction pointer
4. The current value of the base pointer is pushed onto the stack
5. The value of the stack pointer is written to the base pointer
6. The stack pointer is decremented to reserve space on the stack for function 
variables.

At this point you might notice that the location of stack variables in memory 
is very close to the return address. Considering the knowledge that we can 
sometimes write beyond the limits of the variables, you might see how we could 
use this to change the value of the return address.

**To return from a function:**

1. The current value of the base pointer is written into the stack pointer
2. The previous base pointer, which is located in the address pointed to by 
the current base pointer, is written into the base pointer
3. The previous value of the instruction pointer, which was pushed onto the 
stack as the return address during the function call, is written into the 
instruction pointer.

If a buffer within the function variables was overflown, and the return 
address modified, the program would not return correctly. This behavior could 
be exploited to redirect the program to execute code outside it's expected 
behavior. This is how we will be exploting a buffer overflow here.

## Simple Buffer Overflow Example

We will begin with a an attempt to redirect the control flow of a program to 
call a hidden function. 

Examine the source code of hiddenfunc.c. Notice how the function `hiddenfunct` 
should never be called during the execution of the program. So our goal here 
is to cause that function to be called.

### Compile the Project

Before we can execute the program we have to disable some of the protections 
implemented by the compiler and operating system to prevent this sort of thing 
from happening.

**ASLR: Address Space Layout Randomization**

One of the ways that a system tries to protect against attacks. We note here 
that it exists, but don't need to describe it in detail. We'll get back to 
this later. For now we simply disable the functionality temporarily by running 
the command:

`echo 0 | sudo tee /proc/sys/kernel/randomize_va_space`

This will be reset upon a reboot, but can also be reset by the command: 

`echo 2 | sudo tee /proc/sys/kernel/randomize_va_space`

**Stack Protections and Non-Executable Stack**

The gcc compiler has several ways of preventing buffer overflow exploits.
By preventing intructions on the stack from being executable, and by pushing 
small bits of data onto the stack which are checked to verify that the saved 
register data has not been overwritten. Both of these protections are avoided 
for this lesson by compiling with the flags:

`-z execstack -fno-stack-protector`

**GDB: The GNU Project Debugger**

We will use a debugger to get a better look at the execution of the program. 
The debugger used here is GDB, although most other debuggers will have similar 
functionality. In order to use GDB, we need to compile the program with the 
`-g` flag. So compiling our program for this lesson will be done with the 
command:

`gcc -g -z execstack -fno-stack-protector -o hiddenfunc hiddenfunc.c`

## Debugging the Project with GDB

To run the project, use the command:

`gdb hiddenfunc`

This will open the program with GDB. The program will not begin execution 
immediately. This allows you to examine the program in detail. Use the command 
`list` to see the source code of the program. There should be no spurises in 
it's contents. The hidden function should be there, as well as the vulnerable 
function and main.

**Breakpoints**

You can set a breakpoint at the vulnerable function by using the command `b n` 
where *n* is the line number or name of the function. This will cause the 
programs execution to stop, or break,  at that line so that you can examine 
the program in detail. Do this now, it will be important later.

**Executing the Program**

Start the program execution with the `r` command. You can pass arguments to 
the program here like you would when launching the program from the terminal. 
This can also be used to restart the program without exiting the debugger.
Launch the program with any small amount of text. "Hello World!" would work 
here just as well as in most programming tutorials.

`r "Hello World!"`

The program should break at the beginning of `vulnfunc`. You can step through 
lines with `s` or `n`. You can continue the execution of the program with `c`.
Continue the program execution and it should exit normally after printing the 
text you entered as expected.

**Examining the Registers**

Retart the program by running it like earlier, and it should break again at 
`vulnfunc`. This time we will not continue the execution. Instead, we will examine the state of the CPU registers with the command `info registers`.

This will show the contents of the CPU registers. The registers we are 
interested in here are `rbp`, `rsp`, and `rip` if you are on an x86\_64 
machine, or `ebp`, `esp`, `eip` if you are on an x86 machine. These correspond 
to the base pointer, the stack pointer, and the instruction pointer.

**Examining the Stack**

You can examine the memory contents at any given location using the `x` 
command. To examine the stack contents, we can use `x /8x $rsp`. This will 
display eight elements starting at the stack pointer. Alternatively, you could 
use the address we saw in the registers earlier with the `x` command, like 
`x /8x 0x7fffffffdda0`.

The format of the `x` command is `x /[length][format] [address]` where:

- length: the number of elements to display. Elements can be specified as 
follows
    - b: byte
    - h: halfword (16-bits)
    - w: word (32-bits) *Default*
    - g: giant word (64-bit value)
- format: Formats can be specified as follows
    - o: octal
    - x: hexidecimal *default*
    - d: decimal
    - u: unsigned decimal
    - t: binary
    - f: floating point
    - a: address
    - c: char
    - s: string
    - i: instruction

The value we are most interested in is the return address, which we know is 
located after the base pointer from our examination of how a function call is 
made. So for us to examine the return address we can use the command 
`x /gx ($rbp + 8)` if we are on an x86\64 machine, or `x /wx ($ebp + 4)` if we 
are on an x86 machine.

**Dissassemle**

So now that we know how to see the return address, and have an idea of how to 
write to it. We need to figure out what to write to it. In this lesson, we are 
trying to return to the hidden function. So we need to find its location in 
memory. 

We can do this by dissassembling the code. In GDB, we can do this with the 
`disassemble` command which can be given a starting point by address or 
function. In our case we can use `disassemble hiddenfunc`.

The addresses of the instructions are displayed on the left as hexidecimal 
values. You can reference what you are looking at with the corresponding 
source lines by using `disassemble /m hiddenfunc`. This will print the source 
line, followed by the assembly instructions corresponding to that line.

The address of this line will be added to the end of our input, and when our 
input is too long for our buffer, it will overwrite the return address. There 
is one small difference here. Because x86 machines are little endian, we will 
need to enter the address backwards. That is to say, if the address is 
0x0123456789 it will need to be input as $'\x89\x67\x45\x23\x01'. The \x 
simply indicated that the following is a hexidecimal byte, the surrounding 
characters prevent the characters from being interpreted as a string.

We could run a loop at this point, incrementing the length of our input by one 
character each iteration until we properly overflow the buffer. In later 
lessons we will run loops for similar reasons as not all attacks will work on 
the first attempt even if run properly.

In this case, however, we can determine the length of the input required by 
further examining the dissassembled code. To make things easier on ourselves 
we will examine the assembly with the source using `disassemble /m vulnfunc`.

We see under `strcpy(buf, s);`, the vulnerable line, several assembly 
instructions. The one we are looking for it the first `lea`, or load effective 
address. It should look something like: `lea -0x20(%rbp),%rax`.  What this 
tells us is that there is 0x1e bytes reserved for `buf`. Notice that this is 
not the 20 bytes we might expect from looking at our source code.

## Putting it Together

With the knowledge of how large the buffer is, and the address we want to 
write into the return address, we can format our input to overflow the buffer 
and call the hidden function. Keep in mind that we have to write past the 
base pointer as well as the length of the buffer in order to get to the return 
address. For this example, any data will work to fill the buffer, I used the 
letter 'a'. We can test this within GDB by running the program now. For me, 
the correct input was:

`r aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa$'\x5a\x47\x55\x55\x55\x55\x00\x00'`

## Closing Notes

Notice that this causes the program to segfault and crash after returning 
from the hidden function. But by this point, we've already exploited the 
vulnerability. So what do we care if the program crashes after that?

In the next part of this tutorial, we will learn how to leverage a buffer 
overflow to inject executable code. This allows us to not only alter the 
control flow of a program, but to cause programs to run code which they 
were nover programed for.
ow that we see how to leverage a buffer overflow to attain a shell with protections turned off, we can start turning them on.
