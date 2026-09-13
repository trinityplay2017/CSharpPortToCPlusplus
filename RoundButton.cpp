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

    if (radius >= minDim / 2.0f)
    {
        // Capsule / ellipse
        path->AddEllipse(rect);
        return path;
    }

    // Rounded rectangle
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
    GraphicsPath* path = CreateRoundedRectanglePath(rect, radius);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.FillPath(&brush, path);
    if (pen.GetWidth() > 0)
        g.DrawPath(&pen, path);
    delete path;
}

void CRoundButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
    CRect rc = lpDrawItemStruct->rcItem;

    // Clear background (transparent-ish, parent bg)
    pDC->FillSolidRect(&rc, GetSysColor(COLOR_3DFACE)); // or parent color

    Graphics g(lpDrawItemStruct->hDC);
    g.SetSmoothingMode(SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(PixelOffsetModeHighQuality);

    // Inset for border padding (similar to original 10px margin)
    float margin = 4.0f;
    RectF rect(
        (REAL)rc.left + margin,
        (REAL)rc.top + margin,
        (REAL)rc.Width() - 2 * margin,
        (REAL)rc.Height() - 2 * margin);

    // Slightly darker when pressed
    COLORREF bg = m_crBackground;
    if (m_bPressed)
    {
        // Darken a bit
        int r = GetRValue(bg) * 9 / 10;
        int gcol = GetGValue(bg) * 9 / 10;
        int b = GetBValue(bg) * 9 / 10;
        bg = RGB(r, gcol, b);
    }

    SolidBrush brush(Color(255, GetRValue(bg), GetGValue(bg), GetBValue(bg)));
    Pen pen(Color(255, GetRValue(m_crBorder), GetGValue(m_crBorder), GetBValue(m_crBorder)), m_fBorderWidth);

    DrawRoundedRectangle(g, pen, brush, rect, (float)m_nRadius);

    // Draw text
    CString text;
    GetWindowText(text);

    if (!text.IsEmpty())
    {
        Gdiplus::Font font(L"Segoe UI", 12, FontStyleBold, UnitPoint);
        StringFormat sf;
        sf.SetAlignment(StringAlignmentCenter);
        sf.SetLineAlignment(StringAlignmentCenter);
        sf.SetTrimming(StringTrimmingEllipsisCharacter);

        SolidBrush textBrush(Color(255, 0, 0, 0)); // black text, or make configurable
        // For better contrast, could choose white/black based on bg luminance
        int luminance = (GetRValue(bg) * 299 + GetGValue(bg) * 587 + GetBValue(bg) * 114) / 1000;
        if (luminance < 128)
            textBrush.SetColor(Color(255, 255, 255, 255));

        RectF textRect = rect;
        g.DrawString(text, -1, &font, textRect, &sf, &textBrush);
    }
}
