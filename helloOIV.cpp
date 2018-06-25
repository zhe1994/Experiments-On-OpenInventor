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

#include <Inventor/draggers/SoTranslate1Dragger.h>
#include <Inventor/manips/SoClipPlaneManip.h>
#include <Inventor/nodes/SoNode.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <ScaleViz/actions/ScGetBoundingBoxAction.h>

#include <VolumeViz/nodes/SoVolumeGroup.h>
#include <VolumeViz/nodes/SoVolumeMaskGroup.h>
#include <VolumeViz/nodes/SoVolumeMask.h>
#include <VolumeViz/nodes/SoVolumeDataDrawStyle.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/readers/SoVRDicomFileReader.h>
#include <VolumeViz/nodes/SoVolumeRenderingQuality.h>
#include <Medical/InventorMedical.h>
#include <Medical/helpers/MedicalHelper.h>
#include <LDM/nodes/SoDataRange.h>
#include <LDM/nodes/SoTransferFunction.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoInteractiveComplexity.h>
#include <Inventor/nodes/SoMaterial.h>
#include <LDM/nodes/SoMultiDataSeparator.h>
#include <Inventor/nodes/SoGradientBackground.h>
#include <Inventor/SbColorRGBA.h>
#include <Inventor/ViewerComponents/SoCameraInteractor.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/events/SoLocation2Event.h>

#include <LDM/manips/SoROIManip.h>

#define FILENAME "$OIVHOME/examples/source/VolumeViz/Data/3DHEAD.ldm"
const SbString VOLUME_FILENAME = "C:/data/SE000001";
const SbString COLORMAP_FILENAME = "$OIVHOME/examples/source/Medical/data/resources/volrenGlow.am";

static SoRef<SoCameraInteractor> m_interactor;

template<typename T>
void drawSphere(T* dest, const SbVec3i32& dim, const SbSphere& sphere, T value)
{
	size_t offs = 0;
	for (int k = 0; k < dim[2]; k++)
		for (int j = 0; j < dim[1]; j++)
			for (int i = 0; i < dim[0]; i++)
			{
				SbVec3f pc = sphere.getCenter() - SbVec3f(float(i), float(j), float(k));
				if (pc.length() <= sphere.getRadius())
					dest[offs] = value;
				offs++;
			}
}

void ClipPlaneManip(Widget &myWindow) {
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
}

void PlayROI(Widget &myWindow) {
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

	SoROIManip* myROIManip = new SoROIManip();
	SbVec3i32 i_min(bboxAction.getBoundingBox().getMin());
	SbVec3i32 i_max(bboxAction.getBoundingBox().getMax());
	myROIManip->box.setValue(i_min, i_max);
	scene->addChild(myROIManip);


	//SoClipPlaneManip *clipPlaneManip = new SoClipPlaneManip;
	//clipPlaneManip->setValue(bboxAction.getBoundingBox(),SbVec3f(1, 0, 0), 0.1);
	//scene->insertChild(clipPlaneManip, 1);
  
	SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
	myCamera->viewAll(scene, SbViewportRegion(100, 100));

	myRenderArea->setSceneGraph(scene);
	myRenderArea->setTitle("Hello OIV");
	myRenderArea->show();
}

