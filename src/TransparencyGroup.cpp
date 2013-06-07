#include "TransparencyGroup.h"

#include <osg/BlendFunc>

namespace osgtt {

TransparencyGroup::TransparencyGroup() {
	_scene        = new osg::Group();
	_depthPeeling = new DepthPeeling();

	setTransparencyMode(DEPTH_SORTED_BIN);
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
	// NOTE: As we setup the TransparencyMode settings, _mode will still contain the
	// PREVIOUS TransparencyMode, while the mode argument will contain the desired mode.

	// First, remove all of the previous children, whatever they are.
	// _children.clear();
	Group::removeChildren(0, 1);

	// In this mode, we'll just add our proxied scene object and use OSG's default
	// transparency/alpha/blending/whatever.
	if(mode == DEPTH_SORTED_BIN) {
		getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		getOrCreateStateSet()->setAttributeAndModes(
			new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
			osg::StateAttribute::ON
		);

		Group::addChild(_scene);
	}

	else if(mode == DEPTH_PEELING) {
		_depthPeeling->setScene(_scene);
		_depthPeeling->dirty();

		Group::addChild(_depthPeeling);
	}

	_mode = mode;
}

void TransparencyGroup::setDepthPeeling(DepthPeeling* depthPeeling) {
	_depthPeeling = depthPeeling;

	if(_mode == DEPTH_PEELING) setTransparencyMode(DEPTH_PEELING);
}

}

