FROM	debian:bullseye

LABEL	author="mbocquel"
LABEL	version="1.0"

RUN		apt update -y \
		&& apt upgrade -y \
		&& apt install -y nginx \
		&& mkdir -p /etc/nginx/ssl \
		&& apt install openssl -y \
		&& mkdir upload

RUN openssl req -x509 -nodes -out /etc/nginx/ssl/inception.crt -keyout /etc/nginx/ssl/inception.key -subj "/C=FR/ST=IDF/L=Paris/O=42/OU=42/CN=mbocquel.42.fr/UID=mbocquel"

RUN		chmod 755 /var/www/html && \
		chown -R www-data:www-data /var/www/html

EXPOSE	5455

COPY	conf/nginx.conf /etc/nginx/nginx.conf

CMD		["nginx", "-g", "daemon off;"]