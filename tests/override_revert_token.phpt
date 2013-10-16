--TEST--
override_revert() w/override token


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  $input = "test string";

  $closure = function($input) {
    return "not reversed: {$input}";
  };

  print "Original: " . strrev($input) . "\n";

  $token = override_function('strrev', $closure);
  print "Return type: " . gettype($token) . "\n";

  print "Override: " . strrev($input) . "\n";
  override_revert($token);

  print "Reverted: " . strrev($input) . "\n";


--EXPECT--
Original: gnirts tset
Return type: resource
Override: not reversed: test string
Reverted: gnirts tset
