#include "EBCReader.h"
#include "TransparencyGroup.h"
#include "DepthPeeling.h"

#include <osg/LightModel>
#include <osgGA/StateSetManipulator>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

typedef std::vector<std::string> EBCFiles;

#define HELP_ARGS(args) args.getApplicationUsage()->write( \
	osg::notify(osg::NOTICE), \
	osg::ApplicationUsage::COMMAND_LINE_OPTION \
)

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
	
			// OSG_WARN << "RESIZE " << ww << ", " << wh << std::endl;

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
	osg::ArgumentParser args(&argc, argv);

	args.getApplicationUsage()->setCommandLineUsage(args.getApplicationName() + " <CRD> <EBC Files...>");

	while(args.read("--help")) {
		HELP_ARGS(args);

		return 0;
	}

	if(args.argc() < 3) {
		HELP_ARGS(args);

		return 1;
	}

	// Get a list of one or more of the requested EBC files.
	EBCFiles ebcFiles;

	ebcFiles.push_back(args[2]);

	for(int i = 2; i < args.argc(); i++) ebcFiles.push_back(args[i]);

	// Create a Reader object and a Viewer; begin constructing our scene.
	osgtt::EBCReader reader;

	if(!reader.setCRDFile(args[1])) {
		OSG_FATAL << "Couldn't load CRD file '" << args[1] << "'; fatal." << std::endl;

		return 2;
	}

	osgtt::TransparencyGroup* group = new osgtt::TransparencyGroup();
	
	// Iterate over all the EBC files and add them to our group.
	for(EBCFiles::iterator i = ebcFiles.begin(); i != ebcFiles.end(); i++) {
		osgtt::EBCNode* node = reader.readEBCFile(*i, osg::PrimitiveSet::TRIANGLES);

		node->setAlpha(0.5);
		node->setRGB(osg::Vec3(0.3, 0.6, 0.8));

		group->addChild(node);
	}

	// Set a two-sided FFP LightModel.
	osg::StateSet*   stateSet   = group->getOrCreateStateSet();
	osg::LightModel* lightModel = new osg::LightModel();

	lightModel->setTwoSided(true);

	stateSet->setAttribute(lightModel);

	osgViewer::Viewer viewer;

	group->setDepthPeeling(new osgtt::DepthPeeling(512, 512));
	group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_PEELING);
	group->setName("TransparencyGroup");

	viewer.addEventHandler(new EventHandler(group));
	viewer.setSceneData(group);
	viewer.setUpViewInWindow(50, 50, 512, 512);
	viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	return viewer.run();
}

