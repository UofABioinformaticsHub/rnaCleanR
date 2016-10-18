#include <Rcpp.h>
using namespace Rcpp;

// [[Rcpp::export]]
List keepRead(int nbSample,IntegerVector startPosAl,IntegerVector endPosAl,IntegerVector groupNamePos,IntegerVector samplePos,IntegerVector startNegAl,IntegerVector endNegAl,IntegerVector groupNameNeg,IntegerVector sampleNeg,IntegerVector keepWinPos,IntegerVector keepWinNeg,int end,int win,int step,double limit){
  int minP = keepWinPos[0];
  int maxP = keepWinPos[keepWinPos.size()-1];
  bool* keepPos = new bool[maxP+1];
  for (int i=0;i<=maxP;i++) keepPos[i]=false;
  for (int i=0;i<keepWinPos.size();i++){
    keepPos[keepWinPos[i]-1]=true;
  }
  int minM = keepWinNeg[0];
  int maxM = keepWinNeg[keepWinNeg.size()-1];
  bool* keepNeg = new bool[maxM+1];
  for (int i=0;i<=maxM;i++) keepNeg[i]=false;
  for (int i=0;i<keepWinNeg.size();i++){
    keepNeg[keepWinNeg[i]-1]=true;
  }
  std::vector<int>* keepReadPos = new std::vector<int>[nbSample];
  std::vector<int>* keepReadNeg = new std::vector<int>[nbSample];
  int countP=0;
  int countM=0;
  for (int a=0;a<startPosAl.size();a++){
    int s = startPosAl[a];
    int e = endPosAl[a];
    int sam = samplePos[a];
    int lim = (e-s+1)*limit;
    s = s + lim;
    e = e - lim;
    int wS=0;
    if (s>win){
      wS=ceil((s-win-1)/(double)step);//first windows that contain fragment i
    }
    int wE=maxP;
    if (e<(1+step*(maxP))){
      wE=floor(e/(double)step);//last window that contain fragment i
    }
    bool bl=false;
    if (wE>=minP && wS<=maxP){
      int i=wS;
      if (i<minP) i=minP;
      int j=wE;
      if (j>maxP) j=maxP;
      bl = keepPos[i];
      i++;
      while (!bl && i<=j){
        bl = keepPos[i];
        i++;
      }
      if (bl){ keepReadPos[sam-1].push_back(groupNamePos[a]);}
    }
  }
  for (int a=0;a<startNegAl.size();a++){
    int s = startNegAl[a];
    int e = endNegAl[a];
    int sam = sampleNeg[a];
    int lim = (e-s+1)*limit;
    s = s + lim;
    e = e - lim;
    int wS=0;
    if (s>win){
      wS=ceil((s-win-1)/(double)step);//first windows that contain fragment i
    }
    int wE=maxM;
    if (e<(1+step*(maxM))){
      wE=floor(e/(double)step);//last window that contain fragment i
    }
    bool bl=false;
    if (wE>=minM && wS<=maxM){
      int i=wS;
      if (i<minM) i=minM;
      int j=wE;
      if (j>maxM) j=maxM;
      bl = keepNeg[i];
      i++;
      while (!bl && i<=j){
        bl = keepNeg[i];
        i++;
      }
      if (bl){ keepReadNeg[sam-1].push_back(groupNameNeg[a]);}
    }
  }
  delete[] keepPos;
  delete[] keepNeg; 
  List lPos(nbSample);
  for (int i=0;i<nbSample;i++){
    lPos[i] = wrap(keepReadPos[i]);
  }
  List lNeg(nbSample);
  for (int i=0;i<nbSample;i++){
    lNeg[i] = wrap(keepReadNeg[i]);
  }
  return List::create(_["Pos"] = lPos, _["Neg"] = lNeg);
  /*return List::create(
    _["Pos"] = wrap(keepReadPos),
    _["Neg"] = wrap(keepReadNeg)
  );*/
}
