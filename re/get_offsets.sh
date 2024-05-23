LIBCLIENT=(libclient.so.*)
PATTERN=(
    "48 8b 44 c1 10 48 85 c0 ?? ?? 44 8b 42 04 89 f2 81 e2 ff 01 00 00" #code: fetch client entity list selector ptr
    "44 89 ee 48 8b 38 48 8b 07 ff 50 38"                               #code: fetch view angles base structure ptr
    "48 8b 93 ?? ?? 00 00 48 89 83 ?? ?? 00 00 f3 0f 11 4b ??"          #code: offset for view angles (approx. 0x508)
)
OUTPUT=(
    "ENTITY LIST SELECTOR PTR"
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
