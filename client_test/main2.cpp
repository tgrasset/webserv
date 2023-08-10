#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

int	main()
{
	struct stat buf;
	unsigned int size;

	stat("/home/mbocquel/Documents/webserv/goinfre/fr.subject.pdf", &buf);
	size = buf.st_size;
	std::cout << size << std::endl;
	return 0;
}