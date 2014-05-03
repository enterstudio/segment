#include <stdio.h>

#include "ast_printer.h"

typedef struct {
  FILE *out;
  int depth;
} printer_state;

static void print_prefix(printer_state *pstate)
{
  for (int i = 0; i < pstate->depth - 1; i++) {
    fputs("| ", pstate->out);
  }
  if (pstate->depth > 0) {
    fputs("|-", pstate->out);
  }
}

static void print_integer(seg_integer_node *node, void *state)
{
  printer_state *pstate = (printer_state *) state;
  print_prefix(pstate);

  fprintf(pstate->out, "INTEGER: %d\n", node->value);
}

static void print_string(seg_string_node *node, void *state)
{
  printer_state *pstate = (printer_state *) state;
  print_prefix(pstate);

  fprintf(pstate->out, "STRING: [%.*s]\n", (int) node->length, node->value);
}

static void print_symbol(seg_symbol_node *node, void *state)
{
  printer_state *pstate = (printer_state *) state;
  print_prefix(pstate);

  seg_symbol *sym = node->value;
  fprintf(pstate->out, "SYMBOL: [%.*s]\n", (int) sym->length, sym->name);
}

static void print_var(seg_var_node *node, void *state)
{
  printer_state *pstate = (printer_state *) state;
  print_prefix(pstate);

  seg_symbol *varname = node->varname;
  fprintf(pstate->out, "VAR: %.*s\n", (int) varname->length, varname->name);
}

static void print_methodcall(seg_methodcall_node *node, void *state)
{
  printer_state *pstate = (printer_state *) state;
  print_prefix(pstate);

  seg_symbol *selector = node->selector;
  fprintf(pstate->out, "METHODCALL: %.*s\n", (int) selector->length, selector->name);
  pstate->depth++;
}

static void print_block(seg_block_node *node, void *state)
{
  printer_state *pstate = (printer_state *) state;
  print_prefix(pstate);

  fprintf(pstate->out, "BLOCK: ");

  seg_parameter_list *initial = node->parameters;
  seg_parameter_list *current = initial;

  if (initial == NULL) {
    fprintf(pstate->out, "without parameters");
  } else {
    fprintf(pstate->out, "[");
  }

  while(current != NULL) {
    if (current != initial) {
      fputc(' ', pstate->out);
    }

    seg_symbol *parameter = current->parameter;
    fprintf(pstate->out, "%.*s", (int) parameter->length, parameter->name);
    current = current->next;
  }

  if (initial != NULL) {
    fputc(']', pstate->out);
  }
  fputc('\n', pstate->out);

  pstate->depth++;
}

static void pop_depth(void *ignored, void *state)
{
  printer_state *pstate = (printer_state *) state;
  pstate->depth--;
}

void seg_print_ast(seg_block_node *root, FILE *outf)
{
    printer_state pstate;
    pstate.out = outf;
    pstate.depth = 0;

    seg_ast_visitor visitor = seg_new_ast_visitor();

    seg_ast_visit_integer(visitor, &print_integer);
    seg_ast_visit_string(visitor, &print_string);
    seg_ast_visit_symbol(visitor, &print_symbol);

    seg_ast_visit_var(visitor, &print_var);

    seg_ast_visit_methodcall(visitor, SEG_VISIT_PRE, &print_methodcall);
    seg_ast_visit_methodcall(visitor, SEG_VISIT_POST, (seg_methodcall_handler) &pop_depth);

    seg_ast_visit_block(visitor, SEG_VISIT_PRE, &print_block);
    seg_ast_visit_block(visitor, SEG_VISIT_POST, (seg_block_handler) &pop_depth);

    seg_ast_visit(visitor, root, &pstate);

    seg_delete_ast_visitor(visitor);
}
