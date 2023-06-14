/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/14 18:09:38 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRes.hpp"

bool	HttpRes::_verbose = false;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
HttpRes::HttpRes(void)
{
	std::string response("HTTP/1.0 200 OK\r\n\r\n<html><body><h1>Hey you, I just got your message !</h1></body<</html>");
	this->_toSend = response;
	if (HttpRes::_verbose)
		std::cout << "HttpRes default constructor called" << std::endl;
}

HttpRes::HttpRes(HttpRes const & copy)
{
	*this = copy;
	if (HttpRes::_verbose)
		std::cout << "HttpRes copy constructor called" << std::endl;
}

HttpRes::~HttpRes(void)
{
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
		this->_toSend = httpres._toSend;
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */

std::string		HttpRes::getToSend(void) const
{
	return (this->_toSend);
}
