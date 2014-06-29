#include "GameUI.h"
#include "Image2D.h"
#include "MyLib.h"
#include "Game2D.h"
#include "GameEngine.h"
#include <stdarg.h>


/******************************************
     CGUIControl
/*****************************************/

CGUIControl::CGUIControl(CGUIDialog * parent, float x, float y, float w, float h)
:X(x), Y(y), Width(w), Height(h)
{
	bFocused = false;
	Parent = parent;
	Visible = true;
}

CGUIControl::~CGUIControl()
{
}

void CGUIControl::Draw(CGame2D* g2d)
{
	RECT rt = MakeBounds((int)(X + Parent->GetX()), (int)(Y + Parent->GetY()), (int) Width, (int) Height);
	g2d->DrawRect(&rt, 0xFFFFFFFF);
}

//void CGUIControl::Update(float elapsedTime)
//{
//	bool over = IsInBound(g_MouseMgr.MouseX, g_MouseMgr.MouseY);
//	bool press = g_MouseMgr.LButtonDown;
//	bool move = g_MouseMgr.LastMouseX != g_MouseMgr.MouseX || 
//		g_MouseMgr.LastMouseY != g_MouseMgr.MouseY;
//	bool enter = false;
//	bool leave = false;
//	ControlMouseEvent r = MOUSE_EVENT_NONE;
//
//	if (over)
//	{
//		if (!m_bMouseOver) enter = true;
//		if (move)
//		{
//			if (m_bMousePressed) r = MOUSE_EVENT_DRAG;
//			else r = MOUSE_EVENT_MOVE;
//		}
//		if (m_bMousePressed && !press)
//		{
//			r = MOUSE_EVENT_CLICK;	
//		}
//	}
//	else
//	{
//		if (m_bMouseOver) leave = true;
//	}
//	m_bMouseOver = over;
//	m_bMousePressed = over && press;
//	if (enter) OnMouseEnter(fElapsedTime);
//	if (leave) OnMouseLeave(fElapsedTime);
//	return r;
//}

bool CGUIControl::IsInBound(float x, float y) const
{ 
	x -= Parent->GetX();
	y -= Parent->GetY();
	return x >= X && y >= Y && x < X + Width && y < Y + Height;
};

void CGUIControl::Reset()
{
}
/******************************************
   CGUIButton
/*****************************************/


CGUIButton::CGUIButton(CGUIDialog * parent, float x, float y, float w, float h, CImage2D * img)
:CGUIControl(parent, x, y, w, h)
{
	m_img = img;
	if (m_img)
	{
		float cliph = m_img->TextureHeight / 3.0f;
		m_rtNormal = MakeBounds(0, 0, m_img->TextureWidth, (int)(cliph + 0.5f));
		m_rtFocus  = MakeBounds(0, (int)(cliph + 0.5f), m_img->TextureWidth, (int) (cliph + 0.5f));
		m_rtPush   = MakeBounds(0, (int)(cliph * 2 + 0.5f), m_img->TextureWidth, (int) (cliph + 0.5f));
	}
	m_fadeSpeed = 5;
	m_counter = 0;
	m_bPushed = false;
    m_bClicked = false;
}

CGUIButton::~CGUIButton()
{
}

void CGUIButton::Draw( CGame2D* g2d )
{
	if (m_img)
	{
		SetEffect(fxBlend);
		if (m_bPushed)
		{
			g2d->DrawImageScale(m_img, X + Parent->GetX(), Y + Parent->GetY(), Width, Height * 3, &m_rtPush);
		}else
		{
			D3DCOLOR color1 = D3DXCOLOR(1, 1, 1, 1 - m_counter);
			D3DCOLOR color2 = D3DXCOLOR(1, 1, 1, m_counter);

			g2d->DrawImageScale(m_img, X + Parent->GetX(), Y + Parent->GetY(), Width, Height * 3, 
								&m_rtNormal, color1);
			g2d->DrawImageScale(m_img, X + Parent->GetX(), Y + Parent->GetY(), Width, Height * 3, 
								&m_rtFocus, color2);
		}
	}
}

void CGUIButton::BeforeUpdate(float elapsedTime )
{
	if (bFocused)
	{
		m_counter += m_fadeSpeed * elapsedTime;
		if (m_counter > 1) m_counter = 1;
	}
	else
	{
		m_counter -= m_fadeSpeed * elapsedTime;
		if (m_counter < 0) m_counter = 0;
	}

	m_bClicked = false;
}

