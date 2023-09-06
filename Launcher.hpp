/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:33 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/06 16:04:12 by tgrasset         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LAUNCHER_HPP
# define LAUNCHER_HPP
# include "webserv.hpp"
# include "Server.hpp"
# include "Client.hpp"
# include "ConfParser.hpp"

class Server;
class Client;
class ConfParser;

class Launcher
{
private:
	std::vector<Server> 			_servers;
	std::list<Client> 				_clients;
	std::string						_pathConf;
	std::vector< struct pollfd >	_pollEvent;
	bool							_breakPollLoop;
							
	Launcher(void);
	
public:
	Launcher(std::string path);
	Launcher(Launcher const & copy);
	~Launcher(void);
	
	Launcher & operator=(Launcher const & launcher);

	void							launchServers(void); 
	void							parse(void);
	std::vector<Server>::iterator	getServerWithSameHostPort(std::vector<Server>::iterator ite);
	void							processNewClient(int fd);
	bool							checkIfListenSocket(int socket);
	void				 			addServerOfClient(int listen_sock, Client *client);
	void							processReadingFd(int fd);
	void							processWritingFd(int fd);
	void							printSituation(void);
	std::list<Client>::iterator		findClient(int socket);
	void							initiateServersListening(void);
	void							checkTimeoutClients(void);
	void							removeClient(std::list<Client>::iterator client);
	void							testFolderTmp(void) const;
	void							removeFdFromPoll(int fd);
	void							addFdToPollIn(int fd);
	void							addFdToPollOut(int fd);	
	void							printPollEvent(void) const;
	void							processCloseConnexionOrError(int fd);
	std::vector<Server>				&getServers(void);
	std::list<Client>				&getClients(void);
	
	class LauncherException : public std::exception {
	public :
		LauncherException(std::string errMessage) throw() {
			_errMessage = "Launcher Error: " + errMessage;
		}
		virtual const char* what() const throw() {
			return (_errMessage.c_str());
		}
		~LauncherException() throw() {}
	private:
		std::string _errMessage;
	};
	class SigException : public std::exception {};
};

#endif