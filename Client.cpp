/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:25 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/12 10:19:51 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

unsigned int	Client::_count = 0;

/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Client::Client(void)
{
	this->_id = 0;
	this->_byte_sent_header = 0;
	this->_byte_sent_body = 0;
	this->_byteRecivedReqBody = 0;
	this->_req = NULL;
	this->_res = NULL;
	_percentSentFile = 0;
	gettimeofday(&(this->_last_activity), NULL);
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
	this->_byteRecivedReqBody = 0;
	this->_req = NULL;
	this->_res = NULL;
	this->_launcher = launcher;
	_percentSentFile = 0;
	gettimeofday(&(this->_last_activity), NULL);
}

Client::Client(Client const & copy)
{
	*this = copy;
}

Client::~Client(void)
{
	if (this->_req)
		delete this->_req;
	if (this->_res)
		delete this->_res;
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
		this->_byteRecivedReqBody = client._byteRecivedReqBody;
		this->_launcher = client._launcher;
		_percentSentFile = client._percentSentFile;
	}
	return (*this);
}

void	Client::AddServerPtr(Server * new_server_ptr)
{
	if (new_server_ptr)
		this->_server_ptr.push_back(new_server_ptr);
}

int		Client::getComSocket(void) const
{
	return (this->_com_socket);
}

void	Client::setStatus(t_status_c status)
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

Launcher	*Client::getLauncher(void) const {

	return (this->_launcher);
}

void	Client::printClientServer(void) const
{
	for (std::vector< Server *>::const_iterator it = this->_server_ptr.begin();
		it != this->_server_ptr.end(); ++it)
	{
		if (*it != NULL)
			std::cout << " " << (*it)->getServerName();
	}
}

int	Client::getByteSentHeader(void) const
{
	return (this->_byte_sent_header);
}

void	Client::setByteSentHeader(int byte)
{
	this->_byte_sent_header = byte;
}

t_fd	Client::getSocketType(int fd) const
{
	if (fd == this->_com_socket)
		return (COM_SOCKET);
	else if (this->_res != NULL && this->_res->getCgiPipeFd() == fd)
		return (CGI_PIPE);
	else if (this->_res != NULL && this->_res->getFileToSendFd() == fd)
		return (RES_FILE_FD);
	else if (this->_req != NULL && this->_req->getBodyTmpFileFd() == fd)
		return (REQ_FILE_FD);
	else if (this->_res != NULL && this->_res->getUploadTmpInFd() == fd)
		return (UPLOAD_TMP_IN);
	else if (this->_res != NULL && this->_res->getUploadOutFd() == fd)
		return (UPLOAD_OUT);
	else
		return (NOT_MINE);
}

void	Client::printClientFd(void) const
{
	std::cout << TXT_I << "COM_SOCKET : " << this->_com_socket << TXT_END << std::endl;
	std::cout << TXT_I << "CGI_PIPE : " << this->_res->getCgiPipeFd() << TXT_END << std::endl;
	std::cout << TXT_I << "RES_FILE_FD : " << this->_res->getFileToSendFd() << TXT_END << std::endl;
	std::cout << TXT_I << "REQ_FILE_FD : " << this->_req->getBodyTmpFileFd()  << TXT_END << std::endl;
	std::cout << TXT_I << "UPLOAD_TMP_IN : " << this->_res->getUploadTmpInFd() << TXT_END << std::endl;
	std::cout << TXT_I << "UPLOAD_OUT : " << this->_res->getUploadOutFd() << TXT_END << std::endl;
}

int	Client::receiveRequest(void)
{
	this->resetLastActivity();
	if (this->_status == WANT_TO_RECIVE_REQ || this->_status == RECIVING_REQ_HEADER)
		return (this->receiveRequestHeader());
	if (this->_status == RECIVING_REQ_BODY)
		return (this->receiveRequestBody());
	return (1);
}

