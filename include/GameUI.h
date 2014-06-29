/************************************************************************/
/* Direct3D GameEngine                                                  */
/* 用户界面                 编写： 姚春晖                               */
/************************************************************************/

#pragma once
#include <d3dx9.h>
#include <vector>
#include <string>

class CGUIDialog;
class CGame2D;

class CGUIControl
{
	friend class CGUIDialog;
protected:
	bool bFocused;
	float X, Y;
	float Width;
	float Height;
	bool Visible;
	
	CGUIDialog * Parent;

	CGUIControl(CGUIDialog * parent, float x, float y, float w, float h);

	virtual ~CGUIControl();
public:
	virtual void Draw(CGame2D* g2d);

	virtual bool IsInBound(float x, float y) const;
	virtual void BeforeUpdate( float elapsedTime ) {};

	virtual void AfterUpdate( float elapsedTime ) {};

	virtual void OnMouseUp()   {};
	virtual void OnMouseDown() {};

	virtual void OnGainFocus() {};
	virtual void OnLoseFocus() {};
    virtual void Reset();

	float GetX() const { return X; }
	float GetY() const { return Y; }
	float GetWidth() const { return Width; }
	float GetHeight() const { return Height; }

	bool GetVisible() const { return Visible; }

	void SetVisible(bool v) { Visible = v; }
	void SetPosition(float x, float y) { X = x; Y = y; }
	void SetSize(float w, float h) { Width = w; Height = h; }
	
};

class CImage2D;

class CGUIButton : public CGUIControl
{
	friend class CGUIDialog;
private:
	CImage2D * m_img;
	float m_counter;
	float m_fadeSpeed;

	RECT m_rtNormal;
	RECT m_rtFocus;
	RECT m_rtPush;

	bool m_bClicked;
	bool m_bPushed;

protected:
	CGUIButton(CGUIDialog * parent, float x, float y, float w, float h, CImage2D * img);
	~CGUIButton(void);

public:

	void SetFadeSpeed(float f) { m_fadeSpeed = f; };

	void Draw(CGame2D* g2d);
	void BeforeUpdate( float elapsedTime );
	void AfterUpdate( float elapsedTime );

	void OnMouseUp();
	void OnMouseDown();
	void OnLoseFocus();
    void Reset();


	bool IsClicked() const { return m_bClicked; };
};

class CGUICheckBox : public CGUIControl
{
    friend class CGUIDialog;
private:
    CImage2D * m_img;
    float m_counter;
    float m_fadeSpeed;

    RECT m_rtUncheck;
    RECT m_rtCheck;
    RECT m_rtFocusUncheck;
    RECT m_rtFocusCheck;

    bool m_bChecked;
    bool m_bChanged;

protected:
    CGUICheckBox(CGUIDialog * parent, float x, float y, float w, float h, CImage2D * img);
    ~CGUICheckBox(void);

public:

    void SetFadeSpeed(float f) { m_fadeSpeed = f; };

    void Draw(CGame2D* g2d);
    void BeforeUpdate( float elapsedTime );
    void AfterUpdate( float elapsedTime );

    void OnMouseUp();
    void OnMouseDown();
    void OnLoseFocus();
    void Reset();

    bool IsChecked() const { return m_bChecked; };
    bool IsChanged() const { return m_bChanged; };
    void SetChecked(bool check) { m_bChecked = check; };
};

class CGUIList : public CGUIControl
{
	friend class CGUIDialog;
private:
	CImage2D * m_imgBk;
	CImage2D * m_imgSel;

	bool m_bSelChanged;
	int m_index;

	int m_lines;
	int m_topline;

	LPD3DXFONT m_font;

	float m_itemHeight;

	DWORD m_textColor;
	DWORD m_selColor;

	int m_offsetX, m_offsetY;

	CGUIButton * m_btUp;
	CGUIButton * m_btDown;
protected:
	CGUIList(CGUIDialog * parent, float x, float y, float w, float h, int lines, CImage2D * imgBk, CImage2D * imgSel,
				CGUIButton * btUp, CGUIButton * btDown);
	~CGUIList(void);
public:
	std::vector<std::string> Items;
	void Draw(CGame2D* g2d);
	void BeforeUpdate( float elapsedTime );
	void AfterUpdate( float elapsedTime );
    void Reset();
	void OnMouseDown();

	bool IsSelChanged() const { return m_bSelChanged; };
	int GetIndex() const { return m_index; };
	void SetItemHeight(float h) { m_itemHeight = h; }

	void SetOffset(int x, int y) { m_offsetX = x; m_offsetY = y; }
	void SetTextColor(DWORD text, DWORD sel) { m_textColor = text; m_selColor = sel; }
    void SetLines(int l) { m_lines = l; }
    void SetIndex(int index);

