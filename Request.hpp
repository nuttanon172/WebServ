#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "WebServer.hpp"

class Request
{
private:
	std::stringstream inputStream;
	std::stringstream body;
	std::string method;
	std::string path;
	std::string boundaryStart;
	std::string boundaryEnd;
	std::map<std::string, std::string> header_map;

public:
	Request();
	~Request();
	void writeStream(char *str, int size);
	bool parseRequestLine();
	bool parseHttpHeaders();
	bool parseBody();
	bool isMultipart();
};

#endif
