#pragma once
#include <stdint.h>

typedef enum {
    HO_ARG_TYPE_NONE = 0,
    HO_ARG_TYPE_VALUE,
    HO_ARG_TYPE_FLAG,
    HO_ARG_TYPE_KEYED_VALUE,
    HO_ARG_TYPE_LIST,
} Ho_Argument_Type;

typedef enum {
    HO_ARG_DATA_NONE = 0,
    HO_ARG_DATA_STRING,
    HO_ARG_DATA_INT,
    HO_ARG_DATA_FLOAT,
    HO_ARG_DATA_BOOL,
} Ho_Argument_Data_Type;

typedef enum {
    HO_ARG_NO_ERROR = 0,
    HO_ARG_ERROR_INVALID = -1,
    HO_ARG_ERROR_ALREADY_USED = -2,
    HO_ARG_ERROR_TOO_MANY_ARGS = -3,
    HO_ERROR_TOO_FEW_ARGS = -4,
} Ho_Arg_Error;

// Types
typedef struct {
    int   length;
    char* data;
} THo_Arg_String;

typedef struct {
    Ho_Argument_Data_Type type;
    union {
        int64_t        integer_value;
        double         float_value;
        THo_Arg_String string_value;
    };
} Ho_Arg_Data;

typedef struct {
    Ho_Argument_Type type;
    int              type_index;
    int              global_index;
    char             shorthand;
    int              data_count;
    Ho_Arg_Data*     data;
} Ho_Argument;

typedef struct {
    // exposed
    int              argument_count;
    Ho_Argument      arguments[256];

    // internal use
    uint64_t         boolean_shs;
    Ho_Argument_Type registered_sh[26 * 2];
} Ho_Args;

Ho_Arg_Error ho_args_register(Ho_Args* ctx, char c, Ho_Argument_Type type);
Ho_Arg_Error ho_args_parse(Ho_Args* ctx, int argc, char** argv);
int          ho_args_flag_status(Ho_Args* ctx, char c);
void         ho_args_free(Ho_Args* ctx);