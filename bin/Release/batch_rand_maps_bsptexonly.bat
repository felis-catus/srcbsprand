@echo off

set gamedir=E:\Program Files (x86)\Steam\steamapps\common\Half-Life 2\hl2
set mapsdir=E:\Program Files (x86)\Steam\steamapps\common\Half-Life 2\hl2\maps
set outdir=E:\Program Files (x86)\Steam\steamapps\common\Half-Life 2\hl2\custom\rand\maps

if not exist "%gamedir%"  (
	echo Game directory does not exist, open this batch file and check the dir.
	goto failure
)

if not exist "%mapsdir%"  (
	echo Maps directory does not exist, open this batch file and check the dir.
	goto failure
)

if not exist "%outdir%"  (
	echo Maps output directory does not exist, creating...
	mkdir "%outdir%"
)

echo Starting randomization...
for %%f in ("%mapsdir%\*") do srcbsprand "%%f" -game "%gamedir%" -out "%outdir%" -bsptexonly
goto success

:success
echo Randomization finished.
goto exit

:failure
echo Randomization failed.
goto exit

:exit
pause
