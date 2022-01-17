rem echo start copying resources to share repository
cd c:\AlefRelease\bin\ini
svn update -N --username steeple --password 0000
cd c:\AlefRelease\bin\ini\adult
svn update -N --username steeple --password 0000
cd c:\AlefRelease\bin\ini\Object\Artist
svn update -N --username steeple --password 0000
cd c:\AlefRelease\bin\ini\Object\Design
svn update -N --username steeple --password 0000
cd c:\AlefRelease\bin\WORLD
svn update -N --username steeple --password 0000
cd c:\AlefRelease\bin\WORLD\SERVER
svn update -N --username steeple --password 0000

cd c:\AlefRelease
copy BIN\AlefServer.exe			\\121.156.72.242\Share\Alef_Resource\Bin\
copy BIN\AlefServer.pdb			\\121.156.72.242\Share\Alef_Resource\Bin\
xcopy BIN\ini\*.* 			\\121.156.72.242\Share\Alef_Resource\Bin\ini\ /c /d /y
xcopy BIN\ini\adult\*.* 		\\121.156.72.242\Share\Alef_Resource\Bin\ini\adult\ /c /d /y
xcopy BIN\ini\object\design\*.* 	\\121.156.72.242\Share\Alef_Resource\Bin\ini\object\design\ /c /d /y
xcopy BIN\world\*.* 			\\121.156.72.242\Share\Alef_Resource\Bin\world\ /c /d /y
xcopy BIN\world\server\*.* 		\\121.156.72.242\Share\Alef_Resource\Bin\world\server\ /c /d /y

del \\121.156.72.242\Share\Alef_Resource\Bin\ini\DBServerinfo.ini

pause 