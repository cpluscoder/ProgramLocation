#include "StdAfx.h"
#include "ProgramShortcut.h"

using namespace std;

CProgramShortcut::CProgramShortcut(void)
{
}


CProgramShortcut::~CProgramShortcut(void)
{
}

bool GetShortcutTagPath(const string& strLinkFile, string& strLocation)
{
	HRESULT hr;
	IShellLink* pShellLink = NULL;
	TCHAR szGotPath[MAX_PATH] = {0};
	WIN32_FIND_DATA wfd = {0};

	::CoInitialize(NULL);

	do 
	{
		/// 不要在这个循环内return
		hr = ::CoCreateInstance(
			CLSID_ShellLink,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IShellLink,
			(LPVOID *)&pShellLink);

		if(!SUCCEEDED(hr))
		{
			break;
		}

		IPersistFile * pPersistFile = NULL;
		hr =  pShellLink->QueryInterface(IID_IPersistFile, (void **)&pPersistFile);
		if(!SUCCEEDED(hr))
		{
			break;
		}
		LPCOLESTR strTemp;
		{
			USES_CONVERSION;
			strTemp = A2OLE(strLinkFile.c_str());
		}

		hr = pPersistFile->Load(strTemp, STGM_READ);
		if(!SUCCEEDED(hr))
		{
			break;
		}

		hr = pShellLink->Resolve(0, 0);
		if(!SUCCEEDED(hr))
		{
			break;
		}

		hr = pShellLink->GetPath(szGotPath, MAX_PATH, (WIN32_FIND_DATA *)&wfd, SLGP_SHORTPATH);
		if(SUCCEEDED(hr))
		{
			strLocation = szGotPath;
		}

		pPersistFile->Release();
		pShellLink->Release();
	} while (false);

	::CoUninitialize();

	return !strLocation.empty();
}
