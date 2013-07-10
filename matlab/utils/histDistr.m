function histDistr(data, numBins)
%HISTDISTR plot normalized histogram
    [counts,bins] = hist(data, numBins);
    bar(bins, counts / trapz(bins,counts), 'LineStyle', 'none', 'BarWidth', 1.0, 'FaceColor', [0.5,0.5,0.5]);
end

