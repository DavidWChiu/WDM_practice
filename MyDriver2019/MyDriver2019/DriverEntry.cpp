#include<wdm.h>

extern "C"
NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    DbgPrint("Hello, Driver/n");

    return STATUS_SUCCESS;
}

void DriverUnload(
    PDRIVER_OBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    DbgPrint("Bye, Driver/n");
    return;
}