
// ProgramLocationDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ProgramLocation.h"
#include "ProgramLocationDlg.h"
#include "afxdialogex.h"
#include "SnapshotProcess.h"
#include "ShortcutFile.h"
#include "ProgramRegist.h"
#include <algorithm>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CProgramLocationDlg 对话框




CProgramLocationDlg::CProgramLocationDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgramLocationDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProgramLocationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RESULT, m_lstResult);
}

BEGIN_MESSAGE_MAP(CProgramLocationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_PROCESS_SNAPSHOT, &CProgramLocationDlg::OnBnClickedBtnProcessSnapshot)
	ON_BN_CLICKED(IDC_BTN_SHORTCUT, &CProgramLocationDlg::OnBnClickedBtnShortcut)
	ON_BN_CLICKED(IDC_BTN_REG, &CProgramLocationDlg::OnBnClickedBtnReg)
END_MESSAGE_MAP()


// CProgramLocationDlg 消息处理程序

BOOL CProgramLocationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_EDIT_PROCESS)->SetWindowText("devenv.exe");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CProgramLocationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CProgramLocationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CProgramLocationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CProgramLocationDlg::OnBnClickedBtnProcessSnapshot()
{
	// TODO: 在此添加控件通知处理程序代码

	/// 通过进程快照 获取开票软件安装路径
	CString strTarget;
	GetDlgItem(IDC_EDIT_PROCESS)->GetWindowText(strTarget);

	m_lstResult.ResetContent();
	if(strTarget.GetLength() > 0)
	{
		CSnapshotProcess snapshotProc;
		string strInstallPath;
		if(snapshotProc.QueryProcessFullPath(string(strTarget), strInstallPath))
		{
			m_lstResult.AddString(strInstallPath.c_str());
		}
	}
}

void CProgramLocationDlg::OnBnClickedBtnShortcut()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strTarget;
	GetDlgItem(IDC_EDIT_PROCESS)->GetWindowText(strTarget);

	m_lstResult.ResetContent();
	if(strTarget.GetLength() > 0)
	{
		CShortcutFileCtrl shortcutOnStartMenu;
		if(shortcutOnStartMenu.Search())
		{
			unordered_map<string, string> mapShortcut2Link;
			if(shortcutOnStartMenu.FindTarget(string(strTarget), mapShortcut2Link))
			{
				for_each(mapShortcut2Link.begin(), mapShortcut2Link.end(), [&](const unordered_map<string, string>::value_type key2value) {
					m_lstResult.AddString(key2value.first.c_str());
					string strText = "-->";
					strText.append(key2value.second);
					m_lstResult.AddString(strText.c_str());
				});
			}
		}
	}
}


void CProgramLocationDlg::OnBnClickedBtnReg()
{
	// TODO: 在此添加控件通知处理程序代码
	m_lstResult.ResetContent();

	CProgramRegist _regist;
	string strBw, strHx;
	if(_regist.ReadHxInstallPath(strHx))
	{
		TRACE("航信:%s\n", strHx.c_str());
		m_lstResult.AddString(strHx.c_str());
	}
	if(_regist.ReadBwInstallPath(strBw))
	{
		TRACE("百旺:%s\n", strBw.c_str());
		m_lstResult.AddString(strBw.c_str());
	}
}
