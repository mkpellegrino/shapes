#include <iostream>
#include <math.h>
#include <vector>
#include <chrono>


#define SCENEWIDTH 300
#define SCENEHEIGHT 300
#define SCENECENTERX SCENEWIDTH/2
#define SCENECENTERY SCENEHEIGHT/2
#define SCALE 10
#define GRIDX 15
#define D 16
#define XR 4
#define CENTERX SCALE*GRIDX
#define FIDELITY 0.1256636

using namespace std;
class Shape;


#ifndef SVGCOORD_H
#define SVGCPPRD_H
class SVGCoord
{
public:
  SVGCoord(double a, double b){x=a;y=b;};
  
  double X(){return x;};
  double Y(){return y;};
  friend ostream &operator << (ostream &out, const SVGCoord &c); 
  
private:
  double x;
  double y;
};
#endif

ostream & operator << (ostream &out, const SVGCoord &C) 
{
  out << "(" << C.x << "," << C.y << ")";
  return out;
}
#ifndef COORDINATE_H
#define COORDINATE_H

class Coordinate
{
public:
  // Constructors
  Coordinate(); // The Origin
  Coordinate( double x, double y, double z );
  Coordinate( Coordinate * C );

  // Destructor
  ~Coordinate();

  // Methods
  double getX(){return x;};
  double getY(){return y;};
  double getZ(){return z;};

  Coordinate * getProjection(double c);
  
  void setX(double x){this->x=x;};
  void setY(double y){this->y=y;};
  void setZ(double z){this->z=z;};
  void stretchX( double d ){ this->x*=d; };
  void stretchY( double d ){ this->y*=d; };
  void stretchZ( double d ){ this->z*=d; };
  
  void rotate( double angle, Coordinate * c );
  void rotateXY( double angle, Coordinate * c){ rotate( angle, c ); };
  void rotateXZ( double angle, Coordinate * c );
  void rotateYZ( double angle, Coordinate * c );
  void translate( Coordinate * x );
  
  void outputSVG();
  void outputStereoSVG();
  friend ostream &operator << (ostream &out, const Coordinate &c); 
private:
  double x;
  double y;
  double z;
  Coordinate * projection;
  Coordinate * projectionL;
  Coordinate * projectionR;
};

#endif
Coordinate * Coordinate::getProjection(double c=0)
{
#ifdef DEBUG
  cerr << "[ Coordinate * Coordinate::getProjection(" << c << ") ]" << endl; 
#endif
  if(c!=0) c/=abs(c);
  double shift=-XR*c; // For viewing by crossing your eyes
  //double shift=XR*c; // For viewing in 3D goggles.
  projection = new Coordinate( (D*x-shift)/(z+D), (D*y)/(z+D), 0 );
  if( c==1 ) projection->translate( new Coordinate(/*20*/1,0,0));
  return projection;
}


Coordinate::Coordinate( double x, double y, double z=0 )
{
#ifdef DEBUG
  cerr << "[ Coordinate::Coordinate( double x, double y, double z=0 ) ]" << endl; 
#endif
  this->x=x;
  this->y=y;
  this->z=z;
}

Coordinate::Coordinate( Coordinate * c )
{
#ifdef DEBUG
  cerr << "[ Coordinate::Coordinate( Coordinate * C ) ]" << endl;
#endif
  x=c->getX();
  y=c->getY();
  z=c->getZ();
}

Coordinate::Coordinate()
{
#ifdef DEBUG
  cerr << "[ Coordinate::Coordinate() ]" << endl;
#endif
  this->x=0;
  this->y=0;
  this->z=0;
}

Coordinate::~Coordinate()
{
#ifdef DEBUG
  cerr << "[ Coordinate::~Coordinate() ]" << endl;
#endif
}

void Coordinate::translate( Coordinate * c )
{
#ifdef DEBUG
  cerr << "[ Coordinate::translate( " << *c << ") ]" << endl;
#endif
  x+=c->getX();
  y+=c->getY();
  z+=c->getZ();
}

void Coordinate::rotateXZ( double angle, Coordinate * c = new Coordinate())
{
#ifdef DEBUG
  cerr << "[ void Coordinate::rotateXZ(double " << angle << ", Coordinate"<< *c << " )\t\t\t*** rotate on the XZ plane *** ]" << endl;
#endif
  double tmpX = c->getX();
  double tmpZ = c->getZ();
  x-=tmpX;
  z-=tmpZ;
  
  double radian_measure = 3.14159*angle/180;
  double newx = round((x*cos(radian_measure)-z*sin(radian_measure))*10000)/10000;
  double newz = round((x*sin(radian_measure)+z*cos(radian_measure))*10000)/10000;

  setX( newx+tmpX );
  setZ( newz+tmpZ );
}
void Coordinate::rotateYZ( double angle, Coordinate * c = new Coordinate())
{
#ifdef DEBUG
  cerr << "[ void Coordinate::rotateXZ(double " << angle << ", Coordinate"<< *c << " )\t\t\t*** rotate on the XZ plane *** ]" << endl;
#endif
  double tmpY = c->getY();
  double tmpZ = c->getZ();
  y-=tmpY;
  z-=tmpZ;
  
  double radian_measure = 3.14159*angle/180;
  double newy = round((y*cos(radian_measure)-z*sin(radian_measure))*10000)/10000;
  double newz = round((y*sin(radian_measure)+z*cos(radian_measure))*10000)/10000;

  setY( newy+tmpY );
  setZ( newz+tmpZ );
}

