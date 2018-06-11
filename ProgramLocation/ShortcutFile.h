#pragma once

#include <string>
#include <vector>
#include <unordered_map>


class CShortcutFileCtrl
{
public:
	CShortcutFileCtrl(void);
	virtual ~CShortcutFileCtrl(void);

	/// 查找磁盘上的快捷方式
	virtual bool Search(void);

	/// 根据程序名称查找对应快捷方式
	virtual bool FindTarget(const std::string& strTarget, std::unordered_map<std::string, std::string>& mapShortcut2Link);

protected:

	/// 在指定文件夹查找快捷方式
	virtual void GetFromFolder(int csidl);

	/// 在指定文件夹查找快捷方式
	virtual void GetFromFolder(int csidl, const std::string& strSub);

	/// 在指定文件夹查找快捷方式
	virtual void GetFromFolder(const std::string& strShortcutFolder, std::vector<std::string>& vctShortcut) const;

	/// 获取指定文件夹全路径
	virtual bool GetFolder(int csidl, std::string& strFolder);

	/// 获取快捷方式链接目标
	virtual bool Shortcut2Link(const std::string& strShortcut, std::string& strLink) const;

	/// 根据快捷方式链接目标, 判断是否是目标进程
	virtual bool IsTarget(const std::string& strLink, const std::string& strTarget) const;

protected:

	/// 用户桌面快捷方式
	std::vector<std::string> m_vctShortcut;

	/// 用户桌面 快捷方式 映射 目标
	std::unordered_map<std::string, std::string> m_mapShortcut2Link;
};

