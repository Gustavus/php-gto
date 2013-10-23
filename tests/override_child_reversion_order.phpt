--TEST--
override_revert() with out-of-order reversion on inherited methods


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  class MyClass {
    public function test() {
      return "Foo";
    }
  }

  class MyClassExt extends MyClass {
  }


  $obj = new MyClassExt();

  $tokens = [];
  $ins = [];
  $rem = [];
  $ord = [ '123', '132', '213', '231', '312', '321' ];

  $ins[] = function() use (&$tokens) { $tokens[0] = override_method('MyClass', 'test', function() { return "Parent Bar"; }); };
  $ins[] = function() use (&$tokens) { $tokens[1] = override_method('MyClassExt', 'test', function() { return "Child Bar"; }); };
  $ins[] = function() use (&$tokens) { $tokens[2] = override_method('MyClass', 'test', function() { return 'Parent Bar Mk2'; }); };

  $rem[] = function() use (&$tokens) { override_revert($tokens[0]); };
  $rem[] = function() use (&$tokens) { override_revert($tokens[1]); };
  $rem[] = function() use (&$tokens) { override_revert($tokens[2]); };


  for ($i = 0; $i < count($ord); ++$i) {
    for ($r = 0; $r < count($ord); ++$r) {
      print "==============================\n";
      print "Base: " . $obj->test() . "\n";

      $ins[$ord[$i][0] - 1]();
      print "Override {$ord[$i][0]}: " . $obj->test() . "\n";
      $ins[$ord[$i][1] - 1]();
      print "Override {$ord[$i][1]}: " . $obj->test() . "\n";
      $ins[$ord[$i][2] - 1]();
      print "Override {$ord[$i][2]}: " . $obj->test() . "\n";

      $rem[$ord[$r][0] - 1]();
      print "Reverted {$ord[$r][0]}: " . $obj->test() . "\n";
      $rem[$ord[$r][1] - 1]();
      print "Reverted {$ord[$r][1]}: " . $obj->test() . "\n";
      $rem[$ord[$r][2] - 1]();
      print "Reverted {$ord[$r][2]}: " . $obj->test() . "\n";
    }
  }


--EXPECTF--
==============================
Base: Foo
Override 1: Parent Bar
Override 2: Child Bar
Override 3: Child Bar
Reverted 1: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 2: Child Bar
Override 3: Child Bar
Reverted 1: Child Bar
Reverted 3: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 2: Child Bar
Override 3: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 1: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 2: Child Bar
Override 3: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 2: Child Bar
Override 3: Child Bar
Reverted 3: Child Bar
Reverted 1: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 2: Child Bar
Override 3: Child Bar
Reverted 3: Child Bar
Reverted 2: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 3: Parent Bar Mk2
Override 2: Child Bar
Reverted 1: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 3: Parent Bar Mk2
Override 2: Child Bar
Reverted 1: Child Bar
Reverted 3: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 3: Parent Bar Mk2
Override 2: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 1: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 3: Parent Bar Mk2
Override 2: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 3: Parent Bar Mk2
Override 2: Child Bar
Reverted 3: Child Bar
Reverted 1: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 1: Parent Bar
Override 3: Parent Bar Mk2
Override 2: Child Bar
Reverted 3: Child Bar
Reverted 2: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 1: Child Bar
Override 3: Child Bar
Reverted 1: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 1: Child Bar
Override 3: Child Bar
Reverted 1: Child Bar
Reverted 3: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 1: Child Bar
Override 3: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 1: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 1: Child Bar
Override 3: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 1: Child Bar
Override 3: Child Bar
Reverted 3: Child Bar
Reverted 1: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 1: Child Bar
Override 3: Child Bar
Reverted 3: Child Bar
Reverted 2: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 3: Child Bar
Override 1: Child Bar
Reverted 1: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 3: Child Bar
Override 1: Child Bar
Reverted 1: Child Bar
Reverted 3: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 3: Child Bar
Override 1: Child Bar
Reverted 2: Parent Bar
Reverted 1: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 3: Child Bar
Override 1: Child Bar
Reverted 2: Parent Bar
Reverted 3: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 3: Child Bar
Override 1: Child Bar
Reverted 3: Child Bar
Reverted 1: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 2: Child Bar
Override 3: Child Bar
Override 1: Child Bar
Reverted 3: Child Bar
Reverted 2: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 1: Parent Bar
Override 2: Child Bar
Reverted 1: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 1: Parent Bar
Override 2: Child Bar
Reverted 1: Child Bar
Reverted 3: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 1: Parent Bar
Override 2: Child Bar
Reverted 2: Parent Bar
Reverted 1: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 1: Parent Bar
Override 2: Child Bar
Reverted 2: Parent Bar
Reverted 3: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 1: Parent Bar
Override 2: Child Bar
Reverted 3: Child Bar
Reverted 1: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 1: Parent Bar
Override 2: Child Bar
Reverted 3: Child Bar
Reverted 2: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 2: Child Bar
Override 1: Child Bar
Reverted 1: Child Bar
Reverted 2: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 2: Child Bar
Override 1: Child Bar
Reverted 1: Child Bar
Reverted 3: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 2: Child Bar
Override 1: Child Bar
Reverted 2: Parent Bar
Reverted 1: Parent Bar Mk2
Reverted 3: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 2: Child Bar
Override 1: Child Bar
Reverted 2: Parent Bar
Reverted 3: Parent Bar
Reverted 1: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 2: Child Bar
Override 1: Child Bar
Reverted 3: Child Bar
Reverted 1: Child Bar
Reverted 2: Foo
==============================
Base: Foo
Override 3: Parent Bar Mk2
Override 2: Child Bar
Override 1: Child Bar
Reverted 3: Child Bar
Reverted 2: Parent Bar
Reverted 1: Foo
