#include "stdafx.h"
#include "RoundButton.h"

IMPLEMENT_DYNAMIC(CRoundButton, CButton)

CRoundButton::CRoundButton()
    : m_nRadius(20)
    , m_crBackground(RGB(11, 205, 255))
    , m_crBorder(RGB(255, 255, 255))
    , m_fBorderWidth(7.0f)
    , m_bUseMouseOver(true)
    , m_crMouseOver(RGB(200, 200, 200))
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
}

BEGIN_MESSAGE_MAP(CRoundButton, CButton)
    ON_WM_MOUSEMOVE()
    ON_WM_MOUSELEAVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void CRoundButton::SetRadius(int radius)
{
    m_nRadius = max(0, radius);
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

BOOL CRoundButton::OnEraseBkgnd(CDC* pDC)
{
    return TRUE; // Prevent flicker, we draw everything in DrawItem
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

GraphicsPath* CRoundButton::CreateRoundedRectanglePath(RectF rect, float radius)
{
    GraphicsPath* path = new GraphicsPath();

    if (radius <= 0.0f)
    {
        path->AddRectangle(rect);
        return path;
    }

    float diameter = radius * 2.0f;
    float minDim = min(rect.Width, rect.Height);

    if (diameter >= minDim)
    {
        // Fully rounded (pill / ellipse)
        path->AddEllipse(rect);
        return path;
    }

    // Rounded rectangle (clockwise from top-left)
    RectF arc(rect.X, rect.Y, diameter, diameter);
    path->AddArc(arc, 180, 90);                     // top-left
    arc.X = rect.GetRight() - diameter;
    path->AddArc(arc, 270, 90);                     // top-right
    arc.Y = rect.GetBottom() - diameter;
    path->AddArc(arc, 0, 90);                       // bottom-right
    arc.X = rect.X;
    path->AddArc(arc, 90, 90);                      // bottom-left
    path->CloseFigure();

    return path;
}

void CRoundButton::DrawRoundedRectangle(Graphics& g, Pen& pen, Brush& brush, RectF rect, float radius)
{
    // This helper is kept for compatibility; DrawItem now does fill + stroke separately
    GraphicsPath* path = CreateRoundedRectanglePath(rect, radius);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.FillPath(&brush, path);
    if (pen.GetWidth() > 0.0f)
        g.DrawPath(&pen, path);
    delete path;
}

void CRoundButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rc = lpDrawItemStruct->rcItem;

    // Clear to dialog face color
    pDC->FillSolidRect(&rc, GetSysColor(COLOR_3DFACE));

    Graphics g(lpDrawItemStruct->hDC);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    g.SetCompositingQuality(CompositingQualityHighQuality);

    const float bw = m_fBorderWidth;
    const float halfBw = bw * 0.5f;
    // Small outer padding so anti-aliased edges are not clipped by the control bounds
    const float pad = 1.5f;

    // Rectangle that the CENTER of the pen follows
    // Full pen width stays inside the control client area
    RectF strokeRect(
        (REAL)rc.left + pad + halfBw,
        (REAL)rc.top + pad + halfBw,
        (REAL)rc.Width() - 2.0f * (pad + halfBw),
        (REAL)rc.Height() - 2.0f * (pad + halfBw));

    if (strokeRect.Width <= 0.0f || strokeRect.Height <= 0.0f)
        return;

    // Fill rectangle is inset by half the border so the stroke is not covered by the fill
    RectF fillRect(
        strokeRect.X + halfBw,
        strokeRect.Y + halfBw,
        strokeRect.Width - bw,
        strokeRect.Height - bw);

    // Radius for stroke path (outer curve)
    float strokeRadius = (float)m_nRadius;
    // Radius for fill path (inner curve) — keep concentric look
    float fillRadius = max(0.0f, strokeRadius - halfBw);

    // Background color (darken slightly when pressed)
    COLORREF bg = m_crBackground;
    if (m_bPressed)
    {
        int r = GetRValue(bg) * 85 / 100;
        int gcol = GetGValue(bg) * 85 / 100;
        int b = GetBValue(bg) * 85 / 100;
        bg = RGB(r, gcol, b);
    }

    // ---- Fill ----
    if (fillRect.Width > 0.0f && fillRect.Height > 0.0f)
    {
        SolidBrush brush(Color(255, GetRValue(bg), GetGValue(bg), GetBValue(bg)));
        GraphicsPath* fillPath = CreateRoundedRectanglePath(fillRect, fillRadius);
        g.FillPath(&brush, fillPath);
        delete fillPath;
    }

    // ---- Border (stroke) ----
    if (bw > 0.0f)
    {
        Pen pen(Color(255, GetRValue(m_crBorder), GetGValue(m_crBorder), GetBValue(m_crBorder)), bw);
        pen.SetAlignment(PenAlignmentCenter);
        pen.SetLineJoin(LineJoinRound);
        pen.SetStartCap(LineCapRound);
        pen.SetEndCap(LineCapRound);

        GraphicsPath* strokePath = CreateRoundedRectanglePath(strokeRect, strokeRadius);
        g.DrawPath(&pen, strokePath);
        delete strokePath;
    }

    // ---- Text ----
    CString text;
    GetWindowText(text);

    if (!text.IsEmpty())
    {
        Gdiplus::Font font(L"Segoe UI", 12, FontStyleBold, UnitPoint);
        StringFormat sf;
        sf.SetAlignment(StringAlignmentCenter);
        sf.SetLineAlignment(StringAlignmentCenter);
        sf.SetTrimming(StringTrimmingEllipsisCharacter);
        sf.SetFormatFlags(StringFormatFlagsNoWrap);

        // Auto contrast: black or white text based on background luminance
        int luminance = (GetRValue(bg) * 299 + GetGValue(bg) * 587 + GetBValue(bg) * 114) / 1000;
        Color textColor = (luminance < 128)
            ? Color(255, 255, 255, 255)
            : Color(255, 30, 30, 30);

        SolidBrush textBrush(textColor);

        // Text uses the fill area so it stays inside the border
        RectF textRect = (fillRect.Width > 0.0f) ? fillRect : strokeRect;
        g.DrawString(text, -1, &font, textRect, &sf, &textBrush);
    }
}
