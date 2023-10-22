#pragma once


#define WIN32_LEAN_AND_MEAN
#define SUCCESS 0 			
#define ONE_KB 1024
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <map>
#include <thread>

#pragma comment (lib, "ws2_32.lib")



class Connection
{
public:

	
	SOCKET ClientSocket;
	sockaddr_in ClientSocketAddress;
	std::string client_name;
	Connection(const SOCKET ClientSocket, const sockaddr_in ClientSocketAddress);
};

class Server_Socket
{
private:
	WSADATA wsaData;
	SOCKET ServerSocket;
	sockaddr_in ServerSocketAddress;
	std::map<SOCKET, Connection> connections;

	void SetServerSockAddr(sockaddr_in* sockAddr, int PortNumber);
	void read_message(Connection c);
	void add_client_to_room(Connection& c);

public:
	Server_Socket(int RequestVersion);
	~Server_Socket();
	void start_server(int PortNumber);
	void initiate_chat_room();
};