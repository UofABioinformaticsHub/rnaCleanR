% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/getWinOverlapGRanges.R
\name{getWinOverlapGRanges}
\alias{getWinOverlapGRanges}
\title{Get the sliding windows that overlap a GRanges object}
\usage{
getWinOverlapGRanges(x, seqInfo, winWidth = 1000L, winStep = 100L,
  nbOverlapBases = 1)
}
\arguments{
\item{x}{a GRanges object, which defines the coordinates of 
the ranges in the reference genome that all reads mapped to those ranges
must be kept by the filtering method \code{filterDNA}.}

\item{seqInfo}{a data frame that contains some key information of the 
sequences}

\item{winWidth}{the length of the sliding window, 1000 by default.}

\item{winStep}{the step length to sliding the window, 100 by default.}

\item{nbOverlapBases}{a window is considered to overlap with a range of 
\code{x} if it overlaps with at least \code{nbOverlapBases} bases.}
}
\value{
A list of two logical vectors (for positive and negative strand) 
defining which windows that overlap with the given GRanges object.
}
\description{
Get the sliding windows that overlap a GRanges object.
}
\details{
This finds the windows that overlaps the positive/negative strand of a 
GRanges object. The GRanges object, which is \code{mustKeepRanges} in the 
\code{filterDNA} method, defines the coordinates of the ranges in the 
reference genome that all reads mapped to those ranges must be kept by the 
filtering method \code{filterDNA}. 
This method makes use of the method \code{getWinOverlapEachIRange} by 
pretending each given range as the range of a read. Since the widths of 
\code{x} are not necessarily the same (as normal read lengths), we
use \code{nbOverlapBases} to specify the minimum number of bases that a 
window should overlap with a range of \code{x}, instead of using proprotion 
as \code{readProp} in \code{getWinOverlapEachIRange}.
}
\examples{
library(GenomicRanges)
x <- GRanges(seqnames = "10",ranges = IRanges(start = c(10000,15000),
end=c(20000,30000)),strand = c("+","-"))
seqInfo <- data.frame("Sequence"=10,"FirstBaseInPart"=1)
getWinOverlapGRanges(x,seqInfo)
seqInfo <- data.frame("Sequence"=10,"FirstBaseInPart"=10000000)
getWinOverlapGRanges(x,seqInfo)
}
