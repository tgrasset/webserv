/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:25 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/12 16:05:57 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

bool	Client::_verbose = true;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Client::Client(void)
{
	if (Client::_verbose)
		std::cout << "Client default constructor called" << std::endl;
}

Client::Client(Client const & copy)
{
	*this = copy;
	if (Client::_verbose)
		std::cout << "Client copy constructor called" << std::endl;
}

Client::~Client(void)
{
	if (Client::_verbose)
		std::cout << "Client destructor called" << std::endl;
}

/* ************************************************************************** */
/*                     Surcharge d'operateur                                  */
/* ************************************************************************** */
Client	& Client::operator=(Client const & client)
{
	if (this != &client)
	{
		this->_com_socket = client._com_socket;
		this->_status = client._status;
		this->_req.clear();
		this->_req = client._req;
		this->_res.clear();
		this->_res = client._res;
	}
	return (*this);
}
