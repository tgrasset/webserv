/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/12 15:48:32 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/12 10:03:29 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool	Server::_verbose = false;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Server::Server(void) 
{
	if (Server::_verbose)
		std::cout << "Server default constructor called" << std::endl;
	_port = 0;
	_host = 0;
	_serverName = "";
	_root = "";
	_clientMaxBodySize = 0;
	_index = "";
	_errorPages.clear();
	_listenSocket = 0;
	_timeout = -1;
}

Server::Server(Server const &src) 
{
	if (Server::_verbose)
		std::cout << "Server copy constructor called" << std::endl;
	if (this != &src)
        *this = src;
}

Server::~Server() 
{
	if (Server::_verbose)
		std::cout << "Server destructor called" << std::endl;
}

/* ************************************************************************** */
/*                     Surcharge d'operateur                                  */
/* ************************************************************************** */
Server &Server::operator=(Server const &rhs) 
{
	if (this != &rhs)
	{
		_port = rhs.getPort();
		_host = rhs.getHost();
		_servaddr = rhs.getServerAddr();
		_serverName = rhs.getServerName();
		_root = rhs.getRoot();
		_clientMaxBodySize = rhs.getClientMaxBodySize();
		_index = rhs.getIndex();
		_errorPages = rhs.getErrorPages();
		_locations = rhs.getLocations();
		_listenSocket = rhs.getListenSocket();
		_timeout = rhs.getTimeout();
	}
    return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */
in_port_t	Server::getPort(void) const {

	return (_port);
}

in_addr_t	Server::getHost(void) const {

	return (_host);
}

std::string 	Server::getServerName(void) const {

	return (_serverName);
}

std::string		Server::getRoot(void) const {

	return (_root);
}

unsigned long	Server::getClientMaxBodySize(void) const {

	return (_clientMaxBodySize);
}

std::string	Server::getIndex(void) const {

	return (_index);
}

std::map<int, std::string>	Server::getErrorPages(void) const {

	return (_errorPages);
}

std::vector<Location>	Server::getLocations(void) const {

	return (_locations);
}

int	Server::getListenSocket(void) const {

	return (_listenSocket);
}

struct sockaddr_in	Server::getServerAddr(void) const {

	return (_servaddr);
}

int	Server::getTimeout(void) const {

	return (_timeout);
}

void	Server::setPort(std::string port) {
	
	if (isValidConfValue(port) == false)
		throw ServerException("';' symbol needed after 'listen' line");
	int n = stringToInt(port);
	if (n < 1 || n > 65535)
		throw ServerException("Invalid port value in 'listen' line");
	u_int16_t nShort = n;
	_port = htons(nShort);
}

void	Server::setListenSocket(int listen_socket)
{
	_listenSocket = listen_socket;
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
		_serverName = name;
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
	char buffer[4096];
	if (getcwd(buffer, 4096) == false)
		throw ServerException("getcwd error");
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
	_clientMaxBodySize = n;
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
	int code;
	std::string path;
	std::map<int, std::string>::iterator it;

	if (size % 2 != 0)
		throw ServerException("Listing after 'error_page' directive must follow pattern : <error_number> <file_path>");
	for (size_t i = 0 ; i < size ; i++)
	{
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
			it = _errorPages.find(code);
			if (it == _errorPages.end())
				_errorPages.insert(std::make_pair(code, path));
			else
				_errorPages[code] = path;
		}
	}
}

