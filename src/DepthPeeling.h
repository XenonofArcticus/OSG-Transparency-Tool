#pragma once

#include <osgGA/GUIEventHandler>

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

	class EventHandler: public osgGA::GUIEventHandler {
	public:
		EventHandler(DepthPeeling* depthPeeling);

		virtual bool handle(
			const osgGA::GUIEventAdapter& gea,
			osgGA::GUIActionAdapter&      gaa,
			osg::Object*                  obj,
			osg::NodeVisitor*             nv
		);

		osg::observer_ptr<DepthPeeling> _depthPeeling;
	};

	DepthPeeling(unsigned int width = 0, unsigned int height = 0);

	osg::Node*    createQuad(unsigned int layerNumber);
	void          createPeeling();
	EventHandler* createEventHandler();

	void resize(int width, int height);

	void setScene(osg::Node* scene) {
		_scene = scene;
	}

	osg::Node* getRoot() {
		return _root.get();
	}

	void setNumPasses(unsigned int numPasses) {
		_numPasses = numPasses;
		
		createPeeling();
	}

	unsigned int getNumPasses() const {
		return _numPasses;
	}

	void setTexUnit(unsigned int texUnit) {
		_texUnit = texUnit;

		createPeeling();
	}

	void setOffsetValue(unsigned int offsetValue) {
		_offsetValue = offsetValue;

		createPeeling();
	}

	unsigned int getOffsetValue() const {
		return _offsetValue;
	}

	TextureMode  _textureMode;
	DepthMode    _depthMode;
	unsigned int _numPasses;
	unsigned int _texUnit;
	unsigned int _texWidth;
	unsigned int _texHeight;
	unsigned int _offsetValue;

	// A read-only, potentially invalid pointer to the scene to render.
	osg::observer_ptr<osg::Node> _scene;

	osg::ref_ptr<osg::Group>                 _root;
	osg::ref_ptr<osg::Camera>                _compositeCamera;
	osg::ref_ptr<osg::Texture>               _depthTextures[2];
	std::vector<osg::ref_ptr<osg::Texture> > _colorTextures;
};

}

