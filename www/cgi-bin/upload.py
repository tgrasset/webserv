#!/usr/bin/python

import os

# Récupérer la valeur de la variable d'environnement
boundary = os.environ.get("BOUNDARY")
temp_file = os.environ.get("TMP_FILE")

# fonction a refaire...
def upload_file_to_server(temp_file, boundary, upload_dir):
    try:
        with open(temp_file, 'r') as tmp_file:
            header = False
            file_name = ""
            temp_name = ""
            final_dest = ""
            output = None

            for line in tmp_file:
                if boundary in line:
                    header = True
                elif header and line.strip() == "":
                    header = False
                elif header:
                    pos = line.find("filename=")
                    if pos != -1:
                        pos += 10
                        pos2 = line.find('"', pos)
                        if pos2 != -1:
                            temp_name = line[pos:pos2]
                            if file_name == "" or temp_name != file_name:
                                if file_name != "":
                                    output.close()
                                file_name = temp_name
                                final_dest = os.path.join(upload_dir, file_name)
                                try:
                                    output = open(final_dest, 'wb')
                                except IOError:
                                    _status_code = 403
                                    tmp_file.close()
                                    return
                            else:
                                continue
                    else:
                        continue
                elif file_name != "" and line.strip() != "":
                    if output:
                        output.write(line.encode('utf-8'))

        if file_name != "":
            output.close()
        _status_code = 201
        os.remove(temp_file)
    except IOError:
        _status_code = 403

upload_file_to_server(temp_file, boundary, "./www/test/upload/")



print("<!DOCTYPE html>")
print("<html lang=\"en\">")
print("<head>\n")
print("<meta charset=\"UTF-8\">")
print("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
print("<title>Upload</title>")
print("</head>")
print("<body>")
print("<h1>201 OK</h1>")
print("</body>")
print("</html>")
