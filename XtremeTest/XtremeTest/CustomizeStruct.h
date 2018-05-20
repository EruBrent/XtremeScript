#ifndef _CUSTOMIZE_STUCT_H_
#define _CUSTOMIZE_STUCT_H_

// ---- File I/O --------------------------------------------------------------------------

#define EXEC_FILE_EXT				".XSE"		// Executable file extension

#define XSE_ID_STRING               "XSE0"      // Used to validate an .XSE executable

// ---- LoadScript () Error Codes ---------------------------------------------------------

#define LOAD_OK						0			// Load successful
#define LOAD_ERROR_FILE_IO  	    1			// File I/O error (most likely a file
// not found error
#define LOAD_ERROR_INVALID_XSE		2			// Invalid .XSE structure
#define LOAD_ERROR_UNSUPPORTED_VERS	3			// The format version is unsupported

// ---- Operand Types ---------------------------------------------------------------------

#define OP_TYPE_NULL                -1          // Uninitialized/Null data
#define OP_TYPE_INT                 0           // Integer literal value
#define OP_TYPE_FLOAT               1           // Floating-point literal value
#define OP_TYPE_STRING		        2           // String literal value
#define OP_TYPE_ABS_STACK_INDEX     3           // Absolute array index
#define OP_TYPE_REL_STACK_INDEX     4           // Relative array index
#define OP_TYPE_INSTR_INDEX         5           // Instruction index
#define OP_TYPE_FUNC_INDEX          6           // Function index
#define OP_TYPE_HOST_API_CALL_INDEX 7           // Host API call index
#define OP_TYPE_REG                 8           // Register

// ---- Instruction Opcodes ---------------------------------------------------------------

#define INSTR_MOV                   0

#define INSTR_ADD                   1
#define INSTR_SUB                   2
#define INSTR_MUL                   3
#define INSTR_DIV                   4
#define INSTR_MOD                   5
#define INSTR_EXP                   6
#define INSTR_NEG                   7
#define INSTR_INC                   8
#define INSTR_DEC                   9

#define INSTR_AND                   10
#define INSTR_OR                    11
#define INSTR_XOR                   12
#define INSTR_NOT                   13
#define INSTR_SHL                   14
#define INSTR_SHR                   15

#define INSTR_CONCAT                16
#define INSTR_GETCHAR               17
#define INSTR_SETCHAR               18

#define INSTR_JMP                   19
#define INSTR_JE                    20
#define INSTR_JNE                   21
#define INSTR_JG                    22
#define INSTR_JL                    23
#define INSTR_JGE                   24
#define INSTR_JLE                   25

#define INSTR_PUSH                  26
#define INSTR_POP                   27

#define INSTR_CALL                  28
#define INSTR_RET                   29
#define INSTR_CALLHOST              30

#define INSTR_PAUSE                 31
#define INSTR_EXIT                  32

// ---- Stack -----------------------------------------------------------------------------

#define DEF_STACK_SIZE			    1024	    // The default stack size

// ---- Coercion --------------------------------------------------------------------------

#define MAX_COERCION_STRING_SIZE    65          // The maximum allocated space for a
// string coercion (including the null
// terminator)

// ---- Data Structures -----------------------------------------------------------------------

// ---- Runtime Value ---------------------------------------------------------------------

typedef struct _Value							// A runtime value
{
	_Value ()
	{ 
		memset ( this, 0, sizeof ( *this ) );
	}

	int iType;                                  // Type
	union                                       // The value
	{
		int iIntLiteral;                        // Integer literal
		float fFloatLiteral;                    // Float literal
		char* pstrStringLiteral;				// String literal
		int iStackIndex;                        // Stack Index
		int iInstrIndex;                        // Instruction index
		int iFuncIndex;                         // Function index
		int iHostAPICallIndex;                  // Host API Call index
		int iReg;                               // Register code
	};
	int iOffsetIndex;                           // Index of the offset
}
Value;

// ---- Runtime Stack ---------------------------------------------------------------------

typedef struct _RuntimeStack					// A runtime stack
{
	Value * pElmnts;							// The stack elements
	int iSize;									// The number of elements in the stack

	int iTopIndex;								// The top index
	int iFrameIndex;                            // Index of the top of the current
	// stack frame.
}
RuntimeStack;

// ---- Functions -------------------------------------------------------------------------

typedef struct _Func							// A function
{
	_Func (){ memset ( this, 0, sizeof ( *this )); }
	int iEntryPoint;							// The entry point
	int iParamCount;							// The parameter count
	int iLocalDataSize;							// Total size of all local data
	int iStackFrameSize;						// Total size of the stack frame
}
Func;

// ---- Instructions ----------------------------------------------------------------------

typedef struct _Instr                           // An instruction
{
	int iOpcode;                                // The opcode
	int iOpCount;                               // The number of operands
	vector<Value>  pOpList;                            // The operand list
}
Instr;

typedef struct _InstrStream                     // An instruction stream
{
	vector<Instr>  Instrs;							// The instructions themselves
	int iSize;                                  // The number of instructions in the
	// stream
	int iCurrInstr;                             // The instruction pointer
}
InstrStream;

// ---- Host API Call Table ---------------------------------------------------------------

typedef struct _HostAPICallTable				// A host API call table
{
	char ** ppstrCalls;							// Pointer to the call array
	int iSize;									// The number of calls in the array
}
HostAPICallTable;

// ---- Scripts ---------------------------------------------------------------------------

typedef struct _Script							// Encapsulates a full script
{
	// Header data

	int iGlobalDataSize;						// The size of the script's global data
	int iIsMainFuncPresent;                     // Is _Main () present?
	int iMainFuncIndex;							// _Main ()'s function index

	// Runtime tracking

	int iIsPaused;								// Is the script currently paused?
	int iPauseEndTime;			                // If so, when should it resume?

	// Register file

	Value _RetVal;								// The _RetVal register

	// Script data

	InstrStream InstrStream;                    // The instruction stream
	RuntimeStack Stack;                         // The runtime stack
	vector<Func>  vetFuncTable;                          // The function table
	HostAPICallTable HostAPICallTable;			// The host API call table
}
Script;

typedef void ( *HostAPIFuncPntr ) (int iThreadIndex);  // Host API function pointer

typedef struct _HostAPIFunc                     // Host API function
{
	int iIsActive;                              // Is this slot in use?

	int iThreadIndex;                           // The thread to which this function
	// is visible
	char * pstrName;                            // The function name
	HostAPIFuncPntr fnFunc;                     // Pointer to the function definition
}
HostAPIFunc;

#endif // !_C
