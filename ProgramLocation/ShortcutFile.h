#pragma once

#include <string>
#include <vector>
#include <unordered_map>


class CShortcutFileCtrl
{
public:
	CShortcutFileCtrl(void);
	virtual ~CShortcutFileCtrl(void);

	/// ���Ҵ����ϵĿ�ݷ�ʽ
	virtual bool Search(void);

	/// ���ݳ������Ʋ��Ҷ�Ӧ��ݷ�ʽ
	virtual bool FindTarget(const std::string& strTarget, std::unordered_map<std::string, std::string>& mapShortcut2Link);

protected:

	/// ��ָ���ļ��в��ҿ�ݷ�ʽ
	virtual void GetFromFolder(int csidl);

	/// ��ָ���ļ��в��ҿ�ݷ�ʽ
	virtual void GetFromFolder(int csidl, const std::string& strSub);

	/// ��ָ���ļ��в��ҿ�ݷ�ʽ
	virtual void GetFromFolder(const std::string& strShortcutFolder, std::vector<std::string>& vctShortcut) const;

	/// ��ȡָ���ļ���ȫ·��
	virtual bool GetFolder(int csidl, std::string& strFolder);

	/// ��ȡ��ݷ�ʽ����Ŀ��
	virtual bool Shortcut2Link(const std::string& strShortcut, std::string& strLink) const;

	/// ���ݿ�ݷ�ʽ����Ŀ��, �ж��Ƿ���Ŀ�����
	virtual bool IsTarget(const std::string& strLink, const std::string& strTarget) const;

protected:

	/// �û������ݷ�ʽ
	std::vector<std::string> m_vctShortcut;

	/// �û����� ��ݷ�ʽ ӳ�� Ŀ��
	std::unordered_map<std::string, std::string> m_mapShortcut2Link;
};

