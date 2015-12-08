/*=========================================================================

  Program:   Visualization Toolkit
  Module:    brain.cxx

  VTK/Examples/Cxx/Filtering/Glyph3D

=========================================================================*/
#include "ui_SimpleViewUI.h"
#include "SimpleViewUI.h"

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "vtkSmartPointer.h"
#include "vtkCubeSource.h"
#include "vtkSphereSource.h"
#include "vtkLineSource.h"
#include "vtkGlyph3D.h"
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include <vtkTransform.h>
#include <sstream>

#include <vtkTIFFReader.h>
#include <vtkTexture.h>
#include <vtkPlaneSource.h>
#include "vtkImageData.h"
#include "vtkTransform.h"

using namespace std;
#define default_color 61
#define ZStep 3
#define grouplist (348*260)

void paintLine(int x0, int y0, int z0, int x1, int y1, int z1, vtkRenderer *rend) {

	vtkLineSource *line = vtkLineSource::New();
	line->SetPoint1(x0,y0,z0);
	line->SetPoint2(x1,y1,z1);
	line->Update();
	
	vtkPolyDataMapper *map = vtkPolyDataMapper::New();
	map->SetInput(line->GetOutput());
	map->Update();
	
	vtkActor *Axes = vtkActor::New();
	Axes->SetMapper(map);
	Axes->GetProperty()->SetColor(255, 255, 255);
	Axes->GetProperty()->SetLineWidth(1.0);
	
	rend->AddActor(Axes);
	
	Axes->Delete();
	line->Delete();
	map->Delete();
}

void textimg(int img_step, int end, vtkTIFFReader *reader, vtkRenderer *rend, float transparency) {

  int step = ZStep*img_step;

  vtkTexture *atext = vtkTexture::New();
  atext->SetInput(reader->GetOutput());
  atext->InterpolateOn();

  vtkPlaneSource *plane = vtkPlaneSource::New();
  plane->SetOrigin(0.0,-260,0.0);
  plane->SetPoint1(-348,-260,0.0);
  plane->SetPoint2(0,0,0);
  plane->SetXResolution(348);
  plane->SetYResolution(260);

  vtkPolyDataMapper *planeMapper = vtkPolyDataMapper::New();
  planeMapper->SetInput(plane->GetOutput());

  vtkActor *planeActor = vtkActor::New();
  planeActor->SetMapper(planeMapper);
  planeActor->SetTexture(atext);
  planeActor->GetProperty()->SetOpacity(transparency);

  rend->AddActor(planeActor);

  while (step<end) {
	  vtkTransform *t = vtkTransform::New();
      t->Translate(0,0,step);

      vtkActor *planeActor2 = vtkActor::New();
      planeActor2->SetMapper(planeMapper);
      planeActor2->SetTexture(atext);
	  planeActor2->GetProperty()->SetOpacity(transparency);
      planeActor2->SetUserMatrix(t->GetMatrix());

      rend->AddActor(planeActor2);
	  step = step+ZStep*img_step;

	  planeActor2->Delete();
	  t->Delete();
  }
}

