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


#define PHP_GTO_EXTNAME "Gustavus Testing Overrides"
#define PHP_GTO_VERSION "1.0.0"


// Data structures!
ZEND_BEGIN_MODULE_GLOBALS(gto)
  HashTable *ftokens;         // Hashtable of override tokens (for functions)
  HashTable *ctokens;         // Hashtable of hashtables of tokens (for classes)
ZEND_END_MODULE_GLOBALS(gto)

typedef struct _gto_override_token gto_override_token;


// Module init prototypes
PHP_MINIT_FUNCTION(gto);


// PHP Functions
PHP_FUNCTION(override_function);
PHP_FUNCTION(override_method);
PHP_FUNCTION(override_revert);


// Initializers & Destructors
static void gto_init_globals(zend_gto_globals *globals);
static void gto_ortoken_dtor(zend_rsrc_list_entry *rsrc   TSRMLS_DC);
static void gto_htentry_dtor(void *data);


// Utility functions/implementations
static gto_override_token* fetch_override_token(zend_class_entry *ce, char* function_name, int name_len);
static int store_override_token(gto_override_token *token);
static int clear_override_token(gto_override_token *token);
static int free_override_token(gto_override_token *token);

static int gto_inject_override(zend_class_entry *ce, char* function_name, int name_len, zend_function *override, int update_children, gto_override_token **token_out);
static int gto_revert_override(gto_override_token *token);

static int gto_update_children(gto_override_token *token);
