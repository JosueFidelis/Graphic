#include <bits/stdc++.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include "opencv2/opencv.hpp"
#include <thread>
using namespace std;
using namespace cv;

double translate_x = 0, translate_y = 0, translate_z = 0, rotate_x = 0, rotate_y = 0, rotate_z = 0, z_axis = -2, heightI/*y*/, heightO, widthI/*x*/, widthO;
string object1;


void CVThread () {
    bool ok = true;

    double res = 0, ans, x_axis = 0, y_axis = 0, x_axis1 = 0, y_axis1 = 0;

    namedWindow("Match", WINDOW_AUTOSIZE);
	Mat image1 = imread(object1, IMREAD_GRAYSCALE);
	if (image1.empty()) {
		cout << "Image1 vazia\n";
		ok = false;
	}

	VideoCapture cap(0);
	if (!cap.isOpened()) {
		ok = false;
	}
	Mat image2, img;
    
    
    time_t begin = time(0);
    int tick = 0;
    int frameCounter = 0;


	for (;ok;) {
		cap >> img;
		if (img.empty()) {
			cout << "Image2 vazia\n";
			ok = false;
            break;
		}
		cvtColor(img, image2, CV_BGR2GRAY);


		int minHessian = 400;
		vector<KeyPoint> kp1, kp2;
		Mat descriptor1, descriptor2;


		Ptr<Feature2D> orb = ORB::create(minHessian);
		orb->detectAndCompute(image1, Mat(), kp1, descriptor1);
		orb->detectAndCompute(image2, Mat(), kp2, descriptor2);


		if ( descriptor1.empty() ) {
			cvError(0,"MatchFinder","1st descriptor empty",__FILE__,__LINE__);    
			ok = false;
            break;
		}


		descriptor1.convertTo(descriptor1, CV_32F); descriptor2.convertTo(descriptor2, CV_32F);

        if (descriptor2.empty()) {
		    if (waitKey(33) == 27) {
			    ok = false;
		    }
            continue;
        }

		
		vector<DMatch> matches;
		FlannBasedMatcher matcher;
		matcher.match(descriptor1, descriptor2, matches);


        //triagem dos melhores pontos
		vector<DMatch> GMatches;
		double min = 100, max = 0;

		for (int i = 0; i < descriptor1.rows; i ++) {
			if (matches[i].distance < min) {
				min = matches[i].distance;
			}
			if (matches[i].distance > max) {
				max = matches[i].distance;
			}
		}

		for (int i = 0; i < descriptor1.rows; i ++) {
			if ((matches[i].distance < 2.7 * min)) {
				GMatches.push_back(matches[i]);
			}
		}



		//Desenha as linhas entre os keypoints em imageMatch

		Mat imageMatch;
		drawMatches(image1, kp1, image2, kp2, GMatches, imageMatch, Scalar::all(-1), Scalar::all(-1), 
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS); 


        if (GMatches.size() <= 8) {
            imshow("Match", imageMatch);
		    if (waitKey(33) == 27) {
			    ok = false;
		    }
            
            frameCounter ++;
            time_t timenow = time(0) - begin;
            if (timenow - tick >= 1) {
                tick ++;
                cout << frameCounter << "\n";
                frameCounter = 0;
            }
            continue;
        }


		vector<Point2f> staticImage;
		vector<Point2f> videoImage;

		for (int i = 0; i < GMatches.size(); i ++) {
			staticImage.push_back(kp1[GMatches[i].queryIdx].pt);
			videoImage.push_back(kp2[GMatches[i].trainIdx].pt);
		}

		Mat homog = findHomography(staticImage, videoImage, CV_RANSAC);

        if (homog.empty()) {
            imshow("Match", imageMatch);
		    if (waitKey(33) == 27) {
			    ok = false;
		    }
            frameCounter ++;
            time_t timenow = time(0) - begin;
            if (timenow - tick >= 1) {
                tick ++;
                cout << frameCounter << "\n";
                frameCounter = 0;
            }
            continue;
        }

		vector<Point2f> corner(4);

		corner[0] = cvPoint(0, 0);
		corner[1] = cvPoint(image1.cols, 0);
		corner[2] = cvPoint(image1.cols, image1.rows);
		corner[3] = cvPoint(0, image1.rows);
		
		vector<Point2f> videoCorner(4);	
		perspectiveTransform(corner, videoCorner, homog);


		line(imageMatch, videoCorner[0] + Point2f(image1.cols, 0), videoCorner[1] + Point2f(image1.cols, 0), Scalar(255, 0, 0), 4);
		line(imageMatch, videoCorner[1] + Point2f(image1.cols, 0), videoCorner[2] + Point2f(image1.cols, 0), Scalar(255, 0, 0), 4);
		line(imageMatch, videoCorner[2] + Point2f(image1.cols, 0), videoCorner[3] + Point2f(image1.cols, 0), Scalar(255, 0, 0), 4);
		line(imageMatch, videoCorner[3] + Point2f(image1.cols, 0), videoCorner[0] + Point2f(image1.cols, 0), Scalar(255, 0, 0), 4);

        
        //Fazendo o calculo do FPS
        frameCounter ++;
        time_t timenow = time(0) - begin;
        if (timenow - tick >= 1) {
            tick ++;
            cout << frameCounter << "\n";
            frameCounter = 0;
        }

		imshow("Match", imageMatch);
		if (waitKey(33) == 27) {
			ok = false;
		}


        //movendo objeto pelo eixo z
        ans = norm(videoCorner[1] - videoCorner[0]);
        if (res > ans) {
            z_axis -= 0.4;
        } else if (ans > res) {
            z_axis += 0.4;
        }
        res = norm(videoCorner[1] - videoCorner[0]);

        //movendo objeto pelo eixo x
        if (x_axis > videoCorner[0].x && x_axis1 > videoCorner[1].x) {
            widthI -= 0.4;
        } else if (x_axis < videoCorner[0].x && x_axis1 < videoCorner[1].x) {
            widthI += 0.4;
        }
        x_axis = videoCorner[0].x; x_axis1 = videoCorner[1].x;

        //movendo objeto pelo eixo y
        if (y_axis > videoCorner[0].y && y_axis1 > videoCorner[1].y) {
            heightI += 0.4;
        } else if (y_axis < videoCorner[0].y && y_axis1 < videoCorner[1].y) {
            heightI -= 0.4;
        }
        y_axis = videoCorner[0].y; y_axis1 = videoCorner[1].y;

		
	} 
	destroyAllWindows();
}


