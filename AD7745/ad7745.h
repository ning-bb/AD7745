#ifndef __AD7745_H
#define __AD7745_H

#include "sys.h"

extern u8 CapResult_H,CapResult_M,CapResult_L;

#define  RDY  PBin(6)   //∂¡RDY ‰»Î

u8 AD7745_Reset(void);
void AD7745_Read_Cap(void);
u8 AD7745_ReadOneByte(u16 ReadAddr);
void AD7745_WriteOneByte(u16 WriteAddr,u8 DataToWrite);
void AD7745_Init(void);


#endif

