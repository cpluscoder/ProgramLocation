#include "StdAfx.h"
#include "ProgramRegist.h"
#include <algorithm>

using namespace std;

//#define MSDN_SOURCE

#ifdef MSDN_SOURCE
#include <Windows.h>
#include <malloc.h>
#include <stdio.h>
#define TOTALBYTES    8192
#define BYTEINCREMENT 4096
void ReadKey(void)
{
	DWORD BufferSize = TOTALBYTES;
	DWORD cbData;
	DWORD dwRet;

	//PPERF_DATA_BLOCK PerfData = (PPERF_DATA_BLOCK) malloc(BufferSize);
	char *PerfData = new char[TOTALBYTES];
	ZeroMemory(PerfData, TOTALBYTES);
	//cbData = BufferSize;
	cbData = TOTALBYTES;

	TRACE("\nRetrieving the data...");
	dwRet = ::RegQueryValueEx(HKEY_PERFORMANCE_DATA, "Global", NULL, NULL, (LPBYTE) PerfData, &cbData);
	while(dwRet == ERROR_MORE_DATA)
	{
		// Get a buffer that is big enough.
		BufferSize += BYTEINCREMENT;
		//PerfData = (PPERF_DATA_BLOCK) realloc(PerfData, BufferSize);
		PerfData = (char *)(realloc(PerfData, BufferSize));
		cbData = BufferSize;
		TRACE(".");
		dwRet = ::RegQueryValueEx(HKEY_PERFORMANCE_DATA, TEXT("Global"), NULL, NULL, (LPBYTE) PerfData, &cbData);
	}
	if(dwRet == ERROR_SUCCESS)
	{
		TRACE("\n\nFinal buffer size is %d\n", BufferSize);
	}
	else
	{
		TRACE("\nRegQueryValueEx failed (%d)\n", dwRet);
	}
}

BOOL RegDelnodeRecurse(HKEY hKeyRoot, LPTSTR lpSubKey)
{
	LPTSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	TCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;

	// First, see if we can delete the key without having to recurse.
	lResult = RegDeleteKey(hKeyRoot, lpSubKey);
	if(lResult == ERROR_SUCCESS)
	{
		return TRUE;
	}

	lResult = RegOpenKeyEx(hKeyRoot, lpSubKey, 0, KEY_READ, &hKey);
	if(lResult != ERROR_SUCCESS)
	{
		if(lResult == ERROR_FILE_NOT_FOUND)
		{
			printf("Key not found.\n");
			return TRUE;
		}
		else
		{
			printf("Error opening key.\n");
			return FALSE;
		}
	}

	// Check for an ending slash and add one if it is missing.
	lpEnd = lpSubKey + lstrlen(lpSubKey);
	if(*(lpEnd - 1) != TEXT('\\'))
	{
		*lpEnd =  TEXT('\\');
		lpEnd++;
		*lpEnd =  TEXT('\0');
	}

	// Enumerate the keys
	dwSize = MAX_PATH;
	lResult = RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
	if(lResult == ERROR_SUCCESS)
	{
		do
		{
			//StringCchCopy(lpEnd, MAX_PATH*2, szName);
			if(!RegDelnodeRecurse(hKeyRoot, lpSubKey))
			{
				break;
			}
			dwSize = MAX_PATH;
			lResult = ::RegEnumKeyEx(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
		} while(lResult == ERROR_SUCCESS);
	}

	lpEnd--;
	*lpEnd = TEXT('\0');

	::RegCloseKey(hKey);

	// Try again to delete the key.
	lResult = RegDeleteKey(hKeyRoot, lpSubKey);
	if(lResult == ERROR_SUCCESS)
	{
		return TRUE;
	}

	return FALSE;
}
#endif

CProgramRegist::CProgramRegist(void)
{
}

CProgramRegist::~CProgramRegist(void)
{
}

/*
void CProgramRegist::ReadValue(std::string& strHxInstallPath, std::string& strBwInstallPath) const
{
	strHxInstallPath.clear();
	strBwInstallPath.clear();

	const string strHxSub = "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\App Paths\\fwkp.exe";
	const string strHxValueName = "Path";
	ReadValue(HKEY_LOCAL_MACHINE, strHxSub.c_str(), strHxValueName.c_str(), strHxInstallPath);

	const string strBwSub = "SOFTWARE\\skfpkprj\\skfpkprj";
	const string strBwValueName = "installdir";
	ReadValue(HKEY_LOCAL_MACHINE, strBwSub.c_str(), strBwValueName.c_str(), strBwInstallPath);
}
*/

bool CProgramRegist::ReadValue(HKEY hRoot, const string& strSubKey, const string& strValueName, string& strValue) const
{
	strValue.clear();
	if(strSubKey.empty())
	{
		return false;
	}

	HKEY hKey;
	LONG lResult = ::RegOpenKeyEx(hRoot, strSubKey.c_str(), 0, KEY_QUERY_VALUE, &hKey);
	if(lResult != ERROR_SUCCESS)
	{
		return false;
	}

	QueryValue(hKey, strValueName, strValue);

	::RegCloseKey(hKey);

	return strValue.size() > 0;
}

bool CProgramRegist::QueryValue(HKEY hKey, const string& strValueName, string& strValue) const
{
	if(strValueName.empty())
	{
		return false;
	}
	
	DWORD dwType = REG_SZ;
	DWORD dwDataSize = MAX_PATH;
	char szValue[MAX_PATH];
	ZeroMemory(szValue, MAX_PATH);
	if(ERROR_SUCCESS == ::RegQueryValueEx(hKey, strValueName.c_str(), NULL, &dwType, (LPBYTE)szValue, &dwDataSize))
	{
		strValue = szValue;
		return true;
	}
	return false;
}

bool CProgramRegist::ReadHxInstallPath(string& strHxInstallPath) const
{
	strHxInstallPath.clear();
	const string strHxSub = "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\App Paths\\fwkp.exe";
	const string strHxValueName = "Path";
	return ReadValue(HKEY_LOCAL_MACHINE, strHxSub.c_str(), strHxValueName.c_str(), strHxInstallPath);
}

bool CProgramRegist::ReadBwInstallPath(string& strBwInstallPath) const
{
	strBwInstallPath.clear();

	const string strBwSub = "SOFTWARE\\skfpkprj\\skfpkprj";
	const string strBwValueName = "installdir";
	return ReadValue(HKEY_LOCAL_MACHINE, strBwSub.c_str(), strBwValueName.c_str(), strBwInstallPath);
}
