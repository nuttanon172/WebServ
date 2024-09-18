#include "Request.hpp"

Request::Request(ServerConfig *serverBlock) : serverBlock(serverBlock)
{

}

Request::Request(const Request &obj)
{
	*this = obj;
}

Request &Request::operator=(const Request &obj)
{
	if (this != &obj)
	{
		inputStream << obj.inputStream.rdbuf();
		body << obj.body.rdbuf();
		method = obj.method;
		path = obj.method;
		req_path = obj.method;
		boundaryStart = obj.boundaryStart;
		boundaryEnd = obj.boundaryEnd;
		serverBlock = obj.serverBlock;
		header_map = obj.header_map;
	}
	return *this;
}

Request::~Request()
{
	inputStream.clear();
	body.clear();
}

void Request::writeStream(char *str, int size)
{
	if (size < 0)
		return;
	inputStream.write(str, size);
}

bool Request::parseRequestLine()
{
	std::string buffer = "";
	std::getline(inputStream, buffer);
	std::stringstream stream(buffer);

	stream >> method;
	stream >> path;
	path = filterSlashes(path);
	req_path = path;
	path = makePath(path); // return full path
	//std::cout << "Root: " << serverBlock->root << '\n';
	std::cout << "Request Full Path: " << path << '\n';
	std::cout << "Request Path: " << req_path << '\n';
	return (true);
}

std::string Request::makePath(std::string &dest)
{
	std::size_t pos;
	std::string fileName;

	fileName = dest;
	if (serverBlock->root[serverBlock->root.size() - 1] != '/' && dest.size() != 1)
		return (serverBlock->root + fileName);
	else if (serverBlock->root[serverBlock->root.size() - 1] == '/' && dest.size() > 1)
	{
		pos = dest.find_first_not_of('/');
		fileName = dest.substr(pos);
		return (serverBlock->root + fileName);
		//std::cout << "pos: " << pos << '\n';
	}
	else
		return (serverBlock->root);
}

void Request::setPath(std::string &newPath)
{
	path = makePath(newPath);
	std::cout << "new path = " << path << '\n';
}

bool Request::parseHttpHeaders()
{
	std::string buffer = "";
	std::size_t colon;
	std::string key;
	std::string value;
	
	std::getline(inputStream, buffer);
	while (buffer.length() && buffer != "\r\n") // '\r\n' end line
	{
		colon = buffer.find_first_of(':');
		key = buffer.substr(0, colon);
		value = buffer.substr(colon + 2, buffer.size() - (colon + 2) - 1); // -1 trim \r
		header_map[key] = value;
		std::getline(inputStream, buffer);
	}
	return (true);
}

bool Request::parseBody()
{
	std::string buffer = "";
	body << inputStream.str();
	std::getline(inputStream, buffer);
	if (buffer != boundaryStart)
		return (false);
	while (std::getline(inputStream, buffer))
	{
		if (buffer == boundaryEnd)
			return (true);
	}
	return (false);
}

bool Request::isMultipart()
{
	if (header_map["Content-Type"].empty())
		return false;
	else if (header_map["Content-Type"].substr(0, 28) != "multipart/form-data; boundary")
		return false;
	size_t pos = header_map["Content-Type"].find("boundary=");
	if (pos != std::string::npos)
	{
		boundaryStart = header_map["Content-Type"].substr(pos + 9);
		boundaryEnd = "--" + boundaryEnd + "--";
	}
	return (true);
}

std::string &Request::getPath()
{
	return path;
}

std::string &Request::getReqPath()
{
	return req_path;
}

std::string &Request::getMethod()
{
	return method;
}

std::map<std::string, std::string> &Request::getHeaderMap()
{
	return header_map;
}

std::string &Request::getHost()
{
	return header_map["Host"];
}

std::stringstream &Request::getBody()
{
	return body;
}
