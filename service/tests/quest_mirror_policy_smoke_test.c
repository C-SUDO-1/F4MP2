#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "quest_mirror_policy.h"

int main(void) {
    SessionState s;
    MsgQuestUpdateHeader h;
    MsgQuestObjectiveRecord obj[2];
    QuestMirrorPolicyResult r;

    memset(&s, 0, sizeof(s));
    s.rules = RULE_QUEST_MIRROR_ENABLED;

    memset(&h, 0, sizeof(h));
    h.quest_id = 101;
    h.stage = 10;
    h.tracked = 1;
    h.objective_count = 1;
    memset(obj, 0, sizeof(obj));
    obj[0].objective_id = 5001;
    obj[0].state = QOBJ_ACTIVE;

    assert(qmp_validate_update(&s, &h, obj, &r));
    assert(r.accepted);

    s.quest.loaded = true;
    s.quest.quest_id = 101;
    s.quest.stage = 20;
    s.quest.objective_count = 1;
    s.quest.objectives[0].objective_id = 5001;
    s.quest.objectives[0].state = QOBJ_COMPLETED;

    h.stage = 19;
    assert(!qmp_validate_update(&s, &h, obj, &r));
    assert(r.reason_code == 6);

    h.stage = 21;
    obj[0].state = QOBJ_ACTIVE;
    assert(!qmp_validate_update(&s, &h, obj, &r));
    assert(r.reason_code == 7);

    obj[0].state = QOBJ_COMPLETED;
    assert(qmp_validate_update(&s, &h, obj, &r));

    puts("quest_mirror_policy_smoke_test: ok");
    return 0;
}
