/*
  php_gto -- Gustavus Testing Overrides
  An extension to override and mock function/method definitions at runtime.

  ==============================================================================

  Copyright (c) 2013, Gustavus Adolphus College
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    * Neither the name of the copyright holder(s) nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  ==============================================================================

  Author(s): Chris Rog <crog@gustavus.edu>
*/

#include <php.h>

#include "php_gto.h"



// Functions exposed to PHP
ZEND_BEGIN_ARG_INFO(arginfo_override_function, 0)
  ZEND_ARG_INFO(0, function)
  ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_override_method, 0)
  ZEND_ARG_INFO(0, class)
  ZEND_ARG_INFO(0, method)
  ZEND_ARG_INFO(0, callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_override_revert, 0)
  ZEND_ARG_INFO(0, token)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_call_overridden_func, 0)
  ZEND_ARG_INFO(0, token)
  ZEND_ARG_INFO(0, object)
  ZEND_ARG_INFO(0, parameter)
  ZEND_ARG_INFO(0, ...)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_call_overridden_func_array, 0)
  ZEND_ARG_INFO(0, token)
  ZEND_ARG_INFO(0, object)
  ZEND_ARG_INFO(0, parameters) /* ARRAY_INFO(0, params, 1) */
ZEND_END_ARG_INFO()

zend_function_entry gto_functions[] = {
  PHP_FE(override_function,           arginfo_override_function)
  PHP_FE(override_method,             arginfo_override_method)
  PHP_FE(override_revert,             arginfo_override_revert)

  PHP_FE(call_overridden_func,        arginfo_call_overridden_func)
  PHP_FE(call_overridden_func_array,  arginfo_call_overridden_func_array)

  { NULL, NULL, NULL }
};


ZEND_DECLARE_MODULE_GLOBALS(gto)

#ifdef ZTS
#define GTO_G(v) TSRMG(gto_globals_id, zend_gto_globals *, v)
#else
#define GTO_G(v) (gto_globals.v)
#endif



// List entries
#define GTO_ORTOKEN_RES_NAME "GTO Override Token"
int le_gto_override_token;


// Module entry
zend_module_entry gto_module_entry = {
  STANDARD_MODULE_HEADER,
  PHP_GTO_EXTNAME,
  gto_functions,
  PHP_MINIT(gto),
  NULL,
  NULL,
  NULL,
  NULL,
  PHP_GTO_VERSION,
  STANDARD_MODULE_PROPERTIES
};

// Install module
ZEND_GET_MODULE(gto)




// Data structures!
////////////////////////////////////////////////////////////////////////////////////////////////////
struct _gto_override_token {
  int active;

  zend_class_entry *class;
  char *function_name;
  int name_len;

  zend_function *function;
  zend_function *override;

  struct _gto_override_token *prev;   // Same function/method override stack
  struct _gto_override_token *next;   // ^

  struct _gto_override_token *cnext;  // Child method override list
  struct _gto_override_token *cprev;  // ^

  struct _gto_override_token *parent; // Reference to the parent token
};




// Initializers & Destructors
////////////////////////////////////////////////////////////////////////////////////////////////////
PHP_MINIT_FUNCTION(gto)
{
  le_gto_override_token = zend_register_list_destructors_ex(gto_ortoken_dtor, NULL, GTO_ORTOKEN_RES_NAME, module_number);

  ZEND_INIT_MODULE_GLOBALS(gto, gto_init_globals, NULL);
}

static void gto_init_globals(zend_gto_globals *globals)
{
  globals->ctokens = NULL;
  globals->ftokens = NULL;
}

static void gto_ortoken_dtor(zend_rsrc_list_entry *rsrc   TSRMLS_DC)
{
  free_override_token((gto_override_token*) rsrc->ptr);
}

static void gto_htentry_dtor(void *data)
{
  zend_hash_destroy(data);
  //FREE_HASHTABLE(data); // We can't do this, or we blow up the hashtable.
}




// PHP Functions
////////////////////////////////////////////////////////////////////////////////////////////////////
PHP_FUNCTION(override_function)
{
  char *func_name;
  int func_name_len;
  zend_fcall_info fci;
  zend_fcall_info_cache fcc;
  gto_override_token *token;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sf", &func_name, &func_name_len, &fci, &fcc) == FAILURE) {
    RETURN_FALSE;
  }

  if (gto_inject_override(NULL, func_name, func_name_len, fcc.function_handler, 1, &token) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, token, le_gto_override_token);
}

