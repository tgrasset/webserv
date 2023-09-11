/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2023/09/11 17:45:53 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReq.hpp"

std::string		HttpReq::_bodyTmpFolder(BODY_TMP_FOLDER);
unsigned int	HttpReq::_count = 0;

/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
HttpReq::HttpReq(Client *client, std::string &content, std::vector<Server *> servers)
{
	HttpReq::_count++;
	_client = client;
	_statusReq = WAITING_TO_FILL_BODY_FILE;
	this->_id = HttpReq::_count;
	_method = "";
	_uri = "";
	_httpVersion = "";
	_header.clear();
	_host = "";
	_accept.clear();
	_contentType = "";
	_contentLength = 0;
	_toAddBodyFile.clear();
	_keepAlive = true;
	_bodyTmpPath = "";
	_boundary = "";
	_byteRecivedReqBody = 0;
	_byteWroteTmpBodyFile = 0;
	_bodyTmpFileFd = -1;
	_statusBodyFile = CLOSE;
	_uploadFile = false;
	_percentRecievedTmpBodyFile = 0;
	HttpReq::parse(content, servers);
	if (_contentLength == 0 || _method != "POST" || _contentLength > this->_server->getClientMaxBodySize() || unauthorizedMethod())
		_statusReq = COMPLETED;
}

HttpReq::HttpReq(HttpReq const & copy)
{
	*this = copy;
}

HttpReq::~HttpReq(void)
{
	if (access(_bodyTmpPath.c_str(), F_OK) == 0 && std::remove(_bodyTmpPath.c_str()))
		std::cout << "I could not delete the file of client " << this->_client->getId() << std::endl;
	if (this->_statusBodyFile  == OPEN)
	{
		_client->removeFdFromPoll(_bodyTmpFileFd);
		close(_bodyTmpFileFd);
		_statusBodyFile = CLOSE;
		
	}
	if (_server != NULL)
		delete _server;
	if (_location != NULL)
		delete _location;
}

