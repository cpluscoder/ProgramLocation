#include "StdAfx.h"
#include "ShortcutFile.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>


using namespace std;

#define HX_SUBFOLDER_NAME	"开票软件"
#define BW_SUBFOLDER_NAME	"税控发票开票软件(税控盘版)"

CShortcutFileCtrl::CShortcutFileCtrl(void)
{
	m_vctShortcut.clear();
	m_mapShortcut2Link.clear();
}


CShortcutFileCtrl::~CShortcutFileCtrl(void)
{
	m_vctShortcut.clear();
	m_mapShortcut2Link.clear();
}

bool CShortcutFileCtrl::Search(void)
{
	m_vctShortcut.clear();

	/// 获取当前用户桌面文件夹路径
	GetFromFolder(CSIDL_DESKTOPDIRECTORY);

	/// 获取公共用户桌面文件夹路径
	GetFromFolder(CSIDL_COMMON_DESKTOPDIRECTORY);

	/// 获取当前用户开始菜单文件夹路径
	GetFromFolder(CSIDL_PROGRAMS, HX_SUBFOLDER_NAME);
	GetFromFolder(CSIDL_PROGRAMS, BW_SUBFOLDER_NAME);

	/// 获取公共用户开始菜单文件夹路径
	GetFromFolder(CSIDL_COMMON_PROGRAMS, HX_SUBFOLDER_NAME);
	GetFromFolder(CSIDL_COMMON_PROGRAMS, BW_SUBFOLDER_NAME);

	m_mapShortcut2Link.clear();
	for_each(m_vctShortcut.begin(), m_vctShortcut.end(), [&](const string& strShortcut){
		string strLink;
		if(Shortcut2Link(strShortcut, strLink)) {
			m_mapShortcut2Link[strShortcut] = strLink;
		}
	});

	return !(m_mapShortcut2Link.empty());
}

bool CShortcutFileCtrl::GetFolder(int csidl, string& strFolder)
{
	strFolder.clear();
	char szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	if(::SHGetSpecialFolderPath(NULL, szPath, csidl, FALSE))
	{
		strFolder = szPath;
		return !(strFolder.empty());
	}
	return false;
}

void CShortcutFileCtrl::GetFromFolder(int csidl, const string& strSub)
{
	string strFolder;
	if(GetFolder(csidl, strFolder))
	{
		if(!strSub.empty())
		{
			strFolder.append("\\");
			strFolder.append(strSub);
		}
		GetFromFolder(strFolder, m_vctShortcut);
	}
}

void CShortcutFileCtrl::GetFromFolder(int csidl)
{
	string strFolder;
	if(GetFolder(csidl, strFolder))
	{
		GetFromFolder(strFolder, m_vctShortcut);
	}
}

void CShortcutFileCtrl::GetFromFolder(const string& strShortcutFolder, vector<string>& vctShortcut) const
{
	const string cstrExt = ".LNK";
	LPWSTR pwszPath = NULL;
	{USES_CONVERSION; pwszPath = A2W(strShortcutFolder.c_str());}
	string _strShortcutFolder = strShortcutFolder;
	_strShortcutFolder.append("\\");

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
											string strFile = _strShortcutFolder + findData.cFileName;
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
}

bool CShortcutFileCtrl::Shortcut2Link(const string& strShortcut, string& strLink) const
{
	strLink.clear();
	char szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	
	::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED/*COINIT_MULTITHREADED*/);/// add for WinXP
	LPCOLESTR pwszFileName;
	{USES_CONVERSION; pwszFileName = T2COLE(strShortcut.c_str());}
	IShellLink* pIShellLink = NULL;
	if(SUCCEEDED(::CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pIShellLink)))
	{
		IPersistFile* pIPersistFile = NULL;
		if(SUCCEEDED(pIShellLink->QueryInterface(IID_IPersistFile, (void **)&pIPersistFile)))
		{
			if(SUCCEEDED(pIPersistFile->Load(pwszFileName, STGM_READWRITE)))
			{
				if(SUCCEEDED(pIShellLink->GetPath(szPath, sizeof(szPath), NULL, SLGP_UNCPRIORITY)))
				{
					strLink = szPath;
				}
			}
			pIPersistFile->Release();
		}
		pIShellLink->Release();
	}
	::CoUninitialize();/// add for WinXP

	return !(strLink.empty());
}

bool CShortcutFileCtrl::IsTarget(const string& strLink, const string& strTarget) const
{
	auto iterRange = boost::algorithm::ifind_first(strLink, strTarget);
	return !(iterRange.empty());
}

bool CShortcutFileCtrl::FindTarget(const string& strTarget, unordered_map<string, string>& mapShortcut2Link)
{
	mapShortcut2Link.clear();
	for_each(m_mapShortcut2Link.begin(), m_mapShortcut2Link.end(), [&](unordered_map<string, string>::value_type key2value) {
		if(IsTarget(key2value.second, strTarget)) {
			mapShortcut2Link.insert(key2value);
		}
	});
	return !(mapShortcut2Link.empty());
}
