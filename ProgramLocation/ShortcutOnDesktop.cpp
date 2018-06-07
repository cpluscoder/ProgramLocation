#include "StdAfx.h"
#include "ShortcutOnDesktop.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>


using namespace std;

CShortcutOnDesktop::CShortcutOnDesktop(void)
{
#ifdef DOWNLOAD_SOURCE
	IShellFolder *m_pIShellFolder = NULL;
	m_pIEnumFolder=NULL;
	m_pAppData=NULL;
	m_pIEnumFile=NULL;
	m_pFirstLayerFolder=NULL;
	m_pFirstLayerFile=NULL;
	memset(m_strDeskTopPath,0,sizeof(m_strDeskTopPath));
	memset(m_strQuickLanchPath,0,sizeof(m_strQuickLanchPath));
	memset(m_strParentPath,0,sizeof(m_strParentPath));
	Brower[0]="360SE.EXE";//360浏览器
	Brower[1]="IEXPLORE.EXE";//IE浏览器
	Brower[2]="FIREFOX.EXE";//火狐
	Brower[3]="OPERA.EXE";//Opera
	Brower[4]="MAXTHON.EXE";//遨游
	Brower[5]="LOCAL SET";//Google
	Brower[6]="THEWORLD.EXE";//世界之窗
	Brower[7]="TTRAVELER.EXE";//腾讯TT
	m_iCount=0;
#else
	m_vctShortcut.clear();
	m_mapShortcut2Link.clear();
#endif // DOWNLOAD_SOURCE
}

CShortcutOnDesktop::~CShortcutOnDesktop(void)
{
	m_vctShortcut.clear();
	m_mapShortcut2Link.clear();
}

bool CShortcutOnDesktop::GetCurrentUserDesktopFolder(string& strUserDesktop)
{
	char szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH);

	strUserDesktop.clear();
	if(!::SHGetSpecialFolderPath(NULL, szPath, CSIDL_DESKTOPDIRECTORY, FALSE))
	{
		return false;
	}

	TRACE("%s\n", szPath);
	strUserDesktop = szPath;

	return true;
}

bool CShortcutOnDesktop::GetAllUserDesktopFolder(string& strAllUsersDesktop)
{
	char szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH);

	strAllUsersDesktop.clear();
	if(!::SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE))
	{
		return false;
	}

	TRACE("%s\n", szPath);
	strAllUsersDesktop = szPath;

	return true;
}

bool CShortcutOnDesktop::GetShortcut(const string& strDesktopFolder, vector<string>& vctShortcut)
{
	if(strDesktopFolder.empty())
	{
		return false;
	}

	const string cstrExt = ".LNK";
	LPWSTR pwszPath = NULL;
	{USES_CONVERSION; pwszPath = A2W(strDesktopFolder.c_str());}
	string _strDesktopFolder = strDesktopFolder;
	_strDesktopFolder.append("\\");

	IShellFolder *pDesktopFolder = NULL;
	if(SUCCEEDED(::SHGetDesktopFolder(&pDesktopFolder)))
	{
		if(pDesktopFolder)
		{
			LPITEMIDLIST pItemIdList = NULL;
			if(SUCCEEDED(pDesktopFolder->ParseDisplayName(NULL, NULL, pwszPath, NULL, &pItemIdList, NULL)))
			{
				if(pItemIdList)
				{
					IShellFolder* pIUserDesktopFolder = NULL;
					if(SUCCEEDED(pDesktopFolder->BindToObject(pItemIdList, NULL, IID_IShellFolder, (LPVOID*)&pIUserDesktopFolder)))
					{
						if(pIUserDesktopFolder)
						{
							IEnumIDList* pEnumIdList = NULL;
							if(SUCCEEDED(pIUserDesktopFolder->EnumObjects(NULL, SHCONTF_NONFOLDERS, &pEnumIdList)))
							{
								if(pEnumIdList)
								{
									ULONG celtFetched = 0;
									LPITEMIDLIST _pItemIdList = NULL;
									while(S_OK == pEnumIdList->Next(1, &_pItemIdList, &celtFetched))
									{
										WIN32_FIND_DATA findData;
										if(S_OK == ::SHGetDataFromIDList(pDesktopFolder, _pItemIdList, SHGDFIL_FINDDATA, &findData, sizeof(WIN32_FIND_DATA)))
										{
											string strFile = _strDesktopFolder + findData.cFileName;
											boost::filesystem::path fsFile(strFile);
											if(fsFile.has_extension())
											{
												if(boost::algorithm::iequals(cstrExt, fsFile.extension().string()))
												{
													vctShortcut.push_back(strFile);
												}
											}
										}
									}
									pEnumIdList->Release();
								}
							}
							pIUserDesktopFolder->Release();
						}
					}
				}
			}
			pDesktopFolder->Release();
		}
	}

	return !(vctShortcut.empty());
}

