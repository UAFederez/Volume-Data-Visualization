#pragma once

#include <GLFW/glfw3.h>

struct MouseButtonState
{
private:
    int  m_mouseButton = GLFW_MOUSE_BUTTON_LEFT;
    bool m_wasPressed  = false;
    bool m_isPressed   = false;
    bool m_wasReleased = false;
    bool m_wasDragging = false;
    bool m_isDragging  = false;

    double m_currXPos = 0.0, m_currYPos = 0.0;
    double m_prevXPos = 0.0, m_prevYPos = 0.0;

    double m_deltaX = 0.0f;
    double m_deltaY = 0.0f;

    double m_dragStartX = 0.0, m_dragStartY = 0.0;
    double m_dragVecX   = 0.0, m_dragVecY   = 0.0;
public:

    double GetButtonX()  const { return m_currXPos; }
    double GetButtonY()  const { return m_currYPos; }

    double GetDragVecX() const { return m_dragVecX; }
    double GetDragVecY() const { return m_dragVecY; }

    bool WasDragged()  const { return m_wasDragging; }
    bool WasReleased() const { return m_wasReleased; }
    bool IsPressed()   const { return m_isPressed;   }
    bool IsDragging()  const { return m_isDragging;  }

    MouseButtonState(int button) :
        m_mouseButton(button)
    {

    }

    void HandleInput(GLFWwindow* window);
};