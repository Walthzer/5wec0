#include <xiic_l.h>
unsigned XIic_Recv(UINTPTR BaseAddress, u8 Address, u8 *BufferPtr,
                   unsigned ByteCount, u8 Option) {};
unsigned XIic_Send(UINTPTR BaseAddress, u8 Address, u8 *BufferPtr,
                   unsigned ByteCount, u8 Option) {};
unsigned XIic_DynRecv(UINTPTR BaseAddress, u8 Address, u8 *BufferPtr,
                      u8 ByteCount) {};
unsigned XIic_DynSend(UINTPTR BaseAddress, u16 Address, u8 *BufferPtr,
                      u8 ByteCount, u8 Option) {};
int XIic_DynInit(UINTPTR BaseAddress) {};
u32 XIic_CheckIsBusBusy(UINTPTR BaseAddress) {};
u32 XIic_WaitBusFree(UINTPTR BaseAddress) {};
