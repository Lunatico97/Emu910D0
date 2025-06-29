#include <alu.hpp>

ALU::ALU(MMU& mmui): mmu(mmui) {}

void ALU::update_flags()
{
    u8 bit;
    bool parity = false;
    for(u8 i=0x00; i<0x08; i++)
    {
        bit = (ACC & (0x01 << i)) >> i;
        if(i == 0x07 && bit == 0x01) SF |= HX_SIGN; 
        if(bit == 0x01) parity = !parity;
    }

    if(ACC == 0x00) SF |= HX_ZERO;
    if(parity) SF |= HX_PARY;
}

void ALU::add(REG r)
{
    fetch(r);
    ACC += TEMP;
    update_flags();
    load();
}

void ALU::adi(u8 value)
{
    fetch(value);
    ACC += TEMP;
    update_flags();
    load();
}

void ALU::sub(REG r)
{
    fetch(r);
    ACC -= TEMP;
    update_flags();
    load();
}

void ALU::sui(u8 value)
{
    fetch(value);
    ACC -= TEMP;
    update_flags();
    load();
}

void ALU::ana(REG r)
{
    fetch(r);
    ACC &= TEMP;
    update_flags();
    load();
}

void ALU::ani(u8 value)
{
    fetch(value);
    ACC &= TEMP;
    update_flags();
    load();
}

void ALU::ora(REG r)
{
    fetch(r);
    ACC |= TEMP;
    update_flags();
    load();
}

void ALU::ori(u8 value)
{
    fetch(value);
    ACC |= TEMP;
    update_flags();
    load();
}

void ALU::xra(REG r)
{
    fetch(r);
    ACC ^= TEMP;
    update_flags();
    load();
}

void ALU::xri(u8 value)
{
    fetch(value);
    ACC ^= TEMP;
    update_flags();
    load();
}

void ALU::cmp(REG r)
{
    fetch(r);
    if(ACC < TEMP) { SF |= HX_CARY; SF &= ~HX_ZERO; /* A < R => CY = 1, Z = 0 */ }
    else if(ACC == TEMP) { SF &= ~HX_CARY; SF |= HX_ZERO; /* A = R => CY = 0, Z = 1 */ }
    else { SF &= ~(HX_CARY | HX_ZERO); /* A > R => CY = 0, Z = 0 */ }
    load();
}

void ALU::cpi(u8 value)
{
    fetch(value);
    if(ACC < TEMP) { SF |= HX_CARY; SF &= ~HX_ZERO; /* A < R => CY = 1, Z = 0 */ }
    else if(ACC == TEMP) { SF &= ~HX_CARY; SF |= HX_ZERO; /* A = R => CY = 0, Z = 1 */ }
    else { SF &= ~(HX_CARY | HX_ZERO); /* A > R => CY = 0, Z = 0 */ }
    load();
}

void ALU::fetch(u8 value)
{
    ACC = mmu.tapREG(A);
    SF = mmu.tapREG(ST);
    TEMP = value;
}

void ALU::fetch(REG r)
{
    ACC = mmu.tapREG(A);
    SF = mmu.tapREG(ST);
    TEMP = mmu.tapREG(r); 
}

void ALU::load()
{
    mmu.ld(A, ACC);
    mmu.ld(ST, SF);
}
