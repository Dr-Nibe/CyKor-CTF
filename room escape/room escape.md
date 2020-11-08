# Room Escape

:black_nib:N1be(csj.n1be@gmail.com)

---

![image](https://user-images.githubusercontent.com/59759771/98417645-b22e7500-20c4-11eb-82b6-72ecfea66496.png)

방 탈출 테마의 문제입니다. 주변에 있는 물건들로부터 단서들을 찾고 종합해서 방을 탈출해야 합니다. 플래그를 찾으면 탈출할 수 있겠죠?

<br>

---

## 3. post-it

단서들은 연쇄적으로 연결되어 있습니다. 한 물건에서 단서를 찾으면 그 단서는 다른 물건에서 단서를 찾기 위한 열쇠가 됩니다. post-it에서부터 단서를 찾아보도록 합시다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98418325-f706db80-20c5-11eb-9061-eca4c3aefb0d.png)

'-'과 '\_'로 이루어진 문자열이 주어졌습니다. '-'을 '1'로, '_'을 '0'으로 치환해봅시다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98418975-6b8e4a00-20c7-11eb-8117-16d80649c0db.png)

2진수인 것 같습니다. 16진수로 바꿔봅시다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98419044-8e206300-20c7-11eb-82c9-f1923dd2547a.png)

유심히 보면 알파벳과 숫자로만 이루어진 문자열임을 알 수 있습니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98419192-dccdfd00-20c7-11eb-92fe-cdf258513b23.png)

수상한 문자열이 나왔습니다. 딱 봐도 base64 디코딩을 해보고 싶게 생겼습니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98419284-130b7c80-20c8-11eb-91df-084313a84140.png)

base64 디코딩을 3번 거쳐서 'zjavbxj rjsemflaus ghssksek'라는 문자열을 얻었습니다. 한글로 치면 '컴퓨터 건드리면 혼난다'라는 문자열입니다. 얼른 컴퓨터를 건드려보고 싶어지는군요.

<br>

---

## 2. computer

![image](https://user-images.githubusercontent.com/59759771/98419426-6382da00-20c8-11eb-947e-dc79c49e8f8d.png)

컴퓨터를 쓰려면 암호를 입력해야 합니다. post-it으로부터 알아낸 암호를 넣어줍시다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98419494-84e3c600-20c8-11eb-8cc7-43a3f0931028.png)

> https://bit.ly/34WbT2P

수상한 zip 파일을 얻었습니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98419571-afce1a00-20c8-11eb-830c-13cc3f70dda8.png)

![image](https://user-images.githubusercontent.com/59759771/98419580-b65c9180-20c8-11eb-8473-a12614d8bd1b.png)

zip 파일에는 3개의 txt 파일이 압축되어 있습니다. 그 중에서 뭔가 중요해 보이는 key.txt 파일에만 암호가 걸려 있습니다. 이 암호에 대한 정보는 주어지지 않기 때문에 풀 수 없을 겁니다. 그보다 중요한 것은 이 zip 파일의 용량이 60KB라는 것입니다. txt 파일 3개치고는 상당히 큽니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98419882-57e3e300-20c9-11eb-9d65-5e92680b0ccc.png)

binwalk로 파일 헤더를 분석해 보면 중간에 PNG image가 삽입되어 있습니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98419980-8661be00-20c9-11eb-91ff-8974f1a3d169.png)

foremost로 추출해보면 PNG 파일을 하나 얻을 수 있습니다.

<br>

