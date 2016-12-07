@ECHO OFF
SETLOCAL

SET _MSYSPACKAGES=autoconf automake-wrapper autogen bison diffstat dos2unix intltool libtool patch python make zip unzip git subversion wget p7zip mercurial man-db

SET _TOOLSBASEDIR=%~dp0
IF NOT EXIST %_TOOLSBASEDIR% (
  ECHO. You have probably run the script in a path with spaces. This is not supported.
  ECHO. Please correct the path names before continuing.
  PAUSE
  EXIT /B 1
)

SET _BITS=64
IF %PROCESSOR_ARCHITECTURE%==x86 (
  IF NOT DEFINED PROCESSOR_ARCHITEW6432 (
    SET _BITS=32
  )
)

IF %_BITS%==64 (
  SET _MSYS2=msys64
  SET _TOOLSDIR=%_TOOLSBASEDIR%Win64\
  SET _MSYS2DIR=%_TOOLSDIR%msys64\
) ELSE (
  SET _MSYS2=msys32
  SET _TOOLSDIR=%_TOOLSBASEDIR%Win32\
  SET _MSYS2DIR=%_TOOLSDIR%msys32\
)

IF NOT EXIST %_TOOLSDIR% (
  MKDIR %_TOOLSDIR%
)

IF EXIST %_MSYS2DIR%usr\bin\wget.exe (
  GOTO have_msys2_base
)

CD %_TOOLSDIR%
IF EXIST wget.exe (
  IF EXIST 7za.exe (
    IF EXIST grep.exe (
      GOTO have_wget
    )
  )
)

CALL :download_wget
IF ERRORLEVEL 1 (
  EXIT /B 1
)

:have_wget
IF EXIST "%_MSYS2DIR%msys2_shell.bat" (
  GOTO have_msys2_base
)

CALL :download_msys2_base
IF ERRORLEVEL 1 (
  EXIT /B 1
)

:have_msys2_base
SET "_MINTTY=start /I /WAIT %_MSYS2DIR%usr\bin\mintty.exe -d -i /msys2.ico"

IF EXIST %_TOOLSDIR%\mintty.lnk (
  GOTO msys2_updated
)

CALL :update_msys2
IF ERRORLEVEL 1 (
  EXIT /B 1
)

CALL :create_shortcut

:msys2_updated
IF NOT EXIST "%_MSYS2DIR%home\%USERNAME%" (
  MKDIR "%_MSYS2DIR%home\%USERNAME%"
)

IF EXIST "%_MSYS2DIR%home\%USERNAME%\.minttyrc" (
  GOTO has_mintty_settings
)

CALL :set_mintty_settings
IF ERRORLEVEL 1 (
  EXIT /B 1
)

:has_mintty_settings
IF EXIST "%_MSYS2DIR%home\%USERNAME%\.hgrc" (
  GOTO has_hg_settings
)

CALL :set_hg_settings
IF ERRORLEVEL 1 (
  EXIT /B 1
)

:has_hg_settings
IF EXIST "%_MSYS2DIR%home\%USERNAME%\.gitconfig" (
  GOTO has_git_settings
)

CALL :set_git_settings
IF ERRORLEVEL 1 (
  EXIT /B 1
)

:has_git_settings
CALL :create_pacman_base
IF ERRORLEVEL 1 (
  EXIT /B 1
)

IF EXIST "%_MSYS2DIR%usr\bin\make.exe" (
  GOTO base_installed
)

CALL :install_base
IF ERRORLEVEL 1 (
  EXIT /B 1
)

:base_installed
IF EXIST %_MSYS2DIR%usr\bin\hg.bat (
  GOTO has_hg_bat
)

CALL :set_hg_bat
IF ERRORLEVEL 1 (
  EXIT /B 1
)
 
:has_hg_bat

PAUSE
EXIT /B 0

GOTO getmingw32

