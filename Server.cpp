#include "Server.hpp"

/*Server::Server(const std::vector<ServerConfig> &obj) : serverBlock(obj)
{
	initServer();
}*/

Server::Server(const std::string &pathConfig)
{
	if (!parseConfigFile(pathConfig, serverBlock))
	{
		std::cout << "Failed to parse config file" << std::endl;
		exit(EXIT_FAILURE);
	}
	printConfig(serverBlock);
	initServer();
}

Server::~Server()
{
	std::map<int, Client *>::iterator it = client_map.begin();
	for (; it != client_map.end(); it++)
		delete it->second;
}

void Server::initServer()
{
	max_socket = 0;
}

void Server::startServer()
{
	FD_ZERO(&current_sockets); // init set
	FD_ZERO(&ready_sockets);
	FD_ZERO(&listen_sockets);
	initSocket();
	checkClient();
}

void Server::initSocket()
{
	int enable = 1;

	// Socket for each port
	std::vector<ServerConfig>::iterator serverBlock_it = serverBlock.begin();
	std::vector<int>::iterator port_it;
	for (; serverBlock_it != serverBlock.end(); serverBlock_it++)
	{
		port_it = serverBlock_it->listen_ports.begin();
		for (; port_it != serverBlock_it->listen_ports.end(); port_it++)
		{
			std::cout << "Port: " << *port_it << std::endl;
			server_fd = socket(AF_INET, SOCK_STREAM, 0);
			if (server_fd < 0)
			{
				perror("Cannot create socket");
				exit(EXIT_FAILURE);
			}
			// Enable address reuse
			if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			{
				perror("setsockopt(SO_REUSEADDR) failed");
			}
			// SET SOCKET NONBLOCK
			if (fcntl(server_fd, F_SETFL, O_NONBLOCK) < 0)
			{
				perror("NONBLOCK ERROR");
				close(server_fd);
				exit(EXIT_FAILURE);
			}
			identifySocket(*port_it, *serverBlock_it);
		}
	}
	std::map<int, ServerConfig>::iterator it = server_config.begin();
	for (; it != server_config.end(); it++)
	{
		std::cout << "Socket: " << it->first << std::endl;
		std::cout << "Server: " << it->second.server_name << std::endl;
		std::cout << "Port: " << it->second.listen_ports[0] << std::endl;
	}
}

void Server::identifySocket(int port, ServerConfig &serverBlock)
{
	struct sockaddr_in address;

	memset((char *)&address, 0, sizeof(address));
	address.sin_family = AF_INET; // ipv4
	if (serverBlock.host == "127.0.0.1" || serverBlock.host == "0.0.0.0")
		address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 10) < 0)
	{
		perror("Listen ERROR");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	FD_SET(server_fd, &listen_sockets); /* add new socket to set */
	FD_SET(server_fd, &current_sockets);
	if (server_fd > max_socket)
		max_socket = server_fd;
	server_config.insert(std::make_pair(server_fd, serverBlock));
	std::cout << GREEN << "create socket[" << server_fd << "]\n"
			  << DEFAULT;
}

void Server::checkClient()
{
	int status;
	int new_max;

	while (1)
	{
		memcpy(&ready_sockets, &current_sockets, sizeof(current_sockets)); /* because select is destructive */
		status = select(max_socket + 1, &ready_sockets, NULL, NULL, NULL);
		std::cout << "waiting...\n";
		if (status < 0)
		{
			perror("Select Error");
			exit(EXIT_FAILURE);
		}
		for (int socket = 0; socket <= max_socket; socket++)
		{
			std::cout << "Socket: " << socket << '\n';
			std::cout << "Max_Socket: " << max_socket << '\n'
					  << '\n';
			if (FD_ISSET(socket, &ready_sockets))
			{
				std::cout << "Select socket: " << socket << '\n';
				if (FD_ISSET(socket, &listen_sockets))
					this->acceptNewConnection(socket);
				else
				{
					/* Check time each socket */
					std::map<int, Client *>::iterator it = client_map.begin();
					for (; it != client_map.end(); it++)
					{
						if (time(NULL) - it->second->getLastTime() > 5)
							closeSocket(it->first);
					}
					std::cout << "Sending to socket: " << socket << "\n";
					checkRequest(socket);
					client_map[socket]->buildResponse();
					closeSocket(socket);
					status--;
					//client_map[socket]->updateTime();
					/* Display socket value */
					std::cout << "status: " << status << ", message sent\n";
					std::cout << "Max_Socket: " << max_socket << '\n'
							  << '\n';
					std::cout << YELLOW << "Webserver waiting for client....\n"
							  << DEFAULT;
				}
			}
		}
	}
}

void Server::acceptNewConnection(int listen_sockets)
{
	int addrlen = sizeof(address);
	new_socket = accept(listen_sockets, (struct sockaddr *)&address, (socklen_t *)&addrlen);
	if (new_socket < 0)
	{
		perror("Accept ERROR");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	if (fcntl(new_socket, F_SETFL, O_NONBLOCK) < 0)
	{
		perror("NONBLOCK ERROR");
		close(server_fd);
		close(new_socket);
		exit(EXIT_FAILURE);
	}
	client_map[new_socket] = new Client(new_socket, &server_config[new_socket]);
	FD_SET(new_socket, &current_sockets); // Accept New Connection from client
	if (new_socket > max_socket)
		max_socket = new_socket;
	std::cout << GREEN << "accept socket[" << new_socket << "]\n"
			  << DEFAULT;
	std::cout << "new_socket = " << new_socket << '\n';
	std::cout << "max_socket = " << max_socket << '\n';
	std::cout << "Recieve Request...\n";
}

bool Server::checkRequest(int socket)
{
	char buffer[10000];
	int size;

	while (true)
	{
		size = recv(socket, buffer, sizeof(buffer), 0);
		std::cout << "size: " << size << '\n';
		if (size < 0)
			break;
		else if (!size)
			return (false); // no message -- close connection
		buffer[size] = '\0';
		// write to request string stream
		std::cout << buffer << '\n';
		client_map[new_socket]->getRequest()->writeStream(buffer, size);
	}
	return (true);
}

void Server::closeSocket(int socket)
{
	int max = 0;
	if (FD_ISSET(socket, &current_sockets))
	{
		FD_CLR(socket, &current_sockets);
		if (socket == max_socket)
		{
			for (int i = 0; i < max_socket; i++)
			{
				if (FD_ISSET(i, &current_sockets))
					max = i;
			}
			max_socket = max;
		}
		close(socket);
		delete client_map[socket];
		client_map.erase(socket);
	}
}

void Server::shutdownServer()
{

}
