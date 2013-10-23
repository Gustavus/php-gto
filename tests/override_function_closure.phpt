--TEST--
override_function() w/closure


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = "test string";
  $closure = function($input) { return "not reversed: {$input}"; };

  printf("Original: %s\n", strrev($input));

  $token = override_function('strrev', $closure);
  print "Return type: " . gettype($token) . "\n";
  printf("Override: %s\n", strrev($input));

  $token = null;
  printf("Reverted: %s\n", strrev($input));


--EXPECT--
Original: gnirts tset
Return type: resource
Override: not reversed: test string
Reverted: gnirts tset
