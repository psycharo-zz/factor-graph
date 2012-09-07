function sudoku(clues)
% Sudoku solver by sum-product algorithm
%
% Usage:
% load sudoku_4x4_01
% sudoku(clues)

clc;
sdk_order = size(clues,1);

disp('Clues:');
disp(full(clues))
input('Hit return to proceed ...');
disp('Preparing Sudoku Factor Graph ...');

global loopy
loopy = 1;
% set max. number of loops of iteration
num_loop = 100;

node_count = 1;
col_sdk = {};
row_sdk = {};
% generate sudoku constraint nodes
for i = 1:sdk_order
    col_sdk{i} = sdk_node(node_count);
    node_count = node_count + 1;
end
for i = 1:sdk_order
    row_sdk{i} = sdk_node(node_count);
    node_count = node_count + 1;
end
equ_arry = {};       % equ nodes
for i = 1:sdk_order
    for j = 1:sdk_order
        equ_arry{i,j} = equ_node(node_count);
        node_count = node_count + 1;
    end
end
evident_arry = {};   % evident nodes
for i = 1:sdk_order
    for j = 1:sdk_order
        evident_arry{i,j} = evident_node(node_count);
        node_count = node_count + 1;
    end
end

block_order = sqrt(sdk_order);
block_sdk = {};     % block sdk nodes
for i = 1:block_order
    for j = 1:block_order
        block_sdk{i,j} = sdk_node(node_count);
        node_count = node_count + 1;
    end
end

clues_cnt = 1;
% connect sudoku graph
for i = 1:sdk_order           % row index
    for j = 1:sdk_order       % column index
            % connect nodes
            connect(evident_arry{i,j},equ_arry{i,j});
            connect(equ_arry{i,j},row_sdk{i});
            connect(equ_arry{i,j},col_sdk{j});
            connect(equ_arry{i,j},block_sdk{ceil(i/block_order),ceil(j/block_order)});
            % load clues
            if clues(i,j) > 0;
                init_vector = zeros(1,sdk_order);
                init_vector(clues(i,j)) = 1;
                evident_arry{i,j}.setup_init_msg(init_vector);
            else
                evident_arry{i,j}.setup_init_msg(ones(1,sdk_order));
            end
    end
end

% propagate clues to equ nodes
    for i = 1:sdk_order
        for j = 1:sdk_order
            equ_arry{i,j}.update_node(ones(1,sdk_order)/sdk_order);
        end
    end

% update loop
for k = 1:num_loop
    
    % update equ array with no clues
    for i = 1:sdk_order
        for j = 1:sdk_order
            if clues(i,j) == 0
                equ_arry{i,j}.update_node(ones(1,sdk_order)/sdk_order);
            end
        end
    end
    % update row sdk nodes
    for j = 1:sdk_order
        row_sdk{j}.update_node(ones(1,sdk_order)/sdk_order);
    end
    % update column sdk nodes
    for j = 1:sdk_order
        col_sdk{j}.update_node(ones(1,sdk_order)/sdk_order);
    end
    % update block sdk nodes
    for i = 1:block_order
        for j = 1:block_order
            block_sdk{i,j}.update_node(ones(1,sdk_order)/sdk_order);
        end
    end
    
    % report result in Sudoku matrix
    Sudoku = zeros(sdk_order,sdk_order);
    for i = 1:sdk_order           % row index
        for j = 1:sdk_order       % column index
            if clues(i,j) > 0
                % location with clues
                Sudoku(i,j) = clues(i,j);
            else
                % location with inferred value
                tmp = marginal(equ_arry{i,j},evident_arry{i,j});
                if size(find(max(tmp)==tmp),2) == 1
                    % find one single value
                    Sudoku(i,j) = find(max(tmp)==tmp);
                else
                    % more than one possible values
                    Sudoku(i,j) = 0;
                end
            end
        end
    end
    clc
    disp('Sudoku Matrix:')
    disp(Sudoku)
    % check for termination
    done = 1;
    % check each row
    for i = 1:sdk_order
        for j = 1:sdk_order
            check = find(Sudoku(i,:) == j);
            if size(check,2) ~= 1
                done = 0;
                break;
            end
        end
        if done == 0
            break;
        end
    end
    if done > 0
        % check each column
        for i = 1:sdk_order
            for j = 1:sdk_order
                check = find(Sudoku(:,i) == j);
                if size(check,2) ~= 1
                    done = 0;
                    break;
                end
            end
            if done == 0
                break;
            end
        end
    end
    if done > 0
        % check each block
        for i = i:block_order
            for j = 1:block_order
                for z = 1:sdk_order
                    check = find(Sudoku((1:block_order)+(i-1)*block_order,(1:block_order)+(j-1)*block_order) == z);
                    if size(check,2) ~= 1
                        done = 0;
                        break;
                    end
                end
                if done == 0
                    break;
                end
            end
            if done == 0
                break;
            end
        end
    end
    
    if done > 0
        % passed both row and column check
        disp(sprintf('Converged at loop %d.',k));
        break;
    end

    disp(sprintf('loop %d',k))
end % for k = 1:num_loop
if k == num_loop
    disp(sprintf('Not converged after loop %d !!!',k));
end