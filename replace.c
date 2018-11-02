#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
//TODO treat case where expression is between two read buffers

long int replace(char **dest, char buf[BUFFER_SIZE], char *old_string, char *new_string)
{
    size_t old_len = strlen(old_string);
    size_t new_len = strlen(new_string);
    size_t buf_len = strlen(buf);
    long int diff_len = new_len - old_len;

    char *occurence = buf;
    long int total_diff_len = 0;
    int count = 0;

    while((occurence = strstr(occurence,old_string)) != NULL)
    {
        count++;
        occurence += old_len;
    }
    total_diff_len = count * diff_len;
    *dest = malloc(sizeof(char) * (buf_len + total_diff_len) + 1);
    if(*dest == NULL)
    {
        printf("malloc failed\n");
    }
    (*dest)[sizeof(char) * (buf_len + total_diff_len)] = '\0';

    char *dest_pnt = *dest;
    char *src_pnt = buf;
    occurence = buf;
    while((occurence = strstr(occurence,old_string)) != NULL)
    {
        int start_index = occurence - src_pnt;
        strncpy(dest_pnt, src_pnt, start_index);
        strncpy(dest_pnt + start_index, new_string, new_len);
        dest_pnt = dest_pnt + start_index + new_len;
        src_pnt = src_pnt + start_index + old_len;

        occurence += old_len;
    }
    strncpy(dest_pnt, src_pnt, strlen(src_pnt));
    if((*dest)[sizeof(char) * (buf_len + total_diff_len)] != '\0')
    {
        printf("buffer integrity check 2 failed\n");
    }
    return total_diff_len;
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

    fseek (file, 0, SEEK_END);
    long int orig_size=ftell (file);
    rewind(file);

    char buf[BUFFER_SIZE + 1]; buf[BUFFER_SIZE] = '\0';
    long int write_index, read_index = 0;
    size_t nread = 1;
    long int offset = 0;
    while(orig_size + offset != read_index)
    {
        memset(buf,0,BUFFER_SIZE);
        nread = fread(buf, 1, BUFFER_SIZE, file);
        if(ferror(file))
        {
            perror("error occurred while reading file: ");
            return 1;
        }
        if(nread == 0)
        {
            break;
        }
        read_index = ftell(file);
        fseek(file, write_index, SEEK_SET);

        char *new_str = NULL;
        offset += replace(&new_str, buf, argv[2], argv[3]);

        read_index += offset;
        fwrite(new_str, 1, nread + offset, file);
        free(new_str);
        if(ferror(file))
        {
            perror("error occurred while writing file: ");
            return 1;
        }
        write_index = ftell(file);
        fseek(file, read_index, SEEK_SET);
    }
    if(buf[BUFFER_SIZE] != '\0')
    {
        printf("buffer integrity check 1 failed\n");
    }
    fclose(file);
    if(offset < 0)
    {
        if (truncate(argv[1], orig_size + offset) != 0)
        {
            printf("failed to truncate\n");
        }
    }
    return 0;
}
