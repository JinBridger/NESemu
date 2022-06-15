#include <MainBus.h>
#include <iostream>
#include <Log.h>
/*  0x800 = 2KB */
MainBus::MainBus() : m_RAM(0x800, 0)
{
}


bool MainBus::SetMapper(Mapper *mapper)
{
    m_mapper = mapper;

    if (!mapper)
    {
        LOG(Error) << "Mapper pointer is nullptr" << std::endl;
        return false;
    }

    if(mapper->HasExtendedRAM())
        m_extRAM.resize(0x2000);
    
    return true;
}

Byte MainBus::Read(Address addr)
{

    if (addr < 0x2000)
    {
        return m_RAM[addr & 0x7ff];
    }
    else if(addr < 0x4020)
    {

        if (addr < 0x4000)
        {
            auto it = m_readCallbacks.find(static_cast<IORegisters>(addr & 0x2007));
            if (it != m_readCallbacks.end())
            {
                return (it->second)();
            }
            else 
            {
                LOG(InfoVerbose) << "No read callback registered for I/O register at: " << std::hex << +addr << std::endl;
            }
        }
        else if (addr < 0x4018 && addr >= 0x4014)
        {
            auto it = m_readCallbacks.find(static_cast<IORegisters>(addr));
            if (it != m_readCallbacks.end())
                return (it -> second)();
            else
                LOG(InfoVerbose) << "No read callback registered for I/O register at: " << std::hex << +addr << std::endl;
        }
        else 
        {
            LOG(InfoVerbose) << "Read access attempt at: " << std::hex << +addr << std::endl;
        }
    }
    else if (addr < 0x6000)
    {
        LOG(InfoVerbose) << "Expansion ROM read attempted. This is currently unsupported" << std::endl;
    }
    else if ( addr < 0x8000)
    {
        if (m_mapper->HasExtendedRAM())
        {
            return m_extRAM[addr - 0x6000];
        }
    }
    else 
    {
        return m_mapper->ReadPRG(addr);
    }
    return 0;
}

void MainBus::Write(Address addr, Byte value)
{
    if (addr < 0x2000)
    {
        m_RAM[addr & 0x7ff] = value;
    }
    else if (addr < 0x4020)
    {
        if (addr < 0x4000) //PPU registers, mirrored
        {
            auto it = m_writeCallbacks.find(static_cast<IORegisters>(addr & 0x2007));
            if (it != m_writeCallbacks.end())
                (it -> second)(value);

            else
                LOG(InfoVerbose) << "No write callback registered for I/O register at: " << std::hex << +addr << std::endl;
        }
        else if (addr < 0x4017 && addr >= 0x4014) //only some registers
        {
            auto it = m_writeCallbacks.find(static_cast<IORegisters>(addr));
            if (it != m_writeCallbacks.end())
                (it -> second)(value);

            else
                LOG(InfoVerbose) << "No write callback registered for I/O register at: " << std::hex << +addr << std::endl;
        }
        else
            LOG(InfoVerbose) << "Write access attmept at: " << std::hex << +addr << std::endl;
    }
    else if (addr < 0x6000)
    {
        LOG(InfoVerbose) << "Expansion ROM access attempted. This is currently unsupported" << std::endl;
    }
    else if (addr < 0x8000)
    {
        if (m_mapper->HasExtendedRAM())
        {
            m_extRAM[addr - 0x6000] = value;
        }
    }
    else
    {
        m_mapper->WritePRG(addr, value);
    }
}

bool MainBus::SetWriteCallback(IORegisters reg, std::function<void(Byte)> callback)
{
    if (!callback)
    {
        LOG(Error) << "callback argument is nullptr" << std::endl;
        return false;
    }
    return m_writeCallbacks.emplace(reg, callback).second;
}

bool MainBus::SetReadCallback(IORegisters reg, std::function<Byte(void)> callback)
{
    if (!callback)
    {
        LOG(Error) << "callback argument is nullptr" << std::endl;
        return false;
    }
    return m_readCallbacks.emplace(reg, callback).second;
}


const Byte* MainBus::GetPagePtr(Byte page)
{
    Address addr = page << 8;
    if (addr < 0x2000)
        return &m_RAM[addr & 0x7ff];
    else if (addr < 0x4020)
    {
        LOG(Error) << "Register address memory pointer access attempt" << std::endl;
    }
    else if (addr < 0x6000)
    {
        LOG(Error) << "Expansion ROM access attempted, which is unsupported" << std::endl;
    }
    else if (addr < 0x8000)
    {
        if (m_mapper->HasExtendedRAM())
        {
            return &m_extRAM[addr - 0x6000];
        }
    }
    else 
    {

    }
    return nullptr;
}