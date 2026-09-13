#include "stdafx.h"
#include "RoundButton.h"

IMPLEMENT_DYNAMIC(CRoundButton, CButton)

CRoundButton::CRoundButton()
	: m_nRadius(18)
	, m_crBackground(RGB(11, 205, 255))
	, m_crBorder(RGB(0, 90, 180))
	, m_fBorderWidth(6.0f)
	, m_bUseMouseOver(true)
	, m_crMouseOver(RGB(180, 180, 180))
	, m_crOriginalBackground(m_crBackground)
	, m_bMouseOver(false)
	, m_bTracking(false)
	, m_bPressed(false)
	, m_gdiplusToken(0)
{
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
}

CRoundButton::~CRoundButton()
{
	if (m_gdiplusToken)
		GdiplusShutdown(m_gdiplusToken);
}

void CRoundButton::PreSubclassWindow()
{
	ModifyStyle(0, BS_OWNERDRAW);
	CButton::PreSubclassWindow();
	UpdateRegion();
}

BEGIN_MESSAGE_MAP(CRoundButton, CButton)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// ---------------------------------------------------------------------------
// Region – makes the HWND itself rounded (true rounded edges, not just paint)
// ---------------------------------------------------------------------------
void CRoundButton::UpdateRegion()
{
	if (!GetSafeHwnd())
		return;

	CRect rc;
	GetClientRect(&rc);
	if (rc.Width() <= 0 || rc.Height() <= 0)
		return;

	// CreateRoundRectRgn: right/bottom are exclusive → use Width()+1 / Height()+1
	int diameter = max(0, m_nRadius * 2);
	HRGN hRgn = ::CreateRoundRectRgn(
		0, 0,
		rc.Width() + 1,
		rc.Height() + 1,
		diameter, diameter);

	// System takes ownership of hRgn after a successful SetWindowRgn
	SetWindowRgn(hRgn, TRUE);
}

void CRoundButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	UpdateRegion();
}

// ---------------------------------------------------------------------------
// Property setters
// ---------------------------------------------------------------------------
void CRoundButton::SetRadius(int radius)
{
	m_nRadius = max(0, radius);
	UpdateRegion();
	Invalidate();
}

void CRoundButton::SetBackgroundColor(COLORREF color)
{
	m_crBackground = color;
	m_crOriginalBackground = color;
	Invalidate();
}

void CRoundButton::SetBorderColor(COLORREF color)
{
	m_crBorder = color;
	Invalidate();
}

void CRoundButton::SetBorderWidth(float width)
{
	m_fBorderWidth = max(0.0f, width);
	Invalidate();
}

void CRoundButton::SetUseMouseOverBackColor(bool use)
{
	m_bUseMouseOver = use;
}

void CRoundButton::SetMouseOverBackColor(COLORREF color)
{
	m_crMouseOver = color;
}

void CRoundButton::SetButtonText(LPCTSTR text)
{
	SetWindowText(text);
}

CString CRoundButton::GetButtonText() const
{
	CString text;
	GetWindowText(text);
	return text;
}

// ---------------------------------------------------------------------------
// Messages
// ---------------------------------------------------------------------------
BOOL CRoundButton::OnEraseBkgnd(CDC* pDC)
{
	return TRUE; // we paint everything in DrawItem
}

void CRoundButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
		TrackMouseEvent(&tme);
		m_bTracking = true;
	}

	if (!m_bMouseOver)
	{
		m_bMouseOver = true;
		if (m_bUseMouseOver)
		{
			m_crBackground = m_crMouseOver;
			Invalidate();
		}
	}

	CButton::OnMouseMove(nFlags, point);
}

void CRoundButton::OnMouseLeave()
{
	m_bTracking = false;
	m_bMouseOver = false;
	if (m_bUseMouseOver)
	{
		m_crBackground = m_crOriginalBackground;
		Invalidate();
	}
	CButton::OnMouseLeave();
}

void CRoundButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bPressed = true;
	Invalidate();
	CButton::OnLButtonDown(nFlags, point);
}

void CRoundButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bPressed = false;
	Invalidate();
	CButton::OnLButtonUp(nFlags, point);
}

