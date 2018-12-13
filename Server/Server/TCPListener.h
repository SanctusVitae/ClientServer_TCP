#pragma once

#include <iostream>
#include <string>

// Winsock library header file
#include <WS2tcpip.h>
// Code for winsock, static library
#pragma comment (lib, "Ws2_32.lib")

class cTCPListener;
// Message handler callback function
typedef void*(MsgRecvHandler)(cTCPListener* listenerObject, u_int socketID, std::string msgData);

class cTCPClient
{
public:
		SOCKET		m_Socket;
		std::string m_ClientIP;
		std::string m_ClientID;

		cTCPClient(SOCKET clientSocket, std::string clientIP, std::string clientID)
			: m_Socket(clientSocket), m_ClientIP(clientIP), m_ClientID(clientID) {}
		cTCPClient(SOCKET clientSocket, )
};

class cTCPListener {
private:
	std::string		m_MsgData;
	u_int			m_Port;
	MsgRecvHandler	m_MsgHandler;

public:

	cTCPListener(std::string ipAdress, u_int iPortNumber, MsgRecvHandler msgHandler);
	~cTCPListener();

public:
	static bool Init();
	bool		SendMsg(SOCKET clientSocket,	std::string msg);		// Sends a message to specified client
	bool		SendMsg(cTCPClient client,		std::string msg);
	bool		RecvMsg(SOCKET clientSocket,	std::string msg);

	// initialize winsock
	// open socket
	// create listener
	// data loop
	// send data
	// close winsock


};

//cTCPListener::SendMsg