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

	m_btnRound.SetRadius(4);
	m_btnRound.SetBorderWidth(1.0f);
	m_btnRound.SetWindowText(_T("Say Hi"));

	// Per-state background colors
	m_btnRound.SetStateBackgroundColor(RBS_Normal,   RGB(30, 30, 40));
	m_btnRound.SetStateBackgroundColor(RBS_Hover,    RGB(55, 255, 80));
	m_btnRound.SetStateBackgroundColor(RBS_Pressed,  RGB(255, 15, 22));
	m_btnRound.SetStateBackgroundColor(RBS_Disabled, RGB(70, 70, 255));

	// Per-state border colors (used when rainbow is off)
	m_btnRound.SetStateBorderColor(RBS_Normal,   RGB(255, 120, 220));
	m_btnRound.SetStateBorderColor(RBS_Hover,    RGB(100, 0, 255));
	m_btnRound.SetStateBorderColor(RBS_Pressed,  RGB(0, 255, 0));
	m_btnRound.SetStateBorderColor(RBS_Disabled, RGB(110, 110, 110));

	// Smooth color transitions between states
	m_btnRound.SetColorLerp(true);
	m_btnRound.SetColorLerpDuration(270);   // ms for full transition
	m_btnRound.SetColorLerpEase(RBE_EaseOut);

	// Rainbow border + animation
	m_btnRound.SetRainbowBorder(false);
	m_btnRound.SetRainbowAnimate(true);
	m_btnRound.SetRainbowSpeed(16);         // timer interval ms

	m_lblStatus.SetWindowText(_T(""));

	return TRUE;
}

void CRoundButtonDlg::OnBnClickedRoundButton()
{
	m_lblStatus.SetWindowText(_T("Hi !!!"));
}
