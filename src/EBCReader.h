#pragma once

#include "EBCNode.h"

namespace osgtt {

class EBCReader {
public:
	bool     setCRDFile(const std::string& crd);
	EBCNode* readEBCFile(const std::string& ebc, osg::PrimitiveSet::Mode mode);

private:
	osg::ref_ptr<osg::Vec3Array> _vertices;
};

}