// Constructor
SimpleView::SimpleView() 
{
  this->ui = new Ui_SimpleView;
  this->ui->setupUi(this);

  cout<<"Vivo Data Comparison"<<endl;
  // load images of slices
  vtkTIFFReader *reader = vtkTIFFReader::New();
  int tiff = 1;
  //cout<<"Choose the background images: 1 for vivo, 2 for s21, 3 for s24 "<<endl;
  //cin>>tiff;
  switch(tiff) {
  case 1:
	  reader->SetFileName ( "../Data/vivo.tif" );
	  break;
  case 2:
	  reader->SetFileName ( "../Data/s21.tif" );
	  break;
  case 3:
	  reader->SetFileName ( "../Data/s24.tif" );
	  break;
  }

  float transparency = 0.8;
  //cout<<"Setup the transparent value for background images: 0.0~1.0"<<endl;
  //cin>>transparency;

  int imgstep = 50;
  //cout<<endl<<"How many frames between two background images do you want? "<<endl;
  //cin>>imgstep;

  // Get all data from the input file for vtk
  std::string leftfilename, rightfilename;
  int leftfile = 1, rightfile = 1;
  //char cor = 'n';
  cout<<endl<<"Left vivo data (7, 8, 9 or 12): "<<endl;
  cin>>leftfile;
  switch(leftfile) {
  case 1:
	  leftfilename = "../Data/vtkin_vivo.txt";
	  break;
  case 2:
	  leftfilename = "../Data/vtkin_vivo_louvain.txt";
	  break;
  case 7:
	  leftfilename = "../Data/vtkin_vivo_s7.txt";
	  break;
  case 8:
	  leftfilename = "../Data/vtkin_vivo_s8.txt";
	  break;
  case 9:
	  leftfilename = "../Data/vtkin_vivo_s9.txt";
	  break;
  case 12:
	  leftfilename = "../Data/vtkin_vivo_s12.txt";
	  break;
  }
  cout<<endl<<"Right vivo data (7, 8, 9 or 12): "<<endl;
  cin>>rightfile;
  switch(rightfile) {
  case 1:
	  rightfilename = "../Data/vtkin_vivo.txt";
	  break;
  case 2:
	  rightfilename = "../Data/vtkin_vivo_louvain.txt";
	  break;
  case 7:
	  rightfilename = "../Data/vtkin_vivo_s7.txt";
	  break;
  case 8:
	  rightfilename = "../Data/vtkin_vivo_s8.txt";
	  break;
  case 9:
	  rightfilename = "../Data/vtkin_vivo_s9.txt";
	  break;
  case 12:
	  rightfilename = "../Data/vtkin_vivo_s12.txt";
	  break;
  }

  int timeStart = 0;
  //cout<<endl<<"Set the first frame to display. Recommend 0 for vivo, 3000 for s21, and 2500 for s24 "<<endl;
  //cin>>timeStart;

  char stereo = 'n';
  //cout<<endl<<"Turn on stereo? (y/n)"<<endl;
  //cin>>stereo;

  std::ifstream leftfin(leftfilename.c_str());
  if(!leftfin){
	  std::cout << "Unable to open myfile"<< std::endl;
	  //return EXIT_FAILURE;// terminate with error
  }
  std::ifstream rightfin(rightfilename.c_str());
  if(!rightfin){
	  std::cout << "Unable to open myfile"<< std::endl;
	  //return EXIT_FAILURE;// terminate with error
  }

  std::string line;
  vtkSmartPointer<vtkPoints> leftpoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkPoints> rightpoints = vtkSmartPointer<vtkPoints>::New();

  // Setup the colors array
  vtkSmartPointer<vtkUnsignedCharArray> colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
  colors->SetNumberOfComponents(3);
  colors->SetName("Colors");

  unsigned char color_palette[default_color][3] = {{0,0,0},
	{255,0,0},{0,255,0},{0,0,255},{255,255,0},{0,255,255},{255,0,255},
	{138,43,226},{165,42,42},{222,184,135},{95,158,160},{127,250,0},{210,105,30},
	{100,149,237},{220,20,60},{0,0,139},{0,139,139},{184,134,11},{0,100,0},
	{139,0,139},{85,107,47},{255,140,0},{153,50,204},{139,0,0},{233,150,122},
	{72,61,139},{0,206,209},{255,20,147},{0,191,255},{105,105,105},{178,34,34},
	{34,139,34},{218,165,32},{0,128,0},{255,105,180},{205,92,92},{75,0,130},
	{144,238,144},{255,182,193},{255,160,122},{32,178,170},{135,206,250},{50,205,50},
	{128,0,0},{147,112,219},{60,179,113},{0,250,154},{199,21,133},{255,228,181},
	{128,128,0},{255,165,0},{218,112,214},{255,69,0},{175,238,238},{205,133,63},
	{255,192,203},{128,0,128},{65,105,225},{188,143,143},{139,69,19},{46,139,87}
	}; 

  int lefttimestep = 0, leftgroup_num=0;
  int righttimestep = 0, rightgroup_num=0;

  while(std::getline(leftfin, line))
  {
	int time, node_y, node_x, colorid; 
    std::stringstream linestream;
    linestream << line;
	linestream >> time >> node_y >> node_x >> colorid;

	if ((time-timeStart)>=0) {
		leftpoints->InsertNextPoint(-node_x, -node_y, ZStep*(time-timeStart));
		colors->InsertNextTupleValue(color_palette[int(colorid%default_color)]);
	}
	
	lefttimestep = time;
	if (leftgroup_num < colorid)
		leftgroup_num = colorid;
  }
  leftfin.close();

  while(std::getline(rightfin, line))
  {
	int time, node_y, node_x, colorid; 
    std::stringstream linestream;
    linestream << line;
	linestream >> time >> node_y >> node_x >> colorid;

	if ((time-timeStart)>=0) {
		rightpoints->InsertNextPoint(-node_x, -node_y, ZStep*(time-timeStart));
		colors->InsertNextTupleValue(color_palette[int(colorid%default_color)]);
	}
	
	righttimestep = time;
	if (rightgroup_num < colorid)
		rightgroup_num = colorid;
  }
  rightfin.close();

  //cout<<endl<<"The last frame number is: "<<timestep<<endl;
  //cout<<endl<<"The total number of community is: "<<group_num<<endl;


  // from VTK/Examples/Cxx/Filtering/Glyph3D
  vtkSmartPointer<vtkPolyData> leftpolydata  = vtkSmartPointer<vtkPolyData>::New();
  leftpolydata->SetPoints(leftpoints);
  leftpolydata->GetPointData()->SetScalars(colors);
  vtkSmartPointer<vtkPolyData> rightpolydata  = vtkSmartPointer<vtkPolyData>::New();
  rightpolydata->SetPoints(rightpoints);
  rightpolydata->GetPointData()->SetScalars(colors);

  vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();

  vtkSmartPointer<vtkGlyph3D> leftglyph3D = vtkSmartPointer<vtkGlyph3D>::New();
  leftglyph3D->SetColorModeToColorByScalar();
  leftglyph3D->SetSourceConnection(sphereSource->GetOutputPort());
#if VTK_MAJOR_VERSION <=5
  leftglyph3D->SetInput(leftpolydata);
#else
  leftglyph3D->SetInputData(polydata);
#endif
  leftglyph3D->ScalingOff();
  leftglyph3D->Update();

  vtkSmartPointer<vtkGlyph3D> rightglyph3D = vtkSmartPointer<vtkGlyph3D>::New();
  rightglyph3D->SetColorModeToColorByScalar();
  rightglyph3D->SetSourceConnection(sphereSource->GetOutputPort());
#if VTK_MAJOR_VERSION <=5
  rightglyph3D->SetInput(rightpolydata);
#else
  rightglyph3D->SetInputData(polydata);
#endif
  rightglyph3D->ScalingOff();
  rightglyph3D->Update();

  // Create a mapper and actor
  vtkSmartPointer<vtkPolyDataMapper> leftmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  leftmapper->SetInputConnection(leftglyph3D->GetOutputPort());
  vtkSmartPointer<vtkPolyDataMapper> rightmapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  rightmapper->SetInputConnection(rightglyph3D->GetOutputPort());

  vtkSmartPointer<vtkActor> leftactor = vtkSmartPointer<vtkActor>::New();
  leftactor->SetMapper(leftmapper);
  vtkSmartPointer<vtkActor> rightactor = vtkSmartPointer<vtkActor>::New();
  rightactor->SetMapper(rightmapper);

  // Create a camera
  vtkCamera *leftcamera = vtkCamera::New();
  leftcamera->SetPosition(1,1,1);
  leftcamera->SetFocalPoint(0,0,lefttimestep/2);
  vtkCamera *rightcamera = vtkCamera::New();
  rightcamera->SetPosition(1,1,1);
  rightcamera->SetFocalPoint(0,0,righttimestep/2);

  //Create a renderer
  vtkSmartPointer<vtkRenderer> leftrenderer = vtkSmartPointer<vtkRenderer>::New();
  vtkSmartPointer<vtkRenderer> rightrenderer = vtkSmartPointer<vtkRenderer>::New();

  lefttimestep = ZStep*(lefttimestep-timeStart);
  textimg(imgstep, lefttimestep, reader, leftrenderer, transparency); // Add images to the scene
  righttimestep = ZStep*(righttimestep-timeStart);
  textimg(imgstep, righttimestep, reader, rightrenderer, transparency); // Add images to the scene

  // VTK Renderer
  leftrenderer->AddActor(leftactor);  //Add the actor to the scene
  leftrenderer->SetActiveCamera(leftcamera);
  leftrenderer->ResetCamera(); 
  leftrenderer->SetBackground(.3, .6, .3); // Background color greent
  rightrenderer->AddActor(rightactor);  //Add the actor to the scene
  rightrenderer->SetActiveCamera(rightcamera);
  rightrenderer->ResetCamera(); 
  rightrenderer->SetBackground(.3, .6, .3); // Background color greent

  // VTK/Qt wedded
  this->ui->qvtkWidgetLeft->GetRenderWindow()->AddRenderer(leftrenderer);
  this->ui->qvtkWidgetRight->GetRenderWindow()->AddRenderer(rightrenderer);

  // Adding Stereo
  if (stereo=='y') {
  this->ui->qvtkWidgetLeft->GetRenderWindow()->SetStereoCapableWindow(1);
  this->ui->qvtkWidgetLeft->GetRenderWindow()->StereoRenderOn();
  this->ui->qvtkWidgetLeft->GetRenderWindow()->SetStereoTypeToInterlaced();
  this->ui->qvtkWidgetLeft->GetRenderWindow()->StereoUpdate();

  this->ui->qvtkWidgetRight->GetRenderWindow()->SetStereoCapableWindow(1);
  this->ui->qvtkWidgetRight->GetRenderWindow()->StereoRenderOn();
  this->ui->qvtkWidgetRight->GetRenderWindow()->SetStereoTypeToInterlaced();
  this->ui->qvtkWidgetRight->GetRenderWindow()->StereoUpdate();
  }

  // Set up action signals and slots
  connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));  

};

void SimpleView::slotExit() 
{
  qApp->exit();
}
