# Microsoft Developer Studio Project File - Name="Soma" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Soma - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Soma.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Soma.mak" CFG="Soma - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Soma - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Soma - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/SOMA/SomaClient", CAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Soma - Win32 Release"

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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib winmm.lib dinput.lib dxguid.lib IGObject/IGObject_Release.lib DManage\Release\DManage.lib MapManager/Release/MapManager.lib /nologo /subsystem:windows /map /machine:I386

!ELSEIF  "$(CFG)" == "Soma - Win32 Debug"

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
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ddraw.lib winmm.lib dinput.lib dxguid.lib IGObject/IGObject_Debug.lib DManage/Debug/DManage.lib MapManager/Debug/MapManager.lib /nologo /subsystem:windows /profile /map /debug /machine:I386

!ENDIF 

# Begin Target

# Name "Soma - Win32 Release"
# Name "Soma - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\_tile_sub_set_data_tool.cpp
# End Source File
# Begin Source File

SOURCE=.\Armature.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmatureManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ArmatureRes.cpp
# End Source File
# Begin Source File

SOURCE=.\Arrow.cpp
# End Source File
# Begin Source File

SOURCE=.\BaseDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\BB_CircularBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\BB_Socket.cpp
# End Source File
# Begin Source File

SOURCE=.\BBDX_ChatEditor.cpp
# End Source File
# Begin Source File

SOURCE=.\BBDX_ChatOuter.cpp
# End Source File
# Begin Source File

SOURCE=.\BBDX_Editor.cpp
# End Source File
# Begin Source File

SOURCE=.\BBDX_TextOuter.cpp
# End Source File
# Begin Source File

SOURCE=.\BBDX_TextOuterWithScroll.cpp
# End Source File
# Begin Source File

SOURCE=.\Blood.cpp
# End Source File
# Begin Source File

SOURCE=.\BloodRes.cpp
# End Source File
# Begin Source File

SOURCE=.\BuildHouseManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CharManager.cpp
# End Source File
# Begin Source File

SOURCE=.\CharRes.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildView.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_AccountLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Auctionbuy.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_AuctionDetail.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_AuctionSell.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_BlackMagic.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_BlueMagic.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_BoardCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_BoardList.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_BoardRead.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_BoardWrite.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Chat.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Crest.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_DeadMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_EventMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_EventMessage1.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_EventMessage2.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Exchange.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Guild.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildApply.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildJoin.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu1.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu2.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu3.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu4.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu5.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildName.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildPositionName.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_HowMany.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_HowMuch.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Inv.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Login.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_MagicCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Main.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_MakeItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Memo.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_MemoCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Message.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Message2.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Message3.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Name.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Option.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Quit.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Repair.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Shop.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_SmallMain.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_SpecialMagic.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Status.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_Storage.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_TaxRate.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialog_WhiteMagic.cpp
# End Source File
# Begin Source File

SOURCE=.\DialogManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DirectXMng.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\DxCursor.cpp
# End Source File
# Begin Source File

SOURCE=.\FogEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\GdiFont.cpp
# End Source File
# Begin Source File

SOURCE=.\House.cpp
# End Source File
# Begin Source File

SOURCE=.\HouseInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\HouseInfoManager.cpp
# End Source File
# Begin Source File

SOURCE=.\HousePlaceManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Item.cpp
# End Source File
# Begin Source File

SOURCE=.\Item_Field.cpp
# End Source File
# Begin Source File

SOURCE=.\Item_Inv.cpp
# End Source File
# Begin Source File

SOURCE=.\Item_Small.cpp
# End Source File
# Begin Source File

SOURCE=.\Item_Total.cpp
# End Source File
# Begin Source File

SOURCE=.\LightSource.cpp
# End Source File
# Begin Source File

SOURCE=.\Magic.cpp
# End Source File
# Begin Source File

SOURCE=.\MagicComet.cpp
# End Source File
# Begin Source File

SOURCE=.\magicitem.cpp
# End Source File
# Begin Source File

SOURCE=.\MagicManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MagicRes.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\Msg.cpp
# End Source File
# Begin Source File

SOURCE=.\MyInvData.cpp
# End Source File
# Begin Source File

SOURCE=.\MyMagicData.cpp
# End Source File
# Begin Source File

SOURCE=.\Notice.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectClass.cpp
# End Source File
# Begin Source File

SOURCE=.\PacketParser.cpp
# End Source File
# Begin Source File

SOURCE=.\PathFind.cpp
# End Source File
# Begin Source File

SOURCE=.\Rain.cpp
# End Source File
# Begin Source File

SOURCE=.\SLoadingView.cpp
# End Source File
# Begin Source File

SOURCE=.\SocketMng.cpp
# End Source File
# Begin Source File

SOURCE=.\Soma.cpp
# End Source File
# Begin Source File

SOURCE=.\Soma.rc
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\SWater.cpp
# End Source File
# Begin Source File

SOURCE=.\tilemaptool.cpp
# End Source File
# Begin Source File

SOURCE=.\tilesettool.cpp
# End Source File
# Begin Source File

SOURCE=.\TransObj.cpp
# End Source File
# Begin Source File

SOURCE=.\TWFont.cpp
# End Source File
# Begin Source File

SOURCE=.\UIHorzScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\UIRes.cpp
# End Source File
# Begin Source File

SOURCE=.\UIScrollBar.cpp
# End Source File
# Begin Source File

SOURCE=.\User.cpp
# End Source File
# Begin Source File

SOURCE=.\UserManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Weather.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\_tile_sub_set_data_tool.h
# End Source File
# Begin Source File

SOURCE=.\Armature.h
# End Source File
# Begin Source File

