/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/16 18:13:31 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RES_HPP
# define HTTP_RES_HPP

# include "webserv.hpp"
# include "HttpReq.hpp"
# include "Server.hpp"

typedef enum e_type_c {
	NORMALFILE,
	PYTHON,
	PHP,
	REDIRECTION,
	FORBIDDEN,
	NOT_FOUND,
	NOT_ACCEPTABLE,
	AUTOINDEX
}				r_type;

class HttpRes
{	
	public:

		HttpRes(HttpReq &request, std::vector<Server *> servers);
		HttpRes(HttpRes const & copy);
		~HttpRes(void);

		HttpRes	&operator=(HttpRes const & http_res);

		std::string							getHttpVersion() const;
		int									getStatusCode() const;
		std::string							getMethod() const;
		std::string 						getStatusMessage() const;
		std::map<std::string, std::string> 	getHeader() const;
		std::string							getBody() const;
		Server								*getServer() const;
		std::string							getToSend() const;
		bool								getKeepAlive() const;
		std::string							getUriPath() const;
		std::string 						getUriQuery() const;
		r_type								getResourceType() const;
		Location							*getLocation() const;
		size_t								getContentLength() const;

		void	handleRequest(HttpReq &request, std::vector<Server *> servers);
		void	setServer(std::string reqHost, std::vector<Server *> servers);
		int		checkHttpVersion(std::string version, bool &error);
		int		checkMethod(std::string method, bool &error);
		int		checkUri(std::string uri);
		r_type	checkResourceType();
		int		checkRequestBodySize(int maxSize, int bodySize, bool &error);
		int		checkRequestHeader(std::map<std::string, std::string> header, bool &error);
		void	bodyBuild();
		void	headerBuild();
		void	formatResponse();
		void	fillMimeTypes();
		void	checkIfAcceptable(std::vector<std::string> acceptable);
		

	private:
	
		HttpRes(void);
		static bool							_verbose;
		
		std::string							_httpVersion;
		int									_statusCode;
		std::string							_method;
		std::string							_statusMessage;
		std::map<std::string, std::string>	_header;
		std::string							_body;
		Server								*_server;
		Location							*_location;
		std::string							_toSend;
		bool								_keepAlive;
		std::string							_uriPath;
		r_type								_resourceType;
		std::string							_uriQuery;
		size_t								_contentLength;
		static std::map<std::string, std::string> _mimeTypes;
};

#endif