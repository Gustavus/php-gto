--TEST--
override_revert() with out-of-order reversion


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = 'test string';

  $tokens = [];
  $ins = [];
  $rem = [];
  $ord = [ '123', '132', '213', '231', '312', '321' ];

  $ins[] = function() use (&$tokens) { $tokens[0] = override_function('strrev', function($input) { return 'override1: ' . $input; }); };
  $ins[] = function() use (&$tokens) { $tokens[1] = override_function('strrev', function($input) { return 'override2: ' . $input; }); };
  $ins[] = function() use (&$tokens) { $tokens[2] = override_function('strrev', function($input) { return 'override3: ' . $input; }); };

  $rem[] = function() use (&$tokens) { override_revert($tokens[0]); };
  $rem[] = function() use (&$tokens) { override_revert($tokens[1]); };
  $rem[] = function() use (&$tokens) { override_revert($tokens[2]); };

  for ($i = 0; $i < count($ord); ++$i) {
    for ($r = 0; $r < count($ord); ++$r) {
      print "==============================\n";
      print "Base: " . strrev($input) . "\n";

      $ins[$ord[$i][0] - 1]();
      print "Override {$ord[$i][0]}: " . strrev($input) . "\n";
      $ins[$ord[$i][1] - 1]();
      print "Override {$ord[$i][1]}: " . strrev($input) . "\n";
      $ins[$ord[$i][2] - 1]();
      print "Override {$ord[$i][2]}: " . strrev($input) . "\n";

      $rem[$ord[$r][0] - 1]();
      print "Reverted {$ord[$r][0]}: " . strrev($input) . "\n";
      $rem[$ord[$r][1] - 1]();
      print "Reverted {$ord[$r][1]}: " . strrev($input) . "\n";
      $rem[$ord[$r][2] - 1]();
      print "Reverted {$ord[$r][2]}: " . strrev($input) . "\n";
    }
  }



--EXPECTF--
==============================
Base: gnirts tset
Override 1: override1: test string
Override 2: override2: test string
Override 3: override3: test string
Reverted 1: override3: test string
Reverted 2: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 2: override2: test string
Override 3: override3: test string
Reverted 1: override3: test string
Reverted 3: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 2: override2: test string
Override 3: override3: test string
Reverted 2: override3: test string
Reverted 1: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 2: override2: test string
Override 3: override3: test string
Reverted 2: override3: test string
Reverted 3: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 2: override2: test string
Override 3: override3: test string
Reverted 3: override2: test string
Reverted 1: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 2: override2: test string
Override 3: override3: test string
Reverted 3: override2: test string
Reverted 2: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 3: override3: test string
Override 2: override2: test string
Reverted 1: override2: test string
Reverted 2: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 3: override3: test string
Override 2: override2: test string
Reverted 1: override2: test string
Reverted 3: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 3: override3: test string
Override 2: override2: test string
Reverted 2: override3: test string
Reverted 1: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 3: override3: test string
Override 2: override2: test string
Reverted 2: override3: test string
Reverted 3: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 3: override3: test string
Override 2: override2: test string
Reverted 3: override2: test string
Reverted 1: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 1: override1: test string
Override 3: override3: test string
Override 2: override2: test string
Reverted 3: override2: test string
Reverted 2: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 1: override1: test string
Override 3: override3: test string
Reverted 1: override3: test string
Reverted 2: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 1: override1: test string
Override 3: override3: test string
Reverted 1: override3: test string
Reverted 3: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 1: override1: test string
Override 3: override3: test string
Reverted 2: override3: test string
Reverted 1: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 1: override1: test string
Override 3: override3: test string
Reverted 2: override3: test string
Reverted 3: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 1: override1: test string
Override 3: override3: test string
Reverted 3: override1: test string
Reverted 1: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 1: override1: test string
Override 3: override3: test string
Reverted 3: override1: test string
Reverted 2: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 3: override3: test string
Override 1: override1: test string
Reverted 1: override3: test string
Reverted 2: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 3: override3: test string
Override 1: override1: test string
Reverted 1: override3: test string
Reverted 3: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 3: override3: test string
Override 1: override1: test string
Reverted 2: override1: test string
Reverted 1: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 3: override3: test string
Override 1: override1: test string
Reverted 2: override1: test string
Reverted 3: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 3: override3: test string
Override 1: override1: test string
Reverted 3: override1: test string
Reverted 1: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 2: override2: test string
Override 3: override3: test string
Override 1: override1: test string
Reverted 3: override1: test string
Reverted 2: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 1: override1: test string
Override 2: override2: test string
Reverted 1: override2: test string
Reverted 2: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 1: override1: test string
Override 2: override2: test string
Reverted 1: override2: test string
Reverted 3: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 1: override1: test string
Override 2: override2: test string
Reverted 2: override1: test string
Reverted 1: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 1: override1: test string
Override 2: override2: test string
Reverted 2: override1: test string
Reverted 3: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 1: override1: test string
Override 2: override2: test string
Reverted 3: override2: test string
Reverted 1: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 1: override1: test string
Override 2: override2: test string
Reverted 3: override2: test string
Reverted 2: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 2: override2: test string
Override 1: override1: test string
Reverted 1: override2: test string
Reverted 2: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 2: override2: test string
Override 1: override1: test string
Reverted 1: override2: test string
Reverted 3: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 2: override2: test string
Override 1: override1: test string
Reverted 2: override1: test string
Reverted 1: override3: test string
Reverted 3: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 2: override2: test string
Override 1: override1: test string
Reverted 2: override1: test string
Reverted 3: override1: test string
Reverted 1: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 2: override2: test string
Override 1: override1: test string
Reverted 3: override1: test string
Reverted 1: override2: test string
Reverted 2: gnirts tset
==============================
Base: gnirts tset
Override 3: override3: test string
Override 2: override2: test string
Override 1: override1: test string
Reverted 3: override1: test string
Reverted 2: override1: test string
Reverted 1: gnirts tset
