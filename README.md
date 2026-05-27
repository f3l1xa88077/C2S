# CubeSat Camera System (C2S)
Relevant work for the PAST CubeSat 1 payload mission. This project focuses on developing a functioning prototype of the payload camera. The STM32U5 is used for low power performance and the image sensor is connected via DCMI.

The repository includes most aspects of the project including: documentation, hardware (Altium), mechanical 3D models, and software.

## How do I load a CubeIDE project?

The .gitignore has been modified to track critical files only, meaning most libraries and files will need to be generated again.

1. First, download the project locally: `git clone https://github.com/PerthAerospaceStudentTeam/C2S/tree/main`
2. Open the desired project in the STM32CubeIDE via: File>Import>'Projects from Folder or Archive'>Directory.
    - The project will be found in either the C2Sv0.1 or C2Sv1 directory.
3. Click the bullet icon to generate code
4. Click the hammer icon to build the project

If loaded successfully, the console should print the following to verify:

```
Finished building target: C2Sv0.1.elf
 
arm-none-eabi-size  C2Sv0.1.elf 
arm-none-eabi-objdump -h -S C2Sv0.1.elf  > "C2Sv0.1.list"
   text	   data	    bss	    dec	    hex	filename
 110972	    208	  50720	 161900	  2786c	C2Sv0.1.elf
Finished building: default.size.stdout
 
Finished building: C2Sv0.1.list
 

20:14:41 Build Finished. 0 errors, 0 warnings. (took 12s.137ms)

```

If any fatal errors are printed to the console, or the text in the output file is very low `text: 96` then you should address this before proceeding.