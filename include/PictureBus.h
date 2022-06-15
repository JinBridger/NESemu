#ifndef _PICTURE_BUS_H_
#define _PICTURE_BUS_H_
#include<Cartridge.h>
#include <Mapper.h>

class PictureBus
{
    public:
        PictureBus();
        Byte Read(Address addr);
        void Write(Address addr, Byte value);
        void UpdateMirroring();
        bool SetMapper(Mapper *mapper);
        Byte ReadPalette(Byte paletteAddr);
    private:
        std::vector<Byte> m_RAM;
        std::size_t m_NameTable0, m_NameTable1, m_NameTable2, m_NameTable3;

        std::vector<Byte> m_palette;

        Mapper *m_mapper;

};

#endif /* _PICTURE_BUS_H_ */