PHP_FUNCTION(override_method)
{
  zend_class_entry* ce = NULL;

  char *func_name;
  int func_name_len;
  zend_fcall_info fci;
  zend_fcall_info_cache fcc;
  gto_override_token *token;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Csf", &ce, &func_name, &func_name_len, &fci, &fcc) == FAILURE) {
    RETURN_FALSE;
  }

  if (gto_inject_override(ce, func_name, func_name_len, fcc.function_handler, 1, &token) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_REGISTER_RESOURCE(return_value, token, le_gto_override_token);
}

PHP_FUNCTION(override_revert)
{
  zval *res;
  gto_override_token *token;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &res) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(token, gto_override_token*, &res, -1, GTO_ORTOKEN_RES_NAME, le_gto_override_token);

  if (gto_revert_override(token) == FAILURE) {
    // @todo: Maybe throw an error here...?
    RETURN_FALSE;
  }

  zend_list_delete(Z_LVAL_P(res));
  RETURN_TRUE;
}


PHP_FUNCTION(call_overridden_func)
{
  zval *res, *object;
  gto_override_token *token;
  zval ***argv = NULL;
  int argc = 0;


  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zo!*", &res, &object, &argv, &argc) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(token, gto_override_token*, &res, -1, GTO_ORTOKEN_RES_NAME, le_gto_override_token);

  gto_call_override_token(token, object, argc, argv, return_value);

  if (argc) {
    efree(argv);
  }
}

PHP_FUNCTION(call_overridden_func_array)
{
  zval *res, *object;
  gto_override_token *token;
  zval *params, ***argv = NULL;
  int argc = 0, index = 0;
  HashTable *pht;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zo!A/", &res, &object, &params) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(token, gto_override_token*, &res, -1, GTO_ORTOKEN_RES_NAME, le_gto_override_token);

  // Shamelessly stolen from PHP's call_user_method_array (basic_functions.c ~4802)
  pht = HASH_OF(params);
  argc = zend_hash_num_elements(pht);
  argv = (zval***) safe_emalloc(sizeof(zval**), argc, 0);

  for (zend_hash_internal_pointer_reset(pht); zend_hash_get_current_data(pht, (void**) &(argv[index++])); zend_hash_move_forward(pht));
  // No body

  gto_call_override_token(token, object, argc, argv, return_value);

  efree(argv);
}





// Utility functions/implementations
////////////////////////////////////////////////////////////////////////////////////////////////////
static gto_override_token* fetch_override_token(zend_class_entry *ce, char* function_name, int name_len)
{
  gto_override_token **token;
  zend_str_tolower(function_name, name_len);

  if (ce) {
    // Impl note:
    // We don't lcase the class name since PHP already goes through the trouble of converting class
    // strings at the PHP level to class references for us here in C. As such, we should always be
    // getting the proper casing of the class' name.

    HashTable *ftokens;
    return (GTO_G(ctokens) && zend_hash_find(GTO_G(ctokens), ce->name, ce->name_length + 1, (void**)&ftokens) == SUCCESS && zend_hash_find(ftokens, function_name, name_len + 1, (void**)&token) == SUCCESS) ? *token : NULL;
  } else {
    return (GTO_G(ftokens) && zend_hash_find(GTO_G(ftokens), function_name, name_len + 1, (void**)&token) == SUCCESS) ? *token : NULL;
  }
}

static int store_override_token(gto_override_token *token)
{
  if (token) {
    if (token->class) {
      if (!GTO_G(ctokens)) {
        ALLOC_HASHTABLE(GTO_G(ctokens));
        zend_hash_init(GTO_G(ctokens), 10, NULL, &gto_htentry_dtor, 0);
      }

      HashTable *ftokens;

      if (zend_hash_find(GTO_G(ctokens), token->class->name, token->class->name_length + 1, (void**)&ftokens) != SUCCESS) {
        ALLOC_HASHTABLE(ftokens);

        if (zend_hash_add(GTO_G(ctokens), token->class->name, token->class->name_length + 1, ftokens, sizeof(HashTable), (void**)&ftokens) != SUCCESS) {
          FREE_HASHTABLE(ftokens);
          return FAILURE;
        }

        zend_hash_init(ftokens, 5, NULL, NULL, 0);
      }


      return zend_hash_update(ftokens, token->function_name, token->name_len + 1, &token, sizeof(void*), NULL);
    } else {
      if (!GTO_G(ftokens)) {
        ALLOC_HASHTABLE(GTO_G(ftokens));
        zend_hash_init(GTO_G(ftokens), 10, NULL, NULL, 0);
      }


      return zend_hash_update(GTO_G(ftokens), token->function_name, token->name_len + 1, &token, sizeof(void*), NULL);
    }
  }

  return FAILURE;
}

