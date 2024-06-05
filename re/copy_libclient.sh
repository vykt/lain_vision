CLIENT_PATH="${HOME}/.steam/steam/steamapps/common/Counter-Strike Global Offensive/game/csgo/bin/linuxsteamrt64/libclient.so"

TIMESTAMP=$(stat -c '%W' "${CLIENT_PATH}")
TIME=$(date -d "@${TIMESTAMP}" +'%d-%m-%Y')

echo ${TIMESTAMP}
echo ${TIME}

cp "${CLIENT_PATH}" "./libclient.so.${TIME}"
