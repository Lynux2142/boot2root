#!/bin/bash
DST=answer.c
rm -rf ft_fun

if [ -z $1 ]; then
        echo "Usage: ./cmd IP"
        exit 1
fi

wget ftp://$1/fun --ftp-user=lmezard --ftp-pass='G!@M6f4Eatau{sF"'
tar xvf fun
cd ft_fun

for i in {1..750}; do
        FILE=$(grep -lRE "\/\/file$i$" .)
        sed -E 's/\/\/file[0-9]+//' $FILE  >> $DST
done

OUTPUT=$(gcc $DST -o the_answer && ./the_answer | head -n 1 | awk -F': ' '{print $2}' | tr -d '\n' | sha256sum)
echo "Password: $OUTPUT"