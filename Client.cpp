/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:25 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/16 14:01:00 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

bool	Client::_verbose = false;
int		Client::_count = 0;

/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Client::Client(void)
{
	Client::_count++;
	this->_id = Client::_count;
	this->_byte_sent = 0;
	if (Client::_verbose)
		std::cout << "Client default constructor called" << std::endl;
}

Client::Client(struct sockaddr_in client_addr, int com_socket)
{
	Client::_count++;
	this->_id = Client::_count;
	this->_com_socket = com_socket;
	this->_client_addr = client_addr;
	this->_status = WANT_TO_SEND_REQ;
	this->_byte_sent = 0;
	if (Client::_verbose)
		std::cout << "Client constructor called" << std::endl;
}

Client::Client(Client const & copy)
{
	Client::_count++;
	this->_id = Client::_count;
	*this = copy;
	this->_byte_sent = 0;
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
		this->_req = client._req;
		this->_res = client._res;
		this->_incoming_msg = client._incoming_msg;
		this->_server_ptr = client._server_ptr;
		this->_client_addr = client._client_addr;
		this->_req_recived = client._req_recived;
		this->_byte_sent = client._byte_sent;
	}
	return (*this);
}

void	Client::AddServerPtr(Server * new_server_ptr)
{
	if (new_server_ptr)
		this->_server_ptr.push_back(new_server_ptr);
}

int		Client::getCom_socket(void) const
{
	return (this->_com_socket);
}

void	Client::add_to_req_recived(char *str)
{
	this->_req_recived += str;
	if (this->_req_recived.size() >= 4 
		&& this->_req_recived.substr(this->_req_recived.size() - 4, 4) == "\r\n\r\n")
	{
		this->_status = REQ_SENT;
		/* Code temporaire de Maxence */
		std::cout << std::endl << "\e[33m" << getTimestamp() << "	I recieved the following client request from client " << this->_id << ":\e[32m" << std::endl;
		std::cout << this->_req_recived << std::endl;
		std::cout << "\e[0m";
		this->_status = WAITING_FOR_RES;
	}
}

void	Client::SetStatus(t_status_c status)
{
	this->_status = status;
}

t_status_c 	Client::getStatus(void) const
{
	return (this->_status);
}

struct sockaddr_in 	Client::getClient_addr(void) const
{
	return (this->_client_addr);
}

int	Client::getId(void) const
{
	return (this->_id);
}

void	Client::print_ClientServer(void) const
{
	for (std::vector< Server *>::const_iterator it = this->_server_ptr.begin();
		it != this->_server_ptr.end(); ++it)
	{
		if (*it != NULL)
			std::cout << " " << (*it)->getServerName();
	}
}

std::string	Client::get_res_string(void) const
{
	return (this->_res.getToSend());
}

int	Client::get_byte_sent(void) const
{
	return (this->_byte_sent);
}

void	Client::set_byte_sent(int byte)
{
	this->_byte_sent = byte;
}
