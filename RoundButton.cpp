#include "stdafx.h"
#include "RoundButton.h"
#include <cmath>

IMPLEMENT_DYNAMIC(CRoundButton, CButton)

CRoundButton::CRoundButton()
	: m_nRadius(18)
	, m_fBorderWidth(5.0f)
	, m_bUseHoverColors(true)
	, m_bMouseOver(false)
	, m_bTracking(false)
	, m_bPressed(false)
	, m_gdiplusToken(0)
	, m_bRainbowBorder(false)
	, m_bRainbowAnimate(false)
	, m_fRainbowHue(0.0f)
	, m_bColorLerp(true)
	, m_nLerpDurationMs(150)
	, m_eLerpEase(RBE_EaseOut)
	, m_fCurBgR(30), m_fCurBgG(30), m_fCurBgB(40)
	, m_fCurBrR(0), m_fCurBrG(90), m_fCurBrB(180)
	, m_bLerpInited(false)
	, m_nAnimInterval(16)
	, m_bTimerRunning(false)
	, m_dwLastTick(0)
{
	m_crBg[RBS_Normal]   = RGB(30, 30, 40);
	m_crBg[RBS_Hover]    = RGB(50, 50, 70);
	m_crBg[RBS_Pressed]  = RGB(20, 20, 28);
	m_crBg[RBS_Disabled] = RGB(80, 80, 85);

	m_crBorder[RBS_Normal]   = RGB(0, 90, 180);
	m_crBorder[RBS_Hover]    = RGB(40, 140, 220);
	m_crBorder[RBS_Pressed]  = RGB(0, 60, 120);
	m_crBorder[RBS_Disabled] = RGB(120, 120, 120);

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
	CaptureCurrentAsFloats();
	m_bLerpInited = true;
	EnsureAnimTimer();
}

BEGIN_MESSAGE_MAP(CRoundButton, CButton)
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CRoundButton::UpdateRegion()
{
	if (!GetSafeHwnd()) return;
	CRect rc;
	GetClientRect(&rc);
	if (rc.Width() <= 0 || rc.Height() <= 0) return;
	int diameter = max(0, m_nRadius * 2);
	HRGN hRgn = ::CreateRoundRectRgn(0, 0, rc.Width() + 1, rc.Height() + 1, diameter, diameter);
	SetWindowRgn(hRgn, TRUE);
}

void CRoundButton::OnSize(UINT nType, int cx, int cy)
{
	CButton::OnSize(nType, cx, cy);
	UpdateRegion();
}

void CRoundButton::OnDestroy()
{
	StopAnimTimer();
	CButton::OnDestroy();
}

void CRoundButton::SetRadius(int radius)
{
	m_nRadius = max(0, radius);
	UpdateRegion();
	Invalidate(FALSE);
}

void CRoundButton::SetBorderWidth(float width)
{
	m_fBorderWidth = max(0.0f, width);
	Invalidate(FALSE);
}

void CRoundButton::SetStateBackgroundColor(RoundButtonState state, COLORREF color)
{
	if (state < 0 || state >= RBS_Count) return;
	m_crBg[state] = color;
	if (state == RBS_Normal && !m_bLerpInited)
		ColorToFloats(color, m_fCurBgR, m_fCurBgG, m_fCurBgB);
	Invalidate(FALSE);
}

COLORREF CRoundButton::GetStateBackgroundColor(RoundButtonState state) const
{
	if (state < 0 || state >= RBS_Count) return m_crBg[RBS_Normal];
	return m_crBg[state];
}

void CRoundButton::SetStateBorderColor(RoundButtonState state, COLORREF color)
{
	if (state < 0 || state >= RBS_Count) return;
	m_crBorder[state] = color;
	if (state == RBS_Normal && !m_bLerpInited)
		ColorToFloats(color, m_fCurBrR, m_fCurBrG, m_fCurBrB);
	Invalidate(FALSE);
}

COLORREF CRoundButton::GetStateBorderColor(RoundButtonState state) const
{
	if (state < 0 || state >= RBS_Count) return m_crBorder[RBS_Normal];
	return m_crBorder[state];
}

