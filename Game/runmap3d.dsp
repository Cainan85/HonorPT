# Microsoft Developer Studio Project File - Name="runmap3d" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=runmap3d - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "runmap3d.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "runmap3d.mak" CFG="runmap3d - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "runmap3d - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "runmap3d - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "runmap3d - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG"
# ADD RSC /l 0x412 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"game.exe"

!ELSEIF  "$(CFG)" == "runmap3d - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "runmap3d___Win32_Debug"
# PROP BASE Intermediate_Dir "runmap3d___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "runmap3d___Win32_Debug"
# PROP Intermediate_Dir "runmap3d___Win32_Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG"
# ADD RSC /l 0x412 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 dsound.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"game.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "runmap3d - Win32 Release"
# Name "runmap3d - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "smD3D"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\smlib3d\smDsx.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smgeosub.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smJpeg.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smMap3d.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smmatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smObj3d.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smRead3d.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smRend3d.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smSin.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smStage3d.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smStgObj.cpp
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smTexture.cpp
# End Source File
# End Group
# Begin Group "SinBaRam"

# PROP Default_Filter ""
# Begin Group "header"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\sinbaram\sin3D.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinCharStatus.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinCraftItem.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinInterFace.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinInvenTory.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinItem.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinLinkHeader.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinMain.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinShop.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinSkill.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinSkill_Info.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinSubMain.h
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinTrade.h
# End Source File
# End Group
# Begin Group "SkillFunction"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sinbaram\SkillFunction\Tempskron.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\SkillFunction\Tempskron.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\sinbaram\sin3D.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinCharStatus.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinInterFace.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinInvenTory.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinItem.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinMain.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinShop.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinSkill.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinSkill_Info.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinSubMain.cpp
# End Source File
# Begin Source File

SOURCE=.\sinbaram\sinTrade.cpp
# End Source File
# End Group
# Begin Group "sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\srcsound\Dxwav.cpp
# End Source File
# Begin Source File

SOURCE=.\effectsnd.cpp
# End Source File
# Begin Source File

SOURCE=.\srcsound\Wave.cpp
# End Source File
# End Group
# Begin Group "HoDong"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HoBaram\HoAnimData.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoAnimData.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoEffect.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoLinkHeader.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoLinkHeader.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoLogic.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoLogic.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoLogin.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoLogin.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoMessageBox.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoMessageBox.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoNewParticle.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoOpening.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoOpening.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoParticle.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoParticle.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoParty.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoParty.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoPhysics.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoPhysics.h
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoTextFile.cpp
# End Source File
# Begin Source File

SOURCE=.\HoBaram\HoTextFile.h
# End Source File
# End Group
# Begin Group "exc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\smReg.cpp
# End Source File
# End Group
# Begin Group "HanGame"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HanDes.lib
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\SrcServer\gameSQL.cpp
# End Source File
# Begin Source File

SOURCE=.\SrcServer\Gamesql.h
# End Source File
# Begin Source File

SOURCE=.\netplay.cpp
# End Source File
# Begin Source File

SOURCE=.\SrcServer\OnSever.cpp
# End Source File
# Begin Source File

SOURCE=.\SrcServer\PartySever.cpp
# End Source File
# Begin Source File

SOURCE=.\smPacket.h
# End Source File
# End Group
# Begin Group "IME"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Ime.cpp
# End Source File
# Begin Source File

SOURCE=.\Ime.h
# End Source File
# Begin Source File

SOURCE=.\STRCLASS.CPP
# End Source File
# Begin Source File

SOURCE=.\STRCLASS.H
# End Source File
# End Group
# Begin Group "HeaderFile"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\language.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\character.cpp
# End Source File
# Begin Source File

SOURCE=.\cracker.cpp
# End Source File
# Begin Source File

SOURCE=.\Drawsub.cpp
# End Source File
# Begin Source File

SOURCE=.\field.cpp
# End Source File
# Begin Source File

SOURCE=.\fileread.cpp
# End Source File
# Begin Source File

SOURCE=.\makeshadow.cpp
# End Source File
# Begin Source File

SOURCE=.\Particle.cpp
# End Source File
# Begin Source File

SOURCE=.\pbackground.cpp
# End Source File
# Begin Source File

SOURCE=.\playmain.cpp
# End Source File
# Begin Source File

SOURCE=.\playsub.cpp
# End Source File
# Begin Source File

SOURCE=.\record.cpp
# End Source File
# Begin Source File

SOURCE=.\SkillSub.cpp
# End Source File
# Begin Source File

SOURCE=.\smwsock.cpp
# End Source File
# Begin Source File

SOURCE=.\TextMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\Winmain.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\icon2.ico
# End Source File
# Begin Source File

SOURCE=.\loadimg.bmp
# End Source File
# Begin Source File

SOURCE=.\Loading.bmp
# End Source File
# Begin Source File

SOURCE=.\Script1.rc
# End Source File
# Begin Source File

SOURCE=".\·ÎµùÁß.bmp"
# End Source File
# End Group
# Begin Source File

SOURCE="..\..\Program Files\Microsoft Visual Studio\VC98\Include\basetsd.h"
# End Source File
# Begin Source File

SOURCE=.\cpItemList.dat
# End Source File
# Begin Source File

SOURCE=.\effectsnd.h
# End Source File
# Begin Source File

SOURCE=.\makeshadow.h
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smd3d.h
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smJpeg.h
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smmatrix.h
# End Source File
# Begin Source File

SOURCE=.\smlib3d\smRead3d.h
# End Source File
# End Target
# End Project
