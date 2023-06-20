/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/20 16:23:11 by tgrasset         ###   ########.fr       */
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
		std::string							getFormattedHeader() const;
		bool								getKeepAlive() const;
		std::string							getUriPath() const;
		std::string 						getUriQuery() const;
		r_type								getResourceType() const;
		Location							*getLocation() const;
		size_t								getContentLength() const;
		int									getCgiPipeFd() const;
		pid_t								getCgiPid() const;

		void	handleRequest(HttpReq &request, std::vector<Server *> servers);
		void	setServer(std::string reqHost, std::vector<Server *> servers);
		int		checkHttpVersion(std::string version, bool &error);
		int		checkUri(std::string uri);
		r_type	checkResourceType();
		int		checkRequestBodySize(int maxSize, int bodySize, bool &error);
		int		checkRequestHeader(std::map<std::string, std::string> header, bool &error);
		void	bodyBuild();
		void	headerBuild();
		void	formatHeader();
		void	fillMimeTypes();
		void	checkIfAcceptable(std::vector<std::string> acceptable);
		bool	methodIsAllowed(std::string method);
		void	buildCgiResponse(std::string cgiOutput, bool timeout); // LOL
		

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
		std::string							_formattedHeader;
		bool								_keepAlive;
		std::string							_uriPath;
		r_type								_resourceType;
		std::string							_uriQuery;
		size_t								_contentLength;
		static std::map<std::string, std::string> _mimeTypes;
		int									_cgiPipeFd;
		pid_t								_cgiPid;
};

#endif