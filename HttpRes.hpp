/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/12 16:03:07 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RES_HPP
# define HTTP_RES_HPP
# include "webserv.hpp"

class HttpRes
{
private:
	static bool		_verbose;
	
public:
	HttpRes(void);
	HttpRes(HttpRes const & copy);
	~HttpRes(void);

	HttpRes	&operator=(HttpRes const & http_res);
};

#endif