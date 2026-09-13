#pragma once

#include "RoundButton.h"

// CRoundButtonDlg dialog

class CRoundButtonDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CRoundButtonDlg)

public:
	CRoundButtonDlg(CWnd* pParent = nullptr);
	virtual ~CRoundButtonDlg();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ROUNDBUTTON_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedRoundButton();

	DECLARE_MESSAGE_MAP()

private:
	CRoundButton m_btnRound;
	CStatic      m_lblStatus;
};
