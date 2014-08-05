#include <iostream>

#include <cv.h>
#include <highgui.h>

#include <vtkImageActor.h>
#include <vtkImageImport.h>
#include <vtkImageData.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCommand.h>

//Function prototypes
void Ipl2VTK(cv::Mat src, vtkImageData *dst);


//!Class vtkTimerCallback
/**
 * This class add support for timing events
 */
class vtkTimerCallback : public vtkCommand
{

public:
  vtkTimerCallback():frame(0){}
  ~vtkTimerCallback(){};


public:
  static vtkTimerCallback *New()
  {
    vtkTimerCallback *cb = new vtkTimerCallback;
    cb->TimerCount = 0;
    return cb;
  }

  virtual void Execute(vtkObject *vtkNotUsed(caller), unsigned long eventId,
                       void *vtkNotUsed(callData))
  {

    if (vtkCommand::TimerEvent == eventId)
      {
      ++this->TimerCount;
      }
    cout << this->TimerCount << endl;

    frame = cvQueryFrame(capture);

    cv::Mat imageMatrix(frame,true);

    Ipl2VTK(imageMatrix, imageData);

    
    window->Render();

  }

  void SetCapture(CvCapture *cap)
  {
    capture = cap;
  }

  void SetImageData(vtkImageData *id)
  {
    imageData = id;
  }

  void SetActor(vtkImageActor *act)
  {
    actor = act;
  }

  void SetRenderer(vtkRenderer *rend)
  {

    renderer = rend;
  }

  void SetRenderWindow(vtkRenderWindow *wind)
  {
    window = wind;
  }

private:
  int TimerCount;
  IplImage *frame;
  CvCapture *capture;
  vtkImageData *imageData;
  vtkImageActor *actor;
  vtkRenderer *renderer;
  vtkRenderWindow *window;
};



//!Entry point
int main(int argc, char *argv[])
{

  IplImage *frame = NULL;
  CvCapture *capture = NULL;

  capture = cvCaptureFromCAM(0);
  if(!capture)
    {
    std::cout << "Unable to open the device" << std::endl;
    exit(EXIT_FAILURE);
    }

  for(float i=0; i<1; i++)
    {
    frame = cvQueryFrame(capture);
    if(!frame)
      {
      std::cout << "Unable to get frames from the device" << std::endl;
      cvReleaseCapture(&capture);
      exit(EXIT_FAILURE);
      }
    }

  vtkImageData *imageData = vtkImageData::New();
  cv::Mat imageMatrix(frame,true);

  Ipl2VTK(imageMatrix, imageData);

  vtkImageActor *actor = vtkImageActor::New();
  actor->SetInputData(imageData);

  vtkRenderer *renderer = vtkRenderer::New();
  vtkRenderWindow *renderWindow = vtkRenderWindow::New();
  renderWindow->AddRenderer(renderer);

  vtkRenderWindowInteractor *renderInteractor = vtkRenderWindowInteractor::New();
  renderInteractor->SetRenderWindow(renderWindow);
  renderInteractor->Initialize();
  renderInteractor->CreateRepeatingTimer(30);

  renderer->AddActor(actor);
  renderer->SetBackground(1,1,1);
  renderer->ResetCamera();

  vtkTimerCallback *timer = vtkTimerCallback::New();
  timer->SetImageData(imageData);
  timer->SetCapture(capture);
  timer->SetActor(actor);
  timer->SetRenderer(renderer);
  timer->SetRenderWindow(renderWindow);
  renderInteractor->AddObserver(vtkCommand::TimerEvent, timer);


  renderInteractor->Start();

  cvReleaseCapture(&capture);
  
  return 0;
}

//!IplImage to vtkImageData
/**
 * Transforms data from cv::Mat to vtkImageData. It assumes
 * that parameters are correctly initialized.
 *
 * \param src OpenCV matrix representing the image data (source)
 *
 * \param dst vtkImageData representing the image (destiny)
 *
 */
void Ipl2VTK(cv::Mat src, vtkImageData *dst)
{

  vtkImageImport *importer = vtkImageImport::New();

  importer->SetOutput( dst );
  importer->SetDataSpacing( 1, 1, 1 );
  importer->SetDataOrigin( 0, 0, 0 );
  importer->SetWholeExtent(   0, src.size().width-1, 0, src.size().height-1, 0, 0 );
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetNumberOfScalarComponents( src.channels() );
  importer->SetImportVoidPointer( src.data );
  importer->Update();

  return;
}

