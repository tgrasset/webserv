/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 18:53:45 by jlanza            #+#    #+#             */
/*   Updated: 2023/07/04 14:19:37 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include "webserv.hpp"
# include "HttpRes.hpp"
# include "HttpReq.hpp"

class HttpRes;
class HttpReq;

class CGI
{

private:
	CGI(void);
	HttpReq	*_request;
	HttpRes	*_res;

public:
	CGI(HttpReq &request, HttpRes &res);
	~CGI();

	CGI(CGI const & src);
	CGI & operator=(CGI const & rhs);

	char	**setUpEnv(void);
	char	**vector_to_char(std::vector<std::string> vector);
};

#endif
