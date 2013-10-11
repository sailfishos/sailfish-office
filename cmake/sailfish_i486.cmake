set(SAILFISH_SCRATCHBOX 1)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER $ENV{HOME}/.config/SailfishAlpha/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/gcc)
set(CMAKE_CXX_COMPILER $ENV{HOME}/.config/SailfishAlpha/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/gcc)

set(CMAKE_FIND_ROOT_PATH $ENV{HOME}/SailfishOS/mersdk/targets/SailfishOS-i486-x86/)

set(QT_QMAKE_EXECUTABLE $ENV{HOME}/.config/SailfishAlpha/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/qmake)

add_custom_target(deploy COMMENT "Deploying to emulator...")
add_custom_command(TARGET deploy POST_BUILD COMMAND mkdir ARGS -p deploy)
add_custom_command(TARGET deploy POST_BUILD COMMAND make ARGS DESTDIR=deploy install)
add_custom_command(TARGET deploy POST_BUILD COMMAND scp ARGS -r -P 2223 -i $ENV{HOME}/SailfishOS/vmshare/ssh/private_keys/1/root deploy/* root@localhost:/)
add_custom_command(TARGET deploy POST_BUILD COMMAND rm ARGS -rf deploy)
add_dependencies(deploy all)

add_custom_target(run COMMENT "Running application...")
add_custom_command(TARGET run POST_BUILD COMMAND ssh ARGS -p 2223 -i $ENV{HOME}/SailfishOS/vmshare/ssh/private_keys/1/nemo nemo@localhost "/usr/bin/invoker --type=j -s /usr/bin/sailfish-office")
add_dependencies(run deploy)

add_custom_target(debug COMMENT "Debugging application...")
add_custom_command(TARGET debug POST_BUILD COMMAND ssh ARGS -p 2223 -i $ENV{HOME}/SailfishOS/vmshare/ssh/private_keys/1/nemo nemo@localhost 'gdb -ex "run" /usr/bin/sailfish-office')
add_dependencies(debug deploy)
