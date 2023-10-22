#pragma once
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#define SUCCESS 0


#pragma comment (lib, "ws2_32.lib")

class Client_Socket
{
public:
	Client_Socket(int RequestVersion);
	~Client_Socket();
	void ConnectServer(const char* server_ip, const int server_port);
	void initiate_chat_room();

private:
	
	WSADATA wsaData;
	SOCKET ClientSocket;
	sockaddr_in ClientSocketAddress;

	
	void SetClientSockAddr(const char* server_ip, const int server_port);
	void message_send();
	void message_read();
};