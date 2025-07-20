# E910D0
Building a NES Emulator

![E910D0](rsrc/snip.png)

Commands:
- NUM_7 -> STEP (Stepwise Execute)
- NUM_8 -> IRQ (Interrupt Request)
- NUM_9 -> NMI (Non-Maskable Interrupt)
- NUM_0 -> RST (Reset)

Legend:
- White block (Zero page memory block <$0x0000 - $0x00FF>)
- Blue block (Bottom stack <$0x01F0 - $0x01FF>)
- Top right block 
( Status Flags | Yellow block - General Purpose Registers | Magenta block - Special Purpose Registers)

![Sprite Loaded From Cartridge](rsrc/snip2.gif)

(Note: No PPU operation is involved here yet.)

