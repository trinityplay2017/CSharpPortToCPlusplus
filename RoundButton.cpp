#include "stdafx.h"
#include "RoundButton.h"

IMPLEMENT_DYNAMIC(CRoundButton, CButton)

CRoundButton::CRoundButton()
    : m_nRadius(20)
    , m_crBackground(RGB(11, 205, 255))
    , m_crBorder(RGB(0, 90, 180))
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
    return TRUE;
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

void CRoundButton::DrawRoundedRectangle(Graphics& g, Pen& pen, Brush& brush, RectF rect, float radius)
{
    GraphicsPath* path = CreateRoundedRectanglePath(rect, radius);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.FillPath(&brush, path);
    if (pen.GetWidth() > 0.0f)
        g.DrawPath(&pen, path);
    delete path;
}

void CRoundButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    HDC hdc = lpDrawItemStruct->hDC;
    CRect rc = lpDrawItemStruct->rcItem;

    // Clear background
    HBRUSH hBr = ::CreateSolidBrush(GetSysColor(COLOR_3DFACE));
    ::FillRect(hdc, &rc, hBr);
    ::DeleteObject(hBr);

    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    g.SetCompositingQuality(CompositingQualityHighQuality);
    g.SetCompositingMode(CompositingModeSourceOver);

    // Leave 2px margin so edges are not clipped by the window
    const float margin = 2.0f;
    const float bw = max(1.0f, m_fBorderWidth);

    RectF outer(
        (REAL)rc.left + margin,
        (REAL)rc.top + margin,
        (REAL)rc.Width() - 2.0f * margin,
        (REAL)rc.Height() - 2.0f * margin);

    if (outer.Width < 4.0f || outer.Height < 4.0f)
        return;

    // Inner rect = outer inset by border width (solid border ring technique)
    RectF inner(
        outer.X + bw,
        outer.Y + bw,
        outer.Width - 2.0f * bw,
        outer.Height - 2.0f * bw);

    float outerRadius = (float)m_nRadius;
    float innerRadius = max(0.0f, outerRadius - bw);

    // Background (pressed = darker)
    COLORREF bg = m_crBackground;
    if (m_bPressed)
    {
        bg = RGB(GetRValue(bg) * 80 / 100,
                 GetGValue(bg) * 80 / 100,
                 GetBValue(bg) * 80 / 100);
    }

    // 1) Draw OUTER shape filled with BORDER color  → this is the visible border ring
    {
        SolidBrush borderBrush(Color(255,
            GetRValue(m_crBorder),
            GetGValue(m_crBorder),
            GetBValue(m_crBorder)));
        GraphicsPath* outerPath = CreateRoundedRectanglePath(outer, outerRadius);
        g.FillPath(&borderBrush, outerPath);
        delete outerPath;
    }

    // 2) Draw INNER shape filled with BACKGROUND color  → punches the center
    if (inner.Width > 0.0f && inner.Height > 0.0f)
    {
        SolidBrush fillBrush(Color(255,
            GetRValue(bg),
            GetGValue(bg),
            GetBValue(bg)));
        GraphicsPath* innerPath = CreateRoundedRectanglePath(inner, innerRadius);
        g.FillPath(&fillBrush, innerPath);
        delete innerPath;
    }

    // 3) Text centered in the inner area
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
