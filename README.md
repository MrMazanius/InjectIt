# InjectIt Introduction

InjectIt is a straightforward and user-friendly C++ injector designed for developers working on cheats or similar projects.

This tool operates quietly in the background, continuously monitoring for a specified process. Once the target process is detected, InjectIt waits for a user-defined duration before performing the injection.

After the injection is successfully executed, the tool monitors the process for re-opening and repeats the injection process as needed, ensuring smooth and automated functionality.

### Configuration Tutorial

The configuration file must be named `config.ini` and placed in the same directory as `InjectIt.exe`.

If you're compiling the project yourself, note that you cannot run the executable directly from Visual Studio (using the play button). Instead, you need to run it from File Explorer.

#### Example of `config.ini`:
```ini
[Settings]
DllPath=C:\path\to\your\dll\to_inject.dll
Process=cs2.exe
WaitTimeFromProcessFoundToInject_Seconds=10
```

## External Libraries/Frameworks Used
[SimpleIni](https://github.com/brofield/simpleini)

[Simple Injector](https://github.com/adamhlt/DLL-Injector) Used a lot of functions from there.
