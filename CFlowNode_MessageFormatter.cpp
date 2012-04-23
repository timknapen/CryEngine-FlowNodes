#include "StdAfx.h"
#include "G2FlowBaseNode.h"
//#include <WinSock2.h>
//#include <process.h>
//#include <stdio.h>
#include <sstream> //for string stuff



class CFlowNode_MessageFormatter
	: public CFlowBaseNode<eNCT_Instanced>
{


	// -------------------------------------------------------------------------------------------------------------------------------
	// VARIABLES
	// -------------------------------------------------------------------------------------------------------------------------------
	int counter;
	string prefix;
	string output;

	// -------------------------------------------------------------------------------------------------------------------------------
	// DEFINE PORTS
	// -------------------------------------------------------------------------------------------------------------------------------
	enum EInputPorts
	{ 
		EIP_Prefix,
		EIP_Vec
	};

	enum EOutputs
	{
		EOP_Value,
		EOP_Debug
	};


public:
	CFlowNode_MessageFormatter( SActivationInfo* pActInfo )
	{
		
		counter = 0;
		output = "";
		prefix = "";

	}
	

	

	//--------------------------------------------------------
	~CFlowNode_MessageFormatter(){
		
	}
	virtual IFlowNodePtr Clone( SActivationInfo *pActInfo ){
		return new CFlowNode_MessageFormatter(pActInfo);
	}
	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		/*
		EIP_Prefix,
		EIP_Vec
		*/
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<string>("Prefix" ,"",_HELP("Prefix for vector")),
			InputPortConfig< Vec3 >( "Vector", Vec3( ZERO ), _HELP( "vector to send" ) ),
			{0}
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Value", _HELP("Value")),
			OutputPortConfig<string>( "Debug", _HELP("This spits out debug messages.")),
			{0}
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Vector Formatter");
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
				prefix = GetPortString(pActInfo, EIP_Prefix);
			}
			break;
		
		case eFE_Activate:
			{
				if (IsPortActive(pActInfo, EIP_Prefix)){
					prefix = GetPortString(pActInfo, EIP_Prefix);
				}
				
				
				if (IsPortActive(pActInfo, EIP_Vec) ) {
					
					const Vec3 vec = GetPortVec3( pActInfo, EIP_Vec );
					// convert to char *
					char vecBuf[512];
					sprintf(vecBuf,"%s, %.2f, %.2f, %.2f\n", prefix, vec.x, vec.y, vec.z);
					if(((string)vecBuf).compare(output) != 0){ 
						// output value, only if different than last output
						ActivateOutput(pActInfo, EOP_Value , (string)vecBuf);
						output = vecBuf;
					}
					
				}
				
			}
			break;
		}
	}

	virtual void GetMemoryUsage( ICrySizer* s ) const
	{
		s->Add( *this );
	}



private:
};

REGISTER_FLOW_NODE( "AAAWIMMM:MessageFormatter", CFlowNode_MessageFormatter );