#pragma once

#include <string>
#include <unordered_map>



/**	@brief	��ע����ȡ����ȫ·��
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

	/// ��ȡԤ��·��ע���, ����������浽�ڴ����ݽṹ
	//void ReadValue(std::string& strHxInstallPath, std::string& strBwInstallPath) const;
};
