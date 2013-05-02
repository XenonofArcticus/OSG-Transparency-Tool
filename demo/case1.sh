#!/usr/bin/env bash

cd "$(dirname ${0})"

./osgtt-demo case1/mixer.crd \
	case1/blades.ebc \
	case1/inlet.ebc \
	case1/jinlet.ebc \
	case1/jtube.ebc \
	case1/outlet.ebc \
	case1/tube.ebc

