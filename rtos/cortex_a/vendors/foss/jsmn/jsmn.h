#ifndef __JSMN_H_
#define __JSMN_H_

//#include <stddef.h>
#include "AmbaTypes.h"

#define JSMN_PARENT_LINKS (1)

#ifdef __cplusplus
extern "C" {
#endif

/**
 * JSON type identifier. Basic types are:
 * 	o Object
 * 	o Array
 * 	o String
 * 	o Other primitive: number, boolean (true/false) or null
 */
typedef enum {
	JSMN_UNDEFINED = 0,
	JSMN_OBJECT = 1,
	JSMN_ARRAY = 2,
	JSMN_STRING = 3,
	JSMN_PRIMITIVE = 4
} jsmntype_t;

enum jsmnerr {
	/* Not enough tokens were provided */
	JSMN_ERROR_NOMEM = -1,
	/* Invalid character inside JSON string */
	JSMN_ERROR_INVAL = -2,
	/* The string is not a full JSON packet, more bytes expected */
	JSMN_ERROR_PART = -3
};

/**
 * JSON token description.
 * type		type (object, array, string etc.)
 * start	start position in JSON data string
 * end		end position in JSON data string
 */
typedef struct {
	jsmntype_t type;
	INT32 start;
	INT32 end;
	INT32 size;
#ifdef JSMN_PARENT_LINKS
	INT32 parent;
#endif
} jsmntok_t;

/**
 * JSON parser. Contains an array of token blocks available. Also stores
 * the string being parsed now and current position in that string
 */
typedef struct {
	UINT32 pos; /* offset in the JSON string */
	UINT32 toknext; /* next token to allocate */
	INT32 toksuper; /* superior token node, e.g parent object or array */
} jsmn_parser;

/**
 * Create JSON parser over an array of tokens
 */
void jsmn_init(jsmn_parser *parser);

/**
 * Run JSON parser. It parses a JSON data string into and array of tokens, each describing
 * a single JSON object.
 */
INT32 jsmn_parse(jsmn_parser *parser, const char *js, size_t len,
		jsmntok_t *tokens, UINT32 num_tokens);

#ifdef __cplusplus
}
#endif

#endif /* __JSMN_H_ */
