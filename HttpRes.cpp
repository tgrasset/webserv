/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/12 10:14:16 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRes.hpp"

std::map<std::string, std::string> HttpRes::_mimeTypes;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */

HttpRes::HttpRes(Client * client, HttpReq *request) {

	_client = client;
	_request = request;
	_httpVersion = "";
	_statusCode = -1;
	_method = "";
	_statusMessage = "";
	_header.clear();
	_body = "";
	_formattedHeader = "";
	_server = request->getServer();
	if (request->getLocation() == NULL)
		_location = NULL;
	else
	{
		_location = new Location(*request->getLocation());
		if (_location == NULL)
			throw HttpResException("New error in HttpRes");
	}
	_keepAlive = true;
	_uriPath = "";
	_uriQuery = "";
	_resourceType = NORMALFILE;
	_contentLength = 0;
	_cgiFilePath = "";
	_cgiPipeFd = -1;
	_cgiPid = -42;
	_fileToSendFd = -1;
	_fileToSendBuff.clear();
	_statusFileToSend = CLOSE;
	_cgiBuff.clear();
	_cgiBuff_header.clear();
	_statusCgiPipe = PIPE_CLOSE;
	_uploadTmpInFd = -1;
	_uploadOutFd = -1;
	_uploadBuff = "";
	_nameTmpUploadFile = "";
	_nameFinalUploadFile = "";
	_uploadFileHeader = false;
	_uploadFileBody = false;
	_uploadBuffClean = true;
	_backslashRFound = false;
	_uploadFileBodyFirstLine = false;
	_byteWrittenUploadFile = 0;
	_percentWrittenUploadFile = 0;
	_first_line_of_header = true;
	_is_cgi = false;

	if (_mimeTypes.empty() == true)
		fillMimeTypes();
	handleRequest();
}

HttpRes::HttpRes(HttpRes const & copy) {

	*this = copy;
}

