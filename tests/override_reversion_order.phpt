--TEST--
override_revert() with out-of-order reversion


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = 'test string';

  print 'Original: ' . strrev($input) . "\n";

  $token1 = override_function('strrev', function($input) { return 'override1: ' . $input; });
  print 'Override 1: ' . strrev($input) . "\n";

  $token2 = override_function('strrev', function($input) { return 'override2: ' . $input; });
  print 'Override 2: ' . strrev($input) . "\n";

  $token3 = override_function('strrev', function($input) { return 'override3: ' . $input; });
  print 'Override 3: ' . strrev($input) . "\n";

  override_revert($token2);
  print 'Override 2 Reverted: ' . strrev($input) . "\n";

  override_revert($token3);
  print 'Override 3 Reverted: ' . strrev($input) . "\n";

  override_revert($token1);
  print 'All Reverted: ' . strrev($input) . "\n";



--EXPECTF--
Original: gnirts tset
Override 1: override1: test string
Override 2: override2: test string
Override 3: override3: test string
Override 2 Reverted: override3: test string
Override 3 Reverted: override1: test string
All Reverted: gnirts tset
