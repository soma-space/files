# Microsoft Developer Studio Project File - Name="IGT4" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=IGT4 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "IGT4.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "IGT4.mak" CFG="IGT4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "IGT4 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "IGT4 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SOMA/Soma Tool/IGT4", THAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "IGT4 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O1 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# SUBTRACT BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 Release/IGObject_Release.lib DDraw.lib DXGuid.lib /nologo /subsystem:windows /map /machine:I386 /nodefaultlib:"LIBCD" /out:"IGT4_Release.exe"

!ELSEIF  "$(CFG)" == "IGT4 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 Debug/IGObject_Debug.lib DDraw.lib DXGuid.lib /nologo /subsystem:windows /map /debug /machine:I386 /nodefaultlib:"LIBCD.LIB" /out:"IGT4_Debug.exe" /pdbtype:sept
# SUBTRACT LINK32 /nodefaultlib

!ENDIF 

# Begin Target

# Name "IGT4 - Win32 Release"
# Name "IGT4 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_TILE_SUB_SET_DATA_TOOL.cpp
# End Source File
# Begin Source File

SOURCE=.\AniObjTool.cpp
# End Source File
# Begin Source File

SOURCE=.\DDEngineTool4.cpp
# End Source File
# Begin Source File

SOURCE=.\DirTreeCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgAniObj.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgBMPConvert.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgColor.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgColorBltTest.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgFolderSelect.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHouseInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHouseOption.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgIntf.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgItem.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLook.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgResrc.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgScript.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSetCenter.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSprAutoExtract.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgSprMerge.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTileMap.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgTileMapObj.cpp
# End Source File
# Begin Source File

SOURCE=.\font.cpp
# End Source File
# Begin Source File

SOURCE=.\HouseInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\HouseInfoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\HouseNumberDialog.cpp
# End Source File
# Begin Source File

SOURCE=.\IGT4.cpp
# End Source File
# Begin Source File

SOURCE=.\IGT4.rc
# End Source File
# Begin Source File

SOURCE=.\IGT4Dlg.cpp
# End Source File
# Begin Source File

SOURCE=.\IntfTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemTool.cpp
# End Source File
# Begin Source File

SOURCE=.\LightSource.cpp
# End Source File
# Begin Source File

SOURCE=.\LightSourceTool.cpp
# End Source File
# Begin Source File

SOURCE=.\MapObjTool.cpp
# End Source File
# Begin Source File

SOURCE=.\MouseSelectManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MoveCell.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjLookTool.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjTool.cpp
# End Source File
# Begin Source File

SOURCE=.\picture.cpp
# End Source File
# Begin Source File

SOURCE=.\ScriptTool.cpp
# End Source File
# Begin Source File

SOURCE=.\spreadsheet.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TileMapTool.cpp
# End Source File
# Begin Source File

SOURCE=.\TileSetTool.cpp
# End Source File
# Begin Source File

SOURCE=.\WndAniObj.cpp
# End Source File
# Begin Source File

SOURCE=.\WndAniTilePreview.cpp
# End Source File
# Begin Source File

SOURCE=.\WndIntf.cpp
# End Source File
# Begin Source File

SOURCE=.\WndObjPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\WndTileMap.cpp
# End Source File
# Begin Source File

SOURCE=.\WndTileMapPreview.cpp
# End Source File
# Begin Source File

SOURCE=.\WndTileSetPreview.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\_TILE_SUB_SET_DATA_TOOL.h
# End Source File
# Begin Source File

SOURCE=.\AniObjTool.h
# End Source File
# Begin Source File

SOURCE=.\DDEngineTool4.h
# End Source File
# Begin Source File

SOURCE=.\DirTreeCtrl.h
# End Source File
# Begin Source File

SOURCE=.\DlgAniObj.h
# End Source File
# Begin Source File

SOURCE=.\DlgBMPConvert.h
# End Source File
# Begin Source File

SOURCE=.\DlgColor.h
# End Source File
# Begin Source File

SOURCE=.\DlgColorBltTest.h
# End Source File
# Begin Source File

SOURCE=.\DlgFolderSelect.h
# End Source File
# Begin Source File

SOURCE=.\DlgHouseInfo.h
# End Source File
# Begin Source File

SOURCE=.\DlgHouseOption.h
# End Source File
# Begin Source File