::---------------------------------------------------------------------------::
:download_wget
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
IF NOT EXIST wget-pack.exe (
  IF EXIST wget.js (
    DEL wget.js
  )

  (
    ECHO./*from http://superuser.com/a/536400*/
    ECHO.var r=new ActiveXObject("WinHttp.WinHttpRequest.5.1"^);
    ECHO.r.Open("GET",WScript.Arguments(0^),false^);r.Send(^);
    ECHO.b=new ActiveXObject("ADODB.Stream"^);
    ECHO.b.Type=1;b.Open(^);b.Write(r.ResponseBody^);
    ECHO.b.SaveToFile(WScript.Arguments(1^)^);
  ) >wget.js
  
  cscript /nologo wget.js http://i.fsbn.eu/pub/wget-pack.exe wget-pack.exe
)

IF EXIST wget-pack.exe (
  wget-pack.exe x
)

IF NOT EXIST wget.exe (
  ECHO -------------------------------------------------------------
  ECHO Script to download necessary components failed.
  ECHO.
  ECHO Download and place inside "!_TOOLSDIR!":
  ECHO https://i.fsbn.eu/pub/wget-pack.exe
  ECHO -------------------------------------------------------------
  PAUSE
  SET _RESULT=1
) ELSE (
  SET _RESULT=0
)

DEL wget.js wget-pack.exe 2>nul

POPD
IF !_RESULT! EQU 1 GOTO exit_error
GOTO exit_success


::---------------------------------------------------------------------------::
:download_msys2_base
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
IF EXIST "!_MSYS2DIR!" (
  GOTO unpacked
)

IF EXIST "!_TOOLSDIR!msys2-base.tar.xz" (
  GOTO unpack
)

IF "!_MSYS2!"=="msys32" (
	SET _PREFIX=i686
) ELSE (
	SET _PREFIX=x86_64
)

wget --tries=5 --retry-connrefused --waitretry=5 --continue -O msys2-base.tar.xz ^
	"http://repo.msys2.org/distrib/msys2-!_PREFIX!-latest.tar.xz"

:unpack
IF NOT EXIST "!_TOOLSDIR!msys2-base.tar.xz" (
  GOTO unpacked
)

7za.exe x msys2-base.tar.xz -bd -bb0 -so | 7za.exe x -bd -bb0 -aoa -si -ttar -o"!_TOOLSDIR!" >NUL  1>&2
rem DEL msys2-base.tar.xz

:unpacked
IF NOT EXIST "!_MSYS2DIR!usr\bin\msys-2.0.dll" (
  ECHO -------------------------------------------------------------
  ECHO Download msys2 basic system failed, please download it manually from:
  ECHO.
  ECHO Download and place inside "!_TOOLSDIR!":
  ECHO http://repo.msys2.org/distrib/
  ECHO -------------------------------------------------------------
  PAUSE
  SET _RESULT=1
) ELSE (
  SET _RESULT=0
)

POPD
IF !_RESULT! EQU 1 GOTO exit_error
GOTO exit_success

::---------------------------------------------------------------------------::
:create_shortcut
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
IF EXIST setlink.vbs (
  DEL setlink.vbs >NUL
)

(
  ECHO.Set Shell = CreateObject^("WScript.Shell"^)
  ECHO.Set link = Shell.CreateShortcut^("!_TOOLSBASEDIR!\mintty.lnk"^)
  ECHO.link.Arguments = "-full-path -mingw"
  ECHO.link.Description = "msys2 shell console"
  ECHO.link.TargetPath = "!_MSYS2DIR!msys2_shell.cmd"
  ECHO.link.WindowStyle = ^1
  ECHO.link.IconLocation = "!_MSYS2DIR!msys2.ico"
  ECHO.link.WorkingDirectory = "!_MSYS2DIR!"
  ECHO.link.Save
) >setlink.vbs

cscript /nologo setlink.vbs
SET _RESULT=%ERRORLEVEL%
  
DEL setlink.vbs >NUL

POPD
IF !_RESULT! EQU 1 GOTO exit_error
GOTO exit_success


::---------------------------------------------------------------------------::
:update_msys2
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
IF !_MSYS2!==msys32 (
  CALL !_MSYS2DIR!autorebase.bat
)

