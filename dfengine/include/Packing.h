//
//  Packing.h
//  Desktop Adventure
//
//  Copyright (c) 2015 chris. All rights reserved.
//

#ifndef Desktop_Adventure_Packing_h
#define Desktop_Adventure_Packing_h

#ifdef __cplusplus
extern "C" {
#endif

#define packfun(_type_) static inline bool _type_##_pack(char* out,_type_ in) { *(_type_*)out=in; return true; }
#define unpackfun(_type_) static inline bool _type_##_unpack(const char *in,_type_* out) { *out=*(_type_*)in; return true; }
#define readfun(_type_) static inline _type_ _type_##_read(const char* in) { return *(_type_*)in; }

    packfun(uint16_t) unpackfun(uint16_t) readfun(uint16_t)
    packfun(uint32_t) unpackfun(uint32_t) readfun(uint32_t)

#ifdef __cplusplus
}
#endif

#endif
