#include <initp/platform/net.hpp>

#include <winsock2.h>
#include <iphlpapi.h>
#include <windows.h>

namespace initp {
namespace system {
namespace tools {

#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#define INFO_BUFFER_SIZE 32767

std::string get_host_name(void) {
    TCHAR infoBuf[INFO_BUFFER_SIZE];
    DWORD bufCharCount = INFO_BUFFER_SIZE;

    if (!GetComputerName(infoBuf, &bufCharCount))
        return std::string();
    return std::string(infoBuf);
}

std::string get_user_name(void) {
    TCHAR infoBuf[INFO_BUFFER_SIZE];
    DWORD bufCharCount = INFO_BUFFER_SIZE;

    if (!GetUserName(infoBuf, &bufCharCount))
        return std::string();
    return std::string(infoBuf);
}

inline size_t get_interface_index(const std::string name) {

    size_t result = 0;
    DWORD dwRetVal = 0;
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX;
    ULONG family = AF_INET;
    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = WORKING_BUFFER_SIZE;
    ULONG Iterations = 0;
    PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;

    do {

        pAddresses = (IP_ADAPTER_ADDRESSES*) MALLOC(outBufLen);
        if (pAddresses == NULL) {
            return result;
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);
        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            FREE(pAddresses);
            pAddresses = NULL;
        } else {
            break;
        }

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
        pCurrAddresses = pAddresses;
        while (pCurrAddresses) {
            std::wstring ws(pCurrAddresses->FriendlyName);
            std::string s(ws.begin(), ws.end());
            if (s == name) {
                result = pCurrAddresses->IfIndex;
                break;
            }
            pCurrAddresses = pCurrAddresses->Next;
        }
    } else {
        return 0;
    }

    if (pAddresses) {
        FREE(pAddresses);
    }
    return result;
}

std::string get_ip_address(const std::string& name) {

    std::string result("0.0.0.0");
    size_t index = get_interface_index(name);

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    IN_ADDR IPAddr;
    PMIB_IPADDRTABLE pIPAddrTable = (MIB_IPADDRTABLE*)MALLOC(sizeof(MIB_IPADDRTABLE));

    if (pIPAddrTable) {
        if (GetIpAddrTable(pIPAddrTable, &dwSize, 0) == ERROR_INSUFFICIENT_BUFFER) {
            FREE(pIPAddrTable);
            pIPAddrTable = (MIB_IPADDRTABLE *) MALLOC(dwSize);
        }
        if (pIPAddrTable == NULL) {
            return result;
        }
    }

    if ((dwRetVal = GetIpAddrTable(pIPAddrTable, &dwSize, 0 )) != NO_ERROR) {
        return result;
    }

    for (int i = 0; i < (int)pIPAddrTable->dwNumEntries; i++) {
        if (pIPAddrTable->table[i].dwIndex == index) {
            IPAddr.S_un.S_addr = (u_long) pIPAddrTable->table[i].dwAddr;
            result = std::string(inet_ntoa(IPAddr));
        }
    }

    if (pIPAddrTable) {
        FREE(pIPAddrTable);
        pIPAddrTable = NULL;
    }
    return result;
}

}}}
