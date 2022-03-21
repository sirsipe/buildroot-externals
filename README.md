# buildroot-externals
This and that as buildroot BR2_EXTERNAL config. First follow the **Initial setup** and then package specific instructions.

## Initial setup
```
mkdir ~/myproject
cd ~/myproject
git clone git://git.buildroot.net/buildroot
git clone https://github.com/sirsipe/buildroot-externals
```

## Package: RaspberryPi WiFi (rpi-wifi)

This package configures WiFi for Raspberry Pi and allows also configuring OpenSSH with root access, resulting in very easy headless access for RPi devices over WiFi.
Selects also `haveged` to aid with random generator initialization with OpenSSH for significanly faster boot.

Change the `raspberrypi0w_defconfig` according to your RPi board. Not tested but I see no reason why this wouldn't work at least with RPi3, but possibly also with RPi4 and Zero W2.

```
cd ~/myproject/buildroot
make raspberrypi0w_defconfig
make BR2_EXTERNAL=../buildroot-externals menuconfig
```
1. Choose 
  `External Options -> rpi-wifi` and setup `Wifi SSID` and `Wifi Password`. Leave `modprobe brcmfmac` set unless you choose EUDEV as /dev management.

2. To access your device over Wifi using SSH, choose
  `External Options -> rpi-wifi -> OpenSSH Server with root login enabled + HAVENGED for faster boot` and then ensure `Root password (COMPULSORY!)` is not empty.    

3. Exit & Save

4. `make` & wait
