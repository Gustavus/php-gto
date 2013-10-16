--TEST--
override_function() w/function reference


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = "test string";

  function myfunction($input) {
    return "not reversed: {$input}";
  }



  printf("Original: %s\n", strrev($input));

  $token = override_function('strrev', 'myfunction');
  print "Return type: " . gettype($token) . "\n";
  printf("Override: %s\n", strrev($input));

  $token = null;
  printf("Reverted: %s\n", strrev($input));


--EXPECT--
Original: gnirts tset
Return type: resource
Override: not reversed: test string
Reverted: gnirts tset
