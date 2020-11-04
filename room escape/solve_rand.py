from pwn import *
from ctypes import *
from ctypes.util import *

r = process("./room_escape")

r.sendlineafter("> ", "1")
r.recvuntil("> ")

libc = CDLL(find_library('c'))
seed = libc.time(0)
libc.srand(seed)
ran = libc.rand() % 10000

r.sendline(str(ran).rjust(4, '0'))

r.interactive()