IF EXIST !_TOOLSDIR!firstrun.log (
  DEL !_TOOLSDIR!firstrun.log >NUL
)
IF EXIST !_TOOLSDIR!first_update.log (
  DEL !_TOOLSDIR!first_update.log >NUL
)
IF EXIST !_TOOLSDIR!second_update.log (
  DEL !_TOOLSDIR!second_update.log >NUL
)

!_MINTTY! --log 2>&1 !_TOOLSDIR!firstrun.log /usr/bin/bash --login -c exit

(
  ECHO.echo -ne "\033]0;first msys2 update\007"
  ECHO.pacman --noconfirm -Sy --force --asdeps pacman-mirrors
  ECHO.sed -i "s;^^IgnorePkg.*;#&;" /etc/pacman.conf
  ECHO.sleep ^4
  ECHO.exit
) >!_TOOLSDIR!first_update.sh

!_MINTTY! --log 2>&1 !_TOOLSDIR!first_update.log /usr/bin/bash --login !_TOOLSDIR!first_update.sh
DEL !_TOOLSDIR!first_update.sh >NUL

!_MSYS2DIR!usr\bin\sh.exe -l -c "pacman -S --needed --noconfirm --asdeps bash pacman msys2-runtime"

(
  ECHO.echo -ne "\033]0;second msys2 update\007"
  ECHO.pacman --noconfirm -Syu --force --asdeps
  ECHO.exit
) >!_TOOLSDIR!second_update.sh

!_MINTTY! --log 2>&1 !_TOOLSDIR!second_update.log /usr/bin/bash --login !_TOOLSDIR!second_update.sh
DEL !_TOOLSDIR!second_update.sh >NUL

POPD
GOTO exit_success


::---------------------------------------------------------------------------::
:set_mintty_settings
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
(
  ECHO.BoldAsFont=yes
  ECHO.BackgroundColour=39,40,34
  ECHO.ForegroundColour=248,248,242
  ECHO.Transparency=off
  ECHO.FontHeight=^9
  ECHO.FontSmoothing=default
  ECHO.AllowBlinking=yes
  ECHO.Font=Consolas
  ECHO.Columns=120
  ECHO.Rows=30
  ECHO.Term=xterm-256color
  ECHO.CursorType=block
  ECHO.ClicksPlaceCursor=yes
  ECHO.Locale=en_US
  ECHO.Charset=UTF-8  
  ECHO.Black=39,40,34
  ECHO.Red=249,38,114
  ECHO.Green=166,226,46
  ECHO.Yellow=244,191,117
  ECHO.Blue=102,217,239
  ECHO.Magenta=174,129,255
  ECHO.Cyan=161,239,228
  ECHO.White=248,248,242
  ECHO.BoldBlack=117,113,94
  ECHO.BoldRed=204,6,78
  ECHO.BoldGreen=122,172,24
  ECHO.BoldYellow=240,169,69
  ECHO.BoldBlue=33,199,233
  ECHO.BoldMagenta=126,51,255
  ECHO.BoldCyan=95,227,210
  ECHO.BoldWhite=249,248,245
) >>"%_MSYS2DIR%home\%USERNAME%\.minttyrc"

POPD
GOTO exit_success


::---------------------------------------------------------------------------::
:set_hg_settings
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
(
  ECHO.[ui]
  ECHO.username = %USERNAME%
  ECHO.verbose = True
  ECHO.editor = vim
  ECHO.
  ECHO.[web]
  ECHO.cacerts=/usr/ssl/cert.pem
  ECHO.
  ECHO.[extensions]
  ECHO.color =
  ECHO.
  ECHO.[color]
  ECHO.status.modified = magenta bold
  ECHO.status.added = green bold
  ECHO.status.removed = red bold
  ECHO.status.deleted = cyan bold
  ECHO.status.unknown = blue bold
  ECHO.status.ignored = black bold
) >>"%_MSYS2DIR%home\%USERNAME%\.hgrc"
  
POPD
GOTO exit_success