void CShortcutOnDesktop::GetDesktopShortcut(void)
{
	m_vctShortcut.clear();

	string strUserDesktop;
	if(GetCurrentUserDesktopFolder(strUserDesktop))
	{
		GetShortcut(strUserDesktop, m_vctShortcut);
	}

	string strAllUsersDesktop;
	if(GetAllUserDesktopFolder(strAllUsersDesktop))
	{
		GetShortcut(strAllUsersDesktop, m_vctShortcut);
	}

	for_each(m_vctShortcut.begin(), m_vctShortcut.end(), [](const string& strShortcut){
		TRACE("%s\n", strShortcut.c_str());
	});
}

void CShortcutOnDesktop::GetShortcutLink(void)
{
	char szPath[MAX_PATH];
	LPCOLESTR pwszFileName;
	m_mapShortcut2Link.clear();
	for_each(m_vctShortcut.begin(), m_vctShortcut.end(), [&](const string& strShortcut) {
		IShellLink* pIShellLink = NULL;
		if(SUCCEEDED(::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pIShellLink)))
		{
			IPersistFile* pIPersistFile = NULL;
			if(SUCCEEDED(pIShellLink->QueryInterface(IID_IPersistFile, (void **)&pIPersistFile)))
			{
				{USES_CONVERSION; pwszFileName = T2COLE(strShortcut.c_str());}
				if(SUCCEEDED(pIPersistFile->Load(pwszFileName, STGM_READWRITE)))
				{
					ZeroMemory(szPath, sizeof(szPath));
					if(SUCCEEDED(pIShellLink->GetPath(szPath, sizeof(szPath), NULL, SLGP_UNCPRIORITY)))
					{
						m_mapShortcut2Link[strShortcut] = string(szPath);
					}
				}
				pIPersistFile->Release();
			}
			pIShellLink->Release();
		}
	});

/*
	for(auto iter = m_vctShortcut.begin(); iter != m_vctShortcut.end(); ++iter)
	{
		IShellLink* pIShellLink = NULL;
		if(SUCCEEDED(::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pIShellLink)))
		{
			IPersistFile* pIPersistFile = NULL;
			if(SUCCEEDED(pIShellLink->QueryInterface(IID_IPersistFile, (void **)&pIPersistFile)))
			{
				{
					USES_CONVERSION;
					strFileName = T2COLE((*iter).c_str());
				}
				if(SUCCEEDED(pIPersistFile->Load(strFileName, STGM_READWRITE)))
				{
					ZeroMemory(szPath, sizeof(szPath));
					if(SUCCEEDED(pIShellLink->GetPath(szPath, sizeof(szPath), NULL, SLGP_UNCPRIORITY)))
					{
						m_mapShortcut2Link[*iter] = string(szPath);
					}
				}
				pIPersistFile->Release();
			}
			pIShellLink->Release();
		}
	}
*/
}

