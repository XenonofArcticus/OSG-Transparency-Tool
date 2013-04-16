#pragma once

#include "DepthPeeling.h"

#include <osg/Group>

namespace osgtt {

class TransparencyGroup: public osg::Group {
public:
	enum TransparencyMode {
		DEPTH_SORTED_BIN,
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

	DepthPeeling* getDepthPeeling() {
		return _depthPeeling;
	}

protected:
	// virtual void childRemoved(unsigned int pos, unsigned int numChildrenToRemove);
	// virtual void childInserted(unsigned int pos);

	TransparencyMode           _mode;
	osg::ref_ptr<DepthPeeling> _depthPeeling;

private:
	osg::ref_ptr<osg::Group> _scene;
};

}

