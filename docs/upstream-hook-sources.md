# Upstream Hook Sources (Next-Gen oriented)

This project should align with currently active Fallout 4 plugin development patterns instead of older ad hoc F4SE-only layouts.

## Practical upstream lines

### 1. CommonLibF4 template
- Use a thin plugin shell and keep most gameplay/network logic outside the engine-facing layer.
- Current public template line uses XMake and modern C++ for plugin development.
- Good fit for a future plugin bridge target.

### 2. CommonLibF4 library
- Treat CommonLibF4 as the static code-facing layer.
- F4SE runtime is still required for actual Fallout 4 execution.
- Good fit for type-safe bridge shims and ingress observers.

### 3. F4SE official runtime/SDK line
- Still the canonical runtime dependency.
- Useful for compatibility assumptions and loader/runtime constraints.
- This project should assume F4SE runtime remains mandatory even if CommonLibF4 is used as the static development library.

### 4. Next-Gen plugin example lines
- Useful for plugin bootstrap patterns.
- Also useful for Papyrus/Quest-script initiated native calls, especially for safe host-controlled vanilla event ingress.

## Design consequence for this scaffold

The right long-term plugin path is:
- CommonLibF4/NG-style thin plugin layer
- F4SE runtime dependency
- Papyrus/quest-script bridge only where safe and necessary
- keep multiplayer protocol, authority, persistence, and session logic outside the plugin

## What not to copy from older MP attempts
- Do not push major multiplayer logic into the DLL/plugin layer
- Do not make FO4 hooks the source of authority decisions
- Do not invent custom multiplayer gameplay systems when vanilla events can be mirrored instead
