/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/08/04 14:47:06 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRes.hpp"

bool	HttpRes::_verbose = false;
std::map<std::string, std::string> HttpRes::_mimeTypes;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */

HttpRes::HttpRes(HttpReq &request) {

	if (HttpRes::_verbose)
		std::cout << "HttpRes default constructor called" << std::endl;
	_httpVersion = "";
	_statusCode = -1;
	_method = "";
	_statusMessage = "";
	_header.clear();
	_body = "";
	_formattedHeader = "";
	_server = request.getServer();
	_location = NULL;
	_keepAlive = true;
	_uriPath = "";
	_uriQuery = "";
	_resourceType = NORMALFILE;
	_contentLength = 0;
	_cgiPipeFd = -1;
	_cgiPid = -42;
	if (_mimeTypes.empty() == true)
		fillMimeTypes();
	handleRequest(request);
}

HttpRes::HttpRes(HttpRes const & copy) {

	*this = copy;
	if (HttpRes::_verbose)
		std::cout << "HttpRes copy constructor called" << std::endl;
}

HttpRes::~HttpRes(void) {

	if (HttpRes::_verbose)
		std::cout << "HttpRes destructor called" << std::endl;
	if (_location != NULL)
		delete _location;
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
		_uriPathInfo = httpres.getUriPathInfo();
		_uriQuery = httpres.getUriQuery();
		_resourceType = httpres.getResourceType();
		_location = httpres.getLocation();
		_contentLength = httpres.getContentLength();
		_cgiPipeFd = httpres.getCgiPipeFd();
		_cgiPid = httpres.getCgiPid();
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

std::string	HttpRes::getUriPathInfo() const {

	return (_uriPathInfo);
}

std::string HttpRes::getUriQuery() const {

	return (_uriQuery);
}

std::string	HttpRes::getScriptName() const {

	return (_script_name);
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

int	HttpRes::getCgiPipeFd() const {

	return (_cgiPipeFd);
}

pid_t	HttpRes::getCgiPid() const {

	return (_cgiPid);
}

void	HttpRes::setStatusCode(int statusCode)
{
	this->_statusCode = statusCode;
}

void	HttpRes::setCgiPipeFd(int cgiPipeFd)
{
	this->_cgiPipeFd = cgiPipeFd;
}

void	HttpRes::setCgiPid(int cgiPid)
{
	this->_cgiPid = cgiPid;
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

int	HttpRes::checkHttpVersion(std::string version) {

	if (version != "HTTP/1.0" && version != "HTTP/1.1" && version != "HTTP/0.9")
		return (505);
	_httpVersion = "HTTP/1.1";
	return (200);
}

int	HttpRes::checkRequestHeader(std::map<std::string, std::string> header) {

	std::map<std::string, std::string>::iterator it2;

	for (std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
	{
		if (it->first.length() > 70)
			return (431);
		it2 = it;
		it2++;
		for (; it2 != header.end(); it2++)
		{
			if (it2->first == it->first)
				return (400);
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
					std::ifstream is (_uriPath.c_str(), std::ifstream::binary);
  					if (is) {
    					is.seekg(0, is.end);
    					_contentLength = is.tellg();
						is.close();
					}
					else
						return (FORBIDDEN);
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
						std::ifstream is (_uriPath.c_str(), std::ifstream::binary);
  						if (is) {
    						is.seekg(0, is.end);
    						_contentLength = is.tellg();
							is.close();
						}
						else
							return (FORBIDDEN);
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
					std::ifstream is (_uriPath.c_str(), std::ifstream::binary);
  					if (is) {
    					is.seekg(0, is.end);
    					_contentLength = is.tellg();
						is.close();
					}
					else
						return (FORBIDDEN);
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
					std::ifstream is (_uriPath.c_str(), std::ifstream::binary);
  					if (is) {
    					is.seekg(0, is.end);
    					_contentLength = is.tellg();
						is.close();
					}
					else
						return (FORBIDDEN);
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

	size_t separator = uri.find('?');
	size_t end = uri.rfind('#');
	std::string tempPath;
	std::string locPath;
	Location *tempLoc = NULL;

	//stock dans _uriPathInfo le PATH_INFO, puis de supprime de uri
	size_t pos_py = uri.find(".py");
	size_t pos_php = uri.find(".php");
	if (pos_py == std::string::npos)
	{
		if (pos_php == std::string::npos)
			_uriPathInfo = "";
		else
		{
			if (separator != std::string::npos)
				_uriPathInfo = uri.substr(pos_php + 4, separator - pos_php - 4);
			else
				_uriPathInfo = uri.substr(pos_php + 4, uri.size());
			uri = uri.erase(pos_php + 4, _uriPathInfo.size());
		}
	}
	else
	{
		if (separator != std::string::npos)
			_uriPathInfo = uri.substr(pos_py + 3, separator - pos_py - 3);
		else
			_uriPathInfo = uri.substr(pos_py + 3, uri.size());
		uri = uri.erase(pos_py + 3, _uriPathInfo.size());
	}

	// parsing de l'_uriQuerry et de l'_uriPath
	separator = uri.find('?');
	end = uri.rfind('#');
	if (separator == std::string::npos)
		tempPath = uri;
	else
	{
		tempPath = uri.substr(0, separator);
		if (uri[separator + 1] != '\0' && end == std::string::npos)
			_uriQuery = uri.substr(separator + 1, uri.length() - (separator + 1));
		else if (uri[separator + 1] != '\0')
			_uriQuery = uri.substr(separator + 1, end - (separator + 1));
	}
	_script_name = tempPath;
	end = 0;
	std::vector<Location> locs = _server->getLocations();
	for (std::vector<Location>::iterator it = locs.begin(); it != locs.end(); it++)
	{
		locPath = (*it).getPath();
		if (tempLoc == NULL && locPath == "/")
		{
			tempLoc = &(*it);
			continue;
		}
		for (end = 0; tempPath[end] != '\0' && locPath[end] != '\0' && tempPath[end] == locPath[end]; end++)
		{
			;
		}
		if (locPath[end] == '\0' && (tempPath[end] == '\0' || tempPath[end] == '/'))
		{
			if (tempLoc == NULL || (tempLoc != NULL && tempLoc->getPath().length() < (*it).getPath().length()))
				tempLoc = &(*it);
		}
	}
	if (tempLoc != NULL)
		_location = new Location(*tempLoc);
	if (_location != NULL && _location->getRedirectionCode() > 0)
	{
		_resourceType = REDIRECTION;
		return (_location->getRedirectionCode());
	}
	if (_location == NULL || _location->getRoot() == _server->getRoot())
		_uriPath = _server->getRoot() + "/" + tempPath;
	else
	{
		std::string tempPath2 = tempPath.substr(_location->getPath().length(), tempPath.length() - _location->getPath().length());
		_uriPath = _location->getRoot() + "" + tempPath2;
	}
	std::cerr << _uriPathInfo << std::endl;
	_resourceType = checkResourceType();
	if (_resourceType == NOT_FOUND)
		return (404);
	else if (_resourceType == FORBIDDEN)
		return (403);
	else
		return (200);
}

void	HttpRes::checkIfAcceptable(std::vector<std::string> acceptable) {

	if (acceptable.empty())
		return ;
	for (std::vector<std::string>::iterator it = acceptable.begin(); it != acceptable.end(); it++)
	{
		if (getMimeType(_uriPath, _mimeTypes) == *it || (*it).find('*') != std::string::npos)
			return ;
	}
	_resourceType = NOT_ACCEPTABLE;
	_statusCode = 406;
}

void	HttpRes::bodyBuild(std::string requestUri) {

	if (_resourceType == REDIRECTION)
		_body = redirectionHTML(_statusCode, _statusMessage, _location->getRedirectionPath());
	else if (_resourceType == AUTOINDEX)
		_body = autoindexHTML(_uriPath, requestUri);
	else if (_method == "DELETE" && _statusCode == 204)
		_body = successfulDeleteHTML(_uriPath);
	else if (_statusCode != 200)
	{
		std::map<int, std::string> error_pages = _server->getErrorPages();
		if (error_pages.empty() || error_pages.find(_statusCode) == error_pages.end())
			_body = errorHTML(_statusCode, _statusMessage);
		else
			_body = getErrorPageContent(_server->getRoot() + "/" + error_pages[_statusCode], _statusCode, _statusMessage);
	}
	if (_body != "")
		_contentLength = _body.length();
	// autrement, _body reste vide, puisque le fichier est de taille imprevisible et potentiellement tres gros, ou bien c'est un CGI
}

void	HttpRes::headerBuild() {

	_header["Date:"] = timeStamp();
	_header["Server:"] = "Webserv/1.0";
	_header["Content-Length:"] = sizeToString(_contentLength);
	if (_keepAlive == true)
	{
		_header["Connection:"] = "keep-alive";
		_header["Keep-Alive:"] = "timeout=" + sizeToString(MAX_TIME_CLIENT_S);
	}
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
	if (_location == NULL)
	{
		_method = method;
		return (true);
	}
	else
	{
		std::vector<std::string> methods = _location->getMethods();
		for (std::vector<std::string>::iterator it = methods.begin(); it != methods.end(); it++)
		{
			if (method == *it)
			{
				_method = method;
				return (true);
			}
		}
	}
	return (false);
}

void	HttpRes::uploadFileToServer(std::string tempFile, std::string boundary) {

	if (_location == NULL || _location->getUploadDir() == "")
	{
			_statusCode = 403;
			return ;
	}
	else
	{
		std::string uploadDir = _location->getUploadDir();
		struct stat test;
		if (stat(uploadDir.c_str(), &test) != 0 || !S_ISDIR(test.st_mode) || access(uploadDir.c_str(), R_OK | W_OK) != 0)
		{
			_statusCode = 403;
			return ;
		}
		std::ifstream tmpFile;
		std::ofstream output;
		std::string line;
		std::string fileName = "";
		std::string tempName = "";
		std::string finalDest = "";
		bool	header = false;
		size_t	pos;
		size_t	pos2;
		tmpFile.open(tempFile.c_str());
		if (tmpFile.fail())
		{
			_statusCode = 403;
			return ;
		}
		while (getline(tmpFile, line))
		{
			if (line.find(boundary) != std::string::npos)
				header = true;
			else if (header == true && line == "\r")
				header = false;
			else if (header == true)
			{
				pos = line.find("filename=");
				if (pos != std::string::npos)
				{
					pos += 10;
					for (pos2 = pos; line[pos2] != '"'; pos2++)
						;
					tempName = line.substr(pos, pos2 - pos);
					if (fileName == "" || tempName != fileName)
					{
						if (fileName != "")
							output.close();
						fileName = tempName;
						finalDest = uploadDir + "/" + fileName;
						output.open(finalDest.c_str());
						if (output.fail())
						{
							_statusCode = 403;
							tmpFile.close();
							return ;
						}
					}
					else
						continue;
				}
				else
					continue;
			}
			else
			{
				if (fileName != "" && line != "\r")
					output << line << std::endl;
			}
		}
		if (fileName != "")
			output.close();
		tmpFile.close();
		_statusCode = 201;
		std::remove(tempFile.c_str());
	}
}

void	HttpRes::handleRequest(HttpReq &request) {

	_statusCode = checkHttpVersion(request.getHttpVersion());
	if (_statusCode == 200 && request.body_is_too_big() == true)
		_statusCode = 413;
	if (_statusCode == 200)
		_statusCode = checkRequestHeader(request.getHeader());
	if (_statusCode == 200)
		_statusCode = checkUri(request.getUri());
	if (_statusCode == 200 && methodIsAllowed(request.getMethod()) == false)
		_statusCode = 405;
	if (_statusCode == 200 && _method == "GET" && _resourceType == NORMALFILE)
		checkIfAcceptable(request.getAccept());
	else if (_statusCode == 200 && _method == "DELETE")
	{
		if (std::remove(_uriPath.c_str()) != 0)
			_statusCode = 400;
		else
			_statusCode = 204;
	}
	else if (_statusCode == 200 && (_resourceType == PHP || _resourceType == PYTHON))
	{
		CGI cgi(request, *this);
		cgi.execCGI();
		return ;
	}
	else if (_statusCode == 200 && _method == "POST")
		uploadFileToServer(request.getBodyTmpFile(), request.getBoundary());
	_statusMessage = getStatus(_statusCode);
	bodyBuild(request.getUri());
	if (request.getKeepAlive() == false)
		_keepAlive = false;
	headerBuild();
	formatHeader();
}