void Coordinate::rotate(double angle, Coordinate * c = new Coordinate())
{
#ifdef DEBUG
  cerr << "[ void Coordinate::rotate(double " << angle << ", Coordinate"<< *c << " )\t\t\t*** rotate on the XY plane *** ]" << endl;
#endif
  double tmpX = c->getX();
  double tmpY = c->getY();
  x-=tmpX;
  y-=tmpY;
  
  double radian_measure = 3.14159*angle/180;
  double newx = round((x*cos(radian_measure)-y*sin(radian_measure))*10000)/10000;
  double newy = round((x*sin(radian_measure)+y*cos(radian_measure))*10000)/10000;

  setX( newx+tmpX );
  setY( newy+tmpY );
}

// Forward Declaration
//class Scene;

// ----------------------------------------------
#ifndef SCENE_H
#define SCENE_H
class Scene
{
public:
  Scene( int h, int w, int x, int y, int s);
  Scene( int h, int w );
  double getSVGX(double x);
  double getSVGY(double y);
  SVGCoord * SVGXY( double x, double y );
  SVGCoord * SVGXY( Coordinate * C, int proj);
  void initSVG();
  void endSVG();
private:
  
  int width;
  int height;
  int scale;
  int gridx;
  int gridy;
};
#endif
// -----------------------------------------------



void Coordinate::outputSVG()
{
#ifdef DEBUG
  cerr << "[ void Coordinate::outputSVG() ]" << endl;
  cout << "<!-- drawing a coordinate -->" << endl;
#endif
  Scene * S = new Scene(SCENEHEIGHT,SCENEWIDTH);
  SVGCoord * c = S->SVGXY( this, 0 );  

  cout << "<circle cx=\"" << c->X() << "\" cy=\"" << c->Y() << "\" r=\"1\" stroke=\"red\" fill=\"red\" stroke-width=\"2\"/>" << endl;
  delete c;
  delete S;
}

void Coordinate::outputStereoSVG()
{
#ifdef DEBUG
  cerr << "[ void Coordinate::outputStereoSVG() ]" << endl;
  cout << "<!-- drawing a coordinate (left projection)-->" << endl;
#endif
  Scene * S = new Scene(SCENEHEIGHT,SCENEWIDTH);
  SVGCoord * c = S->SVGXY( this, -1);  

  cout << "<circle cx=\"" << c->X() << "\" cy=\"" << c->Y() << "\" r=\"1\" stroke=\"red\" fill=\"red\" stroke-width=\"2\"/>" << endl;
#ifdef DEBUG
  delete c;
#endif
  c = S->SVGXY( this, 1 );
  
#ifdef DEBUG
  cout << "<!-- drawing a coordinate (right projection)-->" << endl;
#endif
  cout << "<circle cx=\"" << c->X() << "\" cy=\"" << c->Y() << "\" r=\"1\" stroke=\"red\" fill=\"red\" stroke-width=\"2\"/>" << endl;

  delete c;
}

ostream & operator << (ostream &out, const Coordinate &C) 
{
  out << "(" << C.x << "," << C.y << "," << C.z << ")";
#ifdef DEBUG
  out << " projects to: (" << (D*C.x)/(C.z+D) << "," << (D*C.y)/(C.z+D) << ",0)";
#endif
  return out;
}
// ==============================================================
// ==============================================================
#ifndef SHAPE_H
#define SHAPE_H

class Shape
{
public:
  // Constructor
  Shape();

  // Copy constructor 
  Shape(const Shape &s2);
  // Destructor
  ~Shape();

  // Methods
  void setColour(int c);
  int getColour(){return colour;};
  void outputColour();
  void rotate( double angle, Coordinate * c );
  void rotateXY( double angle, Coordinate * c ){ rotate( angle, c );};
  void rotateXZ( double angle, Coordinate * c );
  void rotateYZ( double angle, Coordinate * c );
  void setFill( bool b ){ fill=b; };
  void translate( Coordinate * c );
  void translate( double x, double y, double z ){ translate( new Coordinate(x,y,z)); };
  double getPerimeter();
  double getVolume();
  double getSurfaceArea();
  void add( Coordinate * c );
  Coordinate * getCentroid();
  void scale( double d );
  virtual void outputSVG();
  virtual void outputStereoSVG();
  double distance_from_viewer();
  void dumpCoordinates();
  void stretchZ( double d );
  void stretchX( double d );
  void stretchY( double d );
  void stretch( double d );

  
private:
protected:
  int colour;
  bool fill;
  double dfv;
  double dArea;
  double dVolume;
  double dSurfaceArea;
  double dDistanceAround;
  vector<Coordinate*> coordinates;
};
#endif

void Shape::setColour( int c )
{
#ifdef DEBUG
  cerr << "void Shape::setColour( " << c << " )" << endl;
#endif
  colour=c;
  
}

void Shape::outputColour()
{
#ifdef DEBUG
  cerr << "Shape::outputColour()" << endl;
  cerr << "<!-- setting colour -->" << endl;
#endif
  
  switch( colour )
    {
    case 0:
      cout << "black";
      break;
      
    case 1:
      cout << "white";
      break;
      
    case 2:
      cout << "blue";
      break;
      
    case 3:
      cout << "red";
      break;
      
    case 4:
      cout << "yellow";
      break;
      
    case 5:
      cout << "green";
      break;
      
    case 6:
      cout << "grey";
      break;
      
    case 7:
      cout << "brown";
      break;
      
    case 8:
      cout << "pink";
      break;
      
    }
}

Shape::Shape( const Shape &s2 )
{
  fill = s2.fill;
  colour = s2.colour;
  dfv=s2.dfv;
  dArea=s2.dArea;
  dVolume=s2.dVolume;
  dSurfaceArea=s2.dSurfaceArea;
  dDistanceAround=s2.dDistanceAround;
  for( int i=0; i<s2.coordinates.size(); i++ )
    {
      Coordinate * newCoord = new Coordinate( s2.coordinates[i] );
      coordinates.push_back(newCoord);
    }
}

