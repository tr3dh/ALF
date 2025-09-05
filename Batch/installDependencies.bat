@echo off
REM Pfad zu winget.exe als Variable speichern
set WINGET_PATH=%USERPROFILE%\AppData\Local\Microsoft\WindowsApps\winget.exe

REM Winget-Quellen aktualisieren
"%WINGET_PATH%" source update

REM Programme installieren
@REM "%WINGET_PATH%" install ImageMagick.Q16-HDRI -e --accept-package-agreements --accept-source-agreements
@REM "%WINGET_PATH%" install --id=oschwartz10612.Poppler -e --accept-package-agreements --accept-source-agreements
@REM "%WINGET_PATH%" install --id=MiKTeX.MiKTeX -e --accept-package-agreements --accept-source-agreements
"%WINGET_PATH%" install --id=Gyan.FFmpeg -e --accept-package-agreements --accept-source-agreements