Windows portability fix for local MSVC builds

Changes included in this package:
- Removed C++-style enum underlying-type syntax from shared protocol headers (pre-existing local fix)
- Added Windows Winsock support to service/src/socket_transport.h and .c
- Updated plugin/src/plugin_transport_client.c to use Windows socket/select headers under _WIN32
- Linked ws2_32 for Windows builds in CMakeLists.txt
- Updated service/src/service_main.c to avoid unistd.h/usleep on Windows
- Included docs/pre-fix-local-build-powershell.txt as the pre-fix local build reference
