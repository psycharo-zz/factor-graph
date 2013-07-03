
function plotSpectrogram(T,F,P,COLOR_AXIS)
    surf(T,F,10*log10(P), 'EdgeColor','none');
    axis tight;
    view(0,90);
    if nargin == 4
        caxis(COLOR_AXIS);
    end
return