    void SetFont(LPD3DXFONT f) { m_font = f; };
    LPD3DXFONT GetFont() const { return m_font; };

	int GetIndexAt(float x, float y) const;
};

class CGUILabel : public CGUIControl
{
    friend class CGUIDialog;
private:
    LPD3DXFONT m_font;
    std::string m_text;
    D3DXCOLOR m_color;
protected:
    CGUILabel(CGUIDialog * parent, LPD3DXFONT font, float x, float y, const char* text, D3DXCOLOR color);
    ~CGUILabel(void);

public:

    void SetFont(LPD3DXFONT f) { m_font = f; };
    LPD3DXFONT GetFont() const { return m_font; };
    void SetColor(D3DXCOLOR c) { m_color = c; };
    D3DXCOLOR GetColor() const { return m_color; };
    void SetText(const char* text) { m_text = text; };
    const char* GetText() const { return m_text.c_str(); };
    void Printf(const char* fmt, ...);
    void Draw(CGame2D* g2d);
};

class CGUISlider : public CGUIControl
{
    friend class CGUIDialog;
private:
    CImage2D* m_imgBar;
    CImage2D* m_imgBlock;
    float m_value;
    float m_lastValue;
    float m_minValue;
    float m_maxValue;
    float m_blockW;
    float m_blockH;
    int m_offsetX, m_offsetY;
    bool m_bPushed;
    bool m_bChanged;
protected:
    CGUISlider(CGUIDialog * parent, float x, float y, float w, float h, float bw, float bh,
        CImage2D* imgBar, CImage2D* imgBlock);
    ~CGUISlider(void);

    float ComputeValue() const;
public:

    void SetRange(float minValue, float maxValue) 
        { m_minValue = minValue; m_maxValue = maxValue; };
    void SetValue(float v) { m_value = v; };
    float GetValue() const { return m_value; };
    void SetOffset(int x, int y) { m_offsetX = x; m_offsetY = y; }
    void Reset();
    void Draw(CGame2D* g2d);
    void OnMouseDown();
    void AfterUpdate( float elapsedTime );
    bool IsChanged() const { return m_bChanged; }

};



enum DialogState {DIALOG_STATE_FLYIN, DIALOG_STATE_SHOW, DIALOG_STATE_FLYOUT };

class CGUIDialog
{
private:
	std::vector<CGUIControl *> m_children;

	float m_x, m_y;

	float m_width;

	float m_height;

	DialogState m_state;

	float m_speed;

	bool m_bFly;

	float m_counter;

	CGUIControl * m_focusedCtrl;

	int GetFocusIndex() const;

    float m_destX, m_destY;
    float m_origX, m_origY;

    CImage2D* m_bkImg;
	
public: 
	bool IsInBound(float x, float y) const;
	
	size_t GetControlCount() const 
	{
		return m_children.size();
	};

	CGUIButton * AddButton(float x, float y, float w, float h, CImage2D * img);
    CGUICheckBox * AddCheckBox(float x, float y, float w, float h, CImage2D * img);
    CGUILabel * AddLabel(LPD3DXFONT font, float x, float y, const char* text, D3DXCOLOR color);
	CGUIList * AddList(float x, float y, float w, float h, int lines, 
        CImage2D * imgBk, CImage2D * imgSel,
		CGUIButton * btUp, CGUIButton * btDown);
    CGUISlider * AddSlider(float x, float y, float w, float h, float bw, float bh, 
        CImage2D* imgBar, CImage2D* imgBlock, float minv = 0, float maxv = 1);

	void Draw(CGame2D* g2d);

	void Clear();
    
    void Reset();
	
    void SetDest(float x, float y) { m_destX = x; m_destY = y; };

    void SetOrign(float x, float y) { m_origX = x; m_origY = y; };

	void SetPos(float x, float y) { m_x = x; m_y = y; }

    void SetFlySpeed(float speed) { m_speed = speed; }
    
    void SetState(DialogState state) { m_state = state; }

	void Update( float elapsedTime );

    void Move( float elapsedTime );

	float GetX() const { return m_x; }
	float GetY() const { return m_y; }
	float GetWidth() const { return m_width; }
	float GetHeight() const { return m_height; }
    float GetDestX() const { return m_destX; }
    float GetDestY() const { return m_destY; }
    float GetOrignX() const { return m_origX; }
    float GetOrignY() const { return m_origY; }
    float GetFlySpeed() const { return m_speed; }
    DialogState GetState() const { return m_state; };
    

	void SetFocusedControl(CGUIControl * c);
	CGUIControl * GetFocusControl() const { return m_focusedCtrl; }

	CGUIDialog(float x, float y, float w, float h,  bool fly = true, 
               float speed = 100.0f, CImage2D* bk = NULL);

	~CGUIDialog();
};
