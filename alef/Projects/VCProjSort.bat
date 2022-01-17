VCProjSort.exe ApModules\ApModules.vcproj > ApModules\ApModules.vcproj2
del /f ApModules\ApModules.vcproj_org
rename ApModules\ApModules.vcproj ApModules.vcproj_org
rename ApModules\ApModules.vcproj2 ApModules.vcproj

VCProjSort.exe AgpModules\AgpModules.vcproj > AgpModules\AgpModules.vcproj2
del /f AgpModules\AgpModules.vcproj_org
rename AgpModules\AgpModules.vcproj AgpModules.vcproj_org
rename AgpModules\AgpModules.vcproj2 AgpModules.vcproj

VCProjSort.exe AgsModules\AgsModules.vcproj > AgsModules\AgsModules.vcproj2
del /f AgsModules\AgsModules.vcproj_org
rename AgsModules\AgsModules.vcproj AgsModules.vcproj_org
rename AgsModules\AgsModules.vcproj2 AgsModules.vcproj

VCProjSort.exe AgcModules\AgcModules.vcproj > AgcModules\AgcModules.vcproj2
del /f AgcModules\AgcModules.vcproj_org
rename AgcModules\AgcModules.vcproj AgcModules.vcproj_org
rename AgcModules\AgcModules.vcproj2 AgcModules.vcproj
