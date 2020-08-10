# Chip8-VM
Yet another Chip-8 virtual machine implementation. SFML library has been used for graphics and sound.
# Screenshots
![pong](https://user-images.githubusercontent.com/45107680/89814572-ff458e00-db4b-11ea-9147-badd0d95ceda.png)
![invaders](https://user-images.githubusercontent.com/45107680/89814671-2a2fe200-db4c-11ea-91c6-877a37ed2f1b.png)
![tetris](https://user-images.githubusercontent.com/45107680/89814711-39af2b00-db4c-11ea-8ccd-99852517e384.png)
# Dependencies
- GNU gcc compiler with C++ 17 support
- SFML 
- CMake 3.11 and above
# Installation
    $ git clone https://github.com/ang1337/Chip8-VM.git
    $ cd Chip8-VM && mkdir build
    $ cd build
    $ cmake .. && make
# Usage
- By default, the display size is 64x32. The actual window default size is 640x320 (scaled by a factor of 10). If you want to increase the screen size, pass -s <scale factor> option (if you don't pass this option, the size of the window remains 640x320).
- 'ROMs' directory contains 23 different ROMs that can be executed by the virtual machine. A path to specific ROM is passed through -r <path to ROM> option.
- 'sound' directory contains the beep WAV audio file. A path to it is passed through -a <path to sound file> option.
-  Press Escape to exit the game. 
  
        $./chip8vm -r ../ROMs/TETRIS -a ../sound/censor-beep-01.wav -s 16
        $ ./chip8vm -r ../ROMs/PONG -a ../sound/censor-beep-01.wav 
 
 The first example will launch tetris ROM with a 1024x512 window size.
 The second example will launch pong ROM with a default 640x320 window size. 
