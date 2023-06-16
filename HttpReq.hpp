/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:03 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/16 17:20:03 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_REQ_HPP
# define HTTP_REQ_HPP
# include "webserv.hpp"

class HttpReq
{
private:

	//static bool		_verbose;
	HttpReq(void);

	std::string							_method;
	std::string							_uri;
	std::string							_httpVersion;
	std::map<std::string, std::string>	_header;
	std::string							_host;
	std::vector<std::string>			_accept;
	std::string							_contentType;
	int									_contentLength;
	bool								_keepAlive;
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
	std::string		getHost() const;
	std::vector<std::string>	getAccept() const;
	std::string		getContentType() const;
	int				getContentLength() const;
	bool			getKeepAlive() const;
	std::string		getBody() const;
	void			setBody(std::string body);
	
};


#endif