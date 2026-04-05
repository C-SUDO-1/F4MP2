# Workshop hook seam

Vanilla-first workshop ingress should translate engine-facing workshop events into protocol-native object lifecycle messages.

Current scaffold support:
- host workshop event translation helper
- object spawn/update/despawn translation
- workshop hook shim feeding FO4 bridge ingress

This is intended as the first concrete host-event seam because workshop events are more bounded than quest/script state.
