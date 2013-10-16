--TEST--
override_function() w/static function reference


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = "test string";

  class MyClass {
    public static function myStaticFunction($input) {
      return "not reversed: {$input}";
    }
  }



  printf("Original: %s\n", strrev($input));

  $instance = new MyClass();
  $token = override_function('strrev', [$instance, 'myStaticFunction']);
  print "Return type: " . gettype($token) . "\n";
  printf("Override: %s\n", strrev($input));

  $token = null;
  printf("Reverted: %s\n", strrev($input));


--EXPECT--
Original: gnirts tset
Return type: resource
Override: not reversed: test string
Reverted: gnirts tset
