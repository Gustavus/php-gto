--TEST--
call_overridden_func_array()


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = "base";

  class MyClass {
    public static function test($input) {
      return "foo: {$input}";
    }
  }

  printf("Original: %s\n", MyClass::test($input));

  $token = override_method('MyClass', 'test', function($input) use (&$token) { return "bar: " . call_overridden_func($token, null, $input); });
  print "Return type: " . gettype($token) . "\n";
  printf("Override: %s\n", MyClass::test($input));

  $token = null;
  printf("Reverted: %s\n", MyClass::test($input));


--EXPECT--
Original: foo: base
Return type: resource
Override: bar: foo: base
Reverted: foo: base
