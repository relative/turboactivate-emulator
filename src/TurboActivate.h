#ifndef TURBOACTIVATE_H_INCLUDED
#define TURBOACTIVATE_H_INCLUDED

/*
  Scroll down below the list of includes & defines for
  a list of every function and error code with full
  detailed descriptions of everything.
*/

#define TURBOACTIVATE_EXPORTS

/* For size_t definition */
#include <stddef.h>

/* Define "uint32_t" for old versions of MS C++ (VS 2008 and below) */
#if !defined(_WIN32) || _MSC_VER >= 1600
#    include <stdint.h>
#else
typedef unsigned int  uint32_t;
typedef unsigned char uint8_t;
#endif

#ifdef _WIN32
#    include <windows.h>

#    ifdef TURBOACTIVATE_EXPORTS
#        ifdef TURBOACTIVATE_STATIC
#            define TURBOACTIVATE_API extern "C"
#        else
#            define TURBOACTIVATE_API extern "C" __declspec(dllexport)
#        endif
#    else
#        ifdef __cplusplus
#            ifdef TURBOACTIVATE_STATIC
#                define TURBOACTIVATE_API extern "C"
#            else
#                define TURBOACTIVATE_API extern "C" __declspec(dllimport)
#            endif
#        else
#            ifdef TURBOACTIVATE_STATIC
#                define TURBOACTIVATE_API
#            else
#                define TURBOACTIVATE_API __declspec(dllimport)
#            endif
#        endif
#    endif

#    if defined(USE_STDCALL_TA_DLL) && !defined(TURBOACTIVATE_STATIC)
#        define TA_CC __stdcall
#    else
#        define TA_CC __cdecl
#    endif
#    define TA_DEPRECATED(func) __declspec(deprecated) func

#    ifndef LIMELM_TYPES
#        define LIMELM_TYPES
typedef LPWSTR  STRTYPE;
typedef LPCWSTR STRCTYPE;
#    endif
#else
#    if __GNUC__ >= 4 || defined(__SUNPRO_CC)
#        ifdef __cplusplus
#            define TURBOACTIVATE_API extern "C" __attribute__((visibility("default")))
#        else
#            define TURBOACTIVATE_API __attribute__((visibility("default")))
#        endif
#    else
#        ifdef __cplusplus
#            define TURBOACTIVATE_API extern "C"
#        else
#            define TURBOACTIVATE_API
#        endif
#    endif

#    define TA_CC
#    ifdef __GNUC__
#        define TA_DEPRECATED(func) func __attribute__((deprecated))
#    else
#        pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#        define TA_DEPRECATED(func) func
#    endif

#    ifndef LIMELM_TYPES
#        define LIMELM_TYPES
typedef char* STRTYPE;
typedef const char* STRCTYPE;
typedef int32_t       HRESULT;
typedef char          TCHAR;
#    endif

#    ifdef __ANDROID__
#        include <jni.h>
#    endif
#endif


typedef struct _ACTIVATE_OPTIONS
{
    /* The size, in bytes, of this structure. Set this value to
    the size of the ACTIVATE_OPTIONS structure. */
    uint32_t nLength;

    /* Extra data to pass to the LimeLM servers that will be visible for
    you to see and use. Maximum size is 255 UTF-8 characters. */
    STRCTYPE sExtraData;
} ACTIVATE_OPTIONS, * PACTIVATE_OPTIONS;


/* Flags for the TA_IsGeninueEx() function. */

/* If the user activated using offline activation
   (TA_ActivateRequestToFile(), TA_ActivateFromFile() ), then with this
   flag TA_IsGenuineEx() will still try to validate with the LimeLM
   servers, however instead of returning TA_E_INET (when within the
   grace period) or TA_FAIL (when past the grace period) it will
   instead only return TA_OK (if machine has a valid activation).

   If you still want to get the TA_E_INET error code, without
   deactivating after the grace period has expired, then use
   this flag in tandem with TA_OFFLINE_SHOW_INET_ERR.

   If the user activated using online activation then this flag
   is ignored.
*/
#define TA_SKIP_OFFLINE ((uint32_t)1)

