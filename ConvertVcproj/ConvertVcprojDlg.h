
// ConvertVcprojDlg.h : 头文件
//

#pragma once


// CConvertVcprojDlg 对话框
class CConvertVcprojDlg : public CDialog
{
// 构造
public:
	CConvertVcprojDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CONVERTVCPROJ_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	BOOL ConvertSln(const CString &strPath);
	BOOL ConvertVcxproj(const CString &strPath);

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonConvert();
};
