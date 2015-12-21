#include"stdafx.h"
#include "MyGlobal.h"
#include <commdlg.h>
#include<iostream>
#include <io.h>
#include <fcntl.h>
void InitConsoleWindow()
{
	int nCrt = 0;
	FILE* fp;
	AllocConsole();
	nCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	fp = _fdopen(nCrt, "w");
	*stdout = *fp;
	setvbuf(stdout, NULL, _IONBF, 0);
}
MyGlobal::MyGlobal()
{
	g_nowdwFrametmms = 0;
	g_prePeektmms = 0;
	g_prePeekFrameNum = 0;
	g_nowFrameNum = 0;
	// 	f_percent_wh = new double[RES_NUM_SAVE];
	// 	tm_res_wh = new DWORD[RES_NUM_SAVE];
	g_now_resN = 0;
	m_outlineRect = cvRect(0, 0, 300, 400);

	buf = NULL;
	last = 0;

	mhi = NULL; // MHI
	orient = NULL;
	mask = NULL;
	segmask = NULL;
	storage = NULL;

	g_bRunning = true;

	TCHAR szFilePath[MAX_PATH + 1];
	GetModuleFileName(NULL, szFilePath, MAX_PATH);  //获取当前进程已加载模块的文件的完整路径
	(wcsrchr(szFilePath, '\\'))[1] = '\0';
	g_RunningDir = szFilePath;

	g_pathHMM_db = g_RunningDir + L"hmm_db\\";

}


MyGlobal::~MyGlobal()
{

	int i;
	if (buf){
		for (i = 0; i < N; i++) {
			if (buf[i])cvReleaseImage(&buf[i]);
		}
	}

	if (mhi)cvReleaseImage(&mhi);
	if (orient) cvReleaseImage(&orient);
	if (segmask)cvReleaseImage(&segmask);
	if (mask)cvReleaseImage(&mask);
	if (storage)cvReleaseMemStorage(&storage);
}
bool MyGlobal::OpenFileDig(CString &filepath){                           //打开对话框
	OPENFILENAME ofn;      // 公共对话框结构。
	TCHAR szFile[MAX_PATH] = L""; // 保存获取文件名称的缓冲区。          

	// 初始化选择文件对话框。
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Video Files(*.*)\0*.*\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;

	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// 显示打开选择文件对话框。
	if (GetOpenFileName(&ofn))
	{
		filepath = szFile;
		return true;
	}
	return false;

}
CStringA MyGlobal::W2c(CString &wStr){
	CStringA strA(wStr);
	//WideCharToMultiByte(CP_ACP, 0, wszText, -1, szText, 100, NULL, NULL);
	return strA;
}
vector<Mat> MyGlobal::videoToFrames(char* videopath,char* bgPath)
{
	
	VideoCapture capture(videopath);//是否能够自己析构，释放空间？
	if (!capture.isOpened())
	{
		std::cout << "不能打开视频文件" << endl;
		getchar();
	}
	vector<Mat> frameVec;
	Mat background = imread(bgPath);  
	cv::cvtColor(background.clone(), background, CV_RGB2GRAY);
//	imshow("背景图", background);
//	cv::waitKey(0);
	while (true)  //跳出
	{
		Mat frame;
		bool hasMoreFrame = capture.read(frame);
		if (!hasMoreFrame)
		{
			break;
		}
	//	二值化
//		Mat background = frameVec[frameVec.size() - 1];//作为背景图像，此处例外	
		Mat grayFrame;
		cv::cvtColor(frame, grayFrame, CV_RGB2GRAY);
		grayFrame = abs(grayFrame - background);
		for (int nrows = 0; nrows < grayFrame.rows; nrows++)
		{
			for (int ncols = 0; ncols < grayFrame.cols; ncols++)
			{
				/*	std::cout << grayFrame.at<uchar>(nrows, ncols) << std::endl;
				getchar();*/
				if (grayFrame.at<uchar>(nrows, ncols)>20)
				{
					grayFrame.at<uchar>(nrows, ncols) = 255;
				}
				else
				{
					grayFrame.at<uchar>(nrows, ncols) = 0;
				}
			}
		}
//		imshow("除去背景后的二值化图", grayFrame);
//		cv::waitKey(50);
		
		frameVec.push_back(grayFrame);  //结束，看到这
//		imshow("videoframe", frame);
//		waitKey(1000);
	}
	return frameVec;
}