/* If the user activated using offline activation, and you're
   using this flag in tandem with TA_SKIP_OFFLINE, then TA_IsGenuineEx()
   will return TA_E_INET on internet failure instead of TA_OK.

   If the user activated using online activation then this flag
   is ignored.
*/
#define TA_OFFLINE_SHOW_INET_ERR ((uint32_t)2)

typedef struct _GENUINE_OPTIONS
{
    /* The size, in bytes, of this structure. Set this value to
    the size of the GENUINE_OPTIONS structure. */
    uint32_t nLength;

    /* Flags to pass to TA_IsGenuineEx() to control its behavior. */
    uint32_t flags;

    /* How often to contact the LimeLM servers for validation. 90 days recommended. */
    uint32_t nDaysBetweenChecks;

    /* If the call fails because of an internet error,
    how long, in days, should the grace period last (before
    returning deactivating and returning TA_FAIL).

    14 days is recommended. */
    uint32_t nGraceDaysOnInetErr;
} GENUINE_OPTIONS, * PGENUINE_OPTIONS;


/* Flags for the TA_UseTrial() and TA_CheckAndSavePKey() functions. */
#define TA_SYSTEM ((uint32_t)1)
#define TA_USER ((uint32_t)2)

/*
When to use TA_USER or TA_SYSTEM
=======================================

TA_USER saves the activation or trial data in the current user's profile if
(and only if) calling the function with TA_SYSTEM had not previously occurred.

TA_SYSTEM saves the activation or trial data in a place accessible by all user
profiles on the system.


In both cases the product key is activated and "locked" to the machine. The
only difference between TA_SYSTEM and TA_USER is where the files are stored.
In other words if one users activates your app on a machine using TA_USER,
another user can activate your app on the same machine, using the same product
key.

If you want to license your software per-user on machines, then use TurboFloat:

https://wyday.com/limelm/help/using-turbofloat/



For almost all cases we recommend using TA_SYSTEM.
*/


/* TA_UseTrial() specific flags:
   =============================
*/

/*
   Use the TA_DISALLOW_VM flag to disallow trials in virtual machines.
   If you use this flag in TA_UseTrial() and the customer's machine is a Virtual
   Machine, then TA_UseTrial() will return TA_E_IN_VM.
*/
#define TA_DISALLOW_VM ((uint32_t)4)

/*
   Use this flag in TA_UseTrial() to tell TurboActivate to use client-side
   unverified trials. For more information about verified vs. unverified trials,
   see here: https://wyday.com/limelm/help/trials/
   Note: unverified trials are unsecured and can be reset by malicious customers.
*/
#define TA_UNVERIFIED_TRIAL ((uint32_t)16)

/*
   Use the TA_VERIFIED_TRIAL flag to use verified trials instead
   of unverified trials. This means the trial is locked to a particular computer.
   The customer can't reset the trial.
*/
#define TA_VERIFIED_TRIAL ((uint32_t)32)


/*
   Gets the handle that will be used for the TurboActivate functions.


   Returns: 0 on failure. All other return codes are successful.

   If you're getting a "0" return code then it's most likely a result
   of TurboActivate being unable to load your TurboActivate.dat file. In
   that case use TA_PDetsFromPath to load the file first before calling
   TA_GetHandle.
*/
TURBOACTIVATE_API uint32_t TA_CC TA_GetHandle(STRCTYPE versionGUID);