void Shape::dumpCoordinates()
{
#ifdef DEBUG
  cerr << "[ Shape::dumpCoordinates() ]" << endl;
#endif

  for( int i=0; i<coordinates.size(); i++ )
    {
      cout << *coordinates[i] << endl;
    }
}
void Shape::stretchZ( double d )
{
#ifdef DEBUG
  cerr << "[ Shape::stretchZ( " << d << " ) ]" << endl;
#endif
  for( int i=0; i<coordinates.size(); i++ )
    {
      coordinates[i]->stretchZ(d);
    }
}

void Shape::stretchX( double d )
{
#ifdef DEBUG
  cerr << "[ Shape::stretchX( " << d << " ) ]" << endl;
#endif
  for( int i=0; i<coordinates.size(); i++ )
    {
      coordinates[i]->stretchX(d);
    }
}
void Shape::stretchY( double d )
{
#ifdef DEBUG
  cerr << "[ Shape::stretchY( " << d << " ) ]" << endl;
#endif
  for( int i=0; i<coordinates.size(); i++ )
    {
      coordinates[i]->stretchY(d);
    }
}

void Shape::stretch( double d )
{
#ifdef DEBUG
  cerr << "[ Shape::stretch( " << d << " ) ]" << endl;
#endif
  stretchX(d);
  stretchY(d);
  stretchZ(d);
}

  
void Shape::translate( Coordinate * c )
{
#ifdef DEBUG
  cerr << "[ Shape::translate( " << *c << " ) ]" << endl;
#endif
  for( int i=0; i<coordinates.size(); i++ )
    {
      coordinates[i]->translate( c );
    } 
}

void Shape::scale( double d )
{
#ifdef DEBUG
  cerr << "[ Shape::scale( " << d << " ) ]" << endl;
#endif
  Coordinate * c = this->getCentroid();

  // go through each coordinate and move them
  for( int i=0; i<coordinates.size(); i++ )
    {
      double deltax = coordinates[i]->getX() - c->getX();
      double deltay = coordinates[i]->getY() - c->getY();
      double deltaz = coordinates[i]->getZ() - c->getZ();
      deltax*=d;
      deltay*=d;
      deltaz*=d;
      coordinates[i]->setX( c->getX() + deltax );
      coordinates[i]->setY( c->getY() + deltay );
      coordinates[i]->setZ( c->getZ() + deltaz );
    }  
}


Coordinate * Shape::getCentroid()
{
#ifdef DEBUG
  cerr << "[ Coordinate * Shape::getCentroid() ]" << endl;
#endif
  double sumX=0;
  double sumY=0;
  double sumZ=0;
  for( int i=0; i<coordinates.size(); i++ )
    {
      sumX+=coordinates[i]->getX();
      sumY+=coordinates[i]->getY();
      sumZ+=coordinates[i]->getZ();
    }
  sumX/=coordinates.size();
  sumY/=coordinates.size();
  sumZ/=coordinates.size();

  return ( new Coordinate( sumX, sumY, sumZ ) );
}

void Shape::add( Coordinate * c )
{
#ifdef DEBUG
  cerr << "[ void Shape::add( Coordinate * c ) ]" << endl;
#endif
  coordinates.push_back( c );
}

double Shape::getPerimeter()
{
#ifdef DEBUG
  cerr << "[ double Shape::getPerimeter() ]" << endl;
#endif

  dDistanceAround=0;
  for( int i=0; i<coordinates.size()-1; i++ )
    {
      dDistanceAround+=sqrt( (coordinates[i]->getX() - coordinates[i+1]->getX())*(coordinates[i]->getX() - coordinates[i+1]->getX()) + (coordinates[i]->getY() - coordinates[i+1]->getY())*(coordinates[i]->getY() - coordinates[i+1]->getY()));
    }
  dDistanceAround+=sqrt( (coordinates[0]->getX() - coordinates[coordinates.size()-1]->getX())*(coordinates[0]->getX() - coordinates[coordinates.size()-1]->getX()) + (coordinates[0]->getY() - coordinates[coordinates.size()-1]->getY())*(coordinates[0]->getY() - coordinates[coordinates.size()-1]->getY()));
  return dDistanceAround;
}

void Shape::outputSVG()
{
#ifdef DEBUG
  cerr << "[ virtual void Shape::outputSVG() ]" << endl;
  cout << "<!-- drawing a Shape -->" << endl;
#endif
  Scene * S = new Scene(SCENEHEIGHT,SCENEWIDTH);
  if( coordinates.size()==0) return;
  if( coordinates.size()==1)
    {
#ifdef DEBUG
      cout << "<!-- just a Point -->" << endl;
#endif
  Scene * S = new Scene(SCENEHEIGHT,SCENEWIDTH);
  SVGCoord * c = S->SVGXY( coordinates[0], 0 );  

  cout << "<circle cx=\"" << c->X() << "\" cy=\"" << c->Y() << "\" r=\"1\" stroke=\""; outputColour(); cout <<"\" fill=\""; outputColour(); cout <<  "\" stroke-width=\"2\"/>" << endl;
  delete c;
  delete S;
    }
  else if( coordinates.size()==2 )
    {
#ifdef DEBUG
      cout << "<!-- just a line segment -->" << endl;
#endif
      SVGCoord * c0 = S->SVGXY( coordinates[0],0);  
      SVGCoord * c1 = S->SVGXY( coordinates[1],0);   
      cout << "<line x1=\"" << c0->X() << "\" y1=\"" << c0->Y() << "\" x2=\"" << c1->X() << "\" y2=\"" << c1->Y() << "\" stroke=\""; outputColour(); cout << "\" stroke-width=\"1\"/>" << endl;
      delete c0;
      delete c1;
    }
  else
    {
#ifdef DEBUG
      cout << "<!-- NOT just a line segment (A POLYGON) -->" << endl;
#endif
      cout << "<polygon points=\"";

      for( int i=0; i<coordinates.size(); i++ )
	{
	  SVGCoord * c0 = S->SVGXY( coordinates[i],0 );
	  cout << c0->X() << "," << c0->Y() << " ";
	}

      
      cout << "\" ";
      if( fill )
	{
	  cout << "fill=\""; outputColour(); cout << "\""; 
	}
      else
	{
	  cout << "fill=\"none\"";
	}
      cout << " stroke=\""; outputColour(); cout << "\"";
      cout << "/>" << endl;

      if( fill )// the draw the wireframe in either white or black
	{
	  cout << "<polygon points=\"";
	  
	  for( int i=0; i<coordinates.size(); i++ )
	    {
	      SVGCoord * c0 = S->SVGXY( coordinates[i],0 );
	      cout << c0->X() << "," << c0->Y() << " ";
	    }
	  
	  cout << "\" ";
	  cout << "fill=\"none\" stroke=\"";
	  if( colour==0)
	    {
	      cout << "green";
	    }
	  else
	    {
	      cout << "black";
	    }
	  cout << "\"";
	  cout << "/>" << endl;
	}
    }
  delete S;
}

