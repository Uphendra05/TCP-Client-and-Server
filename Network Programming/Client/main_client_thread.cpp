

#include "main_client_thread.h"
#include <buffer.h>


struct PacketHeader
{
	uint32_t packetSize;
	
};

struct ChatMessage
{
	PacketHeader header;
	uint32_t messageLength;
	uint32_t usernamLength;
	uint16_t messageType;
	
	std::string message;
};

Client_Socket::Client_Socket(int RequestVersion) 
{
	ClientSocket = INVALID_SOCKET;
	int ret = WSAStartup(MAKEWORD(RequestVersion, 0), &wsaData);
	if (ret == SUCCESS)
	{
		if (LOBYTE(wsaData.wVersion < RequestVersion))
		{
			throw std::runtime_error("Requested version is not available.");
		}
	}
	else
		throw std::runtime_error("Startup failed! WSAStartup failed. Error Code: " + std::to_string(ret));
}
Client_Socket::~Client_Socket()
{
	WSACleanup();

	// closing Client Socket
	if (ClientSocket != INVALID_SOCKET)
		closesocket(ClientSocket);
}
void Client_Socket::SetClientSockAddr(const char* server_ip, const int server_port)
{
	ClientSocketAddress.sin_addr.S_un.S_addr = inet_addr(server_ip);
	ClientSocketAddress.sin_family = AF_INET;						// Use TCP/IP protocol
	ClientSocketAddress.sin_port = htons(server_port);
}
void Client_Socket::ConnectServer(const char* server_ip, const int server_port)
{
	SetClientSockAddr(server_ip, server_port);

	// Create client socket
	if ((ClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		throw std::runtime_error("Could not create socket.");

	std::cout << "Attempting to connect to Server..." << std::endl;

	if (connect(ClientSocket, (sockaddr*)(&ClientSocketAddress), sizeof(ClientSocketAddress)) != SUCCESS)
		throw std::runtime_error("Could not connect");

	this->initiate_chat_room();
}

// initiating chat too to connect with the server
void Client_Socket::initiate_chat_room()
{
	ChatMessage structMessage;
	const int bufsize = 512;
	Buffer namebuffer(bufsize);

	std::string username;
	uint32_t usernameLenght = 0;

	std::cout << "you are added to chat room" << std::endl << std::endl;
	std::cout << "please enter your name: ";

	
	std::getline(std::cin, username);

	structMessage.usernamLength = usernameLenght;

	namebuffer.SerializeUInt32BE(structMessage.usernamLength);
	namebuffer.SerializeString(username);

	// sending your name to server.
	if (send(ClientSocket, (const char*)(&namebuffer.m_Buffer[0]), bufsize, 0) == SOCKET_ERROR)
	{
		std::cout << "Server probably down. Unable to initiate chat room. :(" << std::endl;
		return;
	}

	

	std::thread thread_for_message_read(&Client_Socket::message_read, this);
	thread_for_message_read.detach();
	Sleep(500); // sleep for 500ms
	std::thread thread_for_message_send(&Client_Socket::message_send, this);
	thread_for_message_send.detach();
}
// reading messages from server to see in client and whoever connected 

void Client_Socket::message_read()
{

		
	bool disconnect = false;
	int message_size = 0;
	while (!disconnect)
	{
		const int clientbufsize = 512;

		Buffer clientBuffer(clientbufsize);
		message_size = recv(ClientSocket, reinterpret_cast<char*>(&clientBuffer.m_Buffer[0]), clientbufsize, 0);
		if (message_size <= 0)
		{
			std::cout << "The connection is closed, reason: " << message_size << ", server down" << std::endl;
			disconnect = true;
		}

		uint32_t usernameLength = clientBuffer.DeserializeUInt32BE();           //Deserializing unsigned int (32 bit)
		std::string username = clientBuffer.DeserializeString(usernameLength);	//Deserializing string
		uint32_t msgLength = clientBuffer.DeserializeUInt32BE();				//Deserializing unsigned int (32 bit)
		std::string message = clientBuffer.DeserializeString(msgLength);		//Deserializing string

		std::cout << "\r\r\r\r" << username + ": " << message << std::endl;
		std::cout << "You: ";
	}
}

// sending messages from client to the server 

void Client_Socket::message_send()
{
	ChatMessage structMessage;
	                     
	std::string message;


	bool disconnect = false;
	while (!disconnect)
	{
		const int bufsize = 512;
		Buffer buffer(bufsize);

		std::cout << "You: ";
		std::getline(std::cin, message);
		structMessage.messageLength = structMessage.message.length();
		structMessage.message = message;
		structMessage.messageType = 1;

		buffer.SerializeUInt32BE(structMessage.messageLength); //serializing unsigned int (32 bit)
		buffer.SerializeString(structMessage.message);         //serializing string
		buffer.SerializeUInt16BE(structMessage.messageType);  //serializing unsigned short (16 bit)

		
		if (send(ClientSocket, (const char*)(&buffer.m_Buffer[0]), bufsize, 0) == SOCKET_ERROR)
		{
			std::cout << "Server probably down. " << std::endl;
			disconnect = true;
		}
	}


}