/*
   Activates the product on this computer. You must call TA_CheckAndSavePKey()
   with a valid product key or have used the TurboActivate Wizard sometime
   before calling this function.

   If you don't want to pass in extra data you can set "options" to null.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PKEY, TA_E_INET, TA_E_INET_TIMEOUT,
                          TA_E_INET_TLS, TA_E_INUSE, TA_E_REVOKED,
                          TA_E_INVALID_HANDLE, TA_E_COM, TA_E_EXPIRED,
                          TA_E_EDATA_LONG, TA_E_INVALID_ARGS, TA_E_IN_VM,
                          TA_E_KEY_FOR_TURBOFLOAT, TA_E_ANDROID_NOT_INIT,
                          TA_E_ACCOUNT_CANCELED, TA_E_ENABLE_NETWORK_ADAPTERS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_Activate(uint32_t handle, PACTIVATE_OPTIONS options);


/*
   Get the "activation request" file for offline activation. You must call TA_CheckAndSavePKey()
   with a valid product key or have used the TurboActivate Wizard sometime
   before calling this function.

   If you don't want to pass in extra data you can set "options" to null.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PKEY, TA_E_INVALID_HANDLE, TA_E_COM,
                          TA_E_EDATA_LONG, TA_E_INVALID_ARGS, TA_E_ANDROID_NOT_INIT,
                          TA_E_ENABLE_NETWORK_ADAPTERS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_ActivationRequestToFile(uint32_t handle, STRCTYPE filename, PACTIVATE_OPTIONS options);


/*
   Activate from the "activation response" file for offline activation.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PKEY, TA_E_INVALID_HANDLE, TA_E_COM,
                          TA_E_EXPIRED, TA_E_IN_VM, TA_E_ANDROID_NOT_INIT,
                          TA_E_ENABLE_NETWORK_ADAPTERS, TA_E_INVALID_ARGS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_ActivateFromFile(uint32_t handle, STRCTYPE filename);


/*
   Checks and saves the product key.


   Note: If you pass in the TA_SYSTEM flag and you don't have "admin" or "elevated"
   permission then the call will fail.

   If you call this function once in the past with the flag TA_SYSTEM and the calling
   process was an admin process then subsequent calls with the TA_SYSTEM flag will
   succeed even if the calling process is *not* admin/elevated.

   If you want to take advantage of this behavior from an admin process
   (e.g. an installer) but the user hasn't entered a product key then you can
   call this function with a null string:

            TA_CheckAndSavePKey(yourHandle, 0, TA_SYSTEM);

   This will set everything up so that subsequent calls with the TA_SYSTEM flag will
   succeed even if from non-admin processes.

   Note: Calling TA_CheckAndSavePKey(yourHandle, 0, TA_SYSTEM) will return TA_FAIL
         if the system files were correctly setup. It will return TA_E_PERMISSION if
         the system files were not setup due to lack of permissions. The reason it
         returns TA_FAIL instead of TA_OK is because passing in a "NULL" product key
         is not valid.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PERMISSION, TA_E_INVALID_FLAGS
                          TA_E_ALREADY_ACTIVATED
*/
TURBOACTIVATE_API HRESULT TA_CC TA_CheckAndSavePKey(uint32_t handle, STRCTYPE productKey, uint32_t flags);


/*
   Deactivates the product on this computer. Set erasePkey to 1 to erase the stored
   product key, 0 to keep the product key around. If you're using deactivate to let
   a user move between computers it's almost always best to *not* erase the product
   key. This way you can just use TA_Activate() when the user wants to reactivate
   instead of forcing the user to re-enter their product key over-and-over again.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PKEY, TA_E_ACTIVATE, TA_E_INET,
                          TA_E_INET_TIMEOUT, TA_E_INET_TLS, TA_E_INVALID_HANDLE,
                          TA_E_COM, TA_E_ANDROID_NOT_INIT,
                          TA_E_NO_MORE_DEACTIVATIONS, TA_E_INVALID_ARGS,
                          TA_E_ENABLE_NETWORK_ADAPTERS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_Deactivate(uint32_t handle, char erasePkey);


/*
   Get the "deactivation request" file for offline deactivation.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PKEY, TA_E_ACTIVATE, TA_E_INVALID_HANDLE,
                          TA_E_COM, TA_E_ANDROID_NOT_INIT, TA_E_INVALID_ARGS,
                          TA_E_ENABLE_NETWORK_ADAPTERS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_DeactivationRequestToFile(uint32_t handle, STRCTYPE filename, char erasePkey);


/*
   Gets the extra data you passed in using TA_Activate().


   lpValueStr
   [out] Pointer to a buffer that receives the value of the string.

   cchValue
   [in] Size (in wide characters on Windows or characters in Unix) of the buffer
        pointed to by the lpValueStr parameter.

   If 'cchValue' is zero, the function returns the required buffer size (in wide characters
   on Windows, characters on Unix) and makes no use of the lpValueStr buffer.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE, TA_E_INSUFFICIENT_BUFFER
*/
TURBOACTIVATE_API HRESULT TA_CC TA_GetExtraData(uint32_t handle, STRTYPE lpValueStr, int cchValue);


