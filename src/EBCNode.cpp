#include "EBCNode.h"

#include <osg/Geometry>

namespace osgtt {

EBCNode::EBCNode(osg::Vec3Array* vertices, osg::DrawElementsUInt* elements, osg::PrimitiveSet::Mode mode) {
	osg::Geometry*  geometry = new osg::Geometry();
	osg::Vec4Array* colors   = new osg::Vec4Array();

	colors->push_back(osg::Vec4(0.0, 0.0, 0.0, 0.5));

	geometry->setVertexArray(vertices);
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(elements);

	unsigned int numElems = 0;

	if(mode == osg::PrimitiveSet::TRIANGLES) numElems = 3;

	osg::Vec3Array* normals = new osg::Vec3Array();
	
	for(unsigned int i = 0; i < elements->size() / numElems; i++) {
		osg::Vec3f pos[3];

		for(unsigned int j = 0; j < 3; j++) pos[j] = (*vertices)[(*elements)[(numElems * i) + j]];
	
		osg::Vec3 normal = (pos[1] - pos[0]) ^ (pos[2] - pos[0]);

		normal.normalize();

		normals->push_back(normal);
	}

	geometry->setNormalArray(normals);
	geometry->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

	addDrawable(geometry);

	/*
	getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	getOrCreateStateSet()->setAttributeAndModes(
		new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
		osg::StateAttribute::ON
	);
	*/
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

