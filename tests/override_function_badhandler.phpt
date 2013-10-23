--TEST--
override_function() w/invalid handler


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $token = override_function('strrev', 'nope!');
  print "Return type: " . gettype($token) . "\n";

  $token = override_function('strrev', [null, 'nope!']);
  print "Return type: " . gettype($token) . "\n";

  $token = override_function('strrev', new StdClass());
  print "Return type: " . gettype($token) . "\n";


--EXPECTF--
Warning: override_function() expects parameter 2 to be a valid callback, function '%s' not found or invalid function name in %s on line %d
Return type: boolean

Warning: override_function() expects parameter 2 to be a valid callback, first array member is not a valid class name or object in %s on line %d
Return type: boolean

Warning: override_function() expects parameter 2 to be a valid callback, no array or string given in %s on line %d
Return type: boolean