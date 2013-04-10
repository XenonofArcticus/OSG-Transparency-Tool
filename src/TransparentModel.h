#pragma once

#include <osg/Array>
#include <osg/Group>
#include <osg/Switch>
#include <osg/Geometry>

namespace osgtt {

class TransparentModel: public osg::Node {
public:
	enum TransparencyMode {
		DEPTH_SORTED_BIN,
		DEPTH_PEELING
	};

	TransparentModel(osg::Vec3Array* vertices, osg::DrawElementsUInt* elements, osg::PrimitiveSet::Mode mode);

	void setRGB(const osg::Vec3& rgb);
	void setAlpha(osg::Vec4::value_type alpha);
	void setColor(const osg::Vec4& color); 
	void setDisplayMode(DisplayMode mode);
	void setTransparencyMode(TransparencyMode mode);

private:
	osg::Geode* _createGeode();

	// Returns the actual, in-use color Vec4 object. Will set the underlying array
	// to "dirty" if you pass in true as the second argument.
	osg::Vec4* _getColor(osg::Geode& geode, bool setDirty=false);

	osg::PrimitiveSet::Mode             _elementMode;
	DisplayMode                         _displayMode;
	TransparencyMode                    _transparencyMode;
	osg::ref_ptr<osg::Vec3Array>        _vertices;
	osg::ref_ptr<osg::DrawElementsUInt> _elements;
	osg::ref_ptr<osg::Geode>            _solidGeode;
	osg::ref_ptr<osg::Geode>            _wireGeode;
};

}