void VolumeMask(Widget &myWindow) {
	// Initialize of VolumeViz extension
	SoVolumeRendering::init();

	// Node to hold the volume data
	SoVolumeData* vd = new SoVolumeData();
	SoVRDicomFileReader *DicomReader = new SoVRDicomFileReader();
	DicomReader->setDirectory(VOLUME_FILENAME);
	vd->setReader(*DicomReader);
	vd->ldmResourceParameters.getValue()->subTileDimension.setValue(32, 32, 32);

	SoDataRange *VRRange = new SoDataRange();
	VRRange->min = 176;
	VRRange->max = 476;

	// Node in charge of drawing the volume
	SoVolumeRender* pVolRender = new SoVolumeRender;
	pVolRender->subdivideTile = TRUE;
	pVolRender->gpuVertexGen = TRUE;
	pVolRender->useEarlyZ = TRUE;
	pVolRender->numSlicesControl = SoVolumeRender::MANUAL;
	pVolRender->numSlices = 256;
	pVolRender->samplingAlignment = SoVolumeRender::BOUNDARY_ALIGNED;
	//pVolRender->lowResMode = SoVolumeRender::DECREASE_SCREEN_RESOLUTION;
	//pVolRender->lowScreenResolutionScale = 2;

	//Material which defines the isosurface diffuse color, transparency,
	//specular color and shininess
	SoMaterial *matVolRend = new SoMaterial;
	matVolRend->specularColor.setValue(0.3f, 0.3f, 0.3f);
	matVolRend->shininess.setValue(0.1f);

	////Draw style and transfer function for intersecting masks
	//SoVolumeDataDrawStyle* vdsIntersection = new SoVolumeDataDrawStyle;
	//vdsIntersection->style = SoVolumeDataDrawStyle::VOLUME_RENDER;

	//SoTransferFunction* tfIntersection = new SoTransferFunction;
	//tfIntersection->predefColorMap = SoTransferFunction::BLUE_RED;
	//tfIntersection->minValue = 42;
	//tfIntersection->transferFunctionId = SoVolumeMaskGroup::TRANSFERFUNCTION_INTERSECTION_ID;

	//Add lighting and preintegration
	SoVolumeRenderingQuality* vrq = new SoVolumeRenderingQuality;
	//vrq->lighting = true;
	vrq->preIntegrated = true;
	vrq->lightingModel = SoVolumeRenderingQuality::OPENGL;
	vrq->interpolateOnMove = TRUE;
	vrq->deferredLighting = TRUE;

	//Add masks/styles/transfer funtion in the SoVolumeMaskGroup
	SoVolumeMaskGroup* vmg = new SoVolumeMaskGroup;
	SoVolumeMask* vm = new SoVolumeMask;
	vm->ldmResourceParameters.getValue()->subTileDimension = vd->ldmResourceParameters.getValue()->subTileDimension;
	vm->extent = vd->extent;
	vm->dataSetId = vd->dataSetId.getValue() + 1;

	SoTransferFunction* tf = new SoTransferFunction;
	tf->predefColorMap = SoTransferFunction::BLUE_RED;
	tf->minValue = 42;
	tf->transferFunctionId.connectFrom(&vm->dataSetId);

	SbVec3i32 size = vd->data.getSize();
	std::vector<unsigned char> maskData(size[0] * size[1] * size[2]);
	drawSphere(&maskData[0], size, SbSphere(SbVec3f(20, 20, 20), 400), (unsigned char)1);
	vm->data.setValue(size, SbDataType::UNSIGNED_BYTE, 0, &maskData[0], SoSFArray::COPY);

	vmg->addChild(tf);
	vmg->addChild(vm);

	//This draw style will be applied on the whole volume
	SoVolumeDataDrawStyle* vdsGlobal = new SoVolumeDataDrawStyle;
	vdsGlobal->style = SoVolumeDataDrawStyle::VOLUME_RENDER;
	SoTransferFunction* tfGlobal = new SoTransferFunction;
	tfGlobal->predefColorMap = SoTransferFunction::GRAY;

	SoMultiDataSeparator* mds = new SoMultiDataSeparator;
	mds->addChild(vrq);
	mds->addChild(vdsGlobal);
	mds->addChild(tfGlobal);
	mds->addChild(vd);
	mds->addChild(VRRange);
	//mds->addChild(tfIntersection);
	//mds->addChild(vdsIntersection);
	mds->addChild(vmg);
	mds->addChild(pVolRender);

	// Assemble the scene graph
	// Note: SoVolumeRender must appear after the SoVolumeData node.
	SoSeparator *root = new SoSeparator;
	root->ref();

	root->addChild(new SoGradientBackground);
	root->addChild(new SoDirectionalLight);
	root->addChild(matVolRend);
	root->addChild(mds);

	// Set up viewer:
	SoXtExaminerViewer *myViewer = new SoXtExaminerViewer(myWindow);
	myViewer->setTransparencyType(SoGLRenderAction::DELAYED_BLEND);
	myViewer->setSceneGraph(root);
	myViewer->setTitle("Volume Mask");
	myViewer->show();
}

void mouseEventCB(void* userData, SoEventCallback* node)
{
	static bool m_buttonDown = false;
	const SoEvent* evt = node->getEvent();
	if (SO_MOUSE_PRESS_EVENT(evt, BUTTON1)) {
		m_buttonDown = true;
		SbVec2f position = evt->getNormalizedPosition(node->getAction()->getViewportRegion());
		m_interactor->activateOrbiting(position);
		m_interactor->setRotationCenter(m_interactor->getFocalPoint());
	}
	else if (SO_MOUSE_RELEASE_EVENT(evt, BUTTON1)) {
		m_buttonDown = false;
	}
	else if (evt->isOfType(SoLocation2Event::getClassTypeId())) {
		if (m_buttonDown) {
			SbVec2f position = evt->getNormalizedPosition(node->getAction()->getViewportRegion());
			m_interactor->orbit(position);
		}
	}
}

