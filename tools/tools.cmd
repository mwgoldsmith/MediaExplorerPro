@ECHO OFF
SETLOCAL

SET _MSYSPACKAGES=asciidoc autoconf automake-wrapper autogen bison diffstat dos2unix help2man ^
intltool libtool patch python xmlto make zip unzip git subversion wget p7zip mercurial man-db ^
gperf winpty texinfo upx

SET _TOOLSBASEDIR=%~dp0
IF NOT EXIST %_TOOLSBASEDIR% (
  ECHO. You have probably run the script in a path with spaces. This is not supported.
  ECHO. Please correct the path names before continuing.
  PAUSE
  EXIT /B 1
)

SET BITS=64
IF %PROCESSOR_ARCHITECTURE%==x86 (
  IF NOT DEFINED PROCESSOR_ARCHITEW6432 (
    SET BITS=32
  )
)

IF %BITS%==64 (
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

:set_hg_settings
IF EXIST "%_MSYS2DIR%home\%USERNAME%\.gitconfig" (
  GOTO has_git_settings
)

CALL :set_git_settings
IF ERRORLEVEL 1 (
  EXIT /B 1
)

:has_git_settings
CALL :install_base
IF ERRORLEVEL 1 (
  EXIT /B 1
)

PAUSE
EXIT /B 0


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
  )>wget.js
  
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

IF EXIST firstrun.log (
  DEL firstrun.log >NUL
)
!_MINTTY! --log 2>&1 firstrun.log /usr/bin/bash --login -c exit

IF EXIST firstUpdate.log (
  DEL firstUpdate.log >NUL
)
(
  ECHO.echo -ne "\033]0;first msys2 update\007"
  ECHO.pacman --noconfirm -Sy --force --asdeps pacman-mirrors
  ECHO.sed -i "s;^^IgnorePkg.*;#&;" /etc/pacman.conf
  ECHO.sleep ^4
  ECHO.exit
)>firstUpdate.sh
!_MINTTY! --log 2>&1 firstUpdate.log /usr/bin/bash --login firstUpdate.sh
DEL firstUpdate.sh >NUL

!_MSYS2DIR!usr\bin\sh.exe -l -c "pacman -S --needed --noconfirm --asdeps bash pacman msys2-runtime"

IF EXIST secondUpdate.log (
  DEL secondUpdate.log >NUL
)
(
  ECHO.echo -ne "\033]0;second msys2 update\007"
  ECHO.pacman --noconfirm -Syu --force --asdeps
  ECHO.exit
)>secondUpdate.sh
!_MINTTY! --log 2>&1 secondUpdate.log /usr/bin/bash --login secondUpdate.sh
DEL secondUpdate.sh >NUL

SET _RESULT=0

POPD
IF !_RESULT! EQU 1 GOTO exit_error
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
)>>"%_MSYS2DIR%home\%USERNAME%\.minttyrc"

POPD
GOTO exit_success


::---------------------------------------------------------------------------::
:set_hg_settings
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
(
  echo.[ui]
  echo.username = %USERNAME%
  echo.verbose = True
  echo.editor = vim
  echo.
  echo.[web]
  echo.cacerts=/usr/ssl/cert.pem
  echo.
  echo.[extensions]
  echo.color =
  echo.
  echo.[color]
  echo.status.modified = magenta bold
  echo.status.added = green bold
  echo.status.removed = red bold
  echo.status.deleted = cyan bold
  echo.status.unknown = blue bold
  echo.status.ignored = black bold
)>>"%_MSYS2DIR%home\%USERNAME%\.hgrc"
  
POPD
GOTO exit_success

::---------------------------------------------------------------------------::
:set_git_settings
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
(
  echo.[user]
  echo.name = %USERNAME%
  echo.email = %USERNAME%@%COMPUTERNAME%
  echo.
  echo.[color]
  echo.ui = true
  echo.
  echo.[core]
  echo.editor = vim
  echo.autocrlf =
  echo.
  echo.[merge]
  echo.tool = vimdiff
  echo.
  echo.[push]
  echo.default = simple
)>>"%_MSYS2DIR%home\%USERNAME%\.gitconfig"

POPD
GOTO exit_success


::---------------------------------------------------------------------------::
:install_base
::---------------------------------------------------------------------------::
SETLOCAL EnableDelayedExpansion
PUSHD "%CD%"

