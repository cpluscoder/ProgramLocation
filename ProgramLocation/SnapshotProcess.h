#pragma once

#include <string>

/**	@brief	�ӽ�����Ϣ��ȡ����ȫ·��
 */
class CSnapshotProcess
{
public:
	CSnapshotProcess(void);
	virtual ~CSnapshotProcess(void);

	bool QueryProcessFullPath(const std::string& strProcessName, std::string& strFullPath);

protected:

	bool GetTargetPID(const std::string& strProcessName, DWORD& dwPID);

	bool GetProcessFullPath(DWORD dwPID, const std::string& strProcessName, std::string& strFullPath);

	bool GetProcessList();
	bool ListProcessModules(DWORD dwPID);
	bool ListProcessThreads(DWORD dwOwnerPID);
	void printError(TCHAR* msg);

};

