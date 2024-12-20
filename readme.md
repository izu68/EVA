## Disclaimer:

This repository is not yet ready to be used, actual documentation coming soon. There are a lot of aspects of the design that I change frequently, so aspects like the memory map and the commands to interface with the Mega Drive are not documented.  
## EVA

This is a simulator/emulator for a general purpose Mega Drive/Genesis accelerator. I call this a simulator because a hardware implementation for this does not exist yet. Features are described below, integration notes for the BlastEm emulator are in another file.  

### Features

- Sprite and tilemap scaling, both horizontally and vertically.
- Sprite and tilemap rotation.
- Uncompressed PCM (WAV) playback. 256 slots for PCM streams and 16 channels.
- Polygon rendering (Not fully implemented yet).

### Feature notes

PCM playback works through SDL2, the emulator responds to commands sent by the Mega Drive and fires SDL audio handling functions. This is an extremely high level sound emulation and should be changed. WAV files are treated as if they were in a CD; the files are not embedded in the ROM, they are put in a separate folder called "flash" together with a file similar to a CUE file, that specifies a sound bank number and the path of the sound file. This is because whenever this is implemented in hardware, sound files will be stored in a separate flash chip from the 68k ROM.  

All rendering is bitmap based but are automatically converted to tile arrangements in a "virtual" VRAM page called EVRAM. The accelerator accesses pixels from EVRAM and converts them to bitmaps on the fly, then converts back to tiles. The Mega Drive can DMA those rendered tilesets to its VRAM for display.  

### Hardware implementation possibilities

My plan is to implement this in a cartridge, possibly as firmware ran by an STM32. That family of MCUs has quite powerful and feature rich models, which would fit well for this application. The STM32 would handle the memory mapping and serve data for both the 68k ROM and the interface memories (such as the virtual VRAM and work RAM). The 68k ROM would go in one flash chip, the PCM data in another, and the interface memories would be store in an SRAM connected to the STM.  

Current MCU candidate: STM32F407ZET6  

### TODO

- Make the sound system a bit more sophisticated.
- Forward bitmap plotting functions so the Mega Drive can run them.
- Forward some DSP functions so the Mega Drive can run them.
- Add texture mapping to the polygon renderer.
