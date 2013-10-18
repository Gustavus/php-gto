--TEST--
override_method() w/child class updating and reversion


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  class MyClass {
    public function test() {
      print "My class name: " . get_class() . ".\n";
      print "My parent class: " . get_parent_class() . ".\n";
    }

    public static function testStatic() {
      print "My called class: " . get_called_class() . ".\n";
    }
  }

  class MyClassExt extends MyClass {
  }



  $obj = new MyClass();
  $ext = new MyClassExt();

  $obj->test();
  MyClass::testStatic();

  $ext->test();
  MyClassExt::testStatic();

  print "-----\n";

  $token1 = override_method('MyClass', 'test', function() {
      print "Override class name: " . get_class() . ".\n";
      print "Override parent class: " . get_parent_class() . ".\n";
  });

  $token2 = override_method('MyClass', 'testStatic', function() {
      print "Override called class: " . get_called_class() . ".\n";
  });

  $obj->test();
  MyClass::testStatic();

  $ext->test();
  MyClassExt::testStatic();

  print "-----\n";

  $token1 = null;
  $token2 = null;

  $obj->test();
  MyClass::testStatic();

  $ext->test();
  MyClassExt::testStatic();


  print "-----\n";

  $token1 = override_method('MyClassExt', 'test', function() {
      print "Override class name: " . get_class() . ".\n";
      print "Override parent class: " . get_parent_class() . ".\n";
  });

  $token2 = override_method('MyClassExt', 'testStatic', function() {
      print "Override called class: " . get_called_class() . ".\n";
  });

  $obj->test();
  MyClass::testStatic();

  $ext->test();
  MyClassExt::testStatic();


  print "-----\n";

  $token1 = null;
  $token2 = null;

  $obj->test();
  MyClass::testStatic();

  $ext->test();
  MyClassExt::testStatic();



--EXPECTF--
My class name: MyClass.
My parent class: .
My called class: MyClass.
My class name: MyClass.
My parent class: .
My called class: MyClassExt.
-----
Override class name: MyClass.
Override parent class: .
Override called class: MyClass.
Override class name: MyClass.
Override parent class: .
Override called class: MyClassExt.
-----
My class name: MyClass.
My parent class: .
My called class: MyClass.
My class name: MyClass.
My parent class: .
My called class: MyClassExt.
-----
My class name: MyClass.
My parent class: .
My called class: MyClass.
Override class name: MyClassExt.
Override parent class: MyClass.
Override called class: MyClassExt.
-----
My class name: MyClass.
My parent class: .
My called class: MyClass.
My class name: MyClass.
My parent class: .
My called class: MyClassExt.
