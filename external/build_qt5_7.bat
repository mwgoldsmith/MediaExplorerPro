ECHO OFF

SET QTVER=Qt5.7
SET QTBRANCH=5.7.1

SET EXTERNALDIR=%CD%
SET BUILDDIR=%EXTERNALDIR%\..\build\
SET BUILDTOOLSDIR=%BUILDDIR%tools\

IF EXIST "%VSSDK140Install%" (
  SET "MSVCDIR=%VSSDK140Install%..\VC\"
  GOTO has_msvc2015
)

REM TODO: Check for prior MSVC versions

:: ------------------------------------------------------------------ ::
:has_msvc2015
REM TODO: Determine if 32-bit or 64-bit

:: ------------------------------------------------------------------ ::
:msvc2015x86
IF NOT EXIST "%MSVCDIR%\bin\vcvars32.bat" (
  ECHO. Missing dependency: msvc
  EXIT
)

CALL "%MSVCDIR%\bin\vcvars32.bat"
ECHO. Using MSVC 2015 (Win32)
SET QMAKESPEC=win32-msvc2015
SET SUFFIX=MSVC2015_x86
SET PREFIX=%BUILDDIR%msvc\x86\
GOTO check_tools


:: ------------------------------------------------------------------ ::
:msvc2015x64
IF NOT EXIST "%MSVCDIR%\bin\amd64\vcvars64.bat" (
  ECHO. Missing dependency: msvc
  EXIT
)

CALL "%MSVCDIR%\bin\amd64\vcvars64.bat"
ECHO. Using MSVC 2015 (x64)
SET QMAKESPEC=win32-msvc2015
SET SUFFIX=MSVC2015_x64
SET PREFIX=%BUILDDIR%msvc\x64\
GOTO check_tools


:: ------------------------------------------------------------------ ::
:check_tools
CALL perl --version >nul 2>&1
IF NOT ERRORLEVEL 0 (
  ECHO. Missing dependency: perl
  EXIT 1
)

CALL git --version >nul 2>&1
IF NOT ERRORLEVEL 0 (
  ECHO. Missing dependency: git
  EXIT 1
)

CALL python --version >nul 2>&1
IF NOT ERRORLEVEL 0 (
  ECHO. Missing dependency: python
  EXIT 1
)

CALL cscript /? >nul 2>&1
IF NOT ERRORLEVEL 0 (
  ECHO. Cannot find dependency: cscript
  EXIT 1
)

IF NOT EXIST "%BUILDTOOLSDIR%\wget.exe" (
  CALL :install_wget
  IF NOT ERRORLEVEL 0 (
    EXIT 1
  )
)

IF NOT EXIST "%BUILDTOOLSDIR%\jom.exe" (
  CALL :install_jom
  IF NOT ERRORLEVEL 0 (
    EXIT 1
  )
)

:: ------------------------------------------------------------------ ::
:havetools
CD %EXTERNALDIR%

:: Create destination directory if it does not exist
IF NOT EXIST "%PREFIX%" (
  MKDIR "%PREFIX%"
)

IF EXIST "%QTVER%_src" (
  ECHO. Qt source directory already exists"
  GOTO qt_checkout
)

ECHO. Cloning %QTVER%
CALL git clone https://code.qt.io/qt/qt5.git %QTVER%_src
IF NOT ERRORLEVEL 0 (
  ECHO. Failed to clone Qt repo. Terminating.
  PAUSE
  EXIT 1
)

:qt_checkout
CD "%QTVER%_src"
ECHO.Checking out bramch %QTBRANCH%
CALL git checkout %QTBRANCH%

ECHO.Initializing submodules
CALL perl init-repository
IF NOT ERRORLEVEL 0 (
  ECHO. Failed to initialize submodules
  PAUSE
  EXIT 1
)

SET PATH=%CD%\bin;%PATH%
SET QTDIR=%CD%\qtbase
SET CL=/MP
CD ..

MKDIR %QTVER%_%SUFFIX%
CD %QTVER%_%SUFFIX%

ECHO. Configuring %QTVER%
CALL %EXTERNALDIR%\%QTVER%_src\configure -prefix %PREFIX% -hostprefix %BUILDTOOLSDIR% -developer-build -confirm-license -debug-and-release -opensource -platform %QMAKESPEC% -opengl desktop -static -nomake tests -nomake examples -make libs -mp -qt-zlib -qt-pcre -qt-libpng -qt-libjpeg

ECHO. Building %QTVER%
%BUILDTOOLSDIR%\jom -j4

ECHO. Installing
%BUILDTOOLSDIR%\jom install


:qt_build_done
EXIT 0

:: ------------------------------------------------------------------ ::
:install_wget
:: ------------------------------------------------------------------ ::
SETLOCAL
PUSHD %CD%

CD %BUILDDIR%
IF NOT EXIST %BUILDTOOLSDIR% (
  MKDIR %BUILDTOOLSDIR%
)

CD %BUILDTOOLSDIR%
IF EXIST "wget.exe" (
  IF EXIST "7za.exe" (
    GOTO install_wget_done
  )
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
IF NOT ERRORLEVEL 0 (
  SET "%~1=1"
) ELSE (
  wget-pack.exe x
)

IF NOT EXIST "wget.exe" (
  ECHO -------------------------------------------------------------------------------
  ECHO Script to download necessary components failed.
  ECHO.
  ECHO Download and extract this manually to inside "build\tools":
  ECHO https://i.fsbn.eu/pub/wget-pack.exe
  ECHO -------------------------------------------------------------------------------
  PAUSE
  SET "%~1=1"
)

:install_wget_done
IF EXIST "wget.js" (
  DEL "wget.js" >nul 2>&1
)

IF EXIST "wget-pack.exe" (
  DEL "wget-pack.exe" >nul 2>&1
)

POPD
(ENDLOCAL & REM -- RETURN VALUES
  IF "%~1" NEQ "" SET "%~1=0"
)
GOTO:EOF


:: ------------------------------------------------------------------ ::
:install_jom
:: ------------------------------------------------------------------ ::
SETLOCAL
PUSHD %CD%

CD %BUILDDIR%
IF NOT EXIST %BUILDTOOLSDIR% (
  MKDIR %BUILDTOOLSDIR%
)

CD "%BUILDTOOLSDIR%
IF EXIST "jom.exe" (
  GOTO install_jom_done
)

wget --tries=5 --retry-connrefused --waitretry=5 --continue -q -O jom.zip "http://download.qt.io/official_releases/jom/jom.zip"
IF NOT ERRORLEVEL 0 (
  SET "%~1=1"
) ELSE (
  7za.exe x jom.zip jom.exe
)

IF NOT EXIST "jom.exe" (
  ECHO -------------------------------------------------------------------------------
  ECHO Script to download necessary components failed.
  ECHO.
  ECHO Download and extract this manually to inside "build\tools":
  ECHO http://download.qt.io/official_releases/jom/jom.zip
  ECHO -------------------------------------------------------------------------------
  PAUSE
  SET "%~1=1"
)

:install_jom_done
IF EXIST "jom.zip" (
  DEL "jom.zip" >nul 2>&1
)

POPD
(ENDLOCAL & REM -- RETURN VALUES
  IF "%~1" NEQ "" SET "%~1=0"
)
GOTO:EOF