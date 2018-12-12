#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#include <fstream>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"


/*
General model for creating a streaming TCP/IP Server and Client.
- Server
1 Initialize Winsock.
2 Create a socket.
3 Bind the socket.
4 Listen on the socket for a client.
5 Accept a connection from a client.
6 Receive and send data.
7 Disconnect.
- Client
1 Initialize Winsock.
2 Create a socket.
3 Connect to the server.
4 Send and receive data.
5 Disconnect.
*/

int SendToClient(const SOCKET& clientSocket, const std::string Data)
{
	int iSendResult = send(clientSocket, Data.data(), Data.length(), 0);
	if (iSendResult == SOCKET_ERROR) {
		std::cout << "Send failed with error: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

/*Function for recieving data from a client app, uses socket and a reference to string data buffer, the buffer then becomes what you work on*/
int RecieveFromClient(SOCKET clientSocket, std::string& Data)
{
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;
	
	int iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
	if (iResult > 0) {
		std::cout << std::string(recvbuf, iResult).data() << std::endl;
	}
	else if (iResult == 0)
		std::cout << "Closing connection..." << std::endl;
	else {
		std::cout << "Recieve failed with error: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}
	return 0;
}

int __cdecl main() {
	std::ofstream oFile("server_debug.txt");
	// oFile.open();

	WSADATA wsaData;				// Create WSADATA struct for winsock data

	// 1 Initialize Winsock.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);  // first parameter is version, returned from a function as WORD
	if (iResult != 0) {
		std::cout << "WinSock initialization failed..." << std::cin.get() << std::endl;
		return 1; }

	// 2 Create a socket.
	struct addrinfo *result = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::cout << "Socket creationg failed..." << std::cin.get() << std::endl;
		WSACleanup();
		return 1; }

	SOCKET scktListen = INVALID_SOCKET;
	scktListen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	// Socket acquisition
	if (scktListen == INVALID_SOCKET) {
		std::cout << "Socket acquisition failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		WSACleanup();
		return 1; }

	freeaddrinfo(result); // no longer needed, do it faster to prevent memory leak

	// 3 Bind the socket.
	// Setup the TCP listening socket
	iResult = bind(scktListen, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Socket acquisition failed with error: " << WSAGetLastError() << std::endl;
		freeaddrinfo(result);
		closesocket(scktListen);
		WSACleanup();
		return 1;
	}

	if (listen(scktListen, SOMAXCONN) == SOCKET_ERROR) {
		std::cout << "Socket acquisition failed with error: " << WSAGetLastError() << std::endl;
		closesocket(scktListen);
		WSACleanup();
		return 1;
	}
	std::cout << "Waiting for connection..." << std::endl;
	// 4 Accept connection on socket.
	// Create client socket to connect to
	SOCKET clientSocket = INVALID_SOCKET;

	//oFile.copyfmt(std::cout);
	oFile.basic_ios<char>::rdbuf(std::cout.rdbuf());

	std::string strFileBuffer = "";

	std::cout.write(&strFileBuffer[0], sizeof(std::cout.rdbuf()));

	
	oFile << strFileBuffer.data();
	//oFile << "Passed first point" << std::endl;

	SOCKADDR_IN client_info = { 0 };
	int client_info_size = sizeof(client_info);

	//while (clientSocket == INVALID_SOCKET) {
		// Accept a client socket - accept single socket with no multithreaded support for now
		clientSocket = accept(scktListen, (struct sockaddr*)&client_info, &client_info_size);

		if (clientSocket == INVALID_SOCKET) {
			std::cout << "Socket acquisition failed with error: " << WSAGetLastError() << std::endl;
			closesocket(scktListen);
			WSACleanup();
			return 1; }
	//}

	closesocket(scktListen); // releasing the server socket, it is no longer necessary as we do not accept more connections from now on in this server test

	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	struct sockaddr_in *ipv4 = (struct sockaddr_in *)result->ai_addr;

	std::string ipStr = "";
	inet_ntop(AF_INET, (void*)&(ipv4->sin_addr), (PSTR)&(ipStr[0]), INET_ADDRSTRLEN);

	SendToClient(clientSocket, "Please input your desired name: ");

	std::string clientID = "";
	// Receive until the peer shuts down the connection
	do {

		iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			std::cout << ipStr.data() << "->" << clientID.data() << ": "<< std::string(recvbuf, iResult).data() << std::endl;
		}
		else if (iResult == 0)
			std::cout << "Closing connection..." << std::endl;
		else {
			std::cout << "Recieve failed with error: " << WSAGetLastError() << std::endl;
			closesocket(clientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the send half of the connection since no more data will be sent
	iResult = shutdown(clientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "Shutdown failed with error: " << WSAGetLastError() << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 1; }

	closesocket(clientSocket);
	WSACleanup();

	std::cin.get();
	return 0;
}