#include <Rcpp.h>
#include "utils.h"
using namespace Rcpp;

//' @title  Compute strand information of sliding window (verbose version)
//'
//' @description Compute the positive proportion, the normalized value to be tested afterward to decide whether the window is kept or not, the sum of reads, the maximum coverage, and the group of each window. Windows are grouped based on their maximum coverage: by default definition, groups spead from 1 to 4, which correspond to the max coverage respectively in the range "0-10","10-100","100-1000",">1000".
//' This method is used in the method filterOne when we have to filter the input bam files together with plotting the window information.
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
//' Each data frame contains the information of window number, proportion of postive reads, the normalized value calculated from the estimated proportion and error, the sum of reads, the max coverage and the group of max coverage.
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
//' windows <- rnaCleanR::computeWinVerbose(runLength(covPos),runValue(covPos),runLength(covNeg),runValue(covNeg),readLength,len,win,step,minCov,maxCov,logitThreshold)
//' 
//' @export
//' 
// [[Rcpp::export]]


List computeWinVerbose(IntegerVector covPosLen,IntegerVector covPosVal,IntegerVector covNegLen,IntegerVector covNegVal,double readLength,int end,int win,int step,int minCov,int maxCov,double logitThreshold){
  int start=0;
  int preP=0;
  int preM=0;
  int iP=0;
  int iM=0;
  std::vector<int> windowP;
  std::vector<int> windowM;
  std::vector<double> valueP;
  std::vector<double> valueM;
  
  std::vector<double> proporP;
  std::vector<double> proporM;
  std::vector<double> sumP;
  std::vector<double> sumM;
  std::vector<int> maxCP;
  std::vector<int> maxCM;
  std::vector<int> groupP;
  std::vector<int> groupM;
  int c=1;
  int maxCovP=0;
  int maxCovM=0;
  while (start<end){
    int Plus=increaseVal(covPosLen,covPosVal,iP,start,win,preP,maxCovP);//get the number of '+' in the current window
    int Minus=increaseVal(covNegLen,covNegVal,iM,start,win,preM,maxCovM);//get the number of '-' in the current window
    increase(covPosLen,iP,start,end,step,preP);//go to the next window of positive coverage
    increase(covNegLen,iM,start,end,step,preM);//go to the next window of negative coverage
    start+=step;
    if (Plus>0 || Minus>0){
      double estimate = (double)Plus/(Plus+Minus);
      double max = maxCovP;
      if (estimate<=0.5) max = maxCovM;
      if (max>minCov){
        double lTestimate=log(estimate/(1-estimate));//The estimated value of positive proportion upon logistic regression: logit(p), where p=Plus/(Plus+Minus)
        double error = sqrt(readLength/(Plus+Minus)/estimate/(1-estimate));//The standard error of positive proportion upon logistic regression: 1/sqrt(n*p*(1-p)), here n = (Plus+Minus)/readLength
        double value=(lTestimate - logitThreshold)/error;
        if (lTestimate<=0) value=-(lTestimate+logitThreshold)/error;
        if (Plus>Minus || (maxCovP>maxCov && maxCov>0)){
          if (Minus==0 || (maxCovP>maxCov && maxCov>0)) valueP.push_back(1e10);//Just put a big value here so that it will always pass the test to be kept
          else valueP.push_back(value);
          windowP.push_back(c);
          proporP.push_back(estimate);
          sumP.push_back((Plus+Minus)/(double)readLength);
          maxCP.push_back(maxCovP);
          if (max>1000) groupP.push_back(4);
          else if (max>100) groupP.push_back(3);
          else if (max>10) groupP.push_back(2);
          else if (max>0) groupP.push_back(1);
        }
        if (Plus<=Minus || (maxCovM>maxCov && maxCov>0)){
          if (Plus==0 || (maxCovM>maxCov && maxCov>0)) valueM.push_back(1e10);
          else valueM.push_back(value);
          windowM.push_back(c);
          proporM.push_back(estimate);
          sumM.push_back((Plus+Minus)/(double)readLength);
          maxCM.push_back(maxCovM);
          if (max>1000) groupM.push_back(4);
          else if (max>100) groupM.push_back(3);
          else if (max>10) groupM.push_back(2);
          else if (max>0) groupM.push_back(1);
        }
      }
    }
    c++;
  }
  return List::create(
    _["Plus"] = DataFrame::create(_["win"]= windowP, _["value"]= valueP, _["propor"] = proporP, _["sum"] = sumP, _["max"] = maxCP, _["group"] = groupP),
      _["Minus"] = DataFrame::create(_["win"]= windowM, _["value"]= valueM, _["propor"] = proporM, _["sum"] = sumM, _["max"] = maxCM, _["group"] = groupM)
  );
  
  
  
}