void drawCube() {
    glBegin(GL_QUADS);
        //front
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(widthI, heightI, z_axis); //(x, y)
        glVertex3f((widthI) - widthO * 2, heightI, z_axis); //(-x, y)
        glVertex3f((widthI) - widthO * 2, (heightI) - heightO * 2, z_axis); //(-x, -y)
        glVertex3f(widthI, (heightI) - heightO * 2, z_axis); //(x, -y)

        //back
        glColor3f(0.0, 0.0, 1.0);
        glVertex3f(widthI, heightI, z_axis - 1); //(x, y)
        glVertex3f((widthI) - widthO * 2, heightI, z_axis - 1); //(-x, y)
        glVertex3f((widthI) - widthO * 2, (heightI) - heightO * 2, z_axis - 1); //(-x, -y)
        glVertex3f(widthI, (heightI) - heightO * 2, z_axis - 1);//(x, -y)

        //left
        glColor3f(0.0, 1.0, 0.0);
        glVertex3f((widthI) - widthO * 2, heightI, z_axis); //(-x, y, +)
        glVertex3f((widthI) - widthO * 2, heightI, z_axis - 1); //(-x, y, -)
        glVertex3f((widthI) - widthO * 2, (heightI) - heightO * 2, z_axis - 1); //(-x, -y, -)
        glVertex3f((widthI) - widthO * 2, (heightI) - heightO * 2, z_axis); //(-x, -y, +)

        //right
        glColor3f(1.0, 1.0, 0.0);
        glVertex3f(widthI, heightI, z_axis); //(x, y, +)
        glVertex3f(widthI, (heightI) - heightO * 2, z_axis); //(x, -y, +)
        glVertex3f(widthI, (heightI) - heightO * 2, z_axis - 1); //(x, -y, -)
        glVertex3f(widthI, heightI, z_axis - 1); //(x, y, -)

        //top
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f((widthI) - widthO * 2, heightI, z_axis); //(-x, y, +)
        glVertex3f(widthI, heightI, z_axis); //(x, y, +)
        glVertex3f(widthI, heightI, z_axis - 1); //(x, y, -)
        glVertex3f((widthI) - widthO * 2, heightI, z_axis - 1); //(-x, y, -)

        //bot
        glColor3f(0.5, 0.5, 1.0);
        glVertex3f((widthI) - widthO * 2, (heightI) - heightO * 2, z_axis); //(-x, -y, +)
        glVertex3f((widthI) - widthO * 2, (heightI) - heightO * 2, z_axis - 1); //(-x, -y, -)
        glVertex3f(widthI, (heightI) - heightO * 2, z_axis - 1); //(x, -y, -)
        glVertex3f(widthI, (heightI) - heightO * 2, z_axis); //(x, -y, +)
    glEnd();
}

