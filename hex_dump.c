#include <stdio.h>
#include <stdlib.h>
#define MAX_CHUNK 2
#define MAX_WIDTH 16
#define MAX_LINES 80
#define SPACE 32

int file_size(FILE* fp)
{
  int size;
  rewind(fp);
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp);
  rewind(fp);
  return size;
}

void hex_dump(FILE* fp)
{
  unsigned char x;
  int lines, size, chunk, width, c;
  lines = chunk = width = c = 0;
  size = file_size(fp);
  char* string = malloc(sizeof(char) * size);
  while(size--)
  {
    if(width == 0)
    {
      printf("%.8d\t", ftell(fp));
    }
    chunk++;
    width++;
    x = fgetc(fp);
    if(x < SPACE)
    {
      string[c++] = '.';
    }
    else
    {
      string[c++] = x;
    }
    printf("%.2x", x);
    if(chunk == MAX_CHUNK)
    {
      printf(" ");
      chunk = 0;
    }
    if(width == MAX_WIDTH)
    {
      string[c] = '\0';
      printf("\t%s", string);
      printf("\n");
      width = 0;
      free(string);
      string = calloc(size, sizeof(char));
      c = 0;
	  lines++;
	  if(lines > MAX_LINES)
	  {
		  printf("Press Enter to continue");
		  getchar();
		  lines = 0;
	  }
    }
  }
  for(int i = 0; i < MAX_WIDTH - width; i++)
  {
    printf("  ");
  }
  printf("\t\t%s", string);
  free(string);
  printf("\n\n");
}

void overwrite_offset(FILE* fp, int offset, unsigned char* data, int data_size)
{
  fseek(fp, offset, SEEK_SET);
  fwrite(data, data_size, 1, fp);
}

void no_overwrite_offset(FILE* fp, int offset, unsigned char* data, int data_size)
{
  int size;
  size = file_size(fp);
  unsigned char* buffer = malloc(sizeof(unsigned char)*size);
  fread(buffer, size, 1, fp);
  rewind(fp);
  if(offset == 0)
  {
    fwrite(data, data_size, 1, fp);
    fwrite((unsigned char*)buffer, size, 1, fp);
    return;
  }
  fwrite(buffer, offset - 1, 1, fp);
  fwrite(data, data_size, 1, fp);
  fwrite(buffer + (offset - 1), size - offset, 1, fp);
  free(buffer);
}

void make_jpg(FILE* fp)
{
  int size;
  unsigned char a = 0xFF;
  unsigned char b = 0xD8;
  size = file_size(fp);
  unsigned char* buffer = malloc(sizeof(unsigned char)*size*2);
  rewind(fp);
  fread(buffer, size, 1, fp);
  rewind(fp);
  fwrite(&a, sizeof(unsigned char), 1, fp);
  fwrite(&b, sizeof(unsigned char), 1, fp);
  fwrite(&a, sizeof(unsigned char), 1, fp);
  fwrite(&b, sizeof(unsigned char), 1, fp);
  fwrite(buffer, size, 1, fp);
  free(buffer);
}

char** string_to_array(char* string, char delimiter)
{
  int MAX_STRING;
  MAX_STRING = strlen(string);
  char* temp = calloc(MAX_STRING, sizeof(char));
  char ** return_array = calloc(MAX_STRING, sizeof(char*));
  int c, rc;
  c = rc = 0;
  while(*string)
  {
    if(*string == delimiter)
    {
      return_array[rc++] = temp;
      c = 0;
      free(temp);
      temp = calloc(MAX_STRING, sizeof(char));
    }
    else
    {
      temp[c++] = *string;
    }
    string++;
  }
  *(return_array + rc) = temp;
  return return_array;
}

void insert_main(FILE* fp)
{
	char offset[64];
	unsigned char data[200];
	printf("Offset byte location\n>> ");
	fgets(offset, 64, stdin);
	printf("Data to write\n>> ");
	fgets(data, 200, stdin);
	no_overwrite_offset(fp, atoi(offset), data, strlen(data) - 1);
}

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    return 1;
  }
  FILE* fp;
  fp = fopen(argv[1], "rb+");
  int go, size;
  char in[200];
  char** ret;
  char* data = calloc(200, sizeof(char));
  go = 1;
  while(go)
  {
    printf("\nd\t\t\t-Hex dump\no <offset> <data>\t-Overwrite with data starting at offset\ni <offset> <data>\t-Insert data starting at offset, no overwrite\nq\t\t\t-Quit\n>> ");
    fgets(in, 200, stdin);
    system("cls");
    if(in[0] == 'd')
    {
      hex_dump(fp);
    }
    else if(in[0] == 'o')
    {
      
      printf("overwrite");
    }
    else if(in[0] == 'i')
    {
      printf("insert\n");
      insert_main(fp);
    }
    else if(in[0] == 'q')
    {
      go = 0;
      break;
    }
  }
  fclose(fp);
  return 0;
}
