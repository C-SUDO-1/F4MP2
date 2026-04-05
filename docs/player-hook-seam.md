# Player Hook Seam

This seam translates a future FO4/CommonLibF4 player update observation into a protocol-native `MsgPlayerState`.

The goal is to keep engine-specific observation separate from replication structures so the external service and plugin runtime remain stable when hook details change.
