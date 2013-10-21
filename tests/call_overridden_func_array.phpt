--TEST--
call_overridden_func_array()


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = "test string";

  printf("Original: %s\n", strrev($input));

  $token = override_function('strrev', function($input) use (&$token) { return "reversed: " . call_overridden_func_array($token, null, [$input]); });
  print "Return type: " . gettype($token) . "\n";
  printf("Override: %s\n", strrev($input));

  $token = null;
  printf("Reverted: %s\n", strrev($input));


--EXPECT--
Original: gnirts tset
Return type: resource
Override: reversed: gnirts tset
Reverted: gnirts tset
