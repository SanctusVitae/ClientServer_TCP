#include "TCPListener.h"

cTCPListener::cTCPListener(std::string ipAdress, u_int iPortNumber, MsgRecvHandler msgHandler)
	: m_Port(iPortNumber), m_MsgHandler
{

}

cTCPListener::~cTCPListener()
{
	this->CloseObject(); 
}