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

		// TODO: This is a hack until Linux generates a single resize event on Viewer creation.
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

int main(int argc, char** argv) {
	osgtt::TransparencyGroup* group  = new osgtt::TransparencyGroup();
	osg::Geode*               geode  = new osg::Geode();
	osg::ShapeDrawable*       sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(), 0.3));
	osg::ShapeDrawable*       box    = new osg::ShapeDrawable(new osg::Box());
	osg::ShapeDrawable*       cone   = new osg::ShapeDrawable(new osg::Cone());

	sphere->setColor(osg::Vec4(1.0, 0.0, 0.0, 1.0));
	box->setColor(osg::Vec4(0.0, 1.0, 0.0, 0.5));
	cone->setColor(osg::Vec4(0.0, 0.0, 1.0, 0.5));

	geode->addDrawable(sphere);
	geode->addDrawable(box);
	geode->addDrawable(cone);

	osgtt::DepthPeeling* dp = new osgtt::DepthPeeling(512, 512);

	dp->setNumPasses(4);

	group->addChild(geode);
	group->setDepthPeeling(dp);
	group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_PEELING);

	osgViewer::Viewer viewer;

	viewer.addEventHandler(new EventHandler(group));
	viewer.setSceneData(group);
	viewer.setUpViewInWindow(50, 50, 512, 512);
	viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	return viewer.run();
}

