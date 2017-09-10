#include <string.h>
#include <stdio.h>

#include "hoedown/html.h"
#include "hoedown/document.h"
#include "hoedown/buffer.h"

#include "marker-utils.h"
#include "marker-markdown.h"

char*
marker_markdown_render(char*  markdown,
                       size_t size)
{
  hoedown_renderer* renderer;
  hoedown_document* document;
  hoedown_buffer* buffer;
  
  renderer = hoedown_html_renderer_new(0,0);
  document = hoedown_document_new(renderer,
                                  HOEDOWN_EXT_BLOCK |
                                  HOEDOWN_EXT_SPAN  |
                                  HOEDOWN_EXT_FLAGS,
                                  16);
  buffer = hoedown_buffer_new(500);
  
  hoedown_buffer_puts(buffer,
                      "<html>\n"
                      "<head>\n"
                      "</head>\n"
                      "<body>");
  hoedown_document_render(document, buffer, markdown, size);
  hoedown_buffer_puts(buffer,
                      "</body>\n"
                      "</html>");
  
  const char* buf_cstr = hoedown_buffer_cstr(buffer);
  size_t buf_cstr_len = strlen(buf_cstr) + 1;
  
  char* html = NULL;
  html = malloc(buf_cstr_len);
  memcpy(html, buf_cstr, buf_cstr_len);
  
  hoedown_html_renderer_free(renderer);
  hoedown_document_free(document);
  hoedown_buffer_free(buffer);
  
  return html;
}

char*
marker_markdown_render_with_css(char*  markdown,
                                size_t size,
                                char*  href)
{
  hoedown_renderer* renderer;
  hoedown_document* document;
  hoedown_buffer* buffer;
  
  renderer = hoedown_html_renderer_new(0,0);
  document = hoedown_document_new(renderer,
                                  HOEDOWN_EXT_BLOCK |
                                  HOEDOWN_EXT_SPAN  |
                                  HOEDOWN_EXT_FLAGS,
                                  16);
  buffer = hoedown_buffer_new(500);
  
  hoedown_buffer_printf(buffer,
                      "<html>\n"
                      "<head>\n"
                      "<link rel=\"stylesheet\" href=\"%s\" type=\"text/css\"/>\n"
                      "</head>\n"
                      "<body>\n",
                      href);
  hoedown_document_render(document, buffer, markdown, size);
  hoedown_buffer_puts(buffer,
                      "</body>\n"
                      "</html>");
  
  const char* buf_cstr = hoedown_buffer_cstr(buffer);
  size_t buf_cstr_len = strlen(buf_cstr) + 1;
  
  char* html = NULL;
  html = malloc(buf_cstr_len);
  memcpy(html, buf_cstr, buf_cstr_len);
  
  hoedown_html_renderer_free(renderer);
  hoedown_document_free(document);
  hoedown_buffer_free(buffer);
  
  return html;
}

void
marker_markdown_render_to_file(char*  markdown,
                               size_t size,
                               char*  filepath)
{
  char* html = marker_markdown_render(markdown, size);
  FILE* fp = fopen(filepath, "w");
  if (fp && html)
  {
    fputs(html, fp);
    fclose(fp);
  }
}
                               
void
marker_markdown_render_to_file_with_css(char*  markdown,
                                        size_t size,
                                        char* filepath,
                                        char* css_filepath)
{
  char* html = marker_markdown_render_with_css(markdown, size, css_filepath);
  FILE* fp = fopen(filepath, "w");
  if (fp && html)
  {
    fputs(html, fp);
    fclose(fp);
  }
}

#ifdef PANDOC
void
marker_markdown_pandoc_export(char*                markdown,
                              MarkerExportSettings settings,
                              char*                format,
                              char*                out_file)
{
  char* filepath = marker_utils_escape_file_path(out_file);
  int slash = marker_utils_rfind('/', out_file);
  char loc[slash + 1];
  memset(loc, 0, sizeof(loc));
  memcpy(loc, out_file, slash);
  int ret = chdir(loc);
  if (ret == 0)
  {
    FILE* fp = NULL;
    fp = fopen(TMP_MD, "w");
    if (fp)
    {
      fputs(markdown, fp);
      fclose(fp);
      char command[256] = "pandoc -s -c ";
      strcat(command, settings.style_sheet);
      strcat(command, " -o ");
      strcat(command, filepath);
      strcat(command, " ");
      strcat(command, TMP_MD);
      strcat(command, " -t ");
      strcat(command, format);
      system(command);
      remove(TMP_MD);
    }
  }
  free(filepath);
}
#endif

