@SET PATH=%1;%PATH%

@setlocal ENABLEDELAYEDEXPANSION
  @set "_args=%*" &set "_first=%1"
  @set "_args=!_args:%_first%=!"
  @set "_args=%_args:~1%"

  windeployqt.exe --pdb --force %_args%
  @IF %errorlevel% == 1 windeployqt.exe --force %_args%
@endlocal
