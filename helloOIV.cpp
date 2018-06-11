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

#include <LDM/manips/SoROIManip.h>

#define FILENAME "$OIVHOME/examples/source/VolumeViz/Data/3DHEAD.ldm"
const SbString VOLUME_FILENAME = "C:/data/SE000001";
const SbString COLORMAP_FILENAME = "$OIVHOME/examples/source/Medical/data/resources/volrenGlow.am";

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

void VolumeMaskII(Widget &myWindow) {
	// Initialize of VolumeViz extension
	SoVolumeRendering::init();

	// Node to hold the volume data
	SoVolumeData* vd = new SoVolumeData();
	// vd->fileName = FILENAME;
	 SoVRDicomFileReader *DicomReader = new SoVRDicomFileReader();
	 DicomReader->setDirectory(VOLUME_FILENAME);
	vd->setReader(*DicomReader);
	//// MedicalHelper::dicomAdjustVolume(vd);
	vd->ldmResourceParameters.getValue()->subTileDimension.setValue(32, 32, 32);

	//SoDataRange *VRRange = new SoDataRange();
	//VRRange->min = 176;
	//VRRange->max = 476;

	// Node in charge of drawing the volume
	SoVolumeRender* pVolRender = new SoVolumeRender;
	pVolRender->subdivideTile = TRUE;
	pVolRender->gpuVertexGen = TRUE;
	pVolRender->useEarlyZ = TRUE;
	pVolRender->numSlicesControl = SoVolumeRender::MANUAL;
	pVolRender->numSlices = 256;
	pVolRender->samplingAlignment = SoVolumeRender::BOUNDARY_ALIGNED;

	//Material which defines the isosurface diffuse color, transparency,
	//specular color and shininess
	SoMaterial *matVolRend = new SoMaterial;
	matVolRend->specularColor.setValue(0.3f, 0.3f, 0.3f);
	matVolRend->shininess.setValue(0.1f);

	//Draw style and transfer function for intersecting masks
	SoVolumeDataDrawStyle* vdsIntersection = new SoVolumeDataDrawStyle;
	vdsIntersection->style = SoVolumeDataDrawStyle::VOLUME_RENDER;

	SoTransferFunction* tfIntersection = new SoTransferFunction;
	tfIntersection->predefColorMap = SoTransferFunction::BLUE_RED;
	tfIntersection->minValue = 42;
	tfIntersection->transferFunctionId = SoVolumeMaskGroup::TRANSFERFUNCTION_INTERSECTION_ID;

	//Add lighting and preintegration
	SoVolumeRenderingQuality* vrq = new SoVolumeRenderingQuality;
	//vrq->lighting = true;
	vrq->preIntegrated = true;
	vrq->lightingModel = SoVolumeRenderingQuality::OPENGL;
	vrq->interpolateOnMove = TRUE;
	vrq->deferredLighting = TRUE;

	//Add masks/styles/transfer funtion in the SoVolumeMaskGroup
	SoVolumeMaskGroup* vmg = new SoVolumeMaskGroup;
	std::vector<SoVolumeMask*> vmList;
	std::vector<SoVolumeDataDrawStyle*> vmStyleList;
	SoVolumeMask* vm = new SoVolumeMask;
	vm->ldmResourceParameters.getValue()->subTileDimension = vd->ldmResourceParameters.getValue()->subTileDimension;
	vm->extent = vd->extent;
	vm->dataSetId = vd->dataSetId.getValue() + 1;
	vmList.push_back(vm);

	vmStyleList.push_back(new SoVolumeDataDrawStyle);

	//Add some random color the the transfer function
	SbVec3f cmColor = SbVec3f((float)rand(), (float)rand(), (float)rand()) / float(RAND_MAX);
	SoTransferFunction* tf = new SoTransferFunction;
	tf->predefColorMap = SoTransferFunction::BLUE_RED;
	tf->minValue = 42;
	tf->transferFunctionId.connectFrom(&vm->dataSetId);

	SbVec3i32 size = vd->data.getSize();
	std::vector<unsigned char> maskData(size[0] * size[1] * size[2]);
	drawSphere(&maskData[0], size, SbSphere(SbVec3f(77, 127, 35), 80), (unsigned char)1);
	// std::fill(maskData.begin(), maskData.end(), 1);
	vmList[0]->data.setValue(size, SbDataType::UNSIGNED_BYTE,
		0, &maskData[0], SoSFArray::COPY);

	vmg->addChild(tf);
	vmg->addChild(vmStyleList.back());
	vmg->addChild(vm);
	//for (size_t i = 0; i < NUM_MASKS; i++)
	//{
	//	SoVolumeMask* vm = new SoVolumeMask;
	//	vm->ldmResourceParameters.getValue()->tileDimension = vd->ldmResourceParameters.getValue()->tileDimension;
	//	vm->extent = vd->extent;
	//	vm->dataSetId = vd->dataSetId.getValue() + int(i) + 1;
	//	vmList.push_back(vm);

	//	vmStyleList.push_back(new SoVolumeDataDrawStyle);

	//	//Add some random color the the transfer function
	//	SbVec3f cmColor = SbVec3f((float)rand(), (float)rand(), (float)rand()) / float(RAND_MAX);
	//	SoTransferFunction* tf = generateTf(cmColor);
	//	tf->transferFunctionId.connectFrom(&vm->dataSetId);
	//	vmg->addChild(tf);
	//	vmg->addChild(vmStyleList.back());
	//	vmg->addChild(vm);
	//}

	//This draw style will be applied on the whole volume
	SoVolumeDataDrawStyle* vdsGlobal = new SoVolumeDataDrawStyle;
	vdsGlobal->style = SoVolumeDataDrawStyle::NONE;
	vdsGlobal->isovalues.set1Value(0, 42);
	vdsGlobal->isosurfacesMaterial = new SoMaterial;
	vdsGlobal->isosurfacesMaterial.getValue()->diffuseColor.set1Value(0, SbColor(0., 0., 1.));
	vdsGlobal->isosurfacesMaterial.getValue()->transparency.set1Value(0, 0.5);

	// generateMasks(vd, vmList, vmStyleList);

	SoMultiDataSeparator* mds = new SoMultiDataSeparator;
	mds->addChild(vrq);
	mds->addChild(vdsGlobal);
	mds->addChild(vd);
	// mds->addChild(VRRange);
	mds->addChild(tfIntersection);
	mds->addChild(vdsIntersection);
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

	pVolRender->lowResMode = SoVolumeRender::DECREASE_SCREEN_RESOLUTION;
	pVolRender->lowScreenResolutionScale = 2;

	// Set up viewer:
	SoXtExaminerViewer *myViewer = new SoXtExaminerViewer(myWindow);
	myViewer->setTransparencyType(SoGLRenderAction::DELAYED_BLEND);
	myViewer->setSceneGraph(root);
	myViewer->setTitle("Volume Mask");
	myViewer->show();
}

