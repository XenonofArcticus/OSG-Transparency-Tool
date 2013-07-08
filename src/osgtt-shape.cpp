#include "TransparencyGroup.h"
#include "DepthPeeling.h"

#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>

class EventHandler: public osgGA::GUIEventHandler {
public:
	EventHandler(osgtt::TransparencyGroup* group):
	_group(group) {
	}

	virtual bool handle(
		const osgGA::GUIEventAdapter& gea,
		osgGA::GUIActionAdapter&      gaa,
		osg::Object*                  obj,
		osg::NodeVisitor*             nv
	) {
		if(!_group) return false;

		osgtt::DepthPeeling* dp = _group->getDepthPeeling();

		if(!dp) return false;

		if(gea.getEventType() == osgGA::GUIEventAdapter::RESIZE) {
			unsigned int ww = gea.getWindowWidth();
			unsigned int wh = gea.getWindowHeight();
	
			dp->resize(ww, wh);

			return true;
		}

		if(gea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN) {
			int key = gea.getKey();

			if(key == 'q') dp->setNumPasses(dp->getNumPasses() + 1);

			else if(key == 'w') dp->setNumPasses(dp->getNumPasses() - 1);

			else if(key == 'a') dp->setOffsetValue(dp->getOffsetValue() + 1);

			else if(key == 's') dp->setOffsetValue(dp->getOffsetValue() - 1);
			
			else if(key == 'z') _group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_SORTED_BIN);
			
			else if(key == 'x') _group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_PEELING);

			else return false;

			return true;
		}

		return false;
	}

protected:
	osg::ref_ptr<osgtt::TransparencyGroup> _group;
};

const osg::Vec4 COLORS[8] = {
	osg::Vec4(1.0, 1.0, 1.0, 1.0),
	osg::Vec4(1.0, 0.0, 0.0, 0.9), 
	osg::Vec4(0.0, 1.0, 0.0, 0.8),
	osg::Vec4(0.0, 0.0, 1.0, 0.7),
	osg::Vec4(1.0, 1.0, 0.0, 0.6),
	osg::Vec4(0.0, 1.0, 1.0, 0.5),
	osg::Vec4(1.0, 0.0, 1.0, 0.4),
	osg::Vec4(0.0, 0.0, 0.0, 0.3),
};

int main(int argc, char** argv) {
	osgtt::TransparencyGroup* group  = new osgtt::TransparencyGroup();
	osg::Geode*               geode  = new osg::Geode();

	// Add 10 spheres inside one another, like little Matryoshka dolls!
	for(unsigned int i = 0; i < 8; i++) {
		osg::ShapeDrawable* sphere = new osg::ShapeDrawable(new osg::Box(osg::Vec3(), (i + 1) * 2.0));

		sphere->setColor(COLORS[i]);

		geode->addDrawable(sphere);
	}

	osgtt::DepthPeeling* dp = new osgtt::DepthPeeling(512, 512);

	dp->setNumPasses(8);

	group->addChild(geode);
	group->setDepthPeeling(dp);
	group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_PEELING);

	osgViewer::Viewer viewer;

	viewer.addEventHandler(new EventHandler(group));
	viewer.setSceneData(group);
	viewer.setUpViewInWindow(50, 50, 512, 512);
	// viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	return viewer.run();
}

