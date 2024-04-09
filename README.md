# 2D Minecraft for the Nios II Processor (De1-SoC)

## Project Premise

The project consists of a 2D version of Minecraft: The player can traverse the world, placing and breaking blocks to build structures. There will also be enemies the player can interact and fight with. This project is built for the Nios II processor using the C programming language.

## Features

The game begins by generating a world based on a seed, this includes the dirt and grass, and trees found in each world. A world is made up of 16 chunks, each of which are 64 blocks wide and 128 blocks tall. Each block is 8x8 while entities have pixel perfect accuracy, colliding with blocks.

When the program begins, a start screen outlines all the user controls and prompts the player to press E to begin playing the game. The player is then loaded into a 2d version of the game minecraft which is made using math functions to generate a unique world. The sky in the background cycles smoothly between 25 colours to simulate a day/night cycle based on program run time.

The player can move left and right using `A` and `D` keys respectively, and the player speed can be controlled by sprinting, ‘ctrl’ key, by crouching, ‘shift’ key, or by walking, no additional keys necessary. The player can also jump using the ‘space’ key when on the ground. This world also has a fully fledged collision system and physics so jumping and movement is affected by the physical environment and gravity.

The player can use the mouse and hover over blocks to highlight them. If a highlighted block is within 2 blocks of the player, the player can break it by left clicking. The player can place blocks using right click if a hovered block area is empty. If a valid location is not hovered by the mouse, a block is chosen to be placed or broken based on player direction.

Breaking or placing a block will also play a unique sound effect. The user can choose one of 8 blocks on their hotbar using the number keys which then can be placed. Once a block is broken, a miniature version of the block is dropped (4x4) which has entity physics. This means that the player can push this smaller block around and that it adheres to the world's gravitational rules.

The player also has a health bar which is shown in the UI above the toolbar, representing the player’s health. A red heart represents a point of health while a black heart shows how many points are lost. Once the player loses all 10 hearts, they die, causing them to respawn at the world's starting point with full health. Health can potentially be lost through fall damage if the player falls from a high point.

## Quick Start Guide

To run this game, you are required to have the following:

