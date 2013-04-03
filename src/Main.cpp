#include <osgDB/ReadFile>
#include <osgGA/GUIEventHandler>
#include <iostream>
#include <osgViewer/Viewer>
#include "MyGroup.h"
#include <osg/LightModel>
#include <osgViewer/ViewerEventHandlers>
using namespace osg;


 /*===========================================================================
 *
 * "readVertices": Reads in the crd file and puts it into a Vec3 array. 
 *
 *===========================================================================
 */
osg::ref_ptr<osg::Vec3Array> readVertices( char* crdFile )
{
    int     i;
    FILE*   fp;
    char    buff[1024];
    char*   ptr;
    double  x,y,z;

    osg::ref_ptr<osg::Vec3Array> vertices= new osg::Vec3Array;
    fp  = fopen( crdFile, "r" );
    while( (ptr = fgets( buff, 1024, fp ) ) != NULL ) {
        sscanf( buff, "%d %le %le %le", &i,&x,&y,&z );
        vertices->push_back( osg::Vec3( x,y,z ) );
    }
    fclose( fp );
    return vertices.get();
}

 /*===========================================================================
 *
 * "main": Main function which runs the program.
 *
 *===========================================================================
 */
int main( int argc, char **argv )
{
    if(  argc != 3 ) {
        fprintf( stderr, "Usage: %s crdFile ebcFile1,ebcFile2..\n", argv[0]);
        return -1;
    }

    char*   crdFile	= NULL;
    char*   ebcList = NULL;
    char*   szp = NULL;
    char*   szq = NULL;
    char*   str = NULL;
    int     nEbcs;
    int     i;
    char**  ebcFiles;

    /* reads .crd and .ebc*/
    crdFile	= (char*) malloc ( sizeof(char)*(1+strlen(argv[1])));
    strcpy( crdFile, argv[1] );
    printf("crdFile = <%s>\n", crdFile );

    ebcList = (char*) malloc ( sizeof(char)*(1+strlen(argv[2]) ) );
    strcpy( ebcList, argv[2] );
    nEbcs   = 1;
    szq = ebcList;
    szp = strchr( szq,',' );
    if( szp == NULL ) {
        nEbcs	= 1;
    } else {
        while( szp != NULL ) {
            szq	= szp + 1;
            if( szq != NULL ) { nEbcs++; }
            *szp = '\0';
            szp = strchr( szq, ',' );
        }
    }
    printf("nEbcs = %d\n", nEbcs );
    szp = NULL;
    szq = NULL;
    free( ebcList );

    ebcList = (char*) malloc ( sizeof(char)*(1+strlen(argv[2]) ) );
    strcpy( ebcList, argv[2] );
    ebcFiles    = (char**) malloc( sizeof(char*)*nEbcs );
    szq = ebcList;
    for( i=0; i<nEbcs; i++ ) {
        szp	= strchr( szq,',' );
        if( szp != NULL ) { *szp = '\0'; }
        ebcFiles[i]	= (char*) malloc (sizeof(char)*(1+strlen(szq)));
        strcpy( ebcFiles[i], szq );
        szq = szp + 1;
    }
    free( ebcList );
    ebcList = NULL;
    for( i=0;i<nEbcs;i++) { printf("ebcFile = %s\n",ebcFiles[i]); }
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
        osg::ref_ptr<MyGroup> myGroup = new MyGroup( vertices.get(),3, ebcFiles[i],2 );
        myGroup->setColor(double(rand())/double(RAND_MAX),double(rand())/double(RAND_MAX),double(rand())/double(RAND_MAX));
	// setting transparency
        myGroup->setTransparency(0.5);
        myGroup->setDisplay(1);
        root->addChild(myGroup);
    }

    viewer->addEventHandler( new osgViewer::StatsHandler() );
    viewer->setSceneData( root.get() );
    viewer->run();
    free(crdFile);
    free(ebcFiles);
    return 0;
}
