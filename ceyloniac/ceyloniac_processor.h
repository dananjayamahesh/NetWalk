#ifndef CEYLONIAC_PROCESSOR_H_INCLUDED
#define CEYLONIAC_PROCESSOR_H_INCLUDED

typedef enum isa{

    DEQUE=0x0001,
    ENQUE=0x0002,
    DROP =0x0003,

    LOAD =0x0004,
    STORE=0x0005,
    LDILW=0x0006,
    LDIHG=0x0007,
    RBITS=0x0008,
    WBITS=0x0009,
    CMP  =0x000a,
    BR   =0x000b,
    BREQ =0x000c,
    BRL  =0x000d,
    BREQL=0x000e,
    NOP  =0x000f,

     //0x0010 reserved
    ADD  =0x0011,
    SUB  =0x0012,
    AND  =0x0013,
    OR   =0x0014,
    XOR  =0x0015,
    NOT  =0x0016,
    SHL  =0x0017,
    INC  =0x0018,
    DEC  =0x0019,

    //0x0020 reserved
    TABLE=0x0021,
    TLKUP=0x0022,

    CRC  =0x0023,
    CHKSM=0x0024

}ISA;

typedef enum reg{
    R0=0x0,
    R1=0x1,
    R2=0x2,
    R3=0x3,
    R4=0x4,
    R5=0x5,
    R6=0x6,
    R7=0x7,
    R8=0x8,
    R9=0x9,
    R10=0xa,
    R11=0xb,
    R12=0xc,
    R13=0xd,
    R14=0xe,
    R15=0xf

}R;

void isa_converter(char *isa_str,ISA *isa);
void isa_converter(char *isa_str,ISA *isa){


}


#endif // CEYLONIAC_PROCESSOR_H_INCLUDED
