# Game Boy Libary

A library to write Game Boy games in assembly using the  [RGBDS](https://github.com/gbdev/rgbds) toolchain.

## Features

TODO

## Documentation

The library exposes the following macros to help you makes games.

| Function Name | Description |
| :--- | :--- |
| gbBreak | Add a code breakpoint. |
| gbMessage | Output debug message \1. |
| gbRecordHighestLine | Record the current line into \1 if the line is bigger than the current stored one. |
| gbCheckZ | Output debug message \3 when bit \2 in byte \1 is raised. |
| gbCheckNZ
 | Output debug message \3 when bit \2 in byte \1 is raised.
 |
| gbCheckLT
 | Output debug message \3 when \1 >= \2.
 |
| gbCheckLE
 | Output debug message \3 when \1 > \2.
 |
| gbCheckEQ
 | Output debug message \3 when \1 != \2.
 |
| gbCheckGT
 | Output debug message \3 when \1 <= \2.
 |
| gbCheckGE
 | Output debug message \3 when \1 < \2.
 |
| gbForceZ
 | Output debug message \3 and block when bit \2 in byte \1 is raised.
 |
| gbForceNZ
 | Output debug message \3 and block when bit \2 in byte \1 is raised.
 |
| gbForceLT
 | Output debug message \3 and block when \1 >= \2.
 |
| gbForceLE
 | Output debug message \3 and block when \1 > \2.
 |
| gbForceEQ
 | Output debug message \3 and block when \1 != \2.
 |
| gbForceGT
 | Output debug message \3 and block when \1 <= \2.
 |
| gbForceGE
 | Output debug message \3 and block when \1 < \2.
 |
| gbDmaOamClear
 | Hides all sprites using DMA.
 |
| gbDmaOamCopy
 | Copy sprites data using DMA.
\1 = high byte of the source address
 |
| gbDmaCopyGeneral
 | Start a general DMA copy.
\1 = high byte of the destination address (higher 3 bits always %100)
\2 = low byte of the destination address (lower 4 bits always %0000)
\3 = high byte of the source address
\4 = low byte of the source address (lower 4 bits always %0000)
\5 = size (data byte size / 16 - 1)
 |
| gbDmaCopyGeneral16
 | Start a general DMA copy.
\1 = the destination address (higher 3 bits always %100, lower 4 bits always %0000))
\2 = the source address (lower 4 bits always %0000)
\3 = size (data byte size / 16 - 1)
 |
| gbDmaCopyHblank
 | Start a hblank DMA copy.
\1 = high byte of the destination address (higher 3 bits always %100)
\2 = low byte of the destination address (lower 4 bits always %0000)
\3 = high byte of the source address
\4 = low byte of the source address (lower 4 bits always %0000)
\5 = size (data byte size / 16 - 1)
 |
| gbDmaCopyHblank16
 | Start a hblank DMA copy.
\1 = the destination address (higher 3 bits always %100, lower 4 bits always %0000))
\2 = the source address (lower 4 bits always %0000)
\3 = size (data byte size / 16 - 1)
 |
| gbInputUpdate
 | Update the keypad input.
Expected to be used only once in the main loop.
Example: gbInputUpdate
 |
| gbInputTestHeld
 | Raise the zero flag if all keys in \1 are held.
Example: gbInputTestHeld PAD_LEFT | PAD_DOWN
 |
| gbInputTestPressed
 | Raise the zero flag if input \1 is pressed.
Example: gbInputTestPressed PAD_LEFT
 |
| gbInputTestReleased
 | Raise the zero flag if input \1 is released.
Example: gbInputTestReleased PAD_A
 |
| gbInputTestPressedAny
 | Raise the zero flag if none of the flags in input \1 are pressed.
Example: gbInputTestPressedAny PAD_LEFT | PAD_RIGHT
 |
| gbInputTestReleasedAny
 | Raise the zero flag if none of the flags in input \1 are released.
Example: gbInputTestReleasedAny PAD_LEFT | PAD_RIGHT
 |
| rcf
 | Reset the carry flag.
 |
| copy
 | Copy \2 into \1 through (a).
Example: copy [$1234], 10
 |
| copyh
 | Copy \2 into \1 through (a) using ldh.
