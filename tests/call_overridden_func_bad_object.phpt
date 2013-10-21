--TEST--
call_overridden_func_array() on method with wrong instance


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = "base";

  class MyClass {
    public function test($input) {
      return "foo: {$input}";
    }
  }

  $obj = new MyClass();

  printf("Original: %s\n", $obj->test($input));

  $token = override_method('MyClass', 'test', function($input) use (&$token) { return "bar: " . call_overridden_func($token, new StdClass(), $input); });
  print "Return type: " . gettype($token) . "\n";
  printf("Override: %s\n", $obj->test($input));


--EXPECTF--
Original: foo: base
Return type: resource

Fatal error: call_overridden_func(): The object provided must be an instance of the class "MyClass" to call this override.
 in %s on line %d
