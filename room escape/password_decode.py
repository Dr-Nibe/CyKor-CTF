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