Example: copyh [$FF84], 10
 |
| gbWaitVblank
 | Wait for the vblank.
 |
| gbFlagVblank
 | Must be used when entering the vblank.
 |
| gbVblankWatchdog
 | Run the vblank watchdog.
 |
| gbCheckVblankOverflow
 | Check whether the vblank overflowed.
 |
| gbGetHardware
 | Get the hardware type in register a.
 |
| gbInt8_AbsA
 | Compute the absolute of register a.
 |
| gbInt16_Load
 | Load the value of a 16-bit register from memory.
\1: destination register
\2: source address
 |
| gbInt16_Store
 | Store the value of a 16-bit integer or literal into memory.
\1: destination address
\2: source register or literal
 |
| gbInt16_Copy
 | Copy the value of a 16-bit register.
\1: destination register
\2: source register
 |
| gbInt16_Conv8
 | Convert an 8-bit register to a 16-bit register.
\1: 16-bit destination register
\2: 8-bit source register
 |
| gbInt16_Inverse
 | Inverse a 16-bit integer.
\1: register
 |
| gbInt16_Sign
 | Raise the zero flag if a 16-bit integer is positive or zero.
\1: register
 |
| gbInt16_Abs
 | Compute the absolute value of a 16-bit integer.
\1: register
 |
| gbInt16_Add
 | Add a 16-bit integer to another 16-bit integer or literal.
\1: register
\2: register or literal
 |
| gbInt16_Sub
 | Subtract a 16-bit integer or a literal from a 16-bit integer.
\1: register
\2: register or literal
 |
| gbInt16_ShiftLeft
 | Shift left a 16-bit integer.
\1: register
 |
| gbInt16_Div2
 | Divide a 16-bit integer by 2.
\1: register
 |
| gbInt16_Comp
 | Compare a 16-bit integer to another 16-bit integer or literal.
The carry is raised if the first 16-bit integer is stricly less than the second 16-bit integer or literal.
This is a signed comparison.
\1: register
\2: register or literal
 |
| gbInt16_Min
 | Return the minimun of a 16-bit integer and another 16-bit integer or a literal.
The result is stored in LHS.
\1: LHS (register)
\2: RHS (register or literal)
 |
| gbInt16_Max
 | Return the maximum of a 16-bit integer and another 16-bit integer or a literal.
The result is stored in LHS.
\1: LHS (register)
\2: RHS (register or literal)
 |
| gbInt16_Clamp
 | Clamp a 16-bit integer.
\1: value to clamp (register)
\2: minimum (register or literal)
\3: maximum (register or literal)
 |
| gbInt16_Modulo
 | Compute \1 % \2.
This is an iterative algorithm, expected to be used with
relatively small divisors.
\1: dividend (register)
\2: divisor (register or literal)
 |
| gbFixed_Set
 | Set the value of a fixed from an RGBASM 32-bit fixed.
\1: result address
\2: an RGBASM 32-bit value
 |
| gbFixed_Zero
 | Set the value of a fixed to zero.
\1: result address
 |
| gbFixed_Sign
 | Raise the zero flag if a fixed is positive or zero.
\1: the fixed to check
 |
| gbFixed_Inverse
 | Inverse a fixed.
\1: destination address
\2: source address
 |
| gbFixed_Abs
 | Compute the absolute value of a fixed.
\1: destination address
\2: source address
 |
| gbFixed_Copy
 | Copy a fixed into another one.
\1: destination address
\2: source address
 |
| gbFixed_CopyInt16
 | Copy a int16 into a fixed.
\1: destination address
\2: source address
 |
| gbFixed_Add
 | Add two fixed.
\1: result address (can be one of the operand)
\2: address of LHS
\3: address of RHS
 |
| gbFixed_Add16
 | Add a 16-bit integer to a fixed.
\1: result address (can be the input fixed)
\2: address of the input fixed
\3: 16-bit register
 |
| gbFixed_Sub
 | Subtract two fixed.
\1: result address (can be one of the operand)
\2: address of LHS
\3: address of RHS
 |
| gbFixed_Div2
 | Divide a fixed by 2.
The sign is conserved.
\1: result address (can be one of the operand)
\2: the value to shift
 |
