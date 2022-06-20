#include <fstream>  // wifstream
#include <iostream> // wcerr; wcout
#include <string>   // wstring

#define WIN32_LEAN_AND_MEAN

#include <TlHelp32.h>
#include <Windows.h>

#include <fcntl.h>
#include <io.h>

DWORD get_pid(const std::wstring &proc_name)
{
  auto proc_snap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (proc_snap != INVALID_HANDLE_VALUE) {
    PROCESSENTRY32W pe32{.dwSize = sizeof(PROCESSENTRY32W)};
    if (::Process32FirstW(proc_snap, &pe32)) {
      do {
        if (!::wcsncmp(pe32.szExeFile, proc_name.c_str(), proc_name.size())) {
          return pe32.th32ProcessID;
        }
      } while (::Process32NextW(proc_snap, &pe32));
    }
  }
  return 0;
}

int main()
{
  [[maybe_unused]] const auto setmode_ret = _setmode(_fileno(stdout), _O_U16TEXT);

  HKEY key = nullptr;
  if (
    ::RegCreateKeyExW(
      HKEY_CURRENT_USER, L"Software\\Valve\\Steam\\ActiveProcess", 0, nullptr,
      REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, nullptr, &key, nullptr
    ) != ERROR_SUCCESS
  ) {
    std::wcerr << L"RegCreateKeyExW() failed. Error code: " << ::GetLastError();
    return -1;
  }

  DWORD pid = 0;
  std::wcout << L"Searching for \"steam.exe\".\n";
  while (!(pid = get_pid(L"steam.exe")));
  std::wcout << L"Found \"steam.exe\" (PID: " << pid << ").\n";

  std::wcout << L"\nHKCU\\Software\\Valve\\Steam\\ActiveProcess\\pid = " << pid << '\n';
  if (
    ::RegSetValueExW(
      key, L"pid", 0, REG_DWORD, reinterpret_cast<const BYTE *>(&pid), 4
    ) != ERROR_SUCCESS
  ) {
    std::wcerr << L"RegSetValueExW() failed. Error code: " << ::GetLastError();
    return -1;
  }

  std::wifstream settings("settings.ini");
  std::wstring steamclientdll_path;
  std::getline(settings, steamclientdll_path);
  steamclientdll_path = steamclientdll_path.substr(steamclientdll_path.find('=') + 1) +
    L"\\steamclient.dll";

  std::wcout
    << L"HKCU\\Software\\Valve\\Steam\\ActiveProcess\\SteamClientDll = "
    << steamclientdll_path << '\n';
  if (
    ::RegSetValueExW(
      key, L"SteamClientDll", 0, REG_SZ,
      reinterpret_cast<const BYTE *>(steamclientdll_path.c_str()),
      steamclientdll_path.size() * 2 + 1
    ) != ERROR_SUCCESS
  ) {
    std::wcerr << L"RegSetValueExW() failed. Error code: " << ::GetLastError();
    return -1;
  }

  /* Possibly unnecessary as I've yet to notice a version of Non-Steam CS set
   * this. */
  steamclientdll_path = steamclientdll_path.substr(0, steamclientdll_path.size() - 4) + L"64.dll";

  std::wcout
    << L"HKCU\\Software\\Valve\\Steam\\ActiveProcess\\SteamClientDll64 = "
    << steamclientdll_path << '\n';
  if (
    ::RegSetValueExW(
      key, L"SteamClientDll64", 0, REG_SZ,
      reinterpret_cast<const BYTE *>(steamclientdll_path.c_str()),
      steamclientdll_path.size() * 2 + 1
    ) != ERROR_SUCCESS
  ) {
    std::wcerr << L"RegSetValueExW() failed. Error code: " << ::GetLastError();
    return -1;
  }

  std::wcout << L"\nFinished. ";

  std::wstring tmp;
  std::getline(settings, tmp);
  if (std::stoi(tmp.substr(tmp.find('=') + 1))) {
    std::wcout << L"Press ENTER to exit...";
    std::wcin.get();
  }

  return 0;
}
