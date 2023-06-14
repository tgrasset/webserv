/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:21 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/14 14:57:10 by tgrasset         ###   ########.fr       */
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
	SENDING_REQ,
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
	
public:
	Client(void);
	Client(struct sockaddr_in _client_addr, int com_socket);
	Client(Client const & copy);
	~Client(void);

	Client	&operator=(Client const & client);

	void		AddServerPtr(Server * new_server_ptr);
	int			getCom_socket(void) const;
	void		add_to_req_recived(char *str);
	void		SetStatus(t_status_c status);
	t_status_c	getStatus(void) const;

};

#endif