/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:21 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/06 16:49:32 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "HttpRes.hpp"
# include "HttpReq.hpp"
# include "Server.hpp"
# include "webserv.hpp"
# include "Launcher.hpp"

class HttpRes;
class HttpReq;
class Server;
class Launcher;

typedef enum e_status_c {
	WANT_TO_RECIVE_REQ,
	RECIVING_REQ_HEADER,
	RECIVING_REQ_BODY,
	REQ_RECIVED,
	WAITING_FOR_RES,
	SENDING_RES_HEADER,
	SENDING_RES_BODY,
	RES_SENT, 
	ERROR_WHILE_SENDING
}				t_status_c;

typedef enum e_type_fd {
	COM_SOCKET,
	CGI_PIPE,
	RES_FILE_FD,
	REQ_FILE_FD,
	NOT_MINE
}				t_fd;

class Client
{
private:
	int						_com_socket;
	t_status_c				_status;
	HttpReq					*_req;
	HttpRes					*_res;
	std::string				_incoming_msg;
	std::vector< Server *> 	_server_ptr;
	struct sockaddr_in		_client_addr;
	std::vector<char>		_req_recived;
	std::string				_req_header;
	unsigned int			_id;
	unsigned int			_byte_sent_header;
	unsigned int			_byte_sent_body;
	unsigned int			_byteRecivedReqBody;
	struct timeval			_last_activity;
	std::ifstream 			_file_to_send;
	unsigned int			_file_to_send_size;
	Launcher				*_launcher;
	
	static bool				_printBody;	
	static unsigned int		_count;
	
public:
	Client(void);
	Client(Launcher	*launcher, struct sockaddr_in _client_addr, int com_socket);
	Client(Client const & copy);
	~Client(void);

	Client	&operator=(Client const & client);

	void						AddServerPtr(Server * new_server_ptr);
	int							getComSocket(void) const;
	void						setStatus(t_status_c status);
	t_status_c					getStatus(void) const;
	struct sockaddr_in		 	getClient_addr(void) const;
	unsigned int				getId(void) const;
	void						printClientServer(void) const;
	int							getByteSentHeader(void) const;
	void						setByteSentHeader(int byte);
	void						sendResponse(void);
	void						sendResponseHeader(void);
	void						sendResponseBody(void);
	void						sendResponseBodyError(void);
	void						sendResponseBodyNormalFile(void);
	void						sendResponseBodyCgi(void);
	int							receiveRequest(void);
	int							receiveRequestHeader(void);
	int							receiveRequestBody(void);
	void						resetLastActivity(void);
	unsigned long				timeSinceLastActivityUs(void) const;
	void						resetClient(void);
	bool						getKeepAlive(void) const;
	t_fd						getSocketType(int fd) const;
	void						removeFdFromPoll(int fd);
	void						addFdToPollIn(int fd);
	void						addFdToPollOut(int fd);
	int							addBodyFileToBuff(void);
	int							addCgiToBuff(void);
	int							writeReqBodyFile(void);
	void						cgiPipeFinishedWriting(void);
	Launcher					*getLauncher(void) const;

	class ClientException : public std::exception {
	public :
		ClientException(std::string errMessage) throw() {
			_errMessage = "Client Error: " + errMessage;
		}
		virtual const char* what() const throw() {
			return (_errMessage.c_str());
		}
		~ClientException() throw() {}
	private:
		std::string _errMessage;
	};

};

#endif