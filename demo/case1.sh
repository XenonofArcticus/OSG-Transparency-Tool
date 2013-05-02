#!/usr/bin/env bash

cd "$(dirname ${0})"

./osgtt-demo case1/mixer.crd case1/jtube.ebc

# dot -Tpdf scene.dot -o scene.pdf
# evince scene.pdf