void CShortcutOnDesktop::SearchShortcut(const string& strTarget, unordered_map<string, string>& mapSearchResult)
{
	mapSearchResult.clear();
	for(auto iter = m_mapShortcut2Link.begin(); iter != m_mapShortcut2Link.end(); ++iter)
	{
		auto iterRange = boost::algorithm::ifind_first(iter->second, strTarget);
		if(!iterRange.empty())
		{
			mapSearchResult[iter->first] = iter->second;
			TRACE("%s\n->\n%s\n", iter->first.c_str(), iter->second.c_str());
		}
	}
}

#ifdef DOWNLOAD_SOURCE
void Test01(void)
{
	LPITEMIDLIST pidlDesktop = NULL;

	char szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH);
	if(::SHGetSpecialFolderPath(NULL, szPath, CSIDL_DESKTOPDIRECTORY, FALSE))
	{
		TRACE("%s\n", szPath);
	}
	if(::SHGetSpecialFolderPath(NULL, szPath, CSIDL_COMMON_DESKTOPDIRECTORY, FALSE))
	{
		TRACE("%s\n", szPath);
	}
}

void Test(void)
{
	IShellFolder *psfDeskTop = NULL;
	LPITEMIDLIST pidlProgFiles = NULL;
	IShellFolder *psfProgFiles = NULL;
	LPITEMIDLIST pidlItems = NULL;
	LPENUMIDLIST pEnumList = NULL;
	IShellFolder *psfFirstFolder = NULL;
	ULONG celtFetched;
	HRESULT hr;
	STRRET strDispName;
	char pszDisplayName[MAX_PATH];
	ULONG uAttr;

	::CoInitialize(NULL);

	hr = SHGetFolderLocation(NULL, CSIDL_PROGRAM_FILES, NULL, NULL, &pidlProgFiles);
	hr = SHGetDesktopFolder(&psfDeskTop);
	hr = psfDeskTop->BindToObject(pidlProgFiles, NULL, IID_IShellFolder, (LPVOID *) &psfProgFiles);
	psfDeskTop->Release();

	hr = psfProgFiles->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnumList);
	while(hr = pEnumList->Next(1, &pidlItems, &celtFetched) == S_OK && (celtFetched) == 1)
	{
		psfProgFiles->GetDisplayNameOf(pidlItems, SHGDN_INFOLDER, &strDispName);
		StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
		TRACE("%s\n", pszDisplayName);
		if(!psfFirstFolder)
		{
			uAttr = SFGAO_FOLDER;
			psfProgFiles->GetAttributesOf(1, (LPCITEMIDLIST*)&pidlItems, &uAttr);
			if(uAttr & SFGAO_FOLDER)
			{
				hr = psfProgFiles->BindToObject(pidlItems, NULL, IID_IShellFolder, (LPVOID *) &psfFirstFolder);
			}
		}
		CoTaskMemFree(pidlItems);
	}
	TRACE("\n\n");
	pEnumList->Release();

	if(psfFirstFolder)
	{
		hr = psfFirstFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnumList);
		while(hr = pEnumList->Next(1, &pidlItems, &celtFetched) == S_OK && (celtFetched) == 1)
		{
			psfFirstFolder->GetDisplayNameOf(pidlItems, SHGDN_INFOLDER, &strDispName);
			StrRetToBuf(&strDispName, pidlItems, pszDisplayName, MAX_PATH);
			TRACE("%s\n", pszDisplayName);
			CoTaskMemFree(pidlItems);
		}
	}
	pEnumList->Release();
	CoTaskMemFree(pidlProgFiles);
	psfProgFiles->Release();
	psfFirstFolder->Release();

	::CoUninitialize();
}

//获取桌面文件夹的IShellFolder接口指针
BOOL CShortcutOnDesktop::GetDesktopIShellFolder(void)
{
	m_pIShellFolder = GetIShellFolderByPath(m_strDeskTopPath);
	m_pAppData = GetIShellFolderByPath(m_strQuickLanchPath);
	return TRUE;
}

