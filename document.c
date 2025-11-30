#include "document.h"
#include "config.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *size_t_to_string(size_t value) {
  char buffer[32];
  int needed = snprintf(buffer, sizeof(buffer), "%zu", value);
  if (needed < 0) {
    return NULL;
  }
  char *result = malloc((size_t)needed + 1);
  if (!result) {
    return NULL;
  }
  snprintf(result, needed + 1, "%zu", value);
  return result;
}
document_t *create_document(header_t *header, body_t *body) {
  if (!header) {
    header = create_default_header();
    header->type = RESPONSE;
    header->response_line = create_response_line(OK, VERSION);
  }
  document_t *document = malloc(sizeof(document_t));
  if (document == NULL) {
    return NULL;
  }
  document->header = header;
  if (!body) {
    document->body = NULL;
    return document;
  }
  document->body = body;
  attach_header(document->header,
                create_header_item("CONTENT-LENGTH",
                                   size_t_to_string(document->body->size)));
  return document;
}

unsigned char *serialize_document(document_t *document, size_t *size) {
  unsigned char *header = serialize_header(document->header);
  size_t header_len = strlen((char *)header);
  size_t total_len = header_len + 1; // +1 for '\0'
  if (document->body) {
    total_len += document->body->size;
  }
  unsigned char *output = malloc(total_len);
  if (!output)
    return NULL;
  memcpy(output, header, header_len);
  if (document->body)
    memcpy(output + header_len, document->body->data, document->body->size);
  *size = total_len;
  output[total_len - 1] = '\0';
  return output;
}

void destroy_document(document_t *document) {
  if (!document) {
    return;
  }
  if (document->header) {
    destroy_header(document->header);
  }
  if (document->body) {
    destroy_body(document->body);
  }
  free(document);
}
