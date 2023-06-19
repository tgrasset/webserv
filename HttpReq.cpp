/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlanza <jlanza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:03 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/19 17:02:39 by jlanza           ###   ########.fr       */
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

void	HttpReq::parse(std::string &content)
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
		//std::cout << "$" << key << "$			$" << value << "$" << std::endl;
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

	/*std::cout << "method		: $" << _method << "$" << std::endl;
	std::cout << "uri		: $" << _uri << "$" << std::endl;
	std::cout << "httpVersion	: $" << _httpVersion << "$" << std::endl << std::endl;
	std::cout << _host << std::endl;
	std::cout << _accept[0] << std::endl;
	std::cout << _accept[1] << std::endl;
	std::cout << _accept[2] << std::endl;
	std::cout << _keepAlive << std::endl;
	std::cout << _contentLength;
	std::cout << _contentType << std::endl; */
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

void		HttpReq::setBody(std::string &body) {

	this->_body = body;
}
