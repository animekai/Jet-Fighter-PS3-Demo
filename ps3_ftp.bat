@echo off
title PS3 FTP Manager - ps3-fighter (192.168.100.82)
mode con: cols=74 lines=24
:MENU
cls
color 0B
echo ========================================================================
echo                  GESTOR FTP PARA PLAYSTATION 3 (HEN)
echo                  Proyecto: ps3-fighter (192.168.100.82)
echo ========================================================================
echo.
color 0E
echo   [!] ADVERTENCIA IMPORTANTE:
echo       Asegurate de tener HEN ACTIVADO en tu PS3 Super Slim y el
echo       servidor FTP activo (webMAN MOD / multiMAN) en el puerto 21.
echo.
color 0B
echo ------------------------------------------------------------------------
echo   [1] Enviar ps3-fighter_0.2.pkg a la PS3 (/dev_hdd0/packages)
echo   [2] Extraer log de debug desde la PS3 (/dev_hdd0/ps3_fighter.log -^> F:\)
echo   [3] Salir
echo ------------------------------------------------------------------------
echo.
set /p op="Selecciona una opcion (1, 2 o 3): "

if "%op%"=="1" goto SUBIR
if "%op%"=="2" goto BAJAR
if "%op%"=="3" exit
goto MENU

:SUBIR
cls
color 0A
echo Subiendo ps3-fighter_0.2.pkg a 192.168.100.82 (/dev_hdd0/packages)...
(
echo open 192.168.100.82 21
echo user anonymous anonymous
echo binary
echo cd /dev_hdd0/packages
echo put F:\ps3-fighter\ps3-fighter_0.2.pkg
echo bye
) > "%TEMP%\ftp_ps3.txt"
ftp -n -s:"%TEMP%\ftp_ps3.txt"
del "%TEMP%\ftp_ps3.txt"
echo.
echo Proceso finalizado. Revisa la consola en Administrar archivos PKG.
pause
goto MENU

:BAJAR
cls
color 0D
echo Descargando /dev_hdd0/ps3_fighter.log hacia F:\ps3_fighter.log...
(
echo open 192.168.100.82 21
echo user anonymous anonymous
echo binary
echo get /dev_hdd0/ps3_fighter.log F:\ps3_fighter.log
echo bye
) > "%TEMP%\ftp_ps3.txt"
ftp -n -s:"%TEMP%\ftp_ps3.txt"
del "%TEMP%\ftp_ps3.txt"
echo.
if exist "F:\ps3_fighter.log" (
    echo === CONTENIDO DEL LOG ===
    type "F:\ps3_fighter.log"
    echo =========================
) else (
    echo [X] No se encontro el log en la PS3.
)
echo.
pause
goto MENU
