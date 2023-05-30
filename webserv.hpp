#ifndef WEBSERV_HPP

# define WEBSERV_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include <fstream>
# include <sstream>

# include <sys/types.h>
# include <sys/stat.h>
# include <netinet/in.h>
# include <unistd.h>

std::vector<std::string>    cpp_split(std::string str, char const *charset);

#endif