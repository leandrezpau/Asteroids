@REM Compilación y Enlace con biblioteca gráfica.
@cls
@echo ---------------------------------------------------
@echo                ESAT Curso 2025-2026 
@echo                 Pau Leandrez Gomez
@echo ---------------------------------------------------
@echo  Proceso por lotes iniciado.
@echo ---------------------------------------------------
@echo off

cl /nologo /Zi /GR- /EHs /MD ^
../src/asteroids.cc ^
-I ../libs/ESAT_rev248/include ^
../libs/ESAT_rev248/bin/ESAT.lib ^
opengl32.lib user32.lib gdi32.lib shell32.lib Ws2_32.lib ^
/Fe:../bin/asteroids.exe

@echo ---------------------------------------------------
@echo  Proceso por lotes finalizado.
@echo ---------------------------------------------------