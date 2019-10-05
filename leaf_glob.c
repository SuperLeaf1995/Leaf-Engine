void _Cdecl fskip(FILE *stream, size_t n) {
    static size_t i;
    for(i = 0; i < (n+1); i++) {
        fgetc(stream); /*Skip palette*/
    }
    return;
}

int32_t _Cdecl switchEndianness32(int32_t end) {
    return (end<<24)|((end<<8)&0x00ff0000)|((end>>8)&0x0000ff00)|((end>>24)&0x000000ff);
}
