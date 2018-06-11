#pragma once

#include <string>
#include <unordered_map>



/**	@brief	从注册表获取进程全路径
 */
class CProgramRegist
{
public:
	CProgramRegist(void);
	virtual ~CProgramRegist(void);

	bool ReadHxInstallPath(std::string& strHxInstallPath) const;

	bool ReadBwInstallPath(std::string& strBwInstallPath) const;

protected:
	bool ReadValue(HKEY hRoot, const std::string& strSubKey, const std::string& strValueName, std::string& strValue) const;
	bool QueryValue(HKEY hKey, const std::string& strValueName, std::string& strValue) const;

	/// 读取预定路径注册表, 并将结果保存到内存数据结构
	//void ReadValue(std::string& strHxInstallPath, std::string& strBwInstallPath) const;
};
