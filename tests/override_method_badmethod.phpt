--TEST--
override_method() w/invalid method


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $token = override_method('StdClass', 'method does not exist', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('StdClass', null, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('StdClass', 123, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('StdClass', 1.23, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('StdClass', true, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('StdClass', false, function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('StdClass', ['nope'], function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('StdClass', new StdClass(), function() { return true; });
  print "Return type: " . gettype($token) . "\n";


  $token = override_method('class does not exist', 'derp', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method(null, 'derp', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method(123, 'derp', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method(1.23, 'derp', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method(true, 'derp', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method(false, 'derp', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method(['StdClass'], 'derp', function() { return true; });
  print "Return type: " . gettype($token) . "\n";

  $token = override_method(new StdClass(), 'derp', function() { return true; });
  print "Return type: " . gettype($token) . "\n";


--EXPECTF--
Return type: boolean
Return type: boolean
Return type: boolean
Return type: boolean
Return type: boolean
Return type: boolean

Warning: override_method() expects parameter 2 to be string, array given in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 2 to be string, object given in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 1 to be a valid class name, 'class does not exist' given in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 1 to be a valid class name, '' given in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 1 to be a valid class name, '123' given in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 1 to be a valid class name, '1.23' given in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 1 to be a valid class name, '1' given in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 1 to be a valid class name, '' given in %s on line %d
Return type: boolean

Notice: Array to string conversion in %s on line %d

Warning: override_method() expects parameter 1 to be a valid class name, 'Array' given in %s on line %d
Return type: boolean

Catchable fatal error: Object of class stdClass could not be converted to string in %s on line %d