static int clear_override_token(gto_override_token *token)
{
  if (token) {
    if (token->class) {
      HashTable *ftokens;
      if (!GTO_G(ctokens) || zend_hash_find(GTO_G(ctokens), token->class->name, token->class->name_length + 1, (void**)&ftokens) != SUCCESS) {
       return FAILURE;
      }

      if (zend_hash_del(ftokens, token->function_name, token->name_len + 1) != SUCCESS) {
       return FAILURE;
      }

      if (!zend_hash_num_elements(ftokens)) {
        // Cleanup is performed by the gto_htentry_dtor function.

        if (zend_hash_del(GTO_G(ctokens), token->class->name, token->class->name_length + 1) == SUCCESS && !zend_hash_num_elements(GTO_G(ctokens))) {
          zend_hash_destroy(GTO_G(ctokens));
          FREE_HASHTABLE(GTO_G(ctokens));
          GTO_G(ctokens) = NULL;
        }
      }

      return SUCCESS;
    } else {
      if (!GTO_G(ftokens) || zend_hash_del(GTO_G(ftokens), token->function_name, token->name_len + 1) != SUCCESS) {
       return FAILURE;
      }

      if (!zend_hash_num_elements(GTO_G(ftokens))) {
        zend_hash_destroy(GTO_G(ftokens));
        FREE_HASHTABLE(GTO_G(ftokens));
        GTO_G(ftokens) = NULL;
      }

      return SUCCESS;
    }
  }

  return FAILURE;
}

static int free_override_token(gto_override_token *token)
{
  // Do NOT call this function twice on the same token.

  if (token) {
    if (token->active && gto_revert_override(token) == FAILURE) {
      // Impl note:
      // This is a problem. If the restoration failed, we still need to free the resources, but our
      // state is completely hosed.
      // This tends to occur when the request has ended and other modules start messing with stuff
      // during their cleanup. In such cases, we don't care much about the state anyhow, since any
      // failures will stem from missing or modified functions.
      // Runkit and runkit-like things are also capable of breaking things in a horrible way.
      // Though, we don't care about such cases nearly as much, as the user should already know the
      // horrors that can occur from messing with the symbol table.


      // @todo:
      // if the function is a user function:
      //  - Check that the class still exists (this shouldn't happen, if we're doing it right.)
      //    - If the class no longer exists, there's nothing to restore anyway, so whatever.

      // If the function is an internal function:
      //  - Check if the module in which the function resides is still loaded.
      //    - If not, we don't care, since the module likely did cleanup while we weren't looking.

      // In all other cases... we still need to free the memory, but our state is completely hosed.

      php_error(E_ERROR, "Unable to restore original state for function/method \"%s\".", token->function_name);
    }

    // Free memory associated with this token.
    efree(token->function_name);
    efree(token->function);
    efree(token->override);

    // Free the list of children...
    for (; token->cnext; token->cnext = token->cnext->cnext) {
      free_override_token(token->cnext);
    }

    // Decrement ref counts...
    if (token->class) {
      --token->class->refcount;
    }

    // Free the token!
    efree(token);
  }
}



static int gto_inject_override(zend_class_entry *ce, char* function_name, int name_len, zend_function *override, int update_children, gto_override_token **token_out)
{
  HashTable *ftable = ce ? &(ce->function_table) : EG(function_table);

  zend_function *function;
  gto_override_token *token = *token_out = emalloc(sizeof(gto_override_token));
  gto_override_token *ptoken = fetch_override_token(ce, function_name, name_len); // This lcases our function name.

  token->class = ce;
  token->function_name = zend_str_tolower_dup(function_name, name_len);
  token->name_len = name_len;

  if (zend_hash_find(ftable, token->function_name, token->name_len + 1, (void**)&function) != SUCCESS) {
    efree(token->function_name);
    efree(token);
    *token_out = NULL;

    return FAILURE;
  }

  token->function = emalloc(sizeof(zend_function));
  memcpy(token->function, function, sizeof(zend_function));

  token->override = emalloc(sizeof(zend_function));
  memcpy(token->override, override, sizeof(zend_function));

  token->override->common.fn_flags = token->function->common.fn_flags;
  token->override->common.scope = ce && update_children ? ce : token->function->common.scope;

  if (token->prev = ptoken) {
    ptoken->next = token;
  }

  token->next = NULL;
  token->cprev = NULL;
  token->cnext = NULL;
  token->parent = NULL;


  if (store_override_token(token) != SUCCESS) {
    free_override_token(token);
    *token_out = NULL;

    return FAILURE;
  }

  if (token->class) {
    ++token->class->refcount;
  }

  function_add_ref(token->override);
  memcpy(function, token->override, sizeof(zend_function));

  if (update_children && token->class && gto_update_children(token) != SUCCESS) {
    free_override_token(token);
    *token_out = NULL;

    return FAILURE;
  }

  token->active = 1;
  return SUCCESS;
}

