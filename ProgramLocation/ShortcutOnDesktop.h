#pragma once

#include <string>
#include <vector>
#include <unordered_map>

/**	@brief	��ȡ�����ݷ�ʽ�ļ�
 */
class CShortcutOnDesktop
{
public:
	CShortcutOnDesktop(void);
	virtual ~CShortcutOnDesktop(void);
	
	/// ��ȡ�����ݷ�ʽ
	void GetDesktopShortcut(void);

	/// ��ȡ��ݷ�ʽĿ��
	void GetShortcutLink(void);

	void SearchShortcut(const std::string& strTarget, std::unordered_map<std::string, std::string>& mapSearchResult);

#ifdef DOWNLOAD_SOURCE
	void Test(void);
#endif // DOWNLOAD_SOURCE

protected:

	/// ��ȡָ���ļ����еĿ�ݷ�ʽ
	bool GetShortcut(const std::string& strDesktopFolder, std::vector<std::string>& vctShortcut);

	/// ��ȡ��ǰ�û������ļ���·��
	bool GetCurrentUserDesktopFolder(std::string& strUserDesktop);

	/// ��ȡ�����û������ļ���·��
	bool GetAllUserDesktopFolder(std::string& strAllUsersDesktop);

protected:
	std::vector<std::string> m_vctShortcut;
	std::unordered_map<std::string, std::string> m_mapShortcut2Link;


#ifdef DOWNLOAD_SOURCE
	//��ȡ��ݷ�ʽ����Ϣ
	BOOL ReadShortCut(LPTSTR linkName);

	//��ȡָ��Ŀ¼��IShellFolder�ӿ�
	IShellFolder * CShortcutOnDesktop::GetIShellFolderByPath(LPTSTR path);

	//��ȡ�����ļ��кͿ��������ļ��е�·��
	int CShortcutOnDesktop::GetPath(char * DeskTop, char * AppData);

	//���������ļ��е�һ�����������
	BOOL CShortcutOnDesktop::EnumAllItems(IEnumIDList *m_pEnum, BOOL isFolder, BOOL isQuickLanch);

	//��ȡ�����ļ��е�IEnumIDList�ӿ�
	BOOL CShortcutOnDesktop::GetIEunmIDList(IShellFolder * m_pShellFolder, BOOL isRecur, BOOL isQuickLanch);

	//��ȡ�����ļ��е�IShellFolder�ӿ�ָ��
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
