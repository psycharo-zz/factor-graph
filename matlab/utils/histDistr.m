function histDistr(data, numBins)
%HISTDISTR plot normalized histogram
    [counts,bins] = hist(data, numBins);
    bar(bins, counts / trapz(bins,counts), 'EdgeColor', 'b');
end

