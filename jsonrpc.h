#include <stdint.h>
#include <jansson.h>

#ifndef _JSONRPC_H_
#define _JSONRPC_H_

#define JSONRPC_PARSE_ERROR			-32700
#define JSONRPC_INVALID_REQUEST 	-32600
#define JSONRPC_METHOD_NOT_FOUND 	-32601
#define JSONRPC_INVALID_PARAMS		-32602
#define JSONRPC_INTERNAL_ERROR		-32603

typedef int (*jsonrpc_method_f)(json_t *json_params, json_t **result, void *userdata);
typedef void (*jsonrpc_response_f)(json_t *result, uint32_t id, void *userdata);

typedef struct jsonrpc_method {
	const char 			*name;
	jsonrpc_method_f 	func;
	const char 			*params_spec;
} jsonrpc_method_t;

char *jsonrpc_handler(const char *msg, size_t len, jsonrpc_method_t methods[], size_t flags, void *userdata, jsonrpc_response_f response_handler);

json_t *jsonrpc_error_object(int code, const char *message, json_t *data);
json_t *jsonrpc_error_object_predefined(int code, json_t *data);
json_t* jsonrpc_notification(const char *method, json_t *params);
json_t* jsonrpc_request(const char *method, json_t *params, uint32_t id);

#endif /* _JSONRPC_H_ */

