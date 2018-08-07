# RPC over COAP project
This project is created to implement RPC over UDP using COAP protocol for constrained devices.

The project is built and tested on Platform: Ubuntu 18.04 LTS

## Getting started
	You can clone or download source code from this git repo.

### Prerequisites
You need to install some softwares before compiling and running.

#### Installation of g++
```
sudo apt install g++
```

#### Installation of libmnl library
Clone the libmnl repo using:
```
git clone git://git.netfilter.org/libmnl
```
Change directory to libmnl and try to run ./autogen.sh. If you dont have autoreconf installed in your machine, install using following command.

```
cd libmnl
sudo apt install dh-autoreconf 
./autogen.sh
```
Now configure and using makefile, install libmnl using following commands
```
./configure
make
sudo make install
```

### Compiling Program
```
make
```
### Running command

For syntax:
```
./bin/irpc help
```
To see IP configuration:
```
./bin/irpc ipshow [-f] <ipv4|ipv6>
```
To see IP links:
```
./bin/irpc iplink [-f] <ipv4|ipv6>
```
To set UP/Down IP link:
```
sudo ./bin/irpc setlink -n <name of device> -s <ip/down>
```
To see IP routes:
```
sudo ./bin/irpc showiproute --msgformat
```
***Note**:--msgformat with any command will show format of message