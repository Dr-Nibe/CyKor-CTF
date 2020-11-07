# solve_rand.py
from pwn import *
from ctypes import *
from ctypes.util import *

r = remote("srv.cykor.kr", 31002)

r.sendlineafter("> ", "1")

r.recvuntil("> ")

libc = CDLL(find_library('c'))
seed = libc.time(0)
print(seed)

libc.srand(seed)
ran = libc.rand() % 10000
print(ran)

r.sendline(str(ran).rjust(4, '0'))

r.interactive()