/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/20 12:18:58 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRes.hpp"

bool	HttpRes::_verbose = false;
std::map<std::string, std::string> HttpRes::_mimeTypes;
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
	_formattedHeader = "";
	_server = NULL;
	_location = NULL;
	_keepAlive = true;
	_uriPath = "";
	_uriQuery = "";
	_resourceType = NORMALFILE;
	_contentLength = 0;
	if (_mimeTypes.empty() == true)
		fillMimeTypes();
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
		_formattedHeader = httpres.getFormattedHeader();
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

std::string	HttpRes::getFormattedHeader() const {

	return (_formattedHeader);
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


void	HttpRes::fillMimeTypes() {

	_mimeTypes[".aac"] = "audio/aac";
	_mimeTypes[".abw"] = "application/x-abiword";
	_mimeTypes[".arc"] = "application/x-freearc";
	_mimeTypes[".avif"] = "image/avif";
	_mimeTypes[".avi"] = "video/x-msvideo";
	_mimeTypes[".azw"] = "application/vnd.amazon.ebook";
	_mimeTypes[".bin"] = "application/octet-stream";
	_mimeTypes[".bmp"] = "image/bmp";
	_mimeTypes[".bz"] = "application/x-bzip";
	_mimeTypes[".bz2"] = "application/x-bzip2";
	_mimeTypes[".cda"] = "application/x-cdf";
	_mimeTypes[".csh"] = "application/x-csh";
	_mimeTypes[".css"] = "text/css";
	_mimeTypes[".csv"] = "text/csv";
	_mimeTypes[".doc"] = "application/msword";
	_mimeTypes[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
	_mimeTypes[".eot"] = "application/vnd.ms-fontobject";
	_mimeTypes[".epub"] = "application/epub+zip";
	_mimeTypes[".gz"] = "application/gzip";
	_mimeTypes[".gif"] = "image/gif";
	_mimeTypes[".htm"] = "text/html";
	_mimeTypes[".html"] = "text/html";
	_mimeTypes[".ico"] = "image/vnd.microsoft.icon";
	_mimeTypes[".ics"] = "text/calendar";
	_mimeTypes[".jar"] = "application/java-archive";
	_mimeTypes[".jpg"] = "image/jpeg";
	_mimeTypes[".jpeg"] = "image/jpeg";
	_mimeTypes[".js"] = "text/javascript";
	_mimeTypes[".json"] = "application/json";
	_mimeTypes[".jsonld"] = "application/ld+json";
	_mimeTypes[".mid"] = "audio/midi";
	_mimeTypes[".midi"] = "audio/midi";
	_mimeTypes[".mjs"] = "text/javascript";
	_mimeTypes[".mp3"] = "audio/mpeg";
	_mimeTypes[".mp4"] = "video/mp4";
	_mimeTypes[".mpeg"] = "video/mpeg";
	_mimeTypes[".mpkg"] = "application/vnd.apple.installer+xml";
	_mimeTypes[".odp"] = "application/vnd.oasis.opendocument.presentation";
	_mimeTypes[".ods"] = "application/vnd.oasis.opendocument.spreadsheet";
	_mimeTypes[".odt"] = "application/vnd.oasis.opendocument.text";
	_mimeTypes[".oga"] = "audio/ogg";
	_mimeTypes[".ogv"] = "video/ogg";
	_mimeTypes[".ogx"] = "application/ogg";
	_mimeTypes[".opus"] = "audio/opus";
	_mimeTypes[".otf"] = "font/otf";
	_mimeTypes[".png"] = "image/png";
	_mimeTypes[".pdf"] = "application/pdf";
	_mimeTypes[".php"] = "application/x-httpd-php";
	_mimeTypes[".ppt"] = "application/vnd.ms-powerpoint";
	_mimeTypes[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
	_mimeTypes[".py"] = "application/x-python-code";
	_mimeTypes[".rar"] = "application/vnd.rar";
	_mimeTypes[".rtf"] = "application/rtf";
	_mimeTypes[".sh"] = "application/x-sh";
	_mimeTypes[".svg"] = "image/svg+xml";
	_mimeTypes[".tar"] = "application/x-tar";
	_mimeTypes[".tif"] = "image/tiff";
	_mimeTypes[".tiff"] = "image/tiff";
	_mimeTypes[".ts"] = "video/mp2t";
	_mimeTypes[".ttf"] = "font/ttf";
	_mimeTypes[".txt"] = "text/plain";
	_mimeTypes[".vsd"] = "application/vnd.visio";
	_mimeTypes[".wav"] = "audio.wav";
	_mimeTypes[".weba"] = "audio/webm";
	_mimeTypes[".webm"] = "video/webm";
	_mimeTypes[".webp"] = "image/webp";
	_mimeTypes[".woff"] = "font/woff";
	_mimeTypes[".woff2"] = "font/woff2";
	_mimeTypes[".xhtml"] = "application/xhtml+xml";
	_mimeTypes[".xls"] = "application/vnd.ms-excel";
	_mimeTypes[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
	_mimeTypes[".xml"] = "application/xml";
	_mimeTypes[".xul"] = "application/vnd.mozilla.xul+xml";
	_mimeTypes[".zip"] = "application/zip";
	_mimeTypes[".3gp"] = "audio/3gpp";
	_mimeTypes[".3g2"] = "audio/3gpp2";
	_mimeTypes[".7z"] = "application/x-7z-compressed";
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
			size_t len = _uriPath.length();
			if (_location != NULL && _location->getCgiBool() == true)
			{
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

void	HttpRes::checkIfAcceptable(std::vector<std::string> acceptable) {

	for (std::vector<std::string>::iterator it = acceptable.begin(); it != acceptable.end(); it++)
	{
		if (getMimeType(_uriPath, _mimeTypes) == *it || (*it).find('*') != std::string::npos)
			return ;
	}
	_resourceType = NOT_ACCEPTABLE;
	_statusCode = 406;
}

void	HttpRes::bodyBuild() {

	if (_resourceType == REDIRECTION)
		_body = redirectionHTML(_statusCode, _statusMessage, _location->getRedirectionPath());
	else if (_resourceType == AUTOINDEX)
		_body = autoindexHTML(_uriPath);
	else if (_method == "DELETE" && _statusCode == 200)
		_body = successfulDeleteHTML(_uriPath);
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
		_header["Content-Type:"] = getMimeType(_uriPath, _mimeTypes);
}

void	HttpRes::formatHeader() {

	std::stringstream response;
	response << _httpVersion << " " << _statusCode << " " << _statusMessage << "\r\n";
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
		response << it->first << " " << it->second << "\r\n";
	response << "\r\n";
	_formattedHeader = response.str();
}

bool	HttpRes::methodIsAllowed(std::string method) {

	if (method != "GET" && method != "POST" && method != "DELETE")
		return (false);
	if (_location != NULL)
	{
		std::vector<std::string> methods = _location->getMethods();
		for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
		{
			if (method == *it)
				return (true);
		}
	}
	return (false);
}

void	HttpRes::handleRequest(HttpReq &request, std::vector<Server *> servers) {
	
	bool	error = false;
	
	setServer(request.getHost(), servers);
	_statusCode = checkHttpVersion(request.getHttpVersion(), error);
	if (error == false && _server->getClientMaxBodySize() != 0)
		_statusCode = checkRequestBodySize(_server->getClientMaxBodySize(), request.getBody().size(), error);
	if (error == false)
		_statusCode = checkRequestHeader(request.getHeader(), error);
	if (error == false)
		_statusCode = checkUri(request.getUri());
	if (_statusCode == 200 && methodIsAllowed(request.getMethod()) == false)
		_statusCode = 405;
	if (_statusCode == 200 && _method == "GET" && _resourceType == NORMALFILE)
		checkIfAcceptable(request.getAccept());
	else if (_statusCode == 200 && _method == "DELETE")
	{
		if (std::remove(_uriPath.c_str()) != 0)
			_statusCode = 400;
	}
	else if (_statusCode == 200 && (_resourceType == PYTHON || _resourceType == PHP))
		;//fonction de gestion de CGI ici
	_statusMessage = getStatus(_statusCode);
	bodyBuild();
	if (request.getKeepAlive() == false)
		_keepAlive = false;
	headerBuild();
	formatHeader();
}