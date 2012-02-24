
; The name of the installer
Name "Route Generator"

; The file to write
OutFile "routegen-win32-1.6.exe"

; The default installation directory
InstallDir "$PROGRAMFILES\Route Generator"

; The text to prompt the user to enter a directory
DirText "This will install Route Generator v1.6 on your computer. Choose a directory where Route Generator should be installed."

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\MJProductions\Route Generator" "Install_Dir"

; Request application privileges for Windows Vista
;RequestExecutionLevel admin

;--------------------------------

; Pages

;Page components
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Route Generator (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File /r redist\*.*
  
  ; Write the installation path into the registry
  WriteRegStr HKLM "Software\MJProductions\Route Generator" "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator" "DisplayName" "Route Generator"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Route Generator"
  CreateShortCut "$SMPROGRAMS\Route Generator\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Route Generator\Route Generator.lnk" "$INSTDIR\routegen.exe" "" "$INSTDIR\routegen.exe" 0
  
SectionEnd

Section /o "Desktop Shortcut" SectionX
    SetShellVarContext current
    CreateShortCut "$DESKTOP\Route Generator.lnk" "$INSTDIR\routegen.exe"
SectionEnd


;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Route Generator"
  DeleteRegKey HKLM SOFTWARE\MJProductions

  ; Remove files and uninstaller
  Delete $INSTDIR\*.*

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Route Generator\*.*"
  Delete "$DESKTOP\Route Generator.lnk"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Route Generator"
  RMDir /r "$INSTDIR"

SectionEnd
