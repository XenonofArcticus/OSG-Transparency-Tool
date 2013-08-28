#include "EBCReader.h"
#include "TransparencyGroup.h"
#include "DepthPeeling.h"
#include <cstdio>
#include <sstream>
#include <osg/LightModel>
#include <osgGA/StateSetManipulator>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <string.h>
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
		osgGA::GUIActionAdapter&	  gaa,
		osg::Object*				  obj,
		osg::NodeVisitor*			  nv
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

			if(key == 'q') {
				dp->setNumPasses(dp->getNumPasses() + 1);
				printf("number of passes: %d\n",dp->getNumPasses());
			}

			else if(key == 'w') dp->setNumPasses(dp->getNumPasses() - 1);

			else if(key == 'a') dp->setOffsetValue(dp->getOffsetValue() + 1);

			else if(key == 's') dp->setOffsetValue(dp->getOffsetValue() - 1);
			
			else if(key == 'z') _group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_SORTED_BIN);
			
			else if(key == 'x') _group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_PEELING);

			else if(key == 'c') _group->setTransparencyMode(osgtt::TransparencyGroup::DELAYED_BLEND);

			else if(key == 'v') _group->setTransparencyMode(osgtt::TransparencyGroup::NO_TRANSPARENCY);

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
    args.getApplicationUsage()->setCommandLineUsage(args.getApplicationName() + " <Input file>");

    while(args.read("--help")) {
        HELP_ARGS(args);

        return 0;
    }

    if(args.argc() != 2) {
        HELP_ARGS(args);
        return 1;
    }

    char buff[1024];
    FILE* fp = std::fopen(args[1], "r");
    char* ptr = 0;
    if (!fp) {
        printf("can not open file: %s\n",args[1]);
        return 0;
    }
    
    // Create a Reader object and a Viewer; begin constructing our scene.
    osgtt::EBCReader reader;
    ptr = std::fgets(buff,1024, fp);
    char crd[1024];
    sscanf(buff, "%s", &crd);
    
    EBCFiles ebcFiles;
    osg::ref_ptr<osg::Vec4Array> Color = new osg::Vec4Array;
    osgtt::TransparencyGroup* group = new osgtt::TransparencyGroup();
    std::vector<int> side_flag;
    
    char	*p;
    char	*q;
    while((ptr = std::fgets(buff, 1024, fp))) {
        p = buff;
        /* strip any space,# or new line */
        if ((q = strchr(p,' ')) != NULL ||(q = strchr(p,'#')) != NULL ||(q = strchr(p,'\n'))!= NULL )
            *q = '\0';
        ebcFiles.push_back(buff);            
        
        ptr = std::fgets(buff, 1024, fp);
        int flag;
        sscanf(buff, "%d", &flag);
        side_flag.push_back(flag);
        
        ptr = std::fgets(buff, 1024, fp);
        float alpha;
        sscanf(buff, "%f", &alpha);

        ptr = std::fgets(buff, 1024, fp);
        float r,g,b;
        sscanf(buff, "%f %f %f", &r,&g,&b);
        Color->push_back( osg::Vec4( r,g,b,alpha ) );
    }
    std::fclose(fp);
    
    if(!reader.setCRDFile(crd)) {
        OSG_FATAL << "Couldn't load CRD file '" << crd << "'; fatal." << std::endl;
        return 2;
    }
    
    int id = 0;
    // Iterate over all the EBC files and add them to our group.
    for(EBCFiles::iterator i = ebcFiles.begin(); i != ebcFiles.end(); i++) {
        osgtt::EBCNode* node = reader.readEBCFile(*i, osg::PrimitiveSet::TRIANGLES);
        osg::Vec4 color = (*Color)[id];
        int flag = (side_flag)[id];
        
        printf("%s\n",(*i).c_str());
        
        if (flag) printf("single-sided\n");
        else printf("double-sided\n");
        
        printf("color %f %f %f %f\n",color.x(),color.y(),color.z(), color.w());
        
        if(color.w()<1){
            group->addChild(node, true); // true = render as transparent
        }
        else{
            group->addChild(node, false); // false = hint that it's not transparent
        }
        node->setColor(color);
        id++;
    }
    // Set a two-sided FFP LightModel.
    osg::StateSet*	 stateSet	= group->getOrCreateStateSet();
    osg::LightModel* lightModel = new osg::LightModel();

    lightModel->setTwoSided(true);

    stateSet->setAttribute(lightModel);

    osgViewer::Viewer viewer;

    // eliminate callback-related crash on exit
    viewer.setThreadingModel(osgViewer::ViewerBase::SingleThreaded);

    group->setDepthPeeling(new osgtt::DepthPeeling(512, 512));
    group->setTransparencyMode(osgtt::TransparencyGroup::DEPTH_PEELING);
    group->setName("TransparencyGroup");

    viewer.addEventHandler(new EventHandler(group));
    viewer.setSceneData(group);
    viewer.setUpViewInWindow(50, 50, 512, 512);
    viewer.getCamera()->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

    return viewer.run();
}

