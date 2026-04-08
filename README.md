# PortScan
Port mapper using Berkeley sockets for Linux/Mac and Winsock for Windows.

## Description
PortScan is a TCP/UDP port scanner written in C++ that detects open ports and attempts to identify the services running on them (e.g. HTTP, HTTPS, SSH).

It includes both a command-line interface (CLI) and a graphical user interface (GUI).

## Features
- Export scan results in HTML, Markdown, or plain text
- Detect common protocols: HTTP/HTTPS, SSH, FTP, SMTP/ESMTP, DNS, TFTP, NTP, SNMP (experimental)
- GUI version supports command history and simultanious scans using threads.

## Third-Party Libraries
This project uses the following libraries:
[ImGui](https://github.com/ocornut/imgui), [glfw](https://github.com/glfw/glfw) and [glad](https://glad.dav1d.de/).

## Requirements
- C++20 compatible compiler
- CMake 3.10 or higher

## IMPORTANT
- UDP can take a *long* time to finish. This is because i dont use multiple threads and it simply waits for up to 1 second for each port. Maybe in the future i will add async or multithreaded probes.

## Usage
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
### Example
- The following command scans all IPv4s that start with 192.168.1 and targets ports 80 and 433. -v means that failure to connect or simply not getting an answer from a port will be displayed.
```bash
portscan --ip 192.168.1.* --ports 80,443 -v
```
- The following scans ports 0-1024 using TCP on 127.0.0.1. It then exports the file in markdown format. This filepath means it will be written on the current working directory.
```bash
portscan --ip 127.0.0.1 --out results.md
```

- The same thing but using UDP
```bash
portscan --ip 127.0.0.1 -u --out results.md
```

- The same thing but doing one pass using TCP and one using UDP
```bash
portscan --ip 127.0.0.1 -b --out results.md
```

- Clone this repository
```bash
git clone https://github.com/dpent/PortScan.git
```

## Linux
0. You might need to install these
```bash
sudo apt install libxinerama-dev pkgconf libxkbcommon-dev libxcursor-dev libx11-dev libxi-dev libgl1-mesa-dev libxrandr-dev mesa-utils libglu1-mesa-dev wayland-protocols libwayland-dev
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