SOURCE=.\DlgIntf.h
# End Source File
# Begin Source File

SOURCE=.\DlgItem.h
# End Source File
# Begin Source File

SOURCE=.\DlgLook.h
# End Source File
# Begin Source File

SOURCE=.\DlgResrc.h
# End Source File
# Begin Source File

SOURCE=.\DlgScript.h
# End Source File
# Begin Source File

SOURCE=.\DlgSetCenter.h
# End Source File
# Begin Source File

SOURCE=.\DlgSprAutoExtract.h
# End Source File
# Begin Source File

SOURCE=.\DlgSprMerge.h
# End Source File
# Begin Source File

SOURCE=.\DlgTileMap.h
# End Source File
# Begin Source File

SOURCE=.\DlgTileMapObj.h
# End Source File
# Begin Source File

SOURCE=.\font.h
# End Source File
# Begin Source File

SOURCE=.\GlobalVar.h
# End Source File
# Begin Source File

SOURCE=.\HouseInfo.h
# End Source File
# Begin Source File

SOURCE=.\HouseInfoManager.h
# End Source File
# Begin Source File

SOURCE=.\HouseNumberDialog.h
# End Source File
# Begin Source File

SOURCE=.\IGT4.h
# End Source File
# Begin Source File

SOURCE=.\IGT4Dlg.h
# End Source File
# Begin Source File

SOURCE=.\IntfTool.h
# End Source File
# Begin Source File

SOURCE=.\ItemTool.h
# End Source File
# Begin Source File

SOURCE=.\LightSource.h
# End Source File
# Begin Source File

SOURCE=.\LightSourceTool.h
# End Source File
# Begin Source File

SOURCE=.\MapObjTool.h
# End Source File
# Begin Source File

SOURCE=.\MouseSelectManager.h
# End Source File
# Begin Source File

SOURCE=.\MoveCell.h
# End Source File
# Begin Source File

SOURCE=.\ObjLookTool.h
# End Source File
# Begin Source File

SOURCE=.\ObjTool.h
# End Source File
# Begin Source File

SOURCE=.\picture.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\ScriptTool.h
# End Source File
# Begin Source File

SOURCE=.\spreadsheet.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TileMapTool.h
# End Source File
# Begin Source File

SOURCE=.\TileSetTool.h
# End Source File
# Begin Source File

SOURCE=.\WndAniObj.h
# End Source File
# Begin Source File

SOURCE=.\WndAniTilePreview.h
# End Source File
# Begin Source File

SOURCE=.\WndIntf.h
# End Source File
# Begin Source File

SOURCE=.\WndObjPreview.h
# End Source File
# Begin Source File

SOURCE=.\WndTileMap.h
# End Source File
# Begin Source File

SOURCE=.\WndTileMapPreview.h
# End Source File
# Begin Source File

SOURCE=.\WndTileSetPreview.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\res\Animation_Sprite Tool.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Auto_Sprite Tool.ico"
# End Source File
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\c_normal.cur
# End Source File
# Begin Source File

SOURCE=.\res\IGT4.ico
# End Source File
# Begin Source File

SOURCE=.\res\IGT4.rc2
# End Source File
# Begin Source File

SOURCE=".\res\Map_Tile Tool.ico"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
# Section IGT4 : {233BE5C0-4D75-11D2-B8B2-0000C00A958C}
# 	2:5:Class:CSpreadSheet
# 	2:10:HeaderFile:spreadsheet.h
# 	2:8:ImplFile:spreadsheet.cpp
# End Section
# Section IGT4 : {BEF6E003-A874-101A-8BBA-00AA00300CAB}
# 	2:5:Class:COleFont
# 	2:10:HeaderFile:font.h
# 	2:8:ImplFile:font.cpp
# End Section
# Section IGT4 : {7BF80981-BF32-101A-8BBB-00AA00300CAB}
# 	2:5:Class:CPicture
# 	2:10:HeaderFile:picture.h
# 	2:8:ImplFile:picture.cpp
# End Section
# Section IGT4 : {E2454650-4D87-11D2-B8B2-0000C00A958C}
# 	2:21:DefaultSinkHeaderFile:spreadsheet.h
# 	2:16:DefaultSinkClass:CSpreadSheet
# End Section
