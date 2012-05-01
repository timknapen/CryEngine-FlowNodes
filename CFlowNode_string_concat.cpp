#include "StdAfx.h"
#include "G2FlowBaseNode.h"
//#include <stdio.h>
#include <sstream> //for string stuff



class CFlowNode_String_Concat
	: public CFlowBaseNode<eNCT_Instanced>
{


	// -------------------------------------------------------------------------------------------------------------------------------
	// VARIABLES
	// -------------------------------------------------------------------------------------------------------------------------------
	int counter;
	string string1, string2, string3;
	string output;

	// -------------------------------------------------------------------------------------------------------------------------------
	// DEFINE PORTS
	// -------------------------------------------------------------------------------------------------------------------------------
	enum EInputPorts
	{ 
		EIP_String_1,
		EIP_String_2,
		EIP_String_3
	};

	enum EOutputs
	{
		EOP_Output,
		EOP_Debug
	};


public:
	CFlowNode_String_Concat( SActivationInfo* pActInfo )
	{

		counter = 0;
		string1 = string2 = string3 = "";
		output="";

	}




	//--------------------------------------------------------
	~CFlowNode_String_Concat(){

	}
	virtual IFlowNodePtr Clone( SActivationInfo *pActInfo ){
		return new CFlowNode_String_Concat(pActInfo);
	}
	virtual void GetConfiguration( SFlowNodeConfig& config )
	{

		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<string>("String1" ,"",_HELP("string1")),
			InputPortConfig<string>("String2" ,"",_HELP("string2")),
			InputPortConfig<string>("String3" ,"",_HELP("string3")),
			{0}
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<string>("Output", _HELP("Out")),
			OutputPortConfig<string>( "Debug", _HELP("This spits out debug messages.")),
			{0}
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Appends strings.");
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
				string1 = GetPortString(pActInfo, EIP_String_1);
				string2 = GetPortString(pActInfo, EIP_String_2);
				string3 = GetPortString(pActInfo, EIP_String_3);
			}
			break;

		case eFE_Activate:
			{
				if (IsPortActive(pActInfo, EIP_String_1)){
					string1 = GetPortString(pActInfo, EIP_String_1);
				}
				if (IsPortActive(pActInfo, EIP_String_2)){
					string2 = GetPortString(pActInfo, EIP_String_2);
				}
				if (IsPortActive(pActInfo, EIP_String_3)){
					string3 = GetPortString(pActInfo, EIP_String_3);
				}

				output = string1 + string2 + string3;

				ActivateOutput(pActInfo, EOP_Output , (string)output);


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

REGISTER_FLOW_NODE( "AAAWIMMM:StringConcatenator", CFlowNode_String_Concat );