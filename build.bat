@echo off

set BuildDir=%~dp0build
set CodeDir=%~dp0code
set ThirdPartyCodeDir=%~dp0thirdparty
set MSVC_CommonCompiler=-I%ThirdPartyCodeDir%\rtss-sdk\Include -I%ThirdPartyCodeDir%\adl-sdk\include -I%ThirdPartyCodeDir%\nvapi-developer -I%ThirdPartyCodeDir%\igcl\includes -I%ThirdPartyCodeDir%\msiab-mahm-sdk\Include -Zo -Zi -O2 -Oi -Oy- -Gy -Gs -nologo user32.lib
REM -Gm -FC -Zo -Zi -GS- -nologo -std:c11 ntdll.lib winmm.lib
echo Building from %BuildDir%
echo Compilation Line: %MSVC_CommonCompiler%

if not exist %BuildDir% mkdir %BuildDir%
pushd %BuildDir%
REM cl.exe %MSVC_CommonCompiler% %CodeDir%\RTSSTweaks.c
cl.exe %MSVC_CommonCompiler% %CodeDir%\nvapi_shim.c /link -pdb:nvapi_shimexe%random%.pdb 
REM cl.exe /MD /LD %MSVC_CommonCompiler% -Fonvapi_shim.obj %CodeDir%\nvapi_shim.c /link -pdb:nvapi_shimdll%random%.pdb
link.exe -nologo -INCREMENTAL:NO -SUBSYSTEM:WINDOWS -OPT:REF -OPT:ICF -TLBID:1 -NXCOMPAT -MACHINE:X86 -DLL -out:nvapi_shim.dll nvapi_shim.obj user32.lib
del *.ilk
del *.exp
REM del *.obj
popd


