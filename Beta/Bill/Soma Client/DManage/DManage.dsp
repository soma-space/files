# Microsoft Developer Studio Project File - Name="DManage" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DManage - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DManage.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DManage.mak" CFG="DManage - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DManage - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DManage - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SOMA/SomaClient/DManage", MFAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DManage - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_DIRECTDLLMODULE" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ddraw.lib winmm.lib dinput.lib dxguid.lib dsound.lib /nologo /subsystem:windows /dll /map /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\dmanage.dll ..\release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "DManage - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_AFXEXT" /D "_DIRECTDLLMODULE" /FR /YX"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib winmm.lib dinput8.lib dxguid.lib dsound.lib d3dim.lib /nologo /subsystem:windows /dll /profile /map /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\dmanage.dll ..\debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "DManage - Win32 Release"
# Name "DManage - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AlphaMask.cpp
# End Source File
# Begin Source File

SOURCE=.\AlphaProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\BB_Bitmap.cpp
# End Source File
# Begin Source File

SOURCE=.\BB_Direct.cpp
# End Source File
# Begin Source File

SOURCE=.\BBDX_MouseInformation.cpp
# End Source File
# Begin Source File

SOURCE=.\ColorVertex.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dtextr.cpp
# End Source File
# Begin Source File

SOURCE=.\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\Direct3D.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectSound.cpp
# End Source File
# Begin Source File

SOURCE=.\DManage.cpp
# End Source File
# Begin Source File

SOURCE=.\DManage.def
# End Source File
# Begin Source File

SOURCE=.\DManage.rc
# End Source File
# Begin Source File

SOURCE=.\Night.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AlphaMask.h
# End Source File
# Begin Source File

SOURCE=.\AlphaProcess.h
# End Source File
# Begin Source File

SOURCE=.\BB_Bitmap.h
# End Source File
# Begin Source File

SOURCE=.\BB_Direct.h
# End Source File
# Begin Source File

SOURCE=.\BBDX_MouseInformation.h
# End Source File
# Begin Source File

SOURCE=.\ColorVertex.h
# End Source File
# Begin Source File

SOURCE=.\d3dtextr.h
# End Source File
# Begin Source File

SOURCE=.\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\Direct3D.h
# End Source File
# Begin Source File

SOURCE=.\DirectSound.h
# End Source File
# Begin Source File

SOURCE=.\Night.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\DManage.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
