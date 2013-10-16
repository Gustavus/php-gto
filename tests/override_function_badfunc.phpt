--TEST--
override_function() w/invalid function


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $token = override_function('this_function_does_not_exist_123asjkjasf', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_function(null, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_function(123, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_function(1.23, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_function(true, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_function(false, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_function(['strrev'], function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_function(new StdClass(), function() { return true; });
  print "Return type: " . gettype($token) . "\n";


--EXPECTF--
Return type: boolean
Return type: boolean
Return type: boolean
Return type: boolean
Return type: boolean
Return type: boolean

Warning: override_function() expects parameter 1 to be string, array given in %s on line %d
Return type: boolean

Warning: override_function() expects parameter 1 to be string, object given in %s on line %d
Return type: boolean
