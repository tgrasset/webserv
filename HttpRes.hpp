/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlanza <jlanza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/23 18:37:36 by jlanza           ###   ########.fr       */
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

		HttpRes(HttpReq &request);
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
		std::string							getUriPathInfo() const;
		std::string 						getUriQuery() const;
		r_type								getResourceType() const;
		Location							*getLocation() const;
		size_t								getContentLength() const;
		int									getCgiPipeFd() const;
		pid_t								getCgiPid() const;

		void	handleRequest(HttpReq &request);
		int		checkHttpVersion(std::string version);
		int		checkUri(std::string uri);
		r_type	checkResourceType();
		int		checkRequestHeader(std::map<std::string, std::string> header);
		void	bodyBuild(std::string requestUri);
		void	headerBuild();
		void	formatHeader();
		void	fillMimeTypes();
		void	checkIfAcceptable(std::vector<std::string> acceptable);
		bool	methodIsAllowed(std::string method);
		void	uploadFileToServer(std::string tempFile);


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
		std::string							_uriPathInfo;
		r_type								_resourceType;
		std::string							_uriQuery;
		size_t								_contentLength;
		static std::map<std::string, std::string> _mimeTypes;
		int									_cgiPipeFd;
		pid_t								_cgiPid;
};

#endif
