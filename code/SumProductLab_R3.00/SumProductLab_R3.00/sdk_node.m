classdef sdk_node < factor_node
%SDK Sudoku constraint node
%   no two input have the same number

   properties
   end

   methods
       function s = sdk_node(id)
           s = s@factor_node(id);
       end
       
       function msg = factor_fun(s, in_msg, from_id, to_id)
            mat_msg = cell2mat(in_msg);
            size_msg = size(mat_msg,2);
            
            msg = zeros(1,size_msg);

            for i = 1:size_msg
                ACC = 0;
                A = mat_msg;
                A(:,i) = [];        % remove i-th column
                % check for zero rows
                size_A = size(A,2);
                zero_row = 0;
                j = 1;
                scale = 1;
                while (j <= size_A) && (size_A ~= 1)
                    row_zero_count = sum(A(j,:) == 0);
                    if row_zero_count == size_A
                        % row with all zeros
                        zero_row = 1;
                        break;
                    elseif row_zero_count == size_A - 1
                        % row with one non-zero
                        % reduce dimension of A
%                         mat_msg
%                         i
%                         j
%                         A
                        scale = scale*A(j,(A(j,:) > 0));
                        A(:,(A(j,:) > 0)) = [];
                        A(j,:) = [];
                        size_A = size_A - 1;
%                         A
                    else
                        j = j + 1;
                    end
                end
                if zero_row == 1
                    msg(i) = 0;
                    continue;
                end
                if size_A == 1
                    msg(i) = scale*A;
                    continue;
                end
                B = A';
                A_offset = size_A*(0:size_A-1);

                % initialize permutation
                idx = 1:size_A;
                direction = zeros(1,size_A);     % 0 = left; 1 = right
                while(1)
                    % accumulate product terms
                    collected_terms = B(idx + A_offset);
                    if sum(collected_terms == 0) == 0
                        ACC = ACC + prod(collected_terms);
                    end
                    % generate permutation by Johnson Trotter Algorithm
                    mobile_member = 0;
                    for n = 1:size_A
                        % check mobility
                        if ((direction(n) == 0) && (n > 1) && (idx(n-1) < idx(n))) || ...
                                (direction(n) == 1) && (n < size_A) && (idx(n) > idx(n+1))
                            if (mobile_member == 0) || (idx(n) > idx(mobile_member))
                                mobile_member = n;
                            end
                        end
                    end
                    if mobile_member == 0
                        break;
                    end
                    % change direction for all member > mobile_member
                    for n = 1:size_A
                        if idx(n) > idx(mobile_member)
                            direction(n) = 1 - direction(n);
                        end
                    end
                    % swap member
                    tmp_idx = idx(mobile_member);
                    tmp_dir = direction(mobile_member);
                    if direction(mobile_member) == 0
                        idx(mobile_member) = idx(mobile_member-1);
                        idx(mobile_member-1) = tmp_idx;
                        direction(mobile_member) = direction(mobile_member-1);
                        direction(mobile_member-1) = tmp_dir;
                    else
                        idx(mobile_member) = idx(mobile_member+1);
                        idx(mobile_member+1) = tmp_idx;
                        direction(mobile_member) = direction(mobile_member+1);
                        direction(mobile_member+1) = tmp_dir;
                    end
                end
                msg(i) = scale*ACC;
                if isnan(msg(i))
                    disp('not a number');
                end

            % verificaton
%                 ACC_B = 0;
%                 A = mat_msg;
%                 A(:,i) = [];        % remove i-th column
%                 % check for zero rows
%                 size_A = size(A,2);
%                 zero_row = 0;
%                 j = 1;
%                 scale = 1;
%                 while (j <= size_A) && (size_A ~= 1)
%                     row_zero_count = sum(A(j,:) == 0);
%                     if row_zero_count == size_A
%                         % row with all zeros
%                         zero_row = 1;
%                         break;
% %                     elseif row_zero_count == size_A - 1
% %                         % row with one non-zero
% %                         % reduce dimension of A
% %                         scale = scale*A(j,(A(j,:) > 0));
% %                         A(:,(A(j,:) > 0)) = [];
% %                         A(j,:) = [];
% %                         size_A = size_A - 1;
%                     else
%                         j = j + 1;
%                     end
%                 end
%                 if zero_row == 1
%                     msg(i) = 0;
%                     continue;
%                 end
%                 if size_A == 1
%                     msg(i) = scale*A;
%                     continue;
%                 end
%                 B = A';
%                 A_offset = size_A*(0:size_A-1);
% 
%                 % initialize permutation
%                 idx = 1:size_A;
%                 direction = zeros(1,size_A);     % 0 = left; 1 = right
%                 while(1)
%                     % accumulate product terms
%                     collected_terms = B(idx + A_offset);
%                     if sum(collected_terms == 0) == 0
%                         ACC_B = ACC_B + prod(collected_terms);
%                     end
%                     % generate permutation by Johnson Trotter Algorithm
%                     mobile_member = 0;
%                     for n = 1:size_A
%                         % check mobility
%                         if ((direction(n) == 0) && (n > 1) && (idx(n-1) < idx(n))) || ...
%                                 (direction(n) == 1) && (n < size_A) && (idx(n) > idx(n+1))
%                             if (mobile_member == 0) || (idx(n) > idx(mobile_member))
%                                 mobile_member = n;
%                             end
%                         end
%                     end
%                     if mobile_member == 0
%                         break;
%                     end
%                     % change direction for all member > mobile_member
%                     for n = 1:size_A
%                         if idx(n) > idx(mobile_member)
%                             direction(n) = 1 - direction(n);
%                         end
%                     end
%                     % swap member
%                     tmp_idx = idx(mobile_member);
%                     tmp_dir = direction(mobile_member);
%                     if direction(mobile_member) == 0
%                         idx(mobile_member) = idx(mobile_member-1);
%                         idx(mobile_member-1) = tmp_idx;
%                         direction(mobile_member) = direction(mobile_member-1);
%                         direction(mobile_member-1) = tmp_dir;
%                     else
%                         idx(mobile_member) = idx(mobile_member+1);
%                         idx(mobile_member+1) = tmp_idx;
%                         direction(mobile_member) = direction(mobile_member+1);
%                         direction(mobile_member+1) = tmp_dir;
%                     end
%                 end
%                 if abs(ACC_B-msg(i)) > 1e-4
%                     disp(sprintf('failed comparison'))
%                 end

            
            end
            sum_msg = sum(msg);
            if sum_msg == 0
                msg = ones(size(msg));
            end
            msg = msg/sum(msg);
       end
   end
end
