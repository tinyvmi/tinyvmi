#!/usr/bin/env python

import socket

TCP_IP = '10.0.0.101'
#TCP_PORT = 13
TCP_PORT = 49152
#BUFFER_SIZE = 29
BUFFER_SIZE = 28
MESSAGE = "Hello, World!"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

print ("socket created, now connect to IP " + TCP_IP + ", port: " + str(TCP_PORT) + " ...")

s.connect((TCP_IP, TCP_PORT))

print ("connected, now send msg ...")

s.send(MESSAGE)

print ("message sent; now receive data...")

data = s.recv(BUFFER_SIZE)

s.close()

print "received data:", data
