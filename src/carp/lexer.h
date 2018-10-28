#ifndef CARP_LEXER_H
#define CARP_LEXER_H

#include <carp/tokenizer.h>
#include <carp/machine.h>

void carp_lex_lex(struct carp_machine_state *, struct carp_tok *);

#endif
