/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:03 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/13 11:43:05 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReq.hpp"

bool	HttpReq::_verbose = true;
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
	_body = "";
	(void)content; // parsing a faire ici, have fun joseph :p
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

std::string		HttpReq::getBody() const {

	return (_body);
}