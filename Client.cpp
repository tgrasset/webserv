/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:25 by mbocquel          #+#    #+#             */
/*   Updated: 2023/08/10 19:33:59 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

bool			Client::_verbose = false;
unsigned int	Client::_count = 0;

/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Client::Client(void)
{
	this->_id = 0;
	this->_byte_sent_header = 0;
	this->_byte_sent_body = 0;
	this->_byte_recived_req_body = 0;
	this->_req = NULL;
	this->_res = NULL;
	gettimeofday(&(this->_last_activity), NULL);
	if (Client::_verbose)
		std::cout << "Client default constructor called" << std::endl;
}

Client::Client(Launcher	*launcher, struct sockaddr_in client_addr, int com_socket)
{
	Client::_count++;
	this->_id = Client::_count;
	this->_com_socket = com_socket;
	this->_client_addr = client_addr;
	this->_status = WANT_TO_RECIVE_REQ;
	this->_byte_sent_header = 0;
	this->_byte_sent_body = 0;
	this->_byte_recived_req_body = 0;
	this->_req = NULL;
	this->_res = NULL;
	this->_launcher = launcher;
	gettimeofday(&(this->_last_activity), NULL);
	if (Client::_verbose)
		std::cout << "Client constructor called" << std::endl;
}

Client::Client(Client const & copy)
{
	*this = copy;
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
		this->_id = client._id;
		this->_com_socket = client._com_socket;
		this->_status = client._status;
		this->_req = client._req;
		this->_res = client._res;
		this->_incoming_msg = client._incoming_msg;
		this->_server_ptr = client._server_ptr;
		this->_client_addr = client._client_addr;
		this->_req_recived = client._req_recived;
		this->_req_header = client._req_header;
		this->_byte_sent_header = client._byte_sent_header;
		this->_byte_sent_body = client._byte_sent_body;
		this->_last_activity = client._last_activity;
		this->_byte_recived_req_body = client._byte_recived_req_body;
		this->_launcher = client._launcher;
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

unsigned int	Client::getId(void) const
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

t_fd	Client::getSocketType(int fd) const
{
	if (fd == this->_com_socket)
		return (COM_SOCKET);
	else if (this->_res != NULL && this->_res->getCgiPipeFd()[0] == fd)
		return (CGI_R_PIPE);
	else if (this->_res != NULL && this->_res->getCgiPipeFd()[1] == fd)
		return (CGI_W_PIPE);
	else if (this->_res != NULL && this->_res->getFileToSendFd() == fd)
		return (RES_FILE_FD);
	else if (this->_req != NULL && this->_req->getBodyTmpFileFd() == fd)
		return (REQ_FILE_FD);
	else
		return (NOT_MINE);
}

int	Client::receive_request(void)
{
	this->reset_last_activity();
	if (this->_status == WANT_TO_RECIVE_REQ || this->_status == RECIVING_REQ_HEADER)
		return (this->receive_request_header());
	if (this->_status == RECIVING_REQ_BODY)
		return (this->receive_request_body());
	return (1);
}

int	Client::receive_request_header(void)
{

	int byte_recv = 0;
	char recvline[BUFFER_SIZE + 1];
	memset(recvline, 0, BUFFER_SIZE + 1);
	byte_recv = recv(this->_com_socket, recvline, BUFFER_SIZE, 0);
	if (byte_recv == 0)
	{
		std::cout << "Client " << this->_id << " has close connexion" << std::endl;
		return (1);
	}
	std::cout << "Client " << this->_id << " just recieved " << byte_recv << " bytes for the request header" << std::endl;
	if (byte_recv > 0)
	{
		this->_status = RECIVING_REQ_HEADER;
		this->_req_recived.insert(this->_req_recived.end(), recvline, recvline + byte_recv);
		std::string req_recived_string(static_cast<char *>(this->_req_recived.data()));
		std::size_t pos_end_header = req_recived_string.find("\r\n\r\n");
		if (pos_end_header != std::string::npos)
		{
			this->_req_header = req_recived_string.substr(0, pos_end_header);
			this->_req = new HttpReq(this, this->_req_header, this->_server_ptr);
			if (this->_req == NULL)
				throw ClientException("New didn't work for _req !");
			if (this->_req->getStatusReq() == COMPLETED)
				this->_status = WAITING_FOR_RES;
			else
			{
				this->_status = RECIVING_REQ_BODY;
				if (this->_req_recived.size() > pos_end_header + 4)
				{
					std::vector<char> to_add_body;
					to_add_body.insert(to_add_body.end(), this->_req_recived.begin() + pos_end_header + 4, this->_req_recived.end());
					this->_req->add_to_body_file_buff(to_add_body);
				}
			}
		}
	}
	if (this->_status == WAITING_FOR_RES)
	{
		std::cout << std::endl << "\e[33m" << getTimestamp() << "	Client " << this->_id << " just recieved the following request:\e[32m" << std::endl;
		std::cout << this->_req_recived.data() << std::endl;
		std::cout << "\e[0m";
	}
	return (0);
}

int	Client::receive_request_body(void)
{
	if (this->_req->getStatusReq() == COMPLETED)
		return (0);
	int byte_recv = 0;
	char recvline[BUFFER_SIZE + 1];
	memset(recvline, 0, BUFFER_SIZE + 1);
	byte_recv = recv(this->_com_socket, recvline, BUFFER_SIZE, 0);
	if (byte_recv == 0)
	{
		std::cout << "Client " << this->_id << " has close connexion" << std::endl;
		return (1);
	}
	std::cout << "Client " << this->_id << " just recieved " << byte_recv << " bytes for the request body" << std::endl;
	if (byte_recv > 0)
	{
		std::vector<char> to_add_body;
		to_add_body.insert(to_add_body.end(), recvline, recvline + byte_recv);
		this->_req->add_to_body_file_buff(to_add_body);
	}
	return (0);
}

void	Client::send_response(void)
{
	this->reset_last_activity();
	if (this->_status == WAITING_FOR_RES)
	{
		if (this->_res != NULL)
			delete this->_res;
		this->_res = new HttpRes(this, *(this->_req));
		if (this->_res == NULL)
			throw ClientException("New didn't work for _res !");
	}
	if (this->_status == WAITING_FOR_RES || this->_status == SENDING_RES_HEADER)
		this->send_response_header();
	else if (this->_status == SENDING_RES_BODY)
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
	if (byte_sent == -1)
	{
		this->_status = ERROR_WHILE_SENDING;
		std::cout << "\e[33m" << getTimestamp() <<  "	I had an error sending response header to client " << this->_id << "\e[0m" << std::endl;
		return ;
	}
	else
	{
		this->_status = SENDING_RES_HEADER;
		this->_byte_sent_header += byte_sent;
	}
	if (this->_byte_sent_header == static_cast<unsigned int>(res_header_full.size()))
	{
		this->_status = SENDING_RES_BODY;
		std::cout << "\e[33m" << getTimestamp() <<  "	I have sent the following response header to client " << this->_id << " : \e[32m " << std::endl;
		std::cout << std::endl << res_header_full << "\e[0m" << std::endl;
	}
}

void	Client::send_response_body(void)
{
	if (this->_res == NULL)
		return ;
	std::string	res_body = this->_res->getBody(); //empty string in case of a file to send
	if (res_body.size() != 0)
		this->send_response_body_error();
	else if (this->_res->getResourceType() == NORMALFILE)
		this->send_response_body_normal_file();
	else if (this->_res->getResourceType() == PYTHON || this->_res->getResourceType() == PHP)
		this->send_response_body_cgi();
}

void	Client::send_response_body_error(void)
{
	int	byte_sent = 0;
	std::string	res_body = this->_res->getBody();
	std::string res_body_remain = res_body.substr(this->_byte_sent_body, res_body.size() - this->_byte_sent_body);
	std::string	to_send_body;
	if (res_body_remain.size() <= BUFFER_SIZE)
		to_send_body = res_body_remain;
	else
		to_send_body = res_body_remain.substr(0, BUFFER_SIZE);
	byte_sent = send(this->_com_socket, to_send_body.c_str(), to_send_body.size(), 0);
	if (byte_sent == -1)
	{
		this->_status = ERROR_WHILE_SENDING;
		std::cout << "\e[33m" << getTimestamp() <<  "	I had an error sending response body to client " << this->_id << "\e[0m" << std::endl;
		return ;
	}
	else
	{
		this->_status = SENDING_RES_BODY;
		this->_byte_sent_body += byte_sent;
		std::cout << "	body : " << this->_byte_sent_body << "/" << res_body_remain.size() << " sent" << std::endl;
	}
	if (this->_byte_sent_body == static_cast<unsigned int>(res_body.size()))
	{
		this->_status = RES_SENT;
		std::cout << "\e[33m" <<  getTimestamp() << "	I have sent the following response body to client " << this->_id << " : \e[32m " << std::endl;
		std::cout << std::endl << res_body << "\e[0m" << std::endl;
	}
}

void	Client::send_response_body_normal_file(void)
{
	std::vector<char>	to_send;
	int byte_sent = 0;
	if (this->_res->getFileToSendFd() == -1)
		this->_res->openBodyFile();
	else
	{
		to_send = this->_res->getFileToSendBuff();
		if (to_send.size() != 0)
		{
			byte_sent = send(this->_com_socket, to_send.data(), to_send.size(), 0);
			if (byte_sent == -1)
			{
				this->_status = ERROR_WHILE_SENDING;
				std::cout << "\e[33m" << getTimestamp() <<  "	I had an error sending file to client " << this->_id << "\e[0m" << std::endl;
				return ;
			}
			this->_byte_sent_body += byte_sent;
			this->_status = SENDING_RES_BODY;
			this->_res->clearFileToSendBuff();
			if (this->_byte_sent_body == this->_res->getFileToSendSize())
				this->_status = RES_SENT;
		}
	}
		
	

	/*
	int byte_read_file = 0;
	int	byte_sent = 0;

	if (this->_res->getFileToSendFd() == -1) //besoin de ouvrir le fichier et ajouter le fd dans epoll
	//open file to send
	//read and store from file to send. 
	if (!this->_res->getFileToSend().is_open())
	{
		this->_res->getFileToSend().open(this->_res->getUriPath().c_str(), std::ifstream::binary);
		this->_file_to_send.seekg (0, _file_to_send.end);
		this->_file_to_send_size = this->_file_to_send.tellg();
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
	byte_sent = send(this->_com_socket, buffer, byte_read_file, 0);
	if (byte_sent == -1)
	{
		this->_status = ERROR_WHILE_SENDING;
		std::cout << "\e[33m" << getTimestamp() <<  "	I had an error sending file to client " << this->_id << "\e[0m" << std::endl;
		_file_to_send.close();
		delete[] buffer;
		return ;
	}
	else
	{
		this->_status = SENDING_RES_BODY;
		this->_byte_sent_body += byte_sent;
		std::cout << "	body : " << (static_cast<double>(this->_byte_sent_body) / static_cast<double>(this->_file_to_send_size)) * 100 << "% sent" << std::endl;
	}
	if (this->_byte_sent_body == this->_file_to_send_size)
	{
		this->_status = RES_SENT;
		std::cout << "\e[33m" << getTimestamp() << "	I have sent the file to client " << this->_id << "\e[0m" << std::endl;
		this->_file_to_send.close();
	}
	delete[] buffer;
	*/
}

