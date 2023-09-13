#include <shlobj.h>

#include <chrono>
#include <fstream>
#include <string>
#include <vector>

#include "TurboActivate.h"
#include "inih.h"

std::vector<void*> imports;
INIReader reader;

namespace config {
  namespace trial {
    bool enabled;
    bool usetrial_ok;
    uint32_t days_remaining;
  }  // namespace trial
  namespace license {
    bool enabled;
    bool is_activated;
    std::string product_key;
    bool valid_product_key;

    bool genuine;
  }  // namespace license

  namespace misc {
    bool antivm_detect;
  }

  namespace deactivate {
    bool force_ok;
  }
}  // namespace config

std::ofstream log_file;

// shamelessly stole these two functions from
// https://stackoverflow.com/a/3999597 i hate winapi Convert a wide Unicode
// string to an UTF8 string
std::string utf8_encode(const std::wstring& wstr) {
  if (wstr.empty()) return std::string();
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(),
                                        NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0],
                      size_needed, NULL, NULL);
  return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string& str) {
  if (str.empty()) return std::wstring();
  int size_needed =
      MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
  std::wstring wstrTo(size_needed, 0);
  MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0],
                      size_needed);
  return wstrTo;
}

void log_write(std::string str) {
  if (log_file.is_open()) log_file << str << std::endl;
}

void initialize_ta_emulator() {
  auto* const turboactivate = LoadLibraryA("turboactivate_orig.dll");
  if (!turboactivate) {
    MessageBoxA(nullptr,
                "turboactivate_orig.dll failed to be found.\nPlease place it "
                "in the same directory as the proxy.",
                "turboactivate-emulator", MB_OK | MB_ICONERROR);
    exit(1);
  }
  reader = INIReader("turboactivate.ini");
  if (reader.ParseError() != 0) {
    MessageBoxA(nullptr,
                "Failed to parse turboactivate.ini\nPlease check your "
                "configuration file.",
                "turboactivate-emulator", MB_OK | MB_ICONERROR);
    exit(1);
  }

  // load config values/defaults
  if (reader.GetBoolean("log", "enabled", true)) {
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                         std::chrono::system_clock::now().time_since_epoch())
                         .count();
    auto str_log_filename =
        std::string(".\\ta-emulator-" + std::to_string(timestamp) + ".log");
    log_file.open(str_log_filename.c_str(), std::ios::app | std::ios::out);
  }

  config::trial::enabled = reader.GetBoolean("trial", "enabled", true);
  config::trial::days_remaining =
      reader.GetInteger("trial", "days_remaining", 9999);
  config::trial::usetrial_ok = reader.GetBoolean("trial", "usetrial_ok", true);

  config::license::enabled = reader.GetBoolean("license", "enabled", false);
  config::license::is_activated =
      reader.GetBoolean("license", "is_activated", true);
  config::license::product_key = reader.Get("license", "product_key", "");
  config::license::valid_product_key =
      reader.GetBoolean("license", "valid_product_key", true);
  config::license::genuine = reader.GetBoolean("license", "genuine", true);

  config::misc::antivm_detect =
      reader.GetBoolean("misc", "antivm_detect", true);

  config::deactivate::force_ok =
      reader.GetBoolean("deactivate", "force_ok", false);

  imports.push_back(GetProcAddress(turboactivate, "TA_GetHandle"));
  imports.push_back(GetProcAddress(turboactivate, "TA_Activate"));
  imports.push_back(
      GetProcAddress(turboactivate, "TA_ActivationRequestToFile"));
  imports.push_back(GetProcAddress(turboactivate, "TA_ActivateFromFile"));
  imports.push_back(GetProcAddress(turboactivate, "TA_CheckAndSavePKey"));
  imports.push_back(GetProcAddress(turboactivate, "TA_Deactivate"));
  imports.push_back(
      GetProcAddress(turboactivate, "TA_DeactivationRequestToFile"));
  imports.push_back(GetProcAddress(turboactivate, "TA_GetExtraData"));
  imports.push_back(GetProcAddress(turboactivate, "TA_GetFeatureValue"));
  imports.push_back(GetProcAddress(turboactivate, "TA_GetPKey"));
  imports.push_back(GetProcAddress(turboactivate, "TA_IsActivated"));
  imports.push_back(GetProcAddress(turboactivate, "TA_IsGenuine"));
  imports.push_back(GetProcAddress(turboactivate, "TA_IsGenuineEx"));
  imports.push_back(GetProcAddress(turboactivate, "TA_GenuineDays"));
  imports.push_back(GetProcAddress(turboactivate, "TA_IsProductKeyValid"));
  imports.push_back(GetProcAddress(turboactivate, "TA_SetCustomProxy"));
  imports.push_back(GetProcAddress(turboactivate, "TA_TrialDaysRemaining"));
  imports.push_back(GetProcAddress(turboactivate, "TA_UseTrial"));
  imports.push_back(
      GetProcAddress(turboactivate, "TA_UseTrialVerifiedRequest"));
  imports.push_back(
      GetProcAddress(turboactivate, "TA_UseTrialVerifiedFromFile"));
  imports.push_back(GetProcAddress(turboactivate, "TA_ExtendTrial"));
  imports.push_back(GetProcAddress(turboactivate, "TA_SetTrialCallback"));
  imports.push_back(GetProcAddress(turboactivate, "TA_PDetsFromPath"));
  imports.push_back(GetProcAddress(turboactivate, "TA_PDetsFromByteArray"));
  imports.push_back(GetProcAddress(turboactivate, "TA_SetCustomActDataPath"));
  imports.push_back(GetProcAddress(turboactivate, "TA_Cleanup"));
  imports.push_back(GetProcAddress(turboactivate, "TA_IsDateValid"));
  imports.push_back(GetProcAddress(turboactivate, "TA_GetVersion"));
}

