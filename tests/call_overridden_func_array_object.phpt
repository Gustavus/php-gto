--TEST--
call_overridden_func_array()


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

  $token = override_method('MyClass', 'test', function($input) use (&$token) { return "bar: " . call_overridden_func_array($token, $this, [$input]); });
  print "Return type: " . gettype($token) . "\n";
  printf("Override: %s\n", $obj->test($input));

  $token = null;
  printf("Reverted: %s\n", $obj->test($input));


--EXPECT--
Original: foo: base
Return type: resource
Override: bar: foo: base
Reverted: foo: base
