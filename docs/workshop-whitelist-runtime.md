# Workshop Whitelist Runtime Loading

The service now supports runtime loading of the workshop whitelist from the JSON schema file in `shared/schemas/allowed_workshop_forms.json`.

## Behavior
- Default startup still seeds a built-in placeholder whitelist.
- A runtime JSON load replaces the in-memory list.
- The loader is intentionally narrow and expects the current schema shape.
- Placeholder form IDs can be detected via `workshop_whitelist_has_placeholder_ids()`.

## Why this matters
This moves the project closer to real vanilla FO4 integration because the multiplayer service no longer needs to be recompiled to replace placeholder workshop form IDs.

## Next step
Replace the placeholder IDs in the schema with extracted vanilla FO4 workshop forms and load them on service startup.
