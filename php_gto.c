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

zend_function_entry gto_functions[] = {
  PHP_FE(override_function, arginfo_override_function)
  PHP_FE(override_method,   arginfo_override_method)
  PHP_FE(override_revert,   arginfo_override_revert)

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

  struct _gto_override_token *prev;
  struct _gto_override_token *next;
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
  gto_override_token *token = (gto_override_token*) rsrc->ptr;

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
    efree(token);
  }
}

static void gto_htentry_dtor(void *data)
{
  zend_hash_destroy(data);
  //FREE_HASHTABLE(data);
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

  if (gto_inject_override(NULL, func_name, func_name_len, fcc.function_handler, &token) == FAILURE) {
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

  if (gto_inject_override(ce, func_name, func_name_len, fcc.function_handler, &token) == FAILURE) {
    RETURN_FALSE;
  }

  // @todo:
  // We may need to add code here to update children methods to refer to the overridden method. This
  // also necessitates code to remove the override reference in children methods as well.
  // In any event, as it stands now, overriding a class' method only impacts that class itself. This
  // may not be a bad thing, as test mocks should likely only be impacting the classes we're
  // targeting explicitly.
  //
  // Things that would need to change to facilitiate this functionality:
  //  - Tokens would need to know in which class the override resides.                        -DONE-
  //  - An update function would need to be called both here and in the token free function.

  ZEND_REGISTER_RESOURCE(return_value, token, le_gto_override_token);
}

PHP_FUNCTION(override_revert)
{
  zval *var;
  gto_override_token *token;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &var) == FAILURE) {
    RETURN_FALSE;
  }

  ZEND_FETCH_RESOURCE(token, gto_override_token*, &var, -1, GTO_ORTOKEN_RES_NAME, le_gto_override_token);

  if (gto_revert_override(token) == FAILURE) {
    // @todo: Maybe throw an error here...?
    RETURN_FALSE;
  }

  zend_list_delete(Z_LVAL_P(var));
  RETURN_TRUE;
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



static int gto_inject_override(zend_class_entry *ce, char* function_name, int name_len, zend_function *override, gto_override_token **token_out)
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
  token->override->common.scope = token->function->common.scope;

  if (token->prev = ptoken) {
    ptoken->next = token;
  }

  token->next = NULL;


  if (store_override_token(token) != SUCCESS) {
    efree(token->function_name);
    efree(token->function);
    efree(token->override);
    efree(token);
    *token_out = NULL;

    return FAILURE;
  }


  function_add_ref(token->override);
  memcpy(function, token->override, sizeof(zend_function));

  token->active = 1;
  return SUCCESS;
}

static int gto_revert_override(gto_override_token *token)
{
  zend_function *function;

  if (token && token->active) {
    token->active = 0;

    if (!token->next) {
      HashTable *ftable = token->class ? &(token->class->function_table) : EG(function_table);
      if (zend_hash_find(ftable, token->function_name, token->name_len + 1, (void**) &function) == FAILURE) {
        // Function has already been removed, it seems; likely due to global cleanup.
        return FAILURE;
      }

      // @todo:
      // Figure out how to check if the "active" token is the token we're expecting?

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
