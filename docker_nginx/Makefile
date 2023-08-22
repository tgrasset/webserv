# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/08/01 17:12:15 by mbocquel          #+#    #+#              #
#    Updated: 2023/08/22 16:35:44 by mbocquel         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

SOURCES = 	srcs/requirements/nginx/conf/nginx.conf \
			srcs/requirements/nginx/Dockerfile \
			srcs/docker-compose.yml

NGINX_IMG =		$(shell docker images | grep nginx | grep 1.0 | wc -l)
WP_VOL = 		$(shell docker volume ls | grep srcs_wordpress_vol | wc -l)

all: 	$(SOURCES)
		docker compose -f srcs/docker-compose.yml up -d --pull never

down:	$(SOURCES)
		docker compose -f srcs/docker-compose.yml down

clean:	down
		@if [ $(NGINX_IMG) = "1" ]; then echo "	Removing NGINX Image"; docker rmi nginx:1.0; fi;
		@if [ $(WP_VOL) = "1" ]; then echo "	Removing Wordpress Volume"; docker volume rm srcs_wordpress_vol; fi;
		docker system prune -af

fclean:	clean

re:		fclean all 

.PHONY: all re down clean