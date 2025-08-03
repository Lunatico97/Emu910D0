#include <mmu.hpp>

MMU::MMU(CardROM *cptr, PPU* pptr)
{
    rb.init();
    rb.loadREG(SP, 0xFF);
    mem = new Memory(cptr, pptr);
}

MMU::~MMU()
{
    delete mem;
}

void MMU::load_mem(u16 address, u8 value)
{
    mem->store(address, value);
}

u8 MMU::fetch_mem(u16 address)
{
    return mem->retreive(address);
}

void MMU::init_pc(u16 value)
{
    rb.loadPC(value);
}

void MMU::push(REG r)
{
    if(rb.fetchREG(SP) > 0x00)
    {
        mem->store(SP_INDEX | rb.fetchREG(SP), rb.fetchREG(r));
        rb.loadREG(SP, rb.fetchREG(SP)-0x01);
    }
    else throw(std::runtime_error("Stack overflow !"));
}

void MMU::pop(REG r)
{
    if(rb.fetchREG(SP) <= 0xFF)
    {
        rb.loadREG(SP, rb.fetchREG(SP)+0x01);
        rb.loadREG(r, mem->retreive(SP_INDEX | rb.fetchREG(SP)));
    }
    else throw(std::runtime_error("Stack underflow !"));
}

void MMU::tr(REG des, REG src)
{
    rb.loadREG(des, rb.fetchREG(src));
    if(des != SP) updf(des);
}

void MMU::ld(REG des, u8 value)
{
    rb.loadREG(des, value);
}

void MMU::ld(REG des, REG off, u16 addr)
{
    if(off != REG::NON) rb.loadREG(des, mem->retreive(addr + rb.fetchREG(off)));
    else rb.loadREG(des, mem->retreive(addr));
    updf(des);
}

void MMU::st(REG src, REG off, u16 addr)
{
    if(off != REG::NON) mem->store(addr + rb.fetchREG(off), rb.fetchREG(src));
    else mem->store(addr, rb.fetchREG(src));
}

void MMU::ldo(REG des, REG off, u16 addr)
{
    if(off != REG::NON) rb.loadREG(des, mem->retreive((addr & 0x00FF) + rb.fetchREG(off)));
    else rb.loadREG(des, mem->retreive(addr & 0x00FF));
    updf(des);
}

void MMU::sto(REG src, REG off, u16 addr)
{
    if(off != REG::NON) mem->store((addr & 0x00FF) + rb.fetchREG(off), rb.fetchREG(src));
    else mem->store((addr & 0x00FF), rb.fetchREG(src));
}

void MMU::ldi(REG des, REG off, u16 addr)
{
    u8 act_addr = get_addr(ADR::IXI, addr, rb.fetchREG(off));
    rb.loadREG(des, mem->retreive(act_addr));
    updf(des);
}

void MMU::sti(REG src, REG off, u16 addr)
{
    u8 act_addr = get_addr(ADR::IXI, addr, rb.fetchREG(off));
    mem->store(act_addr, rb.fetchREG(src));
}

void MMU::ldix(REG des, REG off, u16 addr)
{
    u16 base_addr = get_addr(ADR::IND, addr, 0x00);
    rb.loadREG(des, mem->retreive(base_addr + rb.fetchREG(off)));
    updf(des);
}

void MMU::stix(REG src, REG off, u16 addr)
{
    u16 base_addr = get_addr(ADR::IND, addr, 0x00);
    mem->store(base_addr + rb.fetchREG(off), rb.fetchREG(src));
}

u16 MMU::get_addr(ADR mode, u16 addr, u8 off)
{
    u16 res_addr = addr;

    switch(mode)
    {
        case ADR::ABS: break;
        case ADR::ABX: res_addr = addr + tapREG(X); break;
        case ADR::ABY: res_addr = addr + tapREG(Y); break;
        case ADR::IND: res_addr = (mem->retreive(addr) & 0x00FF) | (mem->retreive(addr+1) & 0xFF00); break;
        case ADR::IIX: res_addr = (mem->retreive(addr) & 0x00FF) | (mem->retreive(addr+1) & 0xFF00) + static_cast<u16>(off); break;
        case ADR::IXI: res_addr = ((mem->retreive(addr) + off) & 0x00FF) | (mem->retreive(addr+1) & 0xFF00); break;
        case ADR::REL: if((off & D7) == D7) res_addr = tapPC() - ((~off+1) & 0x00FF);
                       else res_addr = tapPC() + off; 
                       break;
        case ADR::ZER: res_addr = static_cast<u16>(off); break;
        case ADR::ZEX: res_addr = static_cast<u16>(tapREG(X) + off); break;
        case ADR::ZEY: res_addr = static_cast<u16>(tapREG(Y) + off); break;
        default: break;
    }

    return res_addr;
}

void MMU::updf(REG r)
{
    u8 flags = rb.fetchREG(ST);
    u8 value = rb.fetchREG(r);
    if((value & D7) == D7) flags |= D7;
    else flags &= ~D7;
    if(value == 0x00) flags |= D1;
    else flags &= ~D1;
    rb.loadREG(ST, flags);
}

u8 MMU::tapREG(REG r)
{
    return rb.fetchREG(r);
}

u16 MMU::tapPC()
{
    return rb.fetchPC();
}
