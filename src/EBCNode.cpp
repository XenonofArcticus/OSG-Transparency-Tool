#include "EBCNode.h"

#include <osg/Geometry>

namespace osgtt {

EBCNode::EBCNode(
	osg::Vec3Array*        vertices,
	osg::Vec3Array*        normals,
	osg::DrawElementsUInt* elements
) {
	osg::Geometry*  geometry = new osg::Geometry();
	osg::Vec4Array* colors   = new osg::Vec4Array();

	colors->push_back(osg::Vec4(0.0, 0.0, 0.0, 0.5));

	geometry->setVertexArray(vertices);
	geometry->setNormalArray(normals);
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(elements);
	geometry->dirtyBound();

	addDrawable(geometry);
}

void EBCNode::setRGB(const osg::Vec3& rgb) {
	osg::Vec4* color = _getColor(true);

	if(!color) return;

	color->set(rgb[0], rgb[1], rgb[2], color->a());
}

void EBCNode::setAlpha(osg::Vec4::value_type alpha) {
	osg::Vec4* color = _getColor(true);

	if(!color) return;

	color->set(color->r(), color->g(), color->b(), alpha);
}

void EBCNode::setColor(const osg::Vec4& color) {
	osg::Vec4* col = _getColor(true);

	if(!col) return;

	*col = color;
}

osg::Vec4* EBCNode::_getColor(bool setDirty) {
	osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(getDrawable(0));

	if(!geometry) return 0;

	osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(geometry->getColorArray());

	if(!colors) return 0;

	if(setDirty) colors->dirty();

	return &(*colors)[0];
}

}

