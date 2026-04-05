# Integrity check notes against current hook patterns

Current alignment target:
- F4SE runtime remains required
- CommonLibF4/CommonLibF4 template style thin plugin entrypoint remains the correct direction
- engine-facing seams should stay thin and feed the canonical live hook pipeline

Near-term integrity goal:
- swap provider-backed local observation for a real hook-backed provider
- swap proxy backend stub for a real in-game actor backend
- keep the service/protocol path unchanged while doing so
