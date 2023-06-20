/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:25 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/20 17:57:16 by mbocquel         ###   ########.fr       */
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
	this->_byte_sent_header = 0;
	this->_byte_sent_body = 0;
	this->_req = NULL;
	this->_res = NULL;
	gettimeofday(&(this->_last_activity), NULL);
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
	this->_byte_sent_header = 0;
	this->_byte_sent_body = 0;
	this->_req = NULL;
	this->_res = NULL;
	gettimeofday(&(this->_last_activity), NULL);
	if (Client::_verbose)
		std::cout << "Client constructor called" << std::endl;
}

Client::Client(Client const & copy)
{
	Client::_count++;
	this->_id = Client::_count;
	*this = copy;
	gettimeofday(&(this->_last_activity), NULL);
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
		this->_byte_sent_header = client._byte_sent_header;
		this->_byte_sent_body = client._byte_sent_body;
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

int	Client::get_byte_sent_header(void) const
{
	return (this->_byte_sent_header);
}

void	Client::set_byte_sent_header(int byte)
{
	this->_byte_sent_header = byte;
}

void	Client::receive_request(void)
{
	int byte_recv = 0;
	char recvline[BUFFER_SIZE + 1];
	
	memset(recvline, 0, BUFFER_SIZE + 1);
	byte_recv = recv(this->_com_socket, recvline, BUFFER_SIZE, 0);
	std::cout << "I just got from my client " << byte_recv << std::endl;
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
				this->_req_body = this->_req_recived.substr(pos_end_header + 4, this->_req_recived.size() - pos_end_header - 4);
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


/* Attention c'est a moi d'ouvrir le fichier et de l'envoyer au client. */
void	Client::send_response(void)
{
	if (this->_status == WAITING_FOR_RES)
	{
		if (this->_res != NULL)
			delete this->_res;
		this->_res = new HttpRes(*(this->_req), this->_server_ptr);
		if (this->_res == NULL)
			throw ClientException("New didn't work for _res !");
	}
	if (this->_status == WAITING_FOR_RES || this->_status == RECIVING_RES_HEADER)
		this->send_response_header();
	else if (this->_status == RECIVING_RES_BODY)
		this->send_response_body();
}

void	Client::send_response_header(void)
{
	int	byte_sent = 0;
	std::string	res_header_full = this->_res->getFormattedHeader();
	std::string res_header_remain = res_header_full.substr(this->_byte_sent_header, res_header_full.size() - this->_byte_sent_header);
	std::string	to_send_header;
	if (res_header_remain.size() <= BUFFER_SIZE)
		to_send_header = res_header_remain;
	else
		to_send_header = res_header_remain.substr(0, BUFFER_SIZE);
	byte_sent = send(this->_com_socket, to_send_header.c_str(), to_send_header.size(), 0);
	if (byte_sent != -1)
	{
		this->_status = RECIVING_RES_HEADER;
		this->_byte_sent_header += byte_sent;
	}
	if (this->_byte_sent_header == static_cast<int>(res_header_full.size()))
	{
		this->_status = RECIVING_RES_BODY;
		std::cout << getTimestamp() <<  "\e[33m	I have sent the following response header to client " << this->_id << " : \e[32m " << std::endl;
		std::cout << std::endl << res_header_full << "\e[0m" << std::endl;
	}
}

void	Client::send_response_body(void)
{
	int	byte_sent = 0;
	std::string	res_body = this->_res->getBody();
	
	/* Cas de erreur, le body est directement accesible */
	if (res_body.size() != 0)
	{
		std::string res_body_remain = res_body.substr(this->_byte_sent_body, res_body.size() - this->_byte_sent_body);
		std::string	to_send_body;
		if (res_body_remain.size() <= BUFFER_SIZE)
			to_send_body = res_body_remain;
		else
			to_send_body = res_body_remain.substr(0, BUFFER_SIZE);
		byte_sent = send(this->_com_socket, to_send_body.c_str(), to_send_body.size(), 0);
		if (byte_sent != -1)
		{
			this->_status = RECIVING_RES_BODY;
			this->_byte_sent_body += byte_sent;
		}
		if (this->_byte_sent_body == static_cast<int>(res_body.size()))
		{
			this->_status = RES_SENT;
			std::cout << getTimestamp() <<  "\e[33m	I have sent the following response body to client " << this->_id << " : \e[32m " << std::endl;
			std::cout << std::endl << res_body << "\e[0m" << std::endl;
		}
	}
	/* Cas de fichier normal (pas de CGI) */
	else if (this->_res->getResourceType() == NORMALFILE)
	{
		int byte_read_file = 0;
		
		if (!this->_file_to_send.is_open())
		{
			this->_file_to_send.open(this->_res->getUriPath().c_str(), std::ifstream::binary);
			this->_file_to_send.seekg (0, _file_to_send.end);
			this->_file_to_send_size = this->_file_to_send.tellg();
			std::cout << "file size is : " << _file_to_send_size << std::endl;
			this->_file_to_send.seekg (0, _file_to_send.beg);
		}
			
		if (this->_file_to_send.fail())
			throw ClientException(strerror(errno));
		char * buffer = new char [BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		if (buffer == NULL)
			throw ClientException(" New char buffer");
		this->_file_to_send.read(buffer, BUFFER_SIZE);
		byte_read_file = this->_file_to_send.gcount();
		std::cout << "byte_read_file: " << byte_read_file << std::endl;
		std::cout << buffer << std::endl << std::endl;

		byte_sent = send(this->_com_socket, buffer, byte_read_file, 0);
		if (byte_sent == -1)
		{
			this->_status = RES_SENT;
			std::cout << getTimestamp() <<  "\e[33m	I had an error sending file to client " << this->_id << " : \e[32m " << std::endl;
			_file_to_send.close();
			delete[] buffer;
			return ;
		}
		if (byte_sent != -1)
		{
			this->_status = RECIVING_RES_BODY;
			this->_byte_sent_body += byte_sent;
		}
		if (this->_byte_sent_body == this->_file_to_send_size)
		{
			this->_status = RES_SENT;
			std::cout << getTimestamp() <<  "\e[33m	I have sent the file to client " << this->_id << " : \e[32m " << std::endl;
			this->_file_to_send.close();
		}
		delete[] buffer;
	}
}

void	Client::reset_last_activity(void)
{
	gettimeofday(&(this->_last_activity), NULL);
}

unsigned long	Client::time_since_last_activity_ms(void) const
{
	unsigned long	ts;
	struct timeval	now;

	gettimeofday(&now, NULL);
	ts = (now.tv_sec - this->_last_activity.tv_sec) * 1000000 + (now.tv_usec - this->_last_activity.tv_usec);
	return (ts);
}