/*
   Gets the value of a custom license field.

   More information on custom license fields: https://wyday.com/limelm/help/license-features/


   lpValueStr
   [out] Pointer to a buffer that receives the value of the string.

   cchValue
   [in] Size (in wide characters on Windows or characters in Unix) of the buffer
        pointed to by the lpValueStr parameter.

   If 'cchValue' is zero, the function returns the required buffer size (in wide characters
   on Windows, characters on Unix) and makes no use of the lpValueStr buffer.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE, TA_E_INSUFFICIENT_BUFFER
*/
TURBOACTIVATE_API HRESULT TA_CC TA_GetFeatureValue(uint32_t handle, STRCTYPE featureName, STRTYPE lpValueStr, int cchValue);


/*
   Gets the stored product key. NOTE: if you want to check if a product key is valid
   simply call TA_IsProductKeyValid().


   lpValueStr
   [out] Pointer to a buffer that receives the value of the string.

   cchValue
   [in] Size (in wide characters on Windows or characters in Unix) of the buffer
        pointed to by the lpValueStr parameter.

   If 'cchValue' is zero, the function returns the required buffer size (in wide characters
   on Windows, characters on Unix) and makes no use of the lpValueStr buffer.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PKEY, TA_E_INVALID_HANDLE,
                          TA_E_INSUFFICIENT_BUFFER
*/
TURBOACTIVATE_API HRESULT TA_CC TA_GetPKey(uint32_t handle, STRTYPE lpValueStr, int cchValue);


