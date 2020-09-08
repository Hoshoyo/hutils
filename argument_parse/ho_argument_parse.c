#include <stdlib.h>
#include "ho_argument_parse.h"

const uint64_t HO_ARG_SH_FLAG_L_A = (1ULL << 0);
const uint64_t HO_ARG_SH_FLAG_L_B = (1ULL << 1);
const uint64_t HO_ARG_SH_FLAG_L_C = (1ULL << 2);
const uint64_t HO_ARG_SH_FLAG_L_D = (1ULL << 3);
const uint64_t HO_ARG_SH_FLAG_L_E = (1ULL << 4);
const uint64_t HO_ARG_SH_FLAG_L_F = (1ULL << 5);
const uint64_t HO_ARG_SH_FLAG_L_G = (1ULL << 6);
const uint64_t HO_ARG_SH_FLAG_L_H = (1ULL << 7);
const uint64_t HO_ARG_SH_FLAG_L_I = (1ULL << 8);
const uint64_t HO_ARG_SH_FLAG_L_J = (1ULL << 9);
const uint64_t HO_ARG_SH_FLAG_L_K = (1ULL << 10);
const uint64_t HO_ARG_SH_FLAG_L_L = (1ULL << 11);
const uint64_t HO_ARG_SH_FLAG_L_M = (1ULL << 12);
const uint64_t HO_ARG_SH_FLAG_L_N = (1ULL << 13);
const uint64_t HO_ARG_SH_FLAG_L_O = (1ULL << 14);
const uint64_t HO_ARG_SH_FLAG_L_P = (1ULL << 15);
const uint64_t HO_ARG_SH_FLAG_L_Q = (1ULL << 16);
const uint64_t HO_ARG_SH_FLAG_L_R = (1ULL << 17);
const uint64_t HO_ARG_SH_FLAG_L_S = (1ULL << 18);
const uint64_t HO_ARG_SH_FLAG_L_T = (1ULL << 19);
const uint64_t HO_ARG_SH_FLAG_L_U = (1ULL << 20);
const uint64_t HO_ARG_SH_FLAG_L_V = (1ULL << 21);
const uint64_t HO_ARG_SH_FLAG_L_W = (1ULL << 22);
const uint64_t HO_ARG_SH_FLAG_L_X = (1ULL << 23);
const uint64_t HO_ARG_SH_FLAG_L_Y = (1ULL << 24);
const uint64_t HO_ARG_SH_FLAG_L_Z = (1ULL << 25);
const uint64_t HO_ARG_SH_FLAG_U_A = (1ULL << 26);
const uint64_t HO_ARG_SH_FLAG_U_B = (1ULL << 27);
const uint64_t HO_ARG_SH_FLAG_U_C = (1ULL << 28);
const uint64_t HO_ARG_SH_FLAG_U_D = (1ULL << 29);
const uint64_t HO_ARG_SH_FLAG_U_E = (1ULL << 30);
const uint64_t HO_ARG_SH_FLAG_U_F = (1ULL << 31);
const uint64_t HO_ARG_SH_FLAG_U_G = (1ULL << 32);
const uint64_t HO_ARG_SH_FLAG_U_H = (1ULL << 33);
const uint64_t HO_ARG_SH_FLAG_U_I = (1ULL << 34);
const uint64_t HO_ARG_SH_FLAG_U_J = (1ULL << 35);
const uint64_t HO_ARG_SH_FLAG_U_K = (1ULL << 36);
const uint64_t HO_ARG_SH_FLAG_U_L = (1ULL << 37);
const uint64_t HO_ARG_SH_FLAG_U_M = (1ULL << 38);
const uint64_t HO_ARG_SH_FLAG_U_N = (1ULL << 39);
const uint64_t HO_ARG_SH_FLAG_U_O = (1ULL << 40);
const uint64_t HO_ARG_SH_FLAG_U_P = (1ULL << 41);
const uint64_t HO_ARG_SH_FLAG_U_Q = (1ULL << 42);
const uint64_t HO_ARG_SH_FLAG_U_R = (1ULL << 43);
const uint64_t HO_ARG_SH_FLAG_U_S = (1ULL << 44);
const uint64_t HO_ARG_SH_FLAG_U_T = (1ULL << 45);
const uint64_t HO_ARG_SH_FLAG_U_U = (1ULL << 46);
const uint64_t HO_ARG_SH_FLAG_U_V = (1ULL << 47);
const uint64_t HO_ARG_SH_FLAG_U_W = (1ULL << 48);
const uint64_t HO_ARG_SH_FLAG_U_X = (1ULL << 49);
const uint64_t HO_ARG_SH_FLAG_U_Y = (1ULL << 50);
const uint64_t HO_ARG_SH_FLAG_U_Z = (1ULL << 51);

