LIBCLIENT=(libclient.so.*)
PATTERN=(
    "48 8b 44 c1 10 48 85 c0 ?? ?? 44 8b 42 04 89 f2 81 e2 ff 01 00 00"             #code: client entity list selector ptr
    "74 ?? 0f b7 90 ?? 07 00 00 48 89 d0 48 c1 e8 09 83 e0 ?? 48 8b 04 c6 48 85 c0" #code: controller player index access
    "75 ?? 44 89 e6 48 89 df e8 ?? ?? ?? ?? 8b 83 ?? ?? 00 00 89 83 ?? ?? 00 00"    #code: player health access
    "48 8b 85 ?? ?? ?? ff 4c 89 f7 49 89 86 ?? ??"                                  #code: player coordinates 3D vector access
    "89 83 ?? ?? 00 00 48 8b 5d ?? c9 c3 0f 1f 44 00 00 55 48 89 e5 41 56 49 89 f6" #code: player armour access
    "48 8b 38 48 8b 07 ff 90 ?? ?? 00 00 41 89 c5 48 8d 05 ?? ?? ?? ?? 44 89 ee"    #code: view angles base ptr
    "fa ff ff 48 ?? 83 88 ?? ?? ?? ?? 89 85 c8 fa ff ff 0f b6 83 90"                #code: offset for view angles (add 0x8 to it)
)
OUTPUT=(
    "ENTITY LIST SELECTOR PTR"
    "CONTROLLER PLAYER INDEX"
    "PLAYER HEALTH (BOTH)"
    "PLAYER COORDINATES 3D VECTOR"
    "PLAYER ARMOUR"
    "VIEW ANGLES PARENT STRUCT PTR"
    "VIEW ANGLES OFFSET"
)

RED="\033[0;31m"
CYAN="\033[0;36m"
RESET="\033[0m"


echo -e "${RED}\nFILE: ${LIBCLIENT[0]}${RESET}\n"

for i in "${!PATTERN[@]}"; do
    echo -e "${CYAN} --- ${OUTPUT[$i]} ${RESET}"
    psc ${LIBCLIENT[0]} "${PATTERN[$i]}"
    echo ""
done
