

/// --------------------------------------------------------------------
/// �������ڣ�2018/10/26  10:48
/// �����ߣ�������
/// ���䣺wangxianwenup@outlook.com
/// �����豸��Windows10 64bit + VisualStudio 2017
/// �������ڣ�
/// ������OpenCV ��ɫĿ��׷�٣��ο�OpenCV��װĿ¼��..\opencv\sources\samples\cpp\camshiftdemo.cpp\camshiftdemo.cpp
/// --------------------------------------------------------------------



#include <iostream>
//#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
//#include "opencv2/videoio.hpp"
#include<ctype.h>  //C��׼������ͷ�ļ���������һ��C�����ַ����ຯ��
#include "opencv2/highgui.hpp"
/*
highguiΪ��ƽ̨��gui/IO�����֧��ͼ����Ƶ������ͷ���ļ���ȡ��ʾ�Լ�ת��
*/

using namespace std;
using namespace cv;


//����ȫ�ֱ���




Mat image;
bool backprojMode = false;      //��ʾ�Ƿ���뷴��ͶӰģʽ��true��ʾ׼�����뷴��ͶӰģʽ
bool selectObject = false;		//�����Ƿ���ѡҪ���ٵĳ�ʼĿ�꣬true��ʾ���������ѡ��
int trackObject = 0;			//����׷��Ŀ����Ŀ ��
bool showHist = true;			//�Ƿ���ʾֱ��ͼ
Point origin;					//y=���ڱ�������һ��ѡ��ʱ�������λ��
Rect selection;					//���ڱ������ѡ��ľ��ο�
int vmin = 10, vmax = 256, smin = 30;

//onMouse���ص�����

static void onMouse(int event,int x,int y,int,void*)
{
	/*
		����ƶ�ʱ����if(selectObject)
		Ȼ���µ�������x,yֵ���ᴫ����
	
	*/
	if(selectObject)
	{
		selection.x = MIN(x, origin.x);        //�������ϽǶ�������
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);  //���ο�
		selection.height = std::abs(y - origin.y); //���θ�
	
		//����ȷ����ѡ�ľ���������ͼƬ��Χ�� ��
		selection &= Rect(0, 0, image.cols, image.rows);
	
	}

	switch (event)
	{
	 //��갴��ȥ��һ���¼�����������������棬����case CV_EVENT_LBUTTONDOWN: ��һ�� 
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		selectObject = true;    //ѡ�ڸ���ʱ�ĳ�ʼĿ��
		break;

	 //������̧������¼��������������case CV_EVENT_LBUTTONUP:��һ��  

	case CV_EVENT_LBUTTONUP:
		selectObject = false;   //��ѡ�ڸ���ʱ�ĳ�ʼĿ��
		if (selection.width >0 && selection.height > 0 )
			trackObject = -1;
		break;

	default:
		break;
	}



}


//help( )����  ���������Ϣ

static void ShowHelpText()
{
	cout<<"\n\n\t���ھ�ֵƯ�Ƶ�׷�٣�tracking������\n"
		"\t��������ѡһ������ɫ�����壬��������׷�ٲ���\n"; 


	cout << "\n\n\t����˵���� \n"
		"\t\t������ѡ��������ʼ������\n"
		"\t\tESC - �˳�����\n"
		"\t\tc - ֹͣ׷��\n"
		"\t\tb - ��/��-ͶӰ��ͼ\n"
		"\t\th - ��ʾ/����-����ֱ��ͼ\n"
		"\t\tp - ��ͣ��Ƶ\n";


}
 
const char* keys =
{
	"{1|  | 0 |  camera number}"

};



