/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2023/08/11 16:30:04 by mbocquel         ###   ########.fr       */
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
	_status_req = WAITING_TO_FILL_BODY_FILE;
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
	HttpReq::parse(content, servers);
	if (_contentLength == 0 || _method != "POST" || _contentLength > this->_server->getClientMaxBodySize())
		_status_req = COMPLETED;
	else
		_status_req = WAITING_TO_FILL_BODY_FILE;
}

HttpReq::HttpReq(HttpReq const & copy)
{
	*this = copy;
}

HttpReq::~HttpReq(void)
{
	if (access(_bodyTmpPath.c_str(), F_OK) == 0 && std::remove(_bodyTmpPath.c_str()))
		std::cout << "I could not delete the file of client " << this->_id << std::endl;
	if (_server != NULL)
		delete _server;
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
		_boundary = httpreq._boundary;
		_toAddBodyFile = httpreq._toAddBodyFile;
		_status_req = httpreq._status_req;
		_byteRecivedReqBody = httpreq._byteRecivedReqBody;
		_byteWroteTmpBodyFile = httpreq._byteWroteTmpBodyFile;
		_client = httpreq._client;
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

	// parse first line
	const int	first_space = head[0].find(' ');
	const int	second_space = head[0].find(' ', first_space + 1);
	_method			= head[0].substr(0, first_space);
	_uri			= head[0].substr(first_space + 1, second_space - first_space - 1);
	_httpVersion	= head[0].substr(second_space + 1, second_space - head[0].size());
	head.erase(head.begin());

	//create _header map
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

	// parse rest of request
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
		return ;
	}
	std::string hostname;
	if (_host.find(':') != std::string::npos)
		hostname = _host.substr(0, _host.length() - _host.find(':'));
	else
		hostname = _host;
	for (std::vector<Server *>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		if ((*it)->getServerName() == _host)
		{
			_server = new Server(*(*it));
			return;
		}
	}
	_server = new Server(*servers[0]);
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

std::string	HttpReq::getBoundary() const {

	return (_boundary);
}

status_req		HttpReq::getStatusReq(void) const
{
	return (_status_req);
}

void		HttpReq::addToBodyFileBuff(std::vector<char> str)
{
	
	this->_toAddBodyFile.insert(_toAddBodyFile.end(), str.begin(), str.end());
	if (this->_bodyTmpFileFd == -1)
	{
		std::ostringstream	file_path;
		file_path << HttpReq::_bodyTmpFolder << "body_client_" << this->_id;
		this->_bodyTmpPath = file_path.str();
		_bodyTmpFileFd = open(this->_bodyTmpPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
		this->_client->addFdToPollOut(_bodyTmpFileFd);
		if (_bodyTmpFileFd == -1)
			throw HttpReqException("Opening the body file");
	}
}

void	HttpReq::writeOnReqBodyFile(void)
{
	int	byte_wrote;
	if (this->_bodyTmpFileFd != -1 && this->_toAddBodyFile.size() != 0)
	{
		byte_wrote = send(_bodyTmpFileFd, _toAddBodyFile.data(), _toAddBodyFile.size(), 0);
		if (byte_wrote == -1)
			throw HttpReqException("Writing on the body file");
		_byteWroteTmpBodyFile += byte_wrote;
		_toAddBodyFile.clear();
		if (_contentLength == _byteRecivedReqBody && _contentLength == _byteWroteTmpBodyFile)
		{
			_status_req = COMPLETED;
			this->_client->removeFdFromPoll(_bodyTmpFileFd);
			if (close(_bodyTmpFileFd) == -1)
				throw HttpReqException("Closing tmp body file");
		}
	}
}

void	HttpReq::closeBodyFile()
{
	this->_bodyFile.close();
}

bool	HttpReq::bodyIsTooBig() const
{
	if (this->getContentLength() > this->_server->getClientMaxBodySize())
		return (true);
	else
		return (false);
}

