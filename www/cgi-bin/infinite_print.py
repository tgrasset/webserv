#!/usr/bin/python
import time

print("<!DOCTYPE html>")
print("<html><title>Infinite</title><body>")
print("<h1>Infinite</h1>")

while True:
    print("coucou ! <br>", end=" ", flush=True)
    time.sleep(0.5)