void CGUIButton::AfterUpdate(float elapsedTime )
{

}

void CGUIButton::OnMouseDown()
{
	m_bPushed = true;
}

void CGUIButton::OnMouseUp()
{
	if (m_bPushed)
	{
		m_bClicked = true;
		m_bPushed = false;
	}
}

void CGUIButton::OnLoseFocus()
{
	m_bPushed = false;
}

void CGUIButton::Reset()
{
    m_bPushed = false;
    m_bClicked = false;    
}

/******************************************
CGUICheckBox
/*****************************************/


CGUICheckBox::CGUICheckBox(CGUIDialog * parent, float x, float y, float w, float h, CImage2D * img)
:CGUIControl(parent, x, y, w, h)
{
    m_img = img;
    if (m_img)
    {
        float cliph = m_img->TextureHeight / 4.0f;
        m_rtUncheck = MakeBounds(0, 0, m_img->TextureWidth, (int)(cliph + 0.5f));
        m_rtCheck = MakeBounds(0, (int)(cliph + 0.5f), m_img->TextureWidth, (int) (cliph + 0.5f));
        m_rtFocusUncheck = MakeBounds(0, (int)(cliph * 2 + 0.5f), m_img->TextureWidth, (int) (cliph + 0.5f));
        m_rtFocusCheck = MakeBounds(0, (int)(cliph * 3 + 0.5f), m_img->TextureWidth, (int) (cliph + 0.5f));
    }
    m_fadeSpeed = 15;
    m_counter = 0;
    m_bChecked = false;
    m_bChanged = false;
}

CGUICheckBox::~CGUICheckBox()
{
}

void CGUICheckBox::Draw( CGame2D* g2d )
{
    if (m_img)
    {
        SetEffect(fxBlend);
        D3DCOLOR color1 = D3DXCOLOR(1, 1, 1, 1 - m_counter);
        D3DCOLOR color2 = D3DXCOLOR(1, 1, 1, m_counter);

        if (m_bChecked)
        {
            g2d->DrawImageScale(m_img, X + Parent->GetX(), Y + Parent->GetY(), Width, Height * 4, 
                &m_rtCheck, color1);
            g2d->DrawImageScale(m_img, X + Parent->GetX(), Y + Parent->GetY(), Width, Height * 4, 
                &m_rtFocusCheck, color2);
        }else
        {

            g2d->DrawImageScale(m_img, X + Parent->GetX(), Y + Parent->GetY(), Width, Height * 4, 
                &m_rtUncheck, color1);
            g2d->DrawImageScale(m_img, X + Parent->GetX(), Y + Parent->GetY(), Width, Height * 4, 
                &m_rtFocusUncheck, color2);
        }
    }
}

void CGUICheckBox::BeforeUpdate(float elapsedTime )
{
    if (bFocused)
    {
        m_counter += m_fadeSpeed * elapsedTime;
        if (m_counter > 1) m_counter = 1;
    }
    else
    {
        m_counter -= m_fadeSpeed * elapsedTime;
        if (m_counter < 0) m_counter = 0;
    }
    m_bChanged = false;
}

void CGUICheckBox::AfterUpdate(float elapsedTime )
{

}

void CGUICheckBox::OnMouseDown()
{
    m_bChecked = !m_bChecked;
    m_bChanged = true;
}

void CGUICheckBox::OnMouseUp()
{
}

void CGUICheckBox::OnLoseFocus()
{
}

void CGUICheckBox::Reset()
{
    m_bChanged = false;
}

/******************************************
		CGUIList
/*****************************************/

CGUIList::CGUIList(CGUIDialog * parent, float x, float y, float w, float h, int lines, CImage2D * imgBk, CImage2D * imgSel,
				   CGUIButton * btUp, CGUIButton * btDown)
: CGUIControl(parent, x, y, w, h), m_lines(lines), m_imgBk(imgBk), m_imgSel(imgSel), m_btUp(btUp), m_btDown(btDown)
{
	m_bSelChanged = false;
	m_font = LoadFont("Arial", 20);
	m_topline = 0;
	m_itemHeight = 20;
	m_textColor = 0xFFAAAAAA;
	m_selColor = 0xFFFFFFFF;
	m_index = 0;
	m_offsetX = 5;
	m_offsetY = 5;
}

