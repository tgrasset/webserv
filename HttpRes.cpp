/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/14 14:37:50 by tgrasset         ###   ########.fr       */
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
	_httpVersion = version;
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

int	HttpRes::checkUri(std::string uri, std::string body, bool &error) {
	
	(void)uri;
	(void)body;
	(void)error;
	return (0);
}

void	HttpRes::handleRequest(HttpReq &request, std::vector<Server *> servers) {
	
	bool	error = false;
	
	setServer(request.getHost(), servers);
	_statusCode = checkHttpVersion(request.getHttpVersion(), error);
	if (error == false)
		_statusCode = checkMethod(request.getMethod(), error);
	if (error == false)
		_statusCode = checkUri(request.getUri(), request.getBody(), error); //verif redirections, cgi...
	_statusMessage = getStatus(_statusCode);
	// buildHeader();
	// formatResponse();      tout mettre au bon format dans toSend
	if (request.getKeepAlive() == false)
		_keepAlive = false;
}