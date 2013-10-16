--TEST--
override_revert() with out-of-order auto reversion


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

  $token2 = null;
  print 'Override 2 Reverted: ' . strrev($input) . "\n";

  $token3 = null;
  print 'Override 3 Reverted: ' . strrev($input) . "\n";

  $token1 = null;
  print 'All Reverted: ' . strrev($input) . "\n";



--EXPECTF--
Original: gnirts tset
Override 1: override1: test string
Override 2: override2: test string
Override 3: override3: test string
Override 2 Reverted: override3: test string
Override 3 Reverted: override1: test string
All Reverted: gnirts tset
