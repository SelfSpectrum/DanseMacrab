#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <path_to_ods_file>"
    exit 1
fi

ODS_FILE="$1"

soffice --convert-to csv:"Text - txt - csv (StarCalc)":9,34,UTF8,1,,0,false,true,false,false,false,-1 "$ODS_FILE"

for file in "."/*.csv; do
    if [ -e "$file" ]; then
    	tsv="${file%.csv}.tsv"
    	mv "$file" "$tsv"
    	mv "$tsv" "${tsv##*-}"
    	tsv="${tsv##*-}"
	mv "$tsv" $(echo "$tsv" | tr '[:upper:]' '[:lower:]')
    else
        echo "No files with the extension .csv found."
    fi
done

if [ -e ./animations.tsv ]; then
    mv animations.tsv ./resources/anims
fi
if [ -e ./armors.tsv ]; then
    mv armors.tsv ./resources/combat
fi
if [ -e ./characters.tsv ]; then
    mv characters.tsv ./resources/combat
fi
if [ -e ./charms.tsv ]; then
    mv charms.tsv ./resources/combat
fi
if [ -e ./dialogs.tsv ]; then
    mv dialogs.tsv ./resources/text
fi
if [ -e ./enemies.tsv ]; then
    mv enemies.tsv ./resources/combat
fi
if [ -e ./english.tsv ]; then
    mv english.tsv ./resources/text
fi
if [ -e ./russian.tsv ]; then
    mv russian.tsv ./resources/text
fi
if [ -e ./spanish.tsv ]; then
    mv spanish.tsv ./resources/text
fi
if [ -e ./sprites.tsv ]; then
    mv sprites.tsv ./resources/gfx
fi
if [ -e ./techniques.tsv ]; then
    mv techniques.tsv ./resources/combat
fi
if [ -e ./weapons.tsv ]; then
    mv weapons.tsv ./resources/combat
fi

echo "Conversion complete. TSV files are in the current directory."