void	Client::send_response_body_cgi(void)
{

}

void	Client::reset_last_activity(void)
{
	gettimeofday(&(this->_last_activity), NULL);
}

unsigned long	Client::time_since_last_activity_us(void) const
{
	unsigned long	ts;
	struct timeval	now;

	gettimeofday(&now, NULL);
	ts = (now.tv_sec - this->_last_activity.tv_sec) * 1000000 + (now.tv_usec - this->_last_activity.tv_usec);
	return (ts);
}

void	Client::reset_client(void)
{
	this->reset_last_activity();
	this->_status = WANT_TO_RECIVE_REQ;
	if (this->_req)
	{
		delete this->_req;
		this->_req = NULL;
	}
	if (this->_res)
	{
		delete this->_res;
		this->_res = NULL;
	}
	this->_incoming_msg = "";
	this->_req_recived.clear();
	this->_req_header = "";
	//this->_req_body = "";
	this->_byte_sent_header = 0;
	this->_byte_sent_body = 0;
	this->_file_to_send_size = 0;
}

bool Client::getKeepAlive(void) const
{
	if (this->_res)
		return (this->_res->getKeepAlive());
	return (false);
}


void	Client::remove_fd_from_epoll(int fd)
{
	this->_launcher->remove_fd_from_epoll(fd);
}

void	Client::add_fd_to_epoll_in(int fd)
{
	this->_launcher->add_fd_to_epoll_in(fd);
}

void	Client::add_fd_to_epoll_out(int fd)
{
	this->_launcher->add_fd_to_epoll_out(fd);
}

void	Client::writeReqBodyFile(void)
{
	this->_req->writeOnReqBodyFile();
}

void	Client::addBodyFileToBuff(void)
{
	this->_res->addBodyFileToBuff();
}