int	Client::receiveRequestHeader(void)
{

	int byte_recv = 0;
	char recvline[BUFFER_SIZE + 1];
	memset(recvline, 0, BUFFER_SIZE + 1);
	byte_recv = recv(this->_com_socket, recvline, BUFFER_SIZE, 0);
	if (byte_recv == -1)
	{
		std::cout << "Reading of com socket for client " << this->_id << " failed. The client will be removed." << std::endl;
		return (1);
	}
	else if (byte_recv == 0)
	{
		std::cout << TXT_CY << getTimestamp() << "Client " << _id << ":	Connexion has been closed. The client will be removed." << TXT_END << std::endl;
		return (1);
	}
	else
	{
		this->_status = RECIVING_REQ_HEADER;
		this->_req_recived.insert(this->_req_recived.end(), recvline, recvline + byte_recv);
		std::string req_recived_string(this->_req_recived.begin(), this->_req_recived.end());
		std::size_t pos_end_header = req_recived_string.find("\r\n\r\n");
		if (pos_end_header != std::string::npos)
		{
			this->_req_header = req_recived_string.substr(0, pos_end_header);
			std::cout << TXT_CY << getTimestamp() << "Client " << _id << ":	Request header received" << TXT_END << std::endl;
			if (PRINT_REQ_HEADER)
				std::cout << TXT_I << this->_req_header << TXT_END <<std::endl;
			this->_req = new HttpReq(this, this->_req_header, this->_server_ptr);
			if (this->_req == NULL)
				throw ClientException("New didn't work for _req !");
			if (this->_req->getStatusReq() == COMPLETED)
			{
				this->_status = WAITING_FOR_RES;
				this->_launcher->removeFdFromPoll(_com_socket);
				this->_launcher->addFdToPollOut(_com_socket);
			}
			else
			{
				this->_status = RECIVING_REQ_BODY;
				if (this->_req_recived.size() > pos_end_header + 4)
				{
					std::vector<char> to_add_body;
					to_add_body.insert(to_add_body.end(), this->_req_recived.begin() + pos_end_header + 4, this->_req_recived.end());
					return (this->_req->addToBodyFileBuff(to_add_body));
				}
			}
		}
	}
	return (0);
}

int	Client::receiveRequestBody(void)
{
	if (this->_req->getStatusReq() == COMPLETED)
		return (0);
	int byte_recv = 0;
	char recvline[BUFFER_SIZE + 1];
	memset(recvline, 0, BUFFER_SIZE + 1);
	byte_recv = recv(this->_com_socket, recvline, BUFFER_SIZE, 0);
	if (byte_recv == -1)
	{
		std::cout << TXT_RED << getTimestamp() << "Client " << _id << ":	Reading of com socket failed. The client will be removed." << TXT_END << std::endl;
		return (1);
	}
	else if (byte_recv == 0)
	{
		std::cout << TXT_CY << getTimestamp() << "Client " << _id << ":	Connexion has been closed. The client will be removed." << TXT_END << std::endl;
		return (1);
	}
	else
	{
		std::vector<char> to_add_body;
		to_add_body.insert(to_add_body.end(), recvline, recvline + byte_recv);
		return (this->_req->addToBodyFileBuff(to_add_body));
	}
	return (0);
}

void	Client::sendResponse(void)
{
	if (this->_status == WAITING_FOR_RES)
	{
		if (this->_res != NULL)
			delete this->_res;
		this->_res = new HttpRes(this, this->_req);
		if (this->_res == NULL)
			throw ClientException("New didn't work for _res !");
	}
	if (this->_status == RES_READY_TO_BE_SENT || this->_status == SENDING_RES_HEADER)
		this->sendResponseHeader();
	else if (this->_status == SENDING_RES_BODY)
		this->sendResponseBody();
}

