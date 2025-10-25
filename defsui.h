#ifndef DEFSUI_H
#define DEFSUI_H

#define UPDATE_CHECK_URL                    "https://raw.githubusercontent.com/surhh/nextblock/main/_lastversion.txt"
#define UPDATE_DOWNLOAD_URL                 "https://github.com/surhh/nextblock/releases/latest"

#define TOSTR(x)                            #x
#define TOSTRING(x)                         TOSTR(x)

#define NEXTBLOCK_APP_NAME                  "NextBlock"
#define NEXTBLOCK_MAJOR_VERSION             1
#define NEXTBLOCK_MINOR_VERSION             0

#define NEXTBLOCK_VERSION                   TOSTRING(NEXTBLOCK_MAJOR_VERSION) "." TOSTRING(NEXTBLOCK_MINOR_VERSION)
#define NEXTBLOCK_VERSION_INT               TOSTRING(NEXTBLOCK_MAJOR_VERSION)TOSTRING(NEXTBLOCK_MINOR_VERSION)
#define NEXTBLOCK_APP_FULL_NAME             NEXTBLOCK_APP_NAME " " NEXTBLOCK_VERSION

#define APP_NAME_VERSION_1                  "<html><head/><body><p><span style=\" font-size:16pt; font-weight:600;\">"
#define APP_NAME_VERSION_2                  "</span></p></body></html>"

#endif // DEFSUI_H
