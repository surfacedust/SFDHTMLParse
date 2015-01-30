#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "gumbo.h"

static void read_file(FILE* fp, char** output, int* length) {
  struct stat filestats;
  int fd = fileno(fp);
  fstat(fd, &filestats);
  *length = filestats.st_size;
  *output = (char*)malloc(*length + 1);
  int start = 0;
  int bytes_read;
  while ((bytes_read = fread(*output + start, 1, *length - start, fp))) {
    start += bytes_read;
  }
}

static void print_meta(const GumboNode* root)
{
    assert(root->type == GUMBO_NODE_ELEMENT);
    const GumboVector* root_children = &root->v.element.children;
    GumboNode* head = NULL;
    for (unsigned int i = 0; i < root_children->length; ++i) {
        GumboNode *child = (GumboNode*) root_children->data[i];
        if (child->type == GUMBO_NODE_ELEMENT &&
            child->v.element.tag == GUMBO_TAG_HEAD) {
            head = child;
            break;
        }
    }
    assert(head != NULL);

    GumboVector* head_children = &head->v.element.children;
    for (unsigned int i = 0; i < head_children->length; ++i) {
        GumboNode *child = (GumboNode*)head_children->data[i];
        if (child->type == GUMBO_NODE_ELEMENT &&
            child->v.element.tag == GUMBO_TAG_META) {
            printf("---GumboAttribute---\n");
            for (unsigned int j = 0; j < child->v.element.attributes.length; ++j) {
                GumboAttribute *attr = (GumboAttribute*)child->v.element.attributes.data[j];
                printf("**%s : %s**\n", attr->name, attr->value);
            }
            printf("---children---\n");
            for (unsigned int j = 0; j < child->v.element.children.length; ++j) {
                GumboNode *c = (GumboNode*)child->v.element.children.data[j];
                printf("---%d---\n", c->type);
            }
            printf("--end--\n");
        }
    }
}

int main(int argc, char** argv) {

  if (argc != 2) {
    printf("Usage: need <html filename>.\n");
    exit(EXIT_FAILURE);
  }
  const char* filename = argv[1];

  FILE* fp = fopen(filename, "r");
  if (!fp) {
    printf("File %s not found!\n", filename);
    exit(EXIT_FAILURE);
  }

  char* input;
  int input_length;
  read_file(fp, &input, &input_length);
  GumboOutput* output = gumbo_parse_with_options(
      &kGumboDefaultOptions, input, input_length);
  print_meta(output->root);
  gumbo_destroy_output(&kGumboDefaultOptions, output);
  free(input);
}
