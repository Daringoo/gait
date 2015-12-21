
// MFCApplication1Dlg.h : 头文件
//

#pragma once
#include "MyGlobal.h"

// CMFCApplication1Dlg 对话框
class CMFCApplication1Dlg : public CDialogEx
{
// 构造
public:
	CMFCApplication1Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	MyGlobal m_glb;
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
protected:
	 CEdit m_studyGaitName;
	 CButton m_btShowVideo;
public:
	CEdit m_editVideoPath;
	CEdit m_editRecogName;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedOpenvideobutton();
	afx_msg void OnEnChangeVideopathloaded();
};
