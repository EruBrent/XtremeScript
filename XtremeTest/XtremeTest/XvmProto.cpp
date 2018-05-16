#include "stdafx.h"
#include "XvmProto.h"
#include <stdio.h>

CXvmProto::CXvmProto()
{
}


CXvmProto::~CXvmProto()
{
}

void CXvmProto::Test(vector<string>& argv)
{
	if (argv.empty()) return;

	printf("XVM Prototype\n");
	memset(&m_Script, 0, sizeof(Script));
	
	int ret = LoadScript (argv.at(0));
	
}

int CXvmProto::LoadScript ( string& pstrFilename )
{
	FILE * pScriptFile;
	if ( !( pScriptFile = fopen ( pstrFilename.c_str(), "rb" ) ) )
		return LOAD_ERROR_FILE_IO;

	char buf[5] = { 0 };
	fread ( buf, 4, 1, pScriptFile );
	if ( strcmp ( buf, XSE_ID_STRING ) != 0 ) return LOAD_ERROR_INVALID_XSE;

	int iMajorVersion = 0,
		iMinorVersion = 0;

	fread ( &iMajorVersion, 1, 1, pScriptFile );
	fread ( &iMinorVersion, 1, 1, pScriptFile );

	// Validate the version, since this prototype only supports version 0.4 scripts
	//Script g_Script;
	if ( iMajorVersion != 0 || iMinorVersion != 4 )
		return LOAD_ERROR_UNSUPPORTED_VERS;

	// Read the stack size (4 bytes)

	fread ( &m_Script.Stack.iSize, 4, 1, pScriptFile );

	// Check for a default stack size request

	if ( m_Script.Stack.iSize == 0 )
		m_Script.Stack.iSize = DEF_STACK_SIZE;

	m_Script.Stack.pElmnts = ( Value * )malloc ( m_Script.Stack.iSize * sizeof ( Value ) );

	fread ( &m_Script.iGlobalDataSize, 4, 1, pScriptFile );

	// Check for presence of _Main () (1 byte)

	fread ( &m_Script.iIsMainFuncPresent, 1, 1, pScriptFile );

	// Read _Main ()'s function index (4 bytes)

	fread ( &m_Script.iMainFuncIndex, 4, 1, pScriptFile );

	// ---- Read the instruction stream

	// Read the instruction count (4 bytes)

	fread ( &m_Script.InstrStream.iSize, 4, 1, pScriptFile );

	// Allocate the stream

	m_Script.InstrStream.Instrs.reserve ( m_Script.InstrStream.iSize );
	

	return LOAD_OK;
}