void	Client::sendResponseHeader(void)
{
	int	byte_sent = 0;
	std::string	res_header_full = this->_res->getFormattedHeader();
	std::string res_header_remain = res_header_full.substr(this->_byte_sent_header, res_header_full.size() - this->_byte_sent_header);
	std::string	to_send_header;
	if (res_header_remain.size() <= BUFFER_SIZE)
		to_send_header = res_header_remain;
	else
		to_send_header = res_header_remain.substr(0, BUFFER_SIZE);
	if (to_send_header.size() != 0)
	{
		byte_sent = send(this->_com_socket, to_send_header.c_str(), to_send_header.size(), 0);
		if (byte_sent == -1 || byte_sent != static_cast<int>(to_send_header.size()))
		{
			this->_status = ERROR_WHILE_SENDING;
			return;
		}
		else
		{
			this->resetLastActivity();
			this->_status = SENDING_RES_HEADER;
			this->_byte_sent_header += byte_sent;
		}
		if (this->_byte_sent_header == static_cast<unsigned int>(res_header_full.size()))
		{
			this->_status = SENDING_RES_BODY;
			std::cout << TXT_CY << getTimestamp() << "Client " << _id << ":	Response header has been sent" << TXT_END << std::endl;
			if (PRINT_RES_HEADER)
				std::cout << TXT_I << res_header_full << TXT_END << std::endl;
		}
	}
}

void	Client::sendResponseBody(void)
{
	if (this->_res == NULL)
		return ;
	std::string	res_body = this->_res->getBody();
	if (res_body.size() != 0)
		this->sendResponseBodyError();
	else if (this->_res->getResourceType() == PYTHON || this->_res->getResourceType() == PHP)
		this->sendResponseBodyCgi();
	else
		this->sendResponseBodyNormalFile();
}

void	Client::sendResponseBodyError(void)
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
		return ;
	}
	else if (byte_sent > 0)
	{
		this->resetLastActivity();
		this->_status = SENDING_RES_BODY;
		this->_byte_sent_body += byte_sent;
		int percent_sent = static_cast<int>(static_cast<double>(_byte_sent_body)/static_cast<double>(res_body_remain.size()) * 100);
		std::cout << TXT_CY << getTimestamp() << "Client " << _id << ":	" << percent_sent << "% of the body sent" << TXT_END << std::endl;
	}
	if (this->_byte_sent_body == static_cast<unsigned int>(res_body.size()))
	{
		this->_status = RES_SENT;
		std::cout << TXT_CY <<  getTimestamp() << "Client " << _id << ":	Response body has been sent" << TXT_END << std::endl;
		if (PRINT_RES_BODY)
			std::cout << TXT_I << res_body << TXT_END << std::endl;
	}
}

void	Client::sendResponseBodyNormalFile(void)
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
				return;
			}
			else if (byte_sent > 0)
			{
				this->resetLastActivity();
				this->_byte_sent_body += byte_sent;
				double	percent_sent = (static_cast<double>(_byte_sent_body) / static_cast<double>(this->_res->getFileToSendSize())) * 100;
				if (std::fmod(percent_sent, 5.0) < 0.1 && _percentSentFile < static_cast<int>(percent_sent) && PRINT_RES_FILE_STATUS )
				{
					std::cout << TXT_CY << getTimestamp() << "Client " << _id << ":	" << static_cast<int>(percent_sent) << "% of file " << this->_res->getUriPath() << " sent" << TXT_END << std::endl;
					_percentSentFile = static_cast<int>(percent_sent);
				}		
				this->_status = SENDING_RES_BODY;
				this->_res->clearFileToSendBuff();
				if (this->_byte_sent_body == this->_res->getFileToSendSize())
				{
					this->_res->closeBodyFile();
					this->_status = RES_SENT;
					_percentSentFile = 0;
				}
			}
		}
		else if (this->_res->getFileToSendSize() == 0)
		{
			this->_res->closeBodyFile();
			this->_status = RES_SENT;
		}
	}
}

