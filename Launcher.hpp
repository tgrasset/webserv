/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:33 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/13 14:57:53 by mbocquel         ###   ########.fr       */
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
	std::vector<Client> 			_clients;
	std::string						_path_conf;
	static bool						_verbose;
	std::vector<struct epoll_event>	_ep_event;
	int 							_efd;
	Launcher(void);
	
public:
	Launcher(std::string path);
	Launcher(Launcher const & copy);
	~Launcher(void);
	
	Launcher & operator=(Launcher const & launcher);

	void							launch_servers(void); //Fonction avec boucle globale
	void							parse(void);
	std::vector<Server>::iterator	getServerWithSameHostPort(std::vector<Server>::iterator ite);
	void							process_new_client(int i);
	bool							check_if_listen_socket(int i);
	void				 			add_server_of_client(int listen_sock, Client *client);
	void							process_reading_existing_client(int i);


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
};

#endif