/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 20:36:38 by mbocquel          #+#    #+#             */
/*   Updated: 2023/09/08 16:46:28 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "webserv.hpp"
#include "ConfParser.hpp"
#include "Server.hpp"
#include "Launcher.hpp"

void	handle_sigint(int sig)
{
	throw Launcher::SigException();
	(void)sig;
}

void	handle_sigpipe(int sig)
{
	(void)sig;
}

int main(int ac, char **av)
{
	if (ac > 2)
	{
		std::cerr << "Error: Too many arguments" << std::endl;
		return (1);
	}
	std::string path;
	if (ac == 1)
		path = "./configs/default.conf";
	else
		path = av[1];
	Launcher launcher(path);
	try {
		signal(SIGINT, &handle_sigint);
		signal(SIGPIPE, &handle_sigpipe);
		launcher.testFolderTmp();
		launcher.parse();
		launcher.launchServers();
	}
	catch (Launcher::SigException &e) {
		std::cerr << std::endl << "See you soon!" << std::endl;
		return (EXIT_SUCCESS);
	}
	catch (CGI::CGIexception &e)
	{
		return (EXIT_SUCCESS);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (0);
}