void Shape::outputStereoSVG()
{
#ifdef DEBUG
  cerr << "[ virtual void Shape::outputStereoSVG() ]" << endl;
  cout << "<!-- drawing a Shape -->" << endl;
#endif
  Scene * S = new Scene(SCENEHEIGHT,SCENEWIDTH);
  if( coordinates.size()==0 ) return;
  if( coordinates.size()==1 )
    {
#ifdef DEBUG
      cout << "<!-- just a Point -->" << endl;
#endif
      Scene * S = new Scene(SCENEHEIGHT,SCENEWIDTH);
      SVGCoord * c = S->SVGXY( coordinates[0],-1 );  
      cout << "<circle cx=\"" << c->X() << "\" cy=\"" << c->Y() << "\" r=\"1\" stroke=\""; outputColour(); cout << "\" fill=\""; outputColour();  cout << "\" stroke-width=\"2\"/>" << endl;
#ifdef DEBUG
      delete c;
#endif
      c = S->SVGXY( coordinates[0], 1 );  
      cout << "<circle cx=\"" << c->X() << "\" cy=\"" << c->Y() << "\" r=\"1\" stroke=\""; outputColour();  cout << "\" fill=\""; outputColour(); cout << "\" stroke-width=\"2\"/>" << endl;
      delete c;
      delete S;
      
    }
  else if( coordinates.size()==2 )
    {
#ifdef DEBUG
      cout << "<!-- just a line segment -->" << endl;
#endif
      SVGCoord * c0 = S->SVGXY( coordinates[0],-1);  
      SVGCoord * c1 = S->SVGXY( coordinates[1],-1);   
      cout << "<line x1=\"" << c0->X() << "\" y1=\"" << c0->Y() << "\" x2=\"" << c1->X() << "\" y2=\"" << c1->Y() << "\" stroke=\""; outputColour(); cout << "\" stroke-width=\"1\"/>" << endl;
      delete c0;
      delete c1;
    }
  else
    {
#ifdef DEBUG
      cout << "<!-- NOT just a line segment -->" << endl;
#endif

      cout << "<polygon points=\"";
      for( int i=0; i<coordinates.size(); i++ )
	{
	  SVGCoord * c0 = S->SVGXY( coordinates[i],1 );
	  cout << c0->X() << "," << c0->Y() << " ";
	}

      
      cout << "\" ";
      if( fill )
	{
	  cout << "fill=\""; outputColour(); cout << "\""; 
	}
      else
	{
	  cout << "fill=\"none\"";
	}
      cout << " stroke=\""; outputColour(); cout << "\"";
      cout << "/>" << endl;

            cout << "<polygon points=\"";

      for( int i=0; i<coordinates.size(); i++ )
	{
	  SVGCoord * c0 = S->SVGXY( coordinates[i],-1 );
	  cout << c0->X() << "," << c0->Y() << " ";
	}

      
      cout << "\" ";
      if( fill )
	{
	  cout << "fill=\""; outputColour(); cout << "\""; 
	}
      else
	{
	  cout << "fill=\"none\"";
	}
      cout << " stroke=\""; outputColour(); cout << "\"";
      cout << "/>" << endl;


    }

  if( coordinates.size()==2 )
    {
#ifdef DEBUG
      cout << "<!-- just a line segment -->" << endl;
#endif
      SVGCoord * c0 = S->SVGXY( coordinates[0],1);  
      SVGCoord * c1 = S->SVGXY( coordinates[1],1);   
      cout << "<line x1=\"" << c0->X() << "\" y1=\"" << c0->Y() << "\" x2=\"" << c1->X() << "\" y2=\"" << c1->Y() << "\" stroke=\""; outputColour(); cout << "\" stroke-width=\"1\"/>" << endl;
      delete c0;
      delete c1;
    }
  else
    {
#ifdef DEBUG
      cout << "<!-- NOT just a line segment -->" << endl;
#endif
      for( int i=0; i<coordinates.size()-1; i++ )
	{
	  SVGCoord * c0 = S->SVGXY( coordinates[i],1);
	  SVGCoord * c1 = S->SVGXY( coordinates[i+1],1);
	  cout << "<line x1=\"" << c0->X() << "\" y1=\"" << c0->Y() << "\" x2=\"" << c1->X() << "\" y2=\"" << c1->Y() << "\" stroke=\""; outputColour(); cout << "\" stroke-width=\"1\"/>" << endl;
	  delete c0;
	  delete c1;
	}
      SVGCoord * c0 = S->SVGXY( coordinates[0],1);
      SVGCoord * c1 = S->SVGXY( coordinates[coordinates.size()-1],1);
      cout << "<line x1=\"" << c0->X() << "\" y1=\"" << c0->Y() << "\" x2=\"" << c1->X() << "\" y2=\"" << c1->Y() << "\" stroke=\""; outputColour(); cout << "\" stroke-width=\"1\"/>" << endl;;
      delete c0;
      delete c1;
    }

      if( fill )// then draw the wireframes in either white or black
	{
	  cout << "<polygon points=\"";
	  
	  for( int i=0; i<coordinates.size(); i++ )
	    {
	      SVGCoord * c0 = S->SVGXY( coordinates[i],-1 );
	      cout << c0->X() << "," << c0->Y() << " ";
	    }
	  
	  cout << "\" ";
	  cout << "fill=\"none\" stroke=\"";
	  if( colour==0)
	    {
	      cout << "white";
	    }
	  else
	    {
	      cout << "black";
	    }
	  cout << "\"";
	  cout << "/>" << endl;
	}
      if( fill )// then draw the wireframes in either white or black
	{
	  cout << "<polygon points=\"";
	  
	  for( int i=0; i<coordinates.size(); i++ )
	    {
	      SVGCoord * c0 = S->SVGXY( coordinates[i],1 );
	      cout << c0->X() << "," << c0->Y() << " ";
	    }
	  
	  cout << "\" ";
	  cout << "fill=\"none\" stroke=\"";
	  if( colour==0)
	    {
	      cout << "white";
	    }
	  else
	    {
	      cout << "black";
	    }
	  cout << "\"";
	  cout << "/>" << endl;
	}


  
  delete S;
}


