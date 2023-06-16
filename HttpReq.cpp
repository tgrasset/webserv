/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:03 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/16 17:19:47 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReq.hpp"

//bool	HttpReq::_verbose = false;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
HttpReq::HttpReq(std::string &content)
{
	/*if (HttpReq::_verbose)
		std::cout << "HttpReq default constructor called" << std::endl;*/
	_method = "";
	_uri = "";
	_httpVersion = "";
	_header.clear();
	_host = "";
	_accept.clear();
	_contentType = "";
	_contentLength = 0;
	_keepAlive = true;
	_body = "";
	(void)content; // parsing a faire ici, have fun joseph :p  (j'ai void 'content' juste pour que ca compile)
}

HttpReq::HttpReq(HttpReq const & copy)
{
	*this = copy;
	/*if (HttpReq::_verbose)
		std::cout << "HttpReq copy constructor called" << std::endl;*/
}

HttpReq::~HttpReq(void)
{
	/*if (HttpReq::_verbose)
		std::cout << "HttpReq destructor called" << std::endl;*/
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
		_body = httpreq.getBody();
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */
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

int	HttpReq::getContentLength() const {
	
	return (_contentLength);
}

bool	HttpReq::getKeepAlive() const {

	return (_keepAlive);
}

std::string		HttpReq::getBody() const {

	return (_body);
}

void	HttpReq::setBody(std::string body)
{
	(void)body;
}