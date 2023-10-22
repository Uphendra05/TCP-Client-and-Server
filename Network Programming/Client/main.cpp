#include "main_client_thread.h"
#include <iostream>

#define REQ_WINSOCK_VER	2
#define PORT 8412
#define IP "127.0.0.1"


int main(int argc, char* argv[])
{
	Client_Socket MyClient(REQ_WINSOCK_VER);
	MyClient.ConnectServer(IP, PORT);

	while (true)
	{
		Sleep(1000);
	}
	
	
	return 0;
}
