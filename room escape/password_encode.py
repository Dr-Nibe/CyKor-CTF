import base64

password = b'zjavbxj rjsemflaus ghssksek'

password = base64.b64encode(password)
password = base64.b64encode(password)
password = base64.b64encode(password)

password_encoded = ''

for i in password:
    password_encoded += format(i, 'b').rjust(8, '0')

password_encoded = password_encoded.replace('1', '-')
password_encoded = password_encoded.replace('0', '_')

print(password_encoded)