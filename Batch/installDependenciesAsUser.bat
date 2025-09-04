set SCRIPT2=%~dp0installDependencies.bat

echo Starte das Skript als aktueller Benutzer...
powershell -NoProfile -Command "Start-Process '%SCRIPT2%'"