#ifndef _OBJECT_H__
#define _OBJECT_H__

typedef enum _object_type_t {
    OBJECT_TYPE_PLAYER = 0
} object_type_t;

typedef struct _object_t {
    object_type_t type;
} object_t;

#endif // _OBJECT_H__
