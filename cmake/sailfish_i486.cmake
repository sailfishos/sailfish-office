set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_VERSION 1)

set(CMAKE_C_COMPILER $ENV{HOME}/SailfishOS/share/qtcreator/MerProject/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/gcc)
set(CMAKE_CXX_COMPILER $ENV{HOME}/SailfishOS/share/qtcreator/MerProject/mer-sdk-tools/MerSDK/SailfishOS-i486-x86/gcc)

set(CMAKE_FIND_ROOT_PATH $ENV{HOME}/SailfishOS/mersdk/targets/SailfishOS-i486-x86/)
#link_directories($ENV{HOME}/SailfishOS/mersdk/targets/SailfishOS-i486-x86/usr/lib/)

set(QT_QMAKE_EXECUTABLE $ENV{HOME}/SailfishOS/mersdk/targets/SailfishOS-i486-x86/usr/bin/qmake)