void keyboardCB(void *userData, SoEventCallback *eventCB)
{
	const SoEvent* evt = eventCB->getEvent();
	if (SO_KEY_PRESS_EVENT(evt, Key::W))
	{
		m_interactor->translate(SbVec3f(0.f, 0.f, 0.5f));
		eventCB->setHandled();
	}
	else if (SO_KEY_PRESS_EVENT(evt, Key::S))
	{
		m_interactor->translate(SbVec3f(0.f, 0.f, -0.5f));
		eventCB->setHandled();
	}
	else if (SO_KEY_PRESS_EVENT(evt, Key::A))
	{
		m_interactor->translate(SbVec3f(0.5f, 0.f, 0.f));
		eventCB->setHandled();
	}
	else if (SO_KEY_PRESS_EVENT(evt, Key::D))
	{
		m_interactor->translate(SbVec3f(-0.5f, 0.f, 0.f));
		eventCB->setHandled();
	}
	else
	{
		eventCB->setHandled();
	}
}

void OIVCamera(Widget &myWindow) {
	SoSeparator *scene = new SoSeparator;
	scene->ref();

	SoPerspectiveCamera *myCamera = new SoPerspectiveCamera;
	SoXtRenderArea *myRenderArea = new SoXtRenderArea(myWindow);
	myCamera->viewAll(scene, SbViewportRegion(100, 100));

	SoCameraInteractor *camearInteractor = SoCameraInteractor::getNewInstance(myCamera);
	m_interactor = camearInteractor;

	SoSeparator *cone = new SoSeparator();
	SoMaterial *coneMat = new SoMaterial();
	SoTransform *coneTransform = new SoTransform();
	coneMat->diffuseColor.setValue(1.0, 0.0, 0.0);
	// coneTransform->translation.setValue(SbVec3f(0.f, 0.f, 0.f));
	coneTransform->scaleFactor.setValue(SbVec3f(0.5f, 0.5f, 0.5f));
	cone->addChild(coneTransform);
	cone->addChild(coneMat);
	cone->addChild(new SoCone);

	SoEventCallback *eventCB = new SoEventCallback();
	// eventCB->addEventCallback(SoMouseButtonEvent::getClassTypeId(), mouseEventCB);
	// eventCB->addEventCallback(SoLocation2Event::getClassTypeId(), mouseEventCB);
	eventCB->addEventCallback(SoKeyboardEvent::getClassTypeId(), keyboardCB);

	scene->insertChild(myCamera, 0);
	scene->addChild(eventCB);
	scene->addChild(new SoDirectionalLight);
	scene->addChild(cone);

	myRenderArea->setSceneGraph(scene);
	myRenderArea->setTitle("OIV Camera");
	myRenderArea->show();
}