![00000001](https://user-images.githubusercontent.com/59759771/98420052-b3ae6c00-20c9-11eb-8a2d-f55930b133fa.png)

이 문자열은 key.txt의 내용과 동일한 문자열입니다. 나중에 유용하게 사용될 것 같으니 일단 넘어갑시다.

<br>

---

## 1. safe

![image](https://user-images.githubusercontent.com/59759771/98420153-e8222800-20c9-11eb-88eb-657b06f5b049.png)

금고의 비밀번호를 뚫어야 합니다. 경우의 수는 만 가지입니다. 브루트 포싱을 하면 언젠가는 뚫릴 수도 있겠지만, 조금 더 효율적인 방법을 찾아보도록 합시다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98420323-47803800-20ca-11eb-904c-340d37804b08.png)

프로그램은 4자리 숫자를 입력하라고 했지만, 말을 안 듣고 5자리 이상을 입력하면 some kind of bug를 뱉습니다. 비밀번호를 생성하는 과정을 알려주는군요. 현재 타임스탬프로 시드를 생성하고, rand()가 반환하는 랜덤 값을 10000으로 나눈 4자리 수가 비밀번호입니다.

<br>

```python
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
```

랜덤 값을 재현해서 입력하는 파이썬 코드입니다. ctypes 모듈로 C의 함수들을 가져와서 사용했습니다. 먼저 문제 서버에 연결하고, 서버에서 비밀번호를 생성하는 타이밍에 맞춰서 여기서도 seed = libc.time(0)으로 타임스탬프를 구해옵니다. 그리고 이 값을 인자로 srand()를 호출하여 시드를 생성하고 rand()를 호출하면 서버의 rand()와 동일한 값을 반환할 것입니다. 이 값을 10000으로 나누고 4자리로 맞춰서 입력을 보내주면 됩니다.

WSL에서는 이 코드가 제대로 동작하지 않습니다. 확인해보니 WSL이 타임스탬프를 가져오는 과정에서 뭔가 문제가 있는지 서버 시간과 일치하지 않기 때문이었습니다. 제 노트북의 WSL에서는 서버와 20~25초가량의 시간 차이가 발생했습니다. macOS, Windows, Ubuntu에서는 타임스탬프가 정확하게 일치하는 것을 확인했습니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98421672-37b62300-20cd-11eb-996e-3628e3d688a9.png)

> https://bit.ly/3iXDynM

비밀번호를 맞추면 사진을 하나 줍니다.

<br>

![secret](https://user-images.githubusercontent.com/59759771/98421757-69c78500-20cd-11eb-8144-2e75ab3404d5.jpg)

JPG 파일입니다. 앞에서 컴퓨터에서 찾은 zip 파일에 두 개의 힌트가 있었습니다. 각각의 내용은 "steg"와 "hide"였습니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98421874-ba3ee280-20cd-11eb-8d74-070ebc485c6e.png)

steghide는 리눅스에서 제공하는 스테가노그래피 툴입니다. apt 패키지 매니저를 이용해서 쉽게 설치할 수 있습니다(sudo apt install steghide). 맨 아래 줄을 보면 jpg 파일로부터 숨겨진 데이터를 추출하는 명령어를 친절하게 알려줍니다. secret.jpg에 숨겨진 데이터를 추출해봅시다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98422038-1144b780-20ce-11eb-8013-8dc15022bac8.png)

passphrase를 입력해야 합니다. 앞에서 zip 파일로부터 뽑아낸 PNG 파일에 적힌 문자열(a4cefa0ef4fc6ec2dc9b6e870d6fa63cd0ee44337506923884a4f013fe702899)이 passphrase입니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98422137-44874680-20ce-11eb-9a73-4d2f7fb8cf9f.png)

passphrase를 입력했더니 오류가 발생합니다. 허프만 코드가 손상되었다고 합니다. 허프만 코드는 데이터를 효율적으로 압축하기 위해 사용되는 알고리즘의 한 종류입니다. 허프만 코드의 이론이나 구현 등 자세한 내용은 여기서는 크게 중요하지 않습니다. JPG는 실제 이미지가 효율적으로 손실 압축된 파일이고, 이 압축 과정에서 허프만 코드가 사용되며, JPG 파일 헤더에서 허프만 코드는 FFC4로 시작한다는 사실을 알면 됩니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98425480-9b921900-20d8-11eb-8910-651529b604e4.png)

secret.jpg 파일을 HxD로 열어서 FFC4를 찾아보면 4개의 결과가 나옵니다. 총 4개의 허프만 코드 테이블이 연결되어 있습니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98428192-b8801980-20e3-11eb-89ac-0d2eb9af89c1.png)

![image](https://user-images.githubusercontent.com/59759771/98428208-cfbf0700-20e3-11eb-98e5-40e18c069fb6.png)

개인적인 생각으로, 약간의 게싱이 필요한 부분입니다. 아래는 네 번째 허프만 코드 테이블의 한 부분이고, 정상적인 데이터입니다. 위는 두 번째 허프만 코드 테이블의 한 부분이고, 손상된 데이터입니다. 위쪽을 아래와 같이 고쳐주면 사진이 복구됩니다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98428399-a3f05100-20e4-11eb-8047-d6dd64eda937.png)

![secret](https://user-images.githubusercontent.com/59759771/98428404-ad79b900-20e4-11eb-9b61-a8526e9c2fd1.jpg)

신공학관 앞에서 찍은 사진입니다. 이 사진을 다시 steghide에 넣어봅시다.

<br>

![image](https://user-images.githubusercontent.com/59759771/98428438-d437ef80-20e4-11eb-9fac-9b8fdf1c5c2b.png)

플래그를 얻을 수 있습니다.