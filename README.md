php-gto
=======

PHP 5.4+ extension for temporarily overriding PHP functions/methods at runtime.





## Introduction/About
====================================================================================================

The Gustavus Testing Overrides (GTO) project is a PHP extension we put together to deal with some
difficult situations we found ourselves dealing with during testing. While frameworks like PHPUnit
provide tools to mock class methods, they often are not enough to tackle situations where a test
unit is calling static functions, creating new (temporary) object instances or calling global
functions. In many cases, it's better to simply refactor the afore mentioned test units to eliminate
such problematic, but this isn't always possible or even desirable.

Instead of fighting with our tests and refactoring or wrapping problematic code, we decided that it
was occasionally better to simply override the call during our test. GTO does this by temporarily
modifying PHP's function table for the duration of the test (or, more specifically, the life of an
override token).

A typical override using GTO looks like this:

    $token = override_function('function', function() { return "override!" });

... and that's it. The function will be overridden so long as the resource returned by the
override_function ($token, in the above example) still exists within the current scope. A more
in-depth example can be found further into this document.

It should be noted, however, that while we encourage using this extension during testing, each use
should be carefully scrutinized. In cases where you find yourself using overrides to mock the output
of your own functions or methods, it's likely a sign that your code needs to be refactored. In fact,
about the only truely legitimate use of this extension is to mock the output of 3rd party code that
cannot be easily refactored or wrapped. It should also go without saying that this extension should
never be used in any kind of production environment.  PHP already does enough that can make its
runtime indeterministic, and functionality like that provided by GTO certainly doesn't help things
any.

**tl;dr:** As with any tool, some thought should go into where and when it is used. Arbitrary use of
this extension can lead to a lot of extra (and unnecessary) tracing and debugging.




## Compiling & Installing
====================================================================================================

PHP provides a number of tools which actually makes this step a lot less painful than most would
expect. On Windows, you'll likely need something to process makefiles (we recommend MinGW), but
that's about it.

First and foremost, check PHP's version. We've designed and tested this against PHP 5.4, but there
is nothing in the code that will prevent it from attempting to work on previous versions. That is,
if you're running 5.4 or newer, you're good to go. Anything prior and it may or may not work.

Once you've checked your version, simply drop the source files, tests and the config.m4 file into a
directory somewhere on your file system. In the command line/prompt, navigate to this directory and
run the "phpize" command:

    $> cd /home/crog/php-gto
    $> phpize
    Configuring for:
    PHP Api Version:         20100412
    Zend Module Api No:      20100525
    Zend Extension Api No:   220100525


Once phpize does its thing, you'll need to build the makefile. This is done with the configure
utility that was created during the previous "PHPization" step (Note: on Windows, this will likely
be a batch script).

    $> ./configure
    checking for grep that handles long lines and -e... /bin/grep
    checking for egrep... /bin/grep -E
    checking for a sed that does not truncate output... /bin/sed
    checking for cc... cc
    <9001 other checks>


