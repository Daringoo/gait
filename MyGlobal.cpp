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
	GetModuleFileName(NULL, szFilePath, MAX_PATH);  //��ȡ��ǰ�����Ѽ���ģ����ļ�������·��
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
bool MyGlobal::OpenFileDig(CString &filepath){                           //�򿪶Ի���
	OPENFILENAME ofn;      // �����Ի���ṹ��
	TCHAR szFile[MAX_PATH] = L""; // �����ȡ�ļ����ƵĻ�������          

	// ��ʼ��ѡ���ļ��Ի���
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Video Files(*.*)\0*.*\0\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;

	//ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// ��ʾ��ѡ���ļ��Ի���
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
	
	VideoCapture capture(videopath);//�Ƿ��ܹ��Լ��������ͷſռ䣿
	if (!capture.isOpened())
	{
		std::cout << "���ܴ���Ƶ�ļ�" << endl;
		getchar();
	}
	vector<Mat> frameVec;
	Mat background = imread(bgPath);  
	cv::cvtColor(background.clone(), background, CV_RGB2GRAY);
//	imshow("����ͼ", background);
//	cv::waitKey(0);
	while (true)  //����
	{
		Mat frame;
		bool hasMoreFrame = capture.read(frame);
		if (!hasMoreFrame)
		{
			break;
		}
	//	��ֵ��
//		Mat background = frameVec[frameVec.size() - 1];//��Ϊ����ͼ�񣬴˴�����	
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
//		imshow("��ȥ������Ķ�ֵ��ͼ", grayFrame);
//		cv::waitKey(50);
		
		frameVec.push_back(grayFrame);  //������������
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
		std::cout << "���ܴ���Ƶ�ļ�" << endl;
		getchar();
	}
	vector<Mat> frameVec;
	while (true)  //����
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
	Mat background(frameHeight, frameWidth, CV_8UC1);//�Ҷȹ�����±���ֵ
	for (int nrows = 0; nrows < (int)frameHeight; nrows++)
	{
		for (int ncols = 0; ncols < (int)frameWidth; ncols++)  //ÿ�����ض�������Ҷȱ���ֵ
		{
			//step1�����㵥�����صĻҶ�ƽ������
			int j = 0;//��ʾ�������صĻҶ�������
			for (int f = 0; f < frameVec.size() - 1; f++)//��������֡��ĳ�����ص�ĻҶȲ�ֵ
			{
				int a = abs(frameVec[f].at<uchar>(nrows, ncols) - frameVec[f + 1].at<uchar>(nrows, ncols));
				if (a < Thres)//����ͬһ�Ҷ�����
				{
				//	temp1.push_back(frameVec[f + 1].at<uchar>(nrows, ncols));
					valueRange[j].push_back(frameVec[f + 1].at<uchar>(nrows, ncols));//��άvector��ʼ��
					getchar();
				}
				else
				{
					j++;
					valueRange[j].push_back(frameVec[f + 1].at<uchar>(nrows, ncols));
				}
			}
			//step2:��������Ҷ�ƽ�������ƽ���Ҷ�ֵ
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
			//step3:�Ҷ�����Ĺ�λһ��
			//�������(x,y)�Ҷ�ֵ��������ݽṹΪvector<vector<int>> ValueRange
			//���ÿ���Ҷ�ֵ����ƽ���Ҷȵ����ݽṹΪvector<int> valueMean
			//valueMean.size()=valueRange.size()
			if (valueMean.size()==1)  //��ֻ��һ���Ҷ�����
			{
				int a = nrows;
				int b = ncols;
				background.at<uchar>(nrows, ncols) =(int) valueMean[0];  //���ѽ����Mat�Ĺ��캯������������������
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
				//step 4:ѡ�Ҷ������ڻҶ�ֵ��������ƽ��ֵ��Ϊ�Ҷȱ���ֵ
				//�ҵ��������
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
				//���������Ҷ�ƽ��ֵ
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

// ��ʾȥ����֮��Ķ�ֵͼ
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
	//��������С

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
			if (bodyRect.area()>5000)  //�ҵ���������
			{
				bodySize++;
				break;
			}
		}
		/**���Ի��������*/
			Point pt1, pt2;
			pt1.x = bodyRect.x;
			pt1.y = bodyRect.y;
			pt2.x = bodyRect.x + bodyRect.width;
			pt2.y = bodyRect.y + bodyRect.height;
			rectangle(*it, pt1, pt2, Scalar(255, 255, 255), 1, 8, 0);

     	imshow("���ָ�Ч��ͼ", *it);
		waitKey(50);
		//�ҳ�������Сƽ��ֵ
		bodyArea += bodyRect.area();
	}

	bodyArea = bodyArea / bodySize;
	InitConsoleWindow();
	printf("bodyArea=%d\n", bodyArea);
	getchar();

}