void CRoundButton::SetBackgroundColor(COLORREF color)
{
	m_crBg[RBS_Normal] = color;
	m_crBg[RBS_Hover] = RGB(min(255, GetRValue(color) + 25), min(255, GetGValue(color) + 25), min(255, GetBValue(color) + 25));
	m_crBg[RBS_Pressed] = RGB(GetRValue(color) * 80 / 100, GetGValue(color) * 80 / 100, GetBValue(color) * 80 / 100);
	if (!m_bLerpInited)
		ColorToFloats(color, m_fCurBgR, m_fCurBgG, m_fCurBgB);
	Invalidate(FALSE);
}

COLORREF CRoundButton::GetBackgroundColor() const { return m_crBg[RBS_Normal]; }

void CRoundButton::SetBorderColor(COLORREF color)
{
	m_crBorder[RBS_Normal] = color;
	m_crBorder[RBS_Hover] = RGB(min(255, GetRValue(color) + 40), min(255, GetGValue(color) + 40), min(255, GetBValue(color) + 40));
	m_crBorder[RBS_Pressed] = RGB(GetRValue(color) * 70 / 100, GetGValue(color) * 70 / 100, GetBValue(color) * 70 / 100);
	if (!m_bLerpInited)
		ColorToFloats(color, m_fCurBrR, m_fCurBrG, m_fCurBrB);
	Invalidate(FALSE);
}

COLORREF CRoundButton::GetBorderColor() const { return m_crBorder[RBS_Normal]; }

void CRoundButton::SetUseMouseOverBackColor(bool use) { m_bUseHoverColors = use; }

void CRoundButton::SetMouseOverBackColor(COLORREF color)
{
	m_crBg[RBS_Hover] = color;
	m_bUseHoverColors = true;
	Invalidate(FALSE);
}

void CRoundButton::SetButtonText(LPCTSTR text) { SetWindowText(text); }

CString CRoundButton::GetButtonText() const
{
	CString text;
	GetWindowText(text);
	return text;
}

void CRoundButton::SetRainbowBorder(bool enable)
{
	m_bRainbowBorder = enable;
	EnsureAnimTimer();
	Invalidate(FALSE);
}

void CRoundButton::SetRainbowAnimate(bool enable)
{
	m_bRainbowAnimate = enable;
	EnsureAnimTimer();
	Invalidate(FALSE);
}

void CRoundButton::SetRainbowSpeed(UINT intervalMs)
{
	m_nAnimInterval = max(10u, intervalMs);
	if (m_bTimerRunning) { StopAnimTimer(); EnsureAnimTimer(); }
}

void CRoundButton::SetColorLerp(bool enable)
{
	m_bColorLerp = enable;
	if (!enable)
	{
		RoundButtonState st = ResolveState();
		ColorToFloats(m_crBg[st], m_fCurBgR, m_fCurBgG, m_fCurBgB);
		ColorToFloats(m_crBorder[st], m_fCurBrR, m_fCurBrG, m_fCurBrB);
	}
	EnsureAnimTimer();
	Invalidate(FALSE);
}

void CRoundButton::SetColorLerpDuration(UINT ms) { m_nLerpDurationMs = max(1u, ms); }
void CRoundButton::SetColorLerpEase(RoundButtonEase ease) { m_eLerpEase = ease; }

void CRoundButton::EnsureAnimTimer()
{
	if (!GetSafeHwnd()) return;
	bool need = (m_bRainbowBorder && m_bRainbowAnimate) || m_bColorLerp;
	if (need && !m_bTimerRunning)
	{
		SetTimer(TIMER_ANIM, m_nAnimInterval, NULL);
		m_bTimerRunning = true;
		m_dwLastTick = GetTickCount();
	}
	else if (!need && m_bTimerRunning)
		StopAnimTimer();
}

void CRoundButton::StopAnimTimer()
{
	if (!GetSafeHwnd() || !m_bTimerRunning) return;
	KillTimer(TIMER_ANIM);
	m_bTimerRunning = false;
}

void CRoundButton::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == TIMER_ANIM)
	{
		DWORD now = GetTickCount();
		float dt = (now - m_dwLastTick) / 1000.0f;
		if (dt < 0.0f) dt = 0.0f;
		if (dt > 0.1f) dt = 0.1f;
		m_dwLastTick = now;
		bool dirty = false;
		if (m_bRainbowBorder && m_bRainbowAnimate)
		{
			m_fRainbowHue += 120.0f * dt;
			if (m_fRainbowHue >= 360.0f) m_fRainbowHue -= 360.0f;
			dirty = true;
		}
		if (m_bColorLerp) { ApplyLerpStep(dt); dirty = true; }
		if (dirty) Invalidate(FALSE);
	}
	CButton::OnTimer(nIDEvent);
}