::---------------------------------------------------------------------------::
:set_git_settings
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
(
  ECHO.[user]
  ECHO.name = %USERNAME%
  ECHO.email = %USERNAME%@%COMPUTERNAME%
  ECHO.
  ECHO.[color]
  ECHO.ui = true
  ECHO.
  ECHO.[core]
  ECHO.editor = vim
  ECHO.autocrlf =
  ECHO.
  ECHO.[merge]
  ECHO.tool = vimdiff
  ECHO.
  ECHO.[push]
  ECHO.default = simple
) >>"%_MSYS2DIR%home\%USERNAME%\.gitconfig"

POPD
GOTO exit_success


::---------------------------------------------------------------------------::
:set_hg_bat
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
(
  ECHO.@echo off
  ECHO.
  ECHO.setlocal
  ECHO.set HG=^%%~f0
  ECHO.
  ECHO.set PYTHONHOME=
  ECHO.set in=^%%*
  ECHO.set out=^%%in: ^{= ^"^{^%%
  ECHO.set out=^%%out:^} =^}^" ^%%
  ECHO.
  ECHO.^%%~dp0python2 ^%%~dp0hg ^%%out^%%
) >>%_MSYS2DIR%usr\bin\hg.bat

POPD
GOTO exit_success


::---------------------------------------------------------------------------::
:create_pacman_base
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
IF EXIST "%_MSYS2DIR%etc\pac-base.pk"  (
  DEL "%_MSYS2DIR%etc\pac-base.pk" >NUL
)

FOR %%i IN (%_MSYSPACKAGES%) DO  (
  ECHO.%%i>>%_MSYS2DIR%etc\pac-base.pk
)

POPD
GOTO exit_success


::---------------------------------------------------------------------------::
:install_base
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
IF EXIST install_base_failed (
  DEL install_base_failed >NUL
)

IF EXIST pacman.log (
  DEL pacman.log >NUL
)

(
  ECHO.echo -ne "\033]0;install base system\007"
  ECHO.msysbasesystem="$(cat /etc/pac-base.pk | tr '\n\r' '  ')"
  ECHO.[[ "$(uname)" = *6.1* ]] ^&^& nargs="-n 4"
  ECHO.echo $msysbasesystem ^| xargs $nargs pacman -Sw --noconfirm --needed
  ECHO.echo $msysbasesystem ^| xargs $nargs pacman -S --noconfirm --needed
  ECHO.echo $msysbasesystem ^| xargs $nargs pacman -D --asexplicit
  ECHO.sleep ^3
  ECHO.exit
) >pacman.sh

%_MINTTY% --log 2>&1 pacman.log /usr/bin/bash --login pacman.sh
DEL pacman.sh >NUL

FOR %%i IN (%_MSYS2DIR%usr\ssl\cert.pem) DO (
  IF %%~zi == 0 (
    (
      ECHO.update-ca-trust
      ECHO.sleep ^3
      ECHO.exit
    ) >cert.sh
    
    IF EXIST cert.log (
      DEL cert.log >NUL
    )
    
    %_MINTTY% --log 2>&1 cert.log /usr/bin/bash --login cert.sh
    DEL cert.sh >NUL
  )
)

POPD
GOTO exit_success

::---------------------------------------------------------------------------::

:exit_success
ENDLOCAL
EXIT /B 0

:exit_error
ENDLOCAL
EXIT /B 1






:getmingw32

