#include "main_second_server.h"
#include <iostream>

#define REQ_WINSOCK_VER 2
#define LISTENING_PORT 8412
int main()
{
	
	Server_Socket MyServer(REQ_WINSOCK_VER);
	MyServer.start_server(LISTENING_PORT);
	MyServer.initiate_chat_room();
	while (true)
	{
		Sleep(500);
	}

		
	
	return 0;
}
