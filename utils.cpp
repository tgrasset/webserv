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

std::vector<std::string>    cpp_split_sep(std::string str, std::string sep) {

	std::vector<std::string> res;
	size_t start = 0;
	size_t end = 0;

	while (42)
	{
		if (start == std::string::npos)
			break;
		end = str.find(str, start);
		res.push_back(str.substr(start + sep.size(), end - start));
	}
	return (res);
}

bool	caseInsensitiveCmp(std::string const &a, std::string const &b) {

	size_t size = a.size();
	if (b.size() != size)
		return (false);
	for (size_t i = 0; i < size; i++)
	{
		if (tolower(a[i]) != tolower(b[i]))
			return (false);
	}
	return (true);
}

std::string toUpperCase(const std::string& str)
{
	std::string result;
	for (int i = 0; str.size(); i++)
	{
		result += std::toupper(str[i]);
	}
	return result;
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

bool    checkFile(std::string const &file, std::string const &root) {

    struct stat test;

	if (stat(file.c_str(), &test) == 0 && test.st_mode & S_IFREG && access(file.c_str(), R_OK) == 0)
        return (true);
    std::string full = root + "/" + file;
    if (stat(full.c_str(), &test) == 0 && test.st_mode & S_IFREG && access(full.c_str(), R_OK) == 0){
        return (true); }
    return (false);
}

bool	isValidConfValue(std::string &str) {

	if (str.rfind(';') != str.length() - 1 || str[0] == ';')
		return (false);
	str.erase(str.length() - 1);
	return (true);
}

std::string getStatus(int code) {

	switch (code)
	{
		case 100:
			return ("Continue");
		case 101:
			return ("Switching Protocols");
		case 103:
			return ("Early Hints");
		case 200:
			return ("OK");
		case 201:
			return ("Created");
		case 202:
			return ("Accepted");
		case 203:
			return ("Non-Authoritative Information");
		case 204:
			return ("No Content");
		case 205:
			return ("Reset Content");
		case 206:
			return ("Partial Content");
		case 300:
			return ("Multiple Choices");
		case 301:
			return ("Moved Permanently");
		case 302:
			return ("Found");
		case 303:
			return ("See Other");
		case 304:
			return ("Not Modified");
		case 307:
			return ("Temporary Redirect");
		case 308:
			return ("Permanent Redirect");
		case 400:
			return ("Bad Request");
		case 401:
			return ("Unauthorized");
		case 402:
			return ("Payment Required");
		case 403:
			return ("Forbidden");
		case 404:
			return ("Not Found");
		case 405:
			return ("Method Not Allowed");
		case 406:
			return ("Not Acceptable");
		case 407:
			return ("Proxy Authentication Required");
		case 408:
			return ("Request Timeout");
		case 409:
			return ("Conflict");
		case 410:
			return ("Gone");
		case 411:
			return ("Length Required");
		case 412:
			return ("Precondition Failed");
		case 413:
			return ("Payload Too Large");
		case 414:
			return ("URI Too Long");
		case 415:
			return ("Unsupported Media Type");
		case 416:
			return ("Range Not Satisfiable");
		case 417:
			return ("Expectation Failed");
		case 418:
			return ("I'm a teapot");
		case 421:
			return ("Misdirected Request");
		case 425:
			return ("Too Early");
		case 426:
			return ("Upgrade Required");
		case 428:
			return ("Precondition Required");
		case 429:
			return ("Too Many Requests");
		case 431:
			return ("Request Header Fields Too Large");
		case 451:
			return ("Unavailable For Legal Reasons");
		case 500:
			return ("Internal Server Error");
		case 501:
			return ("Not Implemented");
		case 502:
			return ("Bad Gateway");
		case 503:
			return ("Service Unavailable");
		case 504:
			return ("Gateway Timeout");
		case 505:
			return ("HTTP Version Not Supported");
		case 506:
			return ("Variant Also Negotiates");
		case 510:
			return ("Not Extended");
		case 511:
			return ("Network Authentication Required");
		default:
			return ("Unknown");
	}
}
