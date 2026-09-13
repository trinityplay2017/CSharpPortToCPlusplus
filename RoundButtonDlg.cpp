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

	m_btnRound.SetRadius(18);
	m_btnRound.SetBackgroundColor(RGB(30, 30, 40));
	m_btnRound.SetBorderWidth(5.0f);
	m_btnRound.SetUseMouseOverBackColor(true);
	m_btnRound.SetMouseOverBackColor(RGB(50, 50, 70));
	m_btnRound.SetWindowText(_T("Say Hi"));

	// Rainbow border + animation demo
	m_btnRound.SetRainbowBorder(true);
	m_btnRound.SetRainbowAnimate(true);
	m_btnRound.SetRainbowSpeed(40); // ms per frame

	m_lblStatus.SetWindowText(_T(""));

	return TRUE;
}

void CRoundButtonDlg::OnBnClickedRoundButton()
{
	m_lblStatus.SetWindowText(_T("Hi !!!"));
}
