# -*- coding: utf-8 -*-

import socket

conn = socket.socket()
conn.connect(("localhost", 10000))

while 1:
    conn.send("4")

conn.close()
