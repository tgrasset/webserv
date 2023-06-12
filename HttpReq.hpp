/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:03 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/12 16:03:12 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQ_HPP
# define HTTP_REQ_HPP
# include "webserv.hpp"

class HttpReq
{
private:
	static bool		_verbose;
	
public:
	HttpReq(void);
	HttpReq(HttpReq const & copy);
	~HttpReq(void);

	HttpReq	&operator=(HttpReq const & http_req);
};


#endif