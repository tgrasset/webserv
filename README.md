# Fonctions deja connues 
- execve 
- dup
- dup2
- pipe
- strerror
- gai_strerror
- errno
- dup
- dup2
- fork



# Nouvelles fonctions

## htons, htonl, ntohs, ntohl
The htonl() function converts the unsigned integer hostlong from host byte order to network byte order.
The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.
The ntohl() function converts the unsigned integer netlong from network byte order to host byte order.
The ntohs() function converts the unsigned short integer netshort from network byte order to host byte order.

*En gros ca permet de gerer les problemes de endian, c'est a dire la facon dont les nombres (int, short etc.) sont stockes par le system. En gros le principe est de s'assurer que peut importe la facon dont les donnes sont organises dans la machine(bigendian ou shortendian), elles sont envoyes dans une norme ok pour le reseau.*

Une video pour comprendre l'interet de ces fonctions :
https://youtu.be/OoHich9BPxg

*Je kiffe ce mec ! D'apres lui pour un serveur HTTP on devrait pas trop avoir besoin de se faire chier avec ces fonctions car on devrait envoyer du texte et pas des int ou des short etc.*

## select
Petite explication toujours via mon youtuber chauve prefere : 
https://youtu.be/Y6pFtgRdUts
C'est un moyen d'avoir un peu de parallélisation sans pour autant utiliser des threads. 
Prend un groupe de File Descriptor et te dis quand il y a quelque chose a lire sur l'un de ces FD. 

**Pas sur que ca soit vraiment interessant d'utiliser a la place des Threads, mais a voir...**

## poll
C'est un cousin de select. Mais en mieux.
Pas besoin de mettre les sockets dans un set de FD, ni de calculer le plus grand des FD. Juste besoin d'envoyer un tableau de pollfd struct (aui contient un fd, un short qui decrit le requested event et un short qui decrit le returned event).

- event c'est nous qui le definissont pour dire ce qu'on cherche comme event.
- revents c'est ce qui s'est passe chez le client. 
Interet aussi c'est que contrairement a Select, les fd ne changent pas. 

Partie d'une video qui parle de ca. 
https://youtu.be/tAGF0T2cXRA?t=2935

Pb c'est que pour Select et pour Poll ca peut etre assez lent si on a genre 100 sockets. 

## epoll (epoll_create, epoll_ctl, epoll_wait)

epoll est une variante de poll que l'on peut déclencher par niveau ou par changement d'état, et monte bien en charge pour un grand nombre de descripteurs simultanés. Trois appels système sont fournis pour configurer et commander un ensemble epoll : epoll_create, epoll_ctl, epoll_wait.

Un ensemble epoll est connecté à un descripteur de fichier créé par epoll_create. L'interêt pour certains descripteurs est ensuite enregistré avec epoll_ctl. Enfin, l'attente effective démarre avec l'appel epoll_wait. 

http://manpagesfr.free.fr/man/man7/epoll.7.html

## kqueue (kqueue, kevent)
C'est une autre facon de gerer les IO asynchronus que select et Poll. C'est via la libevent. 
J'ai trouver un tuto qui explique un peu le concept
https://wiki.netbsd.org/tutorials/kqueue_tutorial/


## socket

## accept

## listen

## send

## recv

## bind

## connect

## getaddrinfo

## freeaddrinfo

## setsockopt

## getsockname

## getprotobyname

## fcntl



# Ressources interessantes
## Understanding Nginx Server and Location Block Selection Algorithms
https://www.digitalocean.com/community/tutorials/understanding-nginx-server-and-location-block-selection-algorithms

# Write Your Own HTTP Server
https://betterprogramming.pub/writing-your-own-http-server-introduction-b2f94581268b

# Documentation officielle HTTP
https://www.rfc-editor.org/rfc/rfc9112.html

# Wikipedia Common Gateway Interface
https://en.wikipedia.org/wiki/Common_Gateway_Interface

# Wikipedia Werbserver
https://en.wikipedia.org/wiki/Web_server

# Lecture universite sur Poll et Select 
https://youtu.be/tAGF0T2cXRA


# TO DO dans le code
- On cree une classe "Server_manager" qui va etre en charge de lancer la boucle while principale. avec du multiplexing. 
	- Contient vecteur de serveur (du main curent). Chaqun des serveurs a un listen FD. 
	- Elle a aussi un vecteur de Client. Chaque client a son fd de com (un status du genre, je veux parler, j'attend une reponse etc. ), son socket, sa requette HTTP, sa reponse HTTP. ==> **Besoin d'une classe requette et reponse HTTP.** 

	*Voir comment faire pour que un FD deja utilise puisse etre remis dans la pool de connexion*
	**Attention** si deux serveurs declares ont le meme port et meme host, il faut qu'on utilise le socket deja en place. (on listen sur le meme socket, mais en fonction de la requette on utilisera les param d'un serveur ou d'un autre.)

- Josef prend en charge la creation de la classe requete HTTP qui parse une string comme il faut.