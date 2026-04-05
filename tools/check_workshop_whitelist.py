#!/usr/bin/env python3
import json
import sys
from pathlib import Path

PLACEHOLDERS = {
    "0xAAAAAAAA", "0xAAAAAAAB", "0xBBBBBBBB", "0xCCCCCCCC",
    "0xDDDDDDDD", "0xEEEEEEEE", "0xFFFFFFFF"
}

def main(path_str: str) -> int:
    path = Path(path_str)
    data = json.loads(path.read_text())
    found = []
    for cat in data.get("categories", []):
        for item in cat.get("items", []):
            if item.get("form_id_hex") in PLACEHOLDERS:
                found.append((item.get("editor_id"), item.get("form_id_hex")))
    if found:
        print("placeholder workshop form IDs still present:")
        for editor_id, form_id in found:
            print(f"  {editor_id}: {form_id}")
        return 1
    print("no placeholder workshop form IDs detected")
    return 0

if __name__ == "__main__":
    target = sys.argv[1] if len(sys.argv) > 1 else "shared/schemas/allowed_workshop_forms.json"
    raise SystemExit(main(target))
