#include <PictureBus.h>
#include <Log.h>


PictureBus::PictureBus() : 
    m_RAM(0x800),
    m_palette(0x20),
    m_mapper(nullptr)
{

}

Byte PictureBus::Read(Address addr)
{
    if(addr < 0x2000)
    {
        return m_mapper->ReadCHR(addr);
    }
    else if (addr < 0x3eff)
    {
        auto index = addr & 0x3ff;
        if (addr < 0x2400)      //NT0
            return m_RAM[m_NameTable0 + index];
        else if (addr < 0x2800) //NT1
            return m_RAM[m_NameTable1 + index];
        else if (addr < 0x2c00) //NT2
            return m_RAM[m_NameTable2 + index];
        else                    //NT3
            return m_RAM[m_NameTable3 + index];
    }
    else if (addr < 0x3fff)
    {
        return m_palette[addr & 0x1f];
    }
    return 0;
}

Byte PictureBus::ReadPalette(Byte platetteAddr)
{
    return m_palette[platetteAddr];
}

void PictureBus::Write(Address addr, Byte value)
{
    if (addr < 0x2000)
    {
        m_mapper->WriteCHR(addr, value);
    }
    else if (addr < 0x3eff) //Name tables upto 0x3000, then mirrored upto 3eff
    {
        auto index = addr & 0x3ff;
        if (addr < 0x2400)      //NT0
            m_RAM[m_NameTable0 + index] = value;
        else if (addr < 0x2800) //NT1
            m_RAM[m_NameTable1 + index] = value;
        else if (addr < 0x2c00) //NT2
            m_RAM[m_NameTable2 + index] = value;
        else                    //NT3
            m_RAM[m_NameTable3 + index] = value;
    }
    else if (addr < 0x3fff)
    {
        if (addr == 0x3f10)
            m_palette[0] = value;
        else
            m_palette[addr & 0x1f] = value;
    }
}


void PictureBus::UpdateMirroring()
{
    switch (m_mapper->GetNameTableMirroring())
    {
        case Horizontal:
            m_NameTable0 = m_NameTable1 = 0;
            m_NameTable2 = m_NameTable3 = 0x400;
            LOG(InfoVerbose) << "Horizontal Name Table mirroring set. (Vertical Scrolling)" << std::endl;
            break;
        case Vertical:
            m_NameTable0 = m_NameTable2 = 0;
            m_NameTable1 = m_NameTable3 = 0x400;
            LOG(InfoVerbose) << "Vertical Name Table mirroring set. (Horizontal Scrolling)" << std::endl;
            break;
        case OneScreenLower:
            m_NameTable0 = m_NameTable1 = m_NameTable2 = m_NameTable3 = 0;
            LOG(InfoVerbose) << "Single Screen mirroring set with lower bank." << std::endl;
            break;
        case OneScreenHigher:
            m_NameTable0 = m_NameTable1 = m_NameTable2 = m_NameTable3 = 0x400;
            LOG(InfoVerbose) << "Single Screen mirroring set with higher bank." << std::endl;
            break;
        default:
            m_NameTable0 = m_NameTable1 = m_NameTable2 = m_NameTable3 = 0;
            LOG(Error) << "Unsupported Name Table mirroring : " << m_mapper->GetNameTableMirroring() << std::endl;
    }
}

bool PictureBus::SetMapper(Mapper *mapper)
{
    if (!mapper)
    {
        LOG(Error) << "Mapper argument is nullptr" << std::endl;
        return false;
    }
    m_mapper = mapper;
    UpdateMirroring();
    return true;
}