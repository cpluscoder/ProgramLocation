#include "StdAfx.h"
#include "SearchProcess.h"

using namespace std;


CSearchProcess::CSearchProcess(void)
{
}


CSearchProcess::~CSearchProcess(void)
{
}

#include <tlhelp32.h>
#include <boost/algorithm/string.hpp>


bool CSearchProcess::QueryProcessFullPath(const string& strProcessName, string& strFullPath)
{
	strFullPath.clear();
	if(strProcessName.empty())
	{
		return false;
	}

	bool bResult = false;
	DWORD dwPID = 0;
	if(!GetTargetPID(strProcessName, dwPID))
	{
		return false;
	}
	if(!GetProcessFullPath(dwPID, strProcessName, strFullPath))
	{
		return false;
	}

	return true;

/*
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	PROCESSENTRY32 processEntry32;
	ZeroMemory(&processEntry32, sizeof(processEntry32));
	processEntry32.dwSize = sizeof(PROCESSENTRY32);
	if(::Process32First(hProcessSnap, &processEntry32))
	{
		do
		{
			if(boost::algorithm::iequals(strProcessName, string(processEntry32.szExeFile)))
			{
				TRACE("\n\n=====================================================");
				TRACE("\nPROCESS NAME:  %s", processEntry32.szExeFile);
				TRACE("\n-------------------------------------------------------");
				TRACE("\n  Process ID        = %d", processEntry32.th32ProcessID);
				TRACE("\n  Thread count      = %d", processEntry32.cntThreads);
				TRACE("\n  Parent process ID = %d", processEntry32.th32ParentProcessID);
				TRACE("\n  Priority base     = %d", processEntry32.pcPriClassBase);
				if(GetProcessFullPath(processEntry32.th32ProcessID, strProcessName, strFullPath))
				{
					bResult = true;
				}
				break;
			}
		} while(::Process32Next(hProcessSnap, &processEntry32));
	}

	::CloseHandle(hProcessSnap);
*/
	return bResult;
}

bool CSearchProcess::GetTargetPID(const string& strProcessName, DWORD& dwPID)
{
	if(strProcessName.empty())
	{
		return false;
	}

	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	bool bResult = false;
	PROCESSENTRY32 processEntry32;
	ZeroMemory(&processEntry32, sizeof(processEntry32));
	processEntry32.dwSize = sizeof(PROCESSENTRY32);
	if(::Process32First(hProcessSnap, &processEntry32))
	{
		do
		{
			if(boost::algorithm::iequals(strProcessName, string(processEntry32.szExeFile)))
			{
				TRACE("\n\n=====================================================");
				TRACE("\nPROCESS NAME:  %s", processEntry32.szExeFile);
				TRACE("\n-------------------------------------------------------");
				TRACE("\n  Process ID        = %d", processEntry32.th32ProcessID);
				TRACE("\n  Thread count      = %d", processEntry32.cntThreads);
				TRACE("\n  Parent process ID = %d", processEntry32.th32ParentProcessID);
				TRACE("\n  Priority base     = %d", processEntry32.pcPriClassBase);
				dwPID = processEntry32.th32ProcessID;
				bResult = true;
				break;
			}
		} while(::Process32Next(hProcessSnap, &processEntry32));
	}

	::CloseHandle(hProcessSnap);
	return bResult;
}

