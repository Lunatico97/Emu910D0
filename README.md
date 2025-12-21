# E910D0 v1.0 [NES Emulator]
- Author: Diwas Adhikari
- Status: In Progress
- Description: NES Emulator to relive my childhood memories of playing with a bootleg NES/Famicom console 
               which looked like a PS1 and had a vertical cartridge slot like a Famicom :)
- Comments: This started up as being rigidly behavior-driven but, I kept on removing unnecessary
            junk to speed up and keep stuff clean. The goal itself is to make NES games playable.


## Current Status:
- Working 6502 CPU | PPU | Keyboard & Gamepad Input | Mapper 0 (NROM), 1 (SxROM), 2 (UxROM) & 3 (CNROM)
- Build & Run: `./buildE910D0.sh _.nes` | Run: `e910D0 _.nes`

![Donkey Kong](rsrc/donkey_kong.gif)

### Commands:

<table>
    <tr> <th> Keyboard Controls </th> <th> NES Equivalent / Gamepad Buttons </th> </tr>
    <tr> <td> W/A/S/D </td> <td> D-PAD </td> <tr>
    <tr> <td> SPACE </td> <td> SELECT </td> <tr>
    <tr> <td> ENTER </td> <td> START </td> <tr>
    <tr> <td> [ </td> <td> A-BUTTON / Y-BUTTON </td> <tr>
    <tr> <td> ] </td> <td> B-BUTTON </td> <tr>
</table>

<table>
    <tr> <th> Keyboard Controls </th> <th> NES Commands </th> </tr>
    <tr> <td> ESCAPE </td> <td> EXIT </td> <tr/>
    <td> NUM_6 </td> <td> PAUSE / UNPAUSE (Continuous execute) </td> <tr>
    <tr> <td> NUM_7 </td> <td> STEP (Stepwise Execute) </td> <tr>
    <tr> <td> NUM_8 </td> <td> IRQ (Interrupt Request) </td> <tr>
    <tr> <td> NUM_9 </td> <td> NMI (Non-Maskable Interrupt) </td> <tr>
    <tr> <td> NUM_0 </td> <td> RST (Reset) </td> <tr>
</table>

### Tests:
![NES Test Menu](rsrc/nestest.gif)
- Passes all official tests in video mode at reset vector: 0xC004.
- Matches first 5000 lines of nestest.log (upto official instruction tests) in automation mode with PC force-set at 0xC000.
- CPU tests for every 6502 instruction are available in `tests/e910D0Tests.cpp`.
- Replace the file with `e910D0.cpp` to run test suite.

### Tested NES Games:
- Mapper 0 (Donkey Kong, Ice Climber, Super Mario Bros. 1, Excite Bike, Kung Fu)

<table>
    <tr>
        <td> <img src="rsrc/ice_climber.gif" alt="Ice Climber"> </td>
        <td> <img src="rsrc/smb1.gif" alt="Super Mario Bros. 1"> </td>
    <tr>
</table>

- Mapper 1 (Metroid, The Legend of Zelda, Megaman 2, Snow Bros.) [MMC1 - ASIC]

<table>
    <tr>
        <td> <img src="rsrc/metroid.png" alt="Metroid"> </td>
        <td> <img src="rsrc/megaman2.png" alt="Megaman 2"> </td>
    <tr>
    <tr>
        <td> <img src="rsrc/tloz.png" src="The Legend of Zelda"> </td>
        <td> <img src="rsrc/snowbros.png" src="Snow Bros."> </td>
    <tr>
</table>

- Mapper 2 (Megaman, Castlevania, Contra, Metal Gear)

<table>
    <tr>
        <td> <img src="rsrc/megaman.png" alt="Megaman"> </td>
        <td> <img src="rsrc/castlevania.png" alt="Castlevania"> </td>
    <tr>
    <tr>
        <td> <img src="rsrc/contra.png" src="Contra"> </td>
        <td> <img src="rsrc/metal_gear.png" src="Metal Gear"> </td>
    <tr>
</table>

- Mapper 3 (Milon's Secret Castle, Adventure Island, Track & Field, Tengen's Tetris)

<table>
    <tr>
        <td> <img src="rsrc/milons_secret_castle.png" alt="Milon's Secret Castle"> </td>
        <td> <img src="rsrc/adventure_island.png" alt="Adventure Island"> </td>
    <tr>
        <td> <img src="rsrc/track_and_field.png" alt="Track & Field"> </td>
        <td> <img src="rsrc/tetris_tengen.png" alt="Tengen's Tetris"> </td>
    <tr>
</table>

