#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "workshop_whitelist.h"

int main(void) {
    const char* path = "/tmp/f4mp_whitelist_test.json";
    FILE* f = fopen(path, "wb");
    assert(f != NULL);
    fputs("{\n  \"version\":1,\n  \"categories\":[{\n    \"name\":\"walls\",\n    \"items\":[{\n      \"editor_id\":\"WoodWall01\",\n      \"form_id_hex\":\"0x00112233\",\n      \"movable\":true,\n      \"scrappable\":true,\n      \"snap_enabled\":true,\n      \"animated\":false,\n      \"scripted\":false\n    }]}]}\n", f);
    fclose(f);

    workshop_whitelist_init_default();
    assert(workshop_whitelist_has_placeholder_ids());
    assert(workshop_whitelist_load_json_file(path));
    assert(workshop_whitelist_count() == 1);
    assert(workshop_whitelist_contains(0x00112233u));
    assert(!workshop_whitelist_has_placeholder_ids());
    remove(path);
    return 0;
}
