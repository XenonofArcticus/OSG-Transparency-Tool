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
			int  key   = gea.getKey();
			bool dirty = true;

			// Handle all the key presses; if an action DOESN'T need to update
			// the internal DepthPeeling peel stack, be sure and set dirty to false.
			if(key == 'q') dp->setNumPasses(dp->getNumPasses() + 1);

			else if(key == 'w') dp->setNumPasses(dp->getNumPasses() - 1);

			else if(key == 'a') dp->setOffsetValue(dp->getOffsetValue() + 1);

			else if(key == 's') dp->setOffsetValue(dp->getOffsetValue() - 1);
			
			else if(key == 'z') {
				_group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_SORTED_BIN);

				dirty = false;
			}
			
			else if(key == 'x') _group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_PEELING);

			else if(key == 'c') {
				_group->setTransparencyMode(osgtt::TransparencyGroup::DELAYED_BLEND);

				dirty = false;
			}

			else return false;

			if(dirty) dp->dirty();

			return true;
		}

		return false;
	}

protected:
	osg::ref_ptr<osgtt::TransparencyGroup> _group;
};

const osg::Vec4 COLORS[6] = {
	osg::Vec4(1.0, 0.0, 0.0, 0.25),
	osg::Vec4(1.0, 0.0, 0.0, 0.25),
	osg::Vec4(1.0, 0.0, 0.0, 0.25),
	osg::Vec4(1.0, 0.0, 0.0, 0.25),
	osg::Vec4(1.0, 0.0, 0.0, 0.25),
	osg::Vec4(1.0, 0.0, 0.0, 0.25)

	/*osg::Vec4(0.0, 0.0, 0.0, 0.5),
	osg::Vec4(1.0, 0.0, 0.0, 0.5), 
	osg::Vec4(0.0, 1.0, 0.0, 0.5),
	osg::Vec4(0.0, 0.0, 1.0, 0.25),
	osg::Vec4(1.0, 1.0, 0.0, 0.25),
	osg::Vec4(1.0, 1.0, 1.0, 0.25)*/
};

int main(int argc, char** argv) {
	osgtt::TransparencyGroup* group  = new osgtt::TransparencyGroup();
	osg::Geode*               geode  = new osg::Geode();

	// Add 6 spheres inside one another, like little Matryoshka dolls!
	for(unsigned int i = 0; i < 6; i++) {
		osg::ShapeDrawable* sphere = new osg::ShapeDrawable(new osg::Box(osg::Vec3(), (i + 1) * 2.0));

		sphere->setColor(COLORS[i]);

		geode->addDrawable(sphere);
	}

	osgtt::DepthPeeling* dp = new osgtt::DepthPeeling(512, 512);

	dp->setNumPasses(6);

	group->addChild(geode);
	group->setDepthPeeling(dp);
	group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_PEELING);

	osgViewer::Viewer viewer;

	// eliminate callback-related crash on exit
	viewer.setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

	viewer.addEventHandler(new EventHandler(group));
	viewer.setSceneData(group);
	viewer.setUpViewInWindow(50, 50, 512, 512);

	// TODO: Why is this necessary to avoid near/far computation problems when toggling between the
	// two different TransparencyModes?
	// viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	return viewer.run();
}