void VolumeMask(Widget &myWindow) {

	SoVolumeRendering::init();

	SoSeparator *scene = new SoSeparator;
	scene->ref();

	SoVolumeData *volData = new SoVolumeData();
	// volData->fileName = FILENAME;
	SoVRDicomFileReader *DicomReader = new SoVRDicomFileReader();
	DicomReader->setDirectory(VOLUME_FILENAME);
	volData->setReader(*DicomReader);
	volData->ldmResourceParameters.getValue()->subTileDimension.setValue(32, 32, 32);

	// Add specular to change rendering quality.
	SoMaterial *volumeMaterial = new SoMaterial();
	volumeMaterial->specularColor.setValue(0.26f, 0.26f, 0.26f);

	// Load the colorMap
	SoTransferFunction *pVRTransFunc = new SoTransferFunction();
	pVRTransFunc->loadColormap(COLORMAP_FILENAME);

	// remap data range 
	// Note: Our colormap is tuned for a specific data range so we don't use MedicalHelper.
	SoDataRange *VRRange = new SoDataRange();
	VRRange->min = 176;
	VRRange->max = 476;

	SoInteractiveComplexity *intercatComplexity = new SoInteractiveComplexity();
	intercatComplexity->fieldSettings.set1Value(0, "SoComplexity value 0.3 0.5");
	intercatComplexity->interactiveMode.setValue(SoInteractiveComplexity::FORCE_INTERACTION);

	//// Property node which allows SoVolumeRender to draw High Quality volumes.  
	SoVolumeRenderingQuality *pVRVolQuality = new SoVolumeRenderingQuality();
	pVRVolQuality->interpolateOnMove = true;
	pVRVolQuality->deferredLighting = true;
	pVRVolQuality->preIntegrated = true;
	pVRVolQuality->ambientOcclusion = true;

	SoVolumeRender* pVolRender = new SoVolumeRender;
	pVolRender->subdivideTile = TRUE;
	pVolRender->gpuVertexGen = TRUE;
	pVolRender->useEarlyZ = TRUE;
	pVolRender->numSlicesControl = SoVolumeRender::MANUAL;
	pVolRender->numSlices = 256;
	pVolRender->samplingAlignment = SoVolumeRender::BOUNDARY_ALIGNED;

	/* Volume Mask*/
	SoVolumeMaskGroup* vmg = new SoVolumeMaskGroup();

	std::vector<SoVolumeMask*> vmList;
	std::vector<SoVolumeDataDrawStyle*> vmStyleList;

	SoVolumeMask* vm = new SoVolumeMask();
	std::cout << "Volume Data Tile Dimension:\t" << volData->ldmResourceParameters.getValue()->tileDimension.getValue() << "\n";
	std::cout << "Volume Mask Tile Dimension:\n";
	std::cout << "Before assign:\t" << vm->ldmResourceParameters.getValue()->tileDimension.getValue() << "\n";
	vm->ldmResourceParameters.getValue()->subTileDimension = volData->ldmResourceParameters.getValue()->subTileDimension;
	// vm->ldmResourceParameters.getValue()->tileDimension.setValue(512, 512, 256);
	// vm->ldmResourceParameters.setValue(volData->ldmResourceParameters.getValue());
	std::cout << "After assign:\t" << vm->ldmResourceParameters.getValue()->tileDimension.getValue() << "\n";
	vm->extent = volData->extent;
	vm->dataSetId = volData->dataSetId.getValue() + 1;
	// vmList.push_back(vm);

	vmStyleList.push_back(new SoVolumeDataDrawStyle());
	SbVec3f cmColor = SbVec3f((float)rand(), (float)rand(), (float)rand()) / float(RAND_MAX);
	//SoTransferFunction* tf = new SoTransferFunction;
	//tf->predefColorMap = SoTransferFunction::BLUE_RED;
	//tf->minValue = 42;

	SoTransferFunction* tf = new SoTransferFunction;
	tf->predefColorMap = SoTransferFunction::NONE;
	tf->colorMapType = SoTransferFunction::RGBA;
	tf->predefColorMap = SoTransferFunction::STANDARD;
	tf->minValue = 10;

	float *alphas = new float[256];
	SbColorRGBA* rgba = new SbColorRGBA[256];
	for (size_t i = 0; i < 256; i++)
	{
		rgba[i][0] = 0.f;
		rgba[i][1] = 0.f;
		rgba[i][2] = 1.f;
		rgba[i][3] = 1.f;
		// alphas[i] = 0.f;
	}

	 tf->colorMap.setValues(0, 4 * 256, (float*)rgba);
	// tf->colorMap.setValues(0, 256, alphas);
	if (tf->hasTransparency())
	{
		std::cout << "has tranparency\n";
	}
	else
	{
		std::cout << "nah\n";
	}
	tf->transferFunctionId.connectFrom(&vm->dataSetId);

	// generateMasks(volData, vmList, vmStyleList);
	SbVec3i32 size = volData->data.getSize();
	std::vector<unsigned char> maskData(size[0] * size[1] * size[2]);
	std::fill(maskData.begin(), maskData.end(), 1);
	// drawBox(&maskData[0], size, SbBox3i32(SbVec3i32(100, 0, size[2] / 2), size), (unsigned char)1);
	vm->data.setValue(size, SbDataType::UNSIGNED_BYTE, 0, &maskData[0], SoSFArray::COPY);

	//std::cout << size << "\n";
	//std::cout << vm->ldmResourceParameters.getValue()->tileDimension.getValue() << "\n";
	//std::cout << volData->ldmResourceParameters.getValue()->tileDimension.getValue() << "\n";

	vmg->addChild(tf);
	vmg->addChild(vmStyleList.back());
	vmg->addChild(vm);

	SoMultiDataSeparator* mds = new SoMultiDataSeparator();

	mds->addChild(volData);
	mds->addChild(volumeMaterial);
	mds->addChild(pVRTransFunc);
	mds->addChild(VRRange);
	mds->addChild(intercatComplexity);
	mds->addChild(pVRVolQuality);
	mds->addChild(vmg);
	mds->addChild(pVolRender);

	scene->addChild(mds);

	// Create a viewer
	SoXtExaminerViewer *myViewer = new SoXtExaminerViewer(myWindow);
	// Attach and show viewer
	myViewer->setSceneGraph(scene);
	myViewer->setTitle("File Reader");
	myViewer->show();
}

int main(int argc, char **argv)
{
	// Initialize Inventor and Xt
	Widget myWindow = SoXt::init(argv[0]);
  
	// ClipPlaneManip(myWindow);
	// PlayROI(myWindow);
	VolumeMask(myWindow);
	// VolumeMaskII(myWindow);

	SoXt::show(myWindow);
	SoXt::mainLoop();
	return 0;
}