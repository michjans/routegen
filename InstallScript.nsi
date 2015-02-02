!include MUI2.nsh

; The name of the installer
Name "Route Generator"

; The file to write
OutFile "routegen-win32-1.7.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Route Generator"

; The text to prompt the user to enter a directory
DirText "This will install Route Generator v1.7 on your computer. Choose a directory where Route Generator should be installed."

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\MJProductions\Route Generator" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Variables
  Var StartMenuFolder

;Interface Settings
!define MUI_ABORTWARNING

;--------------------------------
;Icons
!define MUI_ICON "routegen.ico"
!define MUI_UNICON "routegen.ico"

;--------------------------------
;Pages
!insertmacro MUI_PAGE_LICENSE "redist\LICENSE"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY

;Start Menu Folder Page Configuration
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\MJProductions\Route Generator" 
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  

!insertmacro MUI_PAGE_INSTFILES
  
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
;Languages
 
!insertmacro MUI_LANGUAGE "English"

; The stuff to install
Section "Route Generator (required)" MainInstall

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r redist\*.*
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "Software\MJProductions\Route Generator" "Install_Dir" "$INSTDIR"
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator" "DisplayName" "Route Generator"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator" "NoRepair" 1  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts" StartMenu
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
  ;Create shortcuts
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Route Generator.lnk" "$INSTDIR\routegen.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END
 
SectionEnd

Section /o "Desktop Shortcut" DesktopShortcut
    SetShellVarContext current
    CreateShortCut "$DESKTOP\Route Generator.lnk" "$INSTDIR\routegen.exe"
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator"
  DeleteRegKey HKLM "Software\MJProductions"
  DeleteRegKey HKCU "Software\MJProductions"

  ; Remove files and uninstaller
  Delete $INSTDIR\*.*

  ; Remove shortcuts, if any
  Delete "$DESKTOP\Route Generator.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Route Generator"
  RMDir /r "$INSTDIR"
  
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Route Generator.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
SectionEnd
