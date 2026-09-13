#include "stdafx.h"
#include "RoundButtonApp.h"
#include "RoundButtonDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRoundButtonDlg::CRoundButtonDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_ROUNDBUTTON_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_ROUNDBUTTON, &CRoundButtonDlg::OnBnClickedRoundButton)
END_MESSAGE_MAP()

BOOL CRoundButtonDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // Configure the round button to match original demo
    m_btnRound.SetRadius(20);
    m_btnRound.SetBackgroundColor(RGB(11, 205, 255));
    m_btnRound.SetBorderColor(RGB(255, 255, 255));
    m_btnRound.SetBorderWidth(7.0f);
    m_btnRound.SetUseMouseOverBackColor(true);
    m_btnRound.SetMouseOverBackColor(RGB(180, 180, 180));
    m_btnRound.SetButtonText(_T("Say Hi"));
    m_btnRound.SetWindowText(_T("Say Hi"));

    m_lblStatus.SetWindowText(_T(""));

    return TRUE;
}

void CRoundButtonDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this);
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

HCURSOR CRoundButtonDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CRoundButtonDlg::OnBnClickedRoundButton()
{
    m_lblStatus.SetWindowText(_T("Hi !!!"));
}
