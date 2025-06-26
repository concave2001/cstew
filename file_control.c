#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>

#define BUFFER_SIZE (256)
#define BUFFER_STEP (7)

int include_cstew_headers(FILE *fptr)
{
    struct dirent *de;
    DIR *dr = opendir("./includes");
    if (!dr)
        return -1;

    while ((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, "..") != 0 && strcmp(de->d_name, ".") != 0) {
            fprintf(fptr, "#include \"%s\"\n", de->d_name);
        }
    }
    closedir(dr);

    return 0;
}

int write_cstew_func_calls(FILE* fptr)
{
    struct dirent *de;
    FILE* incl_fptr;

    char buffer[BUFFER_SIZE] = {0};
    int buff_i, file_i;
    int rc;

    DIR *dr = opendir("./includes");
    if (!dr)
        return 1;

    while ((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, "..") != 0 && strcmp(de->d_name, ".") != 0) {


            char* incl_path = calloc(BUFFER_SIZE, sizeof(char));
            if(!incl_path) {
                closedir(dr);
                return 5;
            }

            incl_fptr = fopen(strcat(strcat(incl_path, "./includes/"), de->d_name), "r");
            if (!incl_fptr) {
                closedir(dr);
                return 2;
            }
                
            buff_i = 0;
            file_i = 0;
            memset(buffer, 0, BUFFER_SIZE);

            while(fgets(buffer, BUFFER_STEP, incl_fptr) != NULL) {

                if (strncmp("cstew_", buffer, BUFFER_STEP - 1) == 0) {
                    buff_i = BUFFER_STEP - 1;

                    while(1) {
                        rc = fgetc(incl_fptr);
                        if (rc == EOF) {
                            fclose(incl_fptr);
                            closedir(dr);
                            return 3;
                        }

                        if ((char)rc == '(') {
                            buffer[buff_i++] = (char)rc;

                            while(1) {
                                rc = fgetc(incl_fptr);

                                if ((char)rc == ')') {
                                    buffer[buff_i++] = (char)rc;
                                    break;
                                } else {
                                    continue;
                                }
                            }

                            break;
                        }

                        buffer[buff_i++] = (char)rc;
                    }
                    
                    buffer[buff_i++] = ';';
                    printf("%s\n", buffer);

                    break;
                }

                fseek(incl_fptr, ++file_i, SEEK_SET);
            }

            fclose(incl_fptr);

            fprintf(fptr, "\t%s\n", buffer);
        }
    }
    closedir(dr);

    return 0;
}

int main(void)
{
    FILE *fptr = fopen("main.c", "w+");
    if (!fptr)
        return -1;

    fprintf(fptr, "#include <stdio.h>\n");
    fprintf(fptr, "#include <stdlib.h>\n");

    if(include_cstew_headers(fptr))
        return -1;

    fprintf(fptr, "\nint main(void)\n");
    fprintf(fptr, "{\n");

    if(write_cstew_func_calls(fptr))
        return -2;

    fprintf(fptr, "\n\treturn 0;\n");
    fprintf(fptr, "}\n");

    fclose(fptr);

    return 0;
}