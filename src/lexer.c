#include <carp/hashtable.h>
#include <carp/tokenizer.h>
#include <carp/lexer.h>
#include <carp/machine.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <alloca.h>

static void carp_lex_cleanup(struct carp_tok *);
static void carp_lex_exit(struct carp_tok *,
                          struct carp_hashtable *,
                          int);

/* Assigns values to a series of tokens.
 * NUM is obvious.
 * REG is obvious.
 * LBL is NOP instr.
 * FUNC is label lookup value.
 * INSTR is obvious.
 */
void carp_lex_lex(struct carp_machine_state *m, struct carp_tok *tokens) {
  assert(m != NULL);
  assert(tokens != NULL);

  long long length = -1;
  struct carp_tok *tmp = tokens;

  long long num;
  carp_reg reg;
  int64_t instr;
  struct carp_hashtable_entry *res;
  int64_t *code;

  carp_ht_init(&m->labels, 10);

  while (tmp != NULL) {
    switch (tmp->type) {
    case CARP_TOK_UNDEF:
      fprintf(stderr, "Unknown token <%s>\n", tmp->lexeme);
      carp_lex_exit(tokens, &m->labels, EXIT_FAILURE);
      break;
    case CARP_TOK_NUM:
      num = atoi(tmp->lexeme);
      tmp->value = num;
      break;
    case CARP_TOK_REG:
      reg = carp_reg_lookup(tmp->lexeme);
      tmp->value = reg;
      break;
    case CARP_TOK_LBL:
      if (carp_ht_set(&m->labels, tmp->lexeme, tmp->pos)) {
        fprintf(stderr, "Could not make label <%s>\n", tmp->lexeme);
        carp_lex_exit(tokens, &m->labels, EXIT_FAILURE);
      }

      instr = CARP_INSTR_NOP;
      tmp->value = instr;
      break;
    case CARP_TOK_FUNC:
      if ((res = carp_ht_get(&m->labels, tmp->lexeme)) == NULL) {
        fprintf(stderr, "Unknown label <%s>\n", tmp->lexeme);
        carp_lex_exit(tokens, &m->labels, EXIT_FAILURE);
      }

      tmp->value = res->value;
      break;
    case CARP_TOK_VAR:
      break;
    case CARP_TOK_INSTR:
      tmp->value = carp_instr_lookup(tmp->lexeme);
      break;
    }

#ifdef CDEBUG
    printf("[%04ld] %5s (%5s) = %4ld\n",
           tmp->pos, tmp->lexeme, carp_reverse_type[tmp->type], tmp->value);
#endif

    tmp = tmp->next;
    length++;
  }

  tmp = tokens;

  code = alloca(sizeof(int64_t) * length);

  while (tmp != NULL) {
    code[tmp->pos] = tmp->value;
    tmp = tmp->next;
  }

  carp_lex_cleanup(tokens);
  carp_vm_make(m);
  carp_vm_load(m, code, length);
}

/* Frees the carp_tok linked list */
static void carp_lex_cleanup(struct carp_tok *tokens) {
  assert(tokens != NULL);

  struct carp_tok *tmp;

  while (tokens != NULL) {
    tmp = tokens->next;
    free(tokens);
    tokens = tmp;
  }
}

/* Exits cleanly by cleaning up first */
static void carp_lex_exit(struct carp_tok *tokens,
                          struct carp_hashtable *labels,
                          int code) {
  assert(tokens != NULL);
  assert(labels != NULL);

  carp_lex_cleanup(tokens);
  carp_ht_cleanup(labels);

  exit(code);
}
