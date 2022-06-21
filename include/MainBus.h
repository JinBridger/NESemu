#ifndef _MAIN_BUS_H_
#define _MAIN_BUS_H_

#include <map>
#include <vector>

#include "Cartridge.h"
#include "Chip.h"
#include "Mapper.h"

enum IORegisters {
  PPUCTRL = 0x2000,
  PPUMASK,
  PPUSTATUS,
  OAMADDR,
  OAMDATA,
  PPUSCROL,
  PPUADDR,
  PPUDATA,
  OAMDMA = 0x4014,
  JOY1 = 0x4016,
  JOY2 = 0x4017,
};

class MainBus {
 public:
  MainBus();

  bool SetMapper(Mapper* mapper);

  Byte Read(Address addr);
  void Write(Address addr, Byte value);

  const Byte* GetPagePtr(Byte page);
  bool SetWriteCallback(IORegisters reg, std::function<void(Byte)> callback);
  bool SetReadCallback(IORegisters reg, std::function<Byte(void)> callback);

 private:
  std::vector<Byte> m_RAM;
  std::vector<Byte> m_extRAM;
  Mapper* m_mapper;
  std::map<IORegisters, std::function<void(Byte)>> m_writeCallbacks;
  std::map<IORegisters, std::function<Byte(void)>> m_readCallbacks;
};

#endif