void	Server::setLocation(std::string path, std::vector<std::string> content) {
	
	Location loc;
	std::vector<std::string> methods;
	std::vector<std::string> cgiInfo;
	size_t size = content.size();
	bool autoindexAlreadySet = false;

	loc.setPath(path);
	if (path == "/cgi-bin")
		loc.setCgiBool(true);
	for (size_t i = 0; i < size; i++)
	{
		if (content[i] == "root" && i + 1 < size)
		{
			if (loc.getRoot() != "")
				throw ServerException("Each 'location' context can't have more than one 'root' directive");
			loc.setRoot(content[++i], _root);
		}
		else if (content[i] == "allow_methods" && i + 1 < size)
		{
			if (loc.getMethods().empty() == false)
				throw ServerException("Each 'location' context can't have more than one 'allow_methods' directive");
			while (++i < size)
			{
				if (content[i].find(";") != std::string::npos)
				{
					if (isValidConfValue(content[i]) == false)
						throw ServerException("';' symbol needed after 'allow_methods' line in 'location' context");
					methods.push_back(content[i]);
					break;
				}
				else
					methods.push_back(content[i]);
				if (i + 1 == size)
					throw ServerException("';' symbol needed after 'allow_methods' line in 'location' context");
			}
			loc.setMethods(methods);
		}
		else if (content[i] == "autoindex" && i + 1 < size)
		{
			if (autoindexAlreadySet == true)
				throw ServerException("Each 'location' context can't have more than one 'autoindex' directive");
			if (loc.getCgiBool() == true)
				throw ServerException("'autoindex' directive is incompatible with cgi-bin 'location'");
			i++;
			if (content[i] == "on;")
				loc.setAutoIndex(true);
			else if (content[i] != "off;")
				throw ServerException("autoindex must be set to 'on' or 'off'");
			autoindexAlreadySet = true;
		}
		else if(content[i] == "index" && i + 1 < size)
		{
			if (loc.getIndex() != "")
				throw ServerException("Each 'location' context can't have more than one 'index' directive");
			loc.setIndex(content[++i]);
		}
		else if (content[i] == "return" && i + 1 < size)
		{
			if (loc.getRedirectionCode() != 0)
				throw ServerException("Each 'location' context can't have more than one 'return' directive");
			if (loc.getCgiBool() == true)
				throw ServerException("'return' directive is incompatible with cgi-bin 'location'");
			i++;
			int code = stringToInt(content[i]);
			if (code != 300 && code != 301 && code != 302 && code != 303 && code != 304 && code != 307 && code != 308)
				throw ServerException("Unknown redirection code after 'return' directive : " + content[i]);
			loc.setRedirectionCode(code);
			i++;
			if (isValidConfValue(content[i]) == false)
				throw ServerException("';' symbol needed after 'return' line in 'location' context");
			loc.setRedirectionPath(content[i]);	
		}
		else if (content[i] == "cgi" && i + 1 < size)
		{
			i++;
            while (i < size)
            {
                cgiInfo.push_back(content[i]);
                if (content[i].find(';') != std::string::npos)
                    break ;
                i++;
            }
            if (i == size)
                throw ServerException("Missing ';' symbol after 'cgi' line \n" + static_cast<std::string>(strerror(errno)));
		}
		else if (content[i] == "upload_dir" && i + 1 < size)
		{
			if (loc.getUploadDir() != "")
				throw ServerException("Each 'location' context can't have more than one 'upload_dir' directive");
			i++;
			if (isValidConfValue(content[i]) == false)
				throw ServerException("';' symbol needed after 'upload_dir' line in 'location' context");
			loc.setUploadDir(content[i]);
		}
		else
			throw ServerException("Unknown or incomplete directive in 'location' context : " + content[i]
									+ "\n" + static_cast<std::string>(strerror(errno)));
	}
	if (loc.getCgiBool() == true)
		loc.setCgiExtensionAndPath(cgiInfo);
	loc.checkConfig(_root);
	_locations.push_back(loc);
}

void	Server::setServaddr(void)
{
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = _host;
	_servaddr.sin_port = _port;
}

void	Server::setTimeout(std::string timeout) {

	if (isValidConfValue(timeout) == false)
		throw ServerException("';' symbol needed after 'timeout' line");
	int n = stringToInt(timeout);
	if (n < 1 || n > 120)
		throw ServerException("Invalid value in 'timeout' line. Please set timeout between 1 and 120 seconds");
	_timeout = n;
}

void	Server::bind_server(void)
{
	if (bind(this->_listenSocket, (struct sockaddr *)&(this->_servaddr), sizeof(this->_servaddr)) < 0)
		throw ServerException("bind: " + static_cast<std::string>(strerror(errno)));
}

void	Server::checkDoubleLocations(void) {

	if (_locations.empty())
		return ;
	size_t i, j;
	for (i = 0; i < _locations.size() - 1; i++)
	{
		for (j = i + 1; j < _locations.size(); j++)
		{
			if (_locations[i].getPath() == _locations[j].getPath())
				throw ServerException("Two 'location' contexts can't be defined for the same path : " 
				+ _locations[i].getPath() + "\n" + static_cast<std::string>(strerror(errno)));
		}
	}
}