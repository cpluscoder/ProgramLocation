#pragma once

#include <string>

/**	@brief	从进程信息获取进程全路径
 */
class CSnapshotProcess
{
public:
	CSnapshotProcess(void);
	virtual ~CSnapshotProcess(void);

	/// 根据进程名称查找进程快照, 获取进程所在全路径
	bool QueryProcessFullPath(const std::string& strProcessName, std::string& strFullPath);

protected:

	/// 根据进程名称查找进程快照, 获取进程ID
	bool GetTargetPID(const std::string& strProcessName, DWORD& dwPID);

	/// 根据进程ID, 获取进程所在全路径
	bool GetProcessFullPath(DWORD dwPID, const std::string& strProcessName, std::string& strFullPath);

#ifdef USE_MSDN_SRC
	bool GetProcessList();
	bool ListProcessModules(DWORD dwPID);
	bool ListProcessThreads(DWORD dwOwnerPID);
	void printError(TCHAR* msg);
#endif
};

