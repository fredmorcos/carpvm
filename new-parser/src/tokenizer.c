#include <carp/registers.h>
#include <carp/instructions.h>
#include <carp/tokenizer.h>
#include <carp/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
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

int streq(const char *const s1, const char *const s2) {
  return strncasecmp(s1, s2, strlen(s2));
}

void errset(struct carp_parser_err *const err,
            const enum carp_parser_err_type type,
            const int sys_errno) {
  assert(err);

  err->type = type;
  err->sys_errno = sys_errno;
}

void update(struct carp_parser *const parser) {
  assert(parser);

  if (*parser->p == '\n') {
    parser->line++;
    parser->col = 1;
  } else {
    parser->col++;
  }

  parser->p++;
}

bool ateof(const struct carp_parser *const parser) {
  assert(parser);
  return *parser->p == '\0';
}

char peek(const struct carp_parser *const parser) {
  assert(parser);
  return *parser->p;
}

void tokinit(const struct carp_parser *const parser,
             struct carp_tok *const tok) {
  assert(parser);
  assert(tok);

  tok->p = parser->p;
  tok->len = 0;
  tok->line = parser->line;
  tok->col = parser->col;
  tok->next = NULL;
  tok->children = NULL;
  tok->children_len = 0;
}

struct carp_tok *toknew(const struct carp_parser *const parser,
                        struct carp_parser_err *const err) {
  struct carp_tok *tok = NULL;

  assert(parser);
  assert(err);

  if ((tok = malloc(sizeof(*tok))) == NULL) {
    errset(err, CARP_PARSER_ERR_TYPE_ALLOC, errno);
    return NULL;
  }

  tokinit(parser, tok);
  return tok;
}

void parse_while(struct carp_parser *const parser,
                 int (*predicate)(int),
                 struct carp_tok *const tok) {
  assert(parser);
  assert(predicate);

  while (!ateof(parser) && predicate(*parser->p)) {
    update(parser);

    if (tok) {
      tok->len++;
    }
  }
}

void parse_until_delim(struct carp_parser *const parser,
                       const char *const delim,
                       struct carp_tok *const tok) {
  assert(parser);
  assert(delim);

  while (!ateof(parser) && !strchr(delim, *parser->p)) {
    update(parser);

    if (tok) {
      tok->len++;
    }
  }
}

struct carp_tok *parse_tok(struct carp_parser *const parser,
                           struct carp_parser_err *const err,
                           struct carp_tok *const tok) {
  struct carp_parser old_state;

  assert(parser);
  assert(err);
  assert(tok);

  if (ateof(parser)) {                       /* end of file */
    tok->type = CARP_PARSER_TOKEN_TYPE_EOF;
  } else if (isspace(peek(parser))) {        /* spacing */
    parse_while(parser, isspace, NULL);
    return parse_tok(parser, err, tok);
  } else if (peek(parser) == '#') {          /* comment */
    parse_until_delim(parser, "\n", NULL);
    return parse_tok(parser, err, tok);
  } else if (peek(parser) == '-' ||          /* number */
             isdigit(peek(parser))) {
    if (peek(parser) == '-') {
      old_state = *parser;
      update(parser);
      tok->len++;

      if (ateof(parser)) {
        errset(err, CARP_PARSER_ERR_TYPE_UNEXPECTED_EOF, 0);
        *parser = old_state;
        tok->len--;
        return NULL;
      }
    }

    if (isdigit(peek(parser))) {
      parse_while(parser, isdigit, tok);
      tok->type = CARP_PARSER_TOKEN_TYPE_INT;
    } else {
      errset(err, CARP_PARSER_ERR_TYPE_EXPECTED_NUM_DEC, 0);
      *parser = old_state;
      tok->len--;
      return NULL;
    }

    if (!ateof(parser) && peek(parser) == '.') {
      old_state = *parser;
      update(parser);
      tok->len++;

      if (isdigit(peek(parser))) {
        parse_while(parser, isdigit, tok);
        tok->type = CARP_PARSER_TOKEN_TYPE_FLOAT;
      } else {

    }
  } else {                                   /* instruction */
    old_state = *parser;
    parse_until_delim(parser, ":\t ", tok);
    if (peek(parser) == ':') {
      tok->type = CARP_PARSER_TOKEN_TYPE_LABEL;
      update(parser);
    } else {
      if (streq(tok->p, "push") == 0 ||
          streq(tok->p, "pop")  == 0 ||
          streq(tok->p, "add")  == 0) {
        tok->type = CARP_PARSER_TOKEN_TYPE_INSTR;
      } else {
        errset(err, CARP_PARSER_ERR_TYPE_UNKNOWN_INSTR, 0);
        *parser = old_state;
        return NULL;
      }
    }
  }

  return tok;
}

struct carp_tok *parse(struct carp_parser *const parser,
                       struct carp_parser_err *const err) {
  struct carp_parser backup_parser;
  struct carp_tok *tok = NULL;
  struct carp_tok *prev_tok = NULL;
  struct carp_tok *next_tok = NULL;          /* used only for freeing */
  struct carp_tok **top_tok = &tok;

  assert(parser != NULL);
  assert(err != NULL);

  while (!ateof(parser)) {
    if (isspace(peek(parser))) {             /* skip spaces */
      parse_while(parser, isspace, NULL);
    } else if (peek(parser) == '#') {        /* skip comment */
      parse_until_delim(parser, "\n", NULL);
    } else {                                 /* label or keyword */
      prev_tok = tok;
      if ((tok = toknew(parser, err)) == NULL) {
        goto error;
      }

      backup_parser = *parser;
      parse_until_delim(parser, ":\t ", tok);

      if (tok->len < 1) {
        errset(err, CARP_PARSER_ERR_TYPE_EMPTY, 0);
        goto error;
      }

      if (peek(parser) == ':') {             /* label */
        tok->type = CARP_TOK_LBL;
        update(parser);
      } else {
        if (streq(tok->p, "push") == 0) {
          tok->type = CARP_TOK_INSTR_PUSH;

          parse_while(parser, isspace, NULL); /* skip spaces */

          if (ateof(parser)) {
            errset(err, CARP_PARSER_ERR_TYPE_UNEXPECTED_EOF, 0);
            goto error;
          } else {
            if (isdigit(peek(parser))) {
              if ((tok->children = toknew(parser, err)) == NULL) {
                goto error;
              }

              parse_while(parser, isdigit, tok->children);
              tok->children_len = 1;
              tok->children->type = CARP_TOK_NUM;
            } else if (peek(parser) == '-') {
              /* negative int */
            } else {
              errset(err, CARP_PARSER_ERR_TYPE_UNKNOWN_PARAM_TYPE, 0);
              goto error;
            }
          }
        } else if (streq(tok->p, "pop") == 0) {
          tok->type = CARP_TOK_INSTR_POP;
        } else if (streq(tok->p, "add") == 0) {
          tok->type = CARP_TOK_INSTR_ADD;
        } else {
          errset(err, CARP_PARSER_ERR_TYPE_UNKNOWN_INSTR, 0);
          goto error;
        }
      }

      prev_tok->next = tok;
    }
  }

  return *top_tok;

 error:
  *parser = backup_parser;                   /* restore prev parser state */
  free(tok);
  for (tok = *top_tok; tok; tok = next_tok) {
    next_tok = tok->next;
    if (tok->children) {
      free(tok->children);
    }
    free(tok);
  }
  return NULL;
}

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
