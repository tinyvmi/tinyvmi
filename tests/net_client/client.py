#!/usr/bin/env python

import socket

TCP_IP = '10.0.0.101'
TCP_PORT = 13
BUFFER_SIZE = 29
MESSAGE = "Hello, World!"

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

print ("socket created, now connect to IP " + TCP_IP + " ...")

s.connect((TCP_IP, TCP_PORT))

print ("connected, now send msg ...")

s.send(MESSAGE)

print ("message sent; now receive data...")

data = s.recv(BUFFER_SIZE)

s.close()

print "received data:", data
