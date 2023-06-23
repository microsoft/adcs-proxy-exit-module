@echo off
set __scriptDir=%~dp0%..\PsModules
powershell -ExecutionPolicy bypass -Command "$psm1p = Join-Path %__scriptDir% DeveloperHelpers.psm1; Import-Module $psm1p ; Restore-Packages"