set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER $ENV{HOME}/SailfishOS/share/qtcreator/MerProject/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/gcc)
set(CMAKE_CXX_COMPILER $ENV{HOME}/SailfishOS/share/qtcreator/MerProject/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/gcc)

set(CMAKE_FIND_ROOT_PATH $ENV{HOME}/SailfishOS/mersdk/targets/SailfishOS-i486-x86/)

set(QT_QMAKE_EXECUTABLE $ENV{HOME}/SailfishOS/mersdk/targets/SailfishOS-i486-x86/usr/bin/qmake)

add_custom_target(deploy COMMENT "Deploying to emulator...")
add_custom_command(TARGET deploy POST_BUILD COMMAND mkdir ARGS -p deploy)
add_custom_command(TARGET deploy POST_BUILD COMMAND make ARGS DESTDIR=deploy install)
add_custom_command(TARGET deploy POST_BUILD COMMAND scp ARGS -r -P 2223 -i $ENV{HOME}/.ssh/mer-qt-creator-rsa deploy/* root@localhost:/)
add_custom_command(TARGET deploy POST_BUILD COMMAND rm ARGS -rf deploy)
add_custom_command(TARGET deploy POST_BUILD COMMAND ssh ARGS -p 2223 -i $ENV{HOME}/.ssh/mer-qt-creator-rsa root@localhost "DISPLAY=:0 /usr/bin/invoker --type=j -s /opt/sdk/bin/documents")