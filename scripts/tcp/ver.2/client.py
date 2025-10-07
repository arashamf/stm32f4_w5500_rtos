# -*- coding: utf-8 -*-

import socket

conn = socket.socket()
conn.connect(('localhost', 9090))
conn.send(b"Hello! \n")
data = b""
tmp = conn.recv(1024)
while tmp:
    data += tmp
    tmp = conn.recv(1024)
print( data.decode("utf-8") )
conn.close()
