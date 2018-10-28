#ifndef CARP_TOKENIZER_H
#define CARP_TOKENIZER_H

#include <carp/registers.h>
#include <carp/instructions.h>
#include <stdint.h>

#define CARP_TOK_WORD_LENGTH 10

enum carp_id {
  CARP_TOK_UNDEF,
  CARP_TOK_NUM,
  CARP_TOK_REG,
  CARP_TOK_LBL,
  CARP_TOK_FUNC,
  CARP_TOK_VAR,
  CARP_TOK_INSTR,
};

extern char carp_reverse_type[][6];

struct carp_tok {
  char lexeme[CARP_TOK_WORD_LENGTH];
  enum carp_id type;
  int64_t pos;
  int64_t value;
  struct carp_tok *next;
};

struct carp_tok *carp_lex_tokenize(const char *);
carp_reg carp_reg_lookup(const char *);
carp_instr carp_instr_lookup(const char *);

#endif
