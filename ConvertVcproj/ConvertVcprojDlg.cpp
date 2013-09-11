
// ConvertVcprojDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ConvertVcproj.h"
#include "ConvertVcprojDlg.h"
#include "ParseVcxproj.h"
#include "ParseSln.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CConvertVcprojDlg �Ի���




CConvertVcprojDlg::CConvertVcprojDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertVcprojDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConvertVcprojDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConvertVcprojDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_CONVERT, &CConvertVcprojDlg::OnBnClickedButtonConvert)
END_MESSAGE_MAP()


// CConvertVcprojDlg ��Ϣ�������

BOOL CConvertVcprojDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	SetDlgItemText(IDC_EDIT_VCXPROJ, theApp.m_StrVcxprojPath);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CConvertVcprojDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CConvertVcprojDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CConvertVcprojDlg::OnOK()
{
	//CDialog::OnOK();
}

void CConvertVcprojDlg::OnCancel()
{
	//CDialog::OnCancel();
}

void CConvertVcprojDlg::OnClose()
{
	CDialog::OnCancel();
}

void CConvertVcprojDlg::OnBnClickedButtonConvert()
{
	CString strPath;
	GetDlgItemText(IDC_EDIT_VCXPROJ, strPath);
	theApp.m_StrVcxprojPath = strPath;

	BOOL bOk = FALSE;
	strPath = strPath.Trim();
	int nExt = strPath.ReverseFind('.');
	if (nExt != -1)
	{
		CString strExt = strPath.Mid(nExt);
		if (strExt.CompareNoCase(_T(".sln")) == 0)
		{
			bOk = ConvertSln(strPath);
		} 
		else if (strExt.CompareNoCase(_T(".vcxproj")) == 0)
		{
			bOk = ConvertVcxproj(strPath);
		}
	}
	if (bOk)
	{
		AfxMessageBox(_T("succeed"));
	}
	else
	{
		AfxMessageBox(_T("fail"));
	}
}

BOOL CConvertVcprojDlg::ConvertSln(const CString &strPath)
{
	BOOL bOk = FALSE;
	CString strVcxproj;
	CString strPathName;
	int nPos = strPath.ReverseFind('\\');
	if (nPos != -1)
	{
		strPathName = strPath.Left(nPos + 1);
	}

	CParseSln parse;
	CFile file;
	if (file.Open(strPath, CFile::modeRead))
	{
		unsigned int uSize = (unsigned int)file.GetLength();
		char *pBuffer = new char[uSize + 2];
		ZeroMemory(pBuffer, uSize + 2);
#if _MSC_VER >= 1300
		file.Read ((void*)pBuffer, uSize);
#else
		file.ReadHuge ((void*)pBuffer, uSize);
#endif
		if (parse.Parse(pBuffer))
		{
			for (int i = 0; i < parse.GetVcxprojCount(); ++i)
			{
				bOk = TRUE;
				strVcxproj = strPathName; 
				strVcxproj += parse.GetVcxprojByIndex(i);
				if (!ConvertVcxproj(strVcxproj))
				{
					bOk = FALSE;
					break;
				}
			}			
		}
		delete[] pBuffer;
		file.Close();
	}
	if (bOk)
	{
		std::string strOut;
		parse.Save(strOut);
		CString strNewPath = strPath;
		strNewPath.Insert(strPath.ReverseFind('.'), _T("vc2008"));
        if (file.Open(strNewPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
		{
			file.Write(strOut.c_str(), strOut.length());
			file.Close();
		}
		else
		{
			return FALSE;
		}
	}
	return bOk;
}

BOOL CConvertVcprojDlg::ConvertVcxproj(const CString &strPath)
{
	CParseVcxproj parse;
	CFile file;
	if (file.Open(strPath, CFile::modeRead))
	{
		unsigned int uSize = (unsigned int)file.GetLength();
		char *pBuffer = new char[uSize + 2];
		ZeroMemory(pBuffer, uSize + 2);
#if _MSC_VER >= 1300
		file.Read ((void*)pBuffer, uSize);
#else
		file.ReadHuge ((void*)pBuffer, uSize);
#endif
		parse.Parse(pBuffer);
		delete[] pBuffer;
		file.Close();
	}
	else
	{
		return FALSE;
	}
	CString strNewPath = strPath;
	strNewPath.Append(_T(".filters"));
	if (file.Open(strNewPath, CFile::modeRead))
	{
		unsigned int uSize = (unsigned int)file.GetLength();
		char *pBuffer = new char[uSize + 2];
		ZeroMemory(pBuffer, uSize + 2);
#if _MSC_VER >= 1300
		file.Read ((void*)pBuffer, uSize);
#else
		file.ReadHuge ((void*)pBuffer, uSize);
#endif
		parse.ParseFilters(pBuffer);
		delete[] pBuffer;
		file.Close();
	}
	else
	{
		return FALSE;
	}
	std::string strOut;
	parse.Save(strOut);
	strNewPath = strPath;
	int nExt = strPath.ReverseFind('.');
	strNewPath.Delete(nExt, strNewPath.GetLength());
	strNewPath.Append(_T(".vcproj"));
	if (file.Open(strNewPath, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary))
	{
		file.Write(strOut.c_str(), strOut.length());
		file.Close();
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}