bool CSearchProcess::GetProcessFullPath(DWORD dwPID, const string& strProcessName, string& strFullPath)
{
	strFullPath.clear();
	if(strProcessName.empty())
	{
		return false;
	}

	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if(hModuleSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	bool bResult = false;
	MODULEENTRY32 moduleEntry32;
	ZeroMemory(&moduleEntry32, sizeof(MODULEENTRY32));
	moduleEntry32.dwSize = sizeof(MODULEENTRY32);
	if(::Module32First(hModuleSnap, &moduleEntry32))
	{
		do
		{
			TRACE("\n\n     MODULE NAME:     %s",		moduleEntry32.szModule);
			TRACE("\n     Executable     = %s",		moduleEntry32.szExePath);
			TRACE("\n     Process ID     = %d",    moduleEntry32.th32ProcessID);
			if(boost::algorithm::iequals(strProcessName, string(moduleEntry32.szModule)))
			{
				strFullPath = moduleEntry32.szExePath;
				bResult = true;
				break;
			}
		} while(::Module32Next(hModuleSnap, &moduleEntry32));
	}

	::CloseHandle(hModuleSnap);
	return bResult;
}

bool CSearchProcess::GetProcessList(void)
{
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	// Take a snapshot of all processes in the system.
	hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printError(TEXT("CreateToolhelp32Snapshot (of processes)"));
		return false;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// Retrieve information about the first process, and exit if unsuccessful
	if(!::Process32First(hProcessSnap, &pe32))
	{
		// show cause of failure
		printError(TEXT("Process32First"));

		// clean the snapshot object
		::CloseHandle(hProcessSnap);

		return false;
	}

	// Now walk the snapshot of processes, and display information about each process in turn
	do
	{
		TRACE("\n\n=====================================================");
		TRACE("\nPROCESS NAME:  %s", pe32.szExeFile);
		TRACE("\n-------------------------------------------------------");

		// Retrieve the priority class.
		dwPriorityClass = 0;
		hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
		if(hProcess == NULL)
		{
			printError("OpenProcess");
		}
		else
		{
			dwPriorityClass = ::GetPriorityClass(hProcess);
			if(!dwPriorityClass)
			{
				printError("GetPriorityClass");
			}
			::CloseHandle(hProcess);
		}

		TRACE("\n  Process ID        = %d", pe32.th32ProcessID);
		TRACE("\n  Thread count      = %d",   pe32.cntThreads);
		TRACE("\n  Parent process ID = %d", pe32.th32ParentProcessID);
		TRACE("\n  Priority base     = %d", pe32.pcPriClassBase);
		if(dwPriorityClass)
		{
			TRACE("\n  Priority class    = %d", dwPriorityClass);
		}

		// List the modules and threads associated with this process
		ListProcessModules(pe32.th32ProcessID);
		//ListProcessThreads(pe32.th32ProcessID);

	} while(::Process32Next(hProcessSnap, &pe32));

	::CloseHandle(hProcessSnap);
	return true;
}


bool CSearchProcess::ListProcessModules(DWORD dwPID)
{
	// Take a snapshot of all modules in the specified process.
	HANDLE hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
	if(hModuleSnap == INVALID_HANDLE_VALUE)
	{
		printError("CreateToolhelp32Snapshot (of modules)");
		return false;
	}

	MODULEENTRY32 me32;

	// Set the size of the structure before using it.
	me32.dwSize = sizeof(MODULEENTRY32);

	// Retrieve information about the first module, and exit if unsuccessful
	if(::Module32First(hModuleSnap, &me32))
	{
		// Now walk the module list of the process, and display information about each module
		do
		{
			TRACE("\n\n     MODULE NAME:     %s",		me32.szModule);
			TRACE("\n     Executable     = %s",		me32.szExePath);
			//TRACE("\n     Process ID     = 0x%08X",    me32.th32ProcessID);
			TRACE("\n     Process ID     = %d",    me32.th32ProcessID);
			//TRACE("\n     Ref count (g)  = 0x%04X",    me32.GlblcntUsage);
			//TRACE("\n     Ref count (p)  = 0x%04X",    me32.ProccntUsage);
			//TRACE("\n     Base address   = 0x%08X",	(DWORD) me32.modBaseAddr);
			//TRACE("\n     Base size      = %d",        me32.modBaseSize);

		} while(::Module32Next(hModuleSnap, &me32));
	}

	::CloseHandle(hModuleSnap);

	return true;
}

bool CSearchProcess::ListProcessThreads(DWORD dwOwnerPID)
{
	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;
	THREADENTRY32 te32;

	// Take a snapshot of all running threads
	hThreadSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if(hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	// Fill in the size of the structure before using it.
	te32.dwSize = sizeof(THREADENTRY32);

	// Retrieve information about the first thread,
	// and exit if unsuccessful
	if(!::Thread32First(hThreadSnap, &te32))
	{
		printError("Thread32First"); // show cause of failure
		::CloseHandle(hThreadSnap);          // clean the snapshot object
		return false;
	}

	// Now walk the thread list of the system,
	// and display information about each thread
	// associated with the specified process
	do
	{
		if(te32.th32OwnerProcessID == dwOwnerPID)
		{
			TRACE("\n\n     THREAD ID      = 0x%08X", te32.th32ThreadID);
			TRACE("\n     Base priority  = %d", te32.tpBasePri);
			TRACE("\n     Delta priority = %d", te32.tpDeltaPri);
			TRACE("\n");
		}
	} while(Thread32Next(hThreadSnap, &te32));

	::CloseHandle(hThreadSnap);
	return true;
}

void CSearchProcess::printError(TCHAR* msg)
{
	DWORD eNum;
	TCHAR sysMsg[256];
	TCHAR* p;

	eNum = GetLastError();
	::FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, 
		eNum,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		sysMsg, 
		256, 
		NULL);

	// Trim the end of the line and terminate it with a null
	p = sysMsg;
	while((*p > 31) || (*p == 9))
	{
		++p;
	}

	do 
	{
		*p-- = 0;
	} 
	while((p >= sysMsg) && ((*p == '.') || (*p < 33)));

	// Display the message
	TRACE("\n  WARNING: %s failed with error %d (%s)", msg, eNum, sysMsg);
}