/*
   Checks whether the computer is activated. This does not re-verify with the activation
   servers. It just checks the cryptographically-signed activation data stored on the computer
   and verify the current hardware-fingerprint "fuzzy matches" the signed activation data.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE, TA_E_COM,
                          TA_E_IN_VM, TA_E_ANDROID_NOT_INIT, TA_E_ENABLE_NETWORK_ADAPTERS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_IsActivated(uint32_t handle);


/*
   Checks whether the computer is genuinely activated by verifying with the LimeLM servers immediately.
   If reactivation is needed then it will do this as well.

   For almost all use-cases you should use TA_IsGenuineEx(), not this function.


   Returns: TA_OK or TA_E_FEATURES_CHANGED on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_ACTIVATE, TA_E_INET, TA_E_INET_TIMEOUT, TA_E_INET_TLS,
                          TA_E_INVALID_HANDLE, TA_E_COM, TA_E_EXPIRED, TA_E_REVOKED, TA_E_IN_VM,
                          TA_E_FEATURES_CHANGED, TA_E_ANDROID_NOT_INIT, TA_E_ENABLE_NETWORK_ADAPTERS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_IsGenuine(uint32_t handle);


/*
   Checks whether the computer is genuinely activated by verifying with the LimeLM servers
   after a certain number of days you specify.

   This is meant as a replacement of both TA_IsActivated() and TA_IsGenuine(). Call this at the
   top of your program and let TA_IsGenuineEx() handle all the details.

   This differs with TA_IsGenuine() in 3 major ways:

        1. You can specify how often to verify with the LimeLM servers and it handles
           all the date tracking behind the scenes.


        2. TA_IsGenuineEx() prevents your app from hammering the end-user's network after
           and TA_E_INET error return code by not checking with the LimeLM servers until
           at least 5 hours has passed. If you call TA_IsGenuineEx() after a TA_E_INET return
           and before 5 hours has elapsed then this function will return TA_E_INET_DELAYED.

           (If you give the user the option to recheck with LimeLM, e.g. via a button
           like "Retry now" then call TA_IsGenuine() to immediately retry without waiting 5 hours).


        3. If a TA_E_INET error is being returned, and the grace period has expired,
           then TA_IsGenuineEx() will return TA_FAIL. TA_IsGenuineEx() will continue to try
           contacting the LimeLM servers on subsequent calls (5 hours apart), but you
           should treat the TA_FAIL as a hard failure.


   Returns: TA_OK or TA_E_FEATURES_CHANGED on success. Handle TA_E_INET and TA_E_INET_DELAYED as warnings that
            you should let the end user know about.

            If an "TA_E_INET_TIMEOUT" or "TA_E_INET_TLS" error happens, TA_E_INET will be returned instead
            as a more "catch-all" inernet failure. This is the only TurboActivate function that will
            return the more "general error" rather than a specific error. We do this for backwards
            compatibility with existing programs.

            Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_ACTIVATE, TA_E_INET, TA_E_INET_DELAYED, TA_E_INVALID_HANDLE,
                          TA_E_COM, TA_E_EXPIRED, TA_E_REVOKED, TA_E_INVALID_ARGS,
                          TA_E_INVALID_FLAGS, TA_E_IN_VM, TA_E_FEATURES_CHANGED,
                          TA_E_ANDROID_NOT_INIT, TA_E_ENABLE_NETWORK_ADAPTERS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_IsGenuineEx(uint32_t handle, PGENUINE_OPTIONS options);


/*
   Get the number of days until the next time that the TA_IsGenuineEx() function contacts
   the LimeLM activation servers to reverify the activation.

   You must use the same "nDaysBetweenChecks" and "nGraceDaysOnInetErr" parameters you passed
   to TA_IsGenuineEx() using the GENUINE_OPTIONS structure.

   The number of days remaining until the next reverification with the servers will be put
   in the "DaysRemaining" variable. And if the customer is already in the grace period, then
   the "DaysRemaining" remaining will reflect the number of days left in the grace period and
   "inGracePeriod" will be 1.

   If both nDaysBetweenChecks and the nGraceDaysOnInetErr have passed, then "DaysRemaining"
   will be 0.

   Also, if TurboActivate determines that system date, time, or timezone are fraudulent then
   "DaysRemaining" will be 0.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_ACTIVATE, TA_E_INVALID_HANDLE
*/
TURBOACTIVATE_API HRESULT TA_CC TA_GenuineDays(uint32_t handle, uint32_t nDaysBetweenChecks, uint32_t nGraceDaysOnInetErr, uint32_t* DaysRemaining, char* inGracePeriod);


/*
   Checks if the product key installed for this product is valid. This does NOT check if
   the product key is activated or genuine. Use TA_IsActivated() and TA_IsGenuine() instead.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE
*/
TURBOACTIVATE_API HRESULT TA_CC TA_IsProductKeyValid(uint32_t handle);


/*
   Sets the custom proxy to be used by functions that connect to the internet.


   Proxy in the form: http://username:password@host:port/

   Example 1 (just a host): http://127.0.0.1/
   Example 2 (host and port): http://127.0.0.1:8080/
   Example 3 (all 3): http://user:pass@127.0.0.1:8080/

   If the port is not specified, TurboActivate will default to using port 1080 for proxies.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL
*/
TURBOACTIVATE_API HRESULT TA_CC TA_SetCustomProxy(STRCTYPE proxy);


