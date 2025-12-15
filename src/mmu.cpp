#include <mmu.hpp>

MMU::MMU(CardROM* cptr, PPU* pptr, Controller *ctptr)
{
    reset();
    load_reg(SP, 0xFF);
    ctrl = ctptr;
    crom = cptr;
    ppu = pptr;
}

MMU::~MMU() {}

void MMU::push(REG r)
{
    if(fetch_reg(SP) > 0x00)
    {
        store(SP_INDEX | fetch_reg(SP), fetch_reg(r));
        load_reg(SP, fetch_reg(SP)-0x01);
    }
    else throw(std::runtime_error("Stack overflow !"));
}

void MMU::pop(REG r)
{
    if(fetch_reg(SP) <= 0xFF)
    {
        load_reg(SP, fetch_reg(SP)+0x01);
        load_reg(r, retreive(SP_INDEX | fetch_reg(SP)));
    }
    else throw(std::runtime_error("Stack underflow !"));
}

void MMU::pla()
{
    pop(A);
    updf(A);
}

void MMU::php()
{
    if(fetch_reg(SP) > 0x00)
    {
        store(SP_INDEX | fetch_reg(SP), fetch_reg(ST) | 0x30);
        load_reg(SP, fetch_reg(SP)-0x01);
    }
    else throw(std::runtime_error("Stack overflow !"));
}

void MMU::plp()
{
    if(fetch_reg(SP) <= 0xFF)
    {
        load_reg(SP, fetch_reg(SP)+0x01);
        load_reg(ST, (fetch_reg(ST) & 0x30) | (retreive(SP_INDEX | fetch_reg(SP)) & 0xCF));
    }
    else throw(std::runtime_error("Stack underflow !"));
}

void MMU::tr(REG des, REG src)
{
    load_reg(des, fetch_reg(src));
    if(des != SP) updf(des);
}

void MMU::ld(REG des, u8 value)
{
    load_reg(des, value);
    updf(des);
}

void MMU::ld(REG des, REG off, u16 addr)
{
    if(off != REG::NON) load_reg(des, retreive(addr + fetch_reg(off)));
    else load_reg(des, retreive(addr));
    updf(des);
}

void MMU::st(REG src, REG off, u16 addr)
{
    if(off != REG::NON) store(addr + fetch_reg(off), fetch_reg(src));
    else store(addr, fetch_reg(src));
}

void MMU::ldo(REG des, REG off, u16 addr)
{
    if(off != REG::NON) load_reg(des, retreive(static_cast<u8>((addr & 0x00FF) + fetch_reg(off))));
    else load_reg(des, retreive(addr & 0x00FF));
    updf(des);
}

void MMU::sto(REG src, REG off, u16 addr)
{
    if(off != REG::NON) store(static_cast<u8>((addr & 0x00FF) + fetch_reg(off)), fetch_reg(src));
    else store((addr & 0x00FF), fetch_reg(src));
}

void MMU::ldi(REG des, REG off, u16 addr)
{
    u16 act_addr = get_addr(ADR::IXI, addr, fetch_reg(off));
    load_reg(des, retreive(act_addr));
    updf(des);
}

void MMU::sti(REG src, REG off, u16 addr)
{
    u16 act_addr = get_addr(ADR::IXI, addr, fetch_reg(off));
    store(act_addr, fetch_reg(src));
}

void MMU::ldix(REG des, REG off, u16 addr)
{
    u16 base_addr = get_addr(ADR::IND, addr, 0x00);
    load_reg(des, retreive(base_addr + fetch_reg(off)));
    updf(des);
}

void MMU::stix(REG src, REG off, u16 addr)
{
    u16 base_addr = get_addr(ADR::IND, addr, 0x00);
    store(base_addr + fetch_reg(off), fetch_reg(src));
}

