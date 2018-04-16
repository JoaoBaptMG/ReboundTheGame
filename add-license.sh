#!/bin/bash

for i in $(find ./ExportTools ./MainGame ./Commons -name '*pp' ! -name '*tinyxml2*' ! -name '*aes*')
do
  if ! grep -q Copyright $i
  then
    cat copyright.txt $i > $i.new && mv $i.new $i
  fi
done