//获取桌面文件夹的IEnumIDList接口
BOOL CShortcutOnDesktop::GetIEunmIDList(IShellFolder * m_pShellFolder, BOOL isRecur, BOOL isQuickLanch)
{
	if(!m_pShellFolder)
		return FALSE;

	HRESULT hr = m_pShellFolder->EnumObjects(0, SHCONTF_NONFOLDERS, &m_pIEnumFile);
	if(FAILED(hr))
	{
		return FALSE;
	}
	if(!isRecur)
	{
		m_pFirstLayerFile = m_pIEnumFile;//第一层文件夹的遍历接口指针
	}
	EnumAllItems(m_pIEnumFile, FALSE, isQuickLanch);

	if(!isQuickLanch)
	{
		HRESULT hr=m_pShellFolder->EnumObjects(0, SHCONTF_FOLDERS, &m_pIEnumFolder);
		if(FAILED(hr))
		{
			return FALSE;
		}

		if(!isRecur)
		{
			m_pFirstLayerFolder = m_pIEnumFolder;//第一层文件夹的遍历接口指针
		}
		EnumAllItems(m_pIEnumFolder, TRUE, isQuickLanch);
	}

	return TRUE;
}

//遍历桌面文件夹第一层里的所有项
BOOL CShortcutOnDesktop::EnumAllItems(IEnumIDList *pEnum, BOOL isFolder, BOOL isQuickLanch)
{
	ULONG m_ulwork= 0;
	LPITEMIDLIST m_pItem = NULL;
	
	while(pEnum->Next(1, &m_pItem, &m_ulwork)==S_OK)
	{
		//如果是第一层，重置路径
		if(!isQuickLanch)
		{
			if((m_pFirstLayerFolder == pEnum) && (isFolder))
			{
				lstrcpy(m_strParentPath, m_strDeskTopPath);
			}

			if((m_pFirstLayerFile == pEnum) && (!isFolder))
			{
				lstrcpy(m_strParentPath, m_strDeskTopPath);
			}
		}
		else
		{
			if((m_pFirstLayerFile == pEnum) && (!isFolder))
			{
				lstrcpy(m_strParentPath, m_strQuickLanchPath);
			}
		}

		WIN32_FIND_DATA ffd;
		SHGetDataFromIDList(m_pIShellFolder, m_pItem, SHGDFIL_FINDDATA, &ffd, sizeof(WIN32_FIND_DATA));
		if(!isFolder)
		{
			CString m_strTempPath = m_strParentPath;
			m_strTempPath += "\\";
			m_strTempPath += ffd.cFileName;
			TRACE("%s\n", m_strTempPath);
			CString m_strCmp = ".lnk";
			m_strTempPath.MakeUpper();
			m_strCmp.MakeUpper();
			if(m_strTempPath.Right(4) == m_strCmp)
			{
				ReadShortCut(m_strTempPath.GetBuffer());
				m_strTempPath.ReleaseBuffer();
			}
		}
		else
		{
			lstrcat(m_strParentPath, "\\");
			lstrcat(m_strParentPath,ffd.cFileName);
			IShellFolder *m_pITemp = GetIShellFolderByPath(m_strParentPath);
			GetIEunmIDList(m_pITemp,TRUE,isQuickLanch);
		}
	}
	return TRUE;
}

