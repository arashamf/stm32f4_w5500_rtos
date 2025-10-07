# -*- coding: utf-8 -*-

import socket

conn = socket.socket()
conn.connect(('192.168.11.155', 50505))

while 1:
    conn.send("4")

conn.close()
