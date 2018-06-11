#pragma once

#include <string>

/**	@brief	�ӽ�����Ϣ��ȡ����ȫ·��
 */
class CSnapshotProcess
{
public:
	CSnapshotProcess(void);
	virtual ~CSnapshotProcess(void);

	/// ���ݽ������Ʋ��ҽ��̿���, ��ȡ��������ȫ·��
	bool QueryProcessFullPath(const std::string& strProcessName, std::string& strFullPath);

protected:

	/// ���ݽ������Ʋ��ҽ��̿���, ��ȡ����ID
	bool GetTargetPID(const std::string& strProcessName, DWORD& dwPID);

	/// ���ݽ���ID, ��ȡ��������ȫ·��
	bool GetProcessFullPath(DWORD dwPID, const std::string& strProcessName, std::string& strFullPath);

#ifdef USE_MSDN_SRC
	bool GetProcessList();
	bool ListProcessModules(DWORD dwPID);
	bool ListProcessThreads(DWORD dwOwnerPID);
	void printError(TCHAR* msg);
#endif
};

