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

	
}

int CXvmProto::LoadScript ( string& pstrFilename )
{

	return LOAD_OK;
}