HttpRes::~HttpRes(void) {

	if (_location != NULL)
		delete _location;
	closeBodyFile();
	closeCgiPipe();
	if (_cgiPid != -42)
	{
		if (kill(this->_cgiPid, SIGKILL) == 0)
			std::cout << TXT_BL << getTimestamp() << "Client " << this->_client->getId() << ":	Killing the CGI process"<<  TXT_END << std::endl;
	}
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
		_cgiFilePath = httpres._cgiFilePath;
		_cgiPipeFd = httpres._cgiPipeFd;
		_cgiPid = httpres._cgiPid;
		_fileToSendFd = httpres._fileToSendFd;
		_fileToSendBuff = httpres._fileToSendBuff;
		_client = httpres._client;
		_statusFileToSend = httpres._statusFileToSend;
		_cgiBuff = httpres._cgiBuff;
		_cgiBuff_header = httpres._cgiBuff_header;
		_statusCgiPipe = httpres._statusCgiPipe;
		_uploadTmpInFd = httpres._uploadTmpInFd;
		_uploadOutFd = httpres._uploadOutFd;
		_uploadBuff = httpres._uploadBuff;
		_nameTmpUploadFile = httpres._nameTmpUploadFile;
		_uploadFileHeader = httpres._uploadFileHeader;
		_uploadFileBody = httpres._uploadFileBody;
		_nameFinalUploadFile = httpres._nameFinalUploadFile;
		_uploadBuffClean = httpres._uploadBuffClean;
		_uploadFileBodyFirstLine = httpres._uploadFileBodyFirstLine;
		_backslashRFound = httpres._backslashRFound;
		_byteWrittenUploadFile = httpres._byteWrittenUploadFile;
		_percentWrittenUploadFile = httpres._percentWrittenUploadFile;
		_first_line_of_header = httpres._first_line_of_header;
		_is_cgi = httpres._is_cgi;
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */

std::string HttpRes::getHttpVersion(void) const {

	return(_httpVersion);
}

int	HttpRes::getStatusCode(void) const {

	return (_statusCode);
}

std::string	HttpRes::getMethod(void) const {

	return (_method);
}

std::string	HttpRes::getStatusMessage(void) const {

	return (_statusMessage);
}

std::map<std::string, std::string> HttpRes::getHeader(void) const {

	return (_header);
}

std::string	HttpRes::getBody(void) const {

	return (_body);
}
Server	*HttpRes::getServer(void) const {

	return (_server);
}

std::string	HttpRes::getFormattedHeader(void) const {

	return (_formattedHeader);
}

bool	HttpRes::getKeepAlive(void) const {

	return (_keepAlive);
}

std::string	HttpRes::getUriPath(void) const {

	return (_uriPath);
}

std::string	HttpRes::getUriPathInfo(void) const {

	return (_uriPathInfo);
}

std::string HttpRes::getUriQuery(void) const {

	return (_uriQuery);
}

std::string	HttpRes::getScriptName(void) const {

	return (_scriptName);
}

r_type	HttpRes::getResourceType(void) const {

	return (_resourceType);
}

Location	*HttpRes::getLocation(void) const {

	return (_location);
}

size_t	HttpRes::getContentLength(void) const {

	return (_contentLength);
}

std::string	HttpRes::getCgiFilePath(void) const {

	return (_cgiFilePath);
}

int		HttpRes::getFileToSendFd(void) const
{
	return (this->_fileToSendFd);
}

std::vector<char>	HttpRes::getFileToSendBuff(void) const
{
	return (_fileToSendBuff);
}

size_t	HttpRes::getFileToSendSize(void) const {

	return (_fileToSendSize);
}

s_file	HttpRes::getStatusFileToSend(void) const
{
	return (this->_statusFileToSend);
}

int		HttpRes::getCgiPipeFd(void) const {

	return (this->_cgiPipeFd);
}

pid_t	HttpRes::getCgiPid(void) const {

	return (_cgiPid);
}

std::vector<char>	HttpRes::getCgiBuff(void) const
{
	return (_cgiBuff);
}

std::vector<char>	HttpRes::getCgiBuff_header(void) const
{
	return (_cgiBuff_header);
}

s_pipe	HttpRes::getStatusCgiPipe(void) const
{
	return (_statusCgiPipe);
}

void	HttpRes::setStatusCode(int statusCode)
{
	this->_statusCode = statusCode;
}

void	HttpRes::setCgiPipeFd(int cgiPipeFd)
{
	this->_statusCgiPipe = PIPE_OPEN;
	this->_cgiPipeFd = cgiPipeFd;
	addFdToPollIn(cgiPipeFd);
}

void	HttpRes::setCgiPid(int cgiPid)
{
	this->_cgiPid = cgiPid;
}

int	HttpRes::getUploadTmpInFd(void) const
{
	return (_uploadTmpInFd);
}

int	HttpRes::getUploadOutFd(void) const
{
	return (_uploadOutFd);
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

r_type HttpRes::checkResourceType(void) {
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
					struct stat file;
					if (stat(_uriPath.c_str(), &file) == 0)
						_contentLength = file.st_size;
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
					_uriPath = _location->getRoot() + "/" + _location->getIndex();     //LA
					if (access(_uriPath.c_str(), F_OK) != 0)
						return (NOT_FOUND);
					else if (access(_uriPath.c_str(), R_OK) != 0)
						return (FORBIDDEN);
					else
					{
						struct stat file;
						if (stat(_uriPath.c_str(), &file) == 0)
							_contentLength = file.st_size;
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
				if (len > 3 && _uriPath.substr(len - 3, 3) == ".py")
					return (PYTHON);
				else if (len > 4 && _uriPath.substr(len - 4, 4) == ".php")
					return (PHP);
				else
				{
					struct stat file;
					if (stat(_uriPath.c_str(), &file) == 0)
						_contentLength = file.st_size;
					else
						return (FORBIDDEN);
					return (NORMALFILE);
				}
			}
			else
			{
				if (len > 3 && _uriPath.substr(len - 3, 3) == ".py")
					return (FORBIDDEN);
				else if (len > 4 && _uriPath.substr(len - 4, 4) == ".php")
					return (FORBIDDEN);
				else
				{
					struct stat file;
					if (stat(_uriPath.c_str(), &file) == 0)
						_contentLength = file.st_size;
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
	std::string tempPath2;

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
	_scriptName = tempPath;
	if (_location != NULL && _location->getRedirectionCode() > 0)
	{
		_resourceType = REDIRECTION;
		return (_location->getRedirectionCode());
	}
	if (_location == NULL || _location->getRoot() == _server->getRoot())
		_uriPath = _server->getRoot() + "/" + tempPath;
	else
	{
		tempPath2 = tempPath.substr(_location->getPath().length(), tempPath.length() - _location->getPath().length());
		_uriPath = _location->getRoot() + tempPath2;
	}
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
		_body = "";
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
}

void	HttpRes::headerBuild(void) {

	_header["Date:"] = timeStamp();
	_header["Server:"] = "Webserv/1.0";
	if (_resourceType == PHP || _resourceType == PYTHON)
		_header["Transfer-Encoding:"] = "chunked";
	else
		_header["Content-Length:"] = sizeToString(_contentLength);
	if (_keepAlive == true)
	{
		_header["Connection:"] = "keep-alive";
		_header["Keep-Alive:"] = "timeout=" + sizeToString(_client->getTimeOut());
	}
	else
		_header["Connection:"] = "close";
	if (_resourceType != NORMALFILE || _statusCode >= 400)
		_header["Content-Type:"] = "text/html";
	else
		_header["Content-Type:"] = getMimeType(_uriPath, _mimeTypes);
}

void	HttpRes::formatHeader(void) {

	std::stringstream response;
	response << _httpVersion << " " << _statusCode << " " << _statusMessage << "\r\n";
	for (std::map<std::string, std::string>::iterator it = _header.begin(); it != _header.end(); it++)
		response << it->first << " " << it->second << "\r\n";
	if (_is_cgi == false)
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

void	HttpRes::handleRequest(void)
{
	_statusCode = checkHttpVersion(_request->getHttpVersion());
	if (_statusCode == 200 && _request->bodyIsTooBig() == true)
		_statusCode = 413;
	if (_statusCode == 200)
		_statusCode = checkRequestHeader(_request->getHeader());
	if (_statusCode == 200 && methodIsAllowed(_request->getMethod()) == false)
		_statusCode = 405;
	if (_statusCode == 200)
		_statusCode = checkUri(_request->getUri());
	if (_statusCode == 200 && _method == "GET" && _resourceType == NORMALFILE)
		checkIfAcceptable(_request->getAccept());
	else if (_statusCode == 200 && _method == "DELETE")
	{
		if (std::remove(_uriPath.c_str()) != 0)
			_statusCode = 400;
		else
			_statusCode = 204;
	}
	else if (_statusCode == 200 && (_resourceType == PHP || _resourceType == PYTHON))
	{
		if (isAllowedCGI() == false)
		{
			_resourceType = NORMALFILE;
			_statusCode = 403;
		}
		else
		{
			_is_cgi = true;
			CGI cgi(*_request, *this);
			cgi.execCGI();
		}
	}
	else if (_statusCode == 200 && _request->getUploadFile())
	{
		if (_request->getBoundary() == "" || _request->getContentType().find("multipart/form-data") == std::string::npos)
			_statusCode = 400;
		else
		{
			uploadFileToServer();
			return ;
		}
	}
	_statusMessage = getStatus(_statusCode);
	if (!(_resourceType == PHP || _resourceType == PYTHON))
		bodyBuild(_request->getUri());
	if (_request->getKeepAlive() == false)
		_keepAlive = false;
	headerBuild();
	formatHeader();
	if (_client->getStatus() != UPLOADING_FILE)
		_client->setStatus(RES_READY_TO_BE_SENT);
}

void	HttpRes::uploadFileToServer(void)
{
	_client->setStatus(UPLOADING_FILE);
	_uploadTmpInFd = open(_request->getBodyTmpFilePath().c_str(), O_RDONLY);
	addFdToPollIn(_uploadTmpInFd);
	_uploadTmpInStream.open(_request->getBodyTmpFilePath().c_str());
}

void	HttpRes::finishBuildingResAfterUpload(void)
{
	_statusMessage = getStatus(_statusCode);
	if (!(_resourceType == PHP || _resourceType == PYTHON))
		bodyBuild(_client->getHttpReq()->getUri());
	if (_client->getHttpReq()->getKeepAlive() == false)
		_keepAlive = false;
	headerBuild();
	formatHeader();
	_client->setStatus(RES_READY_TO_BE_SENT);
	close(_uploadTmpInFd);
	_uploadTmpInStream.close();
	removeFdFromPoll(_uploadTmpInFd);
	if (access(_nameTmpUploadFile.c_str(), F_OK) == 0 && std::remove(_nameTmpUploadFile.c_str()))
		std::cout << "I could not delete the temp upload file of client " << this->_client->getId() << std::endl;
}

void	HttpRes::transferUploadFileInSide(void)
{
	if (_location == NULL || _location->getUploadDir() == "")
	{
		_statusCode = 403;
		finishBuildingResAfterUpload();
		return ;
	}
	std::string uploadDir = _location->getUploadDir();
	struct stat test;
	if (stat(uploadDir.c_str(), &test) != 0 || !S_ISDIR(test.st_mode) || access(uploadDir.c_str(), R_OK | W_OK) != 0)
	{
		_statusCode = 403;
		finishBuildingResAfterUpload();
		return ;
	}
	if (_uploadOutFd == -1)
	{
		std::ostringstream	file_path;
		file_path << uploadDir << "/" << "tmp_upload_client_" << this->_client->getId();
		_nameTmpUploadFile = file_path.str();
		_uploadOutFd = open(_nameTmpUploadFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (_uploadOutFd == -1)
		{
			_statusCode = 500;
			finishBuildingResAfterUpload();
			return ;
		}
		_uploadOutStream.open(_nameTmpUploadFile.c_str(), std::ios::binary);
		addFdToPollOut(_uploadOutFd);
	}
	if (_uploadBuffClean)
	{
		if (_byteWrittenUploadFile == 0 && PRINT_UPLOAD_FILE_STATUS)
			std::cout << TXT_BL << getTimestamp() << "Client " << this->_client->getId() << ":	The uploaded file transfert is starting"<<  TXT_END << std::endl;
		getline(_uploadTmpInStream, _uploadBuff);
		_uploadBuffClean = false;
		_byteWrittenUploadFile += _uploadBuff.size();
		double	percentWritten = (static_cast<double>(_byteWrittenUploadFile) / static_cast<double>(_request->getContentLength())) * 100;
		if (std::fmod(percentWritten, 5.0) < 0.1 && PRINT_UPLOAD_FILE_STATUS && _percentWrittenUploadFile < static_cast<size_t>(percentWritten) )
		{
			std::cout << TXT_BL << getTimestamp() << "Client " << this->_client->getId() << ":	Transfering uploaded file. "<< static_cast<int>(percentWritten) << "% done."<<  TXT_END << std::endl;
			_percentWrittenUploadFile = static_cast<size_t>(percentWritten);
		}
	}
}

void	HttpRes::transferUploadFileOutSide(void)
{
	if (_uploadBuffClean)
		return;
	if (_uploadBuff.find(_request->getBoundary()) != std::string::npos)
	{
		_uploadFileHeader = true;
		if (_uploadFileBody == true) // On rerentre dans header apres avoir fichier ==> Fermer le fichier, le renommer, et ouvrir un nouveau tmp.
		{
			close(_uploadOutFd);
			_uploadOutStream.close();
			removeFdFromPoll(_uploadOutFd);
			rename(_nameTmpUploadFile.c_str(), _nameFinalUploadFile.c_str());
			if (_uploadBuff.find(_request->getBoundary() + "--") == std::string::npos)
			{
				_uploadOutFd = open(_nameTmpUploadFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
				_uploadOutStream.open(_nameTmpUploadFile.c_str(), std::ios::binary);
				addFdToPollOut(_uploadOutFd);
			}
			else
			{
				if (PRINT_UPLOAD_FILE_STATUS)
					std::cout << TXT_BL << getTimestamp() << "Client " << this->_client->getId() << ":	The uploaded file transfert is done !"<<  TXT_END << std::endl;
				_statusCode = 201;
				finishBuildingResAfterUpload();
			}
			_nameFinalUploadFile = "";
			_uploadFileBody = false;
		}
	}
	else if (_uploadFileHeader && _uploadBuff == "\r") //Fin du header
	{
		_uploadFileHeader = false;
		_uploadFileBody = true;
		_uploadFileBodyFirstLine = true;
	}
	else if (_uploadFileHeader)
	{
		size_t posBeginFilename = _uploadBuff.find("filename=");
		if (posBeginFilename != std::string::npos)
		{
			posBeginFilename += 10;
			size_t posEndFilename = _uploadBuff.substr(posBeginFilename, _uploadBuff.size() - posBeginFilename).find('"');
			std::string file_name = _uploadBuff.substr(posBeginFilename, posEndFilename);
			std::ostringstream	file_path;
			file_path << _location->getUploadDir() << "/" << getTimestampFileName() << "_" << file_name;
			if (file_name != "")
				_nameFinalUploadFile = file_path.str();
			else
				_nameFinalUploadFile = "";
		}
	}
	else if (_uploadFileHeader == false && _uploadFileBody == true && _nameFinalUploadFile != "")
	{
		if (_backslashRFound == true)
		{
			_backslashRFound = false;
			_uploadOutStream << "\r";
		}
		if (_uploadBuff.length() > 0 && _uploadBuff.find("\r") == _uploadBuff.length() -1)
		{
			_uploadBuff.erase(_uploadBuff.length() - 1);
			_backslashRFound = true;
		}
		if (_uploadFileBodyFirstLine)
		{
			_uploadOutStream << _uploadBuff;
			_uploadFileBodyFirstLine = false;
		}
		else
		{
			_uploadOutStream << "\n" << _uploadBuff;
		}
	}
	_uploadBuff.clear();
	_uploadBuffClean = true;
}

void	HttpRes::openBodyFile(void)
{
	if (this->_fileToSendFd != -1 || this->_statusFileToSend == ERROR)
		return ;
	struct stat buf;
	stat(this->_uriPath.c_str(), &buf);
	this->_fileToSendSize = buf.st_size;
	if (PRINT_RES_FILE_STATUS)
		std::cout << TXT_BL << getTimestamp() << "Client " << this->_client->getId() << ":	Opening file " << this->_uriPath << " (size of " << this->_fileToSendSize << ")"<< TXT_END << std::endl;
	this->_fileToSendFd = open(this->_uriPath.c_str(), O_RDONLY);
	if (this->_fileToSendFd == -1)
	{
		this->_statusFileToSend = ERROR;
		this->setStatusCode(500);
		return ;
	}
	this->_statusFileToSend = OPEN;
	this->_client->addFdToPollIn(this->_fileToSendFd);
}

void	HttpRes::clearFileToSendBuff(void)
{
	this->_fileToSendBuff.clear();
}

void	HttpRes::clearCgiBuff(void)
{
	this->_cgiBuff.clear();
}

void	HttpRes::clearCgiBuff_header(void)
{
	this->_cgiBuff_header.clear();
}

void	HttpRes::closeBodyFile(void)
{
	if (this->_fileToSendFd == -1 || this->_statusFileToSend == ERROR)
		return;
	if (PRINT_RES_FILE_STATUS)
		std::cout << TXT_BL << getTimestamp() << "Client " << this->_client->getId() << ":	Closing file " << this->_uriPath << TXT_END << std::endl;
	this->_client->removeFdFromPoll(this->_fileToSendFd);
	if (close(this->_fileToSendFd) == -1)
		this->_statusFileToSend = ERROR;
	this->_statusFileToSend = CLOSE;
	this->_fileToSendFd = -1;
}

int		HttpRes::addBodyFileToBuff(void)
{
	if (this->_statusFileToSend != OPEN)
		return (1);
	int byte_read = 0;
	char readline[BUFFER_SIZE + 1];
	memset(readline, 0, BUFFER_SIZE + 1);
	byte_read = read(this->_fileToSendFd, readline, BUFFER_SIZE);
	if (byte_read == -1)
	{
		this->_statusFileToSend = ERROR;
		this->closeBodyFile();
		return (1);
	}
	else if (byte_read > 0)
		this->_fileToSendBuff.insert(this->_fileToSendBuff.end(), readline, readline + byte_read);
	return (0);
}

int	HttpRes::addCgiToBuff(void)
{
	if (this->_statusCgiPipe != PIPE_OPEN)
		return (1);
	int byte_read = 0;
	char readline[BUFFER_SIZE + 1];
	memset(readline, 0, BUFFER_SIZE + 1);
	byte_read = read(this->_cgiPipeFd, readline, BUFFER_SIZE);
	if (byte_read == -1)
	{
		this->_statusCgiPipe = PIPE_ERROR;
		this->cgiPipeFinishedWriting();
		return (1);
	}
	else if (byte_read > 0 && byte_read >= 12 && !strncmp(readline, "Set-Cookie: ", 12) && _first_line_of_header && strchr(readline, '\n') != NULL)
	{
		_first_line_of_header = false;
		std::string	firstline(readline);
		firstline = firstline.substr(0, firstline.find("\n", 0));
		this->_cgiBuff_header.insert(this->_cgiBuff_header.end(), firstline.c_str(), firstline.c_str() + firstline.size());

		std::string secondline(readline);
		secondline = secondline.substr(secondline.find("\n", 0) + 1);
		this->_cgiBuff.insert(this->_cgiBuff.end(), secondline.c_str(), secondline.c_str() + secondline.size() - 1);
	}
	else if (byte_read > 0)
	{
		this->_cgiBuff.insert(this->_cgiBuff.end(), readline, readline + byte_read);
	}
	return (0);
}

void	HttpRes::closeCgiPipe(void)
{
	if (_cgiPipeFd == -1 || _statusCgiPipe == PIPE_ERROR || _statusCgiPipe == PIPE_CLOSE)
		return;
	_client->removeFdFromPoll(this->_cgiPipeFd);
	if (close(this->_cgiPipeFd) == -1)
		this->_statusCgiPipe = PIPE_ERROR;
	_cgiPipeFd = -1;
}

void	HttpRes::removeFdFromPoll(int fd)
{
	this->_client->removeFdFromPoll(fd);
}

void	HttpRes::addFdToPollIn(int fd)
{
	this->_client->addFdToPollIn(fd);
}

void	HttpRes::cgiPipeFinishedWriting(void)
{
	this->_statusCgiPipe = PIPE_FINISH;
	closeCgiPipe();
}

void	HttpRes::addFdToPollOut(int fd)
{
	this->_client->addFdToPollOut(fd);
}

bool	HttpRes::isAllowedCGI(void) {

	if (_location == NULL || _location->getCgiBool() == false)
		return (false);
	std::map<std::string, std::string> cgiMap = _location->getCgiExtensionAndPath();
	if (_resourceType == PHP && cgiMap.find(".php") == cgiMap.end())
		return (false);
	if (_resourceType == PYTHON && cgiMap.find(".py") == cgiMap.end())
		return (false);
	return (true);
}

std::string HttpRes::getErrorPageContent(std::string path, int code, std::string message) {

	struct stat buf;
	int ret = stat(path.c_str(), &buf);
	std::ifstream fileStream;
	std::stringstream s;

	if (ret < 0 || !S_ISREG(buf.st_mode) || access(path.c_str(), R_OK) != 0)
		s << errorHTML(code, message);
	else
	{
		struct stat file;
		if (stat(path.c_str(), &file) == 0)
			_contentLength = file.st_size;
		_uriPath = path;
		s.clear();
	}
	return (s.str());
}