CGUIList::~CGUIList()
{

}

void CGUIList::Draw(CGame2D* g2d)
{
	if (m_imgBk)
	{
		SetEffect(fxBlend);
		g2d->DrawImageScale(m_imgBk, X + Parent->GetX(), Y + Parent->GetY(), Width, Height);
	}

	for (int i = m_topline; i < (int)Items.size() && i < m_topline + m_lines; i++)
	{
		RECT rt = MakeBounds((int)(X + Parent->GetX() + m_offsetX + 10), 
							(int)(Y + Parent->GetY() + m_offsetY + m_itemHeight * (i - m_topline)),
							 (int)(Width + 0.5f - m_offsetX * 2), (int)(m_itemHeight + 0.5f));
		if (m_index != i)
		{
			g2d->DrawString(m_font, Items[i].c_str(), &rt, m_textColor, DT_VCENTER);
		}else
		{
			g2d->DrawImageScale(m_imgSel, (float)rt.left - 10, (float)rt.top, Width - m_offsetX * 2, m_itemHeight);
			g2d->DrawString(m_font, Items[i].c_str(), &rt, m_selColor, DT_VCENTER);
		}
		
	}
}

void CGUIList::BeforeUpdate(float elapsedTime)
{
    m_bSelChanged = false;
}

void CGUIList::AfterUpdate(float elapsedTime )
{
	if (m_btUp)
	{
		if (m_btUp->IsClicked() && m_topline > 0) m_topline --;
		m_btUp->SetVisible(m_topline > 0);
	}

	if (m_btDown)
	{
		if (m_btDown->IsClicked() && m_topline < (int)Items.size() - m_lines) m_topline ++;
		m_btDown->SetVisible(m_topline < (int)Items.size() - m_lines);
	}

    if (Parent->GetFocusControl() == this)
    {   
        CMouseManager* mouse = GetMouse();
        if (mouse->GetWheel() < 0 && m_index < (int)Items.size() - 1)
            SetIndex(m_index + 1);
        else if (mouse->GetWheel() > 0 && m_index > 0)
            SetIndex(m_index - 1);
    }
}

void CGUIList::OnMouseDown()
{
	int i = GetIndexAt((float) GetMouse()->GetX(), (float) GetMouse()->GetY());
	if (i >= 0) SetIndex(i);
}

int CGUIList::GetIndexAt(float x, float y) const
{
	if (!IsInBound(x, y)) return -1;
	x -= Parent->GetX() + m_offsetX + X;
	y -= Parent->GetY() + m_offsetY + Y;

	int i = (int)floor(y / m_itemHeight);
	if ( i < 0 || i >= m_lines) return -1;
	i += m_topline;
	if (i >= (int)Items.size()) return -1;
	return i;
}

void CGUIList::Reset()
{
    m_bSelChanged = false;
}

void CGUIList::SetIndex(int index)
{
    if (m_index != index)
        m_bSelChanged = true;
    m_index = index;
    if (m_index < 0 || m_index >= (int)Items.size())
        return;
    if (m_index < m_topline)
        m_topline = m_index;
    else if (m_index >= m_topline + m_lines)
        m_topline = m_index - m_lines + 1;

}
/******************************************
       CGUILabel
/*****************************************/
CGUILabel::CGUILabel( CGUIDialog * parent, LPD3DXFONT font, float x, float y, const char* text, D3DXCOLOR color )
: CGUIControl(parent, x, y, 100, 100), m_font(font), m_text(text), m_color(color)
{

}

CGUILabel::~CGUILabel( void )
{

}

void CGUILabel::Draw( CGame2D* g2d )
{
    g2d->DrawStringShadow(m_font, m_text.c_str(), 
        (int)(X + Parent->GetX()), (int)(Y + Parent->GetY()), m_color);
}

void CGUILabel::Printf( const char* fmt, ... )
{
    va_list args;
    static char txt[256];
    va_start(args, fmt);
    vsprintf(txt, fmt, args);
    va_end(args);
    m_text = txt;
}

/******************************************
       CGUISlider
/*****************************************/

CGUISlider::CGUISlider( CGUIDialog * parent, 
                       float x, float y, float w, float h, float bw, float bh,
                       CImage2D* imgBar, CImage2D* imgBlock)
                       :CGUIControl(parent, x, y, w, h), 
                       m_blockW(bw), m_blockH(bh),
                       m_imgBar(imgBar), m_imgBlock(imgBlock),
                       m_minValue(0), m_maxValue(1), m_lastValue(0.5),
                       m_value(0.5f), m_bPushed(false), m_bChanged(true),
                       m_offsetX(0), m_offsetY(0)
{

}