BOOL CRoundButton::PreTranslateMessage(MSG* pMsg)
{
	return CButton::PreTranslateMessage(pMsg);
}

// ---------------------------------------------------------------------------
// GDI+ path helper
// ---------------------------------------------------------------------------
GraphicsPath* CRoundButton::CreateRoundedRectanglePath(RectF rect, float radius)
{
	GraphicsPath* path = new GraphicsPath();

	if (rect.Width <= 0.0f || rect.Height <= 0.0f)
		return path;

	if (radius <= 0.0f)
	{
		path->AddRectangle(rect);
		return path;
	}

	float diameter = radius * 2.0f;
	float minDim = min(rect.Width, rect.Height);

	if (diameter >= minDim)
	{
		path->AddEllipse(rect);
		return path;
	}

	RectF arc(rect.X, rect.Y, diameter, diameter);
	path->AddArc(arc, 180.0f, 90.0f);
	arc.X = rect.GetRight() - diameter;
	path->AddArc(arc, 270.0f, 90.0f);
	arc.Y = rect.GetBottom() - diameter;
	path->AddArc(arc, 0.0f, 90.0f);
	arc.X = rect.X;
	path->AddArc(arc, 90.0f, 90.0f);
	path->CloseFigure();

	return path;
}

// ---------------------------------------------------------------------------
// Owner-draw: solid border-ring + fill inside the rounded region
// ---------------------------------------------------------------------------
void CRoundButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	HDC hdc = lpDrawItemStruct->hDC;
	CRect rc = lpDrawItemStruct->rcItem;

	Graphics g(hdc);
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	const float bw = max(0.0f, m_fBorderWidth);

	// Full client area (region already clips to rounded shape)
	RectF outer(
		(REAL)rc.left,
		(REAL)rc.top,
		(REAL)rc.Width(),
		(REAL)rc.Height());

	if (outer.Width < 2.0f || outer.Height < 2.0f)
		return;

	// Inner fill area inset by border width
	RectF inner(
		outer.X + bw,
		outer.Y + bw,
		outer.Width - 2.0f * bw,
		outer.Height - 2.0f * bw);

	float outerRadius = (float)m_nRadius;
	float innerRadius = max(0.0f, outerRadius - bw);

	COLORREF bg = m_crBackground;
	if (m_bPressed)
	{
		bg = RGB(GetRValue(bg) * 80 / 100,
		         GetGValue(bg) * 80 / 100,
		         GetBValue(bg) * 80 / 100);
	}

	// 1) Outer fill = border color  (forms the solid border ring)
	{
		SolidBrush borderBrush(Color(255,
			GetRValue(m_crBorder),
			GetGValue(m_crBorder),
			GetBValue(m_crBorder)));
		GraphicsPath* path = CreateRoundedRectanglePath(outer, outerRadius);
		g.FillPath(&borderBrush, path);
		delete path;
	}

	// 2) Inner fill = background color
	if (inner.Width > 0.0f && inner.Height > 0.0f)
	{
		SolidBrush fillBrush(Color(255,
			GetRValue(bg),
			GetGValue(bg),
			GetBValue(bg)));
		GraphicsPath* path = CreateRoundedRectanglePath(inner, innerRadius);
		g.FillPath(&fillBrush, path);
		delete path;
	}

	// 3) Caption
	CString text;
	GetWindowText(text);
	if (!text.IsEmpty())
	{
		Gdiplus::Font font(L"Segoe UI", 12.0f, FontStyleBold, UnitPoint);
		StringFormat sf;
		sf.SetAlignment(StringAlignmentCenter);
		sf.SetLineAlignment(StringAlignmentCenter);
		sf.SetTrimming(StringTrimmingEllipsisCharacter);
		sf.SetFormatFlags(StringFormatFlagsNoWrap);

		int luminance = (GetRValue(bg) * 299 + GetGValue(bg) * 587 + GetBValue(bg) * 114) / 1000;
		Color textColor = (luminance < 128)
			? Color(255, 255, 255, 255)
			: Color(255, 20, 20, 20);

		SolidBrush textBrush(textColor);
		RectF textRect = (inner.Width > 0.0f) ? inner : outer;
		g.DrawString(text, -1, &font, textRect, &sf, &textBrush);
	}
}
