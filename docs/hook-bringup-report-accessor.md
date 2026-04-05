# Hook Bring-up Report Accessor

This accessor stores the last family-by-family hook bring-up attempt results.

It complements the plugin startup report:
- startup report: mode-level readiness and blocker counts
- bring-up report: per-family attempt outcomes

Intended use:
- smoke tests
- future diagnostics UI/logging
- future CommonLibF4/F4SE bring-up debugging
