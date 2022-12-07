# Microsoft Developer Studio Project File - Name="IGObject" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=IGObject - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IGObject.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IGObject.mak" CFG="IGObject - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IGObject - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "IGObject - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SOMA/Soma Tool/IGObject", FLAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IGObject - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"IGObject_Release.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy IGObject_Release.lib ..\Igt4\Release
# End Special Build Tool

!ELSEIF  "$(CFG)" == "IGObject - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"IGObject_Debug.lib"
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=copy IGObject_Debug.lib ..\Igt4\Debug
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "IGObject - Win32 Release"
# Name "IGObject - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AniObj.cpp
# End Source File
# Begin Source File

SOURCE=.\AniObjLSP.cpp
# End Source File
# Begin Source File

SOURCE=.\DDEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\DDEngineLSP.cpp
# End Source File
# Begin Source File

SOURCE=.\Dib16.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectInput.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectSound.cpp
# End Source File
# Begin Source File

SOURCE=.\GameResrc.cpp
# End Source File
# Begin Source File

SOURCE=.\Intf.cpp
# End Source File
# Begin Source File

SOURCE=.\Item.cpp
# End Source File
# Begin Source File

SOURCE=.\MapObj.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjBasic.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjLook.cpp
# End Source File
# Begin Source File

SOURCE=.\PathFinder.cpp
# End Source File
# Begin Source File

SOURCE=.\Script.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TileMap.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AniObj.h
# End Source File
# Begin Source File

SOURCE=.\AniObjLSP.h
# End Source File
# Begin Source File

SOURCE=.\DDEngine.h
# End Source File
# Begin Source File

SOURCE=.\DDEngineLSP.h
# End Source File
# Begin Source File

SOURCE=.\Dib16.h
# End Source File
# Begin Source File

SOURCE=.\DirectInput.h
# End Source File
# Begin Source File

SOURCE=.\DirectSound.h
# End Source File
# Begin Source File

SOURCE=.\GameResrc.h
# End Source File
# Begin Source File

SOURCE=.\Intf.h
# End Source File
# Begin Source File

SOURCE=.\Item.h
# End Source File
# Begin Source File

SOURCE=.\MapObj.h
# End Source File
# Begin Source File

SOURCE=.\MyConstAndDef.h
# End Source File
# Begin Source File

SOURCE=.\MyMacro.h
# End Source File
# Begin Source File

SOURCE=.\ObjBasic.h
# End Source File
# Begin Source File

SOURCE=.\ObjLook.h
# End Source File
# Begin Source File

SOURCE=.\PathFinder.h
# End Source File
# Begin Source File

SOURCE=.\Script.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TileMap.h
# End Source File
# End Group
# Begin Group "DirectX 개체 정의"

# PROP Default_Filter "*.h"
# End Group
# End Target
# End Project
