Unicode True
Name "LayerMind 3D for Bambu Studio"
OutFile "${OUTFILE}"
InstallDir "$LOCALAPPDATA\Programs\LayerMind3D"
InstallDirRegKey HKCU "Software\LayerMind3D" "InstallDir"
RequestExecutionLevel user

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "LayerMind 3D" SEC_MAIN
  SetOutPath "$INSTDIR"
  File /r "${ROOTDIR}\*.*"
  WriteUninstaller "$INSTDIR\Uninstall-LayerMind3D.exe"
  WriteRegStr HKCU "Software\LayerMind3D" "InstallDir" "$INSTDIR"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\LayerMind3D" "DisplayName" "LayerMind 3D for Bambu Studio"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\LayerMind3D" "InstallLocation" "$INSTDIR"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\LayerMind3D" "UninstallString" '"$INSTDIR\Uninstall-LayerMind3D.exe"'
  CreateDirectory "$SMPROGRAMS\LayerMind 3D"
  CreateShortcut "$SMPROGRAMS\LayerMind 3D\LayerMind 3D.lnk" "$INSTDIR\BambuStudio.exe" "" "$INSTDIR\BambuStudio.exe" 0
  CreateShortcut "$SMPROGRAMS\LayerMind 3D\Uninstall.lnk" "$INSTDIR\Uninstall-LayerMind3D.exe"
  CreateShortcut "$DESKTOP\LayerMind 3D.lnk" "$INSTDIR\BambuStudio.exe" "" "$INSTDIR\BambuStudio.exe" 0
SectionEnd

Section "Uninstall"
  Delete "$DESKTOP\LayerMind 3D.lnk"
  RMDir /r "$SMPROGRAMS\LayerMind 3D"
  DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\LayerMind3D"
  DeleteRegKey HKCU "Software\LayerMind3D"
  RMDir /r "$INSTDIR"
SectionEnd
