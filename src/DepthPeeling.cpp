#include "DepthPeeling.h"

#include <osg/AlphaFunc>
#include <osg/BlendFunc>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/TexMat>
#include <osg/TexGenNode>
#include <osgUtil/CullVisitor>

#include <osg/io_utils>
#include <sstream>

namespace osgtt {

template<typename TextureType>
osg::Texture* _createTexture(unsigned int width, unsigned int height) {
	TextureType* texture = new TextureType();

	texture->setTextureSize(width, height);

	return texture;
}

void _setTextureSize(osg::Texture* texture, int width, int height) {
	osg::Texture2D* t2d = dynamic_cast<osg::Texture2D*>(texture);

	if(t2d) {
		t2d->setTextureSize(width, height);

		return;
	}
	
	osg::TextureRectangle* tr = dynamic_cast<osg::TextureRectangle*>(texture);

	if(tr) {
		tr->setTextureSize(width, height);

		return;
	}
}

osg::Node* _createQuad(unsigned int layerNumber) {
	std::ostringstream name;

	name << "Layer" << layerNumber;

	osg::Geometry* geometry = osg::createTexturedQuadGeometry(
		osg::Vec3(),
		osg::Vec3(1.0, 0.0, 0.0),
		osg::Vec3(0.0, 1.0, 0.0),
		0.0, 0.0,
		1.0, 1.0
	);

	osg::Geode* geode = new osg::Geode();

	geode->addDrawable(geometry);
	geode->setName(name.str());

	name << " (Geometry)";

	geometry->setName(name.str());

	return geode;
}

DepthPeeling::CullCallback::CullCallback(
	unsigned int texUnit,
	unsigned int texWidth, 
	unsigned int texHeight,
	unsigned int offsetValue,
	TextureMode  textureMode
):
_texUnit     (texUnit),
_texWidth    (texWidth),
_texHeight   (texHeight),
_offsetValue (offsetValue),
_textureMode (textureMode) {
}

void DepthPeeling::CullCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
	osgUtil::CullVisitor* cullVisitor = static_cast<osgUtil::CullVisitor*>(nv);
	osgUtil::RenderStage* renderStage = cullVisitor->getCurrentRenderStage();
	const osg::Viewport*  viewport    = renderStage->getViewport();

	osg::Matrixd m(*cullVisitor->getProjectionMatrix());

	{
		double fovy, ar, near, far;

		osg::Camera* cam = cullVisitor->getCurrentCamera();

		m.getPerspective(fovy, ar, near, far);

		// m = osg::Matrixd::perspective(fovy, ar, 1, 10000);
		// m.getPerspective(fovy, ar, near, far);

		//OSG_WARN << cam->getName() << std::endl;
		//OSG_WARN << "\t" << fovy << ", " << ar << ", " << near << ", " << far << std::endl;
		//OSG_WARN << m << std::endl;
	}

	m.postMultTranslate(osg::Vec3d(1.0, 1.0, 1.0));
	m.postMultScale(osg::Vec3d(0.5, 0.5, 0.5));

	osg::Vec3d::value_type vpw = viewport->width();
	osg::Vec3d::value_type vph = viewport->height();

	if(_textureMode == TEXTURE_STANDARD) {
		vpw /= static_cast<double>(_texWidth);
		vph /= static_cast<double>(_texHeight);
	}

	// Scale the viewport to the texture width and height.
	m.postMultScale(osg::Vec3d(vpw, vph, 1.0));

	// Kind of polygon offset: note this way, we can also offset lines and points.
	// Whereas with the polygon offset we could only handle surface primitives.
	m.postMultTranslate(osg::Vec3d(0, 0, -ldexp(static_cast<double>(_offsetValue), -24)));

	osg::TexMat*   texMat   = new osg::TexMat(m);
	osg::StateSet* stateSet = new osg::StateSet();

	stateSet->setTextureAttribute(_texUnit, texMat);

	cullVisitor->pushStateSet(stateSet);
	
	traverse(node, nv);
	
	cullVisitor->popStateSet();
}

DepthPeeling::DepthPeeling(unsigned int width, unsigned int height):
_textureMode (TEXTURE_STANDARD),
_depthMode   (DEPTH_STANDARD),
_numPasses   (2),
_texUnit     (0),
_texWidth    (width),
_texHeight   (height),
_offsetValue (8) {
	setDataVariance(osg::Object::DYNAMIC);

	// TODO: Should this be called?
	// dirty();
}

void DepthPeeling::resize(int width, int height) {
	if(_textureMode == TEXTURE_STANDARD) {
		width  = osg::Image::computeNearestPowerOfTwo(width, 1.0);
		height = osg::Image::computeNearestPowerOfTwo(height, 1.0);
	}

	if(
		static_cast<int>(_texWidth) == width &&
		static_cast<int>(_texHeight) == height
	) return;

	OSG_WARN << width << " " << height << std::endl;

	_setTextureSize(_depthTextures[0].get(), width, height);
	_setTextureSize(_depthTextures[1].get(), width, height);

	for(unsigned int i = 0; i < _colorTextures.size(); i++) _setTextureSize(_colorTextures[i].get(), width, height);

	_texWidth  = width;
	_texHeight = height;
    // I added
    dirty();
}

