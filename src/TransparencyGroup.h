#pragma once

#include "DepthPeeling.h"

#include <osg/Group>

namespace osgtt {

// A TransparencyGroup is an osg::Group-like node (it uses the Group interface but
// behaves differently internally) that provides a simple API for applying a technique
// for transparency on its child subgraph. Any children you add to this particular
// Group will be managed internally so that whatever technique is currently in effect
// can function properly.
//
// TODO: I believe there is currently a bug which manifests itself when a user switches
// dynamically between the various TransparencyModes that causes OSG to miscalculate the 
// near/far values for the topmost Camera.
class TransparencyGroup: public osg::Group {
public:
	// These are the supported "kinds" of transparency we can apply to our subgraph.
	enum TransparencyMode {
		// This is the simplest, fastest, most innaccurate form; it is the OSG
		// default method, and simply sorts by depth.
		DEPTH_SORTED_BIN,
		// This is the the more accurate method in which an arbitrary number of
		// depth "peels" are applied using projective texturing and RTT. The more
		// passes you use, the more accurate (and slow) you scene will be.
		DEPTH_PEELING
	};

	TransparencyGroup();
	TransparencyGroup(const TransparencyGroup&, const osg::CopyOp& co = osg::CopyOp::SHALLOW_COPY);

	META_Node(osgtt, TransparencyGroup);

	virtual bool addChild(osg::Node* child);
	virtual bool insertChild(unsigned int index, osg::Node* child);
	virtual bool removeChildren(unsigned int pos, unsigned int numChildrenToRemove);
	virtual bool replaceChild(osg::Node* origChild, osg::Node* newChild);
	virtual bool setChild(unsigned int i, osg::Node* child);

	void setTransparencyMode(TransparencyMode mode);

	// Sets the DepthPeeling object that will manage the subgraph when DEPTH_PEELING mode
	// is in effect. The DepthPeeling object itself maintains its own configuration options.
	void setDepthPeeling(DepthPeeling* depthPeeling);

	DepthPeeling* getDepthPeeling() {
		return _depthPeeling.get();
	}

protected:
	// TODO: Is it possible to use this API instead of overridding every osg::Group method?
	// virtual void childRemoved(unsigned int pos, unsigned int numChildrenToRemove);
	// virtual void childInserted(unsigned int pos);

	TransparencyMode           _mode;
	osg::ref_ptr<DepthPeeling> _depthPeeling;

private:
	// Can't touch this! :)
	osg::ref_ptr<osg::Group> _scene;
};

}

