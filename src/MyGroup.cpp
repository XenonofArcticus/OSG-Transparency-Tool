#include <osg/Geode>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include "MyGroup.h"
#include <iostream>
#include <osg/BlendFunc>
#include <stdio.h>

/*===========================================================================
 *
 * "Constructor"
 *
 *===========================================================================
 */
 
/**Constructor*/
MyGroup::MyGroup( 	osg::Vec3Array* 	verts,
                    	int 			nElemNodes,
                    	char*  			ebcFiles,
                    	int    			displayStyle	)
{
    _geom_wire  = new osg::Geometry;
    _geom_solid = new osg::Geometry;
    geode_wire = new osg::Geode;
    geode_solid = new osg::Geode;
    color_wire = new osg::Vec4Array;
    color_solid = new osg::Vec4Array;
    element = NULL;

     alpha = 1;
    _style  = displayStyle;
    _vertices = verts;
    _geom_wire->setUseDisplayList(   true    );
    _geom_wire->setVertexArray( _vertices.get()  );

    _geom_solid->setUseDisplayList( true    );
    _geom_solid->setVertexArray( _vertices.get()  );

    switch( nElemNodes ) {
        case 1:
            element = 
            new osg::DrawElementsUInt( osg::PrimitiveSet::POINTS, 0 );
            break;
        case 2:
            element = 
            new osg::DrawElementsUInt( osg::PrimitiveSet::LINES, 0 );
            break;
        case 3:
            element = 
            new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
            break;
        case 4:
            element = 
            new osg::DrawElementsUInt( osg::PrimitiveSet::QUADS, 0 );
            break;
    }

    element= readElements( ebcFiles, element, nElemNodes);

    _geom_wire->addPrimitiveSet(    element.get()   );
    _geom_solid->addPrimitiveSet(   element.get()   );

    geode_wire->addDrawable(    _geom_wire  );
    geode_solid->addDrawable(   _geom_solid );

    osg::ref_ptr<osg::StateSet> state   = NULL;
    osg::ref_ptr<osg::PolygonMode> pMode1   = new osg::PolygonMode;
    osg::ref_ptr<osg::PolygonMode> pMode2   = new osg::PolygonMode;

    state = geode_wire->getOrCreateStateSet();
    pMode1->setMode( osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
    color_wire->push_back(osg::Vec4(1,0,0,1));
    _geom_wire->setColorArray( color_wire.get() );
    _geom_wire->setColorBinding( osg::Geometry::BIND_OVERALL );
    state->setAttributeAndModes( pMode1.get(),osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON	);

    state = geode_solid->getOrCreateStateSet();
    pMode2->setMode( osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::FILL);
    color_solid->push_back(osg::Vec4(0,1,0,1));
    _geom_solid->setColorArray( color_solid.get() );
    _geom_solid->setColorBinding( osg::Geometry::BIND_OVERALL );
    osg::ref_ptr<osg::PolygonOffset> pOffset = new osg::PolygonOffset;
    pOffset->setFactor( 1.0f );
    pOffset->setUnits( 1.0f );
    state->setAttributeAndModes( pMode2.get(), osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );
    state->setAttributeAndModes( pOffset.get(), osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON );

    Normal( nElemNodes);
    addChild( geode_solid.get() );
    addChild( geode_wire.get() );

    setDisplay( displayStyle );

}

MyGroup::~MyGroup()
{

}

 /*===========================================================================
 *
 * "setColor": Sets a single binding color 
 *
 *===========================================================================
 */
  /**Sets a single binding color.*/
void 
MyGroup::setColor( float r, float g, float b )
{
    color_solid->clear();
    color_wire->clear();
    color_solid->push_back( osg::Vec4(r, g, b, alpha ) );
    switch( _style ) {
        case 0: //LINE
    {
        color_wire->push_back( osg::Vec4(r, g, b, alpha ) );
        break;
    }
    case 1: // SOLID
    {
        color_wire->push_back( osg::Vec4(r, g, b, alpha ) );
        break;
    }
    case 2: // SOLID+WIRE
    {
        color_wire->push_back( osg::Vec4(1.0-r, 1.0-g, 1.0-b, alpha ) );
        break;
    }
    }
    color_wire->dirty();
    color_solid->dirty();
}

 /*===========================================================================
 *
 * "setDisplay": Changes the display style .
 *
 *===========================================================================
 */
  /**Sets the display style.*/
void 
MyGroup::setDisplay( int displayStyle )
{
    osg::Vec3 c_solid = osg::Vec3(color_solid->begin()->x(), color_solid->begin()->y(), color_solid->begin()->z());
    if( _style == 2 && displayStyle != 2 ) {
        color_wire->clear();
        color_wire->push_back( osg::Vec4(c_solid,alpha) );
        _geom_wire->setColorArray( color_wire.get() );
    }
    if( displayStyle == 2 ) {
        color_wire->clear();
        color_wire->push_back( osg::Vec4(1-c_solid.x(), 1.0-c_solid.y(), 1.0-c_solid.z(), alpha) );
        color_wire->dirty();
        _geom_wire->setColorArray( color_wire.get() );
    }
    _style  = displayStyle;
    switch( displayStyle ) {
        case 0: //LINE
        {
            setChildValue(  geode_wire.get(), true );
            setChildValue(  geode_solid.get(), false   );
            break;
        }
        case 1: //SOLID
        {
            setChildValue(  geode_wire.get(), false    );
            setChildValue(  geode_solid.get(), true    );
            break;
        }
        case 2: //SOLID + WIRE
        {
            setChildValue(  geode_wire.get(), true );
            setChildValue(  geode_solid.get(), true    );
            break;
        }
        default: //SOLID
        {
            setChildValue(  geode_wire.get(), false    );
            setChildValue(  geode_solid.get(), true    );
            break;
        }
    } 
}

 /*===========================================================================
 *
 * "readElements": Reads in the ebc files .
 *
 *===========================================================================
 */
  /**Reads in the ebc files.*/
osg::ref_ptr<osg::DrawElementsUInt> 
MyGroup::readElements( char* ebcFile, osg::ref_ptr<osg::DrawElementsUInt> element, int nElemNodes )
{
    FILE*   fp;
    char    buff[1024];
    char*   ptr;
    int	    n1,n2,n3,n4;
    fp  = fopen( ebcFile, "r" );

    switch( nElemNodes ) 
    {
        case 3:
            while( (ptr = fgets( buff, 1024, fp ) ) != NULL ) {
                sscanf( buff, "%d %d %d", &n1,&n2,&n3 );
                element->push_back( n1 );
                element->push_back( n2 );
                element->push_back( n3 );
            }
            break;
        case 4:
            while( (ptr = fgets( buff, 1024, fp ) ) != NULL ) {
                sscanf( buff, "%d %d %d %d", &n1,&n2,&n3, &n4 );
                element->push_back( n1 );
                element->push_back( n2 );
                element->push_back( n3 );
                element->push_back( n4 );
            }
            break;
    }
    fclose( fp );
    return element;
}

/*===========================================================================
 *
 * "setTransparency": sets transparency
 *
 *===========================================================================
 */
void
MyGroup::setTransparency( float value)
{
    osg::ref_ptr<osg::StateSet> transparency = new osg::StateSet;
    osg::Vec3 c_solid = osg::Vec3(color_solid->begin()->x(), color_solid->begin()->y(), color_solid->begin()->z());
    osg::Vec3 c_wire = osg::Vec3(color_wire->begin()->x(), color_wire->begin()->y(), color_wire->begin()->z());
    color_solid->clear();
    color_wire->clear();

    if(value<1){
        color_solid->push_back( osg::Vec4(c_solid,value) );
        color_wire->push_back( osg::Vec4(c_wire,value) );
        transparency->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
        transparency->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA ,GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON); 
        alpha = value;
    } else {
        color_solid->push_back( osg::Vec4(c_solid,1.0) );
        color_wire->push_back( osg::Vec4(c_wire,1.0) );
        transparency->setRenderingHint(osg::StateSet::OPAQUE_BIN);
        alpha = 1.0;
    }

    _geom_solid->setStateSet(transparency);
    _geom_wire->setStateSet(transparency);
    color_wire->dirty();
    color_solid->dirty();

}


/*===========================================================================
 *
 * "Normal":
 *
 *===========================================================================
 */
void 
MyGroup::Normal(int nElemNodes)
{
    int i;
    int j;
    osg::Vec3f normal;
    osg::ref_ptr<osg::Vec3Array> _normal = new osg::Vec3Array;
    // compute normals if needed
    for ( i = 0; i <  element->size()/nElemNodes; ++i ) {
        osg::Vec3f pos[3];
        for ( j = 0; j < 3; ++j ) {
            pos[j] = (*_vertices)[(*element)[nElemNodes*i+j]];
        }
        normal = (pos[1]-pos[0])^(pos[2]-pos[0]);
        normal.normalize();
        _normal->push_back(normal);
    }
    _geom_solid->setNormalArray(_normal.get());
    _geom_solid->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE);

    _geom_wire->setNormalArray(_normal.get());
    _geom_wire->setNormalBinding( osg::Geometry::BIND_PER_PRIMITIVE);

}
