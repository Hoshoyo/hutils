#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../ho_argument_parse.h"

int main(int argc, char** argv)
{
    Ho_Args args = {0};

    // file list
    ho_args_register(&args, 'f', HO_ARG_TYPE_LIST);
    ho_args_register(&args, 'a', HO_ARG_TYPE_FLAG);
    ho_args_register(&args, 'b', HO_ARG_TYPE_FLAG);

    Ho_Arg_Error error = ho_args_parse(&args, argc, argv);
    if (error != HO_ARG_NO_ERROR)
    {
        fprintf(stderr, "could not parse arguments, error: %d\n", error);
        return -1;
    }

    printf("flags set: ");
    for (int i = 0; i < 64; ++i)
    {
        if (args.boolean_shs & (1ULL << i))
        {
            printf("-%c ", i + 'a');
        }
    }

    ho_args_free(&args);

    return 0;
}