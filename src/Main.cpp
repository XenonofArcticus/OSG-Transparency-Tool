#include "Reader.h"

#include <osg/LightModel>
#include <osgGA/StateSetManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

typedef std::vector<std::string> EBCFiles;

#define HELP_ARGS(args) args.getApplicationUsage()->write( \
	osg::notify(osg::NOTICE), \
	osg::ApplicationUsage::COMMAND_LINE_OPTION \
)

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
	osgtt::Reader reader;

	if(!reader.setCRDFile(args[1])) {
		OSG_FATAL << "Couldn't load CRD file '" << args[1] << "'; fatal." << std::endl;

		return 2;
	}

	osgViewer::Viewer viewer;
	osg::Group*       group = new osg::Group();
	
	// Iterate over all the EBC files and add them to our group.
	for(EBCFiles::iterator i = ebcFiles.begin(); i != ebcFiles.end(); i++) {
		osgtt::TransparentModel* model = reader.modelFromEBCFile(*i, osg::PrimitiveSet::TRIANGLES);

		model->setAlpha(0.5);
		model->setRGB(osg::Vec3(0.3, 0.6, 0.8));

		group->addChild(model);
	}

	// Set a two-sided FFP LightModel.
	osg::StateSet*   stateSet   = group->getOrCreateStateSet();
	osg::LightModel* lightModel = new osg::LightModel();

	lightModel->setTwoSided(true);

	stateSet->setAttribute(lightModel);

	viewer.addEventHandler(new osgViewer::StatsHandler());
	viewer.addEventHandler(new osgGA::StateSetManipulator(stateSet));
	viewer.setSceneData(group);

	return viewer.run();
}

#if 0
    osg::ref_ptr<osg::Vec3Array> vertices	= readVertices( crdFile );
    /* end of reading .crd and .ebc*/

    osg::ref_ptr<osgViewer::Viewer> viewer= new osgViewer::Viewer;

    osg::ref_ptr<osg::Group> root= new Group();
    osg::ref_ptr<osg::StateSet> st = root->getOrCreateStateSet();
    osg::ref_ptr< osg::LightModel> pLightModel = new osg::LightModel();
    pLightModel->setTwoSided( true );
    st->setAttribute(pLightModel.get());

    // We are setting color randomly.
    // build the scene data
    for( i=0; i<nEbcs; i++ )  {
        osg::ref_ptr<MyGroup> myGroup = new MyGroup( vertices.get(),3, ebcFiles[i],1);
        // myGroup->setColor(double(rand())/double(RAND_MAX),double(rand())/double(RAND_MAX),double(rand())/double(RAND_MAX));
        myGroup->setColor(C[i][0], C[i][1], C[i][2]);
	// setting transparency
        myGroup->setTransparency(0.5);
        //myGroup->setDisplay(2);
        root->addChild(myGroup);
    }

    viewer->addEventHandler( new osgViewer::StatsHandler() );
    viewer->setSceneData( root.get() );
    viewer->run();
    free(crdFile);
    free(ebcFiles);
#endif

