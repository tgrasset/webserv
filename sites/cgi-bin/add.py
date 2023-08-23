#!/usr/bin/python

# Import modules for CGI handling
import cgi, cgitb

# Create instance of FieldStorage
form = cgi.FieldStorage()
# Get data from fields

def ft_add(a, b):
    return a + b

if "a" in form and "b" in form:
    a = int(form.getvalue('a'))
    b = int(form.getvalue('b'))

    sum = ft_add(a, b)

    print("<!DOCTYPE html>")
    print("<html lang=\"en\">")
    print("<head>\n")
    print("<meta charset=\"UTF-8\">")
    print("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
    print("<title>Basic Calculator</title>")
    print("</head>")
    print("<body>")
    print("<h1>Result :</h1>")
    print("<p>", a, " + ", b, " = ", sum, ".</p>")
    print("</body>")
    print("</html>")

else:
    print("<!DOCTYPE html>")
    print("<html lang=\"en\">")
    print("<head>\n")
    print("<meta charset=\"UTF-8\">")
    print("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
    print("<title>Basic Calculator</title>")
    print("</head>")
    print("<body>")
    print("<h1>Error :</h1>")
    print("<p>'a' and 'b' parameters required.</p>")
    print("</body>")
    print("</html>")
