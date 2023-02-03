#include<wdm.h>

extern "C" DRIVER_INITIALIZE DriverEntry;
extern "C" DRIVER_UNLOAD DriverUnload;

static HANDLE ThreadHandle;

static KEVENT EventDone;

void ThreadFunc(PVOID StartContext) 
{
    UNREFERENCED_PARAMETER(StartContext);
    DbgPrint("Hello Kernel Thread!\n");
    KeSetEvent(&EventDone, IO_NO_INCREMENT, FALSE);
    DbgPrint("Bye Kerenl Thread!\n");
    PsTerminateSystemThread(STATUS_SUCCESS);
}

extern "C"
NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->DriverUnload = DriverUnload;

    DbgPrint("Hello, Driver!\n");

    KeInitializeEvent(&EventDone, NotificationEvent, FALSE);

    PsCreateSystemThread(&ThreadHandle, GENERIC_ALL, NULL, NULL, NULL, ThreadFunc, NULL);

    KeWaitForSingleObject(&EventDone, Executive, KernelMode, FALSE, NULL);

    ZwClose(&ThreadHandle);

    return STATUS_SUCCESS;
}

extern "C" 
void 
DriverUnload(
    PDRIVER_OBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    DbgPrint("Bye, Driver!\n");
    return;
}