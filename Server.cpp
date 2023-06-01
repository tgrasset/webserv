#include "Server.hpp"

Server::Server(void) {

	_port = 0;
	_host = 0;
	_server_name = "";
	_root = "";
	_client_max_body_size = 0;
	_index = "";
	_error_pages.clear();
}

Server::Server(Server const &src) {

	if (this != &src)
        *this = src;
}

Server::~Server() {

}

Server &Server::operator=(Server const &rhs) {

	_port = rhs.getPort();
	_host = rhs.getHost();
	_server_name = rhs.getServerName();
	_root = rhs.getRoot();
	_client_max_body_size = rhs.getClientMaxBodySize();
	_index = rhs.getIndex();
    return (*this);
}

in_port_t	Server::getPort(void) const {

	return (_port);
}

in_addr_t	Server::getHost(void) const {

	return (_host);
}

std::string 	Server::getServerName(void) const {

	return (_server_name);
}

std::string		Server::getRoot(void) const {

	return (_root);
}

unsigned long	Server::getClientMaxBodySize(void) const {

	return (_client_max_body_size);
}

std::string	Server::getIndex(void) const {

	return (_index);
}

std::map<int, std::string>	Server::getErrorPages(void) const {

	return (_error_pages);
}

// std::vector<Location>	Server::getLocations(void) const {

// 	return (_locations);
// }

void	Server::setPort(std::string port) {
	
	if (isValidConfValue(port) == false)
		throw ServerException("';' symbol needed after 'listen' line");
	int n = stringToInt(port);
	if (n < 1 || n > 65535)
		throw ServerException("Invalid port value in 'listen' line");
	_port = htons(n);
}

void	Server::setHost(std::string host) {
	
	if (isValidConfValue(host) == false)
		throw ServerException("';' symbol needed after 'host' line");
	if (host == "localhost")
		host = "127.0.0.1";
	struct sockaddr_in test;
	if (inet_pton(AF_INET, host.c_str(), &test.sin_addr) == 0)
		throw ServerException("IPv4 address format could not be recognised in 'host' line");
	_host = inet_addr(host.c_str());
	
}

void 	Server::setServerName(std::string name) {

	if (isValidConfValue(name) == true)
		_server_name = name;
	else
		throw ServerException("';' symbol needed after 'server_name' line");
}

void	Server::setRoot(std::string root) {
	if (isValidConfValue(root) == false)
		throw ServerException("';' symbol needed after 'root' line");
	struct stat test;
	if (stat(root.c_str(), &test) == 0 && test.st_mode & S_IFDIR)
	{
		_root = root;
		return ;
	}
	char buffer[1024];
	getcwd(buffer, 1024);
	std::string root2 = buffer + root;
	if (stat(root2.c_str(), &test) == 0 && test.st_mode & S_IFDIR)
		_root = root2;
	else
		throw ServerException("Invalid root path");
}

void	Server::setClientMaxBodySize(std::string clientMaxBodySize) {
	
	if (isValidConfValue(clientMaxBodySize) == false)
		throw ServerException("';' symbol needed after 'client_max_body_size' line");
	int n = stringToInt(clientMaxBodySize);
	if (n < 1)
		throw ServerException("Invalid value in 'client_max_body_size' line");
	_client_max_body_size = n;
}

void	Server::setIndex(std::string index) {
	
	if (isValidConfValue(index) == true)
		_index = index;
	else
		throw ServerException("';' symbol needed after 'index' line");
}

void	Server::setErrorPages(std::vector<std::string> errorPages) {
	
	if (errorPages.empty() == true)
		return ;
	size_t size = errorPages.size();
	size_t j;
	int code;
	std::string path;
	if (size % 2 != 0)
		throw ServerException("Listing after 'error_page' directive must follow pattern : <error_number> <file_path>");
	for (size_t i = 0 ; i < size ; i++)
	{
		j = 0;
		if (i % 2 == 0)
		{
			if (errorPages[i].length() != 3)
				throw ServerException("Invalid http error code : " + errorPages[i]);
			code = stringToInt(errorPages[i]);
			if (getStatus(code) == "Unknown")
				throw ServerException("Invalid http error code : " + errorPages[i]);
		}
		else
		{
			path = errorPages[i];
			if (i == errorPages.size() - 1)
			{
				if (isValidConfValue(path) == false)
					throw ServerException("';' symbol must be at the end of 'error_page' list");
			}
			if (checkFile(path, _root) == false)
				throw ServerException("Invalid error file path : " + path);
			// a finir
		}
	}
}

void	Server::setLocations(std::string path, std::vector<std::string> content) {
	(void)content;
	(void)path;
	//a implementer
}