/*
   Get the number of trial days remaining.
   0 days if the trial has expired or has been tampered with
   (1 day means *at most* 1 day, that is it could be 30 seconds)

   You must call TA_UseTrial() at least once in the past before calling this function.
   And you must call this function with the same flags you used with TA_UseTrial().


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE, TA_E_ALREADY_VERIFIED_TRIAL,
                          TA_E_MUST_USE_TRIAL, TA_E_MUST_SPECIFY_TRIAL_TYPE
*/
TURBOACTIVATE_API HRESULT TA_CC TA_TrialDaysRemaining(uint32_t handle, uint32_t useTrialFlags, uint32_t* DaysRemaining);


/*
   Begins the trial the first time it's called. Calling it again will validate the trial
   data hasn't been tampered with.

   It is recommended that you use the following flag combination: TA_VERIFIED_TRIAL | TA_SYSTEM


   Note: If you pass in the TA_SYSTEM flag and you don't have "admin" or "elevated"
   permission then the call will fail.

   If you call this function once in the past with the flag TA_SYSTEM and the calling
   process was an admin process then subsequent calls with the TA_SYSTEM flag will
   succeed even if the calling process is *not* admin/elevated.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE, TA_E_INET, TA_E_INET_TIMEOUT,
                          TA_E_INET_TLS, TA_E_PERMISSION, TA_E_COM, TA_E_INVALID_FLAGS,
                          TA_E_IN_VM, TA_E_ANDROID_NOT_INIT,
                          TA_E_ACCOUNT_CANCELED, TA_E_ENABLE_NETWORK_ADAPTERS,
                          TA_E_ALREADY_VERIFIED_TRIAL, TA_E_EXPIRED, TA_E_TRIAL_EXPIRED,
                          TA_E_MUST_SPECIFY_TRIAL_TYPE, TA_E_EDATA_LONG,
                          TA_E_NO_MORE_TRIALS_ALLOWED
*/
TURBOACTIVATE_API HRESULT TA_CC TA_UseTrial(uint32_t handle, uint32_t flags, STRCTYPE extra_data);


/*
   Generate a "verified trial" offline request file. This file will then need to be submitted to LimeLM.
   You will then need to use the TA_UseTrialVerifiedFromFile() function with the response file from LimeLM
   to actually start the trial.


   The easier solution is to use verified trials online. Simply use the TA_UseTrial() function with
   the TA_VERIFIED_TRIAL | TA_SYSTEM flags.



   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE, TA_E_PERMISSION, TA_E_COM
                          TA_E_ANDROID_NOT_INIT, TA_E_ENABLE_NETWORK_ADAPTERS,
                          TA_E_EDATA_LONG, TA_E_INVALID_ARGS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_UseTrialVerifiedRequest(uint32_t handle, STRCTYPE filename, STRCTYPE extra_data);


/*
   Use the "verified trial response" from LimeLM to start the verified trial.



   To use verified trials online, simply use the TA_UseTrial() function with the TA_VERIFIED_TRIAL | TA_SYSTEM
   flags.


   Note: If you pass in the TA_SYSTEM flag and you don't have "admin" or "elevated"
   permission then the call will fail.

   If you call this function once in the past with the flag TA_SYSTEM and the calling
   process was an admin process then subsequent calls with the TA_SYSTEM flag will
   succeed even if the calling process is *not* admin/elevated.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE, TA_E_PERMISSION, TA_E_COM
                          TA_E_INVALID_FLAGS, TA_E_IN_VM,
                          TA_E_ANDROID_NOT_INIT, TA_E_ENABLE_NETWORK_ADAPTERS,
                          TA_E_MUST_SPECIFY_TRIAL_TYPE, TA_E_INVALID_ARGS
*/
TURBOACTIVATE_API HRESULT TA_CC TA_UseTrialVerifiedFromFile(uint32_t handle, STRCTYPE filename, uint32_t flags);