CD !_TOOLSDIR!
IF EXIST "%_MSYS2DIR%etc\pac-base.pk"  (
  DEL "%_MSYS2DIR%etc\pac-base.pk"
)
FOR %%i IN (%_MSYSPACKAGES%) DO  (
  ECHO.%%i>>%_MSYS2DIR%etc\pac-base.pk
)

IF EXIST %_MSYS2DIR%usr\bin\make.exe (
  GOTO base_installed
)

echo.-------------------------------------------------------------------------------
echo.install msys2 base system
echo.-------------------------------------------------------------------------------
IF EXIST %_TOOLSDIR%\install_base_failed (
  DEL %_TOOLSDIR%\install_base_failed
)
(
  echo.echo -ne "\033]0;install base system\007"
  echo.msysbasesystem="$(cat /etc/pac-base.pk | tr '\n\r' '  ')"
  echo.[[ "$(uname)" = *6.1* ]] ^&^& nargs="-n 4"
  echo.echo $msysbasesystem ^| xargs $nargs pacman -Sw --noconfirm --needed
  echo.echo $msysbasesystem ^| xargs $nargs pacman -S --noconfirm --needed
  echo.echo $msysbasesystem ^| xargs $nargs pacman -D --asexplicit
  echo.sleep ^3
  echo.exit
) >%_TOOLSDIR%\pacman.sh
IF EXIST %_TOOLSDIR%\pacman.log (
  DEL %_TOOLSDIR%\pacman.log
)
%_MINTTY% --log 2>&1 %_TOOLSDIR%\pacman.log /usr/bin/bash --login %_TOOLSDIR%\pacman.sh
DEL %_TOOLSDIR%\pacman.sh

