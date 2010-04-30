@echo off
setlocal

"ejs" -c "App.exeDir" >__ejsweb__.tmp
set /p DIR= <__ejsweb__.tmp
del __ejsweb__.tmp
call "ejs" %DIR%/ejsweb %*
