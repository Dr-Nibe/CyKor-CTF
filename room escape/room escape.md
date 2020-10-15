# room escape

## 3. post-it

![image](https://user-images.githubusercontent.com/59759771/96142193-c4414b80-0f3c-11eb-905e-aca0b2d9a460.png)

'-'과 '_'을 1과 0으로 치환하고, 8비트씩 아스키 문자로 바꿨을 때 base64 인코딩된 문자열이라는 느낌이 들어야 한다. decode를 3번 하면 '컴퓨터 건드리면 혼난다'를 영어로 친 문자열을 얻는다. 컴퓨터의 패스워드다.

<br>

```python
# password_decode.py

import base64

password_encoded = '_-_-_---_--_--___--___--_----____--__-____--__-__-___--__-__-____-_-_-_-_--_---__-_-__-__-_-_-___--____-_--_--___-__-_-__--_----_-_--__-_-_-_--__-_-_--__-__-_--_-__---__-_-_--__--_--___-_--____-__---__-_-_-_-__---__-_--_-__-_-_-__-___--_____---_____-_--__-_-_-_--___--__-___--_-_-_-__----_-_-_---_--_--___--__-___-___---_-_-_---_--_-_-__-_--_-__-_-_-___-_-_--__-_-_---_-_-__-__---__-__-_--__-_--_-_-__-__-_-__-__---__--__-_-_--_--___-_-__-__---_-_-_-_-__--_--_--_-_---_____-__---__--___-__-___--__---______--_-_-'

password_encoded = password_encoded.replace('-', '1')
password_encoded = password_encoded.replace('_', '0')

password_decoded = ''

while len(password_encoded) != 0:
    password_decoded += chr(int(password_encoded[:8], 2))
    password_encoded = password_encoded[8:]

password_decoded = base64.b64decode(password_decoded)
password_decoded = base64.b64decode(password_decoded)
password_decoded = base64.b64decode(password_decoded)

print(password_decoded)
```

![image](https://user-images.githubusercontent.com/59759771/96142337-efc43600-0f3c-11eb-8cf6-f7964114c39f.png)

<br>

---

## 2. computer

![image](https://user-images.githubusercontent.com/59759771/96142485-1c784d80-0f3d-11eb-9d06-a79a60526d20.png)

zip파일의 key.txt 파일에는 암호가 걸려 있다. 암호에 대한 힌트는 얻을 수 없다. 게싱으로 풀면 인정.

<br>

![image](https://user-images.githubusercontent.com/59759771/96147536-61eb4980-0f42-11eb-9f72-b9816be04482.png)

PNG image 파일 헤더 발견.

<br>

![image](https://user-images.githubusercontent.com/59759771/96147600-70396580-0f42-11eb-858b-6bb009fbbd8f.png)

![image](https://user-images.githubusercontent.com/59759771/96143495-18006480-0f3e-11eb-8d9a-9c0b2f47a02f.png)

![00000001](https://user-images.githubusercontent.com/59759771/96143605-35cdc980-0f3e-11eb-92dc-8d4480efb594.png)

`foremost`로 추출하면 이런 이미지 파일이 나온다. 뒤에서 `steghide`의 passphrase로 사용된다.

```
a4cefa0ef4fc6ec2dc9b6e870d6fa63cd0ee44337506923884a4f013fe702899
```

<br>

---

## 1. safe

![image](https://user-images.githubusercontent.com/59759771/96143917-93faac80-0f3e-11eb-83f0-8a13b761edf4.png)

0000~9999 범위 밖의 숫자를 입력해보면 금고의 패스워드를 만드는 루틴을 알아낼 수 있다. 현재 시간을 시드로 해서 랜덤값을 생성한다. 파이썬의 `ctypes` 모듈로 랜덤값을 재현할 수 있다.

```python
# solve_rand.py

from pwn import *
from ctypes import *
from ctypes.util import *

r = process("./room_escape")

r.sendlineafter("3. post-it", "1")
r.recvuntil("Input password. (0000 ~ 9999)")

libc = CDLL(find_library('c'))
seed = libc.time(0)
libc.srand(seed)
ran = libc.rand() % 10000

r.sendline(str(ran).rjust(4, '0'))

r.interactive()
```

![image](https://user-images.githubusercontent.com/59759771/96144163-d45a2a80-0f3e-11eb-8e19-2f246926e101.png)

<br>

![secret](https://user-images.githubusercontent.com/59759771/96144195-e045ec80-0f3e-11eb-94db-6106d556dd4f.jpg)

사진이 깨져 있다. jpeg 헤더의 어딘가가 손상되었을 가능성이 높다.

<br>

![image](https://user-images.githubusercontent.com/59759771/96146190-fe145100-0f40-11eb-861d-e3d683ff021f.png)

HxD로 보면 허프만 코드가 손상되어 있다. 원상복구해준다.

![image](https://user-images.githubusercontent.com/59759771/96146314-1e441000-0f41-11eb-967f-b8b393214b1a.png)

![secret](https://user-images.githubusercontent.com/59759771/96146378-3025b300-0f41-11eb-8330-a8351c0bab05.jpg)

신공학관 앞.

<br>

![image-20201015235126808](C:/Users/sjjo0/AppData/Roaming/Typora/typora-user-images/image-20201015235126808.png)

앞에서 구한 key를 passphrase에 넣으면 flag.txt 파일을 뽑아낼 수 있다. `steghide`를 사용해야 한다는 건 약간의 게싱. hi.txt 파일의 내용이 힌트인 거 같기도 하고.

![image](https://user-images.githubusercontent.com/59759771/96147108-f4d7b400-0f41-11eb-91c6-a0c7f8173347.png)