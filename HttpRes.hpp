/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 19:19:07 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/11 17:57:18 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTP_RES_HPP
# define HTTP_RES_HPP

# include "webserv.hpp"
# include "HttpReq.hpp"
# include "Server.hpp"
# include "CGI.hpp"
# include "Client.hpp"

class Client;
class HttpReq;
class Server;
class CGI;

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

		HttpRes(Client *client, HttpReq *request);
		HttpRes(HttpRes const & copy);
		~HttpRes(void);

		HttpRes	&operator=(HttpRes const & http_res);

		std::string							getHttpVersion(void) const;
		int									getStatusCode(void) const;
		std::string							getMethod(void) const;
		std::string 						getStatusMessage(void) const;
		std::map<std::string, std::string> 	getHeader(void) const;
		std::string							getBody(void) const;
		Server								*getServer(void) const;
		std::string							getFormattedHeader(void) const;
		bool								getKeepAlive(void) const;
		std::string							getUriPath(void) const;
		std::string							getUriPathInfo(void) const;
		std::string							getScriptName(void) const;
		std::string							getUriQuery(void) const;
		r_type								getResourceType(void) const;
		Location							*getLocation(void) const;
		size_t								getContentLength(void) const;
		std::string							getCgiFilePath(void) const;
		std::ifstream						getFileToSend(void) const;
		int									getFileToSendFd(void) const;
		size_t								getFileToSendSize(void) const;
		std::vector<char>					getFileToSendBuff(void) const;
		std::vector<char>					getCgiBuff(void) const;
		std::vector<char>					getCgiBuff_header(void) const;
		s_file								getStatusFileToSend(void) const;
		s_pipe								getStatusCgiPipe(void) const;
		void								openBodyFile(void);
		void								closeBodyFile(void);
		void								clearFileToSendBuff(void);
		void								clearCgiBuff(void);
		void								clearCgiBuff_header(void);
		int									addBodyFileToBuff(void);
		int									getUploadTmpInFd(void) const;
		int									getUploadOutFd(void) const;

		
		int									getCgiPipeFd(void) const;
		pid_t								getCgiPid(void) const;

		void								setStatusCode(int statusCode);
		void								setCgiPipeFd(int cgiPipeFd);
		void								setCgiPid(int cgiPid);

		void								handleRequest(void);
		int									checkHttpVersion(std::string version);
		int									checkUri(std::string uri);
		r_type								checkResourceType(void);
		int									checkRequestHeader(std::map<std::string, std::string> header);
		void								bodyBuild(std::string requestUri);
		void								headerBuild(void);
		void								formatHeader(void);
		void								fillMimeTypes(void);
		void								checkIfAcceptable(std::vector<std::string> acceptable);
		bool								methodIsAllowed(std::string method);
		void								uploadFileToServer(void);
		void								removeFdFromPoll(int fd);
		void								addFdToPollIn(int fd);
		void								addFdToPollOut(int fd);
		int									addCgiToBuff(void);
		void								closeCgiPipe(void);
		void								cgiPipeFinishedWriting(void);
		void								transferUploadFileInSide(void);
		void								transferUploadFileOutSide(void);
		void								finishBuildingResAfterUpload(void);
		bool								isAllowedCGI(void);
		std::string							getErrorPageContent(std::string path, int code, std::string message);

		class HttpResException : public std::exception {
			public :
				HttpResException(std::string errMessage) throw() {
					_errMessage = "HttpRes Error: " + errMessage;
				}
				virtual const char* what() const throw() {
					return (_errMessage.c_str());
				}
				~HttpResException() throw() {}
			private:
				std::string _errMessage;
	};
		


	private:

		HttpRes(void);

		Client										*_client;
		HttpReq 									*_request;
		std::string									_httpVersion;
		int											_statusCode;
		std::string									_method;
		std::string									_statusMessage;
		std::map<std::string, std::string>			_header;
		std::string									_body;
		Server										*_server;
		Location									*_location;
		std::string									_formattedHeader;
		bool										_keepAlive;
		std::string									_uriPath;
		std::string									_uriPathInfo;
		int											_fileToSendFd;
		size_t										_fileToSendSize;
		std::vector<char>							_fileToSendBuff;
		std::vector<char>							_cgiBuff;
		std::vector<char>							_cgiBuff_header;
		s_file										_statusFileToSend;
		std::string									_scriptName;
		r_type										_resourceType;
		std::string									_uriQuery;
		size_t										_contentLength;
		static std::map<std::string, std::string>	_mimeTypes;
		std::string									_cgiFilePath;
		s_pipe										_statusCgiPipe;
		int											_cgiPipeFd;
		pid_t										_cgiPid;
		int											_uploadTmpInFd;
		int											_uploadOutFd;
		std::ifstream								_uploadTmpInStream;
		std::ofstream								_uploadOutStream;
		std::string									_uploadBuff;
		bool										_uploadBuffClean;
		std::string									_nameTmpUploadFile;
		bool										_uploadFileHeader;
		bool										_uploadFileBody;
		bool										_uploadFileBodyFirstLine;
		bool										_backslashRFound;
		std::string									_nameFinalUploadFile;
		int											_percentSentFile;
		size_t										_byteWrittenUploadFile;
		size_t										_percentWrittenUploadFile;
		bool										_first_line_of_header;
		bool										_is_cgi;
};

#endif