RoundButtonState CRoundButton::ResolveState() const
{
	if (GetSafeHwnd() && !IsWindowEnabled())
		return RBS_Disabled;

	// Pressed look only while the mouse is still over the button
	if (m_bPressed && m_bMouseOver)
		return RBS_Pressed;

	// Mouse left while still holding the button → show hover (not pressed)
	if (m_bPressed && !m_bMouseOver && m_bUseHoverColors)
		return RBS_Hover;

	if (m_bMouseOver && m_bUseHoverColors)
		return RBS_Hover;

	return RBS_Normal;
}

void CRoundButton::ColorToFloats(COLORREF c, float& r, float& g, float& b)
{
	r = (float)GetRValue(c); g = (float)GetGValue(c); b = (float)GetBValue(c);
}

COLORREF CRoundButton::FloatsToColor(float r, float g, float b)
{
	auto clamp = [](float v) -> BYTE {
		if (v < 0.0f) return 0; if (v > 255.0f) return 255; return (BYTE)(v + 0.5f);
	};
	return RGB(clamp(r), clamp(g), clamp(b));
}

COLORREF CRoundButton::LerpColor(COLORREF a, COLORREF b, float t)
{
	if (t <= 0.0f) return a; if (t >= 1.0f) return b;
	float ar, ag, ab, br, bg, bb;
	ColorToFloats(a, ar, ag, ab); ColorToFloats(b, br, bg, bb);
	return FloatsToColor(ar + (br - ar) * t, ag + (bg - ag) * t, ab + (bb - ab) * t);
}

float CRoundButton::ApplyEase(float t, RoundButtonEase ease)
{
	if (t <= 0.0f) return 0.0f; if (t >= 1.0f) return 1.0f;
	switch (ease)
	{
	case RBE_EaseIn: return t * t;
	case RBE_EaseOut: return 1.0f - (1.0f - t) * (1.0f - t);
	case RBE_EaseInOut: return (t < 0.5f) ? 2.0f * t * t : 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
	default: return t;
	}
}

void CRoundButton::CaptureCurrentAsFloats()
{
	RoundButtonState st = ResolveState();
	ColorToFloats(m_crBg[st], m_fCurBgR, m_fCurBgG, m_fCurBgB);
	ColorToFloats(m_crBorder[st], m_fCurBrR, m_fCurBrG, m_fCurBrB);
}

void CRoundButton::ApplyLerpStep(float dtSec)
{
	RoundButtonState st = ResolveState();
	float tr, tg, tb;
	float duration = m_nLerpDurationMs / 1000.0f;
	if (duration < 0.001f) duration = 0.001f;
	float step = dtSec / duration;
	if (step > 1.0f) step = 1.0f;
	float k = (m_eLerpEase == RBE_Linear) ? step : (1.0f - powf(1.0f - step, (m_eLerpEase == RBE_EaseIn) ? 0.5f : 2.0f));
	if (k < 0.0f) k = 0.0f; if (k > 1.0f) k = 1.0f;

	ColorToFloats(m_crBg[st], tr, tg, tb);
	m_fCurBgR += (tr - m_fCurBgR) * k;
	m_fCurBgG += (tg - m_fCurBgG) * k;
	m_fCurBgB += (tb - m_fCurBgB) * k;

	ColorToFloats(m_crBorder[st], tr, tg, tb);
	m_fCurBrR += (tr - m_fCurBrR) * k;
	m_fCurBrG += (tg - m_fCurBrG) * k;
	m_fCurBrB += (tb - m_fCurBrB) * k;
}

BOOL CRoundButton::OnEraseBkgnd(CDC*) { return TRUE; }

