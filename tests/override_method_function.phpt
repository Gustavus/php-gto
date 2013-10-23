--TEST--
override_method() w/function


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  class DummyClass {
    public function test() {
      print "foo\n";
    }
  }

  $closure = function() { print "bar\n"; };

  (new DummyClass())->test();

  $token = override_method('DummyClass', 'test', $closure);
  print "Return type: " . gettype($token) . "\n";

  (new DummyClass())->test();

  $token = null;
  (new DummyClass())->test();


--EXPECT--
foo
Return type: resource
bar
foo