CGUISlider::~CGUISlider( void )
{

}

void CGUISlider::Reset()
{
    m_bPushed = false;
}

void CGUISlider::Draw( CGame2D* g2d )
{
    float x = X + Parent->GetX();
    float y = Y + Parent->GetY();
    g2d->DrawImageScale(m_imgBar, x - m_offsetX, y - m_offsetY, 
                        Width + 2 * m_offsetX, 
                        Height + 2 * m_offsetY);
    x += (m_value - m_minValue) / (m_maxValue - m_minValue) * Width;
    g2d->DrawImageScale(m_imgBlock, x - m_blockW / 2, 
                        y - m_blockH / 2 + Height / 2, 
                        m_blockW, m_blockH);
}

float CGUISlider::ComputeValue() const
{
    float x = GetMouse()->GetX() - Parent->GetX() - X;
    float v = x / Width * (m_maxValue - m_minValue) + m_minValue;
    return max(m_minValue, min(v, m_maxValue));
}

void CGUISlider::OnMouseDown()
{
    m_bPushed = true;
}

void CGUISlider::AfterUpdate( float elapsedTime )
{
    m_bChanged = false;
    if (m_bPushed)
    {
        m_bPushed = GetMouse()->IsPressed(0);
        m_value = ComputeValue();
        if (m_value != m_lastValue)
        {
            m_bChanged = true;
            m_lastValue = m_value;
        }
    }
}
/******************************************
		CGUIDialog
/*****************************************/

CGUIDialog::CGUIDialog(float x, float y, float w, float h, bool fly, float speed,
                       CImage2D* bk)
	: m_x(x), m_y(y), m_width(w), m_height(h), m_bFly(fly), m_speed(speed), m_bkImg(bk)
{
    m_state = DIALOG_STATE_SHOW;

	m_focusedCtrl = NULL;

    m_origX = x; m_origY = y;
    m_destX = x; m_destY = y;
}

CGUIDialog::~CGUIDialog()
{
	Clear();
}

CGUIButton * CGUIDialog::AddButton(float x, float y, float w, float h, CImage2D * img)
{
	CGUIButton * bt = new CGUIButton(this, x, y, w, h, img);
	m_children.push_back(bt);
	return bt;
}

CGUICheckBox * CGUIDialog::AddCheckBox(float x, float y, float w, float h, CImage2D * img)
{
    CGUICheckBox * ck = new CGUICheckBox(this, x, y, w, h, img);
    m_children.push_back(ck);
    return ck;
}

CGUIList * CGUIDialog::AddList(float x, float y, float w, float h, int lines, CImage2D *imgBk, CImage2D *imgSel, 
								CGUIButton * btUp, CGUIButton * btDown)
{
	CGUIList * list = new CGUIList(this, x, y, w, h, lines, imgBk, imgSel, btUp, btDown);
	m_children.push_back(list);
	return list;
}


CGUILabel * CGUIDialog::AddLabel( LPD3DXFONT font, float x, float y, 
                                 const char* text, D3DXCOLOR color )
{
    CGUILabel * label = new CGUILabel(this, font, x, y, text, color);
    m_children.push_back(label);
    return label;
}

CGUISlider * CGUIDialog::AddSlider( float x, float y, float w, float h, 
                                   float bw, float bh, CImage2D* imgBar, CImage2D* imgBlock,
                                   float minv, float maxv)
{
    CGUISlider * slider = new CGUISlider(this, x, y, w, h, bw, bh, imgBar, imgBlock);
    m_children.push_back(slider);
    slider->SetRange(minv, maxv);
    return slider;
}

void CGUIDialog::Clear()
{
	for (size_t i = 0; i < m_children.size(); i ++)
	{
		SAFE_DELETE(m_children[i]);
	}
	m_children.clear();
	m_focusedCtrl = NULL;
}

void CGUIDialog::SetFocusedControl(CGUIControl * c)
{
	if (c == m_focusedCtrl) return;

	if (m_focusedCtrl != NULL)
	{
		m_focusedCtrl->bFocused = false;
		m_focusedCtrl->OnLoseFocus();
	}	

	m_focusedCtrl = c;

	if (c)
	{
		c->bFocused = true;
		c->OnGainFocus();
	}
}

