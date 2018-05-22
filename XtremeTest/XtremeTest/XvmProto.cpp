#include "stdafx.h"
#include "XvmProto.h"
#include <stdio.h>

CXvmProto::CXvmProto()
{
	g_iCurrThreadMode = THREAD_MODE_MULTI;
	g_iCurrThread = 0;
}


CXvmProto::~CXvmProto()
{
}

void CXvmProto::Test(vector<string>& argv)
{
	Value* val = ( Value* )malloc ( sizeof ( Value ) );
	memset ( val, 0, sizeof ( Value ) );
	val->iType = 1;
	val->iIntLiteral = 2;
	val->fFloatLiteral = 3;
	Value val1 = val[0];
	if (argv.empty()) return;

	printf("XVM Prototype\n");
	memset(&m_Script, 0, sizeof(Script));
	int ret = LoadScript (argv.at(0));
	
}

int CXvmProto::LoadScript ( string& pstrFilename )
{
	FILE * pScriptFile;
	if ( !( pScriptFile = fopen ( pstrFilename.c_str (), "rb" ) ) )
		return LOAD_ERROR_FILE_IO;

	char buf[5] = { 0 };
	fread ( buf, 4, 1, pScriptFile );
	if ( strcmp ( buf, XSE_ID_STRING ) != 0 ) return LOAD_ERROR_INVALID_XSE;

	int iMajorVersion = 0,
		iMinorVersion = 0;

	fread ( &iMajorVersion, 1, 1, pScriptFile );
	fread ( &iMinorVersion, 1, 1, pScriptFile );

	// Validate the version, since this prototype only supports version 0.4 scripts

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


	for ( int iCurrInstrIndex = 0; iCurrInstrIndex < m_Script.InstrStream.iSize; ++iCurrInstrIndex )
	{
		Instr instr;
		instr.iOpcode = instr.iOpCount = 0;
		fread ( &instr.iOpcode, 2, 1, pScriptFile );
		fread ( &instr.iOpCount, 1, 1, pScriptFile );

		Value value;
		for ( int i = 0; i < instr.iOpCount; i++ )
		{
			value.iType = 0;
			fread ( &value.iType, 1, 1, pScriptFile );
			switch ( value.iType )
			{
				case OP_TYPE_INT:
				fread ( &value.iIntLiteral, sizeof ( int ), 1, pScriptFile );
				break;
				case OP_TYPE_FLOAT:
				fread ( &value.fFloatLiteral, sizeof ( float ), 1, pScriptFile );
				break;
				case OP_TYPE_STRING:
				fread ( &value.pstrStringLiteral, sizeof ( int ), 1, pScriptFile );
				break;
				case OP_TYPE_INSTR_INDEX:
				fread ( &value, sizeof ( int ), 1, pScriptFile );
				break;
				case OP_TYPE_ABS_STACK_INDEX:
				fread ( &value, sizeof ( int ), 1, pScriptFile );
				break;
				case OP_TYPE_REL_STACK_INDEX:
				fread ( &value, sizeof ( int ), 1, pScriptFile );
				break;
				case OP_TYPE_FUNC_INDEX:
				fread ( &value, sizeof ( int ), 1, pScriptFile );
				break;
				case OP_TYPE_HOST_API_CALL_INDEX:
				fread ( &value, sizeof ( int ), 1, pScriptFile );
				break;
				case OP_TYPE_REG:
				fread ( &value, sizeof ( int ), 1, pScriptFile );
				break;
				default:
				break;
			}
			instr.pOpList.push_back ( value );
		}
		m_Script.InstrStream.Instrs.push_back ( instr );
	}

	// ---- Read the string table

	// Read the table size (4 bytes)
	int iStringTableSize;
	fread ( &iStringTableSize, 4, 1, pScriptFile );

	if ( iStringTableSize )
	{
		vector<string> vetStr;
		for ( int i = 0; i < iStringTableSize; i++ )
		{
			int iStringSize;
			fread ( &iStringSize, 4, 1, pScriptFile );
			string str ( iStringSize, '\0' );
			fread ( &str[0], iStringSize, 1, pScriptFile );
			vetStr.push_back ( str );
		}

		for ( int i = 0; i < m_Script.InstrStream.iSize; i++ )
		{
			for ( int j = 0; j < m_Script.InstrStream.Instrs[i].iOpCount; j++ )
			{
				auto& vetOplist = m_Script.InstrStream.Instrs[i].pOpList;
				if ( vetOplist.at ( j ).iType == OP_TYPE_STRING )
				{
					char* pCopy = ( char* )malloc ( vetStr.at ( vetOplist.at ( j ).iIntLiteral ).size () +1 );
					strcpy ( pCopy, vetStr.at ( vetOplist.at ( j ).iIntLiteral ).c_str());
					vetOplist.at ( j ).pstrStringLiteral = pCopy;
				}
			}
		}
	}

	// ---- Read the function table

	int iFuncTableSize;
	fread ( &iFuncTableSize, 4, 1, pScriptFile );
	m_Script.vetFuncTable.reserve ( iFuncTableSize );
	for ( int iCurrFuncIndex = 0; iCurrFuncIndex < iFuncTableSize; ++iCurrFuncIndex )
	{
		// Read the entry point (4 bytes)
		Func fuc;
		int iEntryPoint;
		fread ( &iEntryPoint, 4, 1, pScriptFile );

		// Read the parameter count (1 byte)

		int iParamCount = 0;
		fread ( &iParamCount, 1, 1, pScriptFile );

		// Read the local data size (4 bytes)

		int iLocalDataSize;
		fread ( &iLocalDataSize, 4, 1, pScriptFile );

		// Calculate the stack size

		int iStackFrameSize = iParamCount + 1 + iLocalDataSize;
		fuc.iEntryPoint = iEntryPoint;
		fuc.iLocalDataSize = iLocalDataSize;
		fuc.iParamCount = iParamCount;
		fuc.iStackFrameSize = iStackFrameSize;
		m_Script.vetFuncTable.push_back ( fuc );

	}

	fclose ( pScriptFile );


	printf ( "%s loaded successfully!\n", pstrFilename );
	printf ( "\n" );

	return LOAD_OK;
}

