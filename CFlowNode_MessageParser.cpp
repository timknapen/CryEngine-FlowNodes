#include "StdAfx.h"
#include "G2FlowBaseNode.h"
#include <stdio.h>
#include <sstream> //for string stuff

// CFlowNode_MessageParser
class CFlowNode_MessageParser
	: public CFlowBaseNode<eNCT_Singleton>
{


	// -------------------------------------------------------------------------------------------------------------------------------
	// VARIABLES
	// -------------------------------------------------------------------------------------------------------------------------------
	

	int STRLEN;
	char recMessage[1024];

	std::string problem;
	int counter;


	// -------------------------------------------------------------------------------------------------------------------------------
	// DEFINE PORTS
	// -------------------------------------------------------------------------------------------------------------------------------
	enum EInputPorts
	{
		EIP_InString
	};

	enum EOutputs
	{
		EOP_Position,
		EOP_Debug
	};


public:
	CFlowNode_MessageParser( SActivationInfo* pActInfo )
	{
		// constructor
		problem = "nothing done";
		counter = 0;
	}

	//--------------------------------------------------------
	~CFlowNode_MessageParser(){
		STRLEN = 1024;
	}

	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<string>("InString", _HELP("input string")),
			{0}
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<Vec3>("position", _HELP("output position as Vec3")),
			OutputPortConfig<string>( "Debug", _HELP("This spits out debug messages.")),
			{0}
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("parses messages formated like: x[space]y[space]z[newline] x,y,z are float");
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
				// m_actInfo = *pActInfo; // why?
			}
			break;

		case eFE_Activate:
			{
				if (IsPortActive(pActInfo, EIP_InString)) {
					counter ++;
					string inString = GetPortString(pActInfo, EIP_InString);
					
					char debug[128];
					sprintf(debug, " DEBUG %i :: %s", counter, inString);
					ActivateOutput(pActInfo, EOP_Debug , (string)debug);

					parseMessage(inString,pActInfo);
					
				}
			}
			break;

		case eFE_Update:
			{
				// should not be called
				ActivateOutput(pActInfo, EOP_Debug , (string)"update message parser????");
			}
			break;
		}
	}

	virtual void GetMemoryUsage( ICrySizer* s ) const
	{
		s->Add( *this );
	}


	//----------------- Receive a message -----------------

	

	////////////////////////////////////////////////////
	void parseMessage(string inString, SActivationInfo* pActInfo)
	{
		int ilen = inString.length(); 
		const char* istr = inString.c_str();
		const int buflen = 1024;
		char mssgBuf[buflen];
		int bufpos = 0;
		int coordcount = 0;
		float vec[3];
		Vec3 posVec; 
		for(int i = 0; i<ilen; i++){
			switch(istr[i]){
			case ' ':		// end of a float
				if(coordcount < 3){
					vec[coordcount] = (float)atof(mssgBuf);
				}
				bufpos = 0;
				coordcount++;
				break;
			case '\n':		//end of a vector
				if(coordcount < 3){
					vec[coordcount] = (float)atof(mssgBuf);
				}
				bufpos = 0;
				coordcount++;
				if(coordcount == 3){ // I can make a vector now!
					posVec = Vec3(vec[0],vec[1],vec[2]);
					ActivateOutput(pActInfo, EOP_Position, posVec ); // send it to the output
				}
				coordcount = 0;

				break;
			default:		// add character to message buffer
				if(bufpos < buflen-1){
					mssgBuf[bufpos] = istr[i];
					bufpos++;
					mssgBuf[bufpos] = '\0';
				}
				break;
			}
		}
	}

};

REGISTER_FLOW_NODE( "AAAWIMMM:MessageParser", CFlowNode_MessageParser );