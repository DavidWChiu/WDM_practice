#include<wdm.h>

PVOID pThreadHandle;

KEVENT EventCloseThread;

void ThreadFunc(PVOID StartContext) 
{
    UNREFERENCED_PARAMETER(StartContext);
    DbgPrint("Hello Kernel Thread!\n");
    DbgPrint("Kernel Thread starts working!\n");
    LARGE_INTEGER wait;
    wait.QuadPart = -10 * 10000000; // 10 seconds
    while (true) {
        if (KeWaitForSingleObject(&EventCloseThread, Executive, KernelMode, FALSE, &wait) == STATUS_SUCCESS)
            break;
        DbgPrint("Kernel Thread is working!\n");
    }
    // DbgBreakPoint();
    DbgPrint("Bye Kerenl Thread!\n");
    PsTerminateSystemThread(STATUS_SUCCESS);
}

EXTERN_C_START

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD DriverUnload;
DRIVER_STARTIO DriverStartIo;
IO_WORKITEM_ROUTINE IoWorkitemRoutine;

PDEVICE_OBJECT g_pDeviceObject = NULL;

NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_SUCCESS;

    DriverObject->DriverUnload = DriverUnload;

    DbgPrint("Hello, Driver!\n");

    status = IoCreateDevice(
        DriverObject,
        0,
        NULL,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &g_pDeviceObject);
    if(!NT_SUCCESS(status))
        return status;

    DbgPrint("Hi, Device!\n");
   

    //Practice of creating WorkItem
    PIO_WORKITEM workItem = NULL;
    if (NT_SUCCESS(status)) {
        workItem = IoAllocateWorkItem(g_pDeviceObject);
        if (workItem != NULL) {
            IoInitializeWorkItem(g_pDeviceObject, workItem);
            IoQueueWorkItem(workItem, IoWorkitemRoutine, DelayedWorkQueue, workItem);
        }
        else {
            DbgPrint("Create WorkItem Fail!\n");
        }
    }
    else {
        DbgPrint("Create DeviceObject Fail!\n");
    }

    // Practice of creating System Thread.
    KeInitializeEvent(&EventCloseThread, NotificationEvent, FALSE);
    HANDLE hThreadHandle = NULL;
    status = PsCreateSystemThread(&hThreadHandle, GENERIC_ALL, NULL, NULL, NULL, ThreadFunc, NULL);
    if (status == STATUS_SUCCESS) {
        ObReferenceObjectByHandle(hThreadHandle, GENERIC_ALL, NULL, KernelMode, &pThreadHandle, NULL);
        ZwClose(hThreadHandle);
    }

    return status;
}

void 
DriverUnload(
    PDRIVER_OBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);
    
    KeSetEvent(&EventCloseThread, IO_NO_INCREMENT, FALSE);
    if (pThreadHandle != NULL) {
        KeWaitForSingleObject(pThreadHandle, Executive, KernelMode, FALSE, NULL);
        ObDereferenceObject(pThreadHandle);
        DbgPrint("Free Thread Handle!\n");
    }

    if (g_pDeviceObject) {
        IoDeleteDevice(g_pDeviceObject);
        DbgPrint("Bye, Device!\n");
    }

    DbgPrint("Bye, Driver!\n");
    return;
}

void DriverStartIo(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);
    
    return;
}

void IoWorkitemRoutine(
    PDEVICE_OBJECT DeviceObject,
    PVOID Context
)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Context);

    DbgPrint("Hello WorkItem!\n");
    if (Context != NULL) {
        IoUninitializeWorkItem((PIO_WORKITEM)Context);
        IoFreeWorkItem((PIO_WORKITEM)Context);
        DbgPrint("Free WorkItem!\n");
    }
    DbgPrint("WorkItem Routine Done\n");
    return;
}

EXTERN_C_END