void	Client::sendResponseBodyCgi(void)
{
	std::vector<char> cgi_buff_header;
	int byte_sent_header = 0;
	cgi_buff_header = _res->getCgiBuff_header();
	if (cgi_buff_header.size() > 0)
	{
		std::string str_size_chunk("");
		std::vector<char>	to_send(str_size_chunk.begin(), str_size_chunk.end());
		to_send.insert(to_send.end(), cgi_buff_header.begin(), cgi_buff_header.end());
		to_send.push_back('\r');
		to_send.push_back('\n');
		byte_sent_header = send(this->_com_socket, to_send.data(), to_send.size(), 0);
		if (byte_sent_header == -1)
		{
			this->_status = ERROR_WHILE_SENDING;
			return ;
		}
		else if (byte_sent_header > 0)
		{	
			_res->clearCgiBuff_header();
			this->resetLastActivity();
		}
	}
	std::vector<char>	cgi_buff;
	int byte_sent = 0;
	cgi_buff = this->_res->getCgiBuff();
	int	size_chunk = cgi_buff.size();
	if (size_chunk > 0)
	{
		std::ostringstream ss;
		ss << "\r\n" << std::hex << size_chunk << "\r\n";
		std::string str_size_chunk(ss.str());
		std::vector<char>	to_send(str_size_chunk.begin(), str_size_chunk.end());
		to_send.insert(to_send.end(), cgi_buff.begin(), cgi_buff.end());
		to_send.push_back('\r');
		to_send.push_back('\n');
		byte_sent = send(this->_com_socket, to_send.data(), to_send.size(), 0);
		if (byte_sent == -1)
		{
			this->_status = ERROR_WHILE_SENDING;
			return ;
		}
		else if (byte_sent > 0)
		{
			this->_byte_sent_body += byte_sent;
			this->_status = SENDING_RES_BODY;
			this->_res->clearCgiBuff();
			this->resetLastActivity();
		}
	}
	if (this->_res->getStatusCgiPipe() == PIPE_FINISH)
	{
		std::string end_of_cgi("0\r\n\r\n");
		byte_sent = send(this->_com_socket, end_of_cgi.c_str(), end_of_cgi.size(), 0);
		if (byte_sent == -1)
		{
			this->_status = ERROR_WHILE_SENDING;
			return ; 
		}
		else if (byte_sent > 0)
		{
			this->_status = RES_SENT;
			this->resetLastActivity();
		}
	}
}

void	Client::resetLastActivity(void)
{
	gettimeofday(&(this->_last_activity), NULL);
}

unsigned long	Client::timeSinceLastActivityUs(void) const
{
	unsigned long	ts;
	struct timeval	now;

	gettimeofday(&now, NULL);
	ts = (now.tv_sec - this->_last_activity.tv_sec) * 1000000 + (now.tv_usec - this->_last_activity.tv_usec);
	return (ts);
}

void	Client::resetClient(void)
{
	this->resetLastActivity();
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

void	Client::removeFdFromPoll(int fd)
{
	this->_launcher->removeFdFromPoll(fd);
}

void	Client::addFdToPollIn(int fd)
{
	this->_launcher->addFdToPollIn(fd);
}

void	Client::addFdToPollOut(int fd)
{
	this->_launcher->addFdToPollOut(fd);
}

int	Client::writeReqBodyFile(void)
{
	return (this->_req->writeOnReqBodyFile());
}

int	Client::addBodyFileToBuff(void)
{
	return (this->_res->addBodyFileToBuff());
}

int Client::addCgiToBuff(void)
{
	return (this->_res->addCgiToBuff());
}

void	Client::cgiPipeFinishedWriting(void)
{
	this->_res->cgiPipeFinishedWriting();
}

void 	Client::transferUploadFileInSide(void)
{
	this->_res->transferUploadFileInSide();
}

void 	Client::transferUploadFileOutSide(void)
{
	this->_res->transferUploadFileOutSide();
}

HttpReq	*Client::getHttpReq(void) const
{
	return (this->_req);
}

HttpRes	*Client::getHttpRes(void) const
{
	return (this->_res);
}

int		Client::getTimeOut(void) const
{
	return (_server_ptr[0]->getTimeout());
}