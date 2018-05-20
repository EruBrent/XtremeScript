#pragma once
#include "CustomizeStruct.h"

class CXvmProto
{
public:
	CXvmProto();
	~CXvmProto();

public:
	void	Test ( vector<string>& argv );
	void	XS_Init ();
	void	XS_ShutDown ();
	int		XS_LoadScript ( char * pstrFilename, int & iScriptIndex, int iThreadTimeslice );
	void	XS_UnloadScript ( int iThreadIndex );
	void	XS_ResetScript ( int iThreadIndex );
	void	XS_RunScripts ( int iTimesliceDur );

	// p526
	void	XS_StartScript ( int iThreadIndex );
	// p527
	void	XS_StopScript ( int iThreadIndex );
	void	XS_PauseScript ( int iThreadIndex, int iDur );
	// p528
	void	XS_UnpauseScript ( int iThreadIndex );
	// p530
	void XS_RegisterHostAPIFunc ( int iThreadIndex, char * pstrName, HostAPIFuncPntr fnFunc );

	// p534
	int		XS_GetParamAsInt ( int iThreadIndex, int iParamIndex );
	float	XS_GetParamAsFloat ( int iThreadIndex, int iParamIndex );
	char*   XS_GetParamAsString ( int iThreadIndex, int iParamIndex );

	// p534
	void XS_ReturnFromHost ( int iThreadIndex, int iParamCount );
	void XS_ReturnIntFromHost ( int iThreadIndex, int iParamCount, int iInt );
	void XS_ReturnFloatFromHost ( int iThreadIndex, int iParamCount, float iFloat );
	void XS_ReturnStringFromHost ( int iThreadIndex, int iParamCount, char * pstrString );
	
	// p539,p540
	void XS_InvokeScriptFunc ( int iThreadIndex, char * pstrName );



private:
	int		LoadScript ( string& pstrFilename );
	// p539
	void	CallFunc ( int iThreadIndex, int iIndex );
	// p541
	int GetFuncIndexByName ( int iThreadIndex, char * pstrName );

	//p541
	void XS_PassIntParam ( int iThreadIndex, int iInt );
	void XS_PassFloatParam ( int iThreadIndex, float fFloat );
	void XS_PassStringParam ( int iThreadIndex, char * pstrString );
	// p547
	void XS_CallScriptFunc ( int iThreadIndex, char * pstrName );

	// p549
	int XS_GetReturnValueAsInt ( int iThreadIndex );
	float XS_GetReturnValueAsFloat ( int iThreadIndex );
	char * XS_GetReturnValueAsString ( int iThreadIndex );



private:
	Script	m_Script;
};

