/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_MyKMDF,
    0x3f18cac5,0xdecb,0x4d80,0xa4,0x64,0xe7,0x33,0xde,0x3a,0xc6,0xed);
// {3f18cac5-decb-4d80-a464-e733de3ac6ed}
