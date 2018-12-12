#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <iostream>
#include <string>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main()
{
	WSADATA wsaData;				// Create WSADATA struct for winsock data

	// 1 Initialize Winsock.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);  // first parameter is version, returned from a function as WORD
	if (iResult != 0) {
		std::cout << "WinSock initialization failed..." << std::cin.get() << std::endl;
		return 1;
	}

	// 2 Create a socket.
	/*int i = NULL; // OK
	int i = nullptr; // error - not a integer convertible value
	int* p = NULL; //ok - int converted into pointer
	int* p = nullptr; // ok
	not sure if I can swap NULL with nullptr, legacy code? I know that nullptr is treated like a pointer in templates and such and NULL is simply an integer... but does it matter here? Check it out*/
	struct addrinfo *result = nullptr, *ptr = nullptr;
	struct addrinfo hints;

	std::string SendBuff = "";
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	SOCKET ConnectSocket = INVALID_SOCKET;

	std::cout << "Input IP adress to connect to: ";
	std::string ipAdressStr = "";
	std::getline(std::cin, ipAdressStr);

	// Resolve the server address and port
	iResult = getaddrinfo(&*ipAdressStr.begin(), DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		std::cout << "getaddrinfo failed: " << iResult << std::cin.get() << std::endl;
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			std::cout << "socket failed with error:: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "Unable to estabilish server connection" << std::endl;
		WSACleanup();
		return 1;
	}
	else { std::cout << "Connection to: " << ipAdressStr << " established." << std::endl; }

	while (SendBuff != "exit")
	{
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
			if (iResult > 0)
				std::cout << std::string(recvbuf, iResult).data();
			else if (iResult == 0)
				std::cout << "Connection closed" << std::endl;
			else
				std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;


			std::cout << "You: ";
			std::getline(std::cin, SendBuff);

			// Send an initial buffer
			iResult = send(ConnectSocket, SendBuff.data(), SendBuff.length(), 0);
			if (iResult == SOCKET_ERROR) {
				std::cout << "send failed with error:: " << WSAGetLastError() << std::endl;
				closesocket(ConnectSocket);
				WSACleanup();
				return 1;
			}
	}
	// shutdown the connection
	iResult = shutdown(ConnectSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		std::cout << "shutdown failed with error:: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ConnectSocket);
	WSACleanup();

	std::cin.get();
	return 0;
}