void CRoundButton::OnPaint()
{
	CPaintDC dc(this);
	CRect rc; GetClientRect(&rc);
	CDC memDC; memDC.CreateCompatibleDC(&dc);
	CBitmap bmp; bmp.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap* pOld = memDC.SelectObject(&bmp);
	PaintContent(memDC.GetSafeHdc(), rc);
	dc.BitBlt(0, 0, rc.Width(), rc.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.SelectObject(pOld);
}

void CRoundButton::DrawItem(LPDRAWITEMSTRUCT lp)
{
	CRect rc = lp->rcItem; HDC hdc = lp->hDC;
	HDC hdcMem = ::CreateCompatibleDC(hdc);
	HBITMAP hBmp = ::CreateCompatibleBitmap(hdc, rc.Width(), rc.Height());
	HBITMAP hOld = (HBITMAP)::SelectObject(hdcMem, hBmp);
	PaintContent(hdcMem, rc);
	::BitBlt(hdc, rc.left, rc.top, rc.Width(), rc.Height(), hdcMem, 0, 0, SRCCOPY);
	::SelectObject(hdcMem, hOld); ::DeleteObject(hBmp); ::DeleteDC(hdcMem);
}

void CRoundButton::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, m_hWnd, 0 };
		TrackMouseEvent(&tme);
		m_bTracking = true;
	}

	// With mouse capture (held button), OnMouseLeave may not fire.
	// Always derive hover from whether the cursor is inside the client rect.
	CRect rc;
	GetClientRect(&rc);
	bool inside = (rc.PtInRect(point) != FALSE);

	if (inside != m_bMouseOver)
	{
		m_bMouseOver = inside;
		if (!m_bColorLerp)
			CaptureCurrentAsFloats();
		Invalidate(FALSE);
	}

	CButton::OnMouseMove(nFlags, point);
}

void CRoundButton::OnMouseLeave()
{
	m_bTracking = false;
	if (m_bMouseOver)
	{
		m_bMouseOver = false;
		if (!m_bColorLerp)
			CaptureCurrentAsFloats();
		Invalidate(FALSE);
	}
	CButton::OnMouseLeave();
}

void CRoundButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bPressed = true;
	m_bMouseOver = true; // click started inside
	SetCapture();        // keep receiving moves outside so leave-while-pressed works
	if (!m_bColorLerp)
		CaptureCurrentAsFloats();
	Invalidate(FALSE);
	CButton::OnLButtonDown(nFlags, point);
}

void CRoundButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bPressed = false;
	if (GetCapture() == this)
		ReleaseCapture();

	// Update hover based on release position
	CRect rc;
	GetClientRect(&rc);
	m_bMouseOver = (rc.PtInRect(point) != FALSE);

	if (!m_bColorLerp)
		CaptureCurrentAsFloats();
	Invalidate(FALSE);
	CButton::OnLButtonUp(nFlags, point);
}

void CRoundButton::OnEnable(BOOL bEnable)
{
	CButton::OnEnable(bEnable);
	if (!m_bColorLerp) CaptureCurrentAsFloats();
	Invalidate(FALSE);
}

BOOL CRoundButton::PreTranslateMessage(MSG* pMsg) { return CButton::PreTranslateMessage(pMsg); }

GraphicsPath* CRoundButton::CreateRoundedRectanglePath(RectF rect, float radius)
{
	GraphicsPath* path = new GraphicsPath();
	if (rect.Width <= 0.0f || rect.Height <= 0.0f) return path;
	if (radius <= 0.0f) { path->AddRectangle(rect); return path; }
	float diameter = radius * 2.0f;
	float minDim = min(rect.Width, rect.Height);
	if (diameter >= minDim) { path->AddEllipse(rect); return path; }
	RectF arc(rect.X, rect.Y, diameter, diameter);
	path->AddArc(arc, 180.0f, 90.0f);
	arc.X = rect.GetRight() - diameter; path->AddArc(arc, 270.0f, 90.0f);
	arc.Y = rect.GetBottom() - diameter; path->AddArc(arc, 0.0f, 90.0f);
	arc.X = rect.X; path->AddArc(arc, 90.0f, 90.0f);
	path->CloseFigure();
	return path;
}

static Color ColorFromHSV(float h, float s, float v)
{
	while (h < 0.0f) h += 360.0f; while (h >= 360.0f) h -= 360.0f;
	float c = v * s;
	float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
	float m = v - c;
	float r = 0, g = 0, b = 0;
	if (h < 60) { r = c; g = x; } else if (h < 120) { r = x; g = c; }
	else if (h < 180) { g = c; b = x; } else if (h < 240) { g = x; b = c; }
	else if (h < 300) { r = x; b = c; } else { r = c; b = x; }
	return Color(255, (BYTE)((r + m) * 255 + 0.5f), (BYTE)((g + m) * 255 + 0.5f), (BYTE)((b + m) * 255 + 0.5f));
}

