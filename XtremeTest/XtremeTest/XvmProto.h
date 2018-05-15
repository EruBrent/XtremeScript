#pragma once
#include "CustomizeStruct.h"

class CXvmProto
{
public:
	CXvmProto();
	~CXvmProto();

public:
	void Test(vector<string>& argv);

private:
	int LoadScript ( string& pstrFilename );

private:
	Script	m_Script;
};

