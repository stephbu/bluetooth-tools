// BluetoothProbe.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "BluetoothProbe.h"

void DumpRadios()
{
	BLUETOOTH_FIND_RADIO_PARAMS findParams;
	findParams.dwSize = sizeof(BLUETOOTH_FIND_RADIO_PARAMS);

	HANDLE hRadio;
	HBLUETOOTH_RADIO_FIND pFindRadio = BluetoothFindFirstRadio(&findParams, &hRadio);

	if (pFindRadio != NULL)
	{
		do
		{
			DumpRadioInfo(hRadio);
			DumpRadioDeviceInfo(hRadio);
		} 
		while (BluetoothFindNextRadio(pFindRadio, &hRadio));

		BluetoothFindRadioClose(pFindRadio);
	}
}

void DumpRadioInfo(HANDLE hRadio)
{
	BLUETOOTH_RADIO_INFO radioInfo;
	radioInfo.dwSize = sizeof(BLUETOOTH_RADIO_INFO);

	if (ERROR_SUCCESS == BluetoothGetRadioInfo(hRadio, &radioInfo))
	{
		odwprintf(L"Radio: %s\r\n", radioInfo.szName);
		odwprintf(L"Manufacturer: 0x%04X\r\n", radioInfo.manufacturer);
		odwprintf(L"Subversion: 0x%04X\r\n", radioInfo.lmpSubversion);
		odwprintf(L"Class: 0x%08X\r\n", radioInfo.ulClassofDevice);
		odwprintf(L"Address: %02X:%02X:%02X:%02X:%02X:%02X\r\n", radioInfo.address.rgBytes[5], radioInfo.address.rgBytes[4], radioInfo.address.rgBytes[3], radioInfo.address.rgBytes[2], radioInfo.address.rgBytes[1], radioInfo.address.rgBytes[0]);
		odwprintf(L"\r\n");
	}
	else
	{
		LogError();
	}
}

void LogError()
{
	odwprintf(L"Error: %d", GetLastError());
}

void DumpRadioDeviceInfo(HANDLE hRadio)
{
	BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams;
	searchParams.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
	searchParams.hRadio = hRadio;
	searchParams.fIssueInquiry = true;
	searchParams.cTimeoutMultiplier = 10;
	searchParams.fReturnAuthenticated = true;
	searchParams.fReturnConnected = true;
	searchParams.fReturnRemembered = true;
	searchParams.fReturnUnknown = true;

	BLUETOOTH_DEVICE_INFO deviceInfo;
	InitializeDeviceInfo(&deviceInfo);

	HBLUETOOTH_DEVICE_FIND hFindDevice = BluetoothFindFirstDevice(&searchParams, &deviceInfo);

	if (hFindDevice != NULL)
	{
		do
		{
			odwprintf(L"Device: %s\r\n", deviceInfo.szName);
			odwprintf(L"Address: %02X:%02X:%02X:%02X:%02X:%02X\r\n", deviceInfo.Address.rgBytes[5], deviceInfo.Address.rgBytes[4], deviceInfo.Address.rgBytes[3], deviceInfo.Address.rgBytes[2], deviceInfo.Address.rgBytes[1], deviceInfo.Address.rgBytes[0]);
			odwprintf(L"Class: 0x%08X\r\n", deviceInfo.ulClassofDevice);
			odwprintf(L"Connected: %s\r\n", deviceInfo.fConnected ? L"True" : L"False");
			odwprintf(L"Authenticated: %s\r\n", deviceInfo.fAuthenticated ? L"True" : L"False");
			odwprintf(L"Remembered: %s\r\n", deviceInfo.fRemembered ? L"True" : L"False");
			DumpRadioDeviceServices(hRadio, &deviceInfo);
			odwprintf(L"\r\n");
			InitializeDeviceInfo(&deviceInfo);
		} 
		while (BluetoothFindNextDevice(hFindDevice, &deviceInfo));

		BluetoothFindDeviceClose(hFindDevice);
	}
	else
	{
		LogError();
	}
}

void DumpRadioDeviceServices(HANDLE hRadio, BLUETOOTH_DEVICE_INFO* pDeviceInfo)
{
	const int bufferSize = 100;
	DWORD serviceCount = bufferSize;
	UUID serviceBuffer[bufferSize];

	ZeroMemory(&serviceBuffer, sizeof(UUID) * bufferSize);
	DWORD state = BluetoothEnumerateInstalledServices(hRadio, pDeviceInfo, &serviceCount, serviceBuffer);

	switch (state)
	{
	case ERROR_SUCCESS:
	case ERROR_MORE_DATA:
	{
		for (unsigned int index = 0; index < serviceCount; index++)
		{
			odwprintf(L"Service(%d) {%08lX-%04hX-%04hX-%02X%02X-%02X%02X%02X%02X%02X%02X} ", index + 1, serviceBuffer[index].Data1, serviceBuffer[index].Data2, serviceBuffer[index].Data3,
				serviceBuffer[index].Data4[0], serviceBuffer[index].Data4[1], serviceBuffer[index].Data4[2], serviceBuffer[index].Data4[3],
				serviceBuffer[index].Data4[4], serviceBuffer[index].Data4[5], serviceBuffer[index].Data4[6], serviceBuffer[index].Data4[7]);

			switch (serviceBuffer[index].Data1)
			{

			case 0x1000:
			{
				odwprint(L"ServiceDiscoveryServerServiceClassID\r\n");
				break;
			}

			case 0x1101:
			{
				odwprint(L"SerialPort\r\n");
				break;
			}
			
			case 0x110A:
			{
				odwprint(L"AudioSource\r\n");
				break;
			}
			
			case 0x110B:
			{
				odwprint(L"AudioSink\r\n");
				break;
			}
			
			case 0x110C:
			{
				odwprint(L"A/V_RemoteControlTarget\r\n");
				break;
			}
			
			case 0x110E:
			{
				odwprint(L"A/V_RemoteControl\r\n");
				break;
			}
			
			case 0x111E:
			{
				odwprint(L"Handsfree\r\n");
				break;
			}
			
			case 0x1124:
			{
				odwprint(L"HumanInterfaceDeviceService\r\n");
				break;
			}	

			case 0x1200:
			{
				odwprint(L"PnPInformation\r\n");
				break;
			}
			
			default:
			{
				odwprint(L"Unknown\r\n");
				break;
			}
			}

		}


		break;
	}
	default:
	{
		odwprintf(L"Error:", GetLastError());
	}
	}
}

void InitializeDeviceInfo(BLUETOOTH_DEVICE_INFO* pDeviceInfo)
{
	ZeroMemory(pDeviceInfo, sizeof(BLUETOOTH_DEVICE_INFO));
	pDeviceInfo->dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
}

void odwprint(WCHAR* str)
{
	OutputDebugString(str);
}

void odwprintf(WCHAR* format, ...)
{
	va_list argList;

	va_start(argList, format);
	WCHAR msgBuf[1024];
	ZeroMemory(msgBuf, sizeof(WCHAR) * 1024);
	wvsprintf(msgBuf, format, argList);
	wprintf_s(msgBuf);
	OutputDebugString(msgBuf);

	va_end(argList);
}