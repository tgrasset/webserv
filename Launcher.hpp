/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:33 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/12 17:20:45 by mbocquel         ###   ########.fr       */
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
	std::vector<Server> _servers;
	std::vector<Client> _clients;
	std::string			_path_conf;
	static bool			_verbose;
	Launcher(void);
	
public:
	Launcher(std::string path);
	Launcher(Launcher const & copy);
	~Launcher(void);
	
	Launcher & operator=(Launcher const & launcher);

	void	launch_servers(void); //Fonction avec boucle globale
	void	parse(void);

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