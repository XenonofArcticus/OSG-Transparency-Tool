#include <osg/Geode>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include "TransparentModel.h"
#include <iostream>
#include <osg/BlendFunc>
#include <stdio.h>

namespace osgtt {

TransparentModel::TransparentModel(osg::Vec3Array* vertices, osg::DrawElementsUInt* elements, osg::PrimitiveSet::Mode mode):
_elementMode (mode),
_vertices    (vertices),
_elements    (elements) {
	_solidGeode = _createGeode();
	_wireGeode  = _createGeode();

	/*
	osg::StateSet*    state  = NULL;
	osg::PolygonMode* pMode1 = new osg::PolygonMode();
	osg::PolygonMode* pMode2 = new osg::PolygonMode();

	state = _wireGeode->getOrCreateStateSet();

	pMode1->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
	
	state->setAttributeAndModes( pMode1.get(),osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON	);

	state = _solidGeode->getOrCreateStateSet();

	pMode2->setMode( osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
	
	osg::PolygonOffset* pOffset = new osg::PolygonOffset();

	pOffset->setFactor( 1.0f );
	pOffset->setUnits( 1.0f );
	
	state->setAttributeAndModes( pMode2.get(), osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );
	state->setAttributeAndModes( pOffset.get(), osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );
	*/

	addChild(_solidGeode);
	// addChild(_wireGeode);
	
	setDisplayMode(SOLID);
}

void TransparentModel::setRGB(const osg::Vec3& rgb) {
	osg::Vec4* solidColor = _getColor(*_solidGeode, true);
	osg::Vec4* wireColor  = _getColor(*_wireGeode, true);

	if(!solidColor || !wireColor) return;

	osg::Vec4::value_type r = rgb[0];
	osg::Vec4::value_type g = rgb[1];
	osg::Vec4::value_type b = rgb[2];

	solidColor->set(r, g, b, solidColor->a());

	if(_displayMode == SOLID || _displayMode == WIREFRAME) wireColor->set(r, g, b, wireColor->a());

	else wireColor->set(1.0 - r, 1.0 - g, 1.0 - b, wireColor->a());
}

void TransparentModel::setAlpha(osg::Vec4::value_type alpha) {
	osg::Vec4* solidColor = _getColor(*_solidGeode, true);
	osg::Vec4* wireColor  = _getColor(*_wireGeode, true);

	if(!solidColor || !wireColor) return;

	solidColor->set(solidColor->r(), solidColor->g(), solidColor->b(), alpha);
	wireColor->set(wireColor->r(), wireColor->g(), wireColor->b(), alpha);
}

void TransparentModel::setColor(const osg::Vec3& rgb) {
	osg::Vec4* solidColor = _getColor(*_solidGeode, true);
	osg::Vec4* wireColor  = _getColor(*_wireGeode, true);

	if(!solidColor || !wireColor) return;

	osg::Vec4::value_type r = rgb[0];
	osg::Vec4::value_type g = rgb[1];
	osg::Vec4::value_type b = rgb[2];

	solidColor->set(r, g, b, solidColor->a());

	if(_displayMode == SOLID || _displayMode == WIREFRAME) wireColor->set(r, g, b, wireColor->a());

	else wireColor->set(1.0 - r, 1.0 - g, 1.0 - b, wireColor->a());
}



void TransparentModel::setDisplayMode(DisplayMode mode) {
	_displayMode = mode;

	if(_displayMode == SOLID) {
		setChildValue(_wireGeode, false);
		setChildValue(_solidGeode, true);
	}

	else if(_displayMode == WIREFRAME) {
		setChildValue(_wireGeode.get(), true);
		setChildValue(_solidGeode.get(), false);
	}

	else {
		setChildValue(_wireGeode.get(), true);
		setChildValue(_solidGeode.get(), true);
	}
}

osg::Vec4* TransparentModel::_getColor(osg::Geode& geode, bool setDirty) {
	osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(geode.getDrawable(0));

	if(!geometry) return 0;

	osg::Vec4Array* colors = dynamic_cast<osg::Vec4Array*>(geometry->getColorArray());

	if(!colors) return 0;

	if(setDirty) colors->dirty();

	return &(*colors)[0];
}

osg::Geode* TransparentModel::_createGeode() {
	osg::Geometry*  geometry = new osg::Geometry();
	osg::Geode*     geode    = new osg::Geode();
	osg::Vec4Array* colors   = new osg::Vec4Array();

	colors->push_back(osg::Vec4(0.0, 0.0, 0.0, 0.5));

	geometry->setVertexArray(_vertices);
	geometry->setColorArray(colors);
	geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
	geometry->addPrimitiveSet(_elements);

	unsigned int numElems = 0;

	if(_elementMode == osg::PrimitiveSet::TRIANGLES) numElems = 3;

	osg::Vec3Array* normals = new osg::Vec3Array();
	
	for(unsigned int i = 0; i < _elements->size() / numElems; i++) {
		osg::Vec3f pos[3];

		for(unsigned int j = 0; j < 3; j++) pos[j] = (*_vertices)[(*_elements)[(numElems * i) + j]];
	
		osg::Vec3 normal = (pos[1] - pos[0]) ^ (pos[2] - pos[0]);

		normal.normalize();

		normals->push_back(normal);
	}

	geometry->setNormalArray(normals);
	geometry->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

	geode->addDrawable(geometry);

	geode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	geode->getOrCreateStateSet()->setAttributeAndModes(
		new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
		osg::StateAttribute::ON
	);

	return geode;
}

}

