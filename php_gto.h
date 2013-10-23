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
/**
 * override_function(function, callback)
 *
 * Overrides the specified function with the callback specified.
 *
 * Input:
 *  - (string) function
 *    The fully-qualified name of the function to override.
 *
 *  - (callable) callback
 *    The callback which will be called in place of the original function.
 *
 * Output:
 *  - (resource | null)
 *    An override token representing the override information, or null if the function could not be
 *    overridden for any reason.
 *
 *    The override token returned by this method must be kept in scope to maintain the override. If
 *    the token falls out of scope for any reason, the override will be reverted.
 */
PHP_FUNCTION(override_function);

/**
 * override_method(class, method, callback)
 *
 * Overrides the specified class method or static function with the callback specified.
 *
 * Input:
 *  - (string) class
 *    The fully qualified name of the class containing the method to override
 *
 *  - (string) method
 *    The name of the method or static function to override.
 *
 *  - (callable) callback
 *    The callback which will be called in place of the original method.
 *
 * Output:
 *  - (resource | null)
 *    An override token representing the override information, or null if the method could not be
 *    overridden for any reason.
 *
 *    The override token returned by this method must be kept in scope to maintain the override. If
 *    the token falls out of scope for any reason, the override will be reverted.
 */
PHP_FUNCTION(override_method);

/**
 * override_revert(token)
 *
 * Reverts the override referenced by the specified override token. The given token will be
 * destroyed as the result of a successful call to this function.
 *
 * Input:
 *  - (resource) token
 *    The override token to revert. Must be a valid resource returned by a previous call to
 *    override_function or override_method.
 *
 * Output:
 *  - (boolean)
 *    True if the override represented by the specified token was reverted successfully; false
 *    otherwise.
 */
PHP_FUNCTION(override_revert);

/**
 * call_overridden_func(token, object, params...)
 *
 * Calls the original function or method specified by the given override token and returns the
 * result.
 *
 * Input:
 *  - (resource) token
 *    The override token representing the overridden function/method to call.
 *
 *  - (object) object
 *    The object to use as the context when calling a method. This value is ignored entirely when
 *    the override token represents a global or static function.
 *
 *    When the overriden token represents a method, this value must be an instance of the class in
 *    which the method is defined. That is, if the token represents the method "MyClass.testMethod,"
 *    object must be an instance of MyClass or one of its subclasses.
 *
 *  - (mixed) params...
 *    A variable number of parameters to pass to the overridden function.
 *
 * Output:
 *  - (mixed)
 *    The value returned by the called function/method. If the called function/method has no return
 *    value, this function will return null.
 */
PHP_FUNCTION(call_overridden_func);

/**
 * call_overridden_func_array(token, object, params)
 *
 * Calls the original function or method specified by the given override token and returns the
 * result.
 *
 * Input:
 *  - (resource) token
 *    The override token representing the overridden function/method to call.
 *
 *  - (object) object
 *    The object to use as the context when calling a method. This value is ignored entirely when
 *    the override token represents a global or static function.
 *
 *    When the overriden token represents a method, this value must be an instance of the class in
 *    which the method is defined. That is, if the token represents the method "MyClass.testMethod,"
 *    object must be an instance of MyClass or one of its subclasses.
 *
 *  - (array[mixed]) params
 *    An array containing the parameters to pass to the overridden function.
 *
 * Output:
 *  - (mixed)
 *    The value returned by the called function/method. If the called function/method has no return
 *    value, this function will return null.
 */
PHP_FUNCTION(call_overridden_func_array);


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

static int gto_call_override_token(gto_override_token *token, zval *object, int argc, zval ***argv, zval *retval);