void DepthPeeling::dirty() {
	if(!_texWidth || !_texHeight || !_scene) return;

	removeChildren(0, getNumChildren());
	setName("DepthPeelingRoot");

	_colorTextures.clear();

	_compositeCamera = new osg::Camera();

	_compositeCamera->setName("CompositeCamera");
	_compositeCamera->setDataVariance(osg::Object::DYNAMIC);
	_compositeCamera->setInheritanceMask(osg::Camera::READ_BUFFER | osg::Camera::DRAW_BUFFER);
	_compositeCamera->setRenderOrder(osg::Camera::POST_RENDER);
	_compositeCamera->setClearMask(0);
	_compositeCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	_compositeCamera->setViewMatrix(osg::Matrix());
	_compositeCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, 1, 0, 1));
	_compositeCamera->setCullCallback(new CullCallback(0, _texWidth, _texHeight, 0, _textureMode));
	_compositeCamera->setComputeNearFarMode(osg::Camera::COMPUTE_NEAR_FAR_USING_PRIMITIVES);

	osg::StateSet* stateSet = _compositeCamera->getOrCreateStateSet();

	stateSet->setBinName("TraversalOrderBin");
	stateSet->setRenderBinMode(osg::StateSet::USE_RENDERBIN_DETAILS);

	addChild(_compositeCamera);

	for (unsigned int i = 0; i < 2; i++) {
		if(_textureMode == TEXTURE_STANDARD) _depthTextures[i] = _createTexture<osg::Texture2D>(_texWidth, _texHeight);
		
		else _depthTextures[i] = _createTexture<osg::TextureRectangle>(_texWidth, _texHeight);

		_depthTextures[i]->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
		_depthTextures[i]->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
		_depthTextures[i]->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
		_depthTextures[i]->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);

		if(_depthMode == DEPTH_STANDARD) {
			_depthTextures[i]->setInternalFormat(GL_DEPTH_COMPONENT24);
			_depthTextures[i]->setSourceFormat(GL_DEPTH_COMPONENT);
			_depthTextures[i]->setSourceType(GL_FLOAT);
		}

		else {
			_depthTextures[i]->setInternalFormat(GL_DEPTH24_STENCIL8_EXT);
			_depthTextures[i]->setSourceFormat(GL_DEPTH_STENCIL_EXT);
			_depthTextures[i]->setSourceType(GL_UNSIGNED_INT_24_8_EXT);
		}

		_depthTextures[i]->setShadowComparison(true);
		_depthTextures[i]->setShadowAmbient(0);
		_depthTextures[i]->setShadowCompareFunc(osg::Texture::GREATER);
		_depthTextures[i]->setShadowTextureMode(osg::Texture::INTENSITY);
	}

	for(unsigned int i = 0; i < _numPasses; i++) {
		osg::Camera* camera = new osg::Camera();

		std::ostringstream name;

		name << "Camera_Pass" << i;

		camera->setName(name.str());
		camera->setDataVariance(osg::Object::DYNAMIC);
		camera->setInheritanceMask(osg::Camera::ALL_VARIABLES);
		camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
		camera->setRenderOrder(osg::Camera::PRE_RENDER, i);
		camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		camera->setClearColor(osg::Vec4f(0.0, 0.0, 0.0, 0.0));
		camera->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);

		osg::Texture* depthTexture     = _depthTextures[i % 2].get();
		osg::Texture* prevDepthTexture = _depthTextures[(i + 1) % 2].get();

		if(_depthMode == DEPTH_STANDARD) camera->attach(osg::Camera::DEPTH_BUFFER, depthTexture);

		else camera->attach(osg::Camera::PACKED_DEPTH_STENCIL_BUFFER, depthTexture);

		osg::Texture* colorTexture = 0;

		if(_textureMode == TEXTURE_STANDARD) colorTexture = _createTexture<osg::Texture2D>(_texWidth, _texHeight);
		
		else colorTexture = _createTexture<osg::TextureRectangle>(_texWidth, _texHeight);

		_colorTextures.push_back(colorTexture);

		colorTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
		colorTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
		colorTexture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_BORDER);
		colorTexture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_BORDER);
		colorTexture->setInternalFormat(GL_RGBA);
		camera->attach(osg::Camera::COLOR_BUFFER, colorTexture);

		camera->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

		if(!i) camera->addChild(_scene.get());

		else {
			osg::StateSet* stateSet = camera->getOrCreateStateSet();

			stateSet->setAttributeAndModes(
				new osg::AlphaFunc(osg::AlphaFunc::GREATER, 0.01),
				osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE
			);

			stateSet->setTextureAttributeAndModes(_texUnit, prevDepthTexture);
			stateSet->setTextureMode(_texUnit, GL_TEXTURE_GEN_S, osg::StateAttribute::ON);
			stateSet->setTextureMode(_texUnit, GL_TEXTURE_GEN_T, osg::StateAttribute::ON);
			stateSet->setTextureMode(_texUnit, GL_TEXTURE_GEN_R, osg::StateAttribute::ON);
			stateSet->setTextureMode(_texUnit, GL_TEXTURE_GEN_Q, osg::StateAttribute::ON);

			osg::TexGenNode* texGenNode = new osg::TexGenNode();

			texGenNode->setReferenceFrame(osg::TexGenNode::ABSOLUTE_RF);
			texGenNode->setTextureUnit(_texUnit);
			texGenNode->getTexGen()->setMode(osg::TexGen::EYE_LINEAR);
			
			camera->addChild(texGenNode);
			camera->addCullCallback(new CullCallback(_texUnit, _texWidth, _texHeight, _offsetValue, _textureMode));

			texGenNode->addChild(_scene.get());
		}

		addChild(camera);

		osg::Node* geode = _createQuad(i);

		osg::StateSet* stateSet = geode->getOrCreateStateSet();
		
		stateSet->setTextureAttributeAndModes(0, colorTexture, osg::StateAttribute::ON);
		stateSet->setAttribute(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
		stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		
		_compositeCamera->insertChild(0, geode);
	}
}

}

