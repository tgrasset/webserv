/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlanza <jlanza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 18:53:19 by jlanza            #+#    #+#             */
/*   Updated: 2023/07/02 22:13:23 by jlanza           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI(void)
{
}

CGI::CGI(HttpReq & request, HttpRes & res)
{
	this->_request = request;
	this->_res = res;
}

CGI::~CGI()
{
}

CGI::CGI(CGI const & src)
{
}

CGI & CGI::operator=(CGI const & rhs)
{
}

extern	char** environ;

char	**CGI::setUpEnv(void)
{
	//1 convertir env en vector
	std::vector<std::string>	vector_env;
	int	i;
	while (environ[i])
	{
		vector_env.push_back(environ[i]);
		i++;
	}

	//2 ajouter les variables

	vector_env.push_back("SERVER_SOFTWARE=Webserv/1.0");
	vector_env.push_back("SERVER_NAME="+this->_res.getServer()->getServerName());
	vector_env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	vector_env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	vector_env.push_back("SERVER_PORT="+_res.getServer()->getPort());
	vector_env.push_back("REQUEST_METHOD=" + _request.getMethod());
	vector_env.push_back("PATH_INFO=");// ajouter path info !!!!
	vector_env.push_back("CONTENT_TYPE="+ _request.getContentType());
	std::ostringstream oss;
	oss << _request.getContentLength();
	std::string contentLen = oss.str();
	vector_env.push_back("CONTENT_LENGTH="+ contentLen);
	vector_env.push_back("QUERY_STRING="+ _res.getUriQuery());
}

char	**CGI::vector_to_char(std::vector<std::string> vector)
{

}
