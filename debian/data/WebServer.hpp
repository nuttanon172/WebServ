#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#define TIME_OUT 6

#define DEFAULT "\033[0m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define RED     "\033[31m"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <ctime>
<<<<<<< HEAD
#include <sys/wait.h>
=======
>>>>>>> 4cc5161f3f083c8e5f0dcfff4a93f1b17f661f07
#include "Server.hpp"
#include "Response.hpp"
#include "Request.hpp"
#include "Client.hpp"
#include "util.hpp"
#include "CGI.hpp"

std::string ft_itos(int nbr);

#endif
