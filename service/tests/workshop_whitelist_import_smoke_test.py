#!/usr/bin/env python3
import json, subprocess, sys
from pathlib import Path

root = Path(__file__).resolve().parents[2]
script = root / 'tools' / 'import_workshop_whitelist.py'
source = root / 'tools' / 'sample_workshop_forms.tsv'
out = root / 'build' / 'imported_workshop_forms.json'
subprocess.check_call([sys.executable, str(script), str(source), str(out)])
payload = json.loads(out.read_text(encoding='utf-8'))
assert payload['version'] == 1
assert len(payload['categories']) == 3
assert payload['categories'][0]['items'][0]['form_id_hex'] == '0x00112233'
print('ok')
