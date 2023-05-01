@echo off

REM TODO: This script should be building the intermediary, patched copy of the 
REM  OverlayEditor. See apply_patches.sh and build/RTSSOverlayEditor.mod

set BuildDir=%~dp0build
set ThirdPartyCodeDir=%~dp0thirdparty
set MSVC_CommonCompiler=-I%ThirdPartyCodeDir%\rtss-sdk\Include -I%ThirdPartyCodeDir%\adl-sdk\include -I%ThirdPartyCodeDir%\nvapi-developer -I%ThirdPartyCodeDir%\igcl\includes -I%ThirdPartyCodeDir%\msiab-mahm-sdk\Include -Zo -Zi -nologo ntdll.lib
REM -Gm -FC -Zo -Zi -GS- -nologo -std:c11 ntdll.lib winmm.lib
echo Building from %BuildDir%

pushd %ThirdPartyCodeDir%\RTSSOverlayEditor
msbuild.exe -m OverlayEditor_VS2019.sln /p:Configuration=Debug /p:Platform="Win32" /nologo /p:IncludePath="..\rtss-sdk\Include;..\rtss-sdk\Include\HwInfo;..\rtss-sdk\Include\MSIAB;..\adl-sdk\include;..\nvapi-developer;..\igcl\includes;..\igcl\wrapper;$(IncludePath)"
popd
