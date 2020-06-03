

set "pathDisc=C:\Program Files\IBK\Delphin 6.1\CmdDiscretise.exe"
set "pathSolver=C:\Program Files\IBK\Delphin 6.1\DelphinSolver.exe"

set "pathD6p=Inc0Ori180D10Lambda02Potsdam.d6p"

"%pathDisc%" "%pathD6p%" -o="aa.d6p"
rem "%pathSolver%" --help

"%pathSolver%" "aa.d6p"