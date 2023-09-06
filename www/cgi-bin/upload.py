#!/usr/bin/python

import os

def upload_file_to_server(temp_file, boundary, upload_dir):
	try:
		with open(temp_file, 'rb') as tmp_file:
			header = False
			body = False
			footer = False
			file_name = ""
			path_file = ""
			output = None

			for line in tmp_file:
				if boundary in line.decode('utf-8',  errors='ignore'):
					header = True
					if body == True and output:
						#fin d'un premier fichier, besoin de fermer le fichier
						if output.closed == False:
							output.close()
						file_name = ""
						path_file = ""
						body = False
						output = None
				elif header and line.decode('utf-8',  errors='ignore').strip() == "":
					header = False
					body = True
				elif header:
					posBeginFilename = line.decode('utf-8',  errors='ignore').find("filename=")
					if posBeginFilename != -1:
						posBeginFilename += 10
						posEndFilename = line.decode('utf-8',  errors='ignore').find('"', posBeginFilename)
						if posEndFilename != -1:
							file_name = line.decode('utf-8',  errors='ignore')[posBeginFilename:posEndFilename]
							if output == None and file_name != "":
								path_file = os.path.join(upload_dir, file_name)
								try:
									output = open(path_file, 'wb')
								except IOError:
									tmp_file.close()
									return 403
							else:
								continue
					else:
						continue
				elif header == False and body == True and output != None:
					output.write(line)
		tmp_file.close()
		return 201
	except IOError:
		return 403

# Récupérer la valeur de la variable d'environnement
boundary = os.environ.get("BOUNDARY")
temp_file = os.environ.get("TMP_FILE")
upload_dir = os.environ.get("UPLOAD_DIR")
status_code = upload_file_to_server(temp_file, boundary, upload_dir)

if status_code == 201:
	print("<!DOCTYPE html>")
	print("<html lang=\"en\">")
	print("<head>")
	print("<meta charset=\"UTF-8\">")
	print("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
	print("<title>Upload</title>")
	print("</head>")
	print("<body>")
	print("<h1>201 OK</h1>")
	print("</body>")
	print("</html>")
elif status_code == 403:
	print("<!DOCTYPE html>")
	print("<html lang=\"en\">")
	print("<head>")
	print("<meta charset=\"UTF-8\">")
	print("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">")
	print("<title>Error 403</title>")
	print("</head>")
	print("<body>")
	print("<h1>Error 403</h1>")
	print("</body>")
	print("</html>")

