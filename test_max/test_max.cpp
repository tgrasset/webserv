#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
	struct hostent *h;

	if (argc != 2) {  /* Vérification d'erreurs de la ligne de commande */
		fprintf(stderr,"usage: getip adresse\n");
		exit(1);
	}

	if ((h=gethostbyname(argv[1])) == NULL) {  /* récupérer infos de l'hôte */
		herror("gethostbyname");
		exit(1);
	}

	printf("Host name  : %s\n", h->h_name);
	printf("IP adresse : %s\n",inet_ntoa(*((struct in_addr *)h->h_addr)));

	return 0;
}

