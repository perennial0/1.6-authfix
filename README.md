# Counter-Strike 1.6 Authentication Fix

Fixes an issue encountered on Windows when trying to run the Steam version of Counter-Strike 1.6 subsequent to a Non-Steam one. More specifically, addresses two causes of the error "Failed to initialize authentication interface. Exiting...":
- incorrect PID of `steam.exe` in the registry entry `HKCU\Software\Valve\Steam\ActiveProcess\pid`;
- incorrect path of `steamclient.dll` in `HKCU\Software\Valve\Steam\ActiveProcess\SteamClientDll`.
