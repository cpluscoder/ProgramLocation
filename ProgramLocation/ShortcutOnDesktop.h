#pragma once

#include <string>
#include <vector>
#include <unordered_map>

/**	@brief	获取桌面快捷方式文件
 */
class CShortcutOnDesktop
{
public:
	CShortcutOnDesktop(void);
	virtual ~CShortcutOnDesktop(void);
	
	/// 获取桌面快捷方式
	void GetDesktopShortcut(void);

	/// 获取快捷方式目标
	void GetShortcutLink(void);

	void SearchShortcut(const std::string& strTarget, std::unordered_map<std::string, std::string>& mapSearchResult);

#ifdef DOWNLOAD_SOURCE
	void Test(void);
#endif // DOWNLOAD_SOURCE

protected:

	/// 获取指定文件夹中的快捷方式
	bool GetShortcut(const std::string& strDesktopFolder, std::vector<std::string>& vctShortcut);

	/// 获取当前用户桌面文件夹路径
	bool GetCurrentUserDesktopFolder(std::string& strUserDesktop);

	/// 获取所有用户桌面文件夹路径
	bool GetAllUserDesktopFolder(std::string& strAllUsersDesktop);

protected:
	std::vector<std::string> m_vctShortcut;
	std::unordered_map<std::string, std::string> m_mapShortcut2Link;


#ifdef DOWNLOAD_SOURCE
	//读取快捷方式的信息
	BOOL ReadShortCut(LPTSTR linkName);

	//获取指定目录的IShellFolder接口
	IShellFolder * CShortcutOnDesktop::GetIShellFolderByPath(LPTSTR path);

	//获取桌面文件夹和快速启动文件夹的路径
	int CShortcutOnDesktop::GetPath(char * DeskTop, char * AppData);

	//遍历桌面文件夹第一层里的所有项
	BOOL CShortcutOnDesktop::EnumAllItems(IEnumIDList *m_pEnum, BOOL isFolder, BOOL isQuickLanch);

	//获取桌面文件夹的IEnumIDList接口
	BOOL CShortcutOnDesktop::GetIEunmIDList(IShellFolder * m_pShellFolder, BOOL isRecur, BOOL isQuickLanch);

	//获取桌面文件夹的IShellFolder接口指针
	BOOL CShortcutOnDesktop::GetDesktopIShellFolder(void);


protected:
	IShellFolder * m_pIShellFolder;
	IEnumIDList * m_pIEnumFolder;
	IShellFolder * m_pAppData;
	IEnumIDList * m_pIEnumFile;
	IEnumIDList * m_pFirstLayerFolder;
	IEnumIDList * m_pFirstLayerFile;

	char m_strDeskTopPath[MAX_PATH];
	char m_strQuickLanchPath[MAX_PATH];
	char m_strParentPath[MAX_PATH];
	CString Brower[8];
	int m_iCount;
#endif // DOWNLOAD_SOURCE

};

#ifdef DOWNLOAD_SOURCE
void Test(void);
void Test01(void);
#endif // DOWNLOAD_SOURCE
