/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:25 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/16 17:58:37 by mbocquel         ###   ########.fr       */
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
	this->_req = NULL;
	this->_res = NULL;
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
	this->_req = NULL;
	this->_res = NULL;
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
	if (this->_req)
		delete this->_req;
	if (this->_res)
		delete this->_res;
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
		this->_req_header = client._req_header;
		this->_req_body = client._req_body;
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
	return (this->_res->getToSend());
}

int	Client::get_byte_sent(void) const
{
	return (this->_byte_sent);
}

void	Client::set_byte_sent(int byte)
{
	this->_byte_sent = byte;
}

void	Client::receive_request(void)
{
	int byte_recv = 0;
	char recvline[BUFFER_SIZE + 1];
	
	memset(recvline, 0, BUFFER_SIZE + 1);
	byte_recv = recv(this->_com_socket, recvline, BUFFER_SIZE, 0);
	if (byte_recv > 0)
	{
		if (this->_status == WANT_TO_SEND_REQ)
			this->_status = SENDING_REQ_HEADER;
		this->_req_recived += recvline;
		std::size_t pos_end_header = this->_req_recived.find("\r\n\r\n");
		if (this->_status == SENDING_REQ_HEADER && pos_end_header != std::string::npos)
		{
			this->_req_header = this->_req_recived.substr(0, pos_end_header);
			this->_req = new HttpReq(this->_req_header);
			if (this->_req == NULL)
				throw ClientException("New didn't work for _req !");
			if (this->_req->getContentLength() == 0 || this->_req->getMethod() != "POST")
				this->_status = WAITING_FOR_RES;
			else
			{
				this->_status = SENDING_REQ_BODY;
				if (this->_req_recived.size() > pos_end_header + 4)
					this->_req_body =  this->_req_recived.substr(pos_end_header + 4, this->_req_recived.size() - pos_end_header - 4);
				if (this->_req_body.size() == static_cast<std::size_t>(this->_req->getContentLength()))
					this->_status = WAITING_FOR_RES;
			}
		}
		else if (this->_status == SENDING_REQ_BODY)
		{
			if (this->_req_recived.size() > pos_end_header + 4)
				this->_req_body =  this->_req_recived.substr(pos_end_header + 4, this->_req_recived.size() - pos_end_header - 4);
			if (this->_req_body.size() == static_cast<std::size_t>(this->_req->getContentLength()))
			{
				this->_req->setBody(_req_body);
				this->_status = WAITING_FOR_RES;
			}
		}		
	}
	if (this->_status == WAITING_FOR_RES)
	{
		std::cout << std::endl << "\e[33m" << getTimestamp() << "	I recieved the following client request from client " << this->_id << ":\e[32m" << std::endl;
		std::cout << this->_req_recived << std::endl;
		std::cout << "\e[0m";
	}
}

void	Client::send_response(void)
{
	int	byte_sent = 0;
	std::string	res_full("HTTP/1.0 200 OK\r\n\r\n<html><body><h1>Hey you, I just got your message !</h1></body<</html>");
	//std::string	res_full =this->_res->getToSend();
	std::string res_remain = res_full.substr(this->_byte_sent, res_full.size() - this->_byte_sent);
	std::string	to_send;
	if (res_remain.size() <= BUFFER_SIZE)
		to_send = res_remain;
	else
		to_send = res_remain.substr(0, BUFFER_SIZE);
	byte_sent = send(this->_com_socket, to_send.c_str(), to_send.size(), 0);
	if (byte_sent != -1)
	{
		this->_status = RECIVING_RES;
		this->_byte_sent += byte_sent;
	}
	if (this->_byte_sent == static_cast<int>( res_full.size()))
	{
		this->_status = RES_SENT;
		std::cout << getTimestamp() <<  "\e[33m	I have sent the following response to client " << this->_id << " : \e[32m " << std::endl;
		std::cout << std::endl << res_full << std::endl;
	}
}
