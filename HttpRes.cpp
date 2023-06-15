/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/15 11:28:04 by tgrasset         ###   ########.fr       */
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
	_keepAlive = true;
	_uriPath = "";
	_uriQuery = "";
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

void	HttpRes::setServer(std::string reqHost, std::vector<Server *> servers) {

	if (servers.size() == 1)
	{
		_server = servers[0];
		return ;
	}
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

int	HttpRes::checkUri(std::string uri, std::string body, bool &error) {
	
	size_t questionMark = uri.find('?');
	size_t end = uri.rfind('#');
	std::string tempPath;

	(void)body;
	(void)error;
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
	return (0);
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
		_statusCode = checkUri(request.getUri(), request.getBody(), error);
	// bodyBuild();
	if (request.getKeepAlive() == false)
		_keepAlive = false;
	// headerBuild();
	_statusMessage = getStatus(_statusCode);
	formatResponse();
}