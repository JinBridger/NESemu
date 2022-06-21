#ifndef _MAPPER_H_
#define _MAPPER_H_

#include <functional>
#include <memory>

#include "Cartridge.h"

enum NameTableMirroring {
  Horizontal = 0,
  Vertical = 1,
  FourScreen = 8,
  OneScreenLower,
  OneScreenHigher,
};

class Mapper {
 public:
  enum Type {
    NROM = 0,
    SxROM = 1,
    UxROM = 2,
    CNROM = 3,
  };
  Mapper(Cartridge& cart, Type t) : m_cartridge(cart), m_type(t){};
  /* 虚函数 */
  virtual void WritePRG(Address addr, Byte value) = 0;
  virtual Byte ReadPRG(Address addr) = 0;

  virtual Byte ReadCHR(Address addr) = 0;
  virtual void WriteCHR(Address addr, Byte value) = 0;
  virtual NameTableMirroring GetNameTableMirroring();

  bool inline HasExtendedRAM() { return m_cartridge.HasExtendedRAM(); }
  static std::unique_ptr<Mapper> CreateMapper(
      Type mapper_t, Cartridge& cart,
      std::function<void(void)> mirroring_cb = nullptr);
  virtual ~Mapper() = default;

 protected:
  Cartridge& m_cartridge;
  Type m_type;
};

#endif