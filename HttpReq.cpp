/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:03 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/13 16:32:17 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpReq.hpp"

bool	HttpReq::_verbose = false;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
HttpReq::HttpReq(void)
{
	if (HttpReq::_verbose)
		std::cout << "HttpReq default constructor called" << std::endl;
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
		
	}
	return (*this);
}
