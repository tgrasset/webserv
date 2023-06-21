/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:21 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/21 16:12:38 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "HttpReq.hpp"
# include "HttpRes.hpp"
# include "Server.hpp"
# include "webserv.hpp"

class HttpRes;
class HttpReq;
class Server;

typedef enum e_status_c {
	WANT_TO_SEND_REQ,
	SENDING_REQ_HEADER,
	SENDING_REQ_BODY,
	REQ_SENT,
	WAITING_FOR_RES,
	RECIVING_RES_HEADER,
	RECIVING_RES_BODY,
	RES_SENT, 
	ERROR_WHILE_SENDING
}				t_status_c;

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
	std::string				_req_recived;
	std::string				_req_header;
	std::string				_req_body;
	int						_id;
	int						_byte_sent_header;
	int						_byte_sent_body;
	struct timeval			_last_activity;
	std::ifstream 			_file_to_send;
	int						_file_to_send_size;
	
	static bool				_verbose;
	static	int				_count;
	
public:
	Client(void);
	Client(struct sockaddr_in _client_addr, int com_socket);
	Client(Client const & copy);
	~Client(void);

	Client	&operator=(Client const & client);

	void						AddServerPtr(Server * new_server_ptr);
	int							getCom_socket(void) const;
	void						SetStatus(t_status_c status);
	t_status_c					getStatus(void) const;
	struct sockaddr_in		 	getClient_addr(void) const;
	int							getId(void) const;
	void						print_ClientServer(void) const;
	int							get_byte_sent_header(void) const;
	void						set_byte_sent_header(int byte);
	void						send_response(void);
	void						send_response_header(void);
	void						send_response_body(void);
	void						send_response_body_error(void);
	void						send_response_body_normal_file(void);
	void						send_response_body_cgi(void);
	int							receive_request(void);
	void						reset_last_activity(void);
	unsigned long				time_since_last_activity_us(void) const;
	void						reset_client(void);
	
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