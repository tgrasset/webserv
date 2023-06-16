#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <cstdlib>
# include <ctime>

# include <sys/types.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <netdb.h>
# include <sys/epoll.h>
# include <string.h>
# include <signal.h>

# define MAX_WAIT 15
# define MAX_EVENTS 100
# define BUFFER_SIZE 4096

std::vector<std::string>    cpp_split(std::string str, char const *charset);
int							stringToInt(std::string str);
bool						isValidConfValue(std::string &str);
std::string					getStatus(int code);
bool						checkFile(std::string const &file, std::string const &root);
std::string					getTimestamp(void);

#endif