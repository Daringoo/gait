#pragma once

#include<afx.h>
#include <stdio.h>

#include <math.h>
#include "cv.h"
#include "highgui.h"
#include <time.h>
//#include <math.h>
#include <ctype.h>
//#include <stdio.h>

typedef unsigned char byte;
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "cv.h"
//#include "highgui.h"
#include <string>
////#pragma comment(lib,"opencv_highgui247d.lib")

using namespace cv;
using namespace std;
#include <Windows.h>

typedef enum MyEnumSysStat
{
	MyEnumStudy = 1,
	MyEnumRecogenition
};

class MyGlobal
{
public:
	MyGlobal();
	~MyGlobal();

public:
	vector<Mat> videoToFrames(char* videoPath,char* bgPath);//视频分帧二值化
	void preProcess(char* videoPath);
	Mat BinaryPic(Mat Pic, Mat bg);//原图减背景，转换为只含有运动目标的二值图
	void areaCompare(vector<Mat> Pic);//初级1：面积比较

	void findBodyContour(vector<Mat> frameVec,Mat background);
	void update_mhi(IplImage* img, IplImage* dst, int diff_threshold);
	void mainProcess(const char *fileVideo);
	void myMatFindLines(Mat &matImg);
	void myFindLines(IplImage* img);
	bool myFindContours(IplImage* g_image, IplImage** llimg_dst);
	CvRect myFindCvSeqRect(CvSeq* seq);
	void ComputeValidOutput();
	void ComputeValidOutputAll();
	bool OpenFileDig(CString &filepath);       //打开离线视频路径
	CStringA W2c(CString &wStr);
	int ShExecute(LPCWSTR lpFile, LPCWSTR lpPara, bool bRunNomal = true);
	int ShExecuteCmd(LPCWSTR lpFile, LPCWSTR lpPara, CString &outstr);
	void StopRuningMain(){ g_bRunning = false; }
	const char * GetPath_HMM_DB(){ return CStringA(g_pathHMM_db).GetString(); }
	CString & GetPath_HMM_DB_W(){ return g_pathHMM_db; }
	CString & GetStrHMMSeq(){ return m_strHMMSeq; }
	int GetnHMMSeqT(){ return m_nHMMSeqT; }
	CString & GetRunningDir(){ return g_RunningDir; }
	void SetShowVideo(BOOL bshow){ m_bShowVideo = bshow; }
	void SetSystemStateType(int type){ m_nSysStateType = type; }

public:
	int g_VideoWidth, g_VideoHeight;
	DWORD dwFrameInterval;
	DWORD g_nowdwFrametmms;
	DWORD g_prePeektmms;
	DWORD g_prePeekFrameNum;
	int  g_nowFrameNum;
	vector <int > vctValidOutPeekWHper;

private:
	int m_nSysStateType;
//	vector<myOutputResPeekframe> g_vctOutputResPeekframe;
//	vector<myRecentFrameInf> g_vctRecentFrameInf;
//	vector<myRecentFrameInf> g_vctAllFrameInf;
//	vector<myRecentFrameInf> g_vctPeekCheckFrame;
	vector<int> m_vctHMMSeq;
	vector<int> m_vctOutHMMSeq;
	CString m_strHMMSeq;
	CString m_strStudyPeekHMMSeq;
	int m_nHMMSeqT;
	const double g_f_max_perWH = 0.8, g_f_min_perWH = 0.1;
	const int g_maxPeekFrameDurationtmms = 700;
	const int MAX_RECENT_FRAME_INF_NUM = 10;
	const int RES_NUM_SAVE = 7;
	CString g_pathHMM_db;
	CString g_RunningDir;
	// 	double *f_percent_wh;
	// 	DWORD *tm_res_wh;
	int  g_now_resN;
	const int res_midframe = (RES_NUM_SAVE + 1) / 2 - 1;
	bool g_bRunning;
	BOOL m_bShowVideo;

	CvRect m_outlineRect;


	// ************************* update_mhi fun vars **************************//
	const double MHI_DURATION = 0.001;
	const double MAX_TIME_DELTA = 0.05;
	const double MIN_TIME_DELTA = 0.005;
	// number of cyclic frame buffer used for motion detection
	// (should, probably, depend on FPS)
	const int N = 2;

	// ring image buffer
	IplImage **buf;
	int last;

	// temporary images
	IplImage *mhi; // MHI
	IplImage *orient; // orientation
	IplImage *mask; // valid orientation mask
	IplImage *segmask; // motion segmentation map
	CvMemStorage* storage; // temporary storage

};

