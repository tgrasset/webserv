/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbocquel <mbocquel@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 18:53:19 by jlanza            #+#    #+#             */
/*   Updated: 2023/08/08 09:49:30 by mbocquel         ###   ########.fr       */
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

//Attention, revoir les execptions, je kill me que ca reussisse ou que ca echoue
void	CGI::setUpEnv(void)
{

	setenv("SERVER_SOFTWARE","Webserv/1.0", 1);
	setenv("SERVER_NAME",this->_res->getServer()->getServerName().c_str(), 1);
	setenv("GATEWAY_INTERFACE","CGI/1.1", 1);

	setenv("SERVER_PROTOCOL","HTTP/1.1", 1);
	std::ostringstream oss1;
	oss1 << ntohs(_res->getServer()->getPort());
	std::string portString = oss1.str();
	setenv("SERVER_PORT", portString.c_str(), 1);
	setenv("REQUEST_METHOD", _request->getMethod().c_str(), 1);
	setenv("PATH_INFO", _res->getUriPathInfo().c_str(), 1);
	setenv("PATH_TRANSLATED", _res->getUriPath().c_str(), 1);
	setenv("SCRIPT_NAME", _res->getScriptName().c_str(), 1);
	if (_res->getMethod() == "GET")
		setenv("QUERY_STRING", _res->getUriQuery().c_str(), 1);
	setenv("CONTENT_TYPE", _request->getContentType().c_str(), 1);
	std::ostringstream oss2;
	oss2 << _request->getContentLength();
	std::string contentLen = oss2.str();
	setenv("CONTENT_LENGTH", contentLen.c_str(), 1);
}

void	CGI::execCGI(void)
{
	// CREATE PIPE FOR OUTPUT
	// int	fd_pipe[2];
	// if (pipe(fd_pipe) == -1)
	// {
	// 	std::cerr << "Failed to create pipe." << std::endl;
	// 	_res->setStatusCode(500);
	// 	return ;
	// }
	//_res->setCgiPipeFd(fd_pipe[0]);

	// FORK
	_res->setCgiPid(fork());
	if (_res->getCgiPid() == -1)
	{
		std::cerr << "Failed to fork." << std::endl;
		_res->setStatusCode(500);
		return ;
	}
	if (_res->getCgiPid() == 0)
	{
		//DUP FOR INPUT
		int	file_fd;
		if (_res->getMethod() == "POST")
		{
			file_fd = open(_request->getBodyTmpFilePath().c_str(), O_RDONLY);
			if (file_fd == -1)
				killMe();
			if (dup2(file_fd, 0) == -1)
				killMe();
		}

		// DUP FOR OUTPUT
		// close(fd_pipe[0]);
		// if (dup2(fd_pipe[1], STDOUT_FILENO) == -1)
		// {
		// 	std::cerr << "Failed to dup" << std::endl;
		// 	killMe();
		// }

		//SETUP ENV
		this->setUpEnv();

		//SETUP EXECVE
		char *cmd[3];

		std::string cmd0;
		if (_res->getResourceType() == PYTHON)
			cmd0 = "python3";
		else if (_res->getResourceType() == PHP)
			cmd0 = "php-cgi";
		cmd[0] = const_cast<char *>(cmd0.c_str());
		std::string cmd1 = ("./" + _res->getUriPath());
		cmd[1] = const_cast<char *>(cmd1.c_str());
		cmd[2] = NULL;

		std::string		pathToExec;
		if (_res->getResourceType() == PYTHON)
			pathToExec = _res->getLocation()->getCgiExtensionAndPath()[".py"];
		else if (_res->getResourceType() == PHP)
		{
			pathToExec = _res->getLocation()->getCgiExtensionAndPath()[".php"];
			setenv("REDIRECT_STATUS", "200", 1);
		}

		//EXECUTION
		if (execve(pathToExec.c_str(), cmd, environ) == -1)
		{
			if (_res->getMethod() == "POST")
				close(file_fd);
			killMe();
		}
	}
	else
	{
		//close(fd_pipe[0]);
		//close(fd_pipe[1]);
		//waitpid(_res->getCgiPid(), NULL, 0);
	}
}

void	CGI::killMe(void)
{
	throw CGIexception();
}
