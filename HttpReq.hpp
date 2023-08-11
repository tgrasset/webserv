/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpReq.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2023/08/11 16:30:04 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef HTTP_REQ_HPP
# define HTTP_REQ_HPP
# include "webserv.hpp"
# include "Server.hpp"
# include "Client.hpp"

typedef enum e_status_req {
	COMPLETED,
	WAITING_TO_FILL_BODY_FILE
}				status_req;

class Server;
class Client;

class HttpReq
{
private:
	Client								*_client;
	status_req							_status_req;
	std::string							_method;
	std::string							_uri;
	std::string							_httpVersion;
	std::map<std::string, std::string>	_header;
	std::string							_host;
	std::vector<std::string>			_accept;
	std::string							_contentType;
	unsigned int						_contentLength;
	bool								_keepAlive;
	std::string							_bodyTmpPath;
	s_file								_statusBodyFile;
	std::vector<char>					_toAddBodyFile;
	unsigned int						_byteRecivedReqBody;
	unsigned int						_byteWroteTmpBodyFile;
	int									_bodyTmpFileFd;
	std::ofstream 						_bodyFile;
	unsigned int						_id;
	Server								*_server;
	std::string							_boundary;

	static std::string					_bodyTmpFolder;
	static unsigned int					_count;
	
	HttpReq(void);
	void								parse(std::string &content, std::vector<Server *> servers);

public:

	HttpReq(Client *client, std::string &content, std::vector<Server *> servers);
	HttpReq(HttpReq const & copy);
	~HttpReq(void);

	HttpReq								&operator=(HttpReq const & http_req);

	std::string							getMethod() const;
	std::string							getUri() const;
	std::string							getHttpVersion() const;
	std::map<std::string, std::string>	getHeader() const;
	std::string							getHost() const;
	std::vector<std::string>			getAccept() const;
	std::string							getContentType() const;
	unsigned int						getContentLength() const;
	bool								getKeepAlive() const;
	std::string							getBodyTmpFilePath() const;
	int									getBodyTmpFileFd() const;
	Server 								*getServer() const;
	std::string							getBoundary() const;
	void								addToBodyFileBuff(std::vector<char> str);
	void								closeBodyFile(void);
	void								setServer(std::vector<Server *> servers);
	bool								bodyIsTooBig(void) const;
	status_req							getStatusReq(void) const;
	void								writeOnReqBodyFile(void);
	

	class HttpReqException : public std::exception {
	public :
		HttpReqException(std::string errMessage) throw() {
			_errMessage = "HttpReq Error: " + errMessage;
		}
		virtual const char* what() const throw() {
			return (_errMessage.c_str());
		}
		~HttpReqException() throw() {}
	private:
		std::string _errMessage;
	};
};


#endif