//获取桌面文件夹和快速启动文件夹的路径
int CShortcutOnDesktop::GetPath(char * DeskTop, char * AppData)
{
	CRegKey regKey;
	if(ERROR_SUCCESS == regKey.Open(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders", KEY_READ))
	{
		ULONG m_ulsize1=1000;
		ULONG m_ulsize2=1000;
		regKey.QueryStringValue("Desktop", DeskTop, &m_ulsize1);
		regKey.QueryStringValue("AppData", AppData, &m_ulsize2);
		lstrcat(AppData, "\\Microsoft\\Internet Explorer\\Quick Launch");
	}
	return 0;
}

//获取指定目录的IShellFolder接口
IShellFolder * CShortcutOnDesktop::GetIShellFolderByPath(LPTSTR path)
{
	IShellFolder *m_ShellFolderTopMost = NULL;
	HRESULT hr = SHGetDesktopFolder(&m_ShellFolderTopMost);
	if(FAILED(hr))
	{
		return NULL;
	}
	IShellFolder * m_pFolder = NULL;
	LPITEMIDLIST pidlWorkDir = NULL;

	wchar_t *pwszPath;
	{
		USES_CONVERSION;
		pwszPath = A2W(path);
	}

	hr = m_ShellFolderTopMost->ParseDisplayName(NULL, NULL, pwszPath, NULL, &pidlWorkDir, NULL);
	if(FAILED(hr))
	{
		return NULL;
	}
	hr = m_ShellFolderTopMost->BindToObject(pidlWorkDir, NULL, IID_IShellFolder, (LPVOID*)&m_pFolder);
	if(m_ShellFolderTopMost)
		m_ShellFolderTopMost->Release();
	return m_pFolder;
}

//读取快捷方式的信息
BOOL CShortcutOnDesktop::ReadShortCut(LPTSTR linkName)
{
	::CoInitialize(NULL);

	IShellLink *m_pIShellLink = NULL;
	IPersistFile *m_pIPersistFile = NULL;
	HRESULT hr = ::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&m_pIShellLink);
	if(SUCCEEDED(hr))
	{
		hr = m_pIShellLink->QueryInterface(IID_IPersistFile, (void **)&m_pIPersistFile);
		if(SUCCEEDED(hr))
		{
			LPCOLESTR strFileName;
			{
				USES_CONVERSION;
				strFileName = T2COLE(linkName);
			}
			m_pIPersistFile->Load(strFileName, STGM_READWRITE);
			char szPath[1000] = {0};
			//读取快捷方式的目标路径
			m_pIShellLink->GetPath(szPath, sizeof(szPath), NULL, SLGP_UNCPRIORITY);
			CString strTemp = szPath;
			TRACE("%s\n", strTemp);
/*
			temp.MakeUpper();
			for(int i = 0; i = 7; i++)
			{
				if(i == 5)
				{
					if(strcmp(temp.Right(9), Brower[i])==0)
					{
						m_pIShellLink->SetArguments("");
						LPCOLESTR strFileName;
						{
							USES_CONVERSION;
							strFileName = T2COLE(linkName);
						}
						m_pIPersistFile->Save(strFileName, TRUE);
					}
				}
				else
				{
					if(temp.Find(Brower[i]) != -1)
					{
						m_pIShellLink->SetArguments("");
						LPCOLESTR strFileName;
						{
							USES_CONVERSION;
							strFileName = T2COLE(linkName);
						}
						m_pIPersistFile->Save(strFileName, TRUE);
					}
				}
			}
*/
		}
		else
		{
			if(m_pIShellLink)
				m_pIShellLink->Release();
			if(m_pIPersistFile)
				m_pIPersistFile->Release();

			return FALSE;
		}
	}
	else
	{
		if(m_pIShellLink) m_pIShellLink->Release();
		if(m_pIPersistFile) m_pIPersistFile->Release();
		::CoUninitialize();
		return FALSE;
	}

	if(m_pIShellLink)
		m_pIShellLink->Release();

	if(m_pIPersistFile)
		m_pIPersistFile->Release();

	::CoUninitialize();

	return TRUE;
}

void CShortcutOnDesktop::Test(void)
{
	GetPath(m_strDeskTopPath, m_strQuickLanchPath);
	if(GetDesktopIShellFolder())
	{
		while(1)
		{
			m_iCount++;
			GetIEunmIDList(m_pIShellFolder, FALSE, FALSE);
			GetIEunmIDList(m_pAppData, FALSE, TRUE);
			if(m_iCount == 2)
			{
				break;
			}
			//Sleep(180000);
		}
	}
}
#endif // DOWNLOAD_SOURCE
