# PortScan
Port mapper using Berkeley sockets for Linux/Mac and Winsock for Windows.

## Description
This program uses **TCP/UDP** and tries to find out what open ports do. For example it can detect HTTP, HTTPS and SSH protocols running through ports. There is a terminal and a GUI version all in this repo.

## Features
- Can export results in html, markdown or text form.
- Scans for HTTP/HTTPS, SSH, FTP, SMTP/ESMTP, DNS, TFTP, NTP and maybe SNMP.
- The GUI version can store old commands for easy reuse.
- You can also run multiple scans at the same time using multiple threads if you are on the GUI version

## Libraries
This repo uses ImGui, glfw and glad.

## Stack
1. C++ 20
2. cmake 3.10 or higher

## Controls
Type -h when performing a scan to get the help menu. It should look like this
```bash
Output:
Help - Usage: portscan [options]
Options:
-h                    Show this help message
-v                    Verbose output (show all results)
-b                    Scan both TCP and UDP (default is TCP only)
-u                    Scan UDP instead of TCP
--ip <IP_PATTERN>     IP address or pattern to scan (e.g. 192.168.1.*, 127.0-10.0-24.*)
--ports <PORTS>       Comma-separated list of ports/ranges (e.g. 80,443,1000-2000). Default is 0-1024
--out <FILEPATH>      Filepath of the file you want the results to be exported to. Only in .html, .md, .txt formats.

```

## Build steps

- Clone this repository
```bash
git clone https://github.com/dpent/PortScan.git
```

## Linux
0. You might need to install these
```bash
sudo apt install libxinerama-dev && pkg-config && libxkbcommon-dev && libxcursor-dev && libxi-dev && libx11-dev && libxi-dev && libgl1-mesa-dev && libxrandr-dev && mesa-utils
```

### Terminal
1. Install cmake 
```bash
sudo apt install cmake
```
2. Create a directory under the root directory called build
```bash
mkdir build
```
3. Move into the directory
```bash
cd build
```
4. Type
```bash
cmake -DUSE_GUI=OFF ..
```
5. Once it finishes, type
```bash
make
```
6. The program should now be ready to run. Call it using `./portscan` and add other arguments for it to work.

### GUI
1. Install cmake 
```bash
sudo apt install cmake
```
2. Create a directory under the root directory called build
```bash
mkdir build
```
3. Move into the directory
```bash
cd build
```
4. Type
```bash
cmake -DUSE_GUI=ON ..
```
- Once it finishes, type
```bash
make
```
5. The program should now be ready to run. Call it using `./portscan` and the window should open.

## Windows
- Clone this repository
```bash
git clone https://github.com/dpent/PortScan.git
```

0. You will need a C++ compiler that cmake can use. Personally I installed Visual Studio Community Edition 2026 with the C++ extension.

### Terminal
1. Install cmake. You can either get it from the web or some package manager. I got the installer from the website and it worked fine.

2. Create a directory called build
```powershell
mkdir build
```

3. Move into the directory
```powershell
cd build
```

4. Type
```powershell
cmake .. -DUSE_GUI=OFF
```

5. After that, you can type
```powershell
cmake --build . --config Release
```

6. The program should be build now. Move into the Release directory and type ./portscan.exe to run along with arguments for it to work properly.

### GUI
1. Install cmake. You can either get it from the web or some package manager. I got the installer from the website and it worked fine.

2. Create a directory called build
```powershell
mkdir build
```

3. Move into the directory
```powershell
cd build
```

4. Type
```powershell
cmake .. -DUSE_GUI=ON
```

5. After that, you can type
```powershell
cmake --build . --config Release
```

6. The program should be build now. Move into the Release directory and type ./portscan.exe. A window should open.