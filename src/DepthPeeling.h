#pragma once

// This code was adapted from the osgoit.cpp example.

#include <osg/Group>
#include <osg/Texture>

namespace osgtt {

// A DepthPeeling object is an osg::Group that uses OSG's RTT support to setup
// an arbitrary number of passes to use to attempt to render a transparent scene
// as accurately as possible. It is not reccomended that you use this object
// as a generic group, as the children you add will be deleted every time
// dirty() is called.
//
// This object will create $numPasses RTT cameras and a single "composite" 
// camera and add them as required to itself. There is currently no support
// for stretching an RTT stack internally to fit a viewport that isn't a 
// 1:1 ratio of texture:view dimensions. This could be achieved by modifying
// the CullCallback.
//
// Note that while we need to create a color buffer for each peel/pass, we can
// optimize by only using two depth buffers; modulo-ing (for lack of a better word)
// ourselves by using one-or-the-other for each pass.
class DepthPeeling: public osg::Group {
public:
	// Whether the RTT "stack" will use power-of-two textures or texture
	// rectangles. Your platform will dictate the best choice.
	enum TextureMode {
		TEXTURE_STANDARD,
		TEXTURE_RECTANGLE
	};

	enum DepthMode {
		DEPTH_STANDARD,
		DEPTH_PACKED_STENCIL
	};

	// This CullCallback is responsible for the projective texturing magic that
	// is required by the depth peeling technique to function properly.
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

	// You should initialize the DepthPeeling group to the screen's width/height
	// (if possible) at creation. This isn't mandatory on Windows, but is on Linux.
	DepthPeeling(unsigned int width = 0, unsigned int height = 0);

	// This method should be called anytime the parent Viewport or window is
	// resized. It will, if necessary, reallocate the internal texture stack.
	void resize(int width, int height);

	// This is the "beef" of the DepthPeeling object. It will create the entire
	// RTT stack and composite viewing camera based on whatever options are currently
	// set. It should be called any time you change depth peeling settings.
	//
	// Conviently, most methods do this for you automatically, so its likely you'll
	// never have to worry about it.
	void dirty();

	osg::Node* getScene() {
		return _scene.get();
	}

	const osg::Node* getScene() const {
		return _scene.get();
	}

	// This is the scene/subgraph which your render-to-texture cameras are pointed at.
	void setScene(osg::Node* scene) {
		_scene = scene;
	}

	// The number of "peels" or layers that are used to chop up your transparent
	// scene. The composite camera will combine each pass into a composited, blended
	// final image.
	void setNumPasses(unsigned int numPasses) {
		_numPasses = numPasses;
	}

	unsigned int getNumPasses() const {
		return _numPasses;
	}

	// Here you can set the actualy GL_TEXTURE* unit that should be used. This will
	// almost certainly be ncessary for you to call if your scene is using texturing,
	// otherwise you'll need to make sure your existing state doesn't use GL_TEXTURE0,
	// which is the (arguably silly) default.
	void setTexUnit(unsigned int texUnit) {
		_texUnit = texUnit;
	}

	// This is the depth offset used by the CullCallback during the projective
	// texturing to determine the next "peel" layer. It's generally safe to leave
	// this alone.
	void setOffsetValue(unsigned int offsetValue) {
		_offsetValue = offsetValue;
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
	osg::ref_ptr<osg::Camera>                _compositeCamera;
	osg::ref_ptr<osg::Texture>               _depthTextures[2];
	std::vector<osg::ref_ptr<osg::Texture> > _colorTextures;
};

}