IF EXIST %_MSYS2DIR%\mingw32\bin\gcc.exe GOTO getmingw64
ECHO.-------------------------------------------------------------------------------
ECHO.install 32 bit compiler
ECHO.-------------------------------------------------------------------------------
(
    ECHO.echo -ne "\033]0;install 32 bit compiler\007"
    ECHO.mingw32compiler="$(cat /etc/pac-mingw.pk | sed 's;.*;mingw-w64-i686-&;g' | tr '\n\r' '  ')"
    ECHO.[[ "$(uname)" = *6.1* ]] ^&^& nargs="-n 4"
    ECHO.echo $mingw32compiler ^| xargs $nargs pacman -Sw --noconfirm --needed --force
    ECHO.echo $mingw32compiler ^| xargs $nargs pacman -S --noconfirm --needed --force
    ECHO.sleep ^3
    ECHO.exit
) >%_TOOLSDIR%\mingw32.sh
IF EXIST %_TOOLSDIR%\mingw32.log DEL %_TOOLSDIR%\mingw32.log
%_MINTTY% --log 2>&1 %_TOOLSDIR%\mingw32.log /usr/bin/bash --login %_TOOLSDIR%\mingw32.sh
DEL %_TOOLSDIR%\mingw32.sh

IF NOT exist %_MSYS2DIR%\mingw32\bin\gcc.exe (
        echo -------------------------------------------------------------------------------
        ECHO.
        ECHO.MinGW32 GCC compiler isn't installed; maybe the download didn't work
        ECHO.Do you want to try it again?
        ECHO.
        echo -------------------------------------------------------------------------------
        SET /P try32="try again [y/n]: "

        if %packF%==y (
            GOTO getmingw32
        ) ELSE exit
    )
    
    
:getmingw64
if %build64%==yes (
    IF EXIST %_MSYS2DIR%\mingw64\bin\gcc.exe GOTO updatebase
    ECHO.-------------------------------------------------------------------------------
    ECHO.install 64 bit compiler
    ECHO.-------------------------------------------------------------------------------
        (
        ECHO.echo -ne "\033]0;install 64 bit compiler\007"
        ECHO.mingw64compiler="$(cat /etc/pac-mingw.pk | sed 's;.*;mingw-w64-x86_64-&;g' | tr '\n\r' '  ')"
        ECHO.[[ "$(uname)" = *6.1* ]] ^&^& nargs="-n 4"
        ECHO.echo $mingw64compiler ^| xargs $nargs pacman -Sw --noconfirm --needed --force
        ECHO.echo $mingw64compiler ^| xargs $nargs pacman -S --noconfirm --needed --force
        ECHO.sleep ^3
        ECHO.exit
            )>%_TOOLSDIR%\mingw64.sh
    IF EXIST %_TOOLSDIR%\mingw64.log DEL %_TOOLSDIR%\mingw64.log
    %_MINTTY% --log 2>&1 %_TOOLSDIR%\mingw64.log /usr/bin/bash --login %_TOOLSDIR%\mingw64.sh
    DEL %_TOOLSDIR%\mingw64.sh

    IF NOT exist %_MSYS2DIR%\mingw64\bin\gcc.exe (
        echo -------------------------------------------------------------------------------
        ECHO.
        ECHO.MinGW64 GCC compiler isn't installed; maybe the download didn't work
        ECHO.Do you want to try it again?
        ECHO.
        echo -------------------------------------------------------------------------------
        SET /P try64="try again [y/n]: "

        if %packF%==y (
            GOTO getmingw64
            ) ELSE exit
        )
    )

:updatebase
echo.-------------------------------------------------------------------------------
echo.update autobuild suite
echo.-------------------------------------------------------------------------------

cd %_TOOLSDIR%
set scripts=compile helper update
for %%s in (%scripts%) do (
    IF NOT exist "%_TOOLSDIR%\media-suite_%%s.sh" (
        %_MSYS2DIR%\usr\bin\wget.exe -t 20 --retry-connrefused --waitretry=2 -c ^
        https://github.com/jb-alvarado/media-autobuild_suite/raw/master/build/media-suite_%%s.sh
        )
    )
if %updateSuite%==y (
    IF NOT exist %instdir%\update_suite.sh (
        echo -------------------------------------------------------------------------------
        ECHO. Creating suite update file...
        ECHO.
        ECHO. Run this file by dragging it to mintty before the next time you run
        ECHO. the suite and before reporting an issue.
        ECHO.
        ECHO. It needs to be run separately and with the suite not running!
        echo -------------------------------------------------------------------------------
        )
    (
        ECHO.#!/bin/bash
        ECHO.
        ECHO.# Run this file by dragging it to mintty shortcut.
        ECHO.# Be sure the suite is not running before using it!
        ECHO.
        ECHO.update=yes
        %_MSYS2DIR%\usr\bin\sed -n '/start suite update/,/end suite update/p' ^
            %_TOOLSDIR%/media-suite_update.sh
        )>%instdir%\update_suite.sh
    )

