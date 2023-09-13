#include <Windows.h>

#include "functions.h"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
  switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
      initialize_ta_emulator();
      MessageBoxA(nullptr,
                  "TurboActivate proxy loaded successfully\n\nIf you like the"
                  "software, please support the developers by purchasing a"
                  "legitimate copy.",
                  "turboactivate-emulator", MB_OK | MB_ICONINFORMATION);
      break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
    default:
      break;
  }
  return TRUE;
}
