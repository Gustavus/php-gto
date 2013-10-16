--TEST--
override_revert() w/bad input


--ARGS--
-dextension=gto.so
-dextension=gto.dll


--FILE--
<?php
  override_revert(null);

  override_revert('nope');

  override_revert(123);

  override_revert(1.23);

  override_revert(true);

  override_revert(false);

  override_revert(['nope']);

  override_revert(new StdClass());

  override_revert(STDOUT);


--EXPECTF--
Warning: override_revert(): supplied argument is not a valid GTO Override Token resource in %s on line %d

Warning: override_revert(): supplied argument is not a valid GTO Override Token resource in %s on line %d

Warning: override_revert(): supplied argument is not a valid GTO Override Token resource in %s on line %d

Warning: override_revert(): supplied argument is not a valid GTO Override Token resource in %s on line %d

Warning: override_revert(): supplied argument is not a valid GTO Override Token resource in %s on line %d

Warning: override_revert(): supplied argument is not a valid GTO Override Token resource in %s on line %d

Warning: override_revert(): supplied argument is not a valid GTO Override Token resource in %s on line %d

Warning: override_revert(): supplied argument is not a valid GTO Override Token resource in %s on line %d

Warning: override_revert(): supplied resource is not a valid GTO Override Token resource in %s on line %d
