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
# include <list>
# include <cmath>

# include <sys/types.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <netdb.h>
# include <sys/epoll.h>
# include <poll.h>
# include <sys/time.h>
# include <string.h>
# include <signal.h>
# include <dirent.h>
# include <fcntl.h>

/* Server print options */
# define PRINT_REQ_HEADER true
# define PRINT_REQ_BODY false
# define PRINT_RES_HEADER true
# define PRINT_RES_BODY false
# define PRINT_REQ_FILE_STATUS true
# define PRINT_RES_FILE_STATUS true
# define PRINT_UPLOAD_FILE_STATUS true

# define MAX_WAIT 50
# define MAX_EVENTS 100
# define BUFFER_SIZE 4092
# define MAX_TIME_CLIENT_S "10;"
# define BODY_TMP_FOLDER "tmp_body/"
# define PATH_CGI_UPLOAD "/cgi-upload/upload.py"
# define TXT_END "\033[0m"
# define TXT_YEL "\033[33m"
# define TXT_RED "\033[31m"
# define TXT_GREEN "\033[32m"
# define TXT_BL "\033[34m"
# define TXT_MAG "\033[35m"
# define TXT_CY "\033[36m"
# define TXT_B "\033[1m"
# define TXT_U "\033[4m"
# define TXT_I "\033[3m"

typedef enum eStatus {
	CLOSE,
	OPEN,
	ERROR
}	s_file;

typedef enum ePipeStatus {
	PIPE_CLOSE,
	PIPE_OPEN,
	PIPE_FINISH,
	PIPE_ERROR
}	s_pipe;

std::vector<std::string>    cpp_split(std::string str, char const *charset);
int							stringToInt(std::string str);
bool						isValidConfValue(std::string &str);
std::string					getStatus(int code);
bool						checkFile(std::string const &file, std::string const &root);
std::string					getTimestamp(void);
std::string					getTimestampFileName(void);
std::string					timeStamp(void);
std::string					sizeToString(size_t n);
bool						caseInsensitiveCmp(std::string const &a, std::string const &b);
std::string					toUpperCase(const std::string& str);
std::string					redirectionHTML(int code, std::string message, std::string path);
std::string					autoindexHTML(std::string dirPath, std::string requestUri);
std::string					errorHTML(int code, std::string message);
std::string					getMimeType(std::string path, std::map<std::string, std::string> &types);
void						printVectorChar(std::vector<char> vectChar);

#endif
