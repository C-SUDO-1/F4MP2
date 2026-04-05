# Phase 1R — real local hook dry-run in Fallout 4

This iteration freezes the first real local hook dry-run target on the live stack observed on the user machine:

- Fallout 4 runtime: `1.11.191`
- F4SE build: `0.7.7`
- Address Library required: yes
- CommonLibF4 resource: yes

The dry-run pass remains **resolve-only**. It does not claim a real detour is installed.

## What it verifies

- toolchain manifest matches the live runtime target
- F4SE build text matches the live loader on disk
- plugin directory exists
- runtime profile resolves for `player_character_update_before_input`
- update and input addresses resolve through the provider
- the stack is ready for the first armed attempt

## Useful local checks

```powershell
(Get-Item ".\Fallout4.exe").VersionInfo.FileVersion
(Get-Item ".\f4se_loader.exe").VersionInfo.FileVersion
Get-ChildItem ".\Data\F4SE\Plugins"
```

`Data\F4SE\f4se.log` may be absent until a launch writes it.
