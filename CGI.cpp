/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jlanza <jlanza@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 18:53:19 by jlanza            #+#    #+#             */
/*   Updated: 2023/06/29 19:22:54 by jlanza           ###   ########.fr       */
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

void	CGI::setUpEnv(void)
{
	//1 convertir env en vector
	//2 ajouter les variables
	
	setenv("REQUEST_METHOD", _request.getMethod().c_str(), 1);
	setenv("CONTENT_TYPE", _request.getContentType().c_str(), 1);
	char contentLen[20];
	sprintf(contentLen, "%d", _request.getContentLength());
	setenv("CONTENT_LENGTH", contentLen, 1);
	setenv("QUERY_STRING", _res.getUriQuery().c_str(), 1);
}

char	**CGI::vector_to_char(std::vector<std::string> vector)
{

}
