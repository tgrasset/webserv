/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tgrasset <tgrasset@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 20:36:38 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/14 14:36:40 by tgrasset         ###   ########.fr       */
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
		launcher.parse();
		launcher.launch_servers();
	}
	catch (Launcher::SigException &e) {
		std::cerr << std::endl << "See you soon!" << std::endl;
		return (EXIT_SUCCESS);
	}
	catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (EXIT_FAILURE);
	}
	return (0);
}
