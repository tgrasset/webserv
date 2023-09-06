/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 18:53:45 by jlanza            #+#    #+#             */
/*   Updated: 2023/09/06 16:05:57 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <iostream>
# include "webserv.hpp"
# include "HttpRes.hpp"
# include "HttpReq.hpp"
# include "Client.hpp"

# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <sys/wait.h>

class HttpRes;
class HttpReq;
class Client;

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
	void	closeAllSocketsBeforeExec(void);

	class CGIexception : public std::exception {
            public :
				virtual const char* what() const throw() {
					return ("");
				}
        };

};

#endif