static int
char_to_bool_sh(char c)
{
    if(c >= 'a' && c <= 'z')
        return (c - 'a');
    else if(c >= 'A' && c <= 'Z')
        return (c - 'A') + ('z' - 'a');
    return -1;
}

static int
is_number(char c)
{
    return (c >= '0' && c <= '9');
}

static int
is_valid_shorthand(char c)
{
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z');
}

static int
is_flag_shorthand(Ho_Args* args, char c)
{
    return (args->registered_sh[char_to_bool_sh(c)] == HO_ARG_TYPE_FLAG);
}

static int
boolean_shorthand_get_value(Ho_Args* args, char c)
{
    int sh = char_to_bool_sh(c);
    return(args->boolean_shs & sh);
}

static void
set_boolean_sh(uint64_t* flags, char c, int value)
{
    if(value)
        *flags |= (1 << char_to_bool_sh(c));
    else
        *flags &= ~(1 << char_to_bool_sh(c));
}

static double
str_to_r64(char* text, int length)
{
	double result = 0.0;
	double tenths = 1.0;
	double frac_tenths = 0.1;
	int point_index = 0;

	while (text[point_index] != '.')
		++point_index;

	for (int i = point_index - 1; i >= 0; --i, tenths *= 10.0)
		result += (text[i] - 0x30) * tenths;
	for (int i = point_index + 1; i < length; ++i, frac_tenths *= 0.1)
		result += (text[i] - 0x30) * frac_tenths;
	return result;
}

static int64_t
str_to_s64(char* text, int length)
{
    int negative = 0;
    if(*text == '-') {
        text++;
        negative = 1;
    }
	int64_t result = 0;
	int64_t tenths = 1;
	for (int i = length - 1; i >= 0; --i, tenths *= 10)
		result += (text[i] - 0x30) * tenths;
	return result * ((negative) ? -1 : 1);
}

static int
parse_arg_data(char* str, Ho_Arg_Data* out_data)
{
    char* at = str;
    int numeric = 0;
    int found_float_dot = 0;

    if(is_number(*at) || (*at == '-' && is_number(at[1])))
    {
        numeric = 1;
        if(*at == '-') at++;
        while(is_number(*at))
        {
            at++;
            if(*at == '.')
            {
                if(!found_float_dot)
                {
                    found_float_dot = 1;
                    at++;
                }
                else
                {
                    numeric = 0;
                    break;
                }
            }
        }
        while(*at) at++;
    }
    else
    {
        while(*at) at++;
    }
    if(numeric)
    {
        if(found_float_dot)
        {
            out_data->type = HO_ARG_DATA_FLOAT;
            out_data->float_value = str_to_r64(str, at - str);
        }
        else
        {
            out_data->type = HO_ARG_DATA_INT;
            out_data->integer_value = str_to_s64(str, at - str);
        }
    }
    else
    {
        out_data->type = HO_ARG_DATA_STRING;
        out_data->string_value.data = str;
        out_data->string_value.length = at - str;
    }
    return HO_ARG_NO_ERROR;
}

