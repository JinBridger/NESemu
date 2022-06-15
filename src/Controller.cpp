#include <Controller.h>


Controller::Controller() : 
    m_buttonStates(0),
    m_keyBindings(TotalButtons)
{

}

void Controller::SetKeyBindings(const std::vector<sf::Keyboard::Key> &keys)
{
    m_keyBindings = keys;
}


void Controller::Write(Byte b)
{
    m_strobe = (b & 1 );
    if (!m_strobe)
    {
        m_buttonStates = 0;
        int shift = 0;
        for (int button = A; button < TotalButtons; ++button)
        {
            // 读按键状态
            m_buttonStates |= (sf::Keyboard::isKeyPressed(m_keyBindings[static_cast<Buttons>(button)]) << shift);
            ++shift;
        }
    }
}


Byte Controller::Read()
{
    Byte ret;
    if (m_strobe)
        ret = sf::Keyboard::isKeyPressed(m_keyBindings[A]);
    else 
    {
        ret = (m_buttonStates & 1);
        m_buttonStates >>= 1;
    }

    return ret | 0x40;
}