@echo off

REM TODO: This script should be building the intermediary, patched copy of the 
REM  OverlayEditor. See apply_patches.sh and build/RTSSOverlayEditor.mod

set FirsttBuildDir=%~dp0build

set BuildDir=%~dp0build\RTSSOverlayEditor.mod
set ThirdPartyCodeDir=%~dp0thirdparty
set MSVC_CommonCompiler=-I%ThirdPartyCodeDir%\rtss-sdk\Include -I%ThirdPartyCodeDir%\adl-sdk\include -I%ThirdPartyCodeDir%\nvapi-developer -I%ThirdPartyCodeDir%\igcl\includes -I%ThirdPartyCodeDir%\msiab-mahm-sdk\Include -Zo -Zi -nologo ntdll.lib
REM -Gm -FC -Zo -Zi -GS- -nologo -std:c11 ntdll.lib winmm.lib
echo Building from %BuildDir%

pushd %BuildDir%
set $env:CL="/DOVERLAYEDITOR_BUILD" 
msbuild.exe -m OverlayEditor_VS2019.sln /p:Configuration=Debug /p:Platform="Win32" /nologo /p:IncludePath="..\..\code;..\..\thirdparty\rtss-sdk\Include;..\..\thirdparty\rtss-sdk\Include\MSIAB;..\..\thirdparty\rtss-sdk\Include\HwInfo;..\..\thirdparty\adl-sdk\include;..\..\thirdparty\nvapi-developer;..\..\thirdparty\igcl\includes;$(IncludePath)"

REM msbuild.exe -m OverlayEditor_VS2019.sln /p:Configuration=Debug /p:Platform="Win32" /nologo /p:IncludePath="..\rtss-sdk\Include;..\rtss-sdk\Include\HwInfo;..\rtss-sdk\Include\MSIAB;..\adl-sdk\include;..\nvapi-developer;..\igcl\includes;..\igcl\wrapper;$(IncludePath)"
popd
