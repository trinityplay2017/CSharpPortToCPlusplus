#include "stdafx.h"
#include "RoundButton.h"
#include <cmath>

IMPLEMENT_DYNAMIC(CRoundButton, CButton)

CRoundButton::CRoundButton()
	: m_nRadius(18)
	, m_crBackground(RGB(30, 30, 40))
	, m_crBorder(RGB(0, 90, 180))
	, m_fBorderWidth(5.0f)
	, m_bUseMouseOver(true)
	, m_crMouseOver(RGB(50, 50, 70))
	, m_crOriginalBackground(m_crBackground)
	, m_bMouseOver(false)
	, m_bTracking(false)
	, m_bPressed(false)
	, m_gdiplusToken(0)
	, m_bRainbowBorder(false)
	, m_bRainbowAnimate(false)
	, m_nRainbowInterval(30)
	, m_fRainbowHue(0.0f)
	, m_bTimerRunning(false)
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
	if (m_bRainbowBorder && m_bRainbowAnimate)
		StartRainbowTimer();
}

BEGIN_MESSAGE_MAP(CRoundButton, CButton)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CRoundButton::UpdateRegion()
{
	if (!GetSafeHwnd())
		return;

	CRect rc;
	GetClientRect(&rc);
	if (rc.Width() <= 0 || rc.Height() <= 0)
		return;

	int diameter = max(0, m_nRadius * 2);
	HRGN hRgn = ::CreateRoundRectRgn(
		0, 0,
		rc.Width() + 1,
		rc.Height() + 1,
		diameter, diameter);

	SetWindowRgn(hRgn, TRUE);
}

void CRoundButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	UpdateRegion();
}

void CRoundButton::OnDestroy()
{
	StopRainbowTimer();
	CButton::OnDestroy();
}

void CRoundButton::SetRadius(int radius)
{
	m_nRadius = max(0, radius);
	UpdateRegion();
	Invalidate(FALSE);
}

void CRoundButton::SetBackgroundColor(COLORREF color)
{
	m_crBackground = color;
	m_crOriginalBackground = color;
	Invalidate(FALSE);
}

void CRoundButton::SetBorderColor(COLORREF color)
{
	m_crBorder = color;
	Invalidate(FALSE);
}

void CRoundButton::SetBorderWidth(float width)
{
	m_fBorderWidth = max(0.0f, width);
	Invalidate(FALSE);
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

void CRoundButton::SetRainbowBorder(bool enable)
{
	m_bRainbowBorder = enable;
	if (enable && m_bRainbowAnimate)
		StartRainbowTimer();
	else if (!enable)
		StopRainbowTimer();
	Invalidate(FALSE);
}

void CRoundButton::SetRainbowAnimate(bool enable)
{
	m_bRainbowAnimate = enable;
	if (enable && m_bRainbowBorder)
		StartRainbowTimer();
	else
		StopRainbowTimer();
	Invalidate(FALSE);
}

void CRoundButton::SetRainbowSpeed(UINT intervalMs)
{
	m_nRainbowInterval = max(10u, intervalMs);
	if (m_bTimerRunning)
	{
		StopRainbowTimer();
		StartRainbowTimer();
	}
}

void CRoundButton::StartRainbowTimer()
{
	if (!GetSafeHwnd() || m_bTimerRunning)
		return;
	SetTimer(TIMER_RAINBOW, m_nRainbowInterval, NULL);
	m_bTimerRunning = true;
}

void CRoundButton::StopRainbowTimer()
{
	if (!GetSafeHwnd() || !m_bTimerRunning)
		return;
	KillTimer(TIMER_RAINBOW);
	m_bTimerRunning = false;
}

void CRoundButton::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_RAINBOW)
	{
		m_fRainbowHue += 4.0f;
		if (m_fRainbowHue >= 360.0f)
			m_fRainbowHue -= 360.0f;
		Invalidate(FALSE);
	}
	CButton::OnTimer(nIDEvent);
}

BOOL CRoundButton::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CRoundButton::OnPaint()
{
	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);

	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap* pOld = memDC.SelectObject(&bmp);

	PaintContent(memDC.GetSafeHdc(), rc);

	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOld);
}

void CRoundButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rc = lpDrawItemStruct->rcItem;
	HDC hdc = lpDrawItemStruct->hDC;

	HDC hdcMem = ::CreateCompatibleDC(hdc);
	HBITMAP hBmp = ::CreateCompatibleBitmap(hdc, rc.Width(), rc.Height());
	HBITMAP hOld = (HBITMAP)::SelectObject(hdcMem, hBmp);

	PaintContent(hdcMem, rc);

	::BitBlt(hdc, rc.left, rc.top, rc.Width(), rc.Height(), hdcMem, 0, 0, SRCCOPY);

	::SelectObject(hdcMem, hOld);
	::DeleteObject(hBmp);
	::DeleteDC(hdcMem);
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
			Invalidate(FALSE);
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
		Invalidate(FALSE);
	}
	CButton::OnMouseLeave();
}

void CRoundButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bPressed = true;
	Invalidate(FALSE);
	CButton::OnLButtonDown(nFlags, point);
}

void CRoundButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bPressed = false;
	Invalidate(FALSE);
	CButton::OnLButtonUp(nFlags, point);
}

BOOL CRoundButton::PreTranslateMessage(MSG* pMsg)
{
	return CButton::PreTranslateMessage(pMsg);
}

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

static Color ColorFromHSV(float h, float s, float v)
{
	while (h < 0.0f)   h += 360.0f;
	while (h >= 360.0f) h -= 360.0f;

	float c = v * s;
	float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
	float m = v - c;
	float r = 0, g = 0, b = 0;

	if (h < 60)       { r = c; g = x; b = 0; }
	else if (h < 120) { r = x; g = c; b = 0; }
	else if (h < 180) { r = 0; g = c; b = x; }
	else if (h < 240) { r = 0; g = x; b = c; }
	else if (h < 300) { r = x; g = 0; b = c; }
	else              { r = c; g = 0; b = x; }

	return Color(255,
		(BYTE)((r + m) * 255.0f + 0.5f),
		(BYTE)((g + m) * 255.0f + 0.5f),
		(BYTE)((b + m) * 255.0f + 0.5f));
}

void CRoundButton::BuildRainbowColors(Color* colors, int count, float hueOffset) const
{
	for (int i = 0; i < count; ++i)
	{
		float h = hueOffset + (360.0f * i) / (float)count;
		colors[i] = ColorFromHSV(h, 1.0f, 1.0f);
	}
}

void CRoundButton::PaintContent(HDC hdc, const CRect& rc)
{
	Graphics g(hdc);
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetPixelOffsetMode(PixelOffsetModeHalf);
	g.SetCompositingQuality(CompositingQualityHighQuality);
	g.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	SolidBrush clearBrush(Color(255,
		GetRValue(GetSysColor(COLOR_3DFACE)),
		GetGValue(GetSysColor(COLOR_3DFACE)),
		GetBValue(GetSysColor(COLOR_3DFACE))));
	g.FillRectangle(&clearBrush, rc.left, rc.top, rc.Width(), rc.Height());

	const float bw = max(1.0f, m_fBorderWidth);
	const float halfBw = bw * 0.5f;

	RectF pathRect(
		(REAL)rc.left + halfBw,
		(REAL)rc.top + halfBw,
		(REAL)rc.Width() - bw,
		(REAL)rc.Height() - bw);

	if (pathRect.Width < 2.0f || pathRect.Height < 2.0f)
		return;

	float pathRadius = max(0.0f, (float)m_nRadius - halfBw);

	COLORREF bg = m_crBackground;
	if (m_bPressed)
	{
		bg = RGB(GetRValue(bg) * 80 / 100,
		         GetGValue(bg) * 80 / 100,
		         GetBValue(bg) * 80 / 100);
	}

	GraphicsPath* path = CreateRoundedRectanglePath(pathRect, pathRadius);

	{
		SolidBrush fillBrush(Color(255,
			GetRValue(bg), GetGValue(bg), GetBValue(bg)));
		g.FillPath(&fillBrush, path);
	}

	if (bw > 0.0f)
	{
		if (m_bRainbowBorder)
		{
			const int kStops = 8;
			Color colors[kStops];
			BuildRainbowColors(colors, kStops, m_fRainbowHue);

			REAL positions[kStops];
			for (int i = 0; i < kStops; ++i)
				positions[i] = (REAL)i / (REAL)(kStops - 1);

			RectF gradRect = pathRect;
			gradRect.Inflate(bw, bw);

			LinearGradientBrush lgb(
				gradRect,
				colors[0], colors[kStops - 1],
				m_fRainbowHue);

			lgb.SetInterpolationColors(colors, positions, kStops);
			lgb.SetWrapMode(WrapModeTileFlipX);

			Pen pen(&lgb, bw);
			pen.SetLineJoin(LineJoinRound);
			pen.SetAlignment(PenAlignmentCenter);
			g.DrawPath(&pen, path);
		}
		else
		{
			Pen pen(Color(255,
				GetRValue(m_crBorder),
				GetGValue(m_crBorder),
				GetBValue(m_crBorder)), bw);
			pen.SetLineJoin(LineJoinRound);
			pen.SetAlignment(PenAlignmentCenter);
			g.DrawPath(&pen, path);
		}
	}

	delete path;

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
		RectF textRect = pathRect;
		textRect.Inflate(-bw, -bw);
		g.DrawString(text, -1, &font, textRect, &sf, &textBrush);
	}
}