- A computer which can run `Intel FPGA Monitor Program 18.1` or `Intel FPGA Monitor Program 18.0`.
- A DE1-SoC board which can be interfaced with by the above mentioned computer.
- The program code file named ‘project.c’ which can be found on the [Github](https://github.com/alwyn-t/2D-Minecraft-Nios-II) page.
- PS/2 keyboard and mouse
- PS/2 Y splitter
- Speakers

## Configuration

> Note: Running this project on the Nios II processor is low, however the De1-SoC board does include an ARM chip which can be used instead to have a faster frame rate. Installation guide is the same as the Nios II processor except for switching the architecture in step 3 from `Nios II` to `ARM Cortex-A9` and explicitly adding the compiler flag in step 6 to include `-std=c99`.

1. Begin by opening the monitor program (version 18.0 or 18.1 works).
2. Create a new project by going to `File > New Project…`.
3. Within the pop up window, specify a project directory[^1], project name and select `Nios II` for the architecture then click `Next >`.
4. Select the `DE1-SoC Computer` as the system then click `Next >`.
5. Select the `C Program` as the program type then click `Next >`.
6. Click the `Add…` and select the `project.c` file on your computer folder system and add `-lm` to the additional linker flags[^2] then click `Next >`.
7. Connect the DE1-SoC to the computer via the USB-Blaster II using the Type A to B USB Cable, then click `Refresh` and ensure `DE-SoC [USB-1]` is selected for the host connection then click `Next >`.
8. Select the `Basic` as the linker section presets then click `Save`.
9. When prompted, select `Yes` to download the Nios II system to the DE1-SoC board.
10. Once the system is loaded, click the `Compile & load the current program configuration and start the debugging session`.
    [^1]: Ensure all folders do not include white spaces to avoid errors
    [^2]: Monitor Program 18.1 does not properly generate the correct makefile so it is required to add the -lm flag here: <pre>$(CC) $(LDFLAGS) $(OBJS) <b>-lm</b> -o $@</pre>

## How to play once compiled

1. Plug in keyboard and mouse using a Y splitter. A speaker could also be plugged in for audio.
2. `SW 9` corresponds to the first PS/2 component address while `SW 8` corresponds to the second PS/2 component address. Toggling a switch on will set the address to be used by a keyboard whilst switching it off will set it to be used by a mouse. Based on which PS/2 port of the splitter each component is plugged into, flip the switches accordingly. If the PS/2 keyboard is plugged into the purple side and the mouse is plugged into the green side of the splitter port, set `SW 9` high and set `SW 8` low.
3. Press the `E` key to start the game and enjoy!

## Controls

`E` - start game

`A` - move left

`D` - move right

`Space` - jump

`1-8` - switch selected blocks

`Ctrl` - sprint (faster left and right movement)

`Shift` - sneak (slower left and right movement)

`Mouse position` - crosshair movement

`Left click` - break block (up to 2 blocks in front of the player)

`Right click` - place block (up to 2 blocks in front of the player)

## I/O Components

This project utilises many of the I/O available on the DE1-SoC board and has unique infrastructure built to support the gameplay and rendering. I/O refers to the input and output of the program, specifically, this program uses the Nios II memory mapped devices to interact with the VGA DAC (digital to analog converter), PS/2 ports, switches, LEDs and Audio CODEC.

### VGA double buffering

Using two 240x512 (240x320 of visible screen) to write into one array while reading from the other array to prevent any writing and reading from the same memory.

### PS/2 keyboard

Using the PS/2 keyboard protocol, we take in all the byte code data from the PS/2 register to register all the make and break codes.

### PS/2 mouse

Using the PS/2 mouse, we take all the byte code data which is in packets of 3 (for the default mouse) to move the crosshair on the screen.

### PS/2 dual mode

Using the two PS/2 registers, the user will specify what type of input is in (and therefore can be hot swapped during runtime).

### Switches

Because we desire to have both mouse and keyboard inputs at the same time, we use the switches to swap between mouse and keyboard input polling to ensure whichever input is connected, the user can specify and the dual PS/2 registers will act accordingly.

### LEDs

To ensure user feedback, we use the LEDs to show whether the first or second register is in keyboard or mouse mode.

### Audio CODEC

When a block is broken or placed, a loaded sound file is placed into the audio CODEC FIFO to be played back to the user.

## Infrastructure Components

A lot of the infrastructure was built using structs to ensure proper organisation of different features like entities or chunks. It also allows for global arrays to be set without worry of clutter in the code base.

### Blocks

Each block is an 8x8 pixel box that uses texture arrays to display each block on screen. Transparency is achieved by assigning the 0x0 as `transparent` so anytime the processor receives a 0x0 colour, it will not draw that pixel so any colour behind will be visible. This can most easily be seen with the leaf blocks.

### Chunks

Each chunk is made up of 64 wide and 128 tall arrays of blocks, which each block can be broken or placed. We also ensure that we are only using information that is on screen and therefore needed. Every world is created with 16 chunks to give a large world size.

### World Generation

To create interesting worlds, the world generation uses custom formulas to create the ground and populate the world with trees. The ground is created with two height maps (one for ‘mountains’, sharper peaks and one for ‘hills’, simulating rolling hills) that are smoothly interpolated between (with another function). To populate with trees, another function is used that provides an average tree density over the entire world and will clump trees in regions.

#### Height Map Functions

`y_h`: hill height map, `y_m`: mountain height map, `w_h`: hill weighting, `w_m`: mountain weighting

```cpp
double y_h = 3 * (pow(sin(x/10),2)*sin(x/15+2)/1.5 + cos(x/35)*sin(x/20));
double y_m = 4 * (pow(sin(2*x/25),3)*cos(x/15) + pow(sin(6*x/35), 2)*sin(7*x/15)/2);
double w_h = pow(sin(x/30), 4);
double w_m = 1 - w_h;
```

$$
y_h = 3*\left(\sin^2\left(\frac{x}{10}\right)*\frac{\sin\left(\frac{x}{15}+2\right)}{1.5}+\cos\left(\frac{x}{35}\right)\*\sin(\frac{x}{20})\right)
$$

$$
y_m = 4*\left(\sin^3\left(\frac{2x}{25} \right )*\cos\left(\frac{x}{15} \right ) + \frac{\sin^2\left(\frac{6x}{35} \right )\*\sin\left(\frac{7x}{15} \right )}{2} \right )
$$

$$
w_h = \sin^4\left(\frac{x}{30} \right )
$$

$$
w_m = 1 - w_h
$$

#### Tree Generation Function

```cpp
int x = 64.0/tree_density*i + 12*sin((double)i/4)*pow(cos(2*(double)i/7), 2);
```

$$
t_x = \frac{64.0}{tree_\rho}*i + 12\*\sin\left(\frac{t_i}{4} \right )\*\cos^2\left(\frac{2t_i}{7} \right )
$$

### Entities

Generic entity controller is used to do collision detection, acceleration and entity specific movement such as velocity and position. Using a generic entity controller, we use it for our player and item entities. We have separate hostile, passive and item entity arrays for displaying and updating all entities. We also have a cap on the hostile, passive and item entity arrays individually avoid low performance and we have an index for each array as a ‘guess’ for the next available space for adding an entity, if that spot is filled, we loop through until we find an open spot or loop through the entire array (we then don’t create the entity).

### Sky

We have 25 different colour states for different times of the day and night cycle, to ensure a smooth display, we interpolate (we take the percentage between the two colours and combine each R, G, B value separately before recombining) between the current colour and the next colour to make dynamic sky colour.

## Attribution Table

|                      | Aryan Hussian | Alwyn Tong |
| -------------------- | ------------- | ---------- |
| **I/O**              | ---           | ---        |
| VGA Output/Rendering |               | `WD` `MR`  |
| PS/2 Keyboard        |               | `WD` `MR`  |
| PS/2 Mouse           | `ET`          | `WD` `MR`  |
| Sound                | `WD` `MR`     |            |
| **Gameplay**         | ---           | ---        |
| World Generation     |               | `WD` `MR`  |
| Block Textures       | `WD`          | `WD` `MR`  |
| Start Screen         | `WD` `MR`     |            |
| Player Movement      | `ET`          | `WD` `MR`  |
| Entity Collision     | `WD`          | `MR`       |
| Toolbar              | `WD` `MR`     |            |
| Health + Fall Damage | `WD` `MR`     |            |
| Day and Night Cycle  |               | `WD` `MR`  |
| Items                |               | `WD` `MR`  |

`WD` - Working Draft, `MR` - Major Revision, `ET` - Edited