void CRoundButton::BuildRainbowColors(Color* colors, int count, float hueOffset) const
{
	for (int i = 0; i < count; ++i)
		colors[i] = ColorFromHSV(hueOffset + (360.0f * i) / (float)count, 1.0f, 1.0f);
}

void CRoundButton::PaintContent(HDC hdc, const CRect& rc)
{
	Graphics g(hdc);
	g.SetSmoothingMode(SmoothingModeAntiAlias);
	g.SetPixelOffsetMode(PixelOffsetModeHalf);
	g.SetCompositingQuality(CompositingQualityHighQuality);

	COLORREF face = GetSysColor(COLOR_3DFACE);
	SolidBrush clearBrush(Color(255, GetRValue(face), GetGValue(face), GetBValue(face)));
	g.FillRectangle(&clearBrush, rc.left, rc.top, rc.Width(), rc.Height());

	const float bw = max(1.0f, m_fBorderWidth);
	const float halfBw = bw * 0.5f;
	RectF pathRect((REAL)rc.left + halfBw, (REAL)rc.top + halfBw, (REAL)rc.Width() - bw, (REAL)rc.Height() - bw);
	if (pathRect.Width < 2.0f || pathRect.Height < 2.0f) return;
	float pathRadius = max(0.0f, (float)m_nRadius - halfBw);

	COLORREF bg, br;
	if (m_bColorLerp && m_bLerpInited)
	{
		bg = FloatsToColor(m_fCurBgR, m_fCurBgG, m_fCurBgB);
		br = FloatsToColor(m_fCurBrR, m_fCurBrG, m_fCurBrB);
	}
	else
	{
		RoundButtonState st = ResolveState();
		bg = m_crBg[st]; br = m_crBorder[st];
	}

	GraphicsPath* path = CreateRoundedRectanglePath(pathRect, pathRadius);
	SolidBrush fillBrush(Color(255, GetRValue(bg), GetGValue(bg), GetBValue(bg)));
	g.FillPath(&fillBrush, path);

	if (bw > 0.0f)
	{
		if (m_bRainbowBorder)
		{
			const int kStops = 8;
			Color colors[kStops];
			BuildRainbowColors(colors, kStops, m_fRainbowHue);
			REAL positions[kStops];
			for (int i = 0; i < kStops; ++i) positions[i] = (REAL)i / (REAL)(kStops - 1);
			RectF gradRect = pathRect; gradRect.Inflate(bw, bw);
			LinearGradientBrush lgb(gradRect, colors[0], colors[kStops - 1], m_fRainbowHue);
			lgb.SetInterpolationColors(colors, positions, kStops);
			lgb.SetWrapMode(WrapModeTileFlipX);
			Pen pen(&lgb, bw);
			pen.SetLineJoin(LineJoinRound);
			pen.SetAlignment(PenAlignmentCenter);
			g.DrawPath(&pen, path);
		}
		else
		{
			Pen pen(Color(255, GetRValue(br), GetGValue(br), GetBValue(br)), bw);
			pen.SetLineJoin(LineJoinRound);
			pen.SetAlignment(PenAlignmentCenter);
			g.DrawPath(&pen, path);
		}
	}
	delete path;

	CString text; GetWindowText(text);
	if (!text.IsEmpty())
	{
		Gdiplus::Font font(L"Segoe UI", 12.0f, FontStyleBold, UnitPoint);
		StringFormat sf;
		sf.SetAlignment(StringAlignmentCenter);
		sf.SetLineAlignment(StringAlignmentCenter);
		sf.SetTrimming(StringTrimmingEllipsisCharacter);
		sf.SetFormatFlags(StringFormatFlagsNoWrap);
		int luminance = (GetRValue(bg) * 299 + GetGValue(bg) * 587 + GetBValue(bg) * 114) / 1000;
		Color textColor = (luminance < 128) ? Color(255, 255, 255, 255) : Color(255, 20, 20, 20);
		SolidBrush textBrush(textColor);
		RectF textRect = pathRect; textRect.Inflate(-bw, -bw);
		g.DrawString(text, -1, &font, textRect, &sf, &textBrush);
	}
}
