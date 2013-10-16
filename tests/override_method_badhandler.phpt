--TEST--
override_method() w/invalid handler


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  class MyClass {
    public function test() {
      print "bar\n";
    }
  }

  $token = override_method('MyClass', 'test', 'nope!');
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('MyClass', 'test', [null, 'nope!']);
  print "Return type: " . gettype($token) . "\n";

  $token = override_method('MyClass', 'test', new StdClass());
  print "Return type: " . gettype($token) . "\n";


--EXPECTF--
Warning: override_method() expects parameter 3 to be a valid callback, function '%s' not found or invalid function name in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 3 to be a valid callback, first array member is not a valid class name or object in %s on line %d
Return type: boolean

Warning: override_method() expects parameter 3 to be a valid callback, no array or string given in %s on line %d
Return type: boolean