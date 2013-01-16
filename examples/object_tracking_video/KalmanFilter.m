classdef KalmanFilter
    %KALMANFILTER the wrapper for kalman filter to use in video example
    
    properties
        nwk;   % nwk
        Y;     % node corresponding to the measurements
        xPrev; % previous state
        xNext; % next state
        H;     % this.H matrix of the filter. TODO: add |matrix| method to MultiplicationNode
        R;     % measurement noise covariance matrix
        W;     % measurement noise node
        equ;   % equality node
        PredictSchedule;
        CorrectSchedule;
    end
    
    properties (Constant)
        NDIM = 2; % the number of dimensions
    end
    
    methods
        
       
        function this = KalmanFilter(InitialLocation, InitialEstimateError, MotionNoise, MeasurementNoise)
            % create a kalman filter with:
            %  InitialLocation - the initial position of the object (2D vector)
            %  InitialEstimateError - variances for the initial position and velocity
            %  MotionNoise - process noise variances
            %  MeasurementNoise - measurement variance
            
            % parameters of the kalman filter
            
            InitialPrecision = zeros(4,4);
            MotionPrecision = zeros(4,4);      
            
            for i = 0:1
                InitialPrecision(2*i+1,2*i+1) = 1. / InitialEstimateError(i+1);
                InitialPrecision(2*i+2,2*i+2) = 1. / InitialEstimateError(i+1);
                MotionPrecision(2*i+1,2*i+1) = 1./MotionNoise(i+1);
                MotionPrecision(2*i+2,2*i+2) = 1./MotionNoise(i+1);
            end
            
            this.R = eye(2,2) * MeasurementNoise;
            MeasurementPrecision = inv(this.R);

            % transition matrix
            transitionMatrix = [1 1 0 0;...     % [x  ]            
                                0 1 0 0;...     % [y  ]
                                0 0 1 1;...     % [Vx]
                                0 0 0 1;...     % [Vy]
                                ];
            % measurement matrix
            measurementMatrix = [ 1 0 0 0;...
                                  0 0 1 0 ];
            
            % 
            this.nwk = ffg.Network;
            
            % X_next = A * X_prev + U
            % Y_next = this.H * X_next + this.W

            % hidden variable, X_{t-1}
            this.xPrev = ffg.EvidenceNode(this.nwk);
            
            % transition matrix node
            A = ffg.MultiplicationNode(this.nwk);
            A.setMatrix(transitionMatrix);      
            add_X_U = ffg.AddNode(this.nwk);
            % process noise
            U = ffg.EvidenceNode(this.nwk);
            % observation, corresponds to a half-edge y
            this.Y = ffg.EvidenceNode(this.nwk);
           
            % hidden variable, X_t
            this.xNext = ffg.EvidenceNode(this.nwk);
            this.equ = ffg.EqualityNode(this.nwk);

            % observation noise, W_t
            this.W = ffg.EvidenceNode(this.nwk);

            % measurement matrix node
            this.H = ffg.MultiplicationNode(this.nwk);
            this.H.setMatrix(measurementMatrix);
                      
            % adding the noise
            add_H_W = ffg.AddNode(this.nwk);

            % Connecting nodes
            this.nwk.addEdge(this.xPrev, A);
            this.nwk.addEdge(A, add_X_U);
            this.nwk.addEdge(U, add_X_U);

            this.nwk.addEdge(add_X_U, this.equ);
            this.nwk.addEdge(this.equ, this.xNext);
            this.nwk.addEdge(this.equ, this.H);

            this.nwk.addEdge(this.H, add_H_W);
            this.nwk.addEdge(this.W, add_H_W);
            this.nwk.addEdge(add_H_W, this.Y);
            
            % initialisation 
            % location
            InitialState = [InitialLocation(1), 0, InitialLocation(2), 0];
            
            this.xPrev.receive(ffg.messages.gaussPrecision(InitialState, InitialPrecision));
            this.xNext.receive(ffg.messages.gaussPrecision(InitialState, InitialPrecision));
            
            % measurement noise
            this.W.receive(ffg.messages.gaussPrecision(zeros(1,2), MeasurementPrecision));
            % process/motion noise
            U.receive(ffg.messages.gaussPrecision(zeros(1,4), MotionPrecision));
            
            this.equ.setMessage(ffg.messages.gaussPrecision(zeros(1,4), zeros(4,4), this.H.id));
            
            this.nwk.setSchedule({U, add_X_U;...
                                  this.W, add_H_W;...
                                  this.xPrev, A;...
                                  A, add_X_U;...
                                  add_X_U, this.equ;...
                                  this.equ, this.xNext});
                              
            this.nwk.makeStep();
            
            
           
            this.PredictSchedule = {this.xPrev, A;...
                                    A, add_X_U;...
                                    add_X_U, this.equ;...
                                    this.equ, this.xNext};
            
            this.CorrectSchedule = {this.xPrev, A;...
                                    A, add_X_U;...
                                    add_X_U, this.equ;...
                                    this.equ, this.xNext;...                
                                    this.Y, add_H_W;...
                                    add_H_W, this.H;...
                                    this.H, this.equ;...
                                    this.equ, this.xNext};
                                
        end
        
        function result = predict(this)
            % predicting the future location of the object
            this.nwk.setSchedule(this.PredictSchedule);
            % setting the current estimate from the previous time slice
            this.xPrev.receive(this.xNext.evidence());
            % emulating zero-precision message
            this.equ.setMessage(ffg.messages.gaussPrecision(zeros(1,4), zeros(4,4), this.H.id));
            % running inference
            this.nwk.makeStep();
            est = this.xNext.evidence().mean;
            result = [est(1), est(3)];
        end
        
        function [z_corr, x_corr, P_corr] = correct(this, msmt)
            % updating the filter state with the measurement
            this.nwk.setSchedule(this.CorrectSchedule);
            % making the observation
            this.Y.receive(ffg.messages.gaussPrecision(msmt, 1e9 * eye(2,2)));
            % running inference
            this.nwk.makeStep();
            x_corr = this.xNext.evidence().mean;
            % TODO: switch to this.Y
            z_corr = [x_corr(1), x_corr(3)];            
            P_corr = inv(this.xNext.evidence().precision);
        end
        
        
        
        
        function result = distance(this, z_matrix)
            % computing the distance (see doc for |distance| method of
            % |vision.KalmanFilter|)
            
            mean = this.xNext.evidence().mean';
            covar = inv(this.xNext.evidence().precision);
            H = this.H.matrix;
            S = H * covar * H' + this.R;
            NTRACKS = size(z_matrix,1);
            result = zeros(1, NTRACKS);
            for i = 1:NTRACKS
                z = z_matrix(i, :)';
                result(i) = (z - H * mean)' / S * (z - H * mean) + log(det(S));
            end
        end
        
    end
    
end

