#include <Rcpp.h>
#include "utils.h"
using namespace Rcpp;

//' @title  Compute strand information of sliding window 
//'
//' @description Compute the positive proportion and the value to be tested afterward to decide whether the window is kept or not (this value is calculated from the estimated proportion and error).
//' This method is called by the functions filterOne and filterMulti in the case that we don't need other plotting information from the windows.
//' 
//' @param covPosLen the run length of an Rle object which is the coverage comes from positive reads
//' @param covPosVal the run value of an Rle object which is the coverage comes from positive reads
//' @param covNegLen the run length of an Rle object which is the coverage comes from negative reads
//' @param covNegVal the run value of an Rle object which is the coverage comes from negative reads
//' @param end the last base on the reference chromosome that the sliding window atteints
//' @param readLength the average length of reads
//' @param win the size of the sliding window
//' @param step the step of the sliding window
//' @param minCov if a window has the max coverage smaller than minCov, then it will be rejected regardless its strand proportion.
//' @param maxCov if a window has the max coverage greater than maxCov, then it will be kept regardless its strand proportion. If maxCov=0 then it doesn't have any effect on selecting windows.
//' @param logitThreshold the logit of the threshold
//'
//' @return A list of two data frames Plus and Minus which respectively contains information of positive windows and negative windows. 
//' Each data frame contains the information of window number, proportion of postive reads, and the value to be tested afterward to decide whether the window is kept or not (this value is calculated from the estimated proportion and error).
//' 
//' @seealso filterOne, filterMulti
//' 
//' @examples
//' bamfilein <- system.file("data","s1.chr1.bam",package = "rnaCleanR")
//' alignment <- GenomicAlignments::readGAlignments(bamfilein) 
//' alignmentInChr1 <- alignment[seqnames(alignment)=="1"] 
//' covPos <- alignmentInChr1[strand(alignment)=="+"] %>% GenomicAlignments::coverage() 
//' covNeg <- alignmentInChr1[strand(alignment)=="-"] %>% GenomicAlignments::coverage() 
//' len <- length(covChr)
//' readLength <- 100
//' win <- 1000
//' step <- 100
//' minCov <- 0
//' maxCov <- 0
//' logitThreshold <- binomial()$linkfun(0.7) 
//' windows <- rnaCleanR::computeWin(runLength(covPos),runValue(covPos),runLength(covNeg),runValue(covNeg),readLength,len,win,step,minCov,maxCov,logitThreshold)
//' 
//' @export
//' 
// [[Rcpp::export]]

List computeWin(IntegerVector covPosLen,IntegerVector covPosVal,IntegerVector covNegLen,IntegerVector covNegVal,double readLength,int end,int win,int step,int minCov,int maxCov,double logitThreshold){
  int start=0;
  int preP=0;
  int preM=0;
  int iP=0;
  int iM=0;
  std::vector<int> windowP;
  std::vector<int> windowM;
  std::vector<double> valueP;
  std::vector<double> valueM;
  std::vector<double> proporPos;
  std::vector<double> proporNeg;
  int c=1;
  int maxCovP=0;
  int maxCovN=0;
  while (start<end){
    int Plus=increaseVal(covPosLen,covPosVal,iP,start,win,preP,maxCovP);//get the number of '+' in the current window
    int Minus=increaseVal(covNegLen,covNegVal,iM,start,win,preM,maxCovN);//get the number of '-' in the current window
    increase(covPosLen,iP,start,end,step,preP);//go to the next window of positive coverage
    increase(covNegLen,iM,start,end,step,preM);//go to the next window of negative coverage
    start+=step;
    if (Plus>0 || Minus>0){
      double estimate = (double)Plus/(Plus+Minus);
      double max = maxCovP;
      if (estimate<=0.5) max = maxCovN;
      if (max>minCov){
        double lTestimate=log(estimate/(1-estimate));//The estimated value of positive proportion upon logistic regression: logit(p), where p=Plus/(Plus+Minus
        double error = sqrt(readLength/(Plus+Minus)/estimate/(1-estimate));//The standard error of positive proportion upon logistic regression: 1/sqrt(n*p*(1-p)), here n = (Plus+Minus)/readLength
        double value=(lTestimate - logitThreshold)/error;
        if (lTestimate<=0) value=-(lTestimate+logitThreshold)/error;
        if (Plus>Minus || (maxCovP>maxCov && maxCov>0)){
          if (Minus==0 || (maxCovP>maxCov && maxCov>0)) valueP.push_back(1e10);//Just put a big value here so that it will always pass the test to be kept
          else valueP.push_back(value);
          windowP.push_back(c);
          proporPos.push_back(estimate);
        }
        if (Plus<=Minus || (maxCovN>maxCov & maxCov>0)){
          if (Plus==0 || (maxCovN>maxCov & maxCov>0)) valueM.push_back(1e10);
          else valueM.push_back(value);
          windowM.push_back(c);
          proporNeg.push_back(estimate);
        }
      }
    }
    c++;
  }
  return List::create(
    _["Plus"] = DataFrame::create(_["win"]= windowP, _["value"]= valueP, _["propor"] = proporPos),
      _["Minus"] = DataFrame::create(_["win"]= windowM, _["value"]= valueM, _["propor"] = proporNeg)
  );
}



