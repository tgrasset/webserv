#include "webserv.hpp"
#include "Server.hpp"

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
	for (unsigned long i = 0; i < str.size(); i++)
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

std::string	sizeToString(size_t n) {

	std::stringstream s;
	s << n;
	return (s.str());
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

std::string	getTimestamp(void)
{
	time_t				rawtime;
	struct tm 			*timeinfo;
	std::ostringstream	os;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	if (timeinfo->tm_mday < 9)
		os << "0";
	os << timeinfo->tm_mday;
	os << "/";
	if (timeinfo->tm_mon < 9)
		os << "0";
	os << timeinfo->tm_mon + 1;
	os << "/";
	os << timeinfo->tm_year + 1900;
	os << " ";
	if (timeinfo->tm_hour <= 9)
		os << "0";
	os << timeinfo->tm_hour << ":";
	if (timeinfo->tm_min <= 9)
		os << "0";
	os << timeinfo->tm_min << ":";
	if (timeinfo->tm_sec <= 9)
		os << "0";
	os << timeinfo->tm_sec;
	os << "	";

	return (os.str());
}

std::string	getTimestampFileName(void)
{
	time_t				rawtime;
	struct tm 			*timeinfo;
	std::ostringstream	os;

	time (&rawtime);
	timeinfo = localtime (&rawtime);
	os << timeinfo->tm_year + 1900;
	if (timeinfo->tm_mon < 9)
		os << "0";
	os << timeinfo->tm_mon + 1;
	if (timeinfo->tm_mday < 9)
		os << "0";
	os << timeinfo->tm_mday;
	os << "_";
	if (timeinfo->tm_hour <= 9)
		os << "0";
	os << timeinfo->tm_hour;
	if (timeinfo->tm_min <= 9)
		os << "0";
	os << timeinfo->tm_min;
	if (timeinfo->tm_sec <= 9)
		os << "0";
	os << timeinfo->tm_sec;
	return (os.str());
}

std::string  timeStamp(void) {

	std::stringstream str;
    std::time_t t = time(NULL);
    std::tm *gmt = gmtime(&t);
	std::map<int, std::string> days;
	std::map<int, std::string> mon;

	days[0] = "Sun";
	days[1] = "Mon";
	days[2] = "Tue";
	days[3] = "Wed";
	days[4] = "Thu";
	days[5] = "Fri";
	days[6] = "Sat";

	mon[0] = "Jan";
	mon[1] = "Feb";
	mon[2] = "Mar";
	mon[3] = "Apr";
	mon[4] = "May";
	mon[5] = "Jun";
	mon[6] = "Jul";
	mon[7] = "Aug";
	mon[8] = "Sep";
	mon[9] = "Oct";
	mon[10] = "Nov";
	mon[11] = "Dec";

	str << days[gmt->tm_wday] << ", " << std::setw(2) << std::setfill('0') << gmt->tm_mday << " " << mon[gmt->tm_mon] << " " << gmt->tm_year + 1900 << " " << std::setw(2) << std::setfill('0') << gmt->tm_hour << ":" << std::setw(2) << std::setfill('0') << gmt->tm_min << ":" << std::setw(2) << std::setfill('0') << gmt->tm_sec << " GMT";
	return (str.str());
}

std::string redirectionHTML(int code, std::string message, std::string path) {

	std::stringstream s;
	s << "<!doctype html>\n<html>\n<head>\n<title>" << code << " : " << message << "</title>\n<meta http-equiv=\"refresh\" content=\"3; URL=" << path << "\">\n</head>";
	s << "<body>" << code << " : " << message << "\n<br>\nRedirection to " << path <<  " in 3 seconds.\n</body>\n</html>\n";
	return (s.str());
}

std::string autoindexHTML(std::string dirPath, std::string requestUri) {

	std::string path;
	struct stat test;
	struct tm * timeinfo;
	struct dirent *dirStruct;
	if (requestUri[requestUri.length() - 1] == '/')
		requestUri = requestUri.substr(0, requestUri.length() - 1);
	DIR *dir = opendir(dirPath.c_str());
	if (dir == NULL)
		return ("<!doctype html>\n<html>\n<head>\n<title>Error 403</title>\n</head>\n<body>\n<p>403 : Forbidden</p>\n</body>\n</html>\n");
	std::stringstream s;
	s << "<!doctype html>\n<html>\n<head>\n<title>Index of ";
	if (requestUri == "")
		s << "/";
	else
		s << requestUri; 
	s << "</title>\n</head>\n<body>\n<h1>Index of ";
	if (requestUri == "")
		s << "/";
	else
		s << requestUri;
	s << "</h1>\n";
	s << "<table style=\"width:80%; font-size:15pixel\">\n<hr>\n<th style=\"text-align:left\">File name</th>\n<th style=\"text-align:center\">Last modified</th>\n<th style=\"text-align:right\">Size</th>\n";
	while ((dirStruct = readdir(dir)) != NULL)
	{
		if (strcmp(dirStruct->d_name, ".") == 0)
			continue;	
		path = dirPath + "/" + dirStruct->d_name;
		stat(path.c_str(), &test);
		std::string name(dirStruct->d_name);
		timeinfo = localtime (&test.st_mtim.tv_sec);

		if (dirStruct->d_type == DT_DIR)
			name += "/";
		s << "<tr>\n<td>";
		if (dirStruct->d_type == DT_DIR)
			s << "<b>";
		s << "<a href=\"" << requestUri << "/" << name << "\">" << name << "</a>";
		if (dirStruct->d_type == DT_DIR)
			s << "</b>";
		s << "</td>\n";
		s << "<td style=\"text-align:center\">" << timeinfo->tm_year + 1900 << "-";
		if (timeinfo->tm_mon < 9)
			s << "0";
		s << timeinfo->tm_mon + 1 << "-";
		if (timeinfo->tm_mday < 9)
			s << "0";
		s << timeinfo->tm_mday << " ";
		if (timeinfo->tm_hour < 9)
			s << "0";
		s << timeinfo->tm_hour << ":";
		if (timeinfo->tm_min < 9)
			s << "0"; 
		s << timeinfo->tm_min << ":";
		if (timeinfo->tm_sec < 9)
			s << "0";
		s << timeinfo->tm_sec << "</td>\n";
		s << "<td style=\"text-align: right\">";
		if (dirStruct->d_type == DT_DIR)
			s << "-";
		else
		{
			s << std::setprecision(2);
			s << std::setiosflags(std::ios::fixed);
			if (test.st_size < 1000)
				s << test.st_size << " B </td>\n";
			else if (test.st_size < 1000000)
				s << static_cast<double>(test.st_size) / 1000 << " KB </td>\n";
			else
				s << static_cast<double>(test.st_size) / 1000000 << " MB </td>\n";
		}
		s << "</td>\n</tr>\n";
	}
	closedir(dir);
	s << "</table><hr></body>\n</html>\n";
	return (s.str());
}

std::string errorHTML(int code, std::string message) {
	
	std::stringstream s;
	s << "<!doctype html>\n<html>\n<head>\n<title>" << code << "</title>\n</head>\n<body>\n<p>" << code << " : " << message << "</p>\n</body>\n</html>\n";
	return (s.str());
}

std::string	getMimeType(std::string path, std::map<std::string, std::string> &types) {

	size_t point = path.rfind('.');
	if (point == std::string::npos || point == path.length() -1)
		return ("text/plain");
	std::string ext = path.substr(point, path.length() - point);
	if (types.find(ext) != types.end())
		return (types[ext]);
	return ("text/plain");
}

void	printVectorChar(std::vector<char> vectChar)
{
	for (std::vector<char>::iterator it = vectChar.begin(); it != vectChar.end(); ++it)
	{
		std::cout << *it;
	}
	std::cout << std::endl;
}
