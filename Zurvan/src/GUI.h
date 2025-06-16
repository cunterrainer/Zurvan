#pragma once
#include <array>
#include <cstddef>

// needed because otherwise raygui will define them internally
#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
#define RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT 18

#include "raygui.h"

#include "Renderer.h"

class FloatingWindow
{
private:
    static constexpr int StatusBarHeight = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    static constexpr int CloseButtonSize = RAYGUI_WINDOWBOX_CLOSEBUTTON_HEIGHT;
private:
    Vector2 m_MinSize;
    Rectangle m_Bounds;
    bool m_Moving = false;
    bool m_Visible = false;
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
        : m_MinSize({ static_cast<float>(minWidth), static_cast<float>(minHeight) }), m_Bounds(Rectangle{ static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) }), m_Title(title), m_ActivationKey(activationKey)
    {
        constexpr std::array<GuiStyleProp, 23> darkStyleProps = {{
            { 0, 0, (int)0x878787ff },    // DEFAULT_BORDER_COLOR_NORMAL 
            { 0, 1, (int)0x2c2c2cff },    // DEFAULT_BASE_COLOR_NORMAL 
            { 0, 2, (int)0xc3c3c3ff },    // DEFAULT_TEXT_COLOR_NORMAL 
            { 0, 3, (int)0xe1e1e1ff },    // DEFAULT_BORDER_COLOR_FOCUSED 
            { 0, 4, (int)0x848484ff },    // DEFAULT_BASE_COLOR_FOCUSED 
            { 0, 5, (int)0x181818ff },    // DEFAULT_TEXT_COLOR_FOCUSED 
            { 0, 6, (int)0x000000ff },    // DEFAULT_BORDER_COLOR_PRESSED 
            { 0, 7, (int)0xefefefff },    // DEFAULT_BASE_COLOR_PRESSED 
            { 0, 8, (int)0x202020ff },    // DEFAULT_TEXT_COLOR_PRESSED 
            { 0, 9, (int)0x6a6a6aff },    // DEFAULT_BORDER_COLOR_DISABLED 
            { 0, 10, (int)0x818181ff },    // DEFAULT_BASE_COLOR_DISABLED 
            { 0, 11, (int)0x606060ff },    // DEFAULT_TEXT_COLOR_DISABLED 
            { 0, 16, (int)0x00000010 },    // DEFAULT_TEXT_SIZE 
            { 0, 17, (int)0x00000000 },    // DEFAULT_TEXT_SPACING 
            { 0, 18, (int)0x9d9d9dff },    // DEFAULT_LINE_COLOR 
            { 0, 19, (int)0x3c3c3cff },    // DEFAULT_BACKGROUND_COLOR 
            { 0, 20, (int)0x00000018 },    // DEFAULT_TEXT_LINE_SPACING 
            { 1, 5, (int)0xf7f7f7ff },    // LABEL_TEXT_COLOR_FOCUSED 
            { 1, 8, (int)0x898989ff },    // LABEL_TEXT_COLOR_PRESSED 
            { 4, 5, (int)0xb0b0b0ff },    // SLIDER_TEXT_COLOR_FOCUSED 
            { 5, 5, (int)0x848484ff },    // PROGRESSBAR_TEXT_COLOR_FOCUSED 
            { 9, 5, (int)0xf5f5f5ff },    // TEXTBOX_TEXT_COLOR_FOCUSED 
            { 10, 5, (int)0xf6f6f6ff },    // VALUEBOX_TEXT_COLOR_FOCUSED 
        }};

        // Load style properties provided
        // NOTE: Default properties are propagated
        for (const auto& prop : darkStyleProps)
        {
            GuiSetStyle(prop.controlId, prop.propertyId, prop.propertyValue);
        }

        GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
        GuiSetFont(Renderer::GetFontUI());

        // Setup a white rectangle on the font to be used on shapes drawing,
        // it makes possible to draw shapes and text (full UI) in a single draw call
        Rectangle fontWhiteRec = { 510, 254, 1, 1 };
        SetShapesTexture(Renderer::GetFontUI().texture, fontWhiteRec);
    }

    ~FloatingWindow() noexcept = default;

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

    int m_RenderDistanceScale = 700000000; // 1 px = 700,000,000 meters
    bool m_RenderDistanceScaleEditMode = false;

    int m_RenderRadiusScale = 1e6; // 1 px = 1,000,000 meters
    bool m_RenderRadiusScaleEditMode = false;

    int m_SimulationRate = 1000;
    bool m_SimulationRateEditMode = false;
public:
    SettingsWindow() : FloatingWindow(20, 20, 500, 500, "Settings", KEY_F1, 200, 200) {}

    float GetRenderDistanceScale() const noexcept
    {
        return static_cast<float>(m_RenderDistanceScale);
    }

    float GetRenderRadiusScale() const noexcept
    {
        return static_cast<float>(m_RenderRadiusScale);
    }

    void Draw() noexcept
    {
        FloatingWindow::Show();
        if (!Visible()) return;

        if (m_SimulationModeDropdownEditMode)
            GuiLock();

        GuiLabel(ToWindowSpace(10, 10, 150, 20), "Simulation algorithm");

        const int renderDistanceScale = m_RenderDistanceScale;
        GuiSpinner(ToWindowSpace(10, 50, 150, 20), NULL, &m_RenderDistanceScale, 1e8, 1e9, m_RenderDistanceScaleEditMode);
        GuiLabel(ToWindowSpace(160, 50, 50, 20), "Render distance scale");
        if (renderDistanceScale < m_RenderDistanceScale)
        {
            m_RenderDistanceScale = renderDistanceScale + 1e8;
        }
        else if (renderDistanceScale > m_RenderDistanceScale)
        {
            m_RenderDistanceScale = renderDistanceScale - 1e8;
        }

        const int renderRadiusScale = m_RenderRadiusScale;
        GuiSpinner(ToWindowSpace(10, 70, 150, 20), NULL, &m_RenderRadiusScale, 1e5, 1e7, m_RenderRadiusScaleEditMode);
        if (renderRadiusScale < m_RenderRadiusScale)
        {
            m_RenderRadiusScale = renderRadiusScale + 1e5;
        }
        else if (renderRadiusScale > m_RenderRadiusScale)
        {
            m_RenderRadiusScale = renderRadiusScale - 1e5;
        }

        GuiUnlock();
        if (GuiDropdownBox(ToWindowSpace(10, 30, 170, 20), "Euler integration;Velocity Verlet algorithm;Runge-Kutta 4th", &m_SelectedSimulationMode, (int)m_SimulationModeDropdownEditMode))
            m_SimulationModeDropdownEditMode = !m_SimulationModeDropdownEditMode;
    }
};