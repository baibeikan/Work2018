
// WaveEditDlg.h : 头文件
//

#pragma once
#include "WaveOut.h"
#include "WaveIn.h"


// CWaveEditDlg 对话框
class CWaveEditDlg : public CDialogEx
{
// 构造
public:
	CWaveEditDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_WAVEEDIT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
public:
	void PushWaveOutStream(char* _data, unsigned int _dataSize);

private:
	WaveOut m_waveOut;
	WaveIn m_waveIn;
	char m_arrPath[2][MAX_PATH];
	int m_nIdx;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
