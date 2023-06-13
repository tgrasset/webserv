/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:03 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/13 11:40:34 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQ_HPP
# define HTTP_REQ_HPP
# include "webserv.hpp"

class HttpReq
{
private:

	static bool		_verbose;
	HttpReq(void);

	std::string							_method;
	std::string							_uri;
	std::string							_httpVersion;
	std::map<std::string, std::string>	_header;
	std::string							_body;
	
public:

	HttpReq(std::string &content);    // content sera la string a parser
	HttpReq(HttpReq const & copy);
	~HttpReq(void);

	HttpReq	&operator=(HttpReq const & http_req);

	std::string		getMethod() const;
	std::string		getUri() const;
	std::string		getHttpVersion() const;
	std::map<std::string, std::string> getHeader() const;
	std::string		getBody() const;
	
};


#endif