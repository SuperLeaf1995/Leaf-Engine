void _Cdecl fskip(FILE *stream, size_t n) {
    static size_t i;
    for(i = 0; i < (n+1); i++) {
        fgetc(stream); /*Skip palette*/
    }
    return;
}

uint16_t _Cdecl switchEndiannessUnsigned16(uint16_t num) {
    return (num<<8)|(num>>8);
}
int16_t _Cdecl switchEndiannessSigned16(int16_t num) {
    return (num<<8)|((num>>8)&0xFF);
}
uint32_t _Cdecl switchEndiannessUnsigned32(uint32_t num) {
    num = ((num<< 8)&0xFF00FF00)|((num>>8)&0xFF00FF);
    return (num<<16)|(num>>16);
}
int32_t _Cdecl switchEndiannessSigned32(int32_t num) {
    num = ((num<<8)&0xFF00FF00)|((num>>8)&0xFF00FF);
    return (num<<16)|((num>>16)&0xFFFF);
}
