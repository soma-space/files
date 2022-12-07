# Microsoft Developer Studio Project File - Name="MapManager" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=MapManager - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MapManager.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MapManager.mak" CFG="MapManager - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MapManager - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "MapManager - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SOMA/SomaClient/MapManager", NFAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MapManager - Win32 Release"

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
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 ../DManage/Release/dmanage.lib /nologo /subsystem:windows /dll /map /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy release\mapmanager.dll ..\release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "MapManager - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /D "_AFXDLL" /D "_MBCS" /D "_AFXEXT" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ../DManage/Debug/dmanage.lib /nologo /subsystem:windows /dll /profile /map /debug /machine:I386
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy debug\mapmanager.dll ..\debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "MapManager - Win32 Release"
# Name "MapManager - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CustomTile.cpp
# End Source File
# Begin Source File

SOURCE=.\CustomTileVary.cpp
# End Source File
# Begin Source File

SOURCE=.\Dib.cpp
# End Source File
# Begin Source File

SOURCE=.\MapCell.cpp
# End Source File
# Begin Source File

SOURCE=.\MapManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MapManager.def
# End Source File
# Begin Source File

SOURCE=.\MapManager.rc
# End Source File
# Begin Source File

SOURCE=.\SgrRes.cpp
# End Source File
# Begin Source File

SOURCE=.\SM_MapManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Sprite.cpp
# End Source File
# Begin Source File

SOURCE=.\SpriteUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Tile.cpp
# End Source File
# Begin Source File

SOURCE=.\TileGroup.cpp
# End Source File
# Begin Source File

SOURCE=.\TilePiece.cpp
# End Source File
# Begin Source File

SOURCE=.\TileSurface.cpp
# End Source File
# Begin Source File

SOURCE=.\TileVary.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\CustomTile.h
# End Source File
# Begin Source File

SOURCE=.\CustomTileVary.h
# End Source File
# Begin Source File

SOURCE=.\Dib.h
# End Source File
# Begin Source File

SOURCE=.\MapCell.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SgrRes.h
# End Source File
# Begin Source File

SOURCE=.\SM_MapManager.h
# End Source File
# Begin Source File

SOURCE=.\Sprite.h
# End Source File
# Begin Source File

SOURCE=.\SpriteUnit.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Tile.h
# End Source File
# Begin Source File

SOURCE=.\TileGroup.h
# End Source File
# Begin Source File

SOURCE=.\TilePiece.h
# End Source File
# Begin Source File

SOURCE=.\TileSurface.h
# End Source File
# Begin Source File

SOURCE=.\TileVary.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\MapManager.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
