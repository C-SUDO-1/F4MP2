# Startup blocking policy

This layer classifies hook-family bring-up outcomes as:
- none
- degradable
- fatal

Policy is mode-aware:
- basic sync treats player/workshop failures as fatal
- vanilla mirror treats player/workshop failures as fatal, actor/dialogue-quest failures as degradable

This allows plugin startup to distinguish:
- fully ready
- partially degraded but loadable
- blocked from startup