| gbFixed_Comp
 | Compare two fixed.
The carry is raised if the first fixed is stricly less than the second fixed.
This is a signed comparison.
\1: address of LHS
\2: address of RHS
 |
| gbFixed_Min
 | Return the minimun of two fixed.
\1: result address
\2: address of LHS
\3: address of RHS
 |
| gbFixed_Max
 | Return the maximum of two fixed.
\1: result address
\2: address of LHS
\3: address of RHS
 |
| gbFixed_Clamp
 | Clamp a fixed.
\1: result address
\2: address of the value to clamp
\3: address of the minimum value
\4: address of the maximum value
 |
| gbVector_Set
 | Set the value of a vector from two RGBASM 32-bit fixed.
\1: result address
\2: an RGBASM 32-bit value
\3: an RGBASM 32-bit value
 |
| gbVector_Zero
 | Set the value of a vector to zero.
\1: result address
 |
| gbVector_Copy
 | Copy a vector into another one.
\1: destination address
\2: source address
 |
| gbVector_Abs
 | Compute the absolute value of a vector.
\1: destination address
\2: source address
 |
| gbVector_Add
 | Add two vectors.
\1: result address (can be one of the operand)
\2: address of LHS
\3: address of RHS
 |
| gbVector_Add16
 | Add two 16-bit integer to a vector.
\1: result address (can be the input vector)
\2: address of the input vector
\3: the 16-bit register to add to the abscissa
\4: the 16-bit register to add to the ordinate
 |
| gbVector_Sub
 | Subtract two vectors.
\1: result address (can be one of the operand)
\2: address of LHS
\3: address of RHS
 |
| gbVector_Div2
 | Shift right a vector, component-wise.
\1: result address (can be one of the operand)
\2: the value to shift
 |
| gbVector_Comp
 | Compare two vectors.
The carry is raised if all components of the first vector are stricly less
than all the component in the the second vector.
This is a signed comparison.
\1: address of LHS
\2: address of RHS
 |
| gbVector_Min
 | Return the minimun of two vectors, component-wise.
\1: result address
\2: address of LHS
\3: address of RHS
 |
| gbVector_Max
 | Return the maximum of two vectors, component-wise.
\1: result address
\2: address of LHS
\3: address of RHS
 |
| gbVector_Clamp
 | Clamp a vector,
\1: result address
\2: address of the value to clamp
\3: address of the minimum value
\4: address of the maximum value
 |
| gbLfsr8PickSeed
 | Pick a seed for LFSR and put it in (a).
 |
| gbLfsr8GetNextValue
 | Produce the next LFSR value from the value in (a).
(a) should not be zero, otherwise zero will be returned.
This macro will never produce zero if (a) is not zero.
 |
| gbClearMemory
 | Clear a memory region to zero.
\1 = start address
\2 = size
 |
| gbSetMemory
 | Set a memory region to a given value.
\1 = start address
\2 = size
\3 = value
 |
| gbCopyMemory
 | Copy memory from one address to another.
\1 = destination
\2 = source
\3 = size
 |
| gbSetRomBank
 | Set the ROM bank.
\1 = bank number
 |
| gbSetVramBank
 | Set the VRAM bank.
\1 = bank number
 |
| gbSetWramBank
 | Set the WRAM bank.
\1 = bank number
 |
| gbLoad16
 | Load 2 bytes at address \2 into 16-bit register \1.
Example: glLoad16 de, $FF82
 |
| gbStore16
 | Load the content of 16-bit register \2 into two bytes at address \1.
Example: gbStore16 $FF82, hl
 |
| gbPushAllRegisters
 | Push all the registers onto the stack.
 |
| gbPopAllRegisters
 | Pop all the registers from the stack.
 |
| gbCallHL
 | Call the function pointed by (hl).
 |
| gbExtractAddressFromTableIntoHL
 | Extract an address from a table.
\1 = table, \2 = index memory location
 |
| gbCallFunctionFromTable
 | Call a function from a function table.
\1 = table, \2 = index memory location
 |
| gbJumpToFunctionFromTable
 | Jump to a function from a function table.
\1 = table, \2 = index memory location
 |
| gbToggleCpuSpeed
 | Toggle the CPU between single and double speed.
CGB only.
 |
