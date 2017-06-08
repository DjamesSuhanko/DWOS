#!/bin/bash
[ -d /tmp/CadeiaDeCustodia  ] && rm -rf /tmp/CadeiaDeCustodia

cp -rfv ../CadeiaDeCustodia /tmp

cd /tmp/CadeiaDeCustodia

/opt/qt/qt5/bin/qmake

make

strip DWatson

cp DWatson /media/bkp/dwosWayland/data/bin/DWatson
