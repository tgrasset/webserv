/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/13 16:04:22 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRes.hpp"

bool	HttpRes::_verbose = true;

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

Server	*HttpRes::getServer() const {

	return (_server);
}

std::string	HttpRes::getToSend() const {

	return (_toSend);
}

bool	HttpRes::getKeepAlive() const {

	return (_keepAlive);
}

int	HttpRes::checkHttpVersion(std::string version) {

	if (version != "HTTP/1.0" && version != "HTTP/1.1" && version != "HTTP/0.9")
		return (505);
	_httpVersion = version;
	return (200);
}

int	HttpRes::checkMethod(std::string method) {

	if (method != "GET" && method != "POST" && method != "DELETE")
		return (405);
	_method = method;
	return (200);
}

int	HttpRes::checkHeader(std::map<std::string, std::string> const reqHeader, std::vector <Server *> servers) {

	std::string date;
}

int	HttpRes::checkUri(std::string uri, std::string body) {
	
	
}

void	HttpRes::handleRequest(HttpReq &request, std::vector<Server *> servers) {
	
	_statusCode = checkHttpVersion(request.getHttpVersion());
	if (_statusCode == 200)
		_statusCode = checkMethod(request.getMethod());
	if (_statusCode == 200)
		_statusCode = checkHeader(request.getHeader(), servers);
	if (_statusCode == 200)
		_statusCode = checkUri(request.getUri(), request.getBody()); //verif redirections, cgi...
	_statusMessage = getStatus(_statusCode);  //fonction dans utils.cpp
	formatResponse();				// tout recuperer et mettre au bon format dans _toSend
}