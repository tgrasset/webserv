/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Launcher.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 18:38:41 by mbocquel          #+#    #+#             */
/*   Updated: 2023/06/12 16:05:22 by mbocquel         ###   ########.fr       */
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
	if (this->_conf_parser)
		delete this->_conf_parser;
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
		if (this->_conf_parser)
			delete this->_conf_parser;
		this->_conf_parser = new ConfParser(*(launcher._conf_parser));
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
	this->_conf_parser = new ConfParser(this->_path_conf);
	this->_conf_parser->parse();
	this->_servers = this->_conf_parser->getServers();
}

void	Launcher::launch_servers(void)
{
	
}