Finally, after everything is configured, you can compile, and install, GTO. From the same directory,
run the command "make install" (Note: on *nix setups, you'll likely have to run this command as
root):

    $> sudo make install
    <a bunch of compiling and linking>
    ----------------------------------------------------------------------
    Libraries have been installed in:
       /home/crog/php-gto/modules

    If you ever happen to want to link against installed libraries
    in a given directory, LIBDIR, you must either use libtool, and
    specify the full pathname of the library, or use the `-LLIBDIR'
    flag during linking and do at least one of the following:
       - add LIBDIR to the `LD_LIBRARY_PATH' environment variable
         during execution
       - add LIBDIR to the `LD_RUN_PATH' environment variable
         during linking
       - use the `-Wl,-rpath -Wl,LIBDIR' linker flag
       - have your system administrator add LIBDIR to `/etc/ld.so.conf'

    See any operating system documentation about shared libraries for
    more information, such as the ld(1) and ld.so(8) manual pages.
    ----------------------------------------------------------------------
    Installing shared extensions:     <your PHP install's module directory here>


And that's it! You can now load the extension by adding the line "extension=gto.so" to your php.ini,
or by specifying it on the command line (-d "extension=gto.so").

To begin actually using GTO, see the sections below detail a usage example and some documentation
for each function.




## Example Usage
====================================================================================================

PHP Code:

    <?php
      /**
       * Define a callback to handle calls to the strrev function. We could define this inline during
       * the override_function call, but we're doing it here for clarity.
       *
       * Alternatively, we could use any function or method reference here. Any of the following are
       * valid as well:
       *  - 'trim' (references the global function "trim")
       *  - [new MyClass(), 'myMethod']
       *  - 'MyClass::myStaticMethod'
       */
      $callback = function($input) {
        return "input string: {$input}";
      }
  
      /**
       * Override the 'strrev' function and store the token. If we don't hold on to the returned
       * token, the override will be reverted as soon as PHP cleans up the variable (which, in many
       * cases, is immediate).
       */
      $token = override_function('strrev', $callback);
  
      /**
       * Call the strrev function. It should now call the function we defined above instead of the
       * built-in 'strrev' function.
       */
      print strrev('test string');
  
      /**
       * Revert the override so we can call the original function again. We could also let the token
       * fall out of scope (or manually change its value), but we're going to be explicit here.
       */
      override_revert($token);
  
      /**
       * Calling strrev at this point should now do what we expect.
       */
      print strrev('test string');



Outputs:

    input string: test string
    gnirts tset




## Function Reference
====================================================================================================

#### override_function(function, callback)

  Overrides the specified function with the callback specified.


**Input:**
  - (string) *function*  
    The fully-qualified name of the function to override.

  - (callable) *callback*  
    The callback which will be called in place of the original function.

**Output:**
  - (resource | null)  
    An override token representing the override information, or null if the function could not be
    overridden for any reason.

    The override token returned by this method must be kept in scope to maintain the override. If
    the token falls out of scope for any reason, the override will be reverted.

----------------------------------------------------------------------------------------------------

#### override_method(class, method, callback)

  Overrides the specified class method or static function with the callback specified.


**Input:**
  - (string) *class*  
    The fully qualified name of the class containing the method to override

  - (string) *method*  
    The name of the method or static function to override.

  - (callable) *callback*  
    The callback which will be called in place of the original method.

**Output:**
  - (resource | null)  
    An override token representing the override information, or null if the method could not be
    overridden for any reason.

    The override token returned by this method must be kept in scope to maintain the override. If
    the token falls out of scope for any reason, the override will be reverted.

----------------------------------------------------------------------------------------------------

#### override_revert(token)

  Reverts the override referenced by the specified override token. The given token will be
  destroyed as the result of a successful call to this function.


**Input:**
  - (resource) *token*  
    The override token to revert. Must be a valid resource returned by a previous call to
    override_function or override_method.

**Output:**
  - (boolean)  
    True if the override represented by the specified token was reverted successfully; false
    otherwise.

----------------------------------------------------------------------------------------------------

#### call_overridden_func(token, object, params...)

  Calls the original function or method specified by the given override token and returns the
  result.


**Input:**
  - (resource) *token*  
    The override token representing the overridden function/method to call.

  - (object) *object*  
    The object to use as the context when calling a method. This value is ignored entirely when
    the override token represents a global or static function.

    When the overriden token represents a method, this value must be an instance of the class in
    which the method is defined. That is, if the token represents the method "MyClass.testMethod,"
    object must be an instance of MyClass or one of its subclasses.

  - (mixed) *params...*  
    A variable number of parameters to pass to the overridden function.

**Output:**
  - (mixed)  
    The value returned by the called function/method. If the called function/method has no return
    value, this function will return null.

----------------------------------------------------------------------------------------------------

#### call_overridden_func_array(token, object, params)

  Calls the original function or method specified by the given override token and returns the
  result.


**Input:**
  - (resource) *token*  
    The override token representing the overridden function/method to call.

  - (object) *object*  
    The object to use as the context when calling a method. This value is ignored entirely when
    the override token represents a global or static function.

    When the overriden token represents a method, this value must be an instance of the class in
    which the method is defined. That is, if the token represents the method "MyClass.testMethod,"
    object must be an instance of MyClass or one of its subclasses.

  - (array[mixed]) *params*  
    An array containing the parameters to pass to the overridden function.

**Output:**
  - (mixed)  
    The value returned by the called function/method. If the called function/method has no return
    value, this function will return null.
