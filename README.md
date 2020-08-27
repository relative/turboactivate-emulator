# turboactivate-emulator
Emulator for wyDay's LimeLM TurboActivate library. Acts as a proxy for TurboActivate DLLs in Win32 programs using it.

## Disclaimer
This project is not finished. Hopefully (if time permits) I plan to add
* configuration file (configure the license information, trial info, etc.)

## Usage
Download a binary from the [releases page](https://github.com/relative/turboactivate-emulator/releases/latest) on GitHub, or build from source

Rename the original TurboActivate dll (ex: TurboActivate32.dll) to turboactivate)orig.dll  

Place the downloaded DLL at the same path as the old TurboActivate dll (ex: TurboActivate32.dll)

## Building
Use Visual Studio 2019 with vc142 build tools.
