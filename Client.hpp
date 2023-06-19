/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:21 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/19 14:56:19 by mbocquel         ###   ########.fr       */
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
	RECIVING_RES,
	RES_SENT
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
	static bool				_verbose;
	std::string				_req_recived;
	std::string				_req_header;
	std::string				_req_body;
	int						_id;
	int						_byte_sent;
	struct timeval			_last_activity;
	static int				_count;
	
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
	std::string					get_res_string(void) const;
	int							get_byte_sent(void) const;
	void						set_byte_sent(int byte);
	void						send_response(void);
	void						receive_request(void);
	void						reset_last_activity(void);
	unsigned long				time_since_last_activity_ms(void) const;
	
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