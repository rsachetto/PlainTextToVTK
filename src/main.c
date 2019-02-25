#include <stdio.h>

#define KGFLAGS_IMPLEMENTATION
#include "includes/kgflags.h"

#include "vtk_utils/vtk_unstructured_grid.h"
#include "string/sds.h"
#include "file_utils/file_utils.h"

int main(int argc, char **argv) {

    const char *input_dir = NULL;
    const char *output_dir = NULL;
    const char *file_prefix = NULL;
    bool dont_compress = false;
    bool plain = false;

    kgflags_string("input_dir", NULL, "Directory containing the text files to be converted.", true, &input_dir);
    kgflags_string("output_dir", NULL, "Directory to save the converted files. The default is input_dir/vtu", false, &output_dir);
    kgflags_string("input_prefix", NULL, "Name of the files to be converted without the extension.", true, &file_prefix);
    kgflags_bool("dont_compress", false, "Do not compress the output. The default is to compress", false, &dont_compress);
    kgflags_bool("plain_text", false, "Save the vtu data in plain text. The default is to save in binary", false, &plain);


    if (!kgflags_parse(argc, argv)) {
        kgflags_print_errors();
        kgflags_print_usage();
        return 1;
    }

    if(!dont_compress && plain) {
        printf("The plain_text flag was set to true but it will have no effect as the output will be compressed.\n");
    }

    if(output_dir == NULL) {
        sds out = sdsnew(input_dir);
        out = sdscat(out, "/vtu");
        output_dir = strdup(out);
    }

    bool animate_grid = true;

    create_dir(output_dir);

    convert_to_files_in_dir_to_vtu(input_dir, output_dir, file_prefix, animate_grid, !dont_compress, plain);

    return 0;
}