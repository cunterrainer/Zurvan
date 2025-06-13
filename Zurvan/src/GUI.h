#pragma once
#include "raygui.h"

class GUI
{
private:
    Rectangle m_Bounds;
    const char* m_Title = "Title";
public:
    GUI() noexcept
    {
        m_Bounds.x = 20;
        m_Bounds.y = 20;
        m_Bounds.width = 100;
        m_Bounds.height = 30;
    }

    void Draw() const noexcept
    {
        GuiWindowBox(m_Bounds, m_Title);
    }
};