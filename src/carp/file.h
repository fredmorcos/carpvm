#ifndef CARP_FILE_H
#define CARP_FILE_H

#include <stdlib.h>

/**
 * Different types  of possible file  access or operation  errors that
 * may occur from fuctions in this module.
 */
enum carp_file_err_type {
  CARP_FILE_ERR_NONE = 0,       /**
                                 * No   error  has   occurred.  Ignore
                                 * this.  Should  only  be used  as  a
                                 * reset     value     for     `struct
                                 * file_err_t`.
                                 */

  CARP_FILE_ERR_STAT,           /**
                                 * Error getting file information.
                                 */

  CARP_FILE_ERR_OPEN,           /**
                                 * Error opening file.
                                 */

  CARP_FILE_ERR_ALLOC,          /**
                                 * Error allocating a buffer.
                                 */

  CARP_FILE_ERR_READ,           /**
                                 * Error reading from  file. This is a
                                 * minor error and  will not abort the
                                 * procedure. So explicit checking for
                                 * it   even    though   a   procedure
                                 * succeeded is required.
                                 */

  CARP_FILE_ERR_CLOSE           /**
                                 * Error closing file. This is a minor
                                 * error  and   will  not   abort  the
                                 * procedure. So explicit checking for
                                 * it   even    though   a   procedure
                                 * succeeded is required.
                                 */
};

/**
 * File access  or operation  error information  returned from  one or
 * more of the functions in this module.
 */
struct carp_file_err {
  enum carp_file_err_type type; /**
                                 * Type of error.
                                 */

  int sys_errno;                /**
                                 * System `errno` value.
                                 */

  union {
    size_t alloc_size;          /**
                                 * Allocation size  in case  the error
                                 * is of type `CARP_FILE_ERR_ALLOC`.
                                 */
  };
};

/**
 * Read an entire file into a buffer  and return a pointer to it. That
 * buffer must  be freed by  the caller using  free(3).  In case  of a
 * major error,  `NULL` is returned  and `err` is set  accordingly. In
 * case  of one  or  more minor  errors  such as  `CARP_FILE_ERR_READ`
 * and/or `CARP_FILE_ERR_CLOSE`, the buffer  address is still returned
 * but `err` is set according to the last occurring error.
 *
 * @return A pointer to the buffer or NULL in case of error.
 *
 * @param `fn` Filename to read.
 *
 * @param `err` Error struct to fill in case of error.
 */
char *file_read_all(const char *const fn, struct carp_file_err *const err);

/**
 * Prints  an error  message  to  `stderr` without  a  newline or  any
 * prefixes.
 *
 * @param `fn` Filename of erroneous file.
 *
 * @param `err` Error struct to print error information from.
 */
void file_print_err(const char *const fn, struct carp_file_err *const err);

#endif
