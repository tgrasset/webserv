/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlanza <jlanza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:03 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/15 17:35:32 by jlanza           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReq.hpp"

bool	HttpReq::_verbose = false;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
HttpReq::HttpReq(std::string &content)
{
	if (HttpReq::_verbose)
		std::cout << "HttpReq default constructor called" << std::endl;
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
	HttpReq::parse(content); // parsing a faire ici, have fun joseph :p  (j'ai void 'content' juste pour que ca compile)
}

bool	HttpReq::parse(std::string &content)
{
	std::vector<std::string>	head_and_body;
	std::vector<std::string>	head;
	size_t	body_start = 0;

	head_and_body = cpp_split_sep(content, "\r\n\r\n");
	head = cpp_split_sep(head_and_body[0], "\r\n");

	const int	first_space = head[0].find(' ');
	const int	second_space = head[0].find(' ', first_space);
	_method			= head[0].substr(0, first_space);
	_uri			= head[0].substr(first_space, second_space - first_space);
	_httpVersion	= head[0].substr(second_space, second_space - head[0].size());

	head.erase(head.begin());

	for (int i = 0; i < head.size(); i++)
	{
		int	pos = 0;
		pos = head[i].find(':');
		_header.insert(head[i].substr(0, pos), head[i].substr(pos + 1, head[i].size() - pos));
		if (_header[i] == "host")
	}

	if (head_and_body.size() == 2)
		_body = head_and_body[1];
}

HttpReq::HttpReq(HttpReq const & copy)
{
	*this = copy;
	if (HttpReq::_verbose)
		std::cout << "HttpReq copy constructor called" << std::endl;
}

HttpReq::~HttpReq(void)
{
	if (HttpReq::_verbose)
		std::cout << "HttpReq destructor called" << std::endl;
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
