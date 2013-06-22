#include "EBCReader.h"

#include <cstdio>
#include <sstream>

namespace osgtt {

bool EBCReader::setCRDFile(const std::string& crd) {
	char buff[1024];

	_vertices = new osg::Vec3Array();

	FILE* fp = std::fopen(crd.c_str(), "r");

	if(!fp) return false;

	char* ptr = 0;

	while((ptr = std::fgets(buff, 1024, fp))) {
		osg::Vec3 v;
		
		// TODO: Handle doubles here properly...
		std::sscanf(buff, "%f %f %f %f", &v.x(), &v.x(), &v.y(), &v.z());

		_vertices->push_back(v);
	}

	std::fclose(fp);

	return true;
}

EBCNode* EBCReader::readEBCFile(const std::string& ebc, osg::PrimitiveSet::Mode mode) {
	char buff[1024];

	osg::DrawElementsUInt* elements = new osg::DrawElementsUInt(mode, 0);

	FILE* fp = std::fopen(ebc.c_str(), "r");

	if(!fp) return 0;

	char* ptr = 0;

	while((ptr = std::fgets(buff, 1024, fp))) {
		if(mode == osg::PrimitiveSet::TRIANGLES) {
			unsigned int e0;
			unsigned int e1;
			unsigned int e2;

			sscanf(buff, "%d %d %d", &e0, &e1, &e2);

			elements->push_back(e0);
			elements->push_back(e1);
			elements->push_back(e2);
		}
	}

	std::fclose(fp);

	EBCNode* ebcNode = new EBCNode(_vertices.get(), elements, mode);

	std::ostringstream geometryName;

	geometryName << ebc << " (Geometry)";

	ebcNode->setName(ebc);
	ebcNode->getDrawable(0)->setName(geometryName.str());

	return ebcNode;
}

}

