#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_
#include <SFML/Window.hpp>
#include <vector>

#include "Chip.h"

class Controller {
 public:
  Controller();
  enum Buttons {
    A,
    B,
    Select,
    Start,
    Up,
    Down,
    Left,
    Right,
    TotalButtons,
  };
  void Write(Byte b);

  Byte Read();

  void SetKeyBindings(const std::vector<sf::Keyboard::Key> &keys);

 private:
  bool m_strobe;
  unsigned int m_buttonStates;

  std::vector<sf::Keyboard::Key> m_keyBindings;
};

#endif /* _CONTROLLER_H_ */