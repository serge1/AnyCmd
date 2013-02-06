msbuild anycmd.vcxproj /t:Rebuild /p:project="anycmd" /p:platform="Win32" /p:configuration="Release"
msbuild anycmd.vcxproj /t:Rebuild /p:project="anycmd" /p:platform="x64" /p:configuration="Release"
cscript zip.vbs wlx_anycmd_1.1.zip Release\anycmd.wlx x64\Release\anycmd.wlx64 pluginst.inf doc\index.htm