Shape::Shape()
{
#ifdef DEBUG
  cerr << "[ Empty Shape Constructor ]" << endl;
#endif
  dArea=0;
  fill=false;
}

void Shape::rotate( double angle, Coordinate * c = new Coordinate(0,0))
{
  for( int i=0; i<coordinates.size(); i++ )
    {
      coordinates[i]->rotate( angle, c );
    }
}

void Shape::rotateXZ( double angle, Coordinate * c = new Coordinate(0,0))
{
  for( int i=0; i<coordinates.size(); i++ )
    {
      coordinates[i]->rotateXZ( angle, c );
    }
}
void Shape::rotateYZ( double angle, Coordinate * c = new Coordinate(0,0))
{
  for( int i=0; i<coordinates.size(); i++ )
    {
      coordinates[i]->rotateYZ( angle, c );
    }
}

Shape::~Shape()
{
#ifdef DEBUG
  cerr << "[ Shape Destructor ]" << endl;
#endif

  // Delete all the coordinates in the shape from memory
  for( int i=0; i<coordinates.size(); i++ )
    {
      delete coordinates[i];
      coordinates.erase(coordinates.begin()+i);
    }
}

double Shape::distance_from_viewer()
{
#ifdef DEBUG
  cerr << "Shape::distance_from_viewer()" << endl;
#endif
  Coordinate * centroid = new Coordinate(getCentroid());
  double x=centroid->getX();
  double y=centroid->getY();
  double z=centroid->getZ();
  dfv = sqrt( x*x+y*y+(z+D)*(z+D));
  return( dfv );
}

// ==============================================================
#ifndef COLLECTION_H
#define COLLECTION_H

class Collection
{
public:
  Collection(){};
  
  void add( Shape * S );
  void addCollection( Collection * C );
  void outputSVG();
  void outputStereoSVG();
  void translate( Coordinate * c );
  Coordinate * getCentroid();
  void rotateXY(double a, Coordinate * c);
  void rotateXZ(double a, Coordinate * c);
  void rotateYZ(double a, Coordinate * c);
  void stretchZ( double d );
  void stretchX( double d );
  void stretchY( double d );
  void scale( double d );
  void setFill( bool b );
 
  void sort();
  int getSize(){ return v.size(); };
  Shape * getShape(int i){ if(i>getSize()) return NULL; return v[i]; };
private:
  vector<Shape*> v;
};
#endif

void Collection::addCollection( Collection * C )
{
  for( int i=0; i<C->getSize(); i++ )
    {
      C->sort();
      add( C->getShape(i) );
    }

}

void Collection::scale( double d )
{
 for( int i=0; i<v.size(); i++ )
    {
      v[i]->scale(d);
    }

}

void Collection::setFill( bool b )
{
  for( int i=0; i<v.size(); i++ )
    {
      v[i]->setFill(b);
    }
}

Coordinate * Collection::getCentroid()
{
  double sumX=0;
  double sumY=0;
  double sumZ=0;
  for( int i=0; i<v.size(); i++ )
    {
      sumX+=v[i]->getCentroid()->getX();
      sumY+=v[i]->getCentroid()->getY();
      sumZ+=v[i]->getCentroid()->getZ();
    }
  sumX/=v.size();
  sumY/=v.size();
  sumZ/=v.size();

  return ( new Coordinate( sumX, sumY, sumZ ) );
  
}

void Collection::stretchX( double d )
{
  for( int i=0; i<v.size(); i++ )
    {
      v[i]->stretchX(d);
    }
}
void Collection::stretchY( double d )
{
  for( int i=0; i<v.size(); i++ )
    {
      v[i]->stretchY(d);
    }
}
void Collection::stretchZ( double d )
{
  for( int i=0; i<v.size(); i++ )
    {
      v[i]->stretchZ(d);
    }
}

void Collection::translate( Coordinate * c )
{
  for( int i=0; i<v.size(); i++ )
    {
      v[i]->translate(c);
    }
}
void Collection::rotateXY(double a, Coordinate * c)
{
  for( int i=0; i<v.size(); i++ )
    {
      v[i]->rotateXY( a, c );
    } 
}

