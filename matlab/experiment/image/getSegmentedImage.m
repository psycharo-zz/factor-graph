function img = getSegmentedImage(resps, means, rows, cols)
    img = zeros(rows, cols, 3);
    for x = 1:cols
        for y = 1:rows
            n = (y - 1) * cols + x;
            [p,idx] = max(resps(:,n));
            img(y,x,:) = (means(idx,3:5)+1) / 2;
        end
    end
return