/*
   Extends the trial using a trial extension created in LimeLM.

   You must call TA_UseTrial() at least once in the past before calling this function.
   And you must call this function with the same flags you used with TA_UseTrial().


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INET, TA_E_INET_TIMEOUT, TA_E_INET_TLS,
                          TA_E_INVALID_HANDLE, TA_E_TRIAL_EUSED, TA_E_TRIAL_EEXP,
                          TA_E_MUST_USE_TRIAL, TA_E_MUST_SPECIFY_TRIAL_TYPE
*/
TURBOACTIVATE_API HRESULT TA_CC TA_ExtendTrial(uint32_t handle, uint32_t useTrialFlags, STRCTYPE trialExtension);


/*
The lease callback function type with a user-defined pointer.
*/
typedef void(TA_CC* TrialCallbackType)(uint32_t, void*);

/*
   Set the function that will be called by a separate background thread that notifies
   your app of trial expiration (either naturally, or because of customer fraud).

   TA_SetTrialCallback() also allow you to pass allong "context" in the form
   of a pointer that you defined. This is passed back along by the TurboActivate
   Library when the callback function is called.

   The trial callback function should handle everything defined below (see:
   "Possible callback statuses" at the bottom of this header).


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE
*/
TURBOACTIVATE_API HRESULT TA_CC TA_SetTrialCallback(uint32_t handle, TrialCallbackType callback, void* userDefinedPtr);


/*
   Loads the "TurboActivate.dat" file from a path rather than loading it
   from the same dir as TurboActivate.dll on Windows or the app that
   uses libTurboActivate.dylib / libTurboActivate.so on Mac / Linux.

   You can load multiple *.dat files for licensing multiple products within
   one process. You can switch between the products by using the
   TA_GetHandle() with the VersionGUID of the product you want to use
   licensing for.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PDETS


   Note: Once the product details file has loaded, all subsequent calls to "PDetsFromPath"
   will return TA_FAIL
*/
TURBOACTIVATE_API HRESULT TA_CC TA_PDetsFromPath(STRCTYPE filename);


/*
   Loads the "TurboActivate.dat" file from a byte array rather than loading it
   from the same dir as TurboActivate.dll on Windows or the app that
   uses libTurboActivate.dylib / libTurboActivate.so on Mac / Linux.

   You can load multiple *.dat files for licensing multiple products within
   one process. You can switch between the products by using the
   TA_GetHandle() with the VersionGUID of the product you want to use
   licensing for.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_PDETS


   Note: Once the product details file has loaded, all subsequent calls to "PDetsFromPath"
   will return TA_FAIL
*/
TURBOACTIVATE_API HRESULT TA_CC TA_PDetsFromByteArray(const uint8_t* byteArr, size_t byteArrLen);


/*
   This function allows you to set a custom folder to store the activation
   data files. For normal use we do not recommend you use this function.

   Only use this function if you absolutely must store data into a separate
   folder. For example if your application runs on a USB drive and can't write
   any files to the main disk, then you can use this function to save the activation
   data files to a directory on the USB disk.

   If you are using this function (which we only recommend for very special use-cases)
   then you must call this function on every start of your program at the very top of
   your app before any other functions are called.

   The directory you pass in must already exist. And the process using TurboActivate
   must have permission to create, write, and delete files in that directory.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_HANDLE
*/
TURBOACTIVATE_API HRESULT TA_CC TA_SetCustomActDataPath(uint32_t handle, STRCTYPE directory);


/*
   You should call this before your application exits. This frees up any
   allocated memory for all open handles.


   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL
*/
TURBOACTIVATE_API HRESULT TA_CC TA_Cleanup(void);


/* Flags for the TA_IsDateValid() function. */

/* With this flag, TA_IsDateValid() will return TA_OK if the passed in "date_time"
   has not expired and the system dates have not been tampered with. Otherwise,
   TA_FAIL will be returned.
*/
#define TA_HAS_NOT_EXPIRED ((uint32_t)1)


/*
   Checks if the string in the form "YYYY-MM-DD HH:mm:ss" is a valid
   date/time. The date must be in UTC time and "24-hour" format. If your
   date is in some other time format first convert it to UTC time before
   passing it into this function.

   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL, TA_E_INVALID_FLAGS, TA_E_INVALID_HANDLE
*/
TURBOACTIVATE_API HRESULT TA_CC TA_IsDateValid(uint32_t handle, STRCTYPE date_time, uint32_t flags);


