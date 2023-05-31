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

int	stringToInt(std::string str) {

	if (str.length() > 10)
		return (-42);
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (std::isdigit(str[i] == 0))
			return (-42);
	}
	std::stringstream stream(str);
	int n;
	stream >> n;
	return (n);
}

bool	isValidConfValue(std::string &str) {

	if (str.rfind(';') != str.length() - 1)
		return (false);
	str.erase(str.length() - 1);
	return (true);
}