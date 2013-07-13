function [X,cols,rows] = getImageData(img)
% Based on work by (C) 2008-2010 Mikael Kuusela.

    cols = size(img,2);
    rows = size(img,1);
    numPixels = cols * rows;
    %Convert image data to proper format
    X = zeros(numPixels,5);
    % TODO: it is probably better to select the largest dimension
    for y = 1:rows
        xv = (0:cols-1)' / (cols-1);
        yv = repmat((y-1) / (cols-1), cols, 1);
        rgbv = double(squeeze(img(y,:,:)))/255;
        n1 = (y - 1) * cols + 1;
        n2 = y * cols;
        X(n1:n2,:) = [xv, yv, rgbv];
    end
    % Scale data to interval [-1,1]
    X = (X - 0.5)*2;
return