#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>
# include <cstdlib>
# include <csignal>
# include <ctime>
# include <iomanip>
# include <cstdio>

# include <sys/types.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <netdb.h>
# include <sys/epoll.h>
# include <sys/time.h>
# include <string.h>
# include <signal.h>

# define MAX_WAIT 15
# define MAX_EVENTS 100
# define BUFFER_SIZE 4096
# define MAX_TIME_CLIENT_MS 10000

std::vector<std::string>    cpp_split(std::string str, char const *charset);
int							stringToInt(std::string str);
bool						isValidConfValue(std::string &str);
std::string					getStatus(int code);
bool						checkFile(std::string const &file, std::string const &root);
std::string					getTimestamp(void);
std::string					timeStamp(void);
std::string					sizeToString(size_t n);
bool						caseInsensitiveCmp(std::string const &a, std::string const &b);
std::string					toUpperCase(const std::string& str);
std::string					redirectionHTML(int code, std::string message, std::string path);
std::string					autoindexHTML(std::string path);
std::string					errorHTML(int code, std::string message);
std::string					successfulDeleteHTML(std::string path);
std::string					getErrorPageContent(std::string path, int code, std::string message);
std::string					getMimeType(std::string path, std::map<std::string, std::string> &types);

#endif
