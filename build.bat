@echo off

set BuildDir=%~dp0build
set CodeDir=%~dp0code
set ThirdPartyCodeDir=%~dp0thirdparty
set CommonCompiler=-DNVAPI_SHIM_BUILD_AS_DLL -DNVAPI_SHIM_IMPLEMENTATION -I%ThirdPartyCodeDir%\rtss-sdk\Include -I%ThirdPartyCodeDir%\adl-sdk\include -I%ThirdPartyCodeDir%\nvapi-developer -I%ThirdPartyCodeDir%\igcl\includes -I%ThirdPartyCodeDir%\msiab-mahm-sdk\Include -std:c11 -Zo -Zi -O2 -Oi -Oy- -Gy -Gs- -Gw -nologo user32.lib
set CommonLinker=-nologo -INCREMENTAL:NO -subsystem:WINDOWS -OPT:REF -OPT:ICF -TLBID:1 -NXCOMPAT -MACHINE:X86 user32.lib
echo Building from %BuildDir%
echo Compilation Line: %CommonCompiler% %CommonLinker%

if not exist %BuildDir% mkdir %BuildDir%
pushd %BuildDir%
del nvapi_*.pdb

REM cl.exe %CommonCompiler% %CodeDir%\RTSSTweaks.c

REM -WHOLEARCHIVE
cl.exe %CommonCompiler% %CodeDir%\nvapi_shim.c /link -pdb:nvapi_shim%random%.pdb 
link.exe %CommonLinker% -DLL -DEF:../code/nvapi_shim.def -out:nvapi_shim.dll nvapi_shim.obj %ThirdPartyCodeDir%\nvapi-developer\x86\nvapi.lib

del *.exp
REM del *.obj
popd


