
// ProgramLocationDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CProgramLocationDlg �Ի���




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


// CProgramLocationDlg ��Ϣ�������

BOOL CProgramLocationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	GetDlgItem(IDC_EDIT_PROCESS)->SetWindowText("devenv.exe");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CProgramLocationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CProgramLocationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CProgramLocationDlg::OnBnClickedBtnProcessSnapshot()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	/// ͨ�����̿��� ��ȡ��Ʊ�����װ·��
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_lstResult.ResetContent();

	CProgramRegist _regist;
	string strBw, strHx;
	if(_regist.ReadHxInstallPath(strHx))
	{
		TRACE("����:%s\n", strHx.c_str());
		m_lstResult.AddString(strHx.c_str());
	}
	if(_regist.ReadBwInstallPath(strBw))
	{
		TRACE("����:%s\n", strBw.c_str());
		m_lstResult.AddString(strBw.c_str());
	}
}