static Ho_Arg_Error
parse_arg_next(Ho_Args* args, char* str, Ho_Argument* out_arg)
{
    if(*str == '-' && !is_number(str[1]))
    {
        str++;
        char* at = str;
        int in_bool_chain = 0;
        int negate_bool_chain = 0;
        uint64_t flags = 0;
        while(*at)
        {
            char s = *at;
            if(is_valid_shorthand(s) && is_flag_shorthand(args, s))
            {
                in_bool_chain = 1;
                set_boolean_sh(&flags, s, 1);
                out_arg->type = HO_ARG_TYPE_FLAG;
                at++;
                if(*at == '-' && at[1] == 0)
                {
                    negate_bool_chain = 1;
                    break;
                }
                continue;
            }
            else if(is_valid_shorthand(s))
            {
                if(in_bool_chain)
                {
                    // error, mismatch flag type, should be bool
                    return HO_ARG_ERROR_INVALID;
                }
                at++;
                if(*at != 0)
                {
                    // must be single character flag
                    return HO_ARG_ERROR_INVALID;
                }
                out_arg->type = args->registered_sh[char_to_bool_sh(s)];
                out_arg->shorthand = s;
            }
            else
            {
                // error, invalid shorthand
                return HO_ARG_ERROR_INVALID;
            }
        }
        if(in_bool_chain)
        {
            if(negate_bool_chain)
            {
                args->boolean_shs &= ~(flags);
            }
            else
                args->boolean_shs |= flags;
        }
    }
    else
    {
        // HO_ARG_TYPE_VALUE
        out_arg->type = HO_ARG_TYPE_VALUE;
        out_arg->data = calloc(1, sizeof(*out_arg->data));
        out_arg->data_count = 1;
        parse_arg_data(str, out_arg->data);
    }

    return HO_ARG_NO_ERROR;
}

Ho_Arg_Error
ho_args_register(Ho_Args* ctx, char c, Ho_Argument_Type type)
{
    if(!is_valid_shorthand(c)) return HO_ARG_ERROR_INVALID;
    if(ctx->registered_sh[char_to_bool_sh(c)] == HO_ARG_TYPE_NONE)
    {
        ctx->registered_sh[char_to_bool_sh(c)] = type;
        return HO_ARG_NO_ERROR;
    }
    return HO_ARG_ERROR_ALREADY_USED;
}

Ho_Arg_Error
ho_args_parse(Ho_Args* ctx, int argc, char** argv)
{
    int idx_values = 0;
    int idx_global = 0;

    for(int i = 0; i < argc; ++i)
    {
        Ho_Argument arg = {0};
        Ho_Arg_Error error = parse_arg_next(ctx, argv[i], &arg);
        if(error != HO_ARG_NO_ERROR) return error;
        arg.global_index = idx_global++;
        switch(arg.type)
        {
            case HO_ARG_TYPE_KEYED_VALUE:{
                i++;
                if(i < argc)
                {
                    arg.data = calloc(1, sizeof(*arg.data));
                    arg.data_count = 1;
                    error = parse_arg_data(argv[i], arg.data);
                    if(error != HO_ARG_NO_ERROR) return error;
                }
                else
                {
                    return HO_ERROR_TOO_FEW_ARGS;
                }
            } break;
            case HO_ARG_TYPE_LIST: {
                i++;
                int start = i;
                int lc = 0;
                for(; lc < argc - i; ++lc)
                {
                    if(*argv[i + lc] == '-' && !is_number(argv[i + lc][1]))
                        break;
                }
                i += (lc - 1);
                arg.data = calloc(lc, sizeof(*arg.data));
                arg.data_count = lc;
                for(int k = 0; k < lc; ++k)
                {
                    error = parse_arg_data(argv[start + k], arg.data + k);
                    if(error != HO_ARG_NO_ERROR) return error;
                }
            } break;
            case HO_ARG_TYPE_VALUE:{
                arg.type_index = idx_values++;
            } break;
            case HO_ARG_TYPE_NONE: {
                // here we parse a flag that is not registered.
                // maybe ignore or maybe warn, for now do nothing
            } break;
            default: break;
        }
        if(arg.type != HO_ARG_TYPE_FLAG)
        {
            if(ctx->argument_count >= (sizeof(ctx->arguments) / sizeof(*ctx->arguments)))
                return HO_ARG_ERROR_TOO_MANY_ARGS;
            ctx->arguments[ctx->argument_count++] = arg;
        }
    }
    return HO_ARG_NO_ERROR;
}

void
ho_args_free(Ho_Args* ctx)
{
    for(int i = 0; i < ctx->argument_count; ++i)
    {
        if(ctx->arguments[i].data_count > 0)
            free(ctx->arguments[i].data);
    }
}

int
ho_args_flag_status(Ho_Args* ctx, char c)
{
    if(!is_valid_shorthand(c) || !is_flag_shorthand(ctx, c))
        return -1;
    return boolean_shorthand_get_value(ctx, c);
}