IF EXIST %_TOOLSDIR%\update.log DEL %_TOOLSDIR%\update.log
%_MINTTY% --log 2>&1 %_TOOLSDIR%\update.log /usr/bin/bash --login %_TOOLSDIR%\media-suite_update.sh ^
--build32=%build32% --build64=%build64%

IF EXIST "%_TOOLSDIR%\update_core" (
    ECHO.-------------------------------------------------------------------------------
    ECHO.critical updates
    ECHO.-------------------------------------------------------------------------------
    %_MSYS2DIR%\usr\bin\sh.exe -l -c "pacman -S --needed --noconfirm --asdeps bash pacman msys2-runtime"
    DEL "%_TOOLSDIR%\update_core"
    )

:rebase2
if %_MSYS2%==msys32 (
echo.-------------------------------------------------------------------------------
echo.second rebase %_MSYS2% system
echo.-------------------------------------------------------------------------------
call %_MSYS2DIR%\autorebase.bat
)

:createFolders
if %build32%==yes call :createBaseFolders local32
if %build64%==yes call :createBaseFolders local64

:checkFstab
IF NOT exist %_MSYS2DIR%\etc\fstab. GOTO writeFstab
set "removefstab=no"

set "grep=%_MSYS2DIR%\usr\bin\grep.exe"
set fstab=%_MSYS2DIR%\etc\fstab

%grep% -q build32 %fstab% && SET "removefstab=yes"
%grep% -q trunk %fstab% || SET "removefstab=yes"

for /f "tokens=1 delims= " %%a in ('%grep% trunk %fstab%') do SET searchRes=%%a
IF NOT [%searchRes%]==[%instdir%\] SET "removefstab=yes"

%grep% -q local32 %fstab%
IF NOT errorlevel 1 (
    if [%build32%]==[no] SET "removefstab=yes"
    ) ELSE (
    if [%build32%]==[yes] SET "removefstab=yes"
    )

%grep% -q local64 %fstab%
IF NOT errorlevel 1 (
    if [%build64%]==[no] SET "removefstab=yes"
    ) ELSE (
    if [%build64%]==[yes] SET "removefstab=yes"
    )

if [%removefstab%]==[yes] (
    DEL %_MSYS2DIR%\etc\fstab.
    GOTO writeFstab
    ) ELSE (
    GOTO writeProfile32
    )

    :writeFstab
    echo -------------------------------------------------------------------------------
    ECHO.
    ECHO.- write fstab mount file
    ECHO.
    echo -------------------------------------------------------------------------------

    SET cygdrive=no

    IF EXIST %_MSYS2DIR%\etc\fstab. (
        for /f %%b in ('findstr /i binary %_MSYS2DIR%\etc\fstab.') do SET cygdrive=yes
        )
    if "%cygdrive%"=="no" echo.none / cygdrive binary,posix=0,noacl,user 0 ^0>>%_MSYS2DIR%\etc\fstab.
    (
        ECHO.
        ECHO.%instdir%\ /trunk
        ECHO.%instdir%\build\ /build
        ECHO.%_MSYS2DIR%\mingw32\ /mingw32
        ECHO.%_MSYS2DIR%\mingw64\ /mingw64
        )>>%_MSYS2DIR%\etc\fstab.
    if "%build32%"=="yes" echo.%instdir%\local32\ /local32>>%_MSYS2DIR%\etc\fstab.
    if "%build64%"=="yes" echo.%instdir%\local64\ /local64>>%_MSYS2DIR%\etc\fstab.

::------------------------------------------------------------------
:: write config profiles:
::------------------------------------------------------------------