int CGUIDialog::GetFocusIndex() const
{
	if (m_focusedCtrl != NULL)
	{
		for (size_t i = 0; i < m_children.size(); i ++)
		{
			if (m_focusedCtrl == m_children[i])
				return (int) i;
		}
	}
	return -1;
}


void CGUIDialog::Draw( CGame2D * g2d )
{
    if (m_bkImg)
        g2d->DrawImageScale(m_bkImg, m_x, m_y, m_width, m_height);
	for (size_t i = 0; i < m_children.size(); i ++)
	{
		CGUIControl * c = m_children[i];

		if (c && c->Visible)
		{
			c->Draw(g2d);
		}
	}
}

bool CGUIDialog::IsInBound(float x, float y) const
{
	return x >= m_x && y >= m_y && x < m_x + m_width && y < m_y + m_height;
}

void CGUIDialog::Move( float elapsedTime )
{
    if (m_bFly)
    {
        if (m_state == DIALOG_STATE_FLYIN)
        {
            float dx = m_destX - m_x;
            float dy = m_destY - m_y;
            float len = sqrt(dx * dx + dy * dy);
            if (len > elapsedTime * m_speed)
            {
                m_x += dx * elapsedTime * m_speed / len;
                m_y += dy * elapsedTime * m_speed / len;
            }
            else
            {
                m_x = m_destX;
                m_y = m_destY;
                m_state = DIALOG_STATE_SHOW;
            }
        }
        else if (m_state == DIALOG_STATE_FLYOUT)
        {
            float dx = m_origX - m_x;
            float dy = m_origY - m_y;
            float len = sqrt(dx * dx + dy * dy);
            if (len > elapsedTime * m_speed)
            {
                m_x += dx * elapsedTime * m_speed / len;
                m_y += dy * elapsedTime * m_speed / len;
            }
            else
            {
                m_x = m_origX;
                m_y = m_origY;
                m_state = DIALOG_STATE_SHOW;
            }
        } 
    }
}

void CGUIDialog::Update( float elapsedTime )
{
	CMouseManager * mouse = GetMouse();
	//鼠标悬停在的控件
	CGUIControl * mouse_over = NULL;

	for (size_t i = 0; i < m_children.size(); i ++)
	{
		if (m_children[i]->GetVisible() && m_children[i]->IsInBound((float)mouse->GetX(), (float)mouse->GetY()))
		{
			mouse_over = m_children[i];
		}
		m_children[i]->BeforeUpdate(elapsedTime);
	}

	//鼠标移动，更新焦点控件
	if (mouse->MouseMoved())
	{
		SetFocusedControl(mouse_over);
	}
	
	if (mouse_over)
	{
		if (mouse->IsJustPressed(0))
		{
			SetFocusedControl(mouse_over);
			mouse_over->OnMouseDown();
		}else if (mouse->IsJustReleased(0))
		{
			SetFocusedControl(mouse_over);
			mouse_over->OnMouseUp();
		}
	}

	if (! m_children.empty())
	{
		if (GetKeyboard()->IsJustPressed(VK_DOWN))
		{
			int f = GetFocusIndex() + 1;
			if (f == m_children.size()) f = 0;
			SetFocusedControl(m_children[f]);
		}else if (GetKeyboard()->IsJustPressed(VK_UP))
		{
			int f = GetFocusIndex() - 1;
			if (f < 0) f = (int)m_children.size() - 1;
			SetFocusedControl(m_children[f]);
		}else if (GetKeyboard()->IsJustPressed(VK_RETURN) || GetKeyboard()->IsJustPressed(VK_SPACE))
		{
			if (m_focusedCtrl)
			{
				m_focusedCtrl->OnMouseDown();
			}
		}else if (GetKeyboard()->IsJustReleased(VK_RETURN) || GetKeyboard()->IsJustReleased(VK_SPACE))
		{
			if (m_focusedCtrl)
			{
				m_focusedCtrl->OnMouseUp();
			}
		}
	}

	for (size_t i = 0; i < m_children.size(); i ++)
	{
		m_children[i]->AfterUpdate(elapsedTime);
	}

}

void CGUIDialog::Reset()
{
    for (size_t i = 0; i < m_children.size(); i ++)
    {
        m_children[i]->Reset();
    }
}
