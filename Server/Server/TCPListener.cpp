#include "TCPListener.h"

cTCPListener::cTCPListener(std::string ipAdress, u_int iPortNumber, MsgRecvHandler msgHandler)
	//: m_Port(iPortNumber), m_MsgHandler
{
	this->Init();
}

static cTCPListener::Init()
{

}

bool cTCPListener::SendMsg(cTCPClient client, std::string msg)
{
	send(client.m_Socket, msg.data(), msg.length() + 1, 0);
	
}

cTCPListener::~cTCPListener()
{
	this->CloseObject(); 
}