#include "Server.hpp"

Server::Server(void) {

	_port = 0;
	_host = 0;
	_server_name = "";
	_root = "";
	_client_max_body_size = 0;
	_index = "";
	_listen = 0;
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
	_listen = rhs.getListen();
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

struct sockaddr_in	Server::getAddress(void) const {

	return (_address);
}

int	Server::getListen(void) const {

	return (_listen);
}

void	Server::setPort(std::string &port) {
	(void)port;
}

void	Server::setHost(std::string &host) {
	(void)host;
}

void 	Server::setServerName(std::string &name) {
	(void)name;
}

void	Server::setRoot(std::string &root) {
	(void)root;
}

void	Server::setClientMaxBodySize(std::string &clientMaxBodySize) {
	(void)clientMaxBodySize;
}

void	Server::setIndex(std::string &index) {
	(void)index;
}

void	Server::setErrorPages(std::vector<std::string> &errorPages) {
	(void)errorPages;
}

void	Server::setLocations(std::string &path, std::vector<std::string> &content) {
	(void)content;
	(void)path;
}

void	Server::setAddress(std::string &address) {
	(void)address;
}