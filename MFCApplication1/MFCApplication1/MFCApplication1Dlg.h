
// MFCApplication1Dlg.h : ͷ�ļ�
//

#pragma once
#include "shockwaveflash1.h"
#include "GameSystem.h"

#define WM_FLASH_STOP (WM_USER+100)
#define WM_FLASH_PLAY (WM_USER+101)

// CMFCApplication1Dlg �Ի���
class CMFCApplication1Dlg : public CDialogEx, GameSystem
{
// ����
public:
	CMFCApplication1Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

private:
	void OnInital();

	void OnUpdate(float deltaTime);

	void OnRelease();

	BOOL PreTranslateMessage(MSG* pMsg);

	void SwitchGameStatus(GAME_STATUS eGameStatus);

	void EnterIdle() override;

	void LeaveIdle() override;

	virtual HWND GetWindowHandle() override;


private:
	static DWORD MainThread(LPVOID lParam);

private:
	//GameSystem* m_pGameSystem;
	//GAME_STATUS m_eGameStatus;

	HANDLE m_hThread;
	bool m_bIsMainThreadRun;
	int m_nUpdate;
	int m_nRender;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CShockwaveflash1 m_Shockwaveflash;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};
