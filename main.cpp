/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlanza <jlanza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 20:36:38 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/19 16:26:12 by jlanza           ###   ########.fr       */
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

/* int main(int ac, char **av)
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
} */

int main(void)
{
	std::string	std;
	std = "GET /page.html HTTP/1.0\r\n\
Host: example.com\r\n\
Referer: http://example.com/\r\n\
User-Agent: CERN-LineMode/2.15 libwww/2.17b3\r\n\
Connection: keep-alive\r\n\
Content-Length: 220\r\n\
Content-Type: text/html; charset=utf-8\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\r\n\
\r\n";

	HttpReq toto(std);
}
