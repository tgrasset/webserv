/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/14 18:08:45 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RES_HPP
# define HTTP_RES_HPP
# include "webserv.hpp"

class HttpRes
{
private:
	static bool		_verbose;
	std::string		_toSend;
	
public:
	HttpRes(void);
	HttpRes(HttpRes const & copy);
	~HttpRes(void);

	HttpRes	&operator=(HttpRes const & http_res);
	
	std::string			getToSend(void) const;
};

#endif