void Collection::rotateXZ(double a, Coordinate * c)
{
  for( int i=0; i<v.size(); i++ )
    {
      v[i]->rotateXZ( a, c );
    } 
}
void Collection::rotateYZ(double a, Coordinate * c)
{
  for( int i=0; i<v.size(); i++ )
    {
      v[i]->rotateYZ( a, c );
    } 
}

void Collection::add( Shape * S2 )
{
  v.push_back(new Shape(*S2));
}


void Collection::sort()
{
#ifdef DEBUG
  cerr << "void Collection::sort()" << endl;
#endif
  if( v.size()<2 )return;
  for( int i=0; i<v.size()-1; i++ )
    {
      for( int j=i+1; j<v.size(); j++ )
	{
	  if( v[i]->distance_from_viewer() < v[j]->distance_from_viewer() )
	    {
	      Shape * tmpS = v[i];
	      v[i]=v[j];
	      v[j]=tmpS;
	    }
	}
    }
}
void Collection::outputSVG()
{
  sort();
  for( int i=0; i<v.size(); i++ ) v[i]->outputSVG();
}

void Collection::outputStereoSVG()
{
  sort();
  for( int i=0; i<v.size(); i++ ) v[i]->outputStereoSVG();
}



SVGCoord * Scene::SVGXY( Coordinate * C, int proj=0)
{
  int shift=0;
  if( proj==1 ) shift=2*CENTERX;
  
  SVGCoord * retVal = new SVGCoord( C->getProjection(proj)->getX()*scale+width/2 + shift, C->getProjection(proj)->getY()*(-scale)+height/2);
  return retVal;
}

SVGCoord * Scene::SVGXY( double x, double y )
{
  SVGCoord * retVal = new SVGCoord( scale*x+width/2, -scale*y+height/2);
  return retVal;
}

void Scene::endSVG()
{
  cout << "</svg>" << endl;
}

void Scene::initSVG()
{
  cout << "<svg width=\"" << width*4 << "px\" height=\"" << height << "px\" version=\"1.1\"><defs></defs>" << endl;
}

double Scene::getSVGX( double x )
{
  return scale*x+width/2;
}

double Scene::getSVGY( double y )
{
  return -scale*y+height/2;
}

Scene::Scene( int h, int w, int x, int y, int s)
{
  width=w;
  height=h;
  scale=s;
  gridx=x;
  gridy=y;
}

Scene::Scene( int h, int w )
{
  width=w;
  height=h;
  scale=10;
  gridx=15;
  gridy=15;
}
// ==============================================================
// ==============================================================
#ifndef LINE_H
#define LINE_H


class Line: public Shape
{
public:
  Line( Coordinate * C1, Coordinate * C2, int c);
  Line(){};
  double getArea(){ return 0; };
  double getPerimeter(){ return getLength(); };
  double getLength();
private:
protected:
};
#endif

Line::Line( Coordinate * C1, Coordinate * C2, int c=0)
{
#ifdef DEBUG
  cerr << "[ Line Constructor: " << *C1 << " to " << *C2 << " ]" << endl;
#endif
  setColour(c);
  coordinates.push_back(new Coordinate(C1));
  coordinates.push_back(new Coordinate(C2));
}

double Line::getLength()
{
  return sqrt( (coordinates[0]->getX()-coordinates[1]->getX())*(coordinates[0]->getX()-coordinates[1]->getX()) + (coordinates[0]->getY()-coordinates[1]->getY())*(coordinates[0]->getY()-coordinates[1]->getY()) + (coordinates[0]->getZ()-coordinates[1]->getZ())*(coordinates[0]->getZ()-coordinates[1]->getZ()) );
}
// ==============================================================
#ifndef POLYGON_H
#define POLYGON_H


class Polygon: public Shape
{
public:
  Polygon( vector<Coordinate*> v, int c);
  Polygon();
  double getDistanceAround(){ return getPerimeter(); };
  double getArea();
};
#endif

Polygon::Polygon( vector<Coordinate*> v, int c=3)
{
#ifdef DEBUG
  cerr << "[ double Polygon::Polygon( vector<Coordinate*> v) ]" << endl;
#endif
  setColour(c);
  coordinates=v;
}

Polygon::Polygon()
{
#ifdef DEBUG
  cerr << "[ Polygon::Polygon() ]" << endl;
#endif
  setColour(0);
}

double Polygon::getArea()
{
#ifdef DEBUG
  cerr << "[ double Polygon::getArea() ]" << endl;
#endif
  return 0;
}
// ==============================================================
#ifndef RECTANGLE_H
#define RECTANGLE_H


class Rectangle: public Shape
{
public:
  Rectangle( Coordinate * C1, Coordinate * C2, int c );
  double getDistanceAround(){ return getPerimeter(); };
  double getArea();

private:
  Rectangle(){};
};
#endif

Rectangle::Rectangle( Coordinate * C1, Coordinate * C2,int c=3 )
{
  setColour(c);
  coordinates.push_back( C1 );
  coordinates.push_back( new Coordinate( C1->getX(), C2->getY(), C1->getZ() ));
  coordinates.push_back( C2 );
  coordinates.push_back( new Coordinate( C2->getX(), C1->getY(), C2->getZ() ));
}

double Rectangle::getArea()
{
  double h=coordinates[0]->getX() - coordinates[2]->getX();
  double w=coordinates[0]->getY() - coordinates[2]->getY();
  return h*w;
}
// ==============================================================
#ifndef POINT_H
#define POINT_H
class Point: public Shape
{
public:
  Point( Coordinate * C1 );
  Point();
  double getX(){ return coordinates[0]->getX(); };
  double getY(){ return coordinates[0]->getY(); };
};
#endif

Point::Point( Coordinate * C1 )
{
#ifdef DEBUG
  cerr << "[ Point Constructor: " << *C1 << " ]" << endl;
#endif
  coordinates.push_back(new Coordinate(C1));
}

