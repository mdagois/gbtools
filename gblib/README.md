# Game Boy Libary

A library to write Game Boy games in assembly using the  [RGBDS](https://github.com/gbdev/rgbds) toolchain.

## Usage

* Include `gblib.rgbinc` in any file using the library.
* Compile and link `gblib.rgbasm` into your ROM.

## Features

The library exposes macros to help you makes games covering the following aspects.

* Input
* DMA transfers
* 16-bit integer and 32-bit fixed math
* Memory and bank management
* Jump tables facilities
* Debug facilities

## Documentation

| Function Name | Description |
| :--- | :--- |
| gbBreak | Add a code breakpoint. |
| gbMessage | Output debug message \1. |
| gbRecordHighestLine | Record the current line into \1 if the line is bigger than the current stored<br>one. |
| gbCheckZ | Output debug message \3 when bit \2 in byte \1 is raised. |
| gbCheckNZ | Output debug message \3 when bit \2 in byte \1 is raised. |
| gbCheckLT | Output debug message \3 when \1 >= \2. |
| gbCheckLE | Output debug message \3 when \1 > \2. |
| gbCheckEQ | Output debug message \3 when \1 != \2. |
| gbCheckGT | Output debug message \3 when \1 <= \2. |
| gbCheckGE | Output debug message \3 when \1 < \2. |
| gbForceZ | Output debug message \3 and block when bit \2 in byte \1 is raised. |
| gbForceNZ | Output debug message \3 and block when bit \2 in byte \1 is raised. |
| gbForceLT | Output debug message \3 and block when \1 >= \2. |
| gbForceLE | Output debug message \3 and block when \1 > \2. |
| gbForceEQ | Output debug message \3 and block when \1 != \2. |
| gbForceGT | Output debug message \3 and block when \1 <= \2. |
| gbForceGE | Output debug message \3 and block when \1 < \2. |
| gbDmaOamClear | Hides all sprites using DMA. |
| gbDmaOamCopy | Copy sprites data using DMA.<br>\1 = high byte of the source address |
| gbDmaCopyGeneral | Start a general DMA copy.<br>\1 = high byte of the destination address (higher 3 bits always %100)<br>\2 = low byte of the destination address (lower 4 bits always %0000)<br>\3 = high byte of the source address<br>\4 = low byte of the source address (lower 4 bits always %0000)<br>\5 = size (data byte size / 16 - 1) |
| gbDmaCopyGeneral16 | Start a general DMA copy.<br>\1 = the destination address (higher 3 bits always %100, lower 4 bits always %0000))<br>\2 = the source address (lower 4 bits always %0000)<br>\3 = size (data byte size / 16 - 1) |
| gbDmaCopyHblank | Start a hblank DMA copy.<br>\1 = high byte of the destination address (higher 3 bits always %100)<br>\2 = low byte of the destination address (lower 4 bits always %0000)<br>\3 = high byte of the source address<br>\4 = low byte of the source address (lower 4 bits always %0000)<br>\5 = size (data byte size / 16 - 1) |
| gbDmaCopyHblank16 | Start a hblank DMA copy.<br>\1 = the destination address (higher 3 bits always %100, lower 4 bits always %0000))<br>\2 = the source address (lower 4 bits always %0000)<br>\3 = size (data byte size / 16 - 1) |
| gbInputUpdate | Update the keypad input.<br>Expected to be used only once in the main loop.<br>Example: gbInputUpdate |
| gbInputTestHeld | Raise the zero flag if all keys in \1 are held.<br>Example: gbInputTestHeld PAD_LEFT | PAD_DOWN |
| gbInputTestPressed | Raise the zero flag if input \1 is pressed.<br>Example: gbInputTestPressed PAD_LEFT |
| gbInputTestReleased | Raise the zero flag if input \1 is released.<br>Example: gbInputTestReleased PAD_A |
| gbInputTestPressedAny | Raise the zero flag if none of the flags in input \1 are pressed.<br>Example: gbInputTestPressedAny PAD_LEFT | PAD_RIGHT |
| gbInputTestReleasedAny | Raise the zero flag if none of the flags in input \1 are released.<br>Example: gbInputTestReleasedAny PAD_LEFT | PAD_RIGHT |
| rcf | Reset the carry flag. |
| copy | Copy \2 into \1 through (a).<br>Example: copy [$1234], 10 |
| copyh | Copy \2 into \1 through (a) using ldh.<br>Example: copyh [$FF84], 10 |
| gbWaitVblank | Wait for the vblank. |
| gbFlagVblank | Must be used when entering the vblank. |
| gbVblankWatchdog | Run the vblank watchdog. |
| gbCheckVblankOverflow | Check whether the vblank overflowed. |
| gbGetHardware | Get the hardware type in register a. |
| gbInt8_AbsA | Compute the absolute of register a. |
| gbInt16_Load | Load the value of a 16-bit register from memory.<br>\1: destination register<br>\2: source address |
| gbInt16_Store | Store the value of a 16-bit integer or literal into memory.<br>\1: destination address<br>\2: source register or literal |
| gbInt16_Copy | Copy the value of a 16-bit register.<br>\1: destination register<br>\2: source register |
| gbInt16_Conv8 | Convert an 8-bit register to a 16-bit register.<br>\1: 16-bit destination register<br>\2: 8-bit source register |
| gbInt16_Inverse | Inverse a 16-bit integer.<br>\1: register |
| gbInt16_Sign | Raise the zero flag if a 16-bit integer is positive or zero.<br>\1: register |
| gbInt16_Abs | Compute the absolute value of a 16-bit integer.<br>\1: register |
| gbInt16_Add | Add a 16-bit integer to another 16-bit integer or literal.<br>\1: register<br>\2: register or literal |
| gbInt16_Sub | Subtract a 16-bit integer or a literal from a 16-bit integer.<br>\1: register<br>\2: register or literal |
| gbInt16_ShiftLeft | Shift left a 16-bit integer.<br>\1: register |
| gbInt16_Div2 | Divide a 16-bit integer by 2.<br>\1: register |
| gbInt16_Comp | Compare a 16-bit integer to another 16-bit integer or literal.<br>The carry is raised if the first 16-bit integer is stricly less than the second 16-bit integer or literal.<br>This is a signed comparison.<br>\1: register<br>\2: register or literal |
| gbInt16_Min | Return the minimun of a 16-bit integer and another 16-bit integer or a literal.<br>The result is stored in LHS.<br>\1: LHS (register)<br>\2: RHS (register or literal) |
| gbInt16_Max | Return the maximum of a 16-bit integer and another 16-bit integer or a literal.<br>The result is stored in LHS.<br>\1: LHS (register)<br>\2: RHS (register or literal) |
| gbInt16_Clamp | Clamp a 16-bit integer.<br>\1: value to clamp (register)<br>\2: minimum (register or literal)<br>\3: maximum (register or literal) |
| gbInt16_Modulo | Compute \1 % \2.<br>This is an iterative algorithm, expected to be used with<br>relatively small divisors.<br>\1: dividend (register)<br>\2: divisor (register or literal) |
| gbFixed_Set | Set the value of a fixed from an RGBASM 32-bit fixed.<br>\1: result address<br>\2: an RGBASM 32-bit value |
| gbFixed_Zero | Set the value of a fixed to zero.<br>\1: result address |
| gbFixed_Sign | Raise the zero flag if a fixed is positive or zero.<br>\1: the fixed to check |
| gbFixed_Inverse | Inverse a fixed.<br>\1: destination address<br>\2: source address |
| gbFixed_Abs | Compute the absolute value of a fixed.<br>\1: destination address<br>\2: source address |
| gbFixed_Copy | Copy a fixed into another one.<br>\1: destination address<br>\2: source address |
| gbFixed_CopyInt16 | Copy a int16 into a fixed.<br>\1: destination address<br>\2: source address |
| gbFixed_Add | Add two fixed.<br>\1: result address (can be one of the operand)<br>\2: address of LHS<br>\3: address of RHS |
| gbFixed_Add16 | Add a 16-bit integer to a fixed.<br>\1: result address (can be the input fixed)<br>\2: address of the input fixed<br>\3: 16-bit register |
| gbFixed_Sub | Subtract two fixed.<br>\1: result address (can be one of the operand)<br>\2: address of LHS<br>\3: address of RHS |
| gbFixed_Div2 | Divide a fixed by 2.<br>The sign is conserved.<br>\1: result address (can be one of the operand)<br>\2: the value to shift |
| gbFixed_Comp | Compare two fixed.<br>The carry is raised if the first fixed is stricly less than the second fixed.<br>This is a signed comparison.<br>\1: address of LHS<br>\2: address of RHS |
| gbFixed_Min | Return the minimun of two fixed.<br>\1: result address<br>\2: address of LHS<br>\3: address of RHS |
| gbFixed_Max | Return the maximum of two fixed.<br>\1: result address<br>\2: address of LHS<br>\3: address of RHS |
| gbFixed_Clamp | Clamp a fixed.<br>\1: result address<br>\2: address of the value to clamp<br>\3: address of the minimum value<br>\4: address of the maximum value |
| gbVector_Set | Set the value of a vector from two RGBASM 32-bit fixed.<br>\1: result address<br>\2: an RGBASM 32-bit value<br>\3: an RGBASM 32-bit value |
| gbVector_Zero | Set the value of a vector to zero.<br>\1: result address |
| gbVector_Copy | Copy a vector into another one.<br>\1: destination address<br>\2: source address |
| gbVector_Abs | Compute the absolute value of a vector.<br>\1: destination address<br>\2: source address |
| gbVector_Add | Add two vectors.<br>\1: result address (can be one of the operand)<br>\2: address of LHS<br>\3: address of RHS |
| gbVector_Add16 | Add two 16-bit integer to a vector.<br>\1: result address (can be the input vector)<br>\2: address of the input vector<br>\3: the 16-bit register to add to the abscissa<br>\4: the 16-bit register to add to the ordinate |
| gbVector_Sub | Subtract two vectors.<br>\1: result address (can be one of the operand)<br>\2: address of LHS<br>\3: address of RHS |
| gbVector_Div2 | Shift right a vector, component-wise.<br>\1: result address (can be one of the operand)<br>\2: the value to shift |
| gbVector_Comp | Compare two vectors.<br>The carry is raised if all components of the first vector are stricly less<br>than all the component in the the second vector.<br>This is a signed comparison.<br>\1: address of LHS<br>\2: address of RHS |
| gbVector_Min | Return the minimun of two vectors, component-wise.<br>\1: result address<br>\2: address of LHS<br>\3: address of RHS |
| gbVector_Max | Return the maximum of two vectors, component-wise.<br>\1: result address<br>\2: address of LHS<br>\3: address of RHS |
| gbVector_Clamp | Clamp a vector,<br>\1: result address<br>\2: address of the value to clamp<br>\3: address of the minimum value<br>\4: address of the maximum value |
| gbLfsr8PickSeed | Pick a seed for LFSR and put it in (a). |
| gbLfsr8GetNextValue | Produce the next LFSR value from the value in (a).<br>(a) should not be zero, otherwise zero will be returned.<br>This macro will never produce zero if (a) is not zero. |
| gbSgbInitializeDevice | Initialize the SGB device. |
| gbSgbSendCommand | Send the SGB command at address \1. |
| SgbPacket_Pal01 | Declare a PAL01 command loading from a .pal file \1 with<br>byte offset \2 for the first palette and byte offset \3<br>for the second palette. |
| SgbPacket_Pal23 | Declare a PAL23 command loading from a .pal file \1 with<br>byte offset \2 for the first palette and byte offset \3<br>for the second palette. |
| SgbPacket_Pal03 | Declare a PAL03 command loading from a .pal file \1 with<br>byte offset \2 for the first palette and byte offset \3<br>for the second palette. |
| SgbPacket_Pal12 | Declare a PAL12 command loading from a .pal file \1 with<br>byte offset \2 for the first palette and byte offset \3<br>for the second palette. |
| SgbPacket_PalTrn | Declare a PAL_TRN command. |
| SgbPacket_PalSet | Declare a PAL_SET command.<br>\1 = palette 0 index (0-511)<br>\2 = palette 1 index (0-511)<br>\3 = palette 2 index (0-511)<br>\4 = palette 3 index (0-511)<br>\5 = flags<br>\6 = attribute file number (0-44) (PAL_SET_FLAG_SET_ATTR must be in \5) |
| SgbPacket_AttrTrn | Declare a ATTR_TRN command. |
| SgbPacket_AttrSet | Declare a ATTR_SET command.<br>\1 = flags |
| SgbPacket_ChrTrn | Declare a CHR_TRN command.<br>\1 = SNES VRAM tile block number (0 for tiles $00 to $7F, or 1 for tiles $80 to $FF) |
| SgbPacket_PctTrn | Declare a PCT_TRN command. |
| SgbPacket_Sound | Declare a SOUND command.<br>\1 = port A effect<br>\2 = port B effect<br>\3 = parameters<br>\4 = BGM |
| SgbPacket_MltReq | Declare a MLT_REQ command with joypad count \1. |
| SgbPacket_MltReq_OneJoypad | Declare a MLT_REQ command with one joypad. |
| SgbPacket_MltReq_TwoJoypad | Declare a MLT_REQ command with two joypads. |
| SgbPacket_MltReq_FourJoypad | Declare a MLT_REQ command with four joypads. |
| SgbPacket_AtrcEn | Declare a ATRC_EN command.<br>\1 = flags |
| SgbPacket_IconEn | Declare a ICON_EN command.<br>\1 = flags |
| SgbPacket_PalPri | Declare a PAL_PRI command.<br>\1 = flags |
| SgbPacket_MaskEn | Declare a MASK_EN command of type \1. |
| SgbPacket_MaskEn_Cancel | Declare a MASK_EN command of type CANCEL. |
| SgbPacket_MaskEn_Freeze | Declare a MASK_EN command of type FREEZE. |
| SgbPacket_MaskEn_Black | Declare a MASK_EN command of type BLACK. |
| SgbPacket_MaskEn_Color0 | Declare a MASK_EN command of type COLOR0. |
| gbClearMemory | Clear a memory region to zero.<br>\1 = start address<br>\2 = size |
| gbSetMemory | Set a memory region to a given value.<br>\1 = start address<br>\2 = size<br>\3 = value |
| gbCopyMemory | Copy memory from one address to another.<br>\1 = destination<br>\2 = source<br>\3 = size |
| gbSetRomBank | Set the ROM bank.<br>\1 = bank number |
| gbSetVramBank | Set the VRAM bank.<br>\1 = bank number |
| gbSetWramBank | Set the WRAM bank.<br>\1 = bank number |
| gbLoad16 | Load 2 bytes at address \2 into 16-bit register \1.<br>Example: glLoad16 de, $FF82 |
| gbStore16 | Load the content of 16-bit register \2 into two bytes at address \1.<br>Example: gbStore16 $FF82, hl |
| gbPushAllRegisters | Push all the registers onto the stack. |
| gbPopAllRegisters | Pop all the registers from the stack. |
| gbCallHL | Call the function pointed by (hl). |
| gbExtractAddressFromTableIntoHL | Extract an address from a table.<br>\1 = table, \2 = index memory location |
| gbCallFunctionFromTable | Call a function from a function table.<br>\1 = table, \2 = index memory location |
| gbJumpToFunctionFromTable | Jump to a function from a function table.<br>\1 = table, \2 = index memory location |
| gbToggleCpuSpeed | Toggle the CPU between single and double speed.<br>CGB only. |

