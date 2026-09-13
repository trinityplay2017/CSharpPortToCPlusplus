#include "stdafx.h"
#include "RoundButtonApp.h"
#include "RoundButtonDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CRoundButtonDlg, CDialogEx)

CRoundButtonDlg::CRoundButtonDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ROUNDBUTTON_DIALOG, pParent)
{
}

CRoundButtonDlg::~CRoundButtonDlg()
{
}

void CRoundButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ROUNDBUTTON, m_btnRound);
	DDX_Control(pDX, IDC_STATUS, m_lblStatus);
}

BEGIN_MESSAGE_MAP(CRoundButtonDlg, CDialogEx)
	ON_BN_CLICKED(IDC_ROUNDBUTTON, &CRoundButtonDlg::OnBnClickedRoundButton)
END_MESSAGE_MAP()

BOOL CRoundButtonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Configure the round button to match original C# demo
	m_btnRound.SetRadius(20);
	m_btnRound.SetBackgroundColor(RGB(11, 205, 255));
	m_btnRound.SetBorderColor(RGB(255, 255, 255));
	m_btnRound.SetBorderWidth(7.0f);
	m_btnRound.SetUseMouseOverBackColor(true);
	m_btnRound.SetMouseOverBackColor(RGB(180, 180, 180));
	m_btnRound.SetWindowText(_T("Say Hi"));

	m_lblStatus.SetWindowText(_T(""));

	return TRUE;
}

void CRoundButtonDlg::OnBnClickedRoundButton()
{
	m_lblStatus.SetWindowText(_T("Hi !!!"));
}