void display () {

    glPushMatrix();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glRotatef(rotate_x, 1.0, 0.0, 0.0);
    glRotatef(rotate_y, 0.0, 1.0, 0.0);
    glRotatef(rotate_z, 0.0, 0.0, 1.0);

    glTranslatef(translate_x, 0, 0);
    glTranslatef(0, translate_y, 0);
    glTranslatef(0, 0, translate_z);
    drawCube();
    
    glPopMatrix();

    glutSwapBuffers();
}

void key (int key, int x, int y) {

    //translação
    if (key == GLUT_KEY_RIGHT) {
        translate_x -= 1;
    } else if (key == GLUT_KEY_LEFT) {
        translate_x += 1;
    }
    if (key == GLUT_KEY_UP) {
        translate_y -= 1;
    } else if (key == GLUT_KEY_DOWN) {
        translate_y += 1;
    }
    if (key == GLUT_KEY_F1) {
        translate_z -= 1;
    } else if (key == GLUT_KEY_F2) {
        translate_z += 1;
    }

    //rotação
    if (key == GLUT_KEY_F4) {
        rotate_y += 5;
    } else if (key == GLUT_KEY_F5) {
        rotate_y -= 5;
    }
    if (key == GLUT_KEY_F8) {
        rotate_x += 5;
    } else if (key == GLUT_KEY_F9) {
        rotate_x -= 5;
    }
    if (key == GLUT_KEY_F11) {
        rotate_z += 5;
    } else if (key == GLUT_KEY_F12) {
        rotate_z -= 5;
    } 

    glutPostRedisplay();
}

void reshape (int w, int h) {
    glViewport(0, 0, w, h);
}

void timer (int) {
    glutPostRedisplay();
    glutTimerFunc(1000/30, timer, 0);
}

int main (int argc, char * argv[]) {

    cout << "Qual o nome do arquivo a ser rastreado?\n"; 
    cin >> object1;

    Mat image1 = imread(object1, IMREAD_GRAYSCALE);
	if (image1.empty()) {
		cout << "Image1 vazia\n";
		return 0;
	}
    
    if (image1.size().width > image1.size().height) {
        widthI = image1.size().width/image1.size().height;
        heightI = 1;
    } else {
        heightI = image1.size().height/image1.size().width;
        widthI = 1;
    }
    widthO = widthI;
    heightO = heightI;

    thread first (CVThread);

    glutInit(&argc, argv);

    glutInitWindowSize(500,500);
    glutInitWindowPosition(0,0);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); //
    glutCreateWindow("Objeto"); //
    glEnable(GL_DEPTH_TEST); //

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(1, timer, 0);
    glutSpecialFunc(key);

    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, 1, 1, 20);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(-4.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    glutMainLoop();
    return 0;
} 