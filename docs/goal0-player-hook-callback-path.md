# Goal 0 player hook callback path

This iteration adds a CommonLibF4-style callback registration stub for the player hook family.

Purpose:
- model a real hook callback invoking project code, rather than directly pushing synthetic samples into the observer bridge
- keep Goal 0 focused on local movement capture for two visible moving players

Path:
1. future FO4/CommonLibF4 hook callback fires
2. callback stub emits `CommonLibF4PlayerHookArgs`
3. registered callback forwards into `clf4_phob_submit(...)`
4. observer provider captures through the existing bridge/provider seam
5. movement sync bridge can serialize and relay the movement state
