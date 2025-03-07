#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/imgcodecs/legacy/constants_c.h>
#include <opencv2/core.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/opencv_modules.hpp"
#include "opencv2/core.hpp"
#include <opencv2/core/cuda.hpp>
#include <opencv2/core/cvstd.hpp>
#include <opencv2/core/softfloat.hpp>
#include <opencv2/imgproc/types_c.h>

#include "AppFeature2D.hpp"
#include "esp_log.h"
#include "esp_camera.h"

static const char TAG[] = "App/Feature2D";

using namespace std;
using namespace cv;

extern "C" long int sysconf(int);

long int sysconf(int wtf)
{
    return 1;
}


cv::Ptr<cv::ORB> orb_detector;
std::vector<cv::KeyPoint> mvKeys;
cv::Mat mDescriptors;


AppFeature2D::AppFeature2D(QueueHandle_t queue_i,
                     QueueHandle_t queue_o,
                     bool show_img,
                     void (*callback)(camera_fb_t *)) : Frame(queue_i, queue_o, callback)
{

    ESP_LOGI(TAG, "Init AppFeature2D");
    orb_detector = cv::ORB::create();
    orb_detector->setFastThreshold(29);
    orb_detector->setNLevels(1);
    orb_detector->setMaxFeatures(40);
    orb_detector->setScaleFactor(1.2);
    orb_detector->setEdgeThreshold(35);
    orb_detector->setScoreType(cv::ORB::ScoreType::FAST_SCORE);
    orb_detector->setFirstLevel(0);
    orb_detector->setWTA_K(2);
}


void AppFeature2D::update()
{
    
}

static void task(AppFeature2D *self)
{
    ESP_LOGD(TAG, "Start");
    while (true)
    {
        if (self->queue_i == nullptr)
            break;

        camera_fb_t *frame = NULL;
        if (xQueueReceive(self->queue_i, &frame, portMAX_DELAY))
        {

            ESP_LOGI(TAG, "Extract 2d features from picture %ix%i\n", frame->width, frame->height);
            cv::Mat rawData(frame->height, frame->width, CV_8UC1, (void *)frame->buf);

            orb_detector->detect(rawData, mvKeys);
            orb_detector->compute(rawData, mvKeys, mDescriptors);

            ESP_LOGI(TAG, "For picture: mvKeys.size = %i\n", mvKeys.size());
            ESP_LOGI(TAG, "========================\n\n");

            if (self->queue_o)
                xQueueSend(self->queue_o, &frame, portMAX_DELAY);
            else
                self->callback(frame);
        }
    }
    ESP_LOGD(TAG, "Stop");
    vTaskDelete(NULL);
}

void AppFeature2D::run()
{
    xTaskCreatePinnedToCore((TaskFunction_t)task, TAG,16384, this, 5, NULL, 0);
}