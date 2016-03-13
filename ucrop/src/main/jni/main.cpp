//
// Created by Oleksii Shliama on 3/13/16.
//

#include <stdio.h>
#include <jni.h>
#include <vector>
#include <android/log.h>
#include "com_yalantis_ucrop_view_CropImageView.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

#define LOG_TAG "UCrop"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

JNIEXPORT jboolean JNICALL Java_com_yalantis_ucrop_view_CropImageView_cropFile(JNIEnv *env, jobject obj, jstring pathSource, jstring pathResult, jint left, jint top, jint width, jint height, jfloat angle) {

    const char *file_source_path = env->GetStringUTFChars(pathSource, 0);
    const char *file_result_path = env->GetStringUTFChars(pathResult, 0);

    LOGD("file_source_path: %s \n file_result_path: %s",file_source_path, file_result_path);

    Mat src = imread(file_source_path, CV_LOAD_IMAGE_UNCHANGED);

    // get rotation matrix for rotating the image around its center
    Point2f center(src.cols/2.0, src.rows/2.0);
    Mat rot = getRotationMatrix2D(center, angle, 1.0);
    // determine bounding rectangle
    Rect bbox = RotatedRect(center,src.size(), angle).boundingRect();
    // adjust transformation matrix
    rot.at<double>(0,2) += bbox.width/2.0 - center.x;
    rot.at<double>(1,2) += bbox.height/2.0 - center.y;

    const int JPEG_QUALITY = 80;

    try {

    Mat dst;
    warpAffine(src, dst, rot, bbox.size());

    // Setup a rectangle to define your region of interest
    Rect myROI(left, top, width, height);
    // Crop the full image to that image contained by the rectangle myROI
    // Note that this doesn't copy the data
    Mat croppedImage = dst(myROI);

    // CV_IMWRITE_PNG_COMPRESSION

    imwrite(file_result_path, croppedImage, vector<int>({CV_IMWRITE_JPEG_QUALITY, JPEG_QUALITY}));

    LOGD("imwrite DONE");
        return true;

    } catch (Exception& ex) {
        LOGE("Exception saving image: %s\n", ex.what());
        return false;
    }

    // env->ReleaseStringUTFChars(pathSource, file_source_path);
    //  env->ReleaseStringUTFChars(pathResult, file_result_path);
    }