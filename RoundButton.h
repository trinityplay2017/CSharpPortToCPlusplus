#pragma once

#include <afxwin.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// CRoundButton - MFC port of the C# RoundButton control
// True rounded window shape via SetWindowRgn + GDI+ anti-aliased fill/border.

class CRoundButton : public CButton
{
	DECLARE_DYNAMIC(CRoundButton)

public:
	CRoundButton();
	virtual ~CRoundButton();

	// Properties
	void SetRadius(int radius);
	int  GetRadius() const { return m_nRadius; }

	void SetBackgroundColor(COLORREF color);
	COLORREF GetBackgroundColor() const { return m_crBackground; }

	void SetBorderColor(COLORREF color);
	COLORREF GetBorderColor() const { return m_crBorder; }

	void SetBorderWidth(float width);
	float GetBorderWidth() const { return m_fBorderWidth; }

	void SetUseMouseOverBackColor(bool use);
	bool GetUseMouseOverBackColor() const { return m_bUseMouseOver; }

	void SetMouseOverBackColor(COLORREF color);
	COLORREF GetMouseOverBackColor() const { return m_crMouseOver; }

	void SetButtonText(LPCTSTR text);
	CString GetButtonText() const;

protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP()

private:
	void UpdateRegion();   // apply CreateRoundRectRgn → SetWindowRgn
	GraphicsPath* CreateRoundedRectanglePath(RectF rect, float radius);

	int         m_nRadius;
	COLORREF    m_crBackground;
	COLORREF    m_crBorder;
	float       m_fBorderWidth;
	bool        m_bUseMouseOver;
	COLORREF    m_crMouseOver;
	COLORREF    m_crOriginalBackground;
	bool        m_bMouseOver;
	bool        m_bTracking;
	bool        m_bPressed;
	ULONG_PTR   m_gdiplusToken;
};