:writeProfile32
if %build32%==yes (
    IF EXIST %instdir%\local32\etc\profile2.local GOTO writeProfile64
        echo -------------------------------------------------------------------------------
        ECHO.
        ECHO.- write profile for 32 bit compiling
        ECHO.
        echo -------------------------------------------------------------------------------
        (
            ECHO.# /local32/etc/profile2.local
            ECHO.#
            ECHO.
            ECHO.MSYSTEM=MINGW32
            ECHO.
            ECHO.# package build directory
            ECHO.LOCALBUILDDIR=/build
            ECHO.# package installation prefix
            ECHO.LOCALDESTDIR=/local32
            ECHO.export LOCALBUILDDIR LOCALDESTDIR
            ECHO.
            ECHO.bits='32bit'
            ECHO.arch="x86"
            ECHO.CARCH="i686"
            ECHO.MINGW_PREFIX="/mingw32"
            ECHO.MINGW_PACKAGE_PREFIX="mingw-w64-i686"
            )>>%instdir%\local32\etc\profile2.local
        call :writeCommonProfile 32
        )

:writeProfile64
if %build64%==yes (
    IF EXIST %instdir%\local64\etc\profile2.local GOTO loginProfile
        echo -------------------------------------------------------------------------------
        ECHO.
        ECHO.- write profile for 64 bit compiling
        ECHO.
        echo -------------------------------------------------------------------------------
        (
            ECHO.# /local64/etc/profile2.local
            ECHO.#
            ECHO.
            ECHO.MSYSTEM=MINGW64
            ECHO.
            ECHO.# package build directory
            ECHO.LOCALBUILDDIR=/build
            ECHO.# package installation prefix
            ECHO.LOCALDESTDIR=/local64
            ECHO.export LOCALBUILDDIR LOCALDESTDIR
            ECHO.
            ECHO.bits='64bit'
            ECHO.arch="x86_64"
            ECHO.CARCH="x86_64"
            ECHO.MINGW_PREFIX="/mingw64"
            ECHO.MINGW_PACKAGE_PREFIX="mingw-w64-x86_64"
            )>>%instdir%\local64\etc\profile2.local
        call :writeCommonProfile 64
        )

:loginProfile
%_MSYS2DIR%\usr\bin\grep -q -e 'profile2.local' %_MSYS2DIR%\etc\profile || (
    echo -------------------------------------------------------------------------------
    ECHO.
    ECHO. - writing default profile
    ECHO.
    echo -------------------------------------------------------------------------------
    (
        ECHO.
        ECHO.if [[ -f /local64/etc/profile2.local ]]; then
        ECHO.   source /local64/etc/profile2.local
        ECHO.elif [[ -f /local32/etc/profile2.local ]]; then
        ECHO.   source /local32/etc/profile2.local
        ECHO.fi
        )>>%_MSYS2DIR%\etc\profile.
    )

:compileLocals
cd %instdir%
IF ERRORLEVEL == 1 (
    ECHO Something goes wrong...
    pause
  )

IF EXIST %_TOOLSDIR%\compile.log DEL %_TOOLSDIR%\compile.log
start /I %_MSYS2DIR%\usr\bin\mintty.exe --log 2>&1 %_TOOLSDIR%\compile.log -i /msys2.ico /usr/bin/bash --login ^
%_TOOLSDIR%\media-suite_compile.sh --cpuCount=%cpuCount% --build32=%build32% --build64=%build64% --deleteSource=%deleteSource% ^
--mp4box=%mp4box% --vpx=%vpx2% --x264=%x2642% --x265=%x2652% --other265=%other265% --flac=%flac% --fdkaac=%fdkaac% ^
--mediainfo=%mediainfo% --sox=%sox% --ffmpeg=%ffmpeg% --ffmpegUpdate=%ffmpegUpdate% --ffmpegChoice=%ffmpegChoice% ^
--mplayer=%mplayer% --mpv=%mpv% --license=%license2%  --stripping=%stripFile% --packing=%packFile% --xpcomp=%xpcomp% ^
--rtmpdump=%rtmpdump% --logging=%logging% --bmx=%bmx% --standalone=%standalone% --angle=%angle% --aom=%aom% --daala=%daala%

