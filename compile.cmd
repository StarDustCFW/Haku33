@echo off
make
echo completado
"C:\devkitPro\tools\bin\nxlink.exe" --address 192.168.1.136 Haku33.nro
%systemroot%\system32\timeout.exe 55
exit