void CXvmProto::RunScript ()
{
	Script& g_Script = m_Script;
	int iCurrInstr = m_Script.InstrStream.iCurrInstr;

	// Get the current opcode

	int iOpcode = g_Script.InstrStream.Instrs[iCurrInstr].iOpcode;

	switch ( iOpcode )
	{
		case INSTR_MOV:

		// Arithmetic Operations

		case INSTR_ADD:
		case INSTR_SUB:
		case INSTR_MUL:
		case INSTR_DIV:
		case INSTR_MOD:
		case INSTR_EXP:

		// Bitwise Operations

		case INSTR_AND:
		case INSTR_OR:
		case INSTR_XOR:
		case INSTR_SHL:
		case INSTR_SHR:
		{
			Value Dest = ResolveOpValue ( 0 );

			// Get a local copy of the source operand (operand index 1)

			Value Source = ResolveOpValue ( 1 );

			switch ( iOpcode )
			{
				case INSTR_MOV:

				// Skip cases where the two operands are the same

				if ( ResolveOpPntr ( 0 ) == ResolveOpPntr ( 1 ) )
					break;

				// Copy the source operand into the destination

				CopyValue ( &Dest, Source );

				break;
			}
			break;
		}
	}
}

Value CXvmProto::ResolveOpValue ( int iOpIndex )
{
	int iCurrInstr = m_Script.InstrStream.iCurrInstr;

	// Get the operand type type

	Value OpValue = m_Script.InstrStream.Instrs[iCurrInstr].pOpList[iOpIndex];
	switch ( OpValue.iType )
	{
		// It's a stack index so resolve it

		case OP_TYPE_ABS_STACK_INDEX:
		case OP_TYPE_REL_STACK_INDEX:
		{
			// Resolve the index and use it to return the corresponding stack element
			if ( OpValue.iType == OP_TYPE_ABS_STACK_INDEX )
				return GetStackValue ( OpValue.iStackIndex );
			else
				return GetStackValue (OpValue.iStackIndex + GetStackValue ( OpValue.iOffsetIndex ).iIntLiteral);

		}

		// It's in _RetVal

		case OP_TYPE_REG:
		return m_Script._RetVal;

		// Anything else can be returned as-is

		default:
		return OpValue;
	}
}


Value CXvmProto::GetStackValue ( int iIndex )
{
	// Use ResolveStackIndex () to return the element at the specified index
	auto f = [this]( int iIndex ){return iIndex < 0 ? iIndex += m_Script.Stack.iFrameIndex : iIndex; };
	return m_Script.Stack.pElmnts[f(iIndex)] ;
}