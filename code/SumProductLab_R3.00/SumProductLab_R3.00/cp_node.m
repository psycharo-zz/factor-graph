classdef cp_node < factor_node
%CP Superclass of all cp nodes
%   the cpt property & init_cp method are unique for cp nodes
% CP derived from factor_node class provides a message to other
% nodes corresponding according to the cpt.
% In the linklist, put parent first and then followed by child and the
% indexing of the cpt must be in the same order in the linklist.
% For message with n possible outcome, the message are row vector of
% size(1,n).  For binary nodes, the 1st element is probability of false and
% the 2nd element is the probability of true.
%
% Description of methods:
%   INIT_CP         instantiate an evident node
%
% Usage:
% Initialize a cpt for a node m
%     s = cp2(3);             % initialize a 2-port cp node
%     s.setup_link({c1 m});   % parent child
%     %    c,s    P(s|c)
%     cpts(1,1) = 0.5;        % cpt index order the same as the linklist
%     cpts(1,2) = 0.5;
%     cpts(2,1) = 0.9;
%     cpts(2,2) = 0.1;
%     s.init_cp(cpts);

   properties
       % Conditional probability table in the order of the linklist.
       % The cpt for n-port cp node is a n-dimenion matrix.
       % The size of the matrix is the product of all indeces.
       % e.g. the 1st and 2nd index each has 2 possibilities,
       % then the cpt matrix for a two port cp is 2x2.
       cpt
   end

   methods
       function s = cp_node(id)
           s = s@factor_node(id);
           s.cpt = [];
       end
       
       function init_cp(s, cpt)
           % initialize conditional probability table
           s.cpt = cpt;
       end
       
       function error_check(s)
           error_check@factor_node(s);
           if isempty(s.cpt)
               sprintf('Node %0.0f ERROR from error_check: empty cpt !!!\n',s.id)
           end
       end
   end
end 
