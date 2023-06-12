/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:41 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/12 18:16:37 by mbocquel         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Launcher.hpp"

bool	Launcher::_verbose = true;
/* ************************************************************************** */
/*                     Constructeurs et destructeurs                          */
/* ************************************************************************** */
Launcher::Launcher(std::string path)
{
	if (Launcher::_verbose)
		std::cout << "Launcher path constructor called" << std::endl;
	this->_path_conf = path;
	return ;
}

Launcher::Launcher(Launcher const & copy)
{
	*this = copy;
	if (Launcher::_verbose)
		std::cout << "Launcher copy constructor called" << std::endl;
}

Launcher::~Launcher(void)
{
	if (Launcher::_verbose)
		std::cout << "Launcher destructor called" << std::endl;
}

/* ************************************************************************** */
/*                     Surcharge d'operateur                                  */
/* ************************************************************************** */
Launcher & Launcher::operator=(Launcher const & launcher)
{
	if (this != &launcher)
	{
		this->_servers = launcher._servers;
		this->_clients = launcher._clients;
	}
	return (*this);
}

/* ************************************************************************** */
/*                     Methodes                                               */
/* ************************************************************************** */

void	Launcher::parse(void)
{
	ConfParser conf_parser(this->_path_conf);
	conf_parser.parse();
	this->_servers = conf_parser.getServers();
}

void	Launcher::launch_servers(void)
{
	for (std::vector<Server>::iterator it = this->_servers.begin();
	it != this->_servers.end(); ++it)
	{
		int listensocket = socket(AF_INET, SOCK_STREAM, 0);
		if (listensocket < 0)
			throw LauncherException("Socket error");
		it->setListenSocket(listensocket);
		it->setServaddr();
		it->bind_server();
		if (listen(it->getListenSocket(), MAX_WAIT) < 0)
			throw LauncherException("Impossible de listen !");
	}
	/*while (1)
	{
		
	}*/
}