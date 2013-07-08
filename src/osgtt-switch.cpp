#include "TransparencyGroup.h"
#include "DepthPeeling.h"

#include <osg/ShapeDrawable>
#include <osgViewer/Viewer>
#include <osg/BlendFunc>

class EventHandler: public osgGA::GUIEventHandler {
public:
	EventHandler(osg::Switch* switcher, osgtt::DepthPeeling* dp):
	_switcher (switcher),
	_dp       (dp) {
	}

	virtual bool handle(
		const osgGA::GUIEventAdapter& gea,
		osgGA::GUIActionAdapter&      gaa,
		osg::Object*                  obj,
		osg::NodeVisitor*             nv
	) {
		if(!_switcher) return false;

		if(!_dp) return false;

		/*
		if(gea.getEventType() == osgGA::GUIEventAdapter::RESIZE) {
			unsigned int ww = gea.getWindowWidth();
			unsigned int wh = gea.getWindowHeight();
	
			_dp->resize(ww, wh);

			return true;
		}
		*/

		if(gea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN) {
			int key = gea.getKey();

			if(key == 'q') _dp->setNumPasses(_dp->getNumPasses() + 1);

			else if(key == 'w') _dp->setNumPasses(_dp->getNumPasses() - 1);

			else if(key == 'a') _dp->setOffsetValue(_dp->getOffsetValue() + 1);

			else if(key == 's') _dp->setOffsetValue(_dp->getOffsetValue() - 1);
			
			else if(key == 'z') {
				_switcher->setValue(0, true);
				_switcher->setValue(1, false);
			}
			
			else if(key == 'x') {
				_switcher->setValue(0, false);
				_switcher->setValue(1, true);
			}

			else return false;

			return true;
		}

		return false;
	}

protected:
	osg::ref_ptr<osg::Switch>         _switcher;
	osg::ref_ptr<osgtt::DepthPeeling> _dp;
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
	osg::Switch* switcher = new osg::Switch();
	osg::Geode*  geode    = new osg::Geode();

	// Add 8 spheres inside one another, like little Matryoshka dolls!
	for(unsigned int i = 0; i < 8; i++) {
		osg::ShapeDrawable* sphere = new osg::ShapeDrawable(new osg::Box(osg::Vec3(), (i + 1) * 2.0));

		sphere->setColor(COLORS[i]);

		geode->addDrawable(sphere);
	}

	osgtt::DepthPeeling* dp = new osgtt::DepthPeeling(512, 512);

	dp->setNumPasses(8);

	switcher->addChild(dp, false);
	switcher->addChild(geode, true);

	switcher->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	switcher->getOrCreateStateSet()->setAttributeAndModes(
		new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
		osg::StateAttribute::ON
	);

	dp->setScene(geode);
	dp->dirty();

	osgViewer::Viewer viewer;

	viewer.addEventHandler(new EventHandler(switcher, dp));
	viewer.setSceneData(switcher);
	viewer.setUpViewInWindow(50, 50, 512, 512);
	viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	return viewer.run();
}

