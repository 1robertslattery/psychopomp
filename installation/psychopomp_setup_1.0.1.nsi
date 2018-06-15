!define PRODUCT_NAME "Psychopomp"
!define PRODUCT_FILE "psychopomp_setup_1.0.1"
!define PRODUCT_VERSION "1.0.1"
!define PRODUCT_PUBLISHER "Robert Slattery"
!define PRODUCT_REG_ROOT "HKLM"
!define PRODUCT_REG_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define MUI_ICON "C:\Users\Robert\Desktop\app_icon.ico"
!define MUI_UNICON "C:\Program Files (x86)\NSIS\Contrib\Graphics\Icons\nsis3-uninstall.ico"
!define UNINST_LOG "uninstall.log"
Var UNINST_LOG

!include "C:\Program Files (x86)\NSIS\Include\LogicLib.nsh"
!include "C:\Program Files (x86)\NSIS\Include\UnInstallLog.nsh"
!include "C:\Program Files (x86)\NSIS\Contrib\UltraModernUI\UMUI.nsh"
!include "C:\Program Files (x86)\NSIS\Contrib\Modern UI 2\MUI2.nsh"

; name the installer
Name "${PRODUCT_NAME}"
OutFile "${PRODUCT_FILE}.exe"
ShowInstDetails show
ShowUninstDetails show

; set install directory
InstallDir "$PROGRAMFILES\psychopomp"
InstallDirRegKey ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "InstallDir"

RequestExecutionLevel admin

;AddItem macro
!define AddItem "!insertmacro AddItem"
 
;BackupFile macro
!define BackupFile "!insertmacro BackupFile" 
 
;BackupFiles macro
!define BackupFiles "!insertmacro BackupFiles" 
 
;Copy files macro
!define CopyFiles "!insertmacro CopyFiles"
 
;CreateDirectory macro
!define CreateDirectory "!insertmacro CreateDirectory"
 
;CreateShortcut macro
!define CreateShortCut "!insertmacro CreateShortCut"

;File macro
!define File "!insertmacro File"
 
;Rename macro
!define Rename "!insertmacro Rename"
 
;RestoreFile macro
!define RestoreFile "!insertmacro RestoreFile"    
 
;RestoreFiles macro
!define RestoreFiles "!insertmacro RestoreFiles"
 
;SetOutPath macro
!define SetOutPath "!insertmacro SetOutPath"
 
;WriteRegDWORD macro
!define WriteRegDWORD "!insertmacro WriteRegDWORD" 
 
;WriteRegStr macro
!define WriteRegStr "!insertmacro WriteRegStr"
 
;WriteUninstaller macro
!define WriteUninstaller "!insertmacro WriteUninstaller"

; UI Settings
	!define UMUI_UNIQUEBGIMAGE
	!define UMUI_UNUNIQUEBGIMAGE

	!define UMUI_PAGEBGIMAGE
	!define UMUI_UNPAGEBGIMAGE
	!define UMUI_PAGEBGIMAGE_BMP "C:\Users\Robert\Desktop\page_bkgd.bmp"
	
	!define MUI_ABORTWARNING
	!define MUI_UNABORTWARNING
	
	!define UMUI_USE_ALTERNATE_PAGE
	!define UMUI_USE_UNALTERNATE_PAGE
 
	!define MUI_LICENSEPAGE_RADIOBUTTONS
	!define MUI_PAGE_CUSTOMFUNCTION_SHOW ChangeFont
	
 ; Pages

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE "C:\Users\Robert\Desktop\lgpl_license.txt"
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro UMUI_PAGE_CONFIRM
	!insertmacro MUI_PAGE_INSTFILES
	
		!define MUI_FINISHPAGE_NOAUTOCLOSE
		!define MUI_FINISHPAGE_TEXT "You have successfully installed Psychopomp! Launch it from the desktop icon or application folder."
		!define MUI_FINISHPAGE_RUN
		!define MUI_FINISHPAGE_RUN_NOTCHECKED
		!define MUI_FINISHPAGE_RUN_TEXT "Launch Psychopomp"
		!define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
	!insertmacro MUI_PAGE_FINISH
	
	!insertmacro MUI_UNPAGE_WELCOME
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro UMUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES
	!insertmacro MUI_UNPAGE_FINISH
	
