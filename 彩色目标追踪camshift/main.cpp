

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

					calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
					/*
					��hist����������鷶Χ��һ��������һ����0~255
					void normalize(const InputArray src, OutputArray dst, double alpha=1,
					double beta=0,
					int normType=NORM_L2, int rtype=-1, InputArray mask=noArray())
					�����ڹ�һ��ʱ��normTypeӦ��Ϊcv::NORM_MINMAX��alphaΪ��һ��������ֵ��
					betaΪ��һ�������Сֵ
					http://www.cnblogs.com/mikewolf2002/archive/2012/10/24/2736504.html
					*/

					normalize(hist,hist,0,255,CV_MINMAX);
					trackWindow = selection;

					/*
					ֻҪ���ѡ�������ɿ�����û�а�������0��'c'����trackObjectһֱ����Ϊ1��
					��˸�if����  if( trackObject < 0 )  ֻ��ִ��һ�Σ���������ѡ���������
					*/

					trackObject = 1;

					/*
					�밴��'c'����һ���ģ������all(0)��ʾ���Ǳ��� ȫ����0

					inline CvScalar cvScalarAll( double val0123 );
					ͬʱ�趨VAL0,1,2,3��ֵ��
					OpenCV���Scalar��all����˼��
					scalar����Ԫ������Ϊ0����ʵ����scalar::all(n)������ԭ����CvScalarAll(n)��
					*/

					histimg = Scalar::all(0);

					/*
					histing��һ��200*300�ľ���hsizeӦ����ÿһ��bin�Ŀ�ȣ�
					Ҳ����histing�����ֳܷ�����bin����
					opencvֱ��ͼ��bins�д洢����ʲô?
					https://zhidao.baidu.com/question/337997654.html

					���� ����ֵ 0,0,1,2,3,10,12��13 ��
					��ֵ�binsΪ 0-6 Ϊ��һ��bin��7-13 Ϊһ��bins��
					��ôbins[0] ����һ��bins �洢�������� 4��
					ԭ���� 0,0,1,2,3�ڵ�һ��bin�ķ�Χ�ڣ�
					bins[1] �洢����Ϊ 3��ԭ���� 10,12,13�������[7-13]���bin�ڡ�

					Line111 : hsize=16
					*/

					int binW = histimg.cols / hsize;	//�����

					/*
						Mat::Mat(); //default
						Mat::Mat(int rows, int cols, int type);
						Mat::Mat(Size size, int type);
						Mat::Mat(int rows, int cols, int type, const Scalar& s);
					����˵����
						int rows����
						int cols����
						int type���μ�"Mat���Ͷ���"
						Size size������ߴ磬ע���͸ߵ�˳��Size(cols, rows)
						const Scalar& s�����ڳ�ʼ������Ԫ�ص���ֵ
						const Mat& m������m�ľ���ͷ���µ�Mat����
						���ǲ��������ݣ��൱�ڴ�����m��һ�����ö���
					ת�ԣ�http://blog.csdn.net/holybin/article/details/17751063
					����һ�����嵥bin��������ʹ�õ��ǵڶ��� ���� ������
					���غ�����https://baike.baidu.com/item/%E9%87%8D%E8%BD%BD%E5%87%BD%E6%95%B0/3280477?fr=aladdin
					*/

					Mat buf(1, hsize, CV_8UC3);

					/*
					saturate_cast����Ϊ��һ����ʼ����׼ȷ�任����һ����ʼ����
					saturate_cast<uchar>(int v)������ ���Ƿ�ֹ���������
					�����ԭ����Դ����������£�
					if(data<0)
							data=0;
					if(data>255)
					data=255

					ת��:http://blog.csdn.net/wenhao_ir/article/details/51545330?locationNum=10&fps=1
					Vec3bΪ3��charֵ������
					CV_8UC3 ��ʾʹ��8λ�� unsigned char �ͣ�ÿ������������Ԫ�������ͨ��,
					��ʼ��Ϊ��0��0��255��
					*/

					for (int i = 0;  i < hsize; i++)
					{
						/*
					   ����ɫ���180��
					   ��ɫ->hsv->hue(0,255)->roi->hist(0,255)

					   ��������ֻ�ǣ���iΪ���룬��ֱ��ͼ�����������ε���ɫ�����������buf�
					   hsv����ֵ��ȡֵ��Χ:
					   h 0-180
					   s 0-255
					   v 0-255
					   http://blog.csdn.net/taily_duan/article/details/51506776
					   https://wenku.baidu.com/view/eb2d600dbb68a98271fefadc.html
					   */

						buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
						cvtColor(buf, buf, CV_HSV2BGR);   //��hsv��ת����bgr����������ɫ����BGR��ʽ

					}

					for (int i = 0; i < hsize ;i++)
					{
						/*
					   at����Ϊ����һ��ָ������Ԫ�صĲο�ֵ
					   histimg.rows����=200
					   val�����������εĸ߶�
					   */
						int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);


						/*
					   C++: void rectangle(Mat& img, Rect rec, const Scalar& color, int thickness=1, int lineType=8, int shift=0 )
					   �������ܣ�
					   img ͼ��.
					   pt1 ���ε�һ�����㡣
					   pt2 ���ζԽ����ϵ���һ������
					   color ������ɫ (RGB) �����ȣ��Ҷ�ͼ�� ��(grayscale image����
					   thickness ��ɾ��ε������Ĵ�ϸ�̶ȡ�ȡ��ֵʱ���� CV_FILLED��
					   �������������ɫ�ʵľ��Ρ�
					   line_type ���������͡���cvLine������
					   https://zhidao.baidu.com/question/427970238676959132.html

					   shift ������С����λ����

					   histimg.rows��һ��������histmig.rows=200
					   Scalar(buf.at<Vec3b>(i))  buf����ɫ
					   ���������������Ͻǣ�x�ᳯ�ң�y����
					   val�����������εĸ߶�
					   */
						rectangle(histimg, Point(i*binW, histimg.rows), Point((i + 1)*binW, histimg.rows - val), Scalar(buf.at<Vec3b>(i)), -1, 8);

					}





				}
				/*
				����ͶӰ 
				http://blog.csdn.net/qq_18343569/article/details/48028065
				*/
				calcBackProject(&hue, 1, 0, hist, backppoj, &phranges);
				//����??????��һ��ע����Ҳû�� = = ------���ϵĻ���������˵������Ҫ�ٺܶ�

				backppoj &= mask;

				/*
				Camshift����MeanShift�㷨(Mean Shift�㷨���ֳ�Ϊ��ֵƯ���㷨)�ĸĽ���
				��Ϊ��������Ӧ��MeanShift�㷨��
				CamShift�㷨��ȫ����"Continuously Adaptive Mean-SHIFT"��
				���Ļ���˼������Ƶͼ�������֡��MeanShift���㣬������һ֡�Ľ��
				����Search Window�����ĺʹ�С��
				��Ϊ��һ֡MeanShift�㷨��Search Window�ĳ�ʼֵ����˵�����ȥ��


				����OPENCV�е�CAMSHIFT���ӣ���ͨ������Ŀ��HSV�ռ��µ�HUE����ֱ��ͼ��
				ͨ��ֱ��ͼ����ͶӰ�õ�Ŀ�����صĸ��ʷֲ���
				Ȼ��ͨ������CV���е�CAMSHIFT�㷨���Զ����ٲ�����Ŀ�괰�ڵ�����λ�����С��
				https://baike.baidu.com/item/Camshift/5302311?fr=aladdin

				cvCamShift(IplImage* imgprob, CvRect windowIn, CvTermCriteria criteria,
				CvConnectedComp* out, CvBox2D* box=0);
				imgprob��ɫ�ʸ��ʷֲ�ͼ��
				windowIn��Search Window�ĳ�ʼֵ��
				Criteria�������ж���Ѱ�Ƿ�ֹͣ��һ����׼��
				out������������,�����µ�Search Window��λ�ú������
				box�������������������С���Ρ�
				http://blog.csdn.net/houdy/article/details/191828

				CV_INLINE  CvTermCriteria  cvTermCriteria( int type, int max_iter,
				double epsilon )
				{
				CvTermCriteria t;
				t.type = type;
				t.max_iter = max_iter;
				t.epsilon = (float)epsilon;
				return t;
				}
				�ú������������������ص�ֵΪCvTermCriteria�ṹ�塣
				���ó��ú�������c�ӿ���ʹ��c������ģ���������Ľṹ�����еĲ���Ϊ��
				type��
				- CV_TERMCRIT_ITER  �ڵ��㷨������������max_iter��ʱ����ֹ��
				- CV_TERMCRIT_EPS   �ڵ��㷨�õ��ľ��ȵ���epsolonʱ��ֹ��
				-CV_TERMCRIT_ITER+CV_TERMCRIT_EPS
				���㷨��������max_iter���ߵ���õľ��ȵ���epsilon��ʱ���ĸ��������ֹͣ
				max_iter��������������
				epsilon��Ҫ��ľ���
				http://www.cnblogs.com/shouhuxianjian/p/4529174.html

				L231 trackWindow = selection;
				*/

				//trackWindow Ϊ���ѡ�������TermCriteriaΪȷ��������ֹ��׼��
				RotatedRect trackBox = CamShift(backppoj, trackWindow, TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1));

				if (trackWindow.area() <= 1)
				{
					int cols = backppoj.cols, rows = backppoj.rows, r = (MIN(cols, rows) + 5) / 6;
					trackWindow = Rect(trackWindow.x - r, trackWindow.y - r, trackWindow.x + r, trackWindow.y + r) & Rect(0, 0, cols, rows);


				}

				if (backprojMode)
				{
					cvtColor(backppoj, image, COLOR_GRAY2RGB);

				}
				/*
				void cvEllipse( CvArr* img, CvPoint center, CvSize axes, double angle,
				double start_angle, double end_angle, CvScalar color,
				int thickness=1, int line_type=8, int shift=0 );
				img                 ͼ��
				center              ��ԲԲ�����ꡣ

				axes                ��ĳ��ȡ�
				angle               ƫת�ĽǶȡ�
				start_angle         Բ����ʼ�ǵĽǶȡ�
				end_angle           Բ���ս�ǵĽǶȡ�
				color               ��������ɫ��
				thickness           �����Ĵ�ϸ�̶ȡ�
				line_type           ����������,��CVLINE��������
				shift               Բ������������ľ��ȡ�

				lineType �C ����
				Type of the line:
				8 (or omitted) - 8-connected line.
				4 - 4-connected line.
				CV_AA - antialiased line. ������ߡ�
				shift �C �����С����λ��.

				���ٵ�ʱ������ԲΪ����Ŀ��
				*/

				ellipse(image, trackBox, Scalar(0, 0, 255), 3, CV_AA);
			}



		}

		//����Ĵ����ǲ���pauseΪ�滹��Ϊ�ٶ�Ҫִ�е� 
		else if(trackObject<0)      //ͬʱҲ���ڰ�����ͣ��ĸ�Ժ�
		paused = false;

		if (selectObject && selection.width > 0 && selection.height > 0)
		{
			Mat roi(image, selection);
			bitwise_not(roi, roi);		//bitwise_not Ϊ��ÿһ��bitλȡ��


		}

		imshow("CamShift", image);
		imshow("ֱ��ͼ", histimg);
		int i;

		/*
	   waitKey(x);
	   ��һ�������� �ȴ�x ms������ڴ��ڼ��а������£����������������ذ��°�����
	   ASCII�룬���򷵻�-1
	   ���x=0����ô���޵ȴ���ȥ��ֱ���а�������
	   http://blog.sina.com.cn/s/blog_82a790120101jsp1.html
	   */

		char c = (char)waitKey(10);
		if (27 == c)     //�˳���
			break;
		switch (c)
		{
		case 'b':		//����ͶӰģ�� img/mask����
			backprojMode = !backprojMode;
			break;
		case 'c':		//�������Ŀ�����
				trackObject = 0;
				histimg = Scalar::all(0);
				break;
		case 'h':		//��ʾֱ��ͼ����
			showHist = !showHist;
			if (!showHist)
			{
				destroyWindow("ֱ��ͼ");
			}
			else
			{
				cvNamedWindow("ֱ��ͼ", 1);
			}
			break;

		case 'p':		//��ͣ���ٽ���
			paused = !paused;
			break;

		default:
			break;
		}


	}

	return 0;

}
