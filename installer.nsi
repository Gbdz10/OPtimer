!define APP_VERSION "1.3_release"
Name "OPtimer ${APP_VERSION}"
OutFile "OPtimer_${APP_VERSION}-Setup.exe"
InstallDir "$PROGRAMFILES64\OPtimer"
RequestExecutionLevel admin

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Install"
    SetOutPath "$INSTDIR"
    File "build-win\OPtimer.exe"

    CreateDirectory "$SMPROGRAMS\OPtimer"
    CreateShortcut "$SMPROGRAMS\OPtimer\OPtimer.lnk" "$INSTDIR\OPtimer.exe"
    CreateShortcut "$DESKTOP\OPtimer.lnk" "$INSTDIR\OPtimer.exe"

    WriteUninstaller "$INSTDIR\Uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OPtimer" "DisplayName" "OPtimer ${APP_VERSION}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OPtimer" "DisplayVersion" "${APP_VERSION}"
SectionEnd

Section "Uninstall"
    Delete "$INSTDIR\OPtimer.exe"
    Delete "$INSTDIR\Uninstall.exe"
    RMDir "$INSTDIR"
    Delete "$SMPROGRAMS\OPtimer\OPtimer.lnk"
    RMDir "$SMPROGRAMS\OPtimer"
    Delete "$DESKTOP\OPtimer.lnk"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\OPtimer"
SectionEnd
