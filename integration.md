## BlasEm integration notes

- Execution
EVA should run asynchronously from the rest of the system, but I have decided not to run its process in a separate thread or move it to a function call higher in the emulation hierarchy. All its logic is ran once every time the rest of the components sync, this is precisely in the `sync_components` function call. Last time I tested this seems to be every ~252 clock cycles (should dig further into that).

- Bus connection
EVA's connection to the system should be within the cartridge bus, but as is implemented all reads/writes are handled in `unused_read` and `unused_write`. This effectively means BlastEm won't really know that this should be a cartridge-specific component; all reads and writes to any address can be intercepted through those functions. As far as the emulator is concerned EVA's simulator appears to be an internal system component.
