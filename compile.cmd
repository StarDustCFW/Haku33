@echo off
cd Hakupayload
make
cd ..
move /y Hakupayload\payload.bin romfs\Haku33_payload.bin
make
echo completado
"C:\devkitPro\tools\bin\nxlink.exe" --address 192.168.1.129 Haku33.nro
%systemroot%\system32\timeout.exe 55
exit
