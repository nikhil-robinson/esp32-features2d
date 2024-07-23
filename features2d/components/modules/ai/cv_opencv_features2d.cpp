#include <stdio.h>
#include <stdlib.h>
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

#include <iostream>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_camera.h"

#include "cv_opencv_features2d.hpp"

static const char *TAG = "cv_opencv_features2d";

// using namespace std;
// using namespace cv;

static QueueHandle_t xQueueFrameI = NULL;
static QueueHandle_t xQueueEvent = NULL;
static QueueHandle_t xQueueFrameO = NULL;
static QueueHandle_t xQueueResult = NULL;

static bool gEvent = true;
static bool gReturnFB = true;

long int sysconf(int wtf)
{
    return 1;
}

cv::Ptr<cv::ORB> orb_detector;
std::vector<cv::KeyPoint> mvKeys;
cv::Mat mDescriptors;

void init_opencv_features2d()
{
    printf("OpenCV info print: %s", cv::getBuildInformation().c_str());
    orb_detector = cv::ORB::create();
    orb_detector->setFastThreshold(29);
    orb_detector->setNLevels(1);
    orb_detector->setMaxFeatures(40);
    orb_detector->setScaleFactor(1.2);
    orb_detector->setEdgeThreshold(35);
    orb_detector->setScoreType(cv::ORB::ScoreType::FAST_SCORE);
    orb_detector->setFirstLevel(0);
}

void test_opencv_features2d(uint8_t *data, int width, int heigth)
{
    printf("Extract 2d features from picture %ix%i\n", width, heigth);
    cv::Mat rawData(heigth, width, CV_8UC1, (void *)data);

    orb_detector->detect(rawData, mvKeys);
    orb_detector->compute(rawData, mvKeys, mDescriptors);
    printf("For picture: mvKeys.size = %i\n", mvKeys.size());
    for (int i = 0; i < mvKeys.size(); i++)
    {
        printf("mvKeys[%2i] x=%3.2f, y=%3.2f \n", i, mvKeys[i].pt.x, mvKeys[i].pt.y);
    }
    printf("========================\n\n");
}

static void task_process_handler(void *arg)
{
    camera_fb_t *frame = NULL;

    while (true)
    {
        if (gEvent)
        {
            if (xQueueReceive(xQueueFrameI, &frame, portMAX_DELAY))
            {
                printf("Extract 2d features from picture %ix%i\n", width, heigth);
                cv::Mat rawData(frame->heigth, frame->width, CV_8UC1, (void *)frame->buf);

                orb_detector->detect(rawData, mvKeys);
                orb_detector->compute(rawData, mvKeys, mDescriptors);
                printf("For picture: mvKeys.size = %i\n", mvKeys.size());
                for (int i = 0; i < mvKeys.size(); i++)
                {
                    printf("mvKeys[%2i] x=%3.2f, y=%3.2f \n", i, mvKeys[i].pt.x, mvKeys[i].pt.y);
                }
                printf("========================\n\n");
            }

            if (xQueueFrameO)
            {
                xQueueSend(xQueueFrameO, &frame, portMAX_DELAY);
            }
            else if (gReturnFB)
            {
                esp_camera_fb_return(frame);
            }
            else
            {
                free(frame);
            }

            if (xQueueResult)
            {

                // xQueueSend(xQueueResult, &is_detected, portMAX_DELAY);
            }
        }
    }
}


static void task_event_handler(void *arg)
{
    while (true)
    {
        xQueueReceive(xQueueEvent, &(gEvent), portMAX_DELAY);
    }
}

void register_cv_2d_feature(const QueueHandle_t frame_i,
                                 const QueueHandle_t event,
                                 const QueueHandle_t result,
                                 const QueueHandle_t frame_o,
                                 const bool camera_fb_return)
{
    xQueueFrameI = frame_i;
    xQueueFrameO = frame_o;
    xQueueEvent = event;
    xQueueResult = result;
    gReturnFB = camera_fb_return;

    xTaskCreatePinnedToCore(task_process_handler, "2d_feature_process", 4 * 1024, NULL, 5, NULL, 1);
    if (xQueueEvent)
        xTaskCreatePinnedToCore(task_event_handler, "2d_feature_event", 1 * 1024, NULL, 5, NULL, 1);
}