u16 MMU::get_addr(ADR mode, u16 addr, u8 off)
{
    u16 res_addr;

    switch(mode)
    {
        case ADR::ABS: res_addr = addr; break;
        case ADR::ABX: res_addr = addr + fetch_reg(X); break;
        case ADR::ABY: res_addr = addr + fetch_reg(Y); break;
        case ADR::IND: res_addr = retreive(addr) | (static_cast<u16>(retreive((addr+1) & 0x00FF)) << 8); break;
        case ADR::IIX: res_addr = (retreive(addr) | (static_cast<u16>(retreive((addr+1) & 0x00FF)) << 8)) + static_cast<u16>(off); break;
        case ADR::IXI: res_addr = retreive((addr+off) & 0x00FF) | (static_cast<u16>(retreive((addr+off+1) & 0x00FF)) << 8); break;
        case ADR::REL: addr = fetch_pc();
                       if(off & D7) res_addr = addr - ((~off+1) & 0x00FF);
                       else res_addr = addr + off; 
                       break;
        case ADR::ZER: res_addr = static_cast<u16>(off); break;
        case ADR::ZEX: res_addr = static_cast<u8>(fetch_reg(X) + off); break;
        case ADR::ZEY: res_addr = static_cast<u8>(fetch_reg(Y) + off); break;
        default: break;
    }

    if(mode != ADR::ZER && mode != ADR::ZEX && mode != ZEY)
    {
        cycle_penalty += cross_page(addr, res_addr);
    }

    return res_addr;
}

bool MMU::cross_page(u16 pre_addr, u16 post_addr)
{
    if((pre_addr & 0xFF00) != (post_addr & 0xFF00)) return true;
    else return false;
}

void MMU::updf(REG r)
{
    u8 flags = fetch_reg(ST);
    u8 value = fetch_reg(r);
    if(value & D7) flags |= D7;
    else flags &= ~D7;
    if(value == 0x00) flags |= D1;
    else flags &= ~D1;
    load_reg(ST, flags);
}

void MMU::reset()
{
    memset(RAM, 0x00, RAM_SIZE);
    memset(BANK, 0x00, REG_BANKS);
}

u8 MMU::fetch_reg(REG r)
{
    return *(BANK+r);
}

u16 MMU::fetch_pc()
{
    return (static_cast<u16>(*(BANK+PCH)) << 8) | static_cast<u16>(*(BANK+PCL));
}

void MMU::load_reg(REG r, const u8& value)
{
    *(BANK+r) = value;
}

void MMU::load_pc(const u16& addr)
{
    *(BANK+PCL) = (addr & 0x00FF);
    *(BANK+PCH) = (addr & 0xFF00) >> 8;
}

void MMU::signal_dma(u8 src_hn8)
{
    dma_src = static_cast<u16>(src_hn8) << 8;
    dma_rqst = true;
    dma_off = 0x00;
}

void MMU::perform_dma(bool sync_rw)
{
    if(dma_rqst)
    {
        if(!dma_sync)
        {
            if(sync_rw) dma_sync = true;
        }
        else
        {
            if(!sync_rw) oam_data = retreive(dma_src | dma_off);
            else
            {
                ppu->refOAM[dma_off] = oam_data;
                dma_off++;

                if(dma_off == 0x00) 
                {
                    dma_rqst = false;
                    dma_sync = false;
                }
            }
        }
    }
}

void MMU::store(u16 m_addr, u8 value)
{
    if(m_addr >= 0x0000 && m_addr < 0x2000) RAM[(m_addr & 0x07FF)] = value;
    else if(m_addr >= 0x2000 && m_addr < 0x4000) ppu->write_from_cpu((m_addr & 0x2007), value);
    else if(m_addr == CTRL_P1 || m_addr == CTRL_P2) ctrl->write_state(m_addr & 0x0001);
    else if(m_addr == OAMDMA) this->signal_dma(value);
    else if(m_addr >= 0x4000 && m_addr < 0x4020) return;
    else if(m_addr >= 0x4020 && m_addr < 0x6000) return;
    else crom->write_from_cpu(m_addr, value); 
}

u8 MMU::retreive(u16 m_addr)
{
    if(m_addr >= 0x0000 && m_addr < 0x2000) return RAM[(m_addr & 0x07FF)];
    else if(m_addr >= 0x2000 && m_addr < 0x4000) return ppu->read_from_cpu(m_addr & 0x2007);
    else if(m_addr == CTRL_P1 || m_addr == CTRL_P2) return ctrl->read_state(m_addr & 0x0001);
    else if(m_addr >= 0x4000 && m_addr < 0x4020) return 0x00;
    else if(m_addr >= 0x4020 && m_addr < 0x6000) return 0x00;
    else return crom->read_from_cpu(m_addr);
}
