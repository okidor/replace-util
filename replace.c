#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE 4096

void replace(char buf[BUFFER_SIZE], char *old_string, char *new_string)
{
    char *occurence = strstr(buf,old_string);
    if(occurence != NULL)
    {
        if(strlen(old_string) == strlen(new_string))
        {
            strncpy(occurence, new_string, strlen(new_string));
        }
        else
        {
            printf("replacement of different strings not yet supported\n");
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        printf("usage: %s file oldstring newstring\n",argv[0]);
        return 0;
    }
    FILE *file = fopen(argv[1],"r+");

    if(file == NULL)
    {
        printf("error opening %s\n",argv[1]);
        return 1;
    }

    char buf[BUFFER_SIZE];
    long int write_index, read_index = 0;
    size_t nread = 1;
    while(nread > 0)
    {
        nread = fread(buf, 1, BUFFER_SIZE, file);
        if(ferror(file))
        {
            perror("error occurred while reading file: ");
            return 1;
        }
        read_index = ftell(file);
        fseek(file, write_index, SEEK_SET);

        replace(buf,argv[2],argv[3]);

        fwrite(buf, 1, nread, file);
        if(ferror(file))
        {
            perror("error occurred while writing file: ");
            return 1;
        }
        write_index = ftell(file);
        fseek(file, read_index, SEEK_SET);
    }
    fclose(file);
    return 0;
}
