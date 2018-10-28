#include <carp/registers.h>
#include <carp/instructions.h>
#include <carp/tokenizer.h>
#include <carp/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <assert.h>

char carp_reverse_type[][6] = {
  "undef",
  "num",
  "reg",
  "lbl",
  "func",
  "var",
  "instr"
};

static bool is_sign(char);
static bool is_num(const char *);
static bool is_reg(const char *);
static bool is_label(const char *);
static bool is_var(const char *);
static bool is_instr(const char *);

/* Reads a whole file into memory (really should change to
 * line-by-line), then goes through and copies lexemes, types, and
 * program positions in.
 */
struct carp_tok *carp_lex_tokenize (const char *const fn) {
  char *str = NULL;
  char *toks = NULL;
  struct carp_file_err file_err;
  const char *const delim = " ,\t\n";

  assert(fn != NULL);

  memset(&file_err, 0, sizeof(file_err));

  if ((str = file_read_all(fn, &file_err)) == NULL) {
    fprintf(stderr, "Error: ");
    file_print_err(fn, &file_err);
    fprintf(stderr, "\n");
    return NULL;
  }

  if (file_err.type != CARP_FILE_ERR_NONE) {
    fprintf(stderr, "Warning: ");
    file_print_err(fn, &file_err);
    fprintf(stderr, "\n");
  }

  toks = strtok(str, delim);

  /* empty file, so skip the pain and halt with success there must be
   * a cleaner way to do this
   */
  if (toks == NULL) {
    free(str);
    exit(EXIT_SUCCESS);
  }

  enum carp_id type;
  struct carp_tok *parsed = malloc(sizeof(*parsed));
  struct carp_tok *head = parsed;
  struct carp_tok *lookbehind = NULL;
  int64_t i = 0;

  if (parsed == NULL) {
    free(str);
    fprintf(stderr, "Could not allocate memory (%lu bytes) for a token: %s\n",
            sizeof(*parsed), strerror(errno));
    return NULL;
  }

  while (toks != NULL) {
    int toks_len = strlen(toks);

    if (is_num(toks)) {
      type = CARP_TOK_NUM;
    } else if (is_reg(toks)) {
      type = CARP_TOK_REG;
    } else if (is_label(toks)) {
      type = CARP_TOK_LBL;
    } else if (lookbehind != NULL && strcmp(lookbehind->lexeme, "call") == 0) {
      type = CARP_TOK_FUNC;
    } else if (is_var(toks)) {
      type = CARP_TOK_VAR;
    } else if (is_instr(toks)) {
      type = CARP_TOK_INSTR;
    } else {
      type = CARP_TOK_UNDEF;
    }

    if (type == CARP_TOK_LBL) {
      /* don't copy colon at end */
      memcpy(parsed->lexeme, toks, toks_len - 1);
      parsed->lexeme[toks_len - 1] = 0;
    }
    else if (type == CARP_TOK_VAR) {
      /* don't copy proposed $ at start */
      memcpy(parsed->lexeme, toks + 1, toks_len - 1);
      parsed->lexeme[toks_len - 1] = 0;
    }
    else {
      /* nothing to avoid */
      memcpy(parsed->lexeme, toks, toks_len);
      parsed->lexeme[toks_len] = 0;
    }

    parsed->type = type;
    parsed->pos = i++;
    parsed->next = NULL;

    toks = strtok(NULL, delim);

    if (toks != NULL) {
      parsed->next = malloc(sizeof(*parsed->next));
      lookbehind = parsed;
      parsed = parsed->next;
    }
  }

  free(str);

  return head;
}

/* Returns true if the character is a numeric sign. */
bool is_sign(char c) {
  return c == '+' || c == '-';
}

/* Returns true if the string contains all numbers (can start with a sign). */
bool is_num(const char *s) {
  assert(s != NULL);

  if (!(is_sign(s[0]) || isdigit((unsigned char) s[0]))) {
    return 0;
  }

  for (size_t i = 1; i < strlen(s); i++) {
    if (!isdigit((unsigned char) s[i])) {
      return 0;
    }
  }

  return 1;
}

/* Returns true if the string is in the registers list. */
bool is_reg(const char *s) {
  assert(s != NULL);
  return carp_reg_lookup(s) != CARP_REG_UNDEF;
}

/* Returns true if the string has a : in it. */
bool is_label(const char *s) {
  assert(s != NULL);
  return strchr(s, ':') != NULL;
}

/* Returns true if the string has a $ in it. */
bool is_var(const char *s) {
  assert(s != NULL);
  return strchr(s, '$') != NULL;
}

/* Returns true if the string is in the instructions list. */
bool is_instr(const char *s) {
  assert(s != NULL);
  return carp_instr_lookup(s) != CARP_INSTR_UNDEF;
}

/* Uses strcmp to look up regs. Could probably use a hashtable. */
carp_reg carp_reg_lookup (const char *s) {
  assert(s != NULL);

  for (int i = 0; i < CARP_NUM_REGS; i++)
    if (!strcmp(carp_reverse_reg[i], s))
      return i;

  return CARP_REG_UNDEF;
}

/* Uses strcmp to look up instrs. Could probably use a hashtable. */
carp_instr carp_instr_lookup (const char *s) {
  assert(s != NULL);

  for (int i = 0; i < CARP_NUM_INSTRS; i++)
    if (!strcmp(carp_reverse_instr[i], s))
      return i;

  return CARP_INSTR_UNDEF;
}
