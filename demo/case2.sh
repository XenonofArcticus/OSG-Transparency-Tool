#!/usr/bin/env bash

cd "$(dirname ${0})"

./osgtt-demo case2/strakes.crd case2/outer_box.ebc

cat <<EOF
	case2/inner_box.ebc \
	case2/inner_pipe1.ebc \
	case2/inner_pipe1A.ebc \
	case2/inner_pipe2.ebc \
	case2/inner_pipe2A.ebc \
	case2/inner_pipe3.ebc \
	case2/inner_pipe3A.ebc \
	case2/outer_box.ebc \
	case2/outer_pipe1.ebc \
	case2/outer_pipe1A.ebc \
	case2/outer_pipe2.ebc \
	case2/outer_pipe2A.ebc \
	case2/outer_pipe3.ebc \
	case2/outer_pipe3A.ebc
EOF

