## BlastEm integration notes

Everything is glued to BlastEm in `genesis.c`.

### Execution

EVA should act as a memory map unless it's running some command like scaling a sprite. Whenever a command is written and executed by the 68k through the control registers, EVA would halt the 68k and not release it until it's done rendering, to prevent the 68k from slowing down the rendering by requesting new data from the cartridge bus. The memory access functions fire command execution in response to the execution register being read or written.  

Reset is handled in the `handle_reset_requests` function.  

### Bus connection

EVA's connection to the system should be within the cartridge bus, as is implemented all reads/writes are handled in `unused_read` and `unused_write` in `genesis.c`. The functions `eva_communicate_bus_read` and `eva_communicate_bus_write` inform EVA that certain interface memories or hardware registers have been accessed, the addresses are evaluated and execution is fired.