//main()����  ����̨Ӧ�ó������
int main()
{
	ShowHelpText(); 

	VideoCapture cap;   //����һ������ͷ����������
	Rect trackWindow;
	int hsize = 16;
	float hranges[] = { 0,180 };     //ֱ��ͼ�ķ�Χ  hranges�ں������ֱ��ͼ�ĺ������õ�
	const float* phranges = hranges;


	cap.open(0);     //���ó�Ա����������ͷ

	if (!cap.isOpened())
	{
		cout << "���ܳ�ʼ������ͷ\n";

	}

	namedWindow("ֱ��ͼ",0);
	namedWindow("CamShift", 0);
	setMouseCallback("CamShift",onMouse,0);  //��Ϣ��Ӧ����

	//createTrackbar�����Ĺ������ڶ�Ӧ�Ĵ��ڴ�����������
	createTrackbar("Vmin","CamShift",&vmin,256,0);		// ������Vmin, vmin��ʾ��������ֵ�����Ϊ256
	createTrackbar("Vmax", "CamShift", &vmax, 256, 0);  //���һ������Ϊ0����û�е��û����϶�����Ӧ����  
	createTrackbar("Smin","CamShift",&smin,256,0);		//vmin,vmax,smin��ʼֵ�ֱ�Ϊ10,256,30  

	/*
		CV_8UC1��CV_8UC2��CV_8UC3������1��2��3��ʾͨ������Ʃ��RGB3ͨ������CV_8UC3��
		CV_8UC3 ��ʾʹ��8λ�� unsigned char �ͣ�ÿ������������Ԫ�������ͨ��,
		��ʼ��Ϊ��0��0��255��
		http://blog.csdn.net/augusdi/article/details/8876459

		Mat::zeros ����ָ���Ĵ�С�����͵������顣
		C++: static MatExpr Mat::zeros(int rows, int cols, int type)
		rows�C������    cols  �C������type�C �����ľ�������͡�
		A = Mat::zeros ��3��3��CV_32F����
		�������ʾ���У�ֻҪA���� 3 x 3����������ͻᱻ�����µľ���
		����Ϊ���еľ��� A����㡣
		ת��:http://blog.csdn.net/sherrmoo/article/details/40951997

		hist ֱ��ͼ���־��� ��� -> histimg ֱ��ͼͼ��
		hsv ->��ȡ��h�� hue
		mask�� ��Ĥ��  --------------------------------------------------------------------------------------------------����������
		backproj ����ͶӰ�ľ���
	*/


	Mat frame, hsv, hue, mask, hist, histimg = Mat::zeros(200, 300, CV_8UC3), backppoj;
	bool paused = false;

	for (;;)
	{
		if (!paused)    //û����ͣ
		{
			cap >> frame;       //������ͷץȡһ֡ͼ�����뵽frame��
			if (frame.empty())
				break;
		}

		frame.copyTo(image);

		if (!paused)  //û�а�����ͣ
		{
			cvtColor(image, hsv, CV_BGR2HSV);      //��rgb����ͷ֡ת����hsv�ռ��---תhsv

			 //trackObject��ʼ��Ϊ0,���߰�����̵�'c'����ҲΪ0������굥���ɿ���Ϊ-1 
			if (trackObject)
			{
				int _vmin = vmin, _vmax = vmax;
				/*
				inRange�����Ĺ����Ǽ����������ÿ��Ԫ�ش�С�Ƿ���2��������ֵ֮�䣬
				�����ж�ͨ��,mask����0ͨ������Сֵ��Ҳ����h����
				����������hsv��3��ͨ����
				�Ƚ�h,0~180,s,smin~256,v,min(vmin,vmax),max(vmin,vmax)��
				���3��ͨ�����ڶ�Ӧ�ķ�Χ�ڣ���
				mask��Ӧ���Ǹ����ֵȫΪ1(0xff)������Ϊ0(0x00).
				*/

				inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)), Scalar(180, 256, MAX(_vmin, _vmax)), mask);
				int ch[] = { 0,0 };

				//hue��ʼ��Ϊ��hsv��С���һ���ľ���ɫ���Ķ������ýǶȱ�ʾ��
				//������֮�����120�ȣ���ɫ���180��
				hue.create(hsv.size(), hsv.depth());          //HSV(Hue,saturation,Value)

				/*
				int from_to[] = { 0,2, 1,1, 2,0, 3,3 };
				mixChannels( &rgba, 1, out, 2, from_to, 4 );
				from_to:ͨ�������ԣ�������ÿ����Ԫ��Ϊһ�ԣ���ʾ��Ӧ�Ľ���ͨ��
				pair_count��ͨ�������Ը�������*from_to������������
				http://blog.163.com/jinlong_zhou_cool/blog/static/22511507320138932215239/

				bgr ��ԭɫRGB������γ���������ɫ�����Բ�����һ��ֵ����ʾ��ɫ
				hsv H��ɫ��  S����ǳ,S = 0ʱ��ֻ�лҶ�  V������
				hsv -> hue ��ɫ�ʵ����ֳ���
				http://blog.csdn.net/viewcode/article/details/8203728
				*/

				mixChannels(&hsv, 1, &hue, 1, ch, 1);
				if (trackObject < 0)    //���ѡ�������ɿ��󣬸ú����ڲ��ֽ��丳ֵ-1
				{
					//�˴��Ĺ��캯��roi�õ���Mat hue�ľ���ͷ
					//��roi������ָ��ָ��hue����������ͬ�����ݣ�selectΪ�����Ȥ������
					Mat roi(hue,selection),maskroi(mask,selection);		//mask�����hsv����Сֵ
					 /*
					��roi��0ͨ������ֱ��ͼ��ͨ��mask����hist�У�hsizeΪÿһάֱ��ͼ�Ĵ�С
					calcHist����������ͼ��ֱ��ͼ
					---calcHist����������ʽ
					C++: void calcHist(const Mat* images, int nimages, const int* channels,
					InputArray mask, OutputArray hist, int dims, const int* histSize,
					const float** ranges, bool uniform=true, bool accumulate=false
					�������
					onst Mat* images������ͼ��
					int nimages������ͼ��ĸ���
					const int* channels����Ҫͳ��ֱ��ͼ�ĵڼ�ͨ��
					InputArray mask����Ĥ����������Ĥ�ڵ�ֱ��ͼ  ...Mat()
					OutputArray hist:�����ֱ��ͼ����
					int dims����Ҫͳ��ֱ��ͼͨ���ĸ���
					const int* histSize��ָ����ֱ��ͼ�ֳɶ��ٸ����䣬�������� bin�ĸ���
					const float** ranges�� ͳ������ֵ������
					bool uniform=true::�Ƿ�Եõ���ֱ��ͼ������й�һ������
					bool accumulate=false���ڶ��ͼ��ʱ���Ƿ��ۼƼ�������ֵ�ø���
					http://blog.csdn.net/qq_18343569/article/details/48027639
					*/


				}
			}



		}
	}

}
