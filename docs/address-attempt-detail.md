# Address Attempt Detail

This seam extends hook bring-up reporting beyond binary success/failure by tracking:
- required address count
- resolved address count
- whether a patch/install step was attempted
- whether that step succeeded

It is currently populated by provider-backed family attempt stubs and exposed through the existing hook bring-up report accessor.
