/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:33 by mbocquel          #+#    #+#             */
/*   Updated: 2023/08/08 10:36:16 by mbocquel         ###   ########.fr       */
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


typedef enum e_type_fd {
	COM_SOCKET,
	CGI_R_PIPE,
	CGI_W_PIPE,
	FILE_R,
	FILE_W, 
	NOT_MINE
}				t_fd;

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
	void							process_reading_fd(int i);
	void							process_writing_fd(int i);
	void							print_situation(void);
	std::list<Client>::iterator		find_client(int socket);
	void							initiate_servers_listening(void);
	void							check_timeout_clients(void);
	void							remove_client(std::list<Client>::iterator client);
	void							test_folder_tmp(void) const;
	void							remove_fd_from_epoll(int fd);
	void							add_fd_to_epoll_in(int fd);
	void							add_fd_to_epoll_out(int fd);	
	
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