void MutiVolume(Widget &myWindow) {
	// Initialize of VolumeViz extension
	SoVolumeRendering::init();

	const SbString volume_file1 = "C:/data/skull";
	const SbString volume_file2 = "C:/data/vessel";

	//// Node to hold the volume data
	//SoVolumeData* vd = new SoVolumeData();
	//SoVRDicomFileReader *DicomReader = new SoVRDicomFileReader();
	//DicomReader->setDirectory(volume_file1);
	//vd->setReader(*DicomReader);
	//vd->ldmResourceParameters.getValue()->subTileDimension.setValue(32, 32, 32);

	//SoDataRange *VRRange = new SoDataRange();
	//VRRange->min = 176;
	//VRRange->max = 476;

	//// Node in charge of drawing the volume
	//SoVolumeRender* pVolRender = new SoVolumeRender;
	//pVolRender->subdivideTile = TRUE;
	//pVolRender->gpuVertexGen = TRUE;
	//pVolRender->useEarlyZ = TRUE;
	//pVolRender->numSlicesControl = SoVolumeRender::MANUAL;
	//pVolRender->numSlices = 256;
	//pVolRender->samplingAlignment = SoVolumeRender::BOUNDARY_ALIGNED;

	////Material which defines the isosurface diffuse color, transparency,
	////specular color and shininess
	//SoMaterial *matVolRend = new SoMaterial;
	//matVolRend->specularColor.setValue(0.3f, 0.3f, 0.3f);
	//matVolRend->shininess.setValue(0.1f);

	////Add lighting and preintegration
	//SoVolumeRenderingQuality* vrq = new SoVolumeRenderingQuality;
	////vrq->lighting = true;
	//vrq->preIntegrated = true;
	//vrq->lightingModel = SoVolumeRenderingQuality::OPENGL;
	//vrq->interpolateOnMove = TRUE;
	//vrq->deferredLighting = TRUE;

	////Add masks/styles/transfer funtion in the SoVolumeMaskGroup
	//SoVolumeMaskGroup* vmg = new SoVolumeMaskGroup;
	//SoVolumeMask* vm = new SoVolumeMask;
	//vm->ldmResourceParameters.getValue()->subTileDimension = vd->ldmResourceParameters.getValue()->subTileDimension;
	//vm->extent = vd->extent;
	//vm->dataSetId = vd->dataSetId.getValue() + 1;

	//SoTransferFunction* tf = new SoTransferFunction;
	//tf->predefColorMap = SoTransferFunction::BLUE_RED;
	//tf->minValue = 42;
	//tf->transferFunctionId.connectFrom(&vm->dataSetId);

	//SbVec3i32 size = vd->data.getSize();
	//std::vector<unsigned char> maskData(size[0] * size[1] * size[2]);
	//drawSphere(&maskData[0], size, SbSphere(SbVec3f(20, 20, 20), 400), (unsigned char)1);
	//vm->data.setValue(size, SbDataType::UNSIGNED_BYTE, 0, &maskData[0], SoSFArray::COPY);

	//vmg->addChild(tf);
	//vmg->addChild(vm);

	////This draw style will be applied on the whole volume
	//SoVolumeDataDrawStyle* vdsGlobal = new SoVolumeDataDrawStyle;
	//vdsGlobal->style = SoVolumeDataDrawStyle::VOLUME_RENDER;
	//SoTransferFunction* tfGlobal = new SoTransferFunction;
	//tfGlobal->predefColorMap = SoTransferFunction::GRAY;

	//SoMultiDataSeparator* mds = new SoMultiDataSeparator;
	//mds->addChild(vrq);
	//mds->addChild(vdsGlobal);
	//mds->addChild(tfGlobal);
	//mds->addChild(vd);
	//mds->addChild(VRRange);
	//mds->addChild(vmg);
	//mds->addChild(pVolRender);

	SoVolumeData* vd2 = new SoVolumeData();
	SoVRDicomFileReader *DicomReader2 = new SoVRDicomFileReader();
	DicomReader2->setDirectory(volume_file2);
	vd2->setReader(*DicomReader2);
	vd2->ldmResourceParameters.getValue()->subTileDimension.setValue(32, 32, 32);

	SoDataRange *VRRange2 = new SoDataRange();
	VRRange2->min = 80;
	VRRange2->max = 476;

	// Node in charge of drawing the volume
	SoVolumeRender* pVolRender2 = new SoVolumeRender;
	pVolRender2->subdivideTile = TRUE;
	pVolRender2->gpuVertexGen = TRUE;
	pVolRender2->useEarlyZ = TRUE;
	pVolRender2->numSlicesControl = SoVolumeRender::MANUAL;
	pVolRender2->numSlices = 256;
	pVolRender2->samplingAlignment = SoVolumeRender::BOUNDARY_ALIGNED;

	//Material which defines the isosurface diffuse color, transparency,
	//specular color and shininess
	SoMaterial *matVolRend2 = new SoMaterial;
	matVolRend2->specularColor.setValue(0.3f, 0.3f, 0.3f);
	matVolRend2->shininess.setValue(0.1f);

	//Add lighting and preintegration
	SoVolumeRenderingQuality* vrq2 = new SoVolumeRenderingQuality;
	//vrq->lighting = true;
	vrq2->preIntegrated = true;
	vrq2->lightingModel = SoVolumeRenderingQuality::OPENGL;
	vrq2->interpolateOnMove = TRUE;
	vrq2->deferredLighting = TRUE;

	SoMultiDataSeparator *mds2 = new SoMultiDataSeparator();
	mds2->addChild(vd2);
	mds2->addChild(VRRange2);
	mds2->addChild(matVolRend2);
	mds2->addChild(vrq2);
	mds2->addChild(pVolRender2);

	SoVolumeGroup *volume_group = new SoVolumeGroup();
	// volume_group->addChild(mds);
	volume_group->addChild(mds2);

	// Assemble the scene graph
	// Note: SoVolumeRender must appear after the SoVolumeData node.
	SoSeparator *root = new SoSeparator;
	root->ref();

	root->addChild(new SoGradientBackground);
	root->addChild(new SoDirectionalLight);
	// root->addChild(matVolRend);
	root->addChild(volume_group);

	// Set up viewer:
	SoXtExaminerViewer *myViewer = new SoXtExaminerViewer(myWindow);
	myViewer->setTransparencyType(SoGLRenderAction::DELAYED_BLEND);
	myViewer->setSceneGraph(root);
	myViewer->setTitle("Multi Volume");
	myViewer->show();
}

int main(int argc, char **argv)
{
	// Initialize Inventor and Xt
	Widget myWindow = SoXt::init(argv[0]);
  
	// ClipPlaneManip(myWindow);
	// PlayROI(myWindow);
	// VolumeMask(myWindow);
	// OIVCamera(myWindow);
	MutiVolume(myWindow);

	SoXt::show(myWindow);
	SoXt::mainLoop();
	return 0;
}