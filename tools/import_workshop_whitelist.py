#!/usr/bin/env python3
import csv, json, sys
from pathlib import Path

USAGE = "usage: import_workshop_whitelist.py <input.tsv> <output.json>"

def main():
    if len(sys.argv) != 3:
        print(USAGE, file=sys.stderr)
        return 2
    inp = Path(sys.argv[1])
    out = Path(sys.argv[2])
    categories = {}
    with inp.open('r', encoding='utf-8', newline='') as f:
        reader = csv.DictReader(f, delimiter='\t')
        for row in reader:
            cat = row['category'].strip()
            item = {
                'editor_id': row['editor_id'].strip(),
                'form_id_hex': row['form_id_hex'].strip(),
                'movable': row.get('movable', 'true').strip().lower() == 'true',
                'scrappable': row.get('scrappable', 'true').strip().lower() == 'true',
                'snap_enabled': row.get('snap_enabled', 'true').strip().lower() == 'true',
                'animated': row.get('animated', 'false').strip().lower() == 'true',
                'scripted': row.get('scripted', 'false').strip().lower() == 'true',
            }
            categories.setdefault(cat, []).append(item)
    payload = {'version': 1, 'notes': 'Generated from TSV import', 'categories': []}
    for name, items in categories.items():
        payload['categories'].append({'name': name, 'items': items})
    out.write_text(json.dumps(payload, indent=2) + '\n', encoding='utf-8')
    return 0

if __name__ == '__main__':
    raise SystemExit(main())
