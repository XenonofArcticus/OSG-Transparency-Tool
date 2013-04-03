#ifndef MYGROUP_H
#define MYGROUP_H

#include <osg/Array>
#include <osg/Group>
#include <osg/Switch>
#include <osg/Geometry>

#if defined(_cplusplus)
	extern "C" {
#endif

/** A Group class.
 *  Creates the node of the scenegraph and performs 
 *  actions on the node. 
 */
 /*===========================================================================
 *
 * "MyGroup": Creates the group node of the scenegraph
 *
 *===========================================================================
 */
class MyGroup: public osg::Switch {
    public:
        MyGroup(   osg::Vec3Array* verts,
                    int nElemNodes,
                    char*   ebcFiles,
                    int     displayStyle);

        osg::ref_ptr<osg::DrawElementsUInt> readElements( char* ebcFile, osg::ref_ptr<osg::DrawElementsUInt> element, int nElemNodes );
        void    setColor( float r, float g, float b );
        void    setDisplay( int displayType );
        void    setTransparency ( float alpha); 

    private:
        void Normal(int nElemNodes);
        int _style;
        float alpha;
        osg::Vec3 color;
        osg::ref_ptr<osg::Vec3Array> _vertices;
        osg::ref_ptr<osg::Geode> geode_wire ;
        osg::ref_ptr<osg::Geode> geode_solid;
        osg::ref_ptr<osg::Geometry> _geom_wire;
        osg::ref_ptr<osg::Geometry> _geom_solid;
        osg::ref_ptr<osg::DrawElementsUInt> element ;
        osg::ref_ptr<osg::Vec4Array> color_wire ;
        osg::ref_ptr<osg::Vec4Array> color_solid ;

    protected:
       ~MyGroup();


};

#if defined(_cplusplus)
	}
#endif

#endif
