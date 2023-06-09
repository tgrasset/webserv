/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:09:21 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/09 19:53:47 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "HttpReq.hpp"
# include "HttpRes.hpp"

class HttpRes;
class HttpReq;

typedef enum e_status_c {
	WANT_TO_SEND_REQ,
	RECIVING_REQ,
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
	std::vector<HttpReq>	_req;
	std::vector<HttpRes>	_res;
	
public:
	Client(void);
	Client(Client const & copy);
	~Client(void);

	Client	&operator=(Client const & client);
};

#endif