;Languages

	!insertmacro MUI_LANGUAGE "English"
	!insertmacro MUI_RESERVEFILE_LANGDLL
	
	;Uninstall log file missing.
	LangString UninstLogMissing ${LANG_ENGLISH} "${UNINST_LOG} not found!$\r$\nUninstallation cannot proceed!"
		
Function ChangeFont
    FindWindow $0 "#32770" "" $HWNDPARENT
    GetDlgItem $0 $0 1043
    CreateFont $1 "Arial" 8 400
    SendMessage $0 ${WM_SETFONT} $1 0
	EnableWindow $R0 1
	SetCtlColors $R0 0x000000 0xFFFFFF
FunctionEnd

Section -openlogfile
    CreateDirectory "$INSTDIR\"
    IfFileExists "$INSTDIR\${UNINST_LOG}" +3
      FileOpen $UNINST_LOG "$INSTDIR\${UNINST_LOG}" w
    Goto +4
      SetFileAttributes "$INSTDIR\${UNINST_LOG}" NORMAL
      FileOpen $UNINST_LOG "$INSTDIR\${UNINST_LOG}" a
      FileSeek $UNINST_LOG 0 END
SectionEnd

;Core
Section "Core Components" SecMain
 
	SectionIn RO
	
	${SetOutPath} "$INSTDIR"
	${WriteUninstaller} "$INSTDIR\Uninstall.exe"
	;${AddItem} "$INSTDIR"
	${File} "Qt5Core.dll"
	${File} "Qt5Gui.dll"
	${File} "Qt5Multimedia.dll"
	${File} "Qt5Network.dll"
	${File} "Qt5Widgets.dll"
	${File} "chat.obj"
	${File} "client.obj"
	${File} "main.obj"
	${File} "manager.obj"
	${File} "server.obj"
	${File} "socket.obj"
	${File} "qrc_myresources.obj"
	${File} "moc_chat.obj"
	${File} "moc_client.obj"
	${File} "moc_manager.obj"
	${File} "moc_server.obj"
	${File} "moc_socket.obj"
	${File} "moc_chat.cpp"
	${File} "moc_client.cpp"
	${File} "moc_manager.cpp"
	${File} "moc_predefs.h"
	${File} "moc_server.cpp"
	${File} "moc_socket.cpp"
	${File} "qrc_myresources.cpp"
	${File} "icon.res"
	${File} "image.png"
	${File} "user.txt"
	${File} "anim.txt"
	${File} "psychopomp.exe"
	;{AddItem} "$INSTDIR\Prerequisites"
		
	; Start menu items and read me
	SetShellVarContext all
	${CreateDirectory} "$SMPROGRAMS\psychopomp"
	${CreateShortCut} "$SMPROGRAMS\psychopomp\psychopomp.lnk" "$INSTDIR\psychopomp.exe" "" "$INSTDIR\psychopomp.exe" 0
	${CreateShortCut} "$SMPROGRAMS\psychopomp\UninstallPsychopomp.lnk" "$INSTDIR\Uninstall.exe" "" "$INSTDIR\Uninstall.exe" 0
	
	; Uninstall keys for Windows
	${WriteRegStr} ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "DisplayName" "${PRODUCT_NAME}"
	${WriteRegStr} ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "InstallDir" "$INSTDIR"
	${WriteRegStr} ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
	${WriteRegDWORD} ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "NoModify" 1
	${WriteRegDWORD} ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "NoRepair" 1
	${WriteRegStr} ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "DisplayIcon" "${MUI_ICON}"
	${WriteRegStr} ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
	${WriteRegStr} ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
		
SectionEnd

; Dependencies
Section "Prerequisites" SecPrereq

	SetShellVarContext all
	;${AddItem} "$INSTDIR\Prerequisites"
	${SetOutPath} "$INSTDIR"
	MessageBox MB_YESNO "Install Visual C++ 2015 runtime dependencies? Select YES if you have not already done this on this computer." /SD IDYES IDNO endDependencies
		${File} "vc_redist.x64.exe"
		ExecWait '"$INSTDIR\vc_redist.x64.exe"'
		Goto endDependencies
	endDependencies:
	
