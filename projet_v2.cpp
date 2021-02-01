/*
 * Fichier source pour le projet d'unité
 *  INF-4101C
 *---------------------------------------------------------------------------------------------------
 * Pour compiler : g++ `pkg-config --cflags opencv` projet_v2.cpp `pkg-config --libs opencv` -o projet_v2
 *---------------------------------------------------------------------------------------------------
 * auteur : Eva Dokladalova 09/2015
 * modification : Eva Dokladalova 10/2017
 */


/* 
 * Libraries stantards 
 *
 */ 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
 * Libraries OpenCV "obligatoires" 
 *
 */
//#include "/usr/include/opencv/highgui.h" 
//#include "/usr/include/opencv/cv.h"
#include "highgui.h"
#include "cv.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
  
/* 
 * Définition des "namespace" pour évite cv::, std::, ou autre
 *
 */  
using namespace std;
using namespace cv;
using std::cout;

/*
 * Some usefull defines
 * (comment if not used)
 */
#define PROFILE
#define VAR_KERNEL
#define N_ITER 100

#ifdef PROFILE
#include <time.h>
#include <sys/time.h>
#endif


void sobel(Mat frame1, Mat grad){
  int x;
  int y;

  int i;
  int j;
  for(i = 1; i < frame1.rows-1; i++){
    for(j = 1; j < frame1.cols-1; j++){
      x = -(int)frame1.at<uchar>(i-1, j-1) + (int)frame1.at<uchar>(i+1, j+1) -
          2*(int)frame1.at<uchar>(i-1, j) + (int)2*frame1.at<uchar>(i+1, j) -
          (int)frame1.at<uchar>(i-1, j+1) + (int)frame1.at<uchar>(i+1, j+1);

      y = -(int)frame1.at<uchar>(i-1, j-1) - 2*(int)frame1.at<uchar>(i , j-1) -
          (int)frame1.at<uchar>(i+1, j-1) + (int)frame1.at<uchar>(i-1, j+1) +
          2*(int)frame1.at<uchar>(i, j+1) + (int)frame1.at<uchar>(i+1, j+1);

      grad.at<uchar>(i ,j) = sqrt(x*x + y*y);
    }
  }
}

int compare_function(const void *a,const void *b) {
  int *x = (int *) a;
  int *y = (int *) b;
  return *x - *y;
}

void median(Mat frame_gray, Mat frame1){
  int tab_vois[9];
  int i;
  int j;
  for(i = 1; i < frame_gray.rows-1; i++){
    for(j = 1; j < frame_gray.cols-1; j++){
      tab_vois[0] = frame_gray.at<uchar>(i-1, j-1);
      tab_vois[1] = frame_gray.at<uchar>(i, j-1);
      tab_vois[2] = frame_gray.at<uchar>(i+1, j-1);
      tab_vois[3] = frame_gray.at<uchar>(i-1, j);
      tab_vois[4] = frame_gray.at<uchar>(i, j);
      tab_vois[5] = frame_gray.at<uchar>(i+1, j);
      tab_vois[6] = frame_gray.at<uchar>(i-1, j+1);
      tab_vois[7] = frame_gray.at<uchar>(i, j+1);
      tab_vois[8] = frame_gray.at<uchar>(i+1, j+1);

      qsort(tab_vois,  sizeof(tab_vois)/sizeof(*tab_vois), sizeof(*tab_vois), compare_function);

      frame1.at<uchar>(i, j) = tab_vois[4];

    }
  }
}
/*
 *
 *--------------- MAIN FUNCTION ---------------
 *
 */
