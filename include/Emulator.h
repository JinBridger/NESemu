#ifndef _EMULATOR_H_
#define _EMULATOR_H_
#include <SFML/Graphics.hpp>
#include <chrono>

#include "CPU.h"
#include "Cartridge.h"
#include "Controller.h"
#include "MainBus.h"
#include "Mapper.h"
#include "PPU.h"
#include "PictureBus.h"
#include "VirtualScreen.h"

const int NESVideoWidth = ScanlineVisibleDots;
const int NESVideoHeight = VisibleScanlines;

class Emulator {
 public:
  Emulator();
  void Run(std::string rom_path);

 private:
  void DMA(Byte page);
  CPU m_cpu;
  PPU m_ppu;
  PictureBus m_pictureBus;
  VirtualScreen m_emulatorScreen;

  Cartridge m_cartridge;
  MainBus m_bus;
  std::unique_ptr<Mapper> m_mapper;
  sf::RenderWindow m_window;

  Controller m_controller1, m_controller2;

  float m_screenScale;

  std::chrono::high_resolution_clock::time_point m_cycleTimer;
  std::chrono::high_resolution_clock::duration m_elapsedTime;
  std::chrono::nanoseconds m_cpuCycleDuration;
};

#endif