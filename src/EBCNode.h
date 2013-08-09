#pragma once

#include <osg/Geode>

namespace osgtt {

class EBCNode: public osg::Geode {
public:
	EBCNode(osg::Vec3Array* vertices, osg::DrawElementsUInt* elements, osg::PrimitiveSet::Mode mode);

	void setRGB(const osg::Vec3& rgb);
	void setAlpha(osg::Vec4::value_type alpha);
	void setColor(const osg::Vec4& color); 

private:
	osg::Vec4* _getColor(bool setDirty=false);
};

}

