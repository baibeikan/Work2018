
// WaveEditDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WaveEdit.h"
#include "WaveEditDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CWaveEditDlg 对话框




CWaveEditDlg::CWaveEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CWaveEditDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWaveEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void CWaveEditDlg::PushWaveOutStream(char* _data, unsigned int _dataSize)
{
	m_waveOut.PushStream(_data, _dataSize);
}

BEGIN_MESSAGE_MAP(CWaveEditDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_PLAY, &CWaveEditDlg::OnBnClickedButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_NEXT, &CWaveEditDlg::OnBnClickedButtonNext)
	ON_BN_CLICKED(IDC_BUTTON_RECORD, &CWaveEditDlg::OnBnClickedButtonRecord)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CWaveEditDlg 消息处理程序

BOOL CWaveEditDlg::OnInitDialog()
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
	m_nIdx = 0;
	strcpy(m_arrPath[0], "11.wav");
	strcpy(m_arrPath[1], "22.wav");

	m_waveOut.Play(m_waveIn.GetWaveFormat());
	//m_waveOut.Play("22.wav");

	m_waveIn.m_pOwner = this;
	m_waveIn.StartRecord(0);

	SetTimer(0, 35, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWaveEditDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CWaveEditDlg::OnPaint()
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
HCURSOR CWaveEditDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CWaveEditDlg::OnBnClickedButtonPlay()
{
	// TODO: 在此添加控件通知处理程序代码
	m_waveOut.Play("11.wav");
}


void CWaveEditDlg::OnBnClickedButtonNext()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nIdx = (m_nIdx +1)%2;
	m_waveOut.Play(m_arrPath[m_nIdx]);
}


void CWaveEditDlg::OnBnClickedButtonRecord()
{
	// TODO: 在此添加控件通知处理程序代码
	m_waveIn.StartRecord(0);
}


void CWaveEditDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0)
	{
		m_waveOut.Update();
	}

	CDialogEx::OnTimer(nIDEvent);
}