// ==============================================================


// ==============================================================
#ifndef TRIANGLE_H
#define TRIANGLE_H
class Triangle: public Shape
{
public:
  Triangle( Coordinate C1, Coordinate C2, Coordinate C3 );
  Triangle();
  double getArea();
};
#endif

double Triangle::getArea()
{
#ifdef DEBUG
  cerr << "[ double Triangle::getArea() ]" << endl;
#endif
  dArea = 0.5*(-coordinates[0]->getX()*(coordinates[1]->getY()-coordinates[2]->getY()) +  coordinates[0]->getY()*(coordinates[1]->getX()-coordinates[2]->getX()) - ((coordinates[1]->getX()*coordinates[2]->getY())-(coordinates[1]->getY()*coordinates[2]->getX())));
  return dArea;
}

Triangle::Triangle( Coordinate C1, Coordinate C2, Coordinate C3 )
{
#ifdef DEBUG
  cerr << "[ Triangle Constructor: " << C1 << " " << C2 << " " << C3 << " ]" << endl;
#endif
  coordinates.push_back(new Coordinate(C1));
  coordinates.push_back(new Coordinate(C2));
  coordinates.push_back(new Coordinate(C3));
}
// ==============================================================
#ifndef PRISM_H
#define PRISM_H


class Prism: public Collection
{
public:
  Prism(){};  
  Prism(Coordinate * C1, Coordinate * C2, int c);
  
private:
};
#endif




Prism::Prism( Coordinate * C1, Coordinate * C2, int c=3)
{
#ifdef DEBUG
  cerr << "[ Prism::Prism( " << *C1 << "," << *C2 << " ) ]" << endl;
#endif
  double x1=C1->getX();
  double y1=C1->getY();
  double z1=C1->getZ();
  
  double x2=C2->getX();
  double y2=C2->getY();
  double z2=C2->getZ();

  // top
  vector<Coordinate*> top;
  top.push_back( new Coordinate(x1,y2,z1) );
  top.push_back( new Coordinate(x2,y2,z1)  );
  top.push_back( new Coordinate(x2,y2,z2)  );
  top.push_back( new Coordinate(x1,y2,z2)  );
  this->add( new Polygon(top,c));

  // bottom
  vector<Coordinate*> bottom;
  bottom.push_back( new Coordinate(x1,y1,z1) );
  bottom.push_back( new Coordinate(x2,y1,z1)  );
  bottom.push_back( new Coordinate(x2,y1,z2)  );
  bottom.push_back( new Coordinate(x1,y1,z2)  );
  this->add( new Polygon(bottom, c));

  // left
  vector<Coordinate*> left;
  left.push_back( new Coordinate(x1,y1,z1) );
  left.push_back( new Coordinate(x1,y2,z1)  );
  left.push_back( new Coordinate(x1,y2,z2)  );
  left.push_back( new Coordinate(x1,y1,z2)  );
  this->add( new Polygon(left, c));

  // right
  vector<Coordinate*> right;
  right.push_back( new Coordinate(x2,y1,z1) );
  right.push_back( new Coordinate(x2,y2,z1)  );
  right.push_back( new Coordinate(x2,y2,z2)  );
  right.push_back( new Coordinate(x2,y1,z2)  );
  this->add( new Polygon(right, c));

  // front
  vector<Coordinate*> front;
  front.push_back( new Coordinate(x1,y1,z1) );
  front.push_back( new Coordinate(x2,y1,z1)  );
  front.push_back( new Coordinate(x2,y2,z1)  );
  front.push_back( new Coordinate(x1,y2,z1)  );
  this->add( new Polygon(front, c));

    // back
  vector<Coordinate*> back;
  back.push_back( new Coordinate(x1,y1,z2) );
  back.push_back( new Coordinate(x2,y1,z2)  );
  back.push_back( new Coordinate(x2,y2,z2)  );
  back.push_back( new Coordinate(x1,y2,z2)  );
  this->add( new Polygon(back, c));


  
  Coordinate * C3 = new Coordinate( x2,y2,z1 );  // front right 
  Coordinate * C4 = new Coordinate( x1, y1, z2 ); // back left

  this->add( new Rectangle( C1, new Coordinate( x2, y2, z1 ),c )); // front
   this->add( new Rectangle( new Coordinate( x1, y1, z2), C2,c)); // back

   //this->add( new Rectangle( new Coordinate( x2, y1, z1 ), C2 ,c)); // right
   //this->add( new Rectangle( C1, new Coordinate( x1, y2, z2 ),c)); // left

  //this->add( new Rectangle( new Coordinate( x1, y2, z1 ), C2,c)); // top
  //this->add( new Rectangle( C1, new Coordinate( x2, y1, z2 ),c)); // bottom
}

// ==============================================================
#ifndef AXES_H
#define AXES_H
class Axes: public Shape
{
public:
  Axes();
};
Axes::Axes()
{
  add( new Coordinate());
  add( new Coordinate( GRIDX, 0, 0));
  add( new Coordinate( -GRIDX, 0, 0));
  add( new Coordinate());
  add( new Coordinate( 0, GRIDX, 0 ));
  add( new Coordinate( 0, -GRIDX, 0));
  colour=6;
}

