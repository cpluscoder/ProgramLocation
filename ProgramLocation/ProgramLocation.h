
// ProgramLocation.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CProgramLocationApp:
// �йش����ʵ�֣������ ProgramLocation.cpp
//

class CProgramLocationApp : public CWinApp
{
public:
	CProgramLocationApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CProgramLocationApp theApp;