static int gto_revert_override(gto_override_token *token)
{
  zend_function *function;
  gto_override_token *ctoken;

  if (token && token->active) {
    token->active = 0;

    if (token->cnext) {
      for (ctoken = token->cnext; ctoken; ctoken = ctoken->cnext) {
        gto_revert_override(ctoken);
      }
    }

    if (!token->next) {
      if (token != fetch_override_token(token->class, token->function_name, token->name_len)) {
        // This isn't the top-level token -- our state is hosed.
        php_error(E_ERROR, "Token state mismatch; Attempting to revert overrides out-of-order.\n");
        return FAILURE;
      }

      HashTable *ftable = token->class ? &(token->class->function_table) : EG(function_table);
      if (zend_hash_find(ftable, token->function_name, token->name_len + 1, (void**) &function) == FAILURE) {
        // Function has already been removed, it seems; likely due to global cleanup.
        return FAILURE;
      }

      memcpy(function, token->function, sizeof(zend_function));

      if (token->prev) {
        token->prev->next = NULL;
        if (store_override_token(token->prev) != SUCCESS) {
          php_error(E_ERROR, "Unable to update token state for function \"%s\".\n", token->function_name);
        }
      } else {
        if (clear_override_token(token) != SUCCESS) {
          php_error(E_ERROR, "Unable to clear token state for function \"%s\".\n", token->function_name);
        }
      }
    } else {
      // Swap function references so this token's state can be restored.
      function = token->next->function;
      token->next->function = token->function;
      token->function = function;

      // Remove token from the chain.
      if (token->next->prev = token->prev) {
        token->prev->next = token->next;
      }
    }
  }

  return SUCCESS;
}

static int gto_update_children(gto_override_token *token)
{
  if (!token || !token->class) {
    return FAILURE;
  }

  HashTable *ht = EG(class_table);
  HashPosition hpos;

  zend_class_entry **class;
  zend_function *function;
  gto_override_token *cthead = NULL;
  gto_override_token *ctoken;
  gto_override_token *ptoken;
  gto_override_token *pltoken;


  for (zend_hash_internal_pointer_reset_ex(ht, &hpos); zend_hash_get_current_data_ex(ht, (void**) &class, &hpos) == SUCCESS; zend_hash_move_forward_ex(ht, &hpos)) {
    if (*class != token->class && instanceof_function(*class, token->class)) {

      if (zend_hash_find(&(*class)->function_table, token->function_name, token->name_len + 1, (void**) &function) == SUCCESS) {

        if (function->common.scope && !instanceof_function(function->common.scope, *class)) {

          ctoken = emalloc(sizeof(gto_override_token));
          if (gto_inject_override(*class, token->function_name, token->name_len, token->override, 0, &ctoken) != SUCCESS) {
            php_error(E_ERROR, "Unable to apply override to child method: %s.%s(...)\n", (*class)->name, token->function_name);
            return FAILURE;
          }

          if (ctoken->cprev = cthead) {
            cthead->cnext = ctoken;
          } else {
            token->cnext = ctoken;
          }

          ctoken->cnext = NULL;
          ctoken->parent = token;

          cthead = ctoken;
        } else {

          if (ptoken = fetch_override_token(*class, token->function_name, token->name_len)) {
            ctoken = emalloc(sizeof(gto_override_token));

            ctoken->class = *class;
            ctoken->function_name = zend_str_tolower_dup(token->function_name, token->name_len);
            ctoken->name_len = token->name_len;

            ctoken->function = emalloc(sizeof(zend_function));
            ctoken->override = emalloc(sizeof(zend_function));
            memcpy(ctoken->override, token->override, sizeof(zend_function));

            for (pltoken = ptoken, ptoken = ptoken->prev; ptoken; pltoken = ptoken, ptoken = ptoken->prev) {
              if (ptoken->parent && (ptoken->parent->class == token->class || !instanceof_function(ptoken->parent->class, token->class))) {
                // Inject override token between ptoken and ptoken->next
                memcpy(ctoken->function, ptoken->override, sizeof(zend_function));
                memcpy(ptoken->next->function, ctoken->override, sizeof(zend_function));

                ctoken->next = ptoken->next;
                ctoken->prev = ptoken;

                ptoken->next->prev = ctoken;
                ptoken->next = ctoken;

                goto post_injection; // UH OH! The dredded goto keyword!
              }
            }

            // We didn't find a better injection point, so we just need to put it at the beginning
            // of the token chain.
            memcpy(ctoken->function, pltoken->function, sizeof(zend_function));
            memcpy(pltoken->function, ctoken->override, sizeof(zend_function));

            ctoken->next = pltoken;
            ctoken->prev = NULL;

            pltoken->prev = ctoken;


post_injection:
            ctoken->active = 1;

            if (ctoken->cprev = cthead) {
              cthead->cnext = ctoken;
            } else {
              token->cnext = ctoken;
            }

            ctoken->cnext = NULL;
            ctoken->parent = token;

            cthead = ctoken;
          }

        }
      }
    }
  }

  return SUCCESS;
}

