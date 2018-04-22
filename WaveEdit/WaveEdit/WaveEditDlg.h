
// WaveEditDlg.h : ͷ�ļ�
//

#pragma once
#include "WaveOut.h"
#include "WaveIn.h"


// CWaveEditDlg �Ի���
class CWaveEditDlg : public CDialogEx
{
// ����
public:
	CWaveEditDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_WAVEEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
public:
	void PushWaveOutStream(char* _data, unsigned int _dataSize);

private:
	WaveOut m_waveOut;
	WaveIn m_waveIn;
	char m_arrPath[2][MAX_PATH];
	int m_nIdx;

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonRecord();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
