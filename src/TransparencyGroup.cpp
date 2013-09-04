#include "TransparencyGroup.h"

#include <osg/BlendFunc>
#include <osg/Depth>

namespace osgtt {
	
TransparencyGroup::TransparencyGroup():
_mode(NO_TRANSPARENCY) {
	_scene            = new osg::Group();
	_depthPeeling     = new DepthPeeling();
	_blendFunc        = new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	_transparentState = new osg::StateSet();
	_transparentStateDoubleSided = new osg::StateSet();
	_opaqueState      = new osg::StateSet();
	_opaqueStateDoubleSided      = new osg::StateSet();

	_transparentState->setMode( GL_CULL_FACE, osg::StateAttribute::ON ); // single-sided transparent faces should cull
	_transparentStateDoubleSided->setMode( GL_CULL_FACE, osg::StateAttribute::OFF ); // need to see (lit) back faces in transparency
	_opaqueState->setMode( GL_CULL_FACE, osg::StateAttribute::ON ); // opaque back faces should cull
	_opaqueStateDoubleSided->setMode( GL_CULL_FACE, osg::StateAttribute::OFF ); // opaque doublesided should not cull
}

TransparencyGroup::TransparencyGroup(const TransparencyGroup& tg, const osg::CopyOp& co):
osg::Group    (tg, co),
_mode         (tg._mode),
_depthPeeling (tg._depthPeeling),
_scene        (tg._scene) {
}

bool TransparencyGroup::addChild(osg::Node* child, bool transparent, bool twoSided) {
	if(transparent) {
		if(twoSided) {
			child->setStateSet(_transparentStateDoubleSided);
		}
		else {
			child->setStateSet(_transparentState);
		}

	}
	else {
		if(twoSided) {
			child->setStateSet(_opaqueStateDoubleSided);
		}
		else {
			child->setStateSet(_opaqueState);
		}
	}
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

	// In this mode, we'll just add our proxied scene object and use OSG's default
	// transparency/alpha/blending/whatever.
	if(mode == DEPTH_SORTED_BIN) {
		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( true );
		_transparentState->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );
		_transparentState->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		_transparentState->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::ON);

        /*----------------------------added by yiting-------------------------------------------*/
		_transparentStateDoubleSided->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );
		_transparentStateDoubleSided->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		_transparentStateDoubleSided->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::ON);
        /*---------------------------------------------------------------------------------------*/
        
		Group::addChild(_scene.get());
	}
	// In this mode, we render transparent objects last, but without depth sorting or writing
	else if(mode == DELAYED_BLEND) {

		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( false );
		_transparentState->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );

		_transparentState->setRenderingHint(osg::StateSet::DEFAULT_BIN);
		_transparentState->setRenderBinDetails( 12, "RenderBin");
		_transparentState->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::ON);

        /*----------------------------added by yiting-------------------------------------------*/
		_transparentStateDoubleSided->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );
        
		_transparentStateDoubleSided->setRenderingHint(osg::StateSet::DEFAULT_BIN);
		_transparentStateDoubleSided->setRenderBinDetails( 12, "RenderBin");
		_transparentStateDoubleSided->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::ON);
        /*---------------------------------------------------------------------------------------*/
        
		Group::addChild(_scene.get());
	}
	else if(mode == DEPTH_PEELING) {
		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( true );
		_transparentState->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );

		_transparentState->setRenderingHint(osg::StateSet::DEFAULT_BIN);

        /*----------------------------added by yiting-------------------------------------------*/
		_transparentStateDoubleSided->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );
		_transparentStateDoubleSided->setRenderingHint(osg::StateSet::DEFAULT_BIN);
        /*---------------------------------------------------------------------------------------*/
        
		if(_scene.get() != _depthPeeling->getScene()) {
			_depthPeeling->setScene(_scene.get());
			_depthPeeling->dirty();
		}

		Group::addChild(_depthPeeling);
	}
	else { // NO_TRANSPARENCY
		osg::ref_ptr<osg::Depth> depth = new osg::Depth;
		depth->setWriteMask( true );
		_transparentState->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );

		_transparentState->setRenderingHint(osg::StateSet::DEFAULT_BIN);
		_transparentState->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::OFF);
        
        /*----------------------------added by yiting-------------------------------------------*/
		_transparentStateDoubleSided->setAttributeAndModes( depth.get(), osg::StateAttribute::ON );
		_transparentStateDoubleSided->setRenderingHint(osg::StateSet::DEFAULT_BIN);
		_transparentStateDoubleSided->setAttributeAndModes(_blendFunc.get(), osg::StateAttribute::OFF);
        /*---------------------------------------------------------------------------------------*/
        
		Group::addChild(_scene.get());
	}


	_mode = mode;
}

void TransparencyGroup::setDepthPeeling(DepthPeeling* depthPeeling) {
	_depthPeeling = depthPeeling;

	if(_mode == DEPTH_PEELING) setTransparencyMode(DEPTH_PEELING);
}

}

