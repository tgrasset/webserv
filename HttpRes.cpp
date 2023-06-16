/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/16 14:05:53 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRes.hpp"

bool	HttpRes::_verbose = false;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */

HttpRes::HttpRes(HttpReq &request, std::vector<Server *> servers) {
	
	if (HttpRes::_verbose)
		std::cout << "HttpRes default constructor called" << std::endl;
	_httpVersion = "";
	_statusCode = -1;
	_method = "";
	_statusMessage = "";
	_header.clear();
	_body = "";
	_toSend.clear();
	_server = NULL;
	_location = NULL;
	_keepAlive = true;
	_uriPath = "";
	_uriQuery = "";
	_resourceType = NORMALFILE;
	_contentLength = 0;
	// if (_mimeTypes.empty() == true)
	// {
	// 	//remplir map
	// }
	handleRequest(request, servers);
}

HttpRes::HttpRes(HttpRes const & copy) {
	
	*this = copy;
	if (HttpRes::_verbose)
		std::cout << "HttpRes copy constructor called" << std::endl;
}

HttpRes::~HttpRes(void) {
	
	if (HttpRes::_verbose)
		std::cout << "HttpRes destructor called" << std::endl;
}

/* ************************************************************************** */
/*                     Surcharge d'operateur                                  */
/* ************************************************************************** */