SectionEnd

; Desktop Shortcut
Section "Create Desktop Shortcut" SecShortcut

	SetShellVarContext all
	${CreateShortCut} "$DESKTOP\psychopomp.lnk" "$INSTDIR\psychopomp.exe" "" "$INSTDIR\psychopomp.exe" 0

SectionEnd

Function .onInit
	InitPluginsDir
	File /oname=$PLUGINSDIR\splash.bmp "C:\Users\Robert\Desktop\splash.bmp"
	splash::show 4000 $PLUGINSDIR\splash
	Pop $0 ; $0 has '1' if the user closed the splash screen early,
			; '0' if everything closed normally, and '-1' if some error occurred.
	!insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section "Uninstall"

	SetShellVarContext all
	
	IfFileExists "$INSTDIR\${UNINST_LOG}" +3
	  MessageBox MB_OK|MB_ICONSTOP "$(UninstLogMissing)"
	     Abort
 
	Push $R0
	Push $R1
	Push $R2
	SetFileAttributes "$INSTDIR\${UNINST_LOG}" NORMAL
	FileOpen $UNINST_LOG "$INSTDIR\${UNINST_LOG}" r
  
	StrCpy $R1 -1
 
	GetLineCount:
      ClearErrors
      FileRead $UNINST_LOG $R0
      IntOp $R1 $R1 + 1
      StrCpy $R0 $R0 -2
      Push $R0   
      IfErrors 0 GetLineCount

	Pop $R0
 
	LoopRead:
      StrCmp $R1 0 LoopDone
      Pop $R0
 
      IfFileExists "$R0\*.*" 0 NotDir
        RMDir $R0  #is dir
		Goto LoopNext
	  NotDir:	
      IfFileExists $R0 0 NotFile
        Delete $R0 #is file
	    Goto LoopNext	
	  NotFile:
	  StrCmp $R0 "${PRODUCT_REG_ROOT} ${PRODUCT_REG_KEY}" 0 +2
        DeleteRegKey ${PRODUCT_REG_ROOT} "${PRODUCT_REG_KEY}" #is Reg Element
	  
	  LoopNext:
      IntOp $R1 $R1 - 1
      Goto LoopRead
	LoopDone:
	FileClose $UNINST_LOG
	Delete "$INSTDIR\${UNINST_LOG}"
	RMDir "$INSTDIR"
	Pop $R2
	Pop $R1
	Pop $R0
	
SectionEnd
	
Function un.onInit
  !insertmacro MUI_UNGETLANGUAGE
FunctionEnd

Function .onMouseOverSection
    FindWindow $R0 "#32770" "" $HWNDPARENT
    GetDlgItem $R0 $R0 1043
	
	${If} $0 == -1
		CreateFont $1 "Arial" 9 700
        SetCtlColors $R0 0x000000 0xFFFFFF
		SendMessage $R0 ${WM_SETFONT} $1 0
		SendMessage $R0 ${WM_SETTEXT} 0 "STR:Position your mouse over a component to see its description"
	${Else}
		CreateFont $1 "Arial" 9 700
        SetCtlColors $R0 0x000000 0xFFFFFF
		SendMessage $R0 ${WM_SETFONT} $1 0
		${If} $0 == ${SecMain}
			SendMessage $R0 ${WM_SETTEXT} 0 "STR:Installs the application and its components."
		${ElseIf} $0 == ${SecPrereq}
			SendMessage $R0 ${WM_SETTEXT} 0 "STR:Installs Visual C++ 2015 runtime dependencies. WARNING: The app will NOT run without these files."
		${ElseIf} $0 == ${SecShortcut}
			SendMessage $R0 ${WM_SETTEXT} 0 "STR:Want a desktop shortcut?"
		${EndIf}
	${EndIf}
FunctionEnd

Function LaunchLink
  ExecShell "" "$INSTDIR\psychopomp.exe"
FunctionEnd