/*
   Gets the version number of the currently used TurboActivate library.
   This is a useful alternative for platforms which don't support file meta-data
   (like Linux, FreeBSD, and other unix variants).

   The version format is:  Major.Minor.Build.Revision

   Returns: TA_OK on success. Handle all other return codes as failures.

   Possible return codes: TA_OK, TA_FAIL
*/
TURBOACTIVATE_API HRESULT TA_CC TA_GetVersion(uint32_t* MajorVersion, uint32_t* MinorVersion, uint32_t* BuildVersion, uint32_t* RevisionVersion);


/*
    General Success and Failure return codes.
*/
#define TA_OK ((HRESULT)0L)
#define TA_FAIL ((HRESULT)1L)
#define TA_E_PKEY ((HRESULT)0x00000002L)
#define TA_E_ACTIVATE ((HRESULT)0x00000003L)
#define TA_E_INET ((HRESULT)0x00000004L)
#define TA_E_INUSE ((HRESULT)0x00000005L)
#define TA_E_REVOKED ((HRESULT)0x00000006L)
#define TA_E_PDETS ((HRESULT)0x00000008L)
#define TA_E_TRIAL ((HRESULT)0x00000009L)
#define TA_E_TRIAL_EUSED ((HRESULT)0x0000000CL)
#define TA_E_EXPIRED ((HRESULT)0x0000000DL)
#define TA_E_TRIAL_EEXP ((HRESULT)0x0000000DL)
#define TA_E_COM ((HRESULT)0x0000000BL)
#define TA_E_INSUFFICIENT_BUFFER ((HRESULT)0x0000000EL)
#define TA_E_PERMISSION ((HRESULT)0x0000000FL)
#define TA_E_INVALID_FLAGS ((HRESULT)0x00000010L)
#define TA_E_IN_VM ((HRESULT)0x00000011L)
#define TA_E_EDATA_LONG ((HRESULT)0x00000012L)
#define TA_E_INVALID_ARGS ((HRESULT)0x00000013L)
#define TA_E_KEY_FOR_TURBOFLOAT ((HRESULT)0x00000014L)
#define TA_E_INET_DELAYED ((HRESULT)0x00000015L)
#define TA_E_FEATURES_CHANGED ((HRESULT)0x00000016L)
#define TA_E_ANDROID_NOT_INIT ((HRESULT)0x00000017L)
#define TA_E_NO_MORE_DEACTIVATIONS ((HRESULT)0x00000018L)
#define TA_E_ACCOUNT_CANCELED ((HRESULT)0x00000019L)
#define TA_E_ALREADY_ACTIVATED ((HRESULT)0x0000001AL)
#define TA_E_INVALID_HANDLE ((HRESULT)0x0000001BL)
#define TA_E_ENABLE_NETWORK_ADAPTERS ((HRESULT)0x0000001CL)
#define TA_E_ALREADY_VERIFIED_TRIAL ((HRESULT)0x0000001DL)
#define TA_E_TRIAL_EXPIRED ((HRESULT)0x0000001EL)
#define TA_E_MUST_SPECIFY_TRIAL_TYPE ((HRESULT)0x0000001FL)
#define TA_E_MUST_USE_TRIAL ((HRESULT)0x00000020L)
#define TA_E_NO_MORE_TRIALS_ALLOWED ((HRESULT)0x00000021L)
#define TA_E_BROKEN_WMI ((HRESULT)0x00000022L)
#define TA_E_INET_TIMEOUT ((HRESULT)0x00000023L)
#define TA_E_INET_TLS ((HRESULT)0x00000024L)


/* Possible callback statuses from the TrialCallbackType function: */
#define TA_CB_EXPIRED ((HRESULT)0x00000000UL)
#define TA_CB_EXPIRED_FRAUD ((HRESULT)0x00000001UL)

#endif