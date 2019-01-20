# Wifi Network

Use this for installing compatable libraries for esp32 board: [link](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/).  

Using PlatformIo - seems pretty good. See [this](https://github.com/me-no-dev/ESPAsyncWebServer#installation).

## compiling

It seems as though everything compiles correctly on all platforms except for the `wifi_master/` code. This code only compiles on linux. So you need to compile using a virtual machine. VMWare Workstation Pro / Player and VirtualBox both work well, tested with Ubuntu 18. Hyper-V virtual machines do not work because the usb-passthrough doesn't work. Compiling cannot be done simultaneously as working with a hololens emulator, because the hololens emulator requires Hyper-V to be enabled. But VMWare workstation and VirtualBox both require Hyper-V to be disabled. Run `bcdedit /set hypervisorlaunchtype off` to disable hyper-v and `bcdedit /set hypervisorlaunchtype auto` to enable it (on Windows).

## troubleshooting hyper-v

## Troubleshooting hyper-v and virtual machines

Using the HoloLens emulator - Mixed Reality | Microsoft Docs
https://docs.microsoft.com/en-us/windows/mixed-reality/using-the-hololens-emulator

Developing Vuforia Apps for HoloLens
https://library.vuforia.com/content/vuforia-library/en/articles/Training/Developing-Vuforia-Apps-for-HoloLens.html

Unity - Manual: Building a project using IL2CPP
https://docs.unity3d.com/560/Documentation/Manual/IL2CPP-BuildingProject.html

MR Basics 100 - Getting started with Unity - Mixed Reality | Microsoft Docs
https://docs.microsoft.com/en-us/windows/mixed-reality/holograms-100

jschmidtnj/SpaceSuits: A Hololens-based Space Suit. https://jschmidtnj.github.io/SpaceSuits/
https://github.com/jschmidtnj/SpaceSuits

trying this

https://stackoverflow.com/questions/42055988/hololens-emulator-with-visual-studio-2017-rc
-> doesn't seem to work...

found the problem:

Using the HoloLens emulator - Mixed Reality | Microsoft Docs
https://docs.microsoft.com/en-us/windows/mixed-reality/using-the-hololens-emulator

emulation - Cannot choose HoloLens Emulator as debug target in Visual Studio - Stack Overflow
https://stackoverflow.com/questions/36434087/cannot-choose-hololens-emulator-as-debug-target-in-visual-studio

Dixin's Blog - Run Hyper-V and VMware virtual machines on Windows 10
https://weblogs.asp.net/dixin/run-hyper-v-and-vmware-virtual-machines-on-windows-10

windows - How to disable Hyper-V in command line? - Stack Overflow
https://stackoverflow.com/questions/30496116/how-to-disable-hyper-v-in-command-line

virtualbox - Why VitualBox or VMware can not run with Hyper-V enabled Windows 10 - Super User
https://superuser.com/questions/1208850/why-vitualbox-or-vmware-can-not-run-with-hyper-v-enabled-windows-10

How to convert VMware VM to Hyper-V VM (Step by Step guide)
https://www.youtube.com/watch?v=jCLIfyhXwUY

Create a Virtual Machine with Hyper-V | Microsoft Docs
https://docs.microsoft.com/en-us/virtualization/hyper-v-on-windows/quick-start/quick-create-virtual-machine

bcdedit /set hypervisorlaunchtype off

bcdedit /set hypervisorlaunchtype auto

https://blogs.technet.microsoft.com/virtualization/2018/02/28/sneak-peek-taking-a-spin-with-enhanced-linux-vms/

https://github.com/Microsoft/linux-vm-tools/wiki/Onboarding:-Ubuntu