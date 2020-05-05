#!/bin/sh
curl "http://qrng.anu.edu.au/API/jsonI.php?length=500&type=hex16&size=500"  | jq -r '.data | join("")' | xxd -r -p - entropy.bin && curl -X POST -H "Content-Type: application/octet-stream" --data-binary "@entropy.bin" "https://newtonlib.azurewebsites.net/api/attractors?radius=3000&latitude=33.585096&longitude=130.400177&gid=3333" | python -mjson.tool
