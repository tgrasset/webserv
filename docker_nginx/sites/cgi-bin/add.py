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

    print("Content-type: text/html")
    print()
    print("<h1>Result :</h1>")
    print("<p>", a, " + ", b, " = ", sum, ".</p>")

else:
    print("Content-type: text/html")
    print()
    print("<h1>Error :</h1>")
    print("<p>'a' and 'b' parameters required.</p>")
