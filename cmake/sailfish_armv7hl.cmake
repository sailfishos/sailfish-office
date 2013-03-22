set(SAILFISH_SCRATCHBOX 1)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER /opt/cross/bin/armv7hl-meego-linux-gnueabi-gcc)
#$ENV{HOME}/SailfishOS/share/qtcreator/MerProject/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/gcc)
set(CMAKE_CXX_COMPILER /opt/cross/bin/armv7hl-meego-linux-gnueabi-g++)
#$ENV{HOME}/SailfishOS/share/qtcreator/MerProject/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/gcc)

set(CMAKE_FIND_ROOT_PATH $ENV{HOME}/SailfishOS/mersdk/targets/SailfishOS-armv7hl/)

set(QT_QMAKE_EXECUTABLE $ENV{HOME}/SailfishOS/mersdk/targets/SailfishOS-armv7hl/usr/bin/qmake)

add_custom_target(deploy COMMENT "Deploying to emulator...")
add_custom_command(TARGET deploy POST_BUILD COMMAND mkdir ARGS -p deploy)
add_custom_command(TARGET deploy POST_BUILD COMMAND make ARGS DESTDIR=deploy install)
add_custom_command(TARGET deploy POST_BUILD COMMAND scp ARGS -r -P 2223 -i $ENV{HOME}/.ssh/mer-qt-creator-rsa deploy/* root@localhost:/)
add_custom_command(TARGET deploy POST_BUILD COMMAND rm ARGS -rf deploy)
add_dependencies(deploy all)

add_custom_target(run COMMENT "Running application...")
add_custom_command(TARGET run POST_BUILD COMMAND ssh ARGS -p 2223 -i $ENV{HOME}/.ssh/mer-qt-creator-rsa root@localhost "DISPLAY=:0 /usr/bin/invoker --type=j -s /opt/sdk/bin/documents")
add_dependencies(run deploy)