endlocal
goto :EOF

:createBaseFolders
IF NOT exist %instdir%\%1\share (
    ECHO.-------------------------------------------------------------------------------
    ECHO.create %1 folders
    ECHO.-------------------------------------------------------------------------------
    mkdir %instdir%\%1
    mkdir %instdir%\%1\bin
    mkdir %instdir%\%1\bin-audio
    mkdir %instdir%\%1\bin-global
    mkdir %instdir%\%1\bin-video
    mkdir %instdir%\%1\etc
    mkdir %instdir%\%1\include
    mkdir %instdir%\%1\lib
    mkdir %instdir%\%1\lib\pkgconfig
    mkdir %instdir%\%1\share
    )
goto :EOF

:writeCommonProfile
(
    ECHO.
    ECHO.# common in both profiles
    ECHO.alias dir='ls -la --color=auto'
    ECHO.alias ls='ls --color=auto'
    ECHO.export CC=gcc
    ECHO.
    ECHO.MINGW_CHOST="$CARCH-w64-mingw32"
    ECHO.CPATH="`cygpath -m $LOCALDESTDIR/include`;`cygpath -m $MINGW_PREFIX/include`"
    ECHO.LIBRARY_PATH="`cygpath -m $LOCALDESTDIR/lib`;`cygpath -m $MINGW_PREFIX/lib`"
    ECHO.export CPATH LIBRARY_PATH
    ECHO.
    ECHO.MSYS2_PATH="/usr/local/bin:/usr/bin"
    ECHO.MANPATH="${LOCALDESTDIR}/share/man:${MINGW_PREFIX}/share/man:/usr/share/man"
    ECHO.INFOPATH="${LOCALDESTDIR}/share/info:${MINGW_PREFIX}/share/info:/usr/share/info"
    ECHO.
    ECHO.DXSDK_DIR="${MINGW_PREFIX}/${MINGW_CHOST}"
    ECHO.ACLOCAL_PATH="${LOCALDESTDIR}/share/aclocal:${MINGW_PREFIX}/share/aclocal:/usr/share/aclocal"
    ECHO.PKG_CONFIG="${MINGW_PREFIX}/bin/pkg-config --static"
    ECHO.PKG_CONFIG_PATH="${LOCALDESTDIR}/lib/pkgconfig:${MINGW_PREFIX}/lib/pkgconfig"
    ECHO.CPPFLAGS="-D_FORTIFY_SOURCE=2 -D__USE_MINGW_ANSI_STDIO=1"
    ECHO.CFLAGS="-mthreads -mtune=generic -O2 -pipe"
    ECHO.CXXFLAGS="${CFLAGS}"
    ECHO.LDFLAGS="-pipe -static-libgcc -static-libstdc++"
    ECHO.export DXSDK_DIR ACLOCAL_PATH PKG_CONFIG PKG_CONFIG_PATH CPPFLAGS CFLAGS CXXFLAGS LDFLAGS MSYSTEM
    ECHO.
    ECHO.LANG=en_US.UTF-8
    ECHO.PATH="${LOCALDESTDIR}/bin:${MINGW_PREFIX}/bin:${MSYS2_PATH}:${INFOPATH}:${PATH}"
    ECHO.PATH="${LOCALDESTDIR}/bin-audio:${LOCALDESTDIR}/bin-global:${LOCALDESTDIR}/bin-video:${PATH}"
    ECHO.PS1='\[\033[32m\]\u@\h \[\e[33m\]\w\[\e[0m\]\n\$ '
    ECHO.HOME="/home/${USERNAME}"
    ECHO.GIT_GUI_LIB_DIR=`cygpath -w /usr/share/git-gui/lib`
    ECHO.export LANG PATH PS1 HOME GIT_GUI_LIB_DIR
    ECHO.stty susp undef
    ECHO.cd /trunk
    )>>%instdir%\local%1\etc\profile2.local
goto :EOF
