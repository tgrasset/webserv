#!/usr/bin/env python3

import os
import http.cookies

# Lire les données du cookie
cookie = http.cookies.SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
username = cookie.get("username")

# En-tête de la réponse HTTP
#print("Content-Type: text/html")
#print()

# Corps de la réponse HTML
print("<html><head><title>Valeur du cookie 'username'</title></head><body>")

if username is not None:
    print(f"<p>La valeur du cookie 'username' est : {username.value}</p>")
else:
    print("<p>Le cookie 'username' n'a pas ete defini.</p>")

print("</body></html>")