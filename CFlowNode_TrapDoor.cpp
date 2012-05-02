#include "StdAfx.h"
#include "G2FlowBaseNode.h"
//#include <WinSock2.h>
//#include <process.h>
//#include <stdio.h>
#include <sstream> //for string stuff



class CFlowNode_TrapDoor
	: public CFlowBaseNode<eNCT_Instanced>
{


	// -------------------------------------------------------------------------------------------------------------------------------
	// VARIABLES
	// -------------------------------------------------------------------------------------------------------------------------------
	int counter;
	Vec3 playerVec;
	Vec3 triggerVec;
	Vec3 offsetVec;

	// -------------------------------------------------------------------------------------------------------------------------------
	// DEFINE PORTS
	// -------------------------------------------------------------------------------------------------------------------------------
	enum EInputPorts
	{ 
		EIP_PlayerPos,
		EIP_TriggerPos,
		EIP_Offset
	};

	enum EOutputs
	{
		EOP_Offset,
		EOP_Debug
	};


public:
	CFlowNode_TrapDoor( SActivationInfo* pActInfo )
	{
		counter = 0;
		playerVec = triggerVec = offsetVec = Vec3(ZERO);
	}
	

	

	//--------------------------------------------------------
	~CFlowNode_TrapDoor(){
		
	}
	virtual IFlowNodePtr Clone( SActivationInfo *pActInfo ){
		return new CFlowNode_TrapDoor(pActInfo);
	}
	virtual void GetConfiguration( SFlowNodeConfig& config )
	{
		// Define input ports here, in same order as EInputPorts
		static const SInputPortConfig inputs[] =
		{
			InputPortConfig<Vec3>("PlayerEntity", Vec3( ZERO ), _HELP("The actor position")),
			InputPortConfig<Vec3>("TriggerEntity", Vec3( ZERO ), _HELP("The trigger position")),
			InputPortConfig< Vec3 >( "TargetOffset", Vec3( ZERO ), _HELP("Possible position to warp to" ) ),
			{0}
		};

		// Define output ports here, in same oreder as EOutputPorts
		static const SOutputPortConfig outputs[] =
		{
			OutputPortConfig<Vec3>("Offset", _HELP("Value")),
			OutputPortConfig<string>( "Debug", _HELP("This spits out debug messages.")),
			{0}
		};

		// Fill in configuration
		config.pInputPorts = inputs;
		config.pOutputPorts = outputs;
		config.sDescription = _HELP("Warping System");
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
				playerVec = GetPortVec3( pActInfo, EIP_PlayerPos );
				triggerVec = GetPortVec3( pActInfo, EIP_TriggerPos );
				offsetVec = GetPortVec3( pActInfo, EIP_Offset );
			}
			break;
		
		case eFE_Activate:
			{
				if (IsPortActive(pActInfo, EIP_PlayerPos)){
					// player moves!
					playerVec = GetPortVec3( pActInfo, EIP_PlayerPos );
					Vec3 dif = playerVec - triggerVec;
					// check distance

					if(dif.len() < 1 ){
							ActivateOutput(pActInfo, EOP_Offset ,offsetVec );
					}
				}
				if (IsPortActive(pActInfo, EIP_TriggerPos)){
					triggerVec = GetPortVec3( pActInfo, EIP_TriggerPos );
				}
				
				if (IsPortActive(pActInfo, EIP_Offset)){
					offsetVec = GetPortVec3( pActInfo, EIP_Offset );
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

REGISTER_FLOW_NODE( "AAAWIMMM:TrapDoor", CFlowNode_TrapDoor );