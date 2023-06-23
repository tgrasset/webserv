#include "webserv.hpp"

int	checkUri(std::string uri) {

	size_t separator = uri.find('?');
	//size_t end = uri.rfind('#');
	std::string tempPath;
	std::string locPath;
	size_t pos_py = uri.find(".py");
	size_t pos_php = uri.find(".php");

	std::string	_uriPathInfo;

	// Si on trouve ".py" ou ".php" dams uri, split apres ".php" ou ".py" et creer une autre variable _pathInfo dans la classe pour stocker ce qu'il y a apres
	if (pos_py == std::string::npos)
	{
		if (pos_php == std::string::npos)
			_uriPathInfo = "";
		else
		{
			if (separator != std::string::npos)
				_uriPathInfo = uri.substr(pos_php + 4, separator - pos_php - 4);
			else
				_uriPathInfo = uri.substr(pos_php + 4, uri.size());
			uri = uri.erase(pos_php + 4, _uriPathInfo.size());
		}
	}
	else
	{
		if (separator != std::string::npos)
			_uriPathInfo = uri.substr(pos_py + 3, separator - pos_py - 3);
		else
			_uriPathInfo = uri.substr(pos_py + 3, uri.size());
		uri = uri.erase(pos_py + 3, _uriPathInfo.size());
	}
	separator = uri.find('?');
	//end = uri.rfind('#');
	std::cout << "path_info: " << _uriPathInfo << std::endl;
	std::cout << "rest of uri: " << uri << std::endl;
	return 0;
}

int main(void)
{
	checkUri("http://example.com/cgi-bin/printenv.py/with/additional/path?and=a&query=string");
	checkUri("http://example.com/cgi-bin/printenv.php/with/additional/path");
}
