@echo off

set BuildDir=%~dp0build
set CodeDir=%~dp0code
set ThirdPartyCodeDir=%~dp0thirdparty
set MSVC_CommonCompiler=-I%ThirdPartyCodeDir%\rtss-sdk\Include -I%ThirdPartyCodeDir%\adl-sdk\include -I%ThirdPartyCodeDir%\nvapi-developer -I%ThirdPartyCodeDir%\igcl\includes -I%ThirdPartyCodeDir%\msiab-mahm-sdk\Include -Zo -Zi -nologo ntdll.lib
REM -Gm -FC -Zo -Zi -GS- -nologo -std:c11 ntdll.lib winmm.lib
echo Building from %BuildDir%
echo Compilation Line: %MSVC_CommonCompiler%

if not exist %BuildDir% mkdir %BuildDir%
pushd %BuildDir%
cl.exe %MSVC_CommonCompiler% %CodeDir%\RTSSTweaks.c
del *.ilk
del *.obj
popd