/* ************************************************************************** */
/*                     Surcharge d'operateur                                  */
/* ************************************************************************** */
HttpReq	& HttpReq::operator=(HttpReq const & httpreq)
{
	if (this != &httpreq)
	{
		_method = httpreq.getMethod();
		_uri = httpreq.getUri();
		_httpVersion = httpreq.getHttpVersion();
		_header = httpreq.getHeader();
		_host = httpreq.getHost();
		_accept = httpreq.getAccept();
		_contentType = httpreq.getContentType();
		_contentLength = httpreq.getContentLength();
		_keepAlive = httpreq.getKeepAlive();
		_bodyTmpPath = httpreq._bodyTmpPath;
		_id = httpreq._id;
		_server = httpreq._server;
		_location = httpreq._location;
		_boundary = httpreq._boundary;
		_toAddBodyFile = httpreq._toAddBodyFile;
		_statusReq = httpreq._statusReq;
		_byteRecivedReqBody = httpreq._byteRecivedReqBody;
		_byteWroteTmpBodyFile = httpreq._byteWroteTmpBodyFile;
		_client = httpreq._client;
		_bodyTmpFileFd = httpreq._bodyTmpFileFd;
		_statusBodyFile = httpreq._statusBodyFile;
		_uploadFile = httpreq._uploadFile;
		_percentRecievedTmpBodyFile = httpreq._percentRecievedTmpBodyFile;
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */

void	HttpReq::parse(std::string &content, std::vector<Server *> servers)
{
	//delete \r from string
	std::string::size_type r_pos = content.find('\r');
	while (r_pos != std::string::npos)
	{
		content.erase(r_pos, 1);
		r_pos = content.find('\r', r_pos);
	}

	std::vector<std::string>	head;
	head = cpp_split(content, "\n");

	/* Parse first lineb */
	const int	first_space = head[0].find(' ');
	const int	second_space = head[0].find(' ', first_space + 1);
	_method			= head[0].substr(0, first_space);
	_uri			= head[0].substr(first_space + 1, second_space - first_space - 1);
	_httpVersion	= head[0].substr(second_space + 1, second_space - head[0].size());
	head.erase(head.begin());
	
	std::string uriCopy = _uri;
	/*Case of upload a file*/
	if (_method == "POST" 
		&& !(_uri.size() > 3 && _uri.substr(_uri.size() - 3, 3) == ".py")
		&& !(_uri.size() > 4 && _uri.substr(_uri.size() - 4, 4) == ".php"))
		_uploadFile = true;
		
	/*create _header map*/
	int	pos = 0;
	std::string	key;
	std::string	value;
	for (unsigned long i = 0; i < head.size(); i++)
	{
		pos = head[i].find(':');
		key = toUpperCase(head[i].substr(0, pos));
		value = head[i].substr(pos + 1, head[i].size() - pos);
		if (value[0] == ' ')
			value.erase(0, 1);
		_header.insert(std::make_pair(key, value));
	}

	/* Parse rest of request*/
	if (_header.find("HOST") != _header.end())
		_host = _header["HOST"];
	if (_header.find("ACCEPT") != _header.end())
		_accept = cpp_split(_header["ACCEPT"], ",\n\r");
	if (_header.find("CONNECTION") != _header.end())
		_keepAlive = (_header["CONNECTION"] == "keep-alive");
	if (_header.find("CONTENT-LENGTH") != _header.end())
		std::istringstream(_header["CONTENT-LENGTH"]) >> _contentLength;
	if (_header.find("CONTENT-TYPE") != _header.end())
		_contentType = _header["CONTENT-TYPE"];

	setServer(servers);
	setLocation(uriCopy);
	if (_contentType.find("multipart") != std::string::npos)
	{
		size_t bound = _contentType.find("boundary=");
		if (bound != std::string::npos)
		{
			bound += 9;
			_boundary = _contentType.substr(bound, _contentType.length() - bound);
		}
	}
}

void	HttpReq::setServer(std::vector<Server *> servers) {

	if (servers.size() == 1)
	{
		_server = new Server(*servers[0]);
		if (_server == NULL)
			throw HttpReqException("New error in HttpReq");
		return ;
	}
	std::string hostname;
	if (_host.find(':') != std::string::npos)
		hostname = _host.substr(0, _host.length() - (_host.length() - _host.find(':')));
	else
		hostname = _host;
	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if ((*it)->getServerName() == hostname)
		{
			_server = new Server(*(*it));
			if (_server == NULL)
				throw HttpReqException("New error in HttpReq");
			return;
		}
	}
	_server = new Server(*servers[0]);
	if (_server == NULL)
		throw HttpReqException("New error in HttpReq");
}

void	HttpReq::setLocation(std::string uri) {

	std::string locPath;
	Location *tempLoc = NULL;
	size_t	end;
	std::vector<Location> locs = _server->getLocations();
	for (std::vector<Location>::iterator it = locs.begin(); it != locs.end(); it++)
	{
		locPath = (*it).getPath();
		if (tempLoc == NULL && locPath == "/")
		{
			tempLoc = &(*it);
			continue;
		}
		for (end = 0; uri[end] != '\0' && locPath[end] != '\0' && uri[end] == locPath[end]; end++)
		{
			;
		}
		if (locPath[end] == '\0' && (uri[end] == '\0' || uri[end] == '/'))
		{
			if (tempLoc == NULL || (tempLoc != NULL && tempLoc->getPath().length() < (*it).getPath().length()))
				tempLoc = &(*it);
		}
	}
	if (tempLoc != NULL)
	{
		_location = new Location(*tempLoc);
		if (_location == NULL)
			throw HttpReqException("New error in HttpReq");
	}
	else
		_location = NULL;
}

std::string		HttpReq::getMethod() const {

	return (_method);
}

std::string		HttpReq::getUri() const {

	return (_uri);
}

std::string		HttpReq::getHttpVersion() const {

	return (_httpVersion);
}

std::map<std::string, std::string> HttpReq::getHeader() const {

	return (_header);
}

std::string		HttpReq::getHost() const {

	return (_host);
}

std::vector<std::string>	HttpReq::getAccept() const {

	return (_accept);
}

std::string		HttpReq::getContentType() const {

	return (_contentType);
}

unsigned int	HttpReq::getContentLength() const {

	return (_contentLength);
}

bool	HttpReq::getKeepAlive() const {

	return (_keepAlive);
}

std::string		HttpReq::getBodyTmpFilePath() const {

	return (_bodyTmpPath);
}

int		HttpReq::getBodyTmpFileFd() const
{
	return (_bodyTmpFileFd);
}

Server *HttpReq::getServer() const {

	return (_server);
}

Location	*HttpReq::getLocation()const {

	return (_location);
}

std::string	HttpReq::getBoundary() const {

	return (_boundary);
}

status_req		HttpReq::getStatusReq(void) const
{
	return (_statusReq);
}

Client		*HttpReq::getClient() const {

	return (_client);
}

bool	HttpReq::getUploadFile(void) const
{
	return (_uploadFile);
}

int		HttpReq::addToBodyFileBuff(std::vector<char> str)
{
	this->_toAddBodyFile.insert(_toAddBodyFile.end(), str.begin(), str.end());
	if (this->_bodyTmpFileFd == -1)
	{
		std::ostringstream	file_path;
		file_path << HttpReq::_bodyTmpFolder << "body_client_" << this->_id;
		this->_bodyTmpPath = file_path.str();
		std::cout << TXT_YEL << getTimestamp() << "Client " << this->_client->getId() << ":	Opening " << this->_bodyTmpPath.c_str() << TXT_END << std::endl;
		_bodyTmpFileFd = open(this->_bodyTmpPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (_bodyTmpFileFd == -1)
		{
			_statusBodyFile = ERROR;
			std::cout << TXT_RED << TXT_B << getTimestamp() << "Client "<< this->_client->getId() << 
			": Error while openning the body file, the client will be removed." << TXT_END << std::endl;
			return (1);
		}
		_statusBodyFile = OPEN;
		this->_client->addFdToPollOut(_bodyTmpFileFd);
	}
	return (0);
}

int	HttpReq::writeOnReqBodyFile(void)
{
	int	byte_wrote;
	if (this->_bodyTmpFileFd != -1 && this->_toAddBodyFile.size() != 0 && _statusBodyFile == OPEN)
	{
		byte_wrote = write(_bodyTmpFileFd, _toAddBodyFile.data(), _toAddBodyFile.size());
		if (byte_wrote == -1)
			return (1);
		else if (byte_wrote > 0)
		{
			_byteWroteTmpBodyFile += byte_wrote;
			double	percent_received = (static_cast<double>(_byteWroteTmpBodyFile) / static_cast<double>(_contentLength)) * 100;
			if (std::fmod(percent_received, 5.0) < 0.1 && PRINT_REQ_FILE_STATUS && _percentRecievedTmpBodyFile < static_cast<int>(percent_received) )
			{
				std::cout << TXT_YEL << getTimestamp() << "Client " << _client->getId() << ":	" << static_cast<int>(percent_received) << "% of the Req Body File recieved" << TXT_END << std::endl;
				_percentRecievedTmpBodyFile = static_cast<int>(percent_received);
			}
			if (PRINT_REQ_BODY)
			{
				std::cout << TXT_I;
				printVectorChar(_toAddBodyFile);
			}
			_toAddBodyFile.clear();
			if (_contentLength == _byteWroteTmpBodyFile)
			{
				_statusReq = COMPLETED;
				std::cout << TXT_YEL << getTimestamp() << "Client " << this->_client->getId() << ":	Request body saved in a file. Removing FD of the tmp file from the poll and closing it" << TXT_END << std::endl;
				this->_client->removeFdFromPoll(_bodyTmpFileFd);
				if (close(_bodyTmpFileFd) == -1)
					return (1);
				_bodyTmpFileFd = -1;
				_statusBodyFile = CLOSE;
				this->_client->setStatus(WAITING_FOR_RES);
				this->_client->removeFdFromPoll(_client->getComSocket());
				this->_client->addFdToPollOut(_client->getComSocket());
			}
		}
	}
	else if (_statusBodyFile == OPEN && !(this->_toAddBodyFile.size() == 0 && _contentLength > _byteWroteTmpBodyFile))
	{
		std::cout << TXT_YEL << getTimestamp() << "Client " << this->_client->getId() << ":	Request body saved in a file. Removing FD of the tmp file from the poll and closing it" << TXT_END << std::endl;
		_statusReq = COMPLETED;
		this->_client->removeFdFromPoll(_bodyTmpFileFd);
		if (close(_bodyTmpFileFd) == -1)
			return (1);
		_bodyTmpFileFd = -1;
		_statusBodyFile = CLOSE;
		this->_client->setStatus(WAITING_FOR_RES);
		this->_client->removeFdFromPoll(_client->getComSocket());
		this->_client->addFdToPollOut(_client->getComSocket());
	}
	return (0);
}

bool	HttpReq::bodyIsTooBig() const
{
	if (this->getContentLength() > this->_server->getClientMaxBodySize())
		return (true);
	else
		return (false);
}

void	HttpReq::setUri(std::string new_uri)
{
	this->_uri = new_uri;
}

bool	HttpReq::unauthorizedMethod(void) const {

	if (_location == NULL)
		return (0);
	std::vector<std::string> authMethods = _location->getMethods();
	for (std::vector<std::string>::iterator it = authMethods.begin(); it != authMethods.end(); it++)
	{
		if (_method == *it)
			return (false);
	}
	return (true);
}