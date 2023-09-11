#!/usr/bin/python

# Import modules for CGI handling
import cgi

# Create instance of FieldStorage
form = cgi.FieldStorage()
# Get data from fields

username = str(form.getvalue('username'))

print("Set-Cookie: ", "username=", username)
print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>\n")
print("<meta charset=\"UTF-8\">")
print("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
print("<title>Basic Calculator</title>")
print("</head>")
print("<body>")
print("<h1>", username, " has been set as a cookie !</h1>")
print("<p><a href=\"print_cookie.py\">Voir la page de verification du cookie</a></p>")
print("</body>")
print("</html>")
