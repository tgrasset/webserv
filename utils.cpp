#include "webserv.hpp"

std::vector<std::string>    cpp_split(std::string str, char const *charset) {

	std::vector<std::string> res;
	size_t start = 0;
	size_t end = 0;

	while (42)
	{
		start = str.find_first_not_of(charset, end);
		if (start == std::string::npos)
			break;
		end = str.find_first_of(charset, start);
		res.push_back(str.substr(start, end - start));
	}
	return (res);
}