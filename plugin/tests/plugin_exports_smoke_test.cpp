#include <cassert>
#include "plugin_exports.h"
#include "commonlibf4_entrypoint_stub.h"

int main() {
    PluginInfoMock info{};
    F4SEInterfaceMock f4se{};
    f4se.runtime_version = 0x010A03D8u;
    f4se.is_editor = 0;
    clf4_entrypoint_init();
    assert(clf4_plugin_query(&f4se, &info));
    assert(clf4_plugin_load(&f4se));
    assert(info.name != nullptr);
    return 0;
}
