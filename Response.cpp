#include "Response.hpp"

Response::Response(ServerConfig *serverBlock) : serverBlock(serverBlock)
{
	status = initStatus();
	status_line = "";
	header = "";
	body = "";
	message = "";
}

Response::~Response()
{
	status_line = "";
	header = "";
	body = "";
	message = "";
}

void Response::buildHttpMessages()
{
	message = status_line + header + body;
}

void Response::buildStatusLine(int code)
{
	status_line = "HTTP/1.1 " + ft_itos(code) + " " + status[code] + "\r\n";
}

void Response::buildHeaders()
{
	std::stringstream ss;

	ss << "Date: " + getCurrentTime() + "\r\n";
	if (serverBlock->server_name.empty() != true)
		ss << "Server: " << serverBlock->server_name  << "\r\n";
	ss << "Connection: Keep-Alive\r\n";
	// ss << "Keep-Alive: timeout=6, max=2\r\n"; no directive require
	ss << "Content-Type: text/html; charset=UTF-8" << "\r\n";
	ss << "Content-Length: " << body.size() << "\r\n";
	ss << "\r\n";
	header = ss.str().c_str();
}

void Response::buildErrorBody(int code)
{
	std::stringstream ss;

	ss << "<!DOCTYPE html>\n";
	ss << "<html lang=\"en\">\n";
	ss << "<head>\n";
	ss << "<title>" << code << "-" << status[code] << "</title>\n";
	ss << "</head>\n";
	ss << "<body>\n";
	ss << "<h1>" << code << "</h1>\n";
	ss << "<h1>Error " << code << "</h1>\n";
	ss << "<p>" << status[code] << ".</p>"<< '\n';
	ss << "</body>\n";
	ss << "</html>\r\n";
	body = ss.str().c_str();
}

void Response::buildHttpStatus(int code, int socket)
{
	buildStatusLine(code);
	buildErrorBody(code);
	buildHeaders();
	buildHttpMessages();
	send(socket, message.c_str(), message.size(), 0);
}

void Response::readFile(std::string &path, int socket)
{
	std::ifstream inputFile(path.c_str());
	std::vector<std::string>::iterator it = serverBlock->index.begin();
	std::string indexPath;
	std::string tmp_path;
	std::string line;
	std::stringstream ss;
 	//check if it directory search for index inside else search for index
	if (isDirectory(path) == true || access(path.c_str(), R_OK) != 0)
	{
		if (path[path.size() - 1] != '/')
			tmp_path = path + "/";
		else 
			tmp_path = path;
		std::cout << "tmp_path: " << tmp_path << '\n';
		for (;it != serverBlock->index.end();it++)
		{
			indexPath = tmp_path + *it;
			std::cout << "FileName: " << *it << '\n';
			std::cout << "FilePath: " << indexPath << '\n';
			if (access(indexPath.c_str(), R_OK) == 0)
			{
				path = indexPath;
				this->readFile(path, socket);
				return ;
			}
		}
		tmp_path = tmp_path + serverBlock->error_pages[404];
		std::cout << "tmp_path: " << tmp_path << '\n';
		if (serverBlock->error_pages[404].empty() || access(tmp_path.c_str(), R_OK) != 0)
			this->buildHttpStatus(404, socket);
	}
	while (std::getline(inputFile, line))
		ss << line << "\r\n";
	body = ss.str();
}

bool Response::searchFile(Request *req, int socket)
{
	return true;
}

void Response::sendFavicon(int socket)
{
	std::cout << "sending favicon\n";
	buildStatusLine(200);
	std::string path = "/home/ntairatt/WebServ/docs/favicon.ico";
	readFile(path, socket);
	std::stringstream ss;
	ss << "Date: " + getCurrentTime() + "\r\n";
	ss << "Server: " << serverBlock->server_name << "\r\n";
	ss << "Connection: Keep-Alive\r\n";
	ss << "Content-Type: image/x-icon\r\n";
	ss << "Content-Length: " << body.size() << "\r\n";
	ss << "\r\n";
	header = ss.str();
	//buildHeaders();
	buildHttpMessages();
	/*std::cout << "path: " << path << '\n';
	std::cout << status_line;
	std::cout << header;
	std::cout << body;*/
	send(socket, message.c_str(), message.size(), 0);
}

void Response::serveFile(std::string &path, int socket)
{
	buildStatusLine(200);
	//path = "/home/ntairatt/WebServ/docs/fusion_web/index.html";
	readFile(path, socket);
	buildHeaders();
	buildHttpMessages();
	std::cout << "serveFile called\n";
	/*std::cout << "path: " << path << '\n';
	std::cout << status_line;
	std::cout << header;
	std::cout << body;*/
	send(socket, message.c_str(), message.size(), 0);
}

void Response::buildBody()
{

}
