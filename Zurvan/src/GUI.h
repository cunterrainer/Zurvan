#pragma once
// needed because otherwise raygui will define them internally
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
#define RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT 18

#include "styles/dark/style_dark.h"
#include "styles/amber/style_amber.h"
#include "styles/ashes/style_ashes.h"
#include "raygui.h"

class FloatingWindow
{
private:
    static constexpr int StatusBarHeight = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    static constexpr int CloseButtonSize = RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT;
private:
    Vector2 m_MinSize;
    Rectangle m_Bounds;
    bool m_Moving = false;
    bool m_Visible = true;
    bool m_Resizing = false;
    const char* m_Title;
    KeyboardKey m_ActivationKey = KEY_NULL;
private:
    void Move() noexcept
    {
        Vector2 mouseDelta = GetMouseDelta();
        m_Bounds.x += mouseDelta.x;
        m_Bounds.y += mouseDelta.y;

        if (IsMouseButtonReleased(MouseButton::MOUSE_BUTTON_LEFT))
        {
            m_Moving = false;

            // clamp window position keep it inside the application area
            if (m_Bounds.x < 0)
            {
                m_Bounds.x = 0;
            }
            else if (m_Bounds.x > GetScreenWidth() - m_Bounds.width)
            {
                m_Bounds.x = GetScreenWidth() - m_Bounds.width;
            }

            if (m_Bounds.y < 0)
            {
                m_Bounds.y = 0;
            }
            else if (m_Bounds.y > GetScreenHeight())
            {
                m_Bounds.y = GetScreenHeight() - StatusBarHeight;
            }
        }
    }

    void Resize() noexcept
    {
        Vector2 mouse = GetMousePosition();
        if (mouse.x > m_Bounds.x)
        {
            m_Bounds.width = mouse.x - m_Bounds.x;
        }
        if (mouse.y > m_Bounds.y)
        {
            m_Bounds.height = mouse.y - m_Bounds.y;
        }

        // clamp window size to a minimum value and the window size as the maximum
        if (m_Bounds.width < m_MinSize.x)
        {
            m_Bounds.width = m_MinSize.x;
        }
        else if (m_Bounds.width > GetScreenWidth())
        {
            m_Bounds.width = GetScreenWidth();
        }

        if (m_Bounds.height < m_MinSize.y)
        {
            m_Bounds.height = m_MinSize.y;
        }
        else if (m_Bounds.height > GetScreenHeight())
        {
            m_Bounds.height = GetScreenHeight();
        }

        if (IsMouseButtonReleased(MouseButton::MOUSE_BUTTON_LEFT))
        {
            m_Resizing = false;
        }
    }

    void Update() noexcept
    {
        int closeTitleSizeDeltaHalf = (StatusBarHeight - CloseButtonSize) / 2; // that my status bar collision wont be move when clicking the most left position of the close button
        if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT) && !m_Moving && !m_Resizing)
        {
            const Vector2 mousePosition = GetMousePosition();

            Rectangle titleCollisionRect = { m_Bounds.x, m_Bounds.y, m_Bounds.width - (CloseButtonSize + closeTitleSizeDeltaHalf), StatusBarHeight };
            Rectangle resizeCollisionRect = { m_Bounds.x + m_Bounds.width - 20, m_Bounds.y + m_Bounds.height - 20, 20, 20 };

            if (CheckCollisionPointRec(mousePosition, titleCollisionRect))
            {
                m_Moving = true;
            }
            else if (CheckCollisionPointRec(mousePosition, resizeCollisionRect))
            {
                m_Resizing = true;
            }
        }

        if (m_Moving)
            Move();
        else if (m_Resizing)
            Resize();
    }
public:
    FloatingWindow(int x, int y, int width, int height, const char* title, KeyboardKey activationKey, int minWidth, int minHeight) noexcept
        : m_Bounds(Rectangle{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) }), m_Title(title), m_ActivationKey(activationKey), m_MinSize({ static_cast<float>(minWidth), static_cast<float>(minHeight) })
    {
        GuiLoadStyleDark();
    }

    ~FloatingWindow() noexcept
    {
        // TODO implement cleanup for GuiLoadStyle() -- see raylib header for reference
    }

    Rectangle ToWindowSpace(const Rectangle& rect) const noexcept
    {
        return { m_Bounds.x + rect.x, m_Bounds.y + rect.y + StatusBarHeight, rect.width, rect.height };
    }

    Rectangle ToWindowSpace(int x, int y, int width, int height) const noexcept
    {
        return { m_Bounds.x + x, m_Bounds.y + y + StatusBarHeight, static_cast<float>(width), static_cast<float>(height) };
    }

    void Show() noexcept
    {
        if (IsKeyPressed(m_ActivationKey))
        {
            m_Visible = !m_Visible;

            if (Visible())
                EnableCursor();
            else
                DisableCursor();
        }

        if (Visible())
        {
            Update();
            m_Visible = !GuiWindowBox({ m_Bounds.x, m_Bounds.y, m_Bounds.width, m_Bounds.height }, m_Title);
            GuiDrawIcon(ICON_CURSOR_SCALE_LEFT_FILL, m_Bounds.x + m_Bounds.width - 20, m_Bounds.y + m_Bounds.height - 20, 1, WHITE);
        }
    }

    bool Visible() const noexcept
    {
        return m_Visible;
    }
};


class SettingsWindow : public FloatingWindow
{
private:
    int m_SelectedSimulationMode = 0;
    bool m_SimulationModeDropdownEditMode = false;
public:
    SettingsWindow() : FloatingWindow(20, 20, 500, 500, "Settings", KEY_F1, 200, 200) {}

    void Draw() noexcept
    {
        FloatingWindow::Show();
        if (!Visible()) return;

        if (m_SimulationModeDropdownEditMode)
            GuiLock();

        GuiLabel(ToWindowSpace(10, 10, 150, 20), "Simulation algorithm");

        static bool edit = false;
        static int value = 15;
        GuiSpinner(ToWindowSpace(10, 50, 100, 20), NULL, &value, 10, 30, edit);
        GuiSpinner(ToWindowSpace(10, 70, 100, 20), NULL, &value, 10, 30, edit);

        GuiUnlock();
        if (GuiDropdownBox(ToWindowSpace(10, 30, 170, 20), "Euler integration;Velocity Verlet algorithm;Runge-Kutta 4th", &m_SelectedSimulationMode, (int)m_SimulationModeDropdownEditMode))
            m_SimulationModeDropdownEditMode = !m_SimulationModeDropdownEditMode;
    }
};