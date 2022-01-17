# Microsoft Developer Studio Project File - Name="$$root$$" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=$$root$$ - Win32 D3D8 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "$$root$$.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "$$root$$.mak" CFG="$$root$$ - Win32 D3D8 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "$$root$$ - Win32 D3D8 Release" (based on "Win32 (x86) Application")
!MESSAGE "$$root$$ - Win32 D3D8 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "$$root$$ - Win32 D3D8 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "$$root$$___Win32_D3D8_Release"
# PROP BASE Intermediate_Dir "$$root$$___Win32_D3D8_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj/d3d8"
# PROP Intermediate_Dir "obj/d3d8"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /ML /W3 /GX /O2 /I "../../rwsdk/include/d3d8" /I "./src" /I "./src/win" /I "./share" /I ".share/win" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "RWLOGO" /Fp"obj/d3d8/template.pch" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 rtcharse.lib rtbmp.lib rtpng.lib rpworld.lib rwcore.lib rplogo.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /pdb:"obj/d3d8/template_d3d8.pdb" /machine:I386 /out:"./$$root$$_d3d8.exe" /libpath:"C:/RW/Graphics/rwsdk/lib/d3d8/release"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "$$root$$ - Win32 D3D8 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "$$root$$___Win32_D3D8_Debug0"
# PROP BASE Intermediate_Dir "$$root$$___Win32_D3D8_Debug0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "obj/d3d8d"
# PROP Intermediate_Dir "obj/d3d8d"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../rwsdk/include/d3d8" /I "./src" /I "./src/win" /I "./share" /I "./share/win" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "RWLOGO" /D "RWDEBUG" /FR /Fp"obj/d3d8d/template.pch" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 rtcharse.lib rtbmp.lib rtpng.lib rpworld.lib rwcore.lib rplogo.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /pdb:"obj/d3d8d/template_d3d8d.pdb" /debug /machine:I386 /out:"./$$root$$_d3d8d.exe" /pdbtype:sept /libpath:"C:/RW/Graphics/rwsdk/lib/d3d8/debug"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "$$root$$ - Win32 D3D8 Release"
# Name "$$root$$ - Win32 D3D8 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "AlefSkel"

# PROP Default_Filter ""
# Begin Group "win"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\share\win\win.cpp
# End Source File
# End Group
# Begin Group "sky"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\share\mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\share\skeleton.cpp
# End Source File
# End Group
# Begin Group "AlefCom"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\share\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\share\menu.cpp
# End Source File
# End Group
# Begin Group "Alef"

# PROP Default_Filter ""
# Begin Group "alefwin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\win\events.cpp
# End Source File
# End Group
# Begin Group "alefsky"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\src\$$root$$.cpp
# End Source File
# End Group
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "AlefSkel HD"

# PROP Default_Filter ""
# Begin Group "win HD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\share\win\resource.h
# End Source File
# Begin Source File

SOURCE=.\share\win\win.h
# End Source File
# End Group
# Begin Group "sky HD"

# PROP Default_Filter ""
# End Group
# Begin Source File

SOURCE=.\share\mouse.h
# End Source File
# Begin Source File

SOURCE=.\share\mousedat.h
# End Source File
# Begin Source File

SOURCE=.\share\platform.h
# End Source File
# Begin Source File

SOURCE=.\share\skeleton.h
# End Source File
# End Group
# Begin Group "AlefCom HD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\share\camera.h
# End Source File
# Begin Source File

SOURCE=.\share\menu.h
# End Source File
# Begin Source File

SOURCE=.\share\ptrdata.h
# End Source File
# End Group
# Begin Group "Alef HD"

# PROP Default_Filter ""
# Begin Group "alefwin HD"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\share\events.h
# End Source File
# End Group
# Begin Group "alefsky HD"

# PROP Default_Filter ""
# End Group
# End Group
# End Group
# Begin Group "Misc Files"

# PROP Default_Filter ""
# Begin Group "win misc file"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\share\win\win.rc
# End Source File
# End Group
# Begin Group "sky misc file"

# PROP Default_Filter ""
# End Group
# End Group
# End Target
# End Project
