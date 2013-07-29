#include "TransparencyGroup.h"

#include <osg/BlendFunc>
#include <osg/Depth>

namespace osgtt {

TransparencyGroup::TransparencyGroup():
_mode(NO_TRANSPARENCY) {
	_scene        = new osg::Group();
	_depthPeeling = new DepthPeeling();
	_blendFunc    = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

TransparencyGroup::TransparencyGroup(const TransparencyGroup& tg, const osg::CopyOp& co):
osg::Group    (tg, co),
_mode         (tg._mode),
_depthPeeling (tg._depthPeeling),
_scene        (tg._scene) {
}

bool TransparencyGroup::addChild(osg::Node* child) {
	return _scene->addChild(child);
}

bool TransparencyGroup::insertChild(unsigned int index, osg::Node* child) {
	return _scene->insertChild(index, child);
}

bool TransparencyGroup::removeChildren(unsigned int pos, unsigned int numChildrenToRemove) {
	return _scene->removeChildren(pos, numChildrenToRemove);
}

bool TransparencyGroup::replaceChild(osg::Node* origChild, osg::Node* newChild) {
	return _scene->replaceChild(origChild, newChild);
}

bool TransparencyGroup::setChild(unsigned int i, osg::Node* child) {
	return _scene->setChild(i, child);
}

void TransparencyGroup::setTransparencyMode(TransparencyMode mode) {
	if(_mode == mode) return;

	// NOTE: As we setup the TransparencyMode settings, _mode will still contain the
	// PREVIOUS TransparencyMode, while the mode argument will contain the desired mode.

	// First, remove all of the previous children, whatever they are.
	Group::removeChildren(0, getNumChildren());
	Node::dirtyBound(); // just in case

	osg::StateSet* ss = getOrCreateStateSet();

	// In this mode, we'll just add our proxied scene object and use OSG's default
	// transparency/alpha/blending/whatever.
	if(mode == DEPTH_SORTED_BIN) {
		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( true );
		ss->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );

		ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		ss->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::ON);

		Group::addChild(_scene.get());
	}
	// In this mode, we render transparent objects last, but without depth sorting or writing
	else if(mode == DELAYED_BLEND) {

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( false );
		ss->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );

		ss->setRenderingHint(osg::StateSet::DEFAULT_BIN);
		ss->setRenderBinDetails( 12, "RenderBin");
		ss->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::ON);

		Group::addChild(_scene.get());
	}
	else {
		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( true );
		ss->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );

		ss->setRenderingHint(osg::StateSet::DEFAULT_BIN);
		ss->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::OFF);
	}

	if(mode == DEPTH_PEELING) {
		if(_scene.get() != _depthPeeling->getScene()) {
			_depthPeeling->setScene(_scene.get());
			_depthPeeling->dirty();
		}

		Group::addChild(_depthPeeling);
	}

	_mode = mode;
}

void TransparencyGroup::setDepthPeeling(DepthPeeling* depthPeeling) {
	_depthPeeling = depthPeeling;

	if(_mode == DEPTH_PEELING) setTransparencyMode(DEPTH_PEELING);
}

}

