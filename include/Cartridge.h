#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include <cstdint>
#include <string>
#include <vector>

#include "Chip.h"

class Cartridge {
 public:
  Cartridge();
  bool LoadFromFile(std::string path);
  const std::vector<Byte>& GetROM();
  const std::vector<Byte>& GetVROM();
  bool HasExtendedRAM();
  Byte GetMapper();
  Byte GetNameTableMirroring();

 private:
  std::vector<Byte> m_PRG_ROM;
  std::vector<Byte> m_CHR_ROM;

  Byte m_nameTableMirroring;
  Byte m_mapperNumber;
  bool m_extendedRAM;
};

#endif