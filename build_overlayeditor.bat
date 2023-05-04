@echo off

REM TODO: This script should be building the intermediary, patched copy of the 
REM  OverlayEditor. See apply_patches.sh and build/RTSSOverlayEditor.mod

set FirsttBuildDir=%~dp0build

set BuildDir=%~dp0build\RTSSOverlayEditor.mod
set ThirdPartyCodeDir=%~dp0thirdparty
echo Building from %BuildDir%

pushd %BuildDir%
set $env:CL="/DNVAPI_SHIM_BUILD_AS_DLL" 
msbuild.exe -m OverlayEditor_VS2019.sln /p:Configuration=Release /p:Platform="Win32" /nologo /p:IncludePath="..\..\code;..\..\thirdparty\rtss-sdk\Include;..\..\thirdparty\rtss-sdk\Include\MSIAB;..\..\thirdparty\rtss-sdk\Include\HwInfo;..\..\thirdparty\adl-sdk\include;..\..\thirdparty\nvapi-developer;..\..\thirdparty\igcl\includes;..\..\thirdparty\igcl\wrapper;$(IncludePath)"
popd
