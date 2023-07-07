/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlanza <jlanza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 18:53:19 by jlanza            #+#    #+#             */
/*   Updated: 2023/07/07 16:05:34 by jlanza           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(HttpReq &request, HttpRes &res)
{
	this->_request = &request;
	this->_res = &res;
}

CGI::~CGI()
{
}

CGI::CGI(CGI const & src)
{
	*this = src;
}

CGI & CGI::operator=(CGI const & rhs)
{
	this->_request = rhs._request;
	this->_res = rhs._res;
	return (*this);
}

extern	char** environ;

void	CGI::setUpEnv(void)
{

	setenv("SERVER_SOFTWARE","Webserv/1.0", 1);
	setenv("SERVER_NAME",this->_res->getServer()->getServerName().c_str(), 1);
	setenv("GATEWAY_INTERFACE","CGI/1.1", 1);

	setenv("SERVER_PROTOCOL","HTTP/1.1", 1);
	std::ostringstream oss1; //verifier ca
	oss1 << _res->getServer()->getPort();
	std::string portString = oss1.str();
	setenv("SERVER_PORT", portString.c_str(), 1);
	setenv("REQUEST_METHOD=", _request->getMethod().c_str(), 1);
	setenv("PATH_INFO", _res->getUriPathInfo().c_str(), 1);
	setenv("PATH_TRANSLATED=", _res->getUriPath().c_str(), 1);
	setenv("SCRIPT_NAME", _res->getScriptName().c_str(), 1);
	setenv("QUERY_STRING", _res->getUriQuery().c_str(), 1);
	setenv("CONTENT_TYPE", _request->getContentType().c_str(), 1);
	std::ostringstream oss2;
	oss2 << _request->getContentLength();
	std::string contentLen = oss2.str();
	setenv("CONTENT_LENGTH", contentLen.c_str(), 1);

}

void	CGI::execCGI(void)
{
	int	fd_pipe[2];
	if (pipe(fd_pipe) == -1)
	{
		std::cerr << "Failed to create pipe." << std::endl;
		_res->setStatusCode(500);
		return ;
	}

	_res->setCgiPid(fd_pipe[0]);
	_res->setCgiPid(fork());
	if (_res->getCgiPid() == -1)
	{
		std::cerr << "Failed to fork." << std::endl;
		_res->setStatusCode(500);
		return ;
	}

	if (_res->getCgiPid() == 0)
	{
		close(fd_pipe[0]);
		if (dup2(fd_pipe[1], STDOUT_FILENO) == -1)
		{
			std::cerr << "Failed to dup" << std::endl;
			killMe();
		}
		//setup the env
		this->setUpEnv();

		//setup the execve
		char *cmd[3];

		std::string cmd0;
		if (_res->getResourceType() == PYTHON)
			cmd0 = "python3";
		else if (_res->getResourceType() == PHP)
			cmd0 = "php";
		cmd[0] = const_cast<char *>(cmd0.c_str());
		std::string cmd1 = ("./" + _res->getUriPath());
		cmd[1] = const_cast<char *>(cmd1.c_str());
		cmd[2] = NULL;
		//char **command = reinterpret_cast<char**>(&cmd[0]);
		std::string	pathToPython = _res->getLocation()->getCgiExtensionAndPath()[".py"];

		//execve
		std::cerr << pathToPython << std::endl << std::endl;
		std::cerr << cmd[0] << std::endl;
		std::cerr << cmd[1] << std::endl;

		for (int j = 0; environ[j] != NULL; j++)
			std::cerr << environ[j] << std::endl;

		if (execve(pathToPython.c_str(), cmd, environ) == -1)
		{
			std::cerr << "execve failed" << std::endl;
			killMe();
		}
	}

}

void	CGI::killMe(void)
{
	while (42)
		;
}
