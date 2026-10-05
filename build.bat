@echo off
setlocal

:: If running in CI or GitHub Actions, skip prompt and default to Enterprise
if defined GITHUB_ACTIONS goto Enterprise
if defined CI goto Enterprise

:: Check if VS edition is passed as parameter
if /i "%~1"=="Enterprise" goto Enterprise
if /i "%~1"=="Community" goto Community
if /i "%~1"=="Professional" goto Professional

@echo Choose Visual Studio 2022 Edition (after 5s will default to (2) Enterprise):
@echo (1) Community
@echo (2) Enterprise
@echo (3) Professional
@choice /c:123 /t:5 /d:2 >nul 2>&1
if errorlevel 3 goto Professional
if errorlevel 2 goto Enterprise
if errorlevel 1 goto Community
goto Enterprise

:Enterprise
set Edition=Enterprise
goto Continue

:Community
set Edition=Community
goto Continue

:Professional
set Edition=Professional
goto Continue

:Continue
set "vcvars=C:\Program Files\Microsoft Visual Studio\2022\%Edition%\VC\Auxiliary\Build\vcvarsall.bat"
if exist "%vcvars%" (
    call "%vcvars%" x86_amd64
) else (
    set "vcvarsFallback=C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe"
    if exist "%vcvarsFallback%" (
        for /f "usebackq tokens=*" %%i in (`"%vcvarsFallback%" -latest -products * -property installationPath 2^>nul`) do (
            if exist "%%i\VC\Auxiliary\Build\vcvarsall.bat" (
                call "%%i\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64
            )
        )
    )
)

set solutionsFile=".\PluginDarkNpp.sln"

echo [DarkNpp] Building Release x64...
msbuild -m /t:build /p:Configuration=Release;Platform=x64 %solutionsFile%
if %errorlevel% neq 0 (
    echo [ERROR] Build failed for x64 Release with exit code %errorlevel%
    exit /b %errorlevel%
)

echo [DarkNpp] Building Release Win32...
msbuild -m /t:build /p:Configuration=Release;Platform=Win32 %solutionsFile%
if %errorlevel% neq 0 (
    echo [ERROR] Build failed for Win32 Release with exit code %errorlevel%
    exit /b %errorlevel%
)

echo [SUCCESS] Both x64 and Win32 Release builds succeeded.
exit /b 0