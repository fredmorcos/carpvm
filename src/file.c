#include <carp/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/stat.h>

inline void file_set_err(struct carp_file_err *const err,
                         enum carp_file_err_type type,
                         int sys_errno);

void file_set_err(struct carp_file_err *const err,
                  enum carp_file_err_type type,
                  int sys_errno) {
  err->type = type;
  err->sys_errno = sys_errno;
}

char *file_read_all(const char *const fn, struct carp_file_err *const err) {
  FILE *fp = NULL;
  char *contents = NULL;
  size_t fsize;
  size_t nread;
  struct stat st;

  assert(fn != NULL);

  /* Figure out file size */
  if (stat(fn, &st) == -1) {
    file_set_err(err, CARP_FILE_ERR_STAT, errno);
    goto finish_1;
  }

  if (!(fp = fopen(fn, "r"))) {
    file_set_err(err, CARP_FILE_ERR_OPEN, errno);
    goto finish_1;
  }

  fsize = st.st_size;
  fsize *= sizeof(*contents);

  /* + 1 is for the NULL terminator */
  if (!(contents = malloc(fsize + 1))) {
    file_set_err(err, CARP_FILE_ERR_ALLOC, errno);
    err->alloc_size = fsize + 1;
    goto finish_2;
  }

  if ((nread = fread(contents, sizeof(*contents), fsize, fp)) != fsize) {
    file_set_err(err, CARP_FILE_ERR_READ, errno);
  }

  contents[nread] = '\0';

 finish_2:
  if (fclose(fp) == EOF) {
    assert(fp != NULL);
    file_set_err(err, CARP_FILE_ERR_CLOSE, errno);
    clearerr(fp);
  }

 finish_1:
  return contents;
}

void  file_print_err(const char *const fn, struct carp_file_err *const err) {
  const char *const file_err_fmts[] = {
    [CARP_FILE_ERR_NONE]  = NULL,
    [CARP_FILE_ERR_STAT]  = "Cannot stat `%s': %s",
    [CARP_FILE_ERR_OPEN]  = "Cannot open `%s': %s",
    [CARP_FILE_ERR_ALLOC] = "Cannot allocate %lu bytes for reading `%s': %s",
    [CARP_FILE_ERR_READ]  = "Cannot read `%s': %s",
    [CARP_FILE_ERR_CLOSE] = "Cannot close `%s': %s"
  };

  if (err->type != CARP_FILE_ERR_NONE || err->type != CARP_FILE_ERR_ALLOC) {
    fprintf(stderr, file_err_fmts[err->type],
            fn, strerror(err->sys_errno));
  } else if (err->type == CARP_FILE_ERR_ALLOC) {
    fprintf(stderr, file_err_fmts[err->type],
            err->alloc_size, fn, strerror(err->sys_errno));
  }
}
