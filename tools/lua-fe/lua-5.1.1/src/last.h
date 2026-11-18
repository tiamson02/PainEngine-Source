#ifndef LAST_H_ALREADY_INCLUDED
#define LAST_H_ALREADY_INCLUDED

#include <stdio.h>
#include <llex.h>

int g_indent_level = -1;

void print_indent()
{
	int i;
	for (i = 0; i < g_indent_level; ++i)
	{
		fprintf(stderr, " ");
	}
}


void indent()
{
	g_indent_level++;
}


void deindent()
{
	g_indent_level--;
}

const char * get_op_str(int op) 
{
  switch (op) {
    case '+': return "+";
    case '-': return "-";
    case '*': return "*";
    case '/': return "/";
    case '%': return "%";
    case '^': return "^";
    case TK_CONCAT: return "..";
    case TK_NE: return "~=";
    case TK_EQ: return "==";
    case '<': return "<";
    case TK_LE: return "<=";
    case '>': return ">";
    case TK_GE: return ">=";
    case TK_AND: return "and";
    case TK_OR: return "or";
    case TK_NOT: return "not";
	case '#': return "#";

    default: return 0;
  }
}

const char * get_token_str(LexState * ls)
{
	const char * s = get_op_str( ls->t.token );

	// Operators are tokenized to ints, so try
	// to recover the operator in string form.

	if ( s )
		return s;
	else
		return getstr( ls->t.seminfo.ts );
}

void start_tag(const char * name)
{
	indent();
	print_indent();
	fprintf(stderr, "<%s>\n", name);
}

void end_tag(const char * name)
{
	print_indent();
	fprintf(stderr, "</%s>\n", name);
	deindent();
}

// ugh.
// get this compiling under c++, then use std::string and stl

#define StringlistSize 1000
#define StringlistStringSize 100

char g_string_list[ StringlistSize ][ StringlistStringSize ];
int g_string_list_len = 0;

void clear_string_list()
{
	g_string_list_len = 0;
}

int string_list_len()
{
	return g_string_list_len;
}

const char * get_list_string(int i)
{
	if (i < string_list_len())
		return g_string_list[i];
	else
		return 0;
}

void queue_string(const char * s)
{
	if ( g_string_list_len < StringlistSize )
	{
		strncpy( g_string_list[ g_string_list_len++ ], s, StringlistStringSize-1 );
		g_string_list[ g_string_list_len - 1 ][ StringlistStringSize - 1 ] = '\0';
	}
}

#endif	// LAST_H_ALREADY_INCLUDED