#endif
// ==============================================================
#ifndef SPHERE_H
#define SPHERE_H
class Sphere: public Collection
{
public:
  Sphere(Coordinate * C1, double rx, double ry, int c );
private:
  Sphere();
  double radius_x;
  double radius_y;
  Coordinate * center;
  vector<Coordinate*> slice;
  vector<vector<Coordinate*> > slices;
  
};
Sphere::Sphere( Coordinate * C1, double rx, double ry, int c=3)
{
#ifdef DEBUG
  cerr << "Sphere::Sphere( " << *C1 << ", " << rx << ", " << ry << ", " << c << ")" << endl;
#endif
  //colour = c;
  radius_x = rx;
  radius_y = ry;
  center = new Coordinate(C1);
  for( int i=0; i<60; i++ )
    {
      for( double THETA=0; THETA<(2*3.14159); THETA+=FIDELITY )
	{
	  double x = sin(THETA)*rx*ry/( sqrt( ry*ry*cos(THETA)*cos(THETA)+rx*rx*sin(THETA)*sin(THETA)));
	  double y = cos(THETA)*rx*ry/( sqrt( ry*ry*cos(THETA)*cos(THETA)+rx*rx*sin(THETA)*sin(THETA)));
	  //this->add( new Point( new Coordinate(  x+C1->getX(), y+C1->getY(), C1->getZ() )));
	  slice.push_back( new Coordinate(   x+C1->getX(), y+C1->getY(), C1->getZ() ));
	}
      slices.push_back(slice);
     
    }
  
}

#endif
#ifndef ELLIPSE_H
#define ELLIPSE_H
class Ellipse: public Polygon
{
public:
  Ellipse(Coordinate * C1, double rx, double ry, int c);
private:
  Ellipse();
  double radius_x;
  double radius_y;
  Coordinate * center;
};
#endif

Ellipse::Ellipse()
{
#ifdef DEBUG
  cerr << "[ Empty Ellipse Constructor ]" << endl;
#endif
  radius_x = 1;
  radius_y = 1;
  center = new Coordinate();
}

Ellipse::Ellipse( Coordinate * C1, double rx=1, double ry=1, int c=0)
{
  colour = c;
  radius_x = rx;
  radius_y = ry;
  center = new Coordinate(C1);
  for( double THETA=0; THETA<(2*3.14159); THETA+=FIDELITY )
    {
      double x = sin(THETA)*rx*ry/( sqrt( ry*ry*cos(THETA)*cos(THETA)+rx*rx*sin(THETA)*sin(THETA)));
      double y = cos(THETA)*rx*ry/( sqrt( ry*ry*cos(THETA)*cos(THETA)+rx*rx*sin(THETA)*sin(THETA)));
      this->add( new Coordinate( x+C1->getX(), y+C1->getY(), C1->getZ() ));
    }
}


// ==============================================================
#ifndef CUBE_H
#define CUBE_H
class Cube: public Shape
{
public:
  Cube( Coordinate * C1, Coordinate * C2 );
private:
  Cube(){};
};

Cube::Cube(Coordinate * C1, Coordinate * C2)
{
  double x1, y1, z1, x2, y2, z2;
  x1=C1->getX();
  y1=C1->getY();
  z1=C1->getZ();
  x2=C2->getX();
  y2=C2->getY();
  z2=C2->getZ();
  
  add( new Coordinate( x1,y1,z1 ));
  add( new Coordinate( x2,y1,z1 ));
  add( new Coordinate( x2,y2,z1 ));
  add( new Coordinate( x1,y2,z1 ));
  add( new Coordinate( x1,y1,z1 ));
  add( new Coordinate( x1,y1,z2));
  add( new Coordinate( x1,y2,z2));
  add( new Coordinate( x1,y2,z1));
  add( new Coordinate( x1,y2,z2));
  add( new Coordinate( x2,y2,z2 ));
  add( new Coordinate( x2,y2,z1));
  add( new Coordinate( x2,y2,z2 ));
  add( new Coordinate( x2,y1,z2));
  add( new Coordinate( x1, y1,z2));
  add( new Coordinate( x2,y1,z2));
  add( new Coordinate( x2,y1,z1));
}

#endif

// ==============================================================
// Helper Functions
// ==============================================================
unsigned timestamp()
{
  return(chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count());
}
void initHTML()
{
  cout << "<html><head><title>Shapes</title></head><body bgcolor='#CCCCCC'>" << endl;
}
void endHTML()
{
  cout << "</body></html>" << endl;
}

void gridSVG()
{
#ifdef DEBUG
  cerr << "[ void planeSVG() ]" << endl;
  cout << "<!-- Coordinate grid -->" << endl;
#endif
  // the grid
  Line * x_lines = new Line( new Coordinate( -GRIDX,-GRIDX,0 ), new Coordinate( -GRIDX,GRIDX,0));
  Line * y_lines = new Line( new Coordinate( -GRIDX,-GRIDX,0 ), new Coordinate( GRIDX,-GRIDX,0));
  x_lines->setColour(6);
  y_lines->setColour(6);
  
  for( int i=-GRIDX; i<GRIDX+1; i++ )
    {
      x_lines->outputStereoSVG();
      y_lines->outputStereoSVG();
      x_lines->translate( 1,0,0);
      y_lines->translate( 0,1,0);
    }
}


int main()
{
  // Limits are currently (+/-14,+/- 14, 0 )
  
  unsigned starttime = timestamp();
#ifdef DEBUG
  cerr << "[** - DEBUG MODE - **]" << endl;
#endif
  initHTML(); // write the HTML Header and Info
  Scene * S = new Scene( SCENEHEIGHT, SCENEWIDTH );
  S->initSVG();
  
  Collection * Coll = new Collection();

  //Axes * a = new Axes();
  //Coll->add( a );


  Sphere * S1 = new Sphere( new Coordinate( 10, 0, 0 ), 1, 1 );
  Coll->addCollection( S1 );
  
  Coll->outputStereoSVG();

  
  // ==========================
  
  S->endSVG(); // write the SVG closing tag
  endHTML(); // write the HTML closing tags

  delete S;

  
  //#ifdef DEBUG
  unsigned endtime = timestamp();
  cerr << "Runtime: " << endtime-starttime << " milliseconds" << endl;
  //#endif
  return 0;
}
