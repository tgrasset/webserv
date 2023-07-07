/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlanza <jlanza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 18:53:45 by jlanza            #+#    #+#             */
/*   Updated: 2023/07/07 14:13:24 by jlanza           ###   ########.fr       */
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

	void	setUpEnv(void);
	void	execCGI(void);
	void	killMe(void);

};

#endif
