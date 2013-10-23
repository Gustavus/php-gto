--TEST--
override_method() w/static function reference


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

  class MyClass {
    public static function handler() {
      print "bar\n";
    }
  }

  (new DummyClass())->test();

  $token = override_method('DummyClass', 'test', 'MyClass::handler');
  print "Return type: " . gettype($token) . "\n";

  (new DummyClass())->test();

  $token = null;
  (new DummyClass())->test();


--EXPECT--
foo
Return type: resource
bar
foo
