/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/12 16:05:30 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRes.hpp"

bool	HttpRes::_verbose = true;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
HttpRes::HttpRes(void)
{
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
		
	}
	return (*this);
}
