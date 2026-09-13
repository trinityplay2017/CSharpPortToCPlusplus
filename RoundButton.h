#pragma once

#include <afxwin.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

// ---------------------------------------------------------------------------
// Button visual states
// ---------------------------------------------------------------------------
enum RoundButtonState
{
	RBS_Normal = 0,
	RBS_Hover,
	RBS_Pressed,
	RBS_Disabled,
	RBS_Count
};

// Easing for color lerp
enum RoundButtonEase
{
	RBE_Linear = 0,
	RBE_EaseIn,
	RBE_EaseOut,
	RBE_EaseInOut
};

// ---------------------------------------------------------------------------
// CRoundButton
// ---------------------------------------------------------------------------
class CRoundButton : public CButton
{
	DECLARE_DYNAMIC(CRoundButton)

public:
	CRoundButton();
	virtual ~CRoundButton();

	// Geometry
	void SetRadius(int radius);
	int  GetRadius() const { return m_nRadius; }

	void SetBorderWidth(float width);
	float GetBorderWidth() const { return m_fBorderWidth; }

	// ---- Per-state background / border colors ----
	void SetStateBackgroundColor(RoundButtonState state, COLORREF color);
	COLORREF GetStateBackgroundColor(RoundButtonState state) const;

	void SetStateBorderColor(RoundButtonState state, COLORREF color);
	COLORREF GetStateBorderColor(RoundButtonState state) const;

	// Convenience: set Normal color (and optionally mirror to other states)
	void SetBackgroundColor(COLORREF color);
	COLORREF GetBackgroundColor() const;

	void SetBorderColor(COLORREF color);
	COLORREF GetBorderColor() const;

	// Backward-compatible helpers
	void SetUseMouseOverBackColor(bool use);
	bool GetUseMouseOverBackColor() const { return m_bUseHoverColors; }
	void SetMouseOverBackColor(COLORREF color);

	void SetButtonText(LPCTSTR text);
	CString GetButtonText() const;

	// ---- Rainbow border ----
	void SetRainbowBorder(bool enable);
	bool GetRainbowBorder() const { return m_bRainbowBorder; }

	void SetRainbowAnimate(bool enable);
	bool GetRainbowAnimate() const { return m_bRainbowAnimate; }

	void SetRainbowSpeed(UINT intervalMs);
	UINT GetRainbowSpeed() const { return m_nAnimInterval; }

	// ---- Color lerp (smooth transitions between states) ----
	void SetColorLerp(bool enable);
	bool GetColorLerp() const { return m_bColorLerp; }

	// Full transition duration in milliseconds (default 150)
	void SetColorLerpDuration(UINT ms);
	UINT GetColorLerpDuration() const { return m_nLerpDurationMs; }

	// Easing function for lerp
	void SetColorLerpEase(RoundButtonEase ease);
	RoundButtonEase GetColorLerpEase() const { return m_eLerpEase; }

protected:
	virtual void PreSubclassWindow();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

private:
	void UpdateRegion();
	void EnsureAnimTimer();
	void StopAnimTimer();
	void PaintContent(HDC hdc, const CRect& rc);
	GraphicsPath* CreateRoundedRectanglePath(RectF rect, float radius);
	void BuildRainbowColors(Color* colors, int count, float hueOffset) const;

	RoundButtonState ResolveState() const;
	void CaptureCurrentAsFloats();
	void ApplyLerpStep(float dtSec);
	static float ApplyEase(float t, RoundButtonEase ease);
	static COLORREF LerpColor(COLORREF a, COLORREF b, float t);
	static void ColorToFloats(COLORREF c, float& r, float& g, float& b);
	static COLORREF FloatsToColor(float r, float g, float b);

	static const UINT_PTR TIMER_ANIM = 1001;

	int         m_nRadius;
	float       m_fBorderWidth;

	// Per-state colors
	COLORREF    m_crBg[RBS_Count];
	COLORREF    m_crBorder[RBS_Count];
	bool        m_bUseHoverColors;

	// Interaction
	bool        m_bMouseOver;
	bool        m_bTracking;
	bool        m_bPressed;
	ULONG_PTR   m_gdiplusToken;

	// Rainbow
	bool        m_bRainbowBorder;
	bool        m_bRainbowAnimate;
	float       m_fRainbowHue;

	// Lerp
	bool            m_bColorLerp;
	UINT            m_nLerpDurationMs;
	RoundButtonEase m_eLerpEase;
	float           m_fCurBgR, m_fCurBgG, m_fCurBgB;
	float           m_fCurBrR, m_fCurBrG, m_fCurBrB;
	bool            m_bLerpInited;

	// Shared animation timer (rainbow + lerp)
	UINT        m_nAnimInterval;
	bool        m_bTimerRunning;
	DWORD       m_dwLastTick;
};