HttpRes	& HttpRes::operator=(HttpRes const & httpres)
{
	if (this != &httpres)
	{
		_httpVersion = httpres.getHttpVersion();
		_statusCode = httpres.getStatusCode();
		_method = httpres.getMethod();
		_statusMessage = httpres.getStatusMessage();
		_header = httpres.getHeader();
		_body = httpres.getBody();
		_toSend = httpres.getToSend();
		_server = httpres.getServer();
		_keepAlive = httpres.getKeepAlive();
		_uriPath = httpres.getUriPath();
		_uriQuery = httpres.getUriQuery();
		_resourceType = httpres.getResourceType();
		_location = httpres.getLocation();
		_contentLength = httpres.getContentLength();
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */

std::string HttpRes::getHttpVersion() const {
	
	return(_httpVersion);
}

int	HttpRes::getStatusCode() const {

	return (_statusCode);
}

std::string	HttpRes::getMethod() const {

	return (_method);
}

std::string	HttpRes::getStatusMessage() const {

	return (_statusMessage);
}

std::map<std::string, std::string> HttpRes::getHeader() const {

	return (_header);
}

std::string	HttpRes::getBody() const {
	
	return (_body);
}
Server	*HttpRes::getServer() const {

	return (_server);
}

std::string	HttpRes::getToSend() const {

	return (_toSend);
}

bool	HttpRes::getKeepAlive() const {

	return (_keepAlive);
}

std::string	HttpRes::getUriPath() const {

	return (_uriPath);
}

std::string HttpRes::getUriQuery() const {

	return (_uriQuery);
}

r_type	HttpRes::getResourceType() const {

	return (_resourceType);
}

Location	*HttpRes::getLocation() const {

	return (_location);
}

size_t	HttpRes::getContentLength() const {
	
	return (_contentLength);
}

void	HttpRes::setServer(std::string reqHost, std::vector<Server *> servers) {

	if (servers.size() == 1)
	{
		_server = servers[0];
		return ;
	}
	std::string hostname;
	if (reqHost.find(':') != std::string::npos)
		hostname = reqHost.substr(0, reqHost.length() - reqHost.find(':'));
	else
		hostname = reqHost;
	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if ((*it)->getServerName() == reqHost)
		{
			_server = *it;
			return;
		}
	}
	_server = servers[0];
}

int	HttpRes::checkHttpVersion(std::string version, bool &error) {

	if (version != "HTTP/1.0" && version != "HTTP/1.1" && version != "HTTP/0.9")
	{
		error = true;
		return (505);
	}
	_httpVersion = "HTTP/1.1";
	return (200);
}

int	HttpRes::checkMethod(std::string method, bool &error) {

	if (method != "GET" && method != "POST" && method != "DELETE")
	{
		error = true;
		return (405);
	}
	_method = method;
	return (200);
}

int	HttpRes::checkRequestBodySize(int maxSize, int bodySize, bool &error) {

	if (bodySize > maxSize)
	{
		error = true;
		return (413);
	}
	return (200);
}

int	HttpRes::checkRequestHeader(std::map<std::string, std::string> header, bool &error) {

	std::map<std::string, std::string>::iterator it2;
	
	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
	{
		if (it->first.length() > 70)
		{
			error = true;
			return (431);
		}
		it2 = it;
		it2++;
		for (; it2 != header.end(); it2++)
		{
			if (it2->first == it->first)
			{
				error = true;
				return (400);
			}
		}
	}
	return (200);
}

r_type HttpRes::checkResourceType() {

	
	struct stat test;
	if (access(_uriPath.c_str(), F_OK) != 0)
		return (NOT_FOUND);
	else if (access(_uriPath.c_str(), R_OK) != 0)
		return (FORBIDDEN);
	if (stat(_uriPath.c_str(), &test) == 0)
	{
		if (test.st_mode & S_IFDIR)
		{
			if (_location == NULL)
			{
				_uriPath += _server->getIndex();
				if (access(_uriPath.c_str(), F_OK) != 0)
					return (NOT_FOUND);
				else if (access(_uriPath.c_str(), R_OK) != 0)
					return (FORBIDDEN);
				else
				{
					_contentLength = test.st_size;
					return (NORMALFILE);
				}
			}
			else
			{
				if ((_location->getCgiBool() == true || _location->getAutoIndex() == false) && _location->getIndex() == "")
					return (FORBIDDEN);
				else if (_location->getAutoIndex() == true && _location->getIndex() == "")
					return (AUTOINDEX);
				else
				{
					_uriPath += _location->getIndex();
					if (access(_uriPath.c_str(), F_OK) != 0)
						return (NOT_FOUND);
					else if (access(_uriPath.c_str(), R_OK) != 0)
						return (FORBIDDEN);
					else
					{
						_contentLength = test.st_size;
						return (NORMALFILE);
					}
				}
			}
		}
		else if (test.st_mode & S_IFREG)
		{
			if (_location != NULL && _location->getCgiBool() == true)
			{
				size_t len = _uriPath.length();
				if (len > 3 && _uriPath[len - 1] == 'y' && _uriPath[len - 2] == 'p' && _uriPath[len - 3] == '.')
					return (PYTHON);
				else if (len > 4 && _uriPath[len - 1] == 'p' && _uriPath[len - 2] == 'h' && _uriPath[len - 3] == 'p' && _uriPath[len - 4] == '.')
					return (PHP);
				else
				{
					_contentLength = test.st_size;
					return (NORMALFILE);
				}
			}
			else
			{
				size_t len = _uriPath.length();
				if (len > 3 && _uriPath[len - 1] == 'y' && _uriPath[len - 2] == 'p' && _uriPath[len - 3] == '.')
					return (FORBIDDEN);
				else if (len > 4 && _uriPath[len - 1] == 'p' && _uriPath[len - 2] == 'h' && _uriPath[len - 3] == 'p' && _uriPath[len - 4] == '.')
					return (FORBIDDEN);
				else
				{
					_contentLength = test.st_size;
					return (NORMALFILE);
				}
			}
		}
		else
			return (FORBIDDEN);
	}
	else
		return (FORBIDDEN);
}

int	HttpRes::checkUri(std::string uri) {
	
	size_t questionMark = uri.find('?');
	size_t end = uri.rfind('#');
	std::string tempPath;

	if (questionMark == std::string::npos)
		tempPath = uri;
	else
	{
		tempPath = uri.substr(0, questionMark);
		if (uri[questionMark + 1] != '\0' && end == std::string::npos)
			_uriQuery = uri.substr(questionMark + 1, uri.length() - (questionMark + 1));
		else if (uri[questionMark + 1] != '\0')
			_uriQuery = uri.substr(questionMark + 1, end - (questionMark + 1));
	}
	std::vector<Location> locs = _server->getLocations();
	for (std::vector<Location>::iterator it = locs.begin(); it != locs.end(); it++)
	{
		if (tempPath.find((*it).getPath()) == 0)
		{
			_location = &(*it);
			break;
		}
	}
	if (_location != NULL && _location->getRedirectionCode() > 0)
	{
		_resourceType = REDIRECTION;
		return (_location->getRedirectionCode());
	}
	if (_location == NULL || _location->getRoot() == _server->getRoot())
		_uriPath = _server->getRoot() + "/" + tempPath;
	else 
		_uriPath = _location->getRoot() + "/" + tempPath;
	_resourceType = checkResourceType();
	if (_resourceType == NOT_FOUND)
		return (404);
	else if (_resourceType == FORBIDDEN)
		return (403);
	else
		return (200);
}

void	HttpRes::bodyBuild() {

	if (_resourceType == REDIRECTION)
		_body = redirectionHTML(_statusCode, _statusMessage, _location->getRedirectionPath());
	else if (_resourceType == AUTOINDEX)
		_body = autoindexHTML(_uriPath);
	else if (_statusCode != 200)
	{
		std::map<int, std::string> error_pages = _server->getErrorPages();
		if (error_pages.empty() || error_pages.find(_statusCode) == error_pages.end())
			_body = errorHTML(_statusCode, _statusMessage);
		else
			_body = getErrorPageContent(error_pages[_statusCode], _statusCode, _statusMessage);
	}
	if (_body != "")
		_contentLength = _body.length();
	// autrement, _body reste vide, puisque le fichier est de taille imprevisible et potentiellement tres gros, ou bien c'est un CGI
}

void	HttpRes::headerBuild() {
	
	_header["Date:"] = timeStamp();
	_header["Server:"] = "Webserv/1.0";
	if (_resourceType != PYTHON && _resourceType != PHP)
		_header["Content-Length:"] = sizeToString(_contentLength);
	if (_keepAlive == true)
		_header["Connection:"] = "keep-alive";
	else
		_header["Connection:"] = "close";
	if (_resourceType != NORMALFILE)
		_header["Content-Type:"] = "text/html";
	else
		_header["Content-Type:"] = getMimeType(_uriPath);
}

void	HttpRes::formatResponse() {
	
	std::stringstream response;
	response << _httpVersion << " " << _statusCode << " " << _statusMessage << "\r\n";
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
		response << it->first << " " << it->second << "\r\n";
	response << "\r\n";
	response << _body;
	_toSend = response.str();
}

void	HttpRes::handleRequest(HttpReq &request, std::vector<Server *> servers) {
	
	bool	error = false;
	
	setServer(request.getHost(), servers);
	_statusCode = checkHttpVersion(request.getHttpVersion(), error);
	if (error == false)
		_statusCode = checkMethod(request.getMethod(), error);
	if (error == false && _server->getClientMaxBodySize() != 0)
		_statusCode = checkRequestBodySize(_server->getClientMaxBodySize(), request.getBody().size(), error);
	if (error == false)
		_statusCode = checkRequestHeader(request.getHeader(), error);
	if (error == false)
		_statusCode = checkUri(request.getUri());
	_statusMessage = getStatus(_statusCode);
	bodyBuild();
	if (request.getKeepAlive() == false)
		_keepAlive = false;
	headerBuild();
	formatResponse();
}