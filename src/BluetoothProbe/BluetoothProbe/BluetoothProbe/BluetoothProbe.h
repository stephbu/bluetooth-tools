#pragma once

#include "stdafx.h"

void odwprintf(WCHAR* format, ...);
void odwprint(WCHAR* str);

void InitializeDeviceInfo(BLUETOOTH_DEVICE_INFO* pDeviceInfo);
void LogError();

void DumpRadios();
void DumpRadioInfo(HANDLE hRadio);
void DumpRadioDeviceInfo(HANDLE hRadio);
void DumpRadioDeviceServices(HANDLE hRadio, BLUETOOTH_DEVICE_INFO* pDeviceInfo);