SOURCE=.\ArmatureManager.h
# End Source File
# Begin Source File

SOURCE=.\ArmatureRes.h
# End Source File
# Begin Source File

SOURCE=.\Arrow.h
# End Source File
# Begin Source File

SOURCE=.\BaseDraw.h
# End Source File
# Begin Source File

SOURCE=.\BB_CircularBuffer.h
# End Source File
# Begin Source File

SOURCE=.\BB_Socket.h
# End Source File
# Begin Source File

SOURCE=.\BBDX_ChatEditor.h
# End Source File
# Begin Source File

SOURCE=.\BBDX_ChatOuter.h
# End Source File
# Begin Source File

SOURCE=.\BBDX_Editor.h
# End Source File
# Begin Source File

SOURCE=.\BBDX_TextOuter.h
# End Source File
# Begin Source File

SOURCE=.\BBDX_TextOuterWithScroll.h
# End Source File
# Begin Source File

SOURCE=.\Blood.h
# End Source File
# Begin Source File

SOURCE=.\BloodRes.h
# End Source File
# Begin Source File

SOURCE=.\BuildHouseManager.h
# End Source File
# Begin Source File

SOURCE=.\CharManager.h
# End Source File
# Begin Source File

SOURCE=.\CharRes.h
# End Source File
# Begin Source File

SOURCE=.\ChildView.h
# End Source File
# Begin Source File

SOURCE=.\DarkDef.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_AccountLogin.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Auctionbuy.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_AuctionDetail.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_AuctionSell.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_BlackMagic.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_BlueMagic.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_BoardCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_BoardList.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_BoardRead.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_BoardWrite.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Chat.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Crest.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_DeadMain.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_EventMessage.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_EventMessage1.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_EventMessage2.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Exchange.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Guild.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildApply.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildJoin.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu1.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu2.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu3.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu4.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildMenu5.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildName.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_GuildPositionName.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_HowMany.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_HowMuch.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Inv.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Login.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_MagicCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Main.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_MakeItem.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Memo.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_MemoCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Message.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Message2.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Message3.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Name.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Option.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Quit.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Repair.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Shop.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_SmallMain.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_SpecialMagic.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Status.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_Storage.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_TaxRate.h
# End Source File
# Begin Source File

SOURCE=.\Dialog_WhiteMagic.h
# End Source File
# Begin Source File

SOURCE=.\DialogManager.h
# End Source File
# Begin Source File

SOURCE=.\DirectXMng.h
# End Source File
# Begin Source File

SOURCE=.\DlgLogin.h
# End Source File
# Begin Source File

SOURCE=.\DxCursor.h
# End Source File
# Begin Source File

SOURCE=.\FogEffect.h
# End Source File
# Begin Source File

SOURCE=.\GdiFont.h
# End Source File
# Begin Source File

SOURCE=.\GuildDef.h
# End Source File
# Begin Source File

SOURCE=.\House.h
# End Source File
# Begin Source File

SOURCE=.\HouseInfo.h
# End Source File
# Begin Source File

SOURCE=.\HouseInfoManager.h
# End Source File
# Begin Source File

SOURCE=.\HousePlaceManager.h
# End Source File
# Begin Source File

SOURCE=.\illstruct.h
# End Source File
# Begin Source File

SOURCE=.\Item.h
# End Source File
# Begin Source File

SOURCE=.\Item_Field.h
# End Source File
# Begin Source File

SOURCE=.\Item_Inv.h
# End Source File
# Begin Source File

SOURCE=.\Item_Small.h
# End Source File
# Begin Source File

SOURCE=.\Item_Total.h
# End Source File
# Begin Source File

SOURCE=.\LightSource.h
# End Source File
# Begin Source File

SOURCE=.\Magic.h
# End Source File
# Begin Source File

SOURCE=.\MagicComet.h
# End Source File
# Begin Source File

SOURCE=.\MagicItem.h
# End Source File
# Begin Source File

SOURCE=.\MagicManager.h
# End Source File
# Begin Source File

SOURCE=.\MagicRes.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Msg.h
# End Source File
# Begin Source File

SOURCE=.\MyInvData.h
# End Source File
# Begin Source File

SOURCE=.\MyMagicData.h
# End Source File
# Begin Source File

SOURCE=.\Notice.h
# End Source File
# Begin Source File

SOURCE=.\ObjectClass.h
# End Source File
# Begin Source File

SOURCE=.\PacketDef.h
# End Source File
# Begin Source File

SOURCE=.\PacketParser.h
# End Source File
# Begin Source File

SOURCE=.\PathFind.h
# End Source File
# Begin Source File

SOURCE=.\Rain.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\SLoadingView.h
# End Source File
# Begin Source File

SOURCE=.\SMProtocol.h
# End Source File
# Begin Source File

SOURCE=.\SocketMng.h
# End Source File
# Begin Source File

SOURCE=.\Soma.h
# End Source File
# Begin Source File

SOURCE=.\SoundDef.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\SWater.h
# End Source File
# Begin Source File

SOURCE=.\tilemaptool.h
# End Source File
# Begin Source File

SOURCE=.\tilesettool.h
# End Source File
# Begin Source File

SOURCE=.\TransObj.h
# End Source File
# Begin Source File

SOURCE=.\TWFont.h
# End Source File
# Begin Source File

SOURCE=.\UIHorzScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\UIRes.h
# End Source File
# Begin Source File

SOURCE=.\UIScrollBar.h
# End Source File
# Begin Source File

SOURCE=.\User.h
# End Source File
# Begin Source File

SOURCE=.\UserManager.h
# End Source File
# Begin Source File

SOURCE=.\Weather.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\res\Soma.ico
# End Source File
# Begin Source File

SOURCE=.\res\Soma.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
