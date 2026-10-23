# How to Compile and Run the Program
## Linux And WSL  
First od all open a terminala and run the following commands:

```bash
sudo apt update
sudo apt install build-essential cmake git libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
```

then you can run the command: 
```bash 
mkdir build-linux && cd build-linux
cmake ..
cmake --build .
```

and to run it:
```bash
./JumpMechanics
```

## Windows 
If the wsl has not WSLg on you can generate a .exe and run it like that. 
Open a wsl and run:
```bash
sudo apt update
sudo apt install mingw-w64
```
and then run:
```bash 
mkdir build-win && cd build-win
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain-windows.cmake ..
cmake --build .
```
To run the program go to file explore and put in the path and put 
`\\wsl.localhost\` and go to the project. Then double click on the .exe file.