int main () {
//----------------------------------------------
// Video acquisition - opening
//----------------------------------------------
  VideoCapture cap(0); // le numéro 0 indique le point d'accès à la caméra 0 => /dev/video0
  if(!cap.isOpened()){
    cout << "Errore"; return -1;
  }

//----------------------------------------------
// Déclaration des variables - imagesize
// Mat - structure contenant l'image 2D niveau de gris
// Mat3b - structure contenant l'image 2D en couleur (trois cannaux)
//
  Mat3b frame; // couleur
  Mat frame1; // niveau de gris 
  Mat frame_gray; // niveau de gris 
  Mat grad_x;
  Mat grad_y;
  Mat abs_grad_y;
  Mat abs_grad_x;
  Mat grad;

// variable contenant les paramètres des images ou d'éxécution  
  int ddepth = CV_16S;
  int scale = 1;
  int delta = 0;	
  unsigned char key = '0';

 #define PROFILE
  
#ifdef PROFILE
// profiling / instrumentation libraries
#include <time.h>
#include <sys/time.h>
#endif
  
//----------------------------------------------------
// Création des fenêtres pour affichage des résultats
// vous pouvez ne pas les utiliser ou ajouter selon ces exemple
// 
  cvNamedWindow("Video input", WINDOW_AUTOSIZE);
  cvNamedWindow("Video gray levels", WINDOW_AUTOSIZE);
  cvNamedWindow("Video Mediane", WINDOW_AUTOSIZE);
  cvNamedWindow("Video Edge detection", WINDOW_AUTOSIZE);
// placement arbitraire des  fenêtre sur écran 
// sinon les fenêtres sont superposée l'une sur l'autre
  cvMoveWindow("Video input", 10, 30);
  cvMoveWindow("Video gray levels", 800, 30);
  cvMoveWindow("Video Mediane", 10, 500);
  cvMoveWindow("Video Edge detection", 800, 500);
  
  
// --------------------------------------------------
// boucle infinie pour traiter la séquence vidéo  
//
  int somme_median = 0;
  int somme_sobel = 0;
  int nb_frames = 1;

  while(key!='q'){
    printf("---FRAME %d---\n", nb_frames);
    // acquisition d'une trame video - librairie OpenCV
      cap.read(frame);
    //conversion en niveau de gris - librairie OpenCV
      cvtColor(frame, frame_gray, CV_BGR2GRAY);

  	
     // image smoothing by median blur
     //

   

   int n = 17;
   int k = 1; 
   #ifdef PROFILE
   struct timeval start, end;
  //  for (k;k<n;k+=2)
  // { 
   gettimeofday(&start, NULL);
   #endif
       //medianBlur(frame_gray, frame1, k);
       frame1 = frame_gray.clone();
       median(frame_gray, frame1);
   #ifdef PROFILE
   gettimeofday(&end, NULL);
   double e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
   double s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
   somme_median += e-s;
   printf("mediane : %d\n", somme_median/nb_frames);

  //}
   #endif
    
    gettimeofday(&start, NULL);
  	// ------------------------------------------------
  	// calcul du gradient- librairie OpenCV
      /// Gradient Y
   //    Sobel( frame1, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
  	// /// absolute value
   //    convertScaleAbs( grad_x, abs_grad_x );
   //    /// Gradient Y
   //    Sobel( frame1, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
  	// /// absolute value
   //    convertScaleAbs( grad_y, abs_grad_y );
   //    /// Total Gradient (approximate)
   //    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad ); 	
    grad = frame1.clone();
    sobel(frame1, grad);

    gettimeofday(&end, NULL);
    e = ((double) end.tv_sec * 1000000.0 + (double) end.tv_usec);
    s = ((double) start.tv_sec * 1000000.0 + (double) start.tv_usec);
    somme_sobel += e-s;
    printf("sobel : %d\n", somme_sobel/nb_frames);
      
      // -------------------------------------------------
  	// visualisation
  	// taille d'image réduite pour meuilleure disposition sur écran
      //    resize(frame, frame, Size(), 0.5, 0.5);
      //    resize(frame_gray, frame_gray, Size(), 0.5, 0.5);
      //    resize(grad, grad, Size(), 0.5, 0.5);
      imshow("Video input",frame);
      imshow("Video gray levels",frame_gray);
      imshow("Video Mediane",frame1);    
      imshow("Video Edge detection",grad);  
      
      
      nb_frames++;
      key=waitKey(5);
  }
  printf("----FINAL----\n");
  printf("moyene mediane : %d\n", somme_median/nb_frames);
  printf("moyenne sobel : %d\n", somme_sobel/nb_frames);
}

    
