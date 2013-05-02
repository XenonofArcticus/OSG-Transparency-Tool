#pragma once

#include <osg/Group>
#include <osg/Texture>

namespace osgtt {

class DepthPeeling: public osg::Referenced {
public:
	enum TextureMode {
		TEXTURE_STANDARD,
		TEXTURE_RECTANGLE
	};

	enum DepthMode {
		DEPTH_STANDARD,
		DEPTH_PACKED_STENCIL
	};

	class CullCallback: public osg::NodeCallback {
	public:
		CullCallback(
			unsigned int texUnit,
			unsigned int texWidth,
			unsigned int texHeight,
			unsigned int offsetValue,
			TextureMode  textureMode
		);

		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

	private:
		unsigned int _texUnit;
		unsigned int _texWidth;
		unsigned int _texHeight;
		unsigned int _offsetValue;
		TextureMode  _textureMode;
	};

	DepthPeeling(unsigned int width = 0, unsigned int height = 0);

	void resize(int width, int height);
	void dirty();

	void setScene(osg::Node* scene) {
		_scene = scene;
	}

	osg::Node* getRoot() {
		return _root;
	}

	void setNumPasses(unsigned int numPasses) {
		_numPasses = numPasses;
		
		dirty();
	}

	unsigned int getNumPasses() const {
		return _numPasses;
	}

	void setTexUnit(unsigned int texUnit) {
		_texUnit = texUnit;

		dirty();
	}

	void setOffsetValue(unsigned int offsetValue) {
		_offsetValue = offsetValue;

		dirty();
	}

	unsigned int getOffsetValue() const {
		return _offsetValue;
	}

	unsigned int getTexWidth() const {
		return _texWidth;
	}

	unsigned int getTexHeight() const {
		return _texHeight;
	}

protected:
	TextureMode  _textureMode;
	DepthMode    _depthMode;
	unsigned int _numPasses;
	unsigned int _texUnit;
	unsigned int _texWidth;
	unsigned int _texHeight;
	unsigned int _offsetValue;

	osg::ref_ptr<osg::Node>                  _scene;
	osg::ref_ptr<osg::Group>                 _root;
	osg::ref_ptr<osg::Camera>                _compositeCamera;
	osg::ref_ptr<osg::Texture>               _depthTextures[2];
	std::vector<osg::ref_ptr<osg::Texture> > _colorTextures;
};

}

