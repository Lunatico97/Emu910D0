#include <mmu.hpp>

MMU::MMU(CardROM* cptr, PPU* pptr, APU* aptr, Controller *ctptr)
{
    reset();
    load_reg(SP, 0xFF);
    ctrl = ctptr;
    crom = cptr;
    ppu = pptr;
    apu = aptr;
}

MMU::~MMU() {}

void MMU::push(REG r)
{
    store(SP_INDEX | fetch_reg(SP), fetch_reg(r));
    load_reg(SP, fetch_reg(SP)-0x01);
}

void MMU::pop(REG r)
{
    load_reg(SP, fetch_reg(SP)+0x01);
    load_reg(r, retreive(SP_INDEX | fetch_reg(SP)));
}

void MMU::pla()
{
    pop(A);
    updf(A);
}

void MMU::php()
{
    store(SP_INDEX | fetch_reg(SP), fetch_reg(ST) | 0x30);
    load_reg(SP, fetch_reg(SP)-0x01);
}

void MMU::plp()
{
    load_reg(SP, fetch_reg(SP)+0x01);
    load_reg(ST, (fetch_reg(ST) & 0x30) | (retreive(SP_INDEX | fetch_reg(SP)) & 0xCF));
}

void MMU::tr(REG des, REG src)
{
    load_reg(des, fetch_reg(src));
    if(des != SP) updf(des);
}

void MMU::lda(REG des, ADR mode, u16 addr, u8 off)
{
    if(mode == -1) load_reg(des, off);
    else
    {
        u16 res_addr = retreive(get_addr(mode, addr, off, 1));
        load_reg(des, res_addr);
    } 
    updf(des);  
}

void MMU::sta(REG src, ADR mode, u16 addr, u8 off)
{
    store(get_addr(mode, addr, off), fetch_reg(src));
}

u16 MMU::get_addr(ADR mode, u16 addr, u8 off, bool cp_check)
{
    u16 res_addr;

    switch(mode)
    {
        case ADR::ABS: res_addr = addr; break;
        case ADR::ABX: res_addr = addr + fetch_reg(X); break;
        case ADR::ABY: res_addr = addr + fetch_reg(Y); break;
        case ADR::IND: res_addr = retreive(addr & 0x00FF) | (static_cast<u16>(retreive((addr+1) & 0x00FF)) << 8); break;
        case ADR::IIX: addr = retreive(addr & 0x00FF) | (static_cast<u16>(retreive((addr+1) & 0x00FF)) << 8);
                       res_addr = addr + off; 
                       break;
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

    if(cp_check && (mode == ADR::ABX || mode == ADR::ABY || mode == ADR::IIX))
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
    oam_des = *ppu->refoam_addr;
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
                ppu->refOAM[oam_des] = oam_data;
                oam_des++;
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
    else if(m_addr == CTRL_P1) ctrl->write_state(m_addr & 0x0001);
    else if(m_addr == OAMDMA) this->signal_dma(value);
    else if(m_addr >= 0x4000 && m_addr < 0x4018) apu->write_from_cpu(m_addr, value);
    else if(m_addr >= 0x4018 && m_addr < 0x6000) return;
    else crom->write_from_cpu(m_addr, value); 
}

u8 MMU::retreive(u16 m_addr)
{
    if(m_addr >= 0x0000 && m_addr < 0x2000) return RAM[(m_addr & 0x07FF)];
    else if(m_addr >= 0x2000 && m_addr < 0x4000) return ppu->read_from_cpu(m_addr & 0x2007);
    else if(m_addr == CTRL_P1 || m_addr == CTRL_P2) return ctrl->read_state(m_addr & 0x0001);
    else if(m_addr >= 0x4000 && m_addr < 0x4018) return apu->read_from_cpu(m_addr);
    else if(m_addr >= 0x4018 && m_addr < 0x6000) return 0x00;
    else return crom->read_from_cpu(m_addr);
}

void MMU::peek_mmu(bool *mmu_up)
{
    ImGui::SetNextWindowPos({0.0f, SCRH-200.0f});
    ImGui::SetNextWindowSize({SCRW, 200.0f});
    ImGui::Begin("Memory Viewer", mmu_up, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse); 

    // Registers
    ImGui::BeginGroup();
    ImGui::TextUnformatted("Registers");
    ImGui::BulletText("PC: %04x", fetch_pc());
    ImGui::BulletText("SP: %02x", fetch_reg(SP));
    ImGui::BulletText("ST: %02x", fetch_reg(ST));
    ImGui::BulletText("A: %02x", fetch_reg(A));
    ImGui::BulletText("X: %02x", fetch_reg(X));
    ImGui::BulletText("Y: %02x", fetch_reg(Y));
    ImGui::EndGroup();

    // Zero-page RAM
    ImGui::SameLine(0.0f, 50.0f);
    ImGui::BeginGroup();
    ImGui::TextUnformatted("Zero-page RAM");
    if(ImGui::BeginTable("zero-ram", 16, ImGuiTableFlags_Borders))
    {
        for(u8 row = 0; row < 0x10; row++)
        {
            ImGui::TableNextRow();
            for (u8 column = 0; column < 0x10; column++)
            {
                ImGui::TableSetColumnIndex(column);
                ImGui::Text("%02x", RAM[row*0x10+column]);
            }
        }   

        ImGui::EndTable();
    }
    ImGui::EndGroup();

    ImGui::End();
}
