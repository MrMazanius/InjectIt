# InjectIt Introduction

InjectIt is a straightforward and user-friendly C++ injector designed for developers working on cheats or similar projects.

This tool operates quietly in the background, continuously monitoring for a specified process. Once the target process is detected, InjectIt waits for a user-defined duration before performing the injection.

After the injection is successfully executed, the tool monitors the process for re-opening and repeats the injection process as needed, ensuring smooth and automated functionality.

### Configging Tutorial

The config file has to be named `config.ini`. 

Config.ini Example:
```bash
[Settings]
DllPath=C:\\Users\\obiot\\Documents\\GitHub\\CutieHook\\build\\x64\\Debug\\Counter-Strike2 Internal.dll
Process=cs2.exe
WaitTimeFromProcessFoundToInject_Seconds=10
```