for %%i in (%_MSYS2DIR%usr\ssl\cert.pem) do (
  if %%~zi==0 (
    (
      echo.update-ca-trust
      echo.sleep ^3
      echo.exit
    )>%_TOOLSDIR%\cert.sh
    if exist %_TOOLSDIR%\cert.log (
      del %_TOOLSDIR%\cert.log
    )
    %_MINTTY% --log 2>&1 %_TOOLSDIR%\cert.log /usr/bin/bash --login %_TOOLSDIR%\cert.sh
    del %_TOOLSDIR%\cert.sh
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




:base_installed
if exist %_MSYS2DIR%usr\bin\hg.bat GOTO getmingw32
(
    echo.@echo off
    echo.
    echo.setlocal
    echo.set HG=^%%~f0
    echo.
    echo.set PYTHONHOME=
    echo.set in=^%%*
    echo.set out=^%%in: ^{= ^"^{^%%
    echo.set out=^%%out:^} =^}^" ^%%
    echo.
    echo.^%%~dp0python2 ^%%~dp0hg ^%%out^%%
    )>>%_MSYS2DIR%usr\bin\hg.bat


:getmingw32
if exist "%instdir%\%msys2%\etc\pac-mingw.pk" del "%instdir%\%msys2%\etc\pac-mingw.pk"
for %%i in (%mingwpackages%) do echo.%%i>>%instdir%\%msys2%\etc\pac-mingw.pk

if %build32%==yes (
    if exist %instdir%\%msys2%\mingw32\bin\gcc.exe GOTO getmingw64
    echo.-------------------------------------------------------------------------------
    echo.install 32 bit compiler
    echo.-------------------------------------------------------------------------------
    (
        echo.echo -ne "\033]0;install 32 bit compiler\007"
        echo.mingw32compiler="$(cat /etc/pac-mingw.pk | sed 's;.*;mingw-w64-i686-&;g' | tr '\n\r' '  ')"
        echo.[[ "$(uname)" = *6.1* ]] ^&^& nargs="-n 4"
        echo.echo $mingw32compiler ^| xargs $nargs pacman -Sw --noconfirm --needed --force
        echo.echo $mingw32compiler ^| xargs $nargs pacman -S --noconfirm --needed --force
        echo.sleep ^3
        echo.exit
        )>%build%\mingw32.sh
    if exist %build%\mingw32.log del %build%\mingw32.log
    %mintty% --log 2>&1 %build%\mingw32.log /usr/bin/bash --login %build%\mingw32.sh
    del %build%\mingw32.sh
    
    if not exist %instdir%\%msys2%\mingw32\bin\gcc.exe (
        echo -------------------------------------------------------------------------------
        echo.
        echo.MinGW32 GCC compiler isn't installed; maybe the download didn't work
        echo.Do you want to try it again?
        echo.
        echo -------------------------------------------------------------------------------
        set /P try32="try again [y/n]: "

        if %packF%==y (
            GOTO getmingw32
            ) else exit
        )
    )
    
:getmingw64
if %build64%==yes (
    if exist %instdir%\%msys2%\mingw64\bin\gcc.exe GOTO updatebase
    echo.-------------------------------------------------------------------------------
    echo.install 64 bit compiler
    echo.-------------------------------------------------------------------------------
        (
        echo.echo -ne "\033]0;install 64 bit compiler\007"
        echo.mingw64compiler="$(cat /etc/pac-mingw.pk | sed 's;.*;mingw-w64-x86_64-&;g' | tr '\n\r' '  ')"
        echo.[[ "$(uname)" = *6.1* ]] ^&^& nargs="-n 4"
        echo.echo $mingw64compiler ^| xargs $nargs pacman -Sw --noconfirm --needed --force
        echo.echo $mingw64compiler ^| xargs $nargs pacman -S --noconfirm --needed --force
        echo.sleep ^3
        echo.exit
            )>%build%\mingw64.sh
    if exist %build%\mingw64.log del %build%\mingw64.log
    %mintty% --log 2>&1 %build%\mingw64.log /usr/bin/bash --login %build%\mingw64.sh
    del %build%\mingw64.sh

    if not exist %instdir%\%msys2%\mingw64\bin\gcc.exe (
        echo -------------------------------------------------------------------------------
        echo.
        echo.MinGW64 GCC compiler isn't installed; maybe the download didn't work
        echo.Do you want to try it again?
        echo.
        echo -------------------------------------------------------------------------------
        set /P try64="try again [y/n]: "

        if %packF%==y (
            GOTO getmingw64
            ) else exit
        )
    )

:updatebase
echo.-------------------------------------------------------------------------------
echo.update autobuild suite
echo.-------------------------------------------------------------------------------

cd %build%
set scripts=compile helper update
for %%s in (%scripts%) do (
    if not exist "%build%\media-suite_%%s.sh" (
        %instdir%\%msys2%\usr\bin\wget.exe -t 20 --retry-connrefused --waitretry=2 -c ^
        https://github.com/jb-alvarado/media-autobuild_suite/raw/master/build/media-suite_%%s.sh
        )
    )
if %updateSuite%==y (
    if not exist %instdir%\update_suite.sh (
        echo -------------------------------------------------------------------------------
        echo. Creating suite update file...
        echo.
        echo. Run this file by dragging it to mintty before the next time you run
        echo. the suite and before reporting an issue.
        echo.
        echo. It needs to be run separately and with the suite not running!
        echo -------------------------------------------------------------------------------
        )
    (
        echo.#!/bin/bash
        echo.
        echo.# Run this file by dragging it to mintty shortcut.
        echo.# Be sure the suite is not running before using it!
        echo.
        echo.update=yes
        %instdir%\%msys2%\usr\bin\sed -n '/start suite update/,/end suite update/p' ^
            %build%/media-suite_update.sh
        )>%instdir%\update_suite.sh
    )

if exist %build%\update.log del %build%\update.log
%mintty% --log 2>&1 %build%\update.log /usr/bin/bash --login %build%\media-suite_update.sh ^
--build32=%build32% --build64=%build64%

if exist "%build%\update_core" (
    echo.-------------------------------------------------------------------------------
    echo.critical updates
    echo.-------------------------------------------------------------------------------
    %instdir%\%msys2%\usr\bin\sh.exe -l -c "pacman -S --needed --noconfirm --asdeps bash pacman msys2-runtime"
    del "%build%\update_core"
    )

:rebase2
if %msys2%==msys32 (
echo.-------------------------------------------------------------------------------
echo.second rebase %msys2% system
echo.-------------------------------------------------------------------------------
call %instdir%\%msys2%\autorebase.bat
)

:createFolders
if %build32%==yes call :createBaseFolders local32
if %build64%==yes call :createBaseFolders local64

:checkFstab
if not exist %instdir%\%msys2%\etc\fstab. GOTO writeFstab
set "removefstab=no"

set "grep=%instdir%\%msys2%\usr\bin\grep.exe"
set fstab=%instdir%\%msys2%\etc\fstab

%grep% -q build32 %fstab% && set "removefstab=yes"
%grep% -q trunk %fstab% || set "removefstab=yes"

for /f "tokens=1 delims= " %%a in ('%grep% trunk %fstab%') do set searchRes=%%a
if not [%searchRes%]==[%instdir%\] set "removefstab=yes"

%grep% -q local32 %fstab%
if not errorlevel 1 (
    if [%build32%]==[no] set "removefstab=yes"
    ) else (
    if [%build32%]==[yes] set "removefstab=yes"
    )

%grep% -q local64 %fstab%
if not errorlevel 1 (
    if [%build64%]==[no] set "removefstab=yes"
    ) else (
    if [%build64%]==[yes] set "removefstab=yes"
    )

if [%removefstab%]==[yes] (
    del %instdir%\%msys2%\etc\fstab.
    GOTO writeFstab
    ) else (
    GOTO writeProfile32
    )

    :writeFstab
    echo -------------------------------------------------------------------------------
    echo.
    echo.- write fstab mount file
    echo.
    echo -------------------------------------------------------------------------------

    set cygdrive=no

    if exist %instdir%\%msys2%\etc\fstab. (
        for /f %%b in ('findstr /i binary %instdir%\%msys2%\etc\fstab.') do set cygdrive=yes
        )
    if "%cygdrive%"=="no" echo.none / cygdrive binary,posix=0,noacl,user 0 ^0>>%instdir%\%msys2%\etc\fstab.
    (
        echo.
        echo.%instdir%\ /trunk
        echo.%instdir%\build\ /build
        echo.%instdir%\%msys2%\mingw32\ /mingw32
        echo.%instdir%\%msys2%\mingw64\ /mingw64
        )>>%instdir%\%msys2%\etc\fstab.
    if "%build32%"=="yes" echo.%instdir%\local32\ /local32>>%instdir%\%msys2%\etc\fstab.
    if "%build64%"=="yes" echo.%instdir%\local64\ /local64>>%instdir%\%msys2%\etc\fstab.

::------------------------------------------------------------------
:: write config profiles:
::------------------------------------------------------------------

:writeProfile32
if %build32%==yes (
    if exist %instdir%\local32\etc\profile2.local GOTO writeProfile64
        echo -------------------------------------------------------------------------------
        echo.
        echo.- write profile for 32 bit compiling
        echo.
        echo -------------------------------------------------------------------------------
        (
            echo.# /local32/etc/profile2.local
            echo.#
            echo.
            echo.MSYSTEM=MINGW32
            echo.
            echo.# package build directory
            echo.LOCALBUILDDIR=/build
            echo.# package installation prefix
            echo.LOCALDESTDIR=/local32
            echo.export LOCALBUILDDIR LOCALDESTDIR
            echo.
            echo.bits='32bit'
            echo.arch="x86"
            echo.CARCH="i686"
            echo.MINGW_PREFIX="/mingw32"
            echo.MINGW_PACKAGE_PREFIX="mingw-w64-i686"
            )>>%instdir%\local32\etc\profile2.local
        call :writeCommonProfile 32
        )

:writeProfile64
if %build64%==yes (
    if exist %instdir%\local64\etc\profile2.local GOTO loginProfile
        echo -------------------------------------------------------------------------------
        echo.
        echo.- write profile for 64 bit compiling
        echo.
        echo -------------------------------------------------------------------------------
        (
            echo.# /local64/etc/profile2.local
            echo.#
            echo.
            echo.MSYSTEM=MINGW64
            echo.
            echo.# package build directory
            echo.LOCALBUILDDIR=/build
            echo.# package installation prefix
            echo.LOCALDESTDIR=/local64
            echo.export LOCALBUILDDIR LOCALDESTDIR
            echo.
            echo.bits='64bit'
            echo.arch="x86_64"
            echo.CARCH="x86_64"
            echo.MINGW_PREFIX="/mingw64"
            echo.MINGW_PACKAGE_PREFIX="mingw-w64-x86_64"
            )>>%instdir%\local64\etc\profile2.local
        call :writeCommonProfile 64
        )

:loginProfile
%instdir%\%msys2%\usr\bin\grep -q -e 'profile2.local' %instdir%\%msys2%\etc\profile || (
    echo -------------------------------------------------------------------------------
    echo.
    echo. - writing default profile
    echo.
    echo -------------------------------------------------------------------------------
    (
        echo.
        echo.if [[ -f /local64/etc/profile2.local ]]; then
        echo.   source /local64/etc/profile2.local
        echo.elif [[ -f /local32/etc/profile2.local ]]; then
        echo.   source /local32/etc/profile2.local
        echo.fi
        )>>%instdir%\%msys2%\etc\profile.
    )

:compileLocals
cd %instdir%
IF ERRORLEVEL == 1 (
    ECHO Something goes wrong...
    pause
  )

if exist %build%\compile.log del %build%\compile.log
start /I %instdir%\%msys2%\usr\bin\mintty.exe --log 2>&1 %build%\compile.log -i /msys2.ico /usr/bin/bash --login ^
%build%\media-suite_compile.sh --cpuCount=%cpuCount% --build32=%build32% --build64=%build64% --deleteSource=%deleteSource% ^
--mp4box=%mp4box% --vpx=%vpx2% --x264=%x2642% --x265=%x2652% --other265=%other265% --flac=%flac% --fdkaac=%fdkaac% ^
--mediainfo=%mediainfo% --sox=%sox% --ffmpeg=%ffmpeg% --ffmpegUpdate=%ffmpegUpdate% --ffmpegChoice=%ffmpegChoice% ^
--mplayer=%mplayer% --mpv=%mpv% --license=%license2%  --stripping=%stripFile% --packing=%packFile% --xpcomp=%xpcomp% ^
--rtmpdump=%rtmpdump% --logging=%logging% --bmx=%bmx% --standalone=%standalone% --angle=%angle% --aom=%aom% --daala=%daala%

endlocal
goto :EOF

:createBaseFolders
if not exist %instdir%\%1\share (
    echo.-------------------------------------------------------------------------------
    echo.create %1 folders
    echo.-------------------------------------------------------------------------------
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
    echo.
    echo.# common in both profiles
    echo.alias dir='ls -la --color=auto'
    echo.alias ls='ls --color=auto'
    echo.export CC=gcc
    echo.
    echo.MINGW_CHOST="$CARCH-w64-mingw32"
    echo.CPATH="`cygpath -m $LOCALDESTDIR/include`;`cygpath -m $MINGW_PREFIX/include`"
    echo.LIBRARY_PATH="`cygpath -m $LOCALDESTDIR/lib`;`cygpath -m $MINGW_PREFIX/lib`"
    echo.export CPATH LIBRARY_PATH
    echo.
    echo.MSYS2_PATH="/usr/local/bin:/usr/bin"
    echo.MANPATH="${LOCALDESTDIR}/share/man:${MINGW_PREFIX}/share/man:/usr/share/man"
    echo.INFOPATH="${LOCALDESTDIR}/share/info:${MINGW_PREFIX}/share/info:/usr/share/info"
    echo.
    echo.DXSDK_DIR="${MINGW_PREFIX}/${MINGW_CHOST}"
    echo.ACLOCAL_PATH="${LOCALDESTDIR}/share/aclocal:${MINGW_PREFIX}/share/aclocal:/usr/share/aclocal"
    echo.PKG_CONFIG="${MINGW_PREFIX}/bin/pkg-config --static"
    echo.PKG_CONFIG_PATH="${LOCALDESTDIR}/lib/pkgconfig:${MINGW_PREFIX}/lib/pkgconfig"
    echo.CPPFLAGS="-D_FORTIFY_SOURCE=2 -D__USE_MINGW_ANSI_STDIO=1"
    echo.CFLAGS="-mthreads -mtune=generic -O2 -pipe"
    echo.CXXFLAGS="${CFLAGS}"
    echo.LDFLAGS="-pipe -static-libgcc -static-libstdc++"
    echo.export DXSDK_DIR ACLOCAL_PATH PKG_CONFIG PKG_CONFIG_PATH CPPFLAGS CFLAGS CXXFLAGS LDFLAGS MSYSTEM
    echo.
    echo.LANG=en_US.UTF-8
    echo.PATH="${LOCALDESTDIR}/bin:${MINGW_PREFIX}/bin:${MSYS2_PATH}:${INFOPATH}:${PATH}"
    echo.PATH="${LOCALDESTDIR}/bin-audio:${LOCALDESTDIR}/bin-global:${LOCALDESTDIR}/bin-video:${PATH}"
    echo.PS1='\[\033[32m\]\u@\h \[\e[33m\]\w\[\e[0m\]\n\$ '
    echo.HOME="/home/${USERNAME}"
    echo.GIT_GUI_LIB_DIR=`cygpath -w /usr/share/git-gui/lib`
    echo.export LANG PATH PS1 HOME GIT_GUI_LIB_DIR
    echo.stty susp undef
    echo.cd /trunk
    )>>%instdir%\local%1\etc\profile2.local
goto :EOF