void ta_emulator_end() {
  if (log_file.is_open()) {
    log_file.close();
  }
}

TURBOACTIVATE_API uint32_t TA_CC TA_GetHandle(STRCTYPE versionGUID) {
  log_write("TA_GetHandle called!");
  using originalfn = uint32_t(TA_CC*)(STRCTYPE);
  return static_cast<originalfn>(imports.at(0))(versionGUID);
}

TURBOACTIVATE_API HRESULT TA_CC TA_Activate(uint32_t handle,
                                            PACTIVATE_OPTIONS options) {
  using originalfn = HRESULT(TA_CC*)(uint32_t, PACTIVATE_OPTIONS);
  log_write("TA_Activate called!");
  if (!config::license::enabled)
    return static_cast<originalfn>(imports.at(1))(handle, options);
  return TA_OK;
}

TURBOACTIVATE_API HRESULT TA_CC TA_ActivationRequestToFile(
    uint32_t handle, STRCTYPE filename, PACTIVATE_OPTIONS options) {
  log_write("TA_ActivationRequestToFile called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE, PACTIVATE_OPTIONS);
  return static_cast<originalfn>(imports.at(2))(handle, filename, options);
}

TURBOACTIVATE_API HRESULT TA_CC TA_ActivateFromFile(uint32_t handle,
                                                    STRCTYPE filename) {
  log_write("TA_ActivateFromFile called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE);
  return static_cast<originalfn>(imports.at(3))(handle, filename);
}

TURBOACTIVATE_API HRESULT TA_CC TA_CheckAndSavePKey(uint32_t handle,
                                                    STRCTYPE productKey,
                                                    uint32_t flags) {
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE, uint32_t);
  log_write("TA_CheckAndSavePKey called!");
  if (!config::license::enabled)
    return static_cast<originalfn>(imports.at(4))(handle, productKey, flags);

  // TODO: add more customization for this return?
  return TA_OK;
}

TURBOACTIVATE_API HRESULT TA_CC TA_Deactivate(uint32_t handle, char erasePkey) {
  using originalfn = HRESULT(TA_CC*)(uint32_t, char);
  log_write("TA_Deactivate called!");
  if (config::deactivate::force_ok) return TA_OK;
  return static_cast<originalfn>(imports.at(5))(handle, erasePkey);
}

TURBOACTIVATE_API HRESULT TA_CC TA_DeactivationRequestToFile(uint32_t handle,
                                                             STRCTYPE filename,
                                                             char erasePkey) {
  log_write("TA_DeactivationRequestToFile called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE, char);
  return static_cast<originalfn>(imports.at(6))(handle, filename, erasePkey);
}

TURBOACTIVATE_API HRESULT TA_CC TA_GetExtraData(uint32_t handle,
                                                STRTYPE lpValueStr,
                                                int cchValue) {
  log_write("TA_GetExtraData called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRTYPE, int);
  return static_cast<originalfn>(imports.at(7))(handle, lpValueStr, cchValue);
}

TURBOACTIVATE_API HRESULT TA_CC TA_GetFeatureValue(uint32_t handle,
                                                   STRCTYPE featureName,
                                                   STRTYPE lpValueStr,
                                                   int cchValue) {
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE, STRTYPE, int);
  const auto str = utf8_encode(featureName);
  log_write("TA_GetFeatureValue called with featureName=" + str);

  if (reader.Get("features", str, "unlikelyvaluetobeused") !=
      "unlikelyvaluetobeused") {
    auto val = utf8_decode(reader.Get("features", str, ""));
    if (cchValue == 0) {
      // return required size of buffer
      return val.size();
    }
    wcscpy_s(lpValueStr, cchValue,
             val.c_str());  // theoretically should work, i hate c++
    return TA_OK;
  }

  if (cchValue == 0) {
    auto size = static_cast<originalfn>(imports.at(8))(
        handle, featureName, lpValueStr,
        cchValue);  // returns req size of buffer
    log_write("TA_GetFeatureValue(" + str + ") = (size) " +
              std::to_string(size));
    return size;
  }
  auto resp = static_cast<originalfn>(imports.at(8))(handle, featureName,
                                                     lpValueStr, cchValue);
  log_write("TA_GetFeatureValue(" + str + ") = " + utf8_encode(lpValueStr));

  return resp;
}

TURBOACTIVATE_API HRESULT TA_CC TA_GetPKey(uint32_t handle, STRTYPE lpValueStr,
                                           int cchValue) {
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRTYPE, int);
  log_write("TA_GetPKey called!");
  if (!config::license::enabled)
    return static_cast<originalfn>(imports.at(9))(handle, lpValueStr, cchValue);
  auto val = utf8_decode(config::license::product_key);
  wcscpy_s(lpValueStr, cchValue,
           val.c_str());  // theoretically should work, i hate c++
  return TA_OK;
}

TURBOACTIVATE_API HRESULT TA_CC TA_IsActivated(uint32_t handle) {
  using originalfn = HRESULT(TA_CC*)(uint32_t);
  log_write("TA_IsActivated called!");
  if (!config::license::enabled)
    return static_cast<originalfn>(imports.at(10))(handle);
  return config::license::is_activated ? TA_OK : TA_FAIL;
}

TURBOACTIVATE_API HRESULT TA_CC TA_IsGenuine(uint32_t handle) {
  using originalfn = HRESULT(TA_CC*)(uint32_t);
  log_write("TA_IsGenuine called!");
  if (!config::license::enabled)
    return static_cast<originalfn>(imports.at(11))(handle);
  return config::license::genuine ? TA_OK : TA_E_ACTIVATE;
}

TURBOACTIVATE_API HRESULT TA_CC TA_IsGenuineEx(uint32_t handle,
                                               PGENUINE_OPTIONS options) {
  using originalfn = HRESULT(TA_CC*)(uint32_t, PGENUINE_OPTIONS);
  log_write("TA_IsGenuineEx called!");
  if (!config::license::enabled)
    return static_cast<originalfn>(imports.at(12))(handle, options);
  return config::license::genuine ? TA_OK : TA_E_ACTIVATE;
}

TURBOACTIVATE_API HRESULT TA_CC TA_GenuineDays(uint32_t handle,
                                               uint32_t nDaysBetweenChecks,
                                               uint32_t nGraceDaysOnInetErr,
                                               uint32_t* DaysRemaining,
                                               char* inGracePeriod) {
  log_write("TA_GenuineDays called!");
  using originalfn =
      HRESULT(TA_CC*)(uint32_t, uint32_t, uint32_t, uint32_t*, char*);
  return static_cast<originalfn>(imports.at(13))(handle, nDaysBetweenChecks,
                                                 nGraceDaysOnInetErr,
                                                 DaysRemaining, inGracePeriod);
}

TURBOACTIVATE_API HRESULT TA_CC TA_IsProductKeyValid(uint32_t handle) {
  using originalfn = HRESULT(TA_CC*)(uint32_t);
  log_write("TA_IsProductKeyValid called!");
  if (!config::license::enabled)
    return static_cast<originalfn>(imports.at(14))(handle);
  return config::license::valid_product_key ? TA_OK : TA_FAIL;
}

TURBOACTIVATE_API HRESULT TA_CC TA_SetCustomProxy(STRCTYPE proxy) {
  log_write("TA_SetCustomProxy called!");
  using originalfn = HRESULT(TA_CC*)(STRCTYPE);
  return static_cast<originalfn>(imports.at(15))(proxy);
}

TURBOACTIVATE_API HRESULT TA_CC TA_TrialDaysRemaining(uint32_t handle,
                                                      uint32_t useTrialFlags,
                                                      uint32_t* DaysRemaining) {
  log_write("TA_TrialDaysRemaining called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, uint32_t, uint32_t*);

  if (config::misc::antivm_detect) {
    useTrialFlags = useTrialFlags & ~TA_DISALLOW_VM;
  }

  if (!config::trial::enabled)
    return static_cast<originalfn>(imports.at(16))(handle, useTrialFlags,
                                                   DaysRemaining);

  *DaysRemaining = config::trial::days_remaining;
  return TA_OK;
}

TURBOACTIVATE_API HRESULT TA_CC TA_UseTrial(uint32_t handle, uint32_t flags,
                                            STRCTYPE extra_data) {
  log_write("TA_UseTrial called!");
  if (config::misc::antivm_detect) {
    flags &= ~TA_DISALLOW_VM;
  }

  if (config::trial::usetrial_ok) {
    return TA_OK;
  }

  using originalfn = HRESULT(TA_CC*)(uint32_t, uint32_t, STRCTYPE);
  auto retval =
      static_cast<originalfn>(imports.at(17))(handle, flags, extra_data);
  if (config::misc::antivm_detect && retval == TA_E_IN_VM) retval = TA_OK;
  return retval;
}

TURBOACTIVATE_API HRESULT TA_CC TA_UseTrialVerifiedRequest(
    uint32_t handle, STRCTYPE filename, STRCTYPE extra_data) {
  log_write("TA_UseTrialVerifiedRequest called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE, STRCTYPE);
  return static_cast<originalfn>(imports.at(18))(handle, filename, extra_data);
}

TURBOACTIVATE_API HRESULT TA_CC TA_UseTrialVerifiedFromFile(uint32_t handle,
                                                            STRCTYPE filename,
                                                            uint32_t flags) {
  log_write("TA_UseTrialVerifiedFromFile called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE, uint32_t);
  return static_cast<originalfn>(imports.at(19))(handle, filename, flags);
}

TURBOACTIVATE_API HRESULT TA_CC TA_ExtendTrial(uint32_t handle,
                                               uint32_t useTrialFlags,
                                               STRCTYPE trialExtension) {
  log_write("TA_ExtendTrial called!");

  if (config::misc::antivm_detect) {
    useTrialFlags = useTrialFlags & ~TA_DISALLOW_VM;
  }

  using originalfn = HRESULT(TA_CC*)(uint32_t, uint32_t, STRCTYPE);
  return static_cast<originalfn>(imports.at(20))(handle, useTrialFlags,
                                                 trialExtension);
}

TURBOACTIVATE_API HRESULT TA_CC TA_SetTrialCallback(uint32_t handle,
                                                    TrialCallbackType callback,
                                                    void* userDefinedPtr) {
  log_write("TA_SetTrialCallback called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, TrialCallbackType, void*);
  return static_cast<originalfn>(imports.at(21))(handle, callback,
                                                 userDefinedPtr);
}

TURBOACTIVATE_API HRESULT TA_CC TA_PDetsFromPath(STRCTYPE filename) {
  log_write("TA_PDetsFromPath called!");
  using originalfn = HRESULT(TA_CC*)(STRCTYPE);
  return static_cast<originalfn>(imports.at(22))(filename);
}

TURBOACTIVATE_API HRESULT TA_CC TA_PDetsFromByteArray(const uint8_t* byteArr,
                                                      size_t byteArrLen) {
  log_write("TA_PDetsFromByteArray called!");
  using originalfn = HRESULT(TA_CC*)(const uint8_t*, size_t);
  return static_cast<originalfn>(imports.at(23))(byteArr, byteArrLen);
}

TURBOACTIVATE_API HRESULT TA_CC TA_SetCustomActDataPath(uint32_t handle,
                                                        STRCTYPE directory) {
  log_write("TA_SetCustomActDataPath called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE);
  return static_cast<originalfn>(imports.at(24))(handle, directory);
}

TURBOACTIVATE_API HRESULT TA_CC TA_Cleanup(void) {
  log_write("TA_Cleanup called!");
  using originalfn = HRESULT(TA_CC*)(void);
  return static_cast<originalfn>(imports.at(25))();
}

TURBOACTIVATE_API HRESULT TA_CC TA_IsDateValid(uint32_t handle,
                                               STRCTYPE date_time,
                                               uint32_t flags) {
  auto str = utf8_encode(date_time);
  log_write("TA_IsDateValid(" + str + ") called!");
  using originalfn = HRESULT(TA_CC*)(uint32_t, STRCTYPE, uint32_t);
  return static_cast<originalfn>(imports.at(26))(handle, date_time, flags);
}

TURBOACTIVATE_API HRESULT TA_CC TA_GetVersion(uint32_t* MajorVersion,
                                              uint32_t* MinorVersion,
                                              uint32_t* BuildVersion,
                                              uint32_t* RevisionVersion) {
  log_write("TA_GetVersion called!");
  using originalfn =
      HRESULT(TA_CC*)(uint32_t*, uint32_t*, uint32_t*, uint32_t*);
  return static_cast<originalfn>(imports.at(27))(MajorVersion, MinorVersion,
                                                 BuildVersion, RevisionVersion);
}
