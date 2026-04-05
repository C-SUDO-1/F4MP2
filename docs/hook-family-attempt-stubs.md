# Hook Family Attempt Stubs

The plugin scaffold now has family-specific attempt stubs for:
- player hooks
- actor hooks
- workshop hooks

These remain synthetic, but they let bring-up exercise:
- unsupported runtime failures
- address-missing partial installs
- dependency-missing failures
- patch rejection failures

The CommonLibF4 hook adapter uses these stubs when no explicit install plan is injected, which makes plugin load closer to a realistic startup path.
