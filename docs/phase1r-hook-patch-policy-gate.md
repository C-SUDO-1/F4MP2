# Phase 1R Hook Patch Policy Gate

Iteration 143 adds the first explicit patch policy gate for the
`player_character_update_before_input` prototype.

## Modes

- `dry_run_resolve_only`
  - resolve runtime and addresses
  - report prototype and candidate details
  - do not attempt patching
  - do not register the callback-fed adapter path

- `armed_install`
  - resolve runtime and addresses
  - attempt the patch step when caller policy allows it
  - register the callback-fed adapter path only after install succeeds

## Why this matters

The install path can now distinguish:

1. "the site resolves and looks viable"
2. "the site is armed and should drive live callback flow"

That prevents the project from treating address resolution as if it were already a real installed hook.
