

#include "main_second_server.h"
#include <buffer.h>


Connection::Connection(SOCKET ClientSocket, sockaddr_in ClientSocketAddress)
{
	this->ClientSocket = ClientSocket;
	this->ClientSocketAddress = ClientSocketAddress;
	this->client_name = std::to_string(ClientSocket);
}

// starting setup
Server_Socket::Server_Socket(int RequestVersion)
{
	ServerSocket = INVALID_SOCKET;
	int ret = WSAStartup(MAKEWORD(RequestVersion, 0), &wsaData);
	if (ret == SUCCESS)		// Check required version
	{
		if (LOBYTE(wsaData.wVersion < RequestVersion))
		{
			throw std::runtime_error("Requested version is not available.");
		}
	}
	else
		throw std::runtime_error("Startup failed! WSAStartup failed. Error Code: " + std::to_string(ret));
}

// Cleanups 
Server_Socket::~Server_Socket()
{
	WSACleanup();

	// Closing all client Sockets, if any remains
	for (auto i : connections)
	{
		if (i.first != INVALID_SOCKET)
			closesocket(i.first);
	}

	if (ServerSocket != INVALID_SOCKET)
		closesocket(ServerSocket);
}
// listening IP
void Server_Socket::SetServerSockAddr(sockaddr_in* ServerSocketAddress, int PortNumber)
{
	ServerSocketAddress->sin_family = AF_INET;
	ServerSocketAddress->sin_port = htons(PortNumber);
	ServerSocketAddress->sin_addr.S_un.S_addr = INADDR_ANY;			// Listen on host IP
}

//Starting server socket, bind, listen
void Server_Socket::start_server(int PortNumber)
{
	// Create server socket
	if ((ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
		throw std::runtime_error("Could not create socket.");
	SetServerSockAddr(&ServerSocketAddress, PortNumber);

	// Bind the created socket with Socket Address
	if (bind(ServerSocket, (sockaddr*)(&ServerSocketAddress), sizeof(ServerSocketAddress)) != SUCCESS)
		throw std::runtime_error("Could not bind socket.");

	// Put the Server Socket on listening. It will accept SOMAXCON
	if (listen(ServerSocket, SOMAXCONN) != SUCCESS)			//SOMAXCONN is maximum no. of connections
		throw std::runtime_error("Could not put the socket in listening mode.");

	std::cout << "Server is started." << std::endl;
}

//initiating chat room

void Server_Socket::initiate_chat_room()
{
	std::thread init_thread([&]()
		{	
			while (true)
			{
				SOCKET ClientSocket;
				sockaddr_in ClientAddr;
				int SizeAddr = sizeof(ClientAddr);
				ClientSocket = accept(ServerSocket, (sockaddr*)(&ClientAddr), &SizeAddr);			// Accepting the new client(person) in chat room
				Connection c(ClientSocket, ClientAddr);
				this->add_client_to_room(c);
			}
		}
	);

	// detaching the thread to run independently.
	init_thread.detach();
	std::cout << "Chat room has been created." << std::endl;
}

// adding client to the room
void Server_Socket::add_client_to_room(Connection& c)
{
	std::thread t(&Server_Socket::read_message, this, c);
	t.detach();
	connections.insert(std::pair<SOCKET, Connection>(c.ClientSocket, c));
}

// reading message which comes from client 
void Server_Socket::read_message(Connection c)
{

	const int bufsize = 512;
	Buffer readbuffer(bufsize);

	bool disconnect = true;
	SOCKET ClientSocket = c.ClientSocket;

	std::string broadcast_msg;
	uint32_t usernameLength;
	
	// Getting Name of Client, that will be send by client Application.
	int message_size = recv(ClientSocket, reinterpret_cast<char*>(&readbuffer.m_Buffer[0]), bufsize, 0);
	if (message_size > 0)
	{

		usernameLength = readbuffer.DeserializeUInt32BE();            //Deserializing unsigned int (32 bit)
		broadcast_msg = readbuffer.DeserializeString(usernameLength); //Deserializing string
		c.client_name = broadcast_msg;
		disconnect = false;
		std::cout << "SERVER MSG: " << c.client_name << " Has Appeared" << std::endl;

	}
	else
	{
		std::cout << "SERVER MSG: " << c.client_name << " is unable to Start Communication" << std::endl;
	}
	// Starting communication loop	
	while (!disconnect)
	{
		const int msgbufsize = 512;
		Buffer msgbuffer(msgbufsize);
		
			message_size = recv(ClientSocket, reinterpret_cast<char*>(&msgbuffer.m_Buffer[0]), msgbufsize, 0);
			if (message_size == 0)
			{
				disconnect = true;
				std::cout << "SERVER MSG: " << c.client_name << ": closed the connection." << std::endl;
			}
			else if (message_size == SOCKET_ERROR)
			{
				disconnect = true;
				std::cout << "SERVER MSG: " << c.client_name << ": seems to be offline." << std::endl;
			}
			else
			{
				//recieving messages from clients to the server

				uint32_t messageLenght = msgbuffer.DeserializeUInt32BE();         //Deserializing unsigned int (32 bit)
				std::string message = msgbuffer.DeserializeString(messageLenght); //Deserializing string
				uint16_t messageType = msgbuffer.DeserializeUInt16BE();			  //Deserializing unsigned short (16 bit)
																				 
				if (messageType == 1)
				{
					std::cout << c.client_name << ": " << message << std::endl;
					std::cout << "Message Type: " << messageType << std::endl;
				}
				
					
				const int clientbufsize = 512;
				Buffer clientBuffer(clientbufsize);

				clientBuffer.SerializeUInt32BE(usernameLength);          //serializing unsigned int (32 bit)
				clientBuffer.SerializeString(broadcast_msg);			 //serializing string
				clientBuffer.SerializeUInt32BE(messageLenght);           //serializing unsigned int (32 bit)
				clientBuffer.SerializeString(message);					 //serializing string

				

				// sending message to other people in the chat room
				for (auto i : connections)
				{
					if (i.first != ClientSocket)
					{
						if (send(i.first, reinterpret_cast<char*>(&clientBuffer.m_Buffer[0]), clientbufsize,0) == SOCKET_ERROR)
							std::cout << "SERVER MSG: " << "Forwarding message to socket: [" << i.first << " " << i.second.client_name << "] failed" << std::endl;
					}
				}
			}
		

		
	}
	// removing the socket of disconnected person from chat room
	connections.erase(ClientSocket);
	// closing the connection with client socket from my side(server)
	closesocket(ClientSocket);
	std::cout << "SERVER MSG: " << c.client_name << ": removed from chat room." << std::endl;
}




