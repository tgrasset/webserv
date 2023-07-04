/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:33 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/23 14:42:05 by mbocquel         ###   ########.fr       */
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
	bool							check_if_listen_socket(int socket);
	void				 			add_server_of_client(int listen_sock, Client *client);
	void							process_reading_existing_client(int i);
	void							process_writing_to_client(int i);
	void							print_situation(void);
	std::list<Client>::iterator		find_client(int socket);
	void							initiate_servers_listening(void);
	void							check_timeout_clients(void);
	void							remove_client(std::list<Client>::iterator client);
	void							test_folder_tmp(void) const;
	
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