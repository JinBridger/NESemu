#include <Emulator.h>
#include <Log.h>

/*
A      = J
B      = K
Select = RShift
Start  = Return
Up     = W
Down   = S
Left   = A
Right  = D
*/
std::vector<sf::Keyboard::Key> con_one = { 
    sf::Keyboard::Key::J, 
    sf::Keyboard::Key::K, 
    sf::Keyboard::Key::RShift,
    sf::Keyboard::Key::Return,
    sf::Keyboard::Key::W,
    sf::Keyboard::Key::S,
    sf::Keyboard::Key::A,
    sf::Keyboard::Key::D,
};

/*
[Player2]
A      = Numpad5
B      = Numpad6
Select = Numpad8
Start  = Numpad9
Up     = Up
Down   = Down
Left   = Left
Right  = Right
*/
std::vector<sf::Keyboard::Key> con_two = { 
    sf::Keyboard::Key::Numpad5, 
    sf::Keyboard::Key::Numpad6, 
    sf::Keyboard::Key::Numpad8,
    sf::Keyboard::Key::Numpad9,
    sf::Keyboard::Key::Up,
    sf::Keyboard::Key::Down,
    sf::Keyboard::Key::Left,
    sf::Keyboard::Key::Right,
};
Emulator::Emulator(): 
    m_cpu(m_bus),
    m_screenScale(2.f),
    m_ppu(m_pictureBus, m_emulatorScreen),
    m_cycleTimer(),
    m_cpuCycleDuration(std::chrono::nanoseconds(559))
{
    if(!m_bus.SetReadCallback(PPUSTATUS, [&](void) {return m_ppu.GetStatus();}) ||
        !m_bus.SetReadCallback(PPUDATA, [&](void) {return m_ppu.GetData();}) ||
        !m_bus.SetReadCallback(JOY1, [&](void) {return m_controller1.Read();}) ||
        !m_bus.SetReadCallback(JOY2, [&](void) {return m_controller2.Read();}) ||
        !m_bus.SetReadCallback(OAMDATA, [&](void) {return m_ppu.GetOAMData();}))
    {
        LOG(Error) << "Critical error: Failed to set I/O callbacks" << std::endl;
    }

    if(!m_bus.SetWriteCallback(PPUCTRL, [&](Byte b) {m_ppu.Control(b);}) ||
        !m_bus.SetWriteCallback(PPUMASK, [&](Byte b) {m_ppu.SetMask(b);}) ||
        !m_bus.SetWriteCallback(OAMADDR, [&](Byte b) {m_ppu.SetOAMAddress(b);}) ||
        !m_bus.SetWriteCallback(PPUADDR, [&](Byte b) {m_ppu.SetDataAddress(b);}) ||
        !m_bus.SetWriteCallback(PPUSCROL, [&](Byte b) {m_ppu.SetScroll(b);}) ||
        !m_bus.SetWriteCallback(PPUDATA, [&](Byte b) {m_ppu.SetData(b);}) ||
        !m_bus.SetWriteCallback(OAMDMA, [&](Byte b) {DMA(b);}) ||
        !m_bus.SetWriteCallback(JOY1, [&](Byte b) {m_controller1.Write(b); m_controller2.Write(b);}) ||
        !m_bus.SetWriteCallback(OAMDATA, [&](Byte b) {m_ppu.SetOAMData(b);}))
    {
        LOG(Error) << "Critical error: Failed to set I/O callbacks" << std::endl;
    }
    m_ppu.SetInterruptCallback([&](){ m_cpu.Interrupt(CPU::NMI); });
    m_controller1.SetKeyBindings(con_one);
    m_controller2.SetKeyBindings(con_two);
}
void Emulator::Run(std::string rom_path)
{
    if (!m_cartridge.LoadFromFile(rom_path))
    {
        LOG(Error) << "Unable to load ROM from file:" << rom_path << std::endl;
        return;
    }

    m_mapper = Mapper::CreateMapper(static_cast<Mapper::Type>(m_cartridge.GetMapper()),
                                    m_cartridge);

    if(!m_mapper)
    {
        LOG(Error) << "Creating Mapper failed. Probably unsupported." << std::endl;
        return;
    }
    if(!m_bus.SetMapper(m_mapper.get())|| 
        !m_pictureBus.SetMapper(m_mapper.get()))
    {
        return;
    }

    m_cpu.Reset();
    m_ppu.Reset();

    m_window.create(sf::VideoMode(NESVideoWidth * m_screenScale, NESVideoHeight * m_screenScale),
                    "NESemu", sf::Style::Titlebar | sf::Style::Close);
    m_window.setVerticalSyncEnabled(true);
    // sf::CircleShape shape(NESVideoWidth);
    // shape.setFillColor(sf::Color::Red);
    m_emulatorScreen.Create(NESVideoWidth, NESVideoHeight, m_screenScale, sf::Color::White);

    m_cycleTimer = std::chrono::high_resolution_clock::now();
    m_elapsedTime = m_cycleTimer - m_cycleTimer;
    sf::Event event;
    bool isFocus = true, isPause = false;

    while(m_window.isOpen())
    {
        while (m_window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                m_window.close();
                return;
            }
            else if (event.type == sf::Event::GainedFocus)
            {
                isFocus = true;
                m_cycleTimer = std::chrono::high_resolution_clock::now();
            }
            else if (event.type == sf::Event::LostFocus)
                isFocus = false;
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2)
            {
                isPause = !isPause;
                if (!isPause)
                    m_cycleTimer = std::chrono::high_resolution_clock::now();
            }
            else if (isPause && event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::F3)
            {
                for (int i = 0; i < 29781; ++i) //Around one frame
                {
                    //PPU
                    m_ppu.Step();
                    m_ppu.Step();
                    m_ppu.Step();
                    //CPU
                    m_cpu.Step();
                }
            }
                
        }

        if (isFocus && !isPause)
        {
            m_elapsedTime += std::chrono::high_resolution_clock::now() - m_cycleTimer;
            m_cycleTimer = std::chrono::high_resolution_clock::now();
            while (m_elapsedTime > m_cpuCycleDuration)
            {
                m_ppu.Step();
                m_ppu.Step();
                m_ppu.Step();

                m_cpu.Step();

                m_elapsedTime -= m_cpuCycleDuration;

            }
            m_window.draw(m_emulatorScreen);
            m_window.display();
        }
        else
        {
            sf::sleep(sf::milliseconds(1000/60));
        }
    }
}

void Emulator::DMA(Byte page)
{
    m_cpu.SkipDMACycles();
    auto page_ptr = m_bus.GetPagePtr(page);
    m_ppu.DoDMA(page_ptr);
}
