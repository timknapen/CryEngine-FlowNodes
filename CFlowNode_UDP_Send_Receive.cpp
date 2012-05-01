#include "StdAfx.h"
#include "G2FlowBaseNode.h"
#include <WinSock2.h>
#include <process.h>
#include <stdio.h>
#include <sstream> //for string stuff



class CFlowNode_UDP_Send_Receive
	: public CFlowBaseNode<eNCT_Singleton>
{


	// -------------------------------------------------------------------------------------------------------------------------------
	// VARIABLES
	// -------------------------------------------------------------------------------------------------------------------------------
	// UDP socket stuff
	//init
	int server_length;
	int port;
	int STRLEN;
	char recMessage[1024];
	WSADATA wsaData;
	SOCKET mySocket;
	sockaddr_in myAddress;

	bool socketWorking;
	std::string problem;

	int counter;
	string output;

	bool m_bEnabled;
	//SActivationInfo m_actInfo;  // is this needed?  We already just use pActInfo
	CTimeValue m_lastSend;
	CTimeValue m_lastTime;



	// -------------------------------------------------------------------------------------------------------------------------------
	// DEFINE PORTS
	// -------------------------------------------------------------------------------------------------------------------------------
	enum EInputPorts
	{
		EIP_Enable,
		EIP_Disable,
		EIP_Port, 
		EIP_Message
	};

	enum EOutputs
	{
		EOP_Success = 0,
		EOP_Fail,
		EOP_Received,
		EOP_Value,
		EOP_Debug
	};


public:
	CFlowNode_UDP_Send_Receive( SActivationInfo* pActInfo )
	{
		// constructor
		socketWorking = false;
		m_bEnabled = false;
		problem = "nothing done";
		output ="";
		counter = 0;
		CTimeValue currTime(0.0f);
		m_lastTime = currTime;
		m_lastSend = currTime;
	}
	////////////////////////////////////////////////////
	void endSocket() {
		socketWorking = false;
		closesocket(mySocket);
		WSACleanup();
	}

	//--------------------------------------------------------
	void startSocket(int port,  SActivationInfo* pActInfo ) {
		// init
		STRLEN = 1024;
		socketWorking = false;

		//create socket
		// a bit messy, from : http://www.cplusplus.com/forum/windows/24301/
		// and non-blocking from: http://www.adp-gmbh.ch/win/misc/sockets.html Socket.cpp

		if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
		{
			problem = "Socket Initialization: Error with WSAStartup\n";
			WSACleanup();

		} else {


			mySocket = socket(AF_INET, SOCK_DGRAM, 0);
			if (mySocket == INVALID_SOCKET)
			{
				problem = "Socket Initialization: Error creating socket";
				WSACleanup();
			} else {

				//bind
				myAddress.sin_family = AF_INET;
				myAddress.sin_addr.s_addr = inet_addr( "0.0.0.0" );
				myAddress.sin_port = htons(port);

				// set up as non-blocking
				u_long arg = 1;
				ioctlsocket(mySocket, FIONBIO, &arg);

				if(bind(mySocket, (SOCKADDR*) &myAddress, sizeof(myAddress)) == SOCKET_ERROR)
				{
					problem = "ServerSocket: Failed to connect\n";
					WSACleanup();
				} else {
					// all went well, send Success signal, and set details
					socketWorking = true;
					problem = "no problem";
					ActivateOutput(pActInfo, EOP_Success, true);
					return;
				}

			}
		}

		// failed, send Failed signal
		ActivateOutput(pActInfo, EOP_Fail, true); 
		return;
	}

	//--------------------------------------------------------
	~CFlowNode_UDP_Send_Receive(){
		// destructor
		if (socketWorking) {
			endSocket();
		}
	}

	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		/*

		EIP_PrefixA,
		EIP_VecA,
		EIP_PrefixB,
		EIP_VecB
		*/
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig_Void("Enable", _HELP("Enable receiving signals")),
			InputPortConfig_Void("Disable", _HELP("Disable receiving signals")),
			InputPortConfig<int>("Port", 1234, _HELP("Port number"), 0,0),
			InputPortConfig<string>("Message" ,"",_HELP("Message to send over UDP")),
			{0}
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<bool>("Success", _HELP("UDP socket successfully opened for listening")), 
			OutputPortConfig<bool>("Fail", _HELP("UDP socket failed to open")), 
			OutputPortConfig<bool>("Received", _HELP("New data")), 
			OutputPortConfig_Void("Value", _HELP("Value")),
			OutputPortConfig<string>( "Debug", _HELP("This spits out debug messages.")),
			{0}
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("test UDP listener");
		//config.SetCategory(EFLN_ADVANCED);
	}


	// -------------------------------------------------------------------------------------------------------------------------------
	// CALLED ON EVENT TRIGGER, ACTIVATE OR UPDATE
	// -------------------------------------------------------------------------------------------------------------------------------

	////////////////////////////////////////////////////
	virtual void ProcessEvent(EFlowEvent event, SActivationInfo *pActInfo)
	{
		switch (event)
		{
		case eFE_Initialize:
			{
				// m_actInfo = *pActInfo; // why??

				output = GetPortString(pActInfo, EIP_Message);

			}
			break;

		case eFE_Activate:
			{
				if (IsPortActive(pActInfo, EIP_Message) && socketWorking){
					output = GetPortString(pActInfo, EIP_Message);

					// check for time..
					CTimeValue currTime(gEnv->pTimer->GetCurrTime());
					float delay = 0.1;  // processing delay
					float time = (currTime-m_lastSend).GetSeconds();


					// Execute?
					if (time > delay)
					{
						m_lastSend = currTime;

						const char* c_str = output.c_str();
						int iResult = sendto(mySocket, c_str, (int)strlen(c_str), 0, (sockaddr*) &myAddress, sizeof(myAddress)); 
						if( iResult == SOCKET_ERROR){
							char buf[512];
							sprintf(buf, "send failed with error: %d\n", WSAGetLastError());
							ActivateOutput(pActInfo, EOP_Debug , (string)buf);
						}
					}else{
						char buf[512];
						sprintf(buf, "not sending, time is %f", time);
						ActivateOutput(pActInfo, EOP_Debug , (string)buf);
					}
				}
				if (IsPortActive(pActInfo, EIP_Enable)) {

					if (socketWorking) {
						endSocket();
					}

					m_bEnabled = true;
					// try to open port socket
					port = GetPortInt(pActInfo, EIP_Port);
					startSocket(port, pActInfo);

					char debug[128];
					sprintf(debug, " DEBUG %i :: %s", counter, " starting at port ", port);
					ActivateOutput(pActInfo, EOP_Debug , (string)debug);

					Execute(pActInfo);
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, true);
				}
				if (IsPortActive(pActInfo, EIP_Disable)) {
					m_bEnabled = false;
					endSocket();
					pActInfo->pGraph->SetRegularlyUpdated(pActInfo->myID, false);
				}
			}
			break;

		case eFE_Update:
			{
				CTimeValue currTime(gEnv->pTimer->GetCurrTime());
				float delay = 0;  // processing delay
				delay -= (currTime-m_lastTime).GetSeconds();
				m_lastTime = currTime;

				// Execute?
				if (delay <= 0.0f)
				{
					Execute(pActInfo);
				}
			}
			break;
		}
	}

	virtual void GetMemoryUsage( ICrySizer* s ) const
	{
		s->Add( *this );
	}


	//----------------- Receive a message -----------------

	int ReceiveLine( SActivationInfo *pActInfo) {
		counter ++;
		int size = -1;
		if (socketWorking) {
			server_length = sizeof(struct sockaddr_in);
			size = recvfrom(mySocket, recMessage, STRLEN, 0, (SOCKADDR*) &myAddress, &server_length);
			/*
			char * mssg;
			mssg = "OK"; 
			if (size == SOCKET_ERROR) {
			// get last error
			switch(WSAGetLastError()) {
			case WSANOTINITIALISED:
			mssg = "WSANOTINITIALISED";
			case WSAENETDOWN:
			mssg = "WSAENETDOWN";
			case WSAEFAULT:
			mssg = "WSAEFAULT";
			case WSAENOTCONN:
			mssg = "WSAENOTCONN";
			case WSAEINTR:
			mssg = "WSAEINTR";
			case WSAEINPROGRESS:
			mssg = "WSAEINPROGRESS";
			case WSAENETRESET:
			mssg = "WSAENETRESET";
			case WSAENOTSOCK:
			mssg = "WSAENOTSOCK";
			case WSAEOPNOTSUPP:
			mssg = "WSAEOPNOTSUPP";
			case WSAESHUTDOWN:
			mssg = "WSAESHUTDOWN";
			case WSAEWOULDBLOCK:
			mssg = "WSAEWOULDBLOCK";
			case WSAEMSGSIZE:
			mssg = "WSAEMSGSIZE";
			case WSAEINVAL:
			mssg = "WSAEINVAL";
			case WSAECONNABORTED:
			mssg = "WSAECONNABORTED";
			case WSAETIMEDOUT:
			mssg = "WSAETIMEDOUT";
			case WSAECONNRESET:
			mssg = "WSAECONNRESET";
			default:
			mssg = "NO ERROR (no info?";
			}
			}
			char debug[128];
			sprintf(debug, " DEBUG %i :: %s", counter, mssg);
			ActivateOutput(pActInfo, EOP_Debug , (string)debug);
			//*/
			if (size != SOCKET_ERROR) {
				recMessage[size] = '\0';
			} 
		}
		return size;
	}

	////////////////////////////////////////////////////
	virtual void Execute(SActivationInfo *pActInfo)
	{
		bool bResult = false;
		// did the socket connect okay?
		if (socketWorking) {
			if (ReceiveLine(pActInfo) != -1) {
				std::string r = recMessage;
				string value = r.c_str();
				ActivateOutput(pActInfo, EOP_Value, value);
				ActivateOutput(pActInfo, EOP_Debug , value);
				bResult = true;
			}
		}

		// return false if socket error, or no message

		if (bResult) ActivateOutput(pActInfo, EOP_Received, true);
		return;
	}


private:

	float m_smoothedHitDistance;
	float m_hitDistanceChangeRate;
};

REGISTER_FLOW_NODE( "AAAWIMMM:UDPSendReceive", CFlowNode_UDP_Send_Receive );