static int gto_call_override_token(gto_override_token *token, zval *object, int argc, zval ***argv, zval *retval)
{
  zval *retval_ptr;
  zend_fcall_info fci;
  zend_fcall_info_cache fcc;
  zend_class_entry *ce;

  if (token->class && !(token->function->common.fn_flags & ZEND_ACC_STATIC)) {
    if (!object || ZVAL_IS_NULL(object)) {
      php_error_docref(NULL TSRMLS_CC, E_ERROR, "An object instance is required when calling an overridden method.\n");
      ZVAL_NULL(retval);
      return FAILURE;
    }

    // Check object class here. We need to have the same class as the token for the scope to have
    // meaning. While I'm curious what kind of shenanigans can happen if we provide an instance of
    // the wrong class, it's not likely desirable. I can only imagine debugging such an issue would
    // cause all sorts of headaches.
    if (!(ce = zend_get_class_entry(object)) || !instanceof_function(ce, token->class)) {
      php_error_docref(NULL TSRMLS_CC, E_ERROR, "The object provided must be an instance of the class \"%s\" to call this override.\n", token->class->name);
      ZVAL_NULL(retval);
      return FAILURE;
    }
  }

  // ZVAL-ize our string. Hopefully this won't segfault on us when it falls off the stack.
  zval funcname;
  ZVAL_STRING(&funcname, token->function_name, 0);

  // Impl note:
  // We're not using zend_fcall_info_init here because what we're doing is far off from what is
  // expected by PHP. Moreover, the afore mentioned function checks if our target is callable, which
  // isn't something we care about as we already know the target is valid and reflection can violate
  // any visibility modifiers anyhow.
  fci.size = sizeof(fci);
  fci.function_name = &funcname;
  fci.function_table = token->class ? &token->class->function_table : EG(function_table);
  fci.symbol_table = NULL; // null by default and call_user_method sets the symbol table to null. Should probably remain null.
  fci.retval_ptr_ptr = &retval_ptr;
  fci.param_count = argc;
  fci.params = argv;
  fci.object_ptr = (token->class && !(token->function->common.fn_flags & ZEND_ACC_STATIC)) ? object : NULL;
  fci.no_separation = token->class ? 1 : 0; // call_user_method doesn't do this, so we probably shouldn't either.

  fcc.initialized = 1;
  fcc.function_handler = token->function;
  fcc.calling_scope = token->class ? token->class : NULL;
  fcc.called_scope = EG(scope);
  fcc.object_ptr = fci.object_ptr;

  if (zend_call_function(&fci, &fcc TSRMLS_CC) == SUCCESS && fci.retval_ptr_ptr && *fci.retval_ptr_ptr) {
    COPY_PZVAL_TO_ZVAL(*retval, *fci.retval_ptr_ptr);
  } else {
    ZVAL_NULL(retval);
  }

  return SUCCESS;
}