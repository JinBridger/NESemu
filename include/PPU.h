#ifndef _PPU_H_
#define _PPU_H_
#include <Chip.h>
#include <PictureBus.h>
#include <VirtualScreen.h>

#include <vector>

// Some const data
const int ScanlineCycleLength = 341;
const int ScanlineEndCycle = 340;
const int ScanlineVisibleDots = 256;
const int VisibleScanlines = 240;
const int FrameEndScanline = 261;
const int AttributeOffset = 0x3C0;

class PPU {
 public:
  PPU(PictureBus &bus, VirtualScreen &screen);

  void Reset();
  void Step();
  void SetInterruptCallback(std::function<void(void)> cb);
  /*OAM (Object Attribute Memory) */

  // IO Resisters

  void SetMask(Byte mask);
  Byte GetStatus();
  void SetDataAddress(Byte addr);
  Byte GetData();
  void SetData(Byte data);
  Byte GetOAMData();
  void Control(Byte ctrl);
  void SetOAMAddress(Byte addr);
  void SetOAMData(Byte value);
  void SetScroll(Byte scroll);
  void DoDMA(const Byte *page_ptr);

 private:
  Byte Read(Address addr);
  Byte ReadOAM(Byte addr);
  void WriteOAM(Byte addr, Byte value);
  PictureBus &m_bus;
  VirtualScreen &m_screen;

  std::vector<Byte> m_spriteMemory;
  std::vector<Byte> m_scanlineSprites;

  enum PipelineState {
    PreRender,
    Render,
    PostRender,
    VerticalBlank
  } m_pipelineState;

  int m_cycle;
  int m_scanline;
  bool m_evenFrame;

  bool m_vblank;
  bool m_sprZeroHit;

  std::function<void(void)> m_vblankCallback;

  // Some resisters
  // Register V, T, X, W
  // From: https://www.nesdev.org/wiki/PPU_scrolling

  Address m_reg_v;
  Address m_reg_t;
  Byte m_reg_x;
  bool m_reg_w;

  // Some registers

  Byte m_dataBuffer;
  Byte m_spriteDataAddress;

  // IO Resister data
  // Merged to one byte when IO Register is called

  bool m_longSprites;
  bool m_generateInterrupt;
  bool m_greyscaleMode;
  bool m_showSprites;
  bool m_showBackground;
  bool m_hideEdgeSprites;
  bool m_hideEdgeBackground;

  enum CharacterPage {
    Low,
    High,
  } m_bgPage,
      m_sprPage;

  Address m_dataAddrIncrement;

  std::vector<std::vector<sf::Color>> m_pictureBuffer;
};

#endif /* _PPU_H_ */