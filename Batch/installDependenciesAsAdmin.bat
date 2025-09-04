set SCRIPT2=%~dp0installDependencies.bat

echo Starte das Skript als Administrator...
powershell -Command "Start-Process '%SCRIPT2%' -Verb runAs"