void MyGlobal::preProcess(char* videoPath)
{

	VideoCapture capture(videoPath);
	if (!capture.isOpened())
	{
		std::cout << "不能打开视频文件" << endl;
		getchar();
	}
	vector<Mat> frameVec;
	while (true)  //跳出
	{
		Mat frame;
		bool hasMoreFrame = capture.read(frame);
		if (!hasMoreFrame)
		{
			break;
		}
		cvtColor(frame.clone(), frame, CV_RGB2GRAY);
		frameVec.push_back(frame);
	}
	double frameWidth = capture.get(CV_CAP_PROP_FRAME_WIDTH);
	double frameHeight = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	int Thres = 21;
	vector<vector<int>> valueRange(frameHeight);  
	vector<int> temp1,temp2;
	Mat background(frameHeight, frameWidth, CV_8UC1);//灰度归类的新背景值
	for (int nrows = 0; nrows < (int)frameHeight; nrows++)
	{
		for (int ncols = 0; ncols < (int)frameWidth; ncols++)  //每个像素都计算其灰度背景值
		{
			//step1：计算单个像素的灰度平稳区间
			int j = 0;//表示单个像素的灰度区间数
			for (int f = 0; f < frameVec.size() - 1; f++)//计算相邻帧的某个像素点的灰度差值
			{
				int a = abs(frameVec[f].at<uchar>(nrows, ncols) - frameVec[f + 1].at<uchar>(nrows, ncols));
				if (a < Thres)//属于同一灰度区间
				{
				//	temp1.push_back(frameVec[f + 1].at<uchar>(nrows, ncols));
					valueRange[j].push_back(frameVec[f + 1].at<uchar>(nrows, ncols));//二维vector初始化
					getchar();
				}
				else
				{
					j++;
					valueRange[j].push_back(frameVec[f + 1].at<uchar>(nrows, ncols));
				}
			}
			//step2:计算各个灰度平稳区间的平均灰度值
			vector<int> valueMean;
			for (int i = 0; i <= j; i++)
			{
				int sum = 0;
				double mean = 0.0;
				for (vector<int>::iterator it = valueRange[i].begin(); it != valueRange[i].end(); it++)
				{
					sum += *it;
				}
				mean = (int)sum / valueRange[i].size();
				valueMean.push_back(mean);
			}
			//step3:灰度相近的归位一类
			//存放像素(x,y)灰度值区间的数据结构为vector<vector<int>> ValueRange
			//存放每个灰度值区间平均灰度的数据结构为vector<int> valueMean
			//valueMean.size()=valueRange.size()
			if (valueMean.size()==1)  //若只有一个灰度区间
			{
				int a = nrows;
				int b = ncols;
				background.at<uchar>(nrows, ncols) =(int) valueMean[0];  //！已解决！Mat的构造函数加上了行列与类型
			}
			else
			{
				for (int i = 0; i < valueMean.size() - 1; i++)
				{
					
					for (int j = i + 1; j < valueMean.size(); j++)
					{
						if (abs(valueMean[i] - valueMean[j]) < Thres)
						{
							for (int num = 0; num < valueRange[j].size(); num++)
							{
								valueRange[i].push_back(valueRange[j][num]);
							}

						}
					}
				}
				//step 4:选灰度区间内灰度值最多的区间平均值作为灰度背景值
				//找到最大区间
				int script = 0;
				for (int i = 1; i < valueRange.size(); i++)
				{
					int max = valueRange[0].size();

					if (valueRange[i].size() > max)
					{
						max = valueRange[i].size();
						script = i;
					}
				}
				//求最大区间灰度平均值
				for (int i = 0; i < valueRange.size(); i++)
				{
					if (script == valueRange[i].size())
					{
						int sum = 0;
						double mean = 0.0;
						for (vector<int>::iterator it = valueRange[i].begin(); it != valueRange[i].end(); it++)
						{
							sum += *it++;
						}
						mean = (int)sum / valueRange[i].size();
					}
				}
				background.at<uchar>(nrows, ncols) = (int)mean;
			}
			
		}
	}//two nrows,ncols loop end

}

// 显示去背景之后的二值图
Mat MyGlobal::BinaryPic(Mat Pic, Mat bg)
{
	Mat grayFrame = abs(Pic - bg);
	for (int nrows = 0; nrows < grayFrame.rows; nrows++)
	{
		for (int ncols = 0; ncols < grayFrame.cols; ncols++)
		{
			/*	std::cout << grayFrame.at<uchar>(nrows, ncols) << std::endl;
			getchar();*/
			if (grayFrame.at<uchar>(nrows, ncols)>20)
			{
				grayFrame.at<uchar>(nrows, ncols) = 255;
			}
			else
			{
				grayFrame.at<uchar>(nrows, ncols) = 0;
			}
		}
	}
	return grayFrame;
}

void MyGlobal::areaCompare(vector<Mat> Pic)
{
	//找轮廓大小

	int bodyArea=0;
	int bodySize = 0;
	for (vector<Mat>::iterator it=Pic.begin(); it!=Pic.end(); it++)
	{
		vector<vector<Point>> contours;
		findContours((*it).clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		Rect bodyRect;
		for (int i = 0; i<contours.size(); i++)
		{
			bodyRect = boundingRect(contours[i]);
			if (bodyRect.area()>5000)  //找到身子主体
			{
				bodySize++;
				break;
			}
		}
		/**测试画出人体框*/
			Point pt1, pt2;
			pt1.x = bodyRect.x;
			pt1.y = bodyRect.y;
			pt2.x = bodyRect.x + bodyRect.width;
			pt2.y = bodyRect.y + bodyRect.height;
			rectangle(*it, pt1, pt2, Scalar(255, 255, 255), 1, 8, 0);

     	imshow("最后分割效果图", *it);
		waitKey(50);
		//找出轮廓大小平均值
		bodyArea += bodyRect.area();
	}

	bodyArea = bodyArea / bodySize;
	InitConsoleWindow();
	printf("bodyArea=%d\n", bodyArea);
	getchar();

}