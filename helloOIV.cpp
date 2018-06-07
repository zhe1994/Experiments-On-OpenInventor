#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoSeparator.h>

#include <Inventor/engines/SoCalculator.h>
#include <Inventor/nodekits/SoShapeKit.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoText3.h>
#include <Inventor/nodes/SoTransform.h>

#include <Inventor/Xt/SoXt.h>
#include <Inventor/Xt/viewers/SoXtExaminerViewer.h>

#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/manips/SoClipPlaneManip.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <ScaleViz/actions/ScGetBoundingBoxAction.h>

int main(int argc, char **argv)
{
		// Initialize Inventor and Xt
	Widget myWindow = SoXt::init(argv[0]);
  
	SoSeparator *scene = new SoSeparator;
	SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
	SoMaterial *myMaterial = new SoMaterial;
	scene->ref();
	scene->insertChild(myCamera, 0);
	scene->addChild(new SoDirectionalLight);
	myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);   // Red
	scene->addChild(myMaterial);
	scene->addChild(new SoCone);
  
	// Compute the bounding box of the scene graph.
	SoGetBoundingBoxAction bboxAction(SbViewportRegion(100, 100));
	bboxAction.apply(scene);
	// Inserting the manipulator at the center of the scene graph
	// and in the plane YZ.
	SoClipPlaneManip *clipPlaneManip = new SoClipPlaneManip;
	clipPlaneManip->setValue(bboxAction.getBoundingBox(),SbVec3f(1, 0, 0), 0.1);
	scene->insertChild(clipPlaneManip, 1);
  
	SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
	myCamera->viewAll(scene, SbViewportRegion(100, 100));

	myRenderArea->setSceneGraph(scene);
	myRenderArea->setTitle("Hello OIV");
	myRenderArea->show();

  
	// Create a viewer
	// SoXtExaminerViewer *myViewer = new SoXtExaminerViewer(myWindow);
	// Attach and show viewer
	// myViewer->setSceneGraph(scene);
	// myViewer->setTitle("File Reader");
	// myViewer->show();
  
	// Loop forever
	SoXt::show(myWindow);
	SoXt::mainLoop();
	return 0;
}