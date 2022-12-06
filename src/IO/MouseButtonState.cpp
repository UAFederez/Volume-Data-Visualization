#include "MouseButtonState.h"

void MouseButtonState::HandleInput(GLFWwindow* window)
{
    int state = glfwGetMouseButton(window, m_mouseButton);
    glfwGetCursorPos(window, &m_currXPos, &m_currYPos);

    m_isPressed   = (state == GLFW_PRESS);
    m_wasReleased = (!m_isPressed && m_wasPressed);
    m_isDragging  = (!m_wasReleased && m_isDragging);
    m_wasDragging = (m_wasReleased && !m_isDragging && !m_wasDragging);

    if (m_isPressed)
    {
        double deltaX = m_currXPos - m_prevXPos;
        double deltaY = m_currYPos - m_prevYPos;

        if (!m_isDragging && (deltaX != 0 || deltaY != 0))
        {
            m_isDragging = true;
            m_dragStartX = m_prevXPos;
            m_dragStartY = m_prevYPos;
        }

        m_dragVecX = m_currXPos - m_dragStartX;
        m_dragVecY = m_currYPos - m_dragStartY;
    }

    m_prevXPos   = m_currXPos;
    m_prevYPos   = m_currYPos;
    m_wasPressed = m_isPressed;
}
