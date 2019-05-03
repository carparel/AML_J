function exe_3_3(retractionAng, dx_command,wr)
% exe_3_3(retractionAng, dx_command,wr)
% Inputs:
%   retractionAng: Retraction angle (deg) ... (default: 90)
%   dx_command: forward-speed (m/s) ... (default: 0)
%   wr: constant angular leg velocity (rad/s)
% (EXE-3) System stability

% *************************************************************************
% This tutorial is based on the open-source Gait-Creation Framework published
% by David Remy et. al., 2011. 
% 'A MATLAB Framework For Gait Creation', 2011, C. David Remy (1), Keith
%  Buffinton (2), and Roland Siegwart (1),  International Conference on
%  Intelligent Robots and Systems, September 25-30, San Francisco, USA
% For more information please see
% http://www.asl.ethz.ch/people/cremy/personal/GaitCreation
%
% Analysis and Moldeling of Locomotion Course
% SLIP: Spring Loaded Inverted Pendulum Model
% Assignment-1, Exercise-3
% Biorobotics laboratoty, EPFL
% Jessica Lanini (jessica.lanini@epfl.ch)
% Spring 2017
% *************************************************************************

if nargin<3
    e = 1;
end
if nargin<2
    dx_command = 0;
end
if nargin<1
    retractionAng = 90;
end

% first initialize the simulation
initSimulation;

% The slope of the terrain
global slope
global p
slope = 0.0; % the tangent of the ground

% Initialize the simulation parameters
yIN = ContStateDefinition;
zIN = DiscStateDefinition;
p   = SystParamDefinition;

p(systParamIndices.angAtt) = (90-retractionAng)*pi/180;
yIN(contStateIndices.dx)   = dx_command; 
p(systParamIndices.initAngAtt) = (90-retractionAng)*pi/180;
p(systParamIndices.wr) =wr;
% yIN(contStateIndices.y) = ?;    % vertical position of the main body CoG
% p(systParamIndices.k)   = ?;    % [m_0*g/l_0] linear spring stiffness in the leg
% p(systParamIndices.l_0) = ?;    % [*] uncompressed leg length
% p(systParamIndices.m_0) = ?;    % [m_0] total mass

% Animate the solution and watch it!
disp('=======================================================');
    disp(' HERE IS THE RECORDED DATA AND HOW YOU CAN PLOT THEM');
    disp('=======================================================');
    fprintf('...\n')    
    fprintf('...\n') 
    fprintf('...\n') 

    % reset the simulation time
    simOptions.tIN  = 0; 
    simOptions.tMAX = 5;  % The simulation will abort when t reaches 5.  This prevents an infinite simulation-loop, when the terminal event is missed.

    % A class for recording the state variables
    recOUTPUT = RecordStateCLASS();
    nr_steps = 20;
    
    yINp = yIN; 
    zINp = zIN;
    
    % run the simulation for the number of stable steps
    for i = 1 : nr_steps
        yINp(contStateIndices.time)=0;
        % save the variables as the output to plot afterwards
        [p,yOUT, zOUT, tOUT, recOUTPUT] = HybridDynamics(yINp, zINp, p, recOUTPUT, simOptions);

        % update the initial state and go to the next step
        yINp = yOUT; 
        zINp = zOUT;
        simOptions.tIN  = yINp(contStateIndices.time); 
        simOptions.tMAX = tOUT + 5; 

        % check for the simulation quit conditions
        contPoint_y = yINp(contStateIndices.y) - p(systParamIndices.l_0) * cos(p(systParamIndices.angAtt));
        contPoint_x = yINp(contStateIndices.x) + p(systParamIndices.l_0) * sin(p(systParamIndices.angAtt));
        if (contPoint_y < 0+terrain_prof(contPoint_x))
            disp('********* Simularion QUIT: Running Failure *******');
            disp('******* HINT: try another set of parameters ******');
            break;
        end

    end

    simRES = recOUTPUT.retrieve();
%     figure('Name','SLIP model: dx and y of forward hopping','WindowStyle','docked')
%     grid on; 
%     hold on; 
%     box on;

    % Define which states are plotted:
figure   
plot(simRES.continuousStates(3,:),'r','LineWidth', 2)
    %axis([0, 10, 0, 3])
    
hold on
yIN = ContStateDefinition;
zIN = DiscStateDefinition;
p   = SystParamDefinition;

p(systParamIndices.angAtt) = (90-retractionAng)*pi/180;
yIN(contStateIndices.dx)   = dx_command; 
p(systParamIndices.initAngAtt) = (90-retractionAng)*pi/180;
p(systParamIndices.wr) =0;
disp('=======================================================');
    disp(' HERE IS THE RECORDED DATA AND HOW YOU CAN PLOT THEM');
    disp('=======================================================');
    fprintf('...\n')    
    fprintf('...\n') 
    fprintf('...\n') 

    % reset the simulation time
    simOptions.tIN  = 0; 
    simOptions.tMAX = 5;  % The simulation will abort when t reaches 5.  This prevents an infinite simulation-loop, when the terminal event is missed.

    % A class for recording the state variables
    recOUTPUT = RecordStateCLASS();
    nr_steps = 20;
    
    yINp = yIN; 
    zINp = zIN;
    
    % run the simulation for the number of stable steps
    for i = 1 : nr_steps
        yINp(contStateIndices.time)=0;
        % save the variables as the output to plot afterwards
        [p,yOUT, zOUT, tOUT, recOUTPUT] = HybridDynamics(yINp, zINp, p, recOUTPUT, simOptions);

        % update the initial state and go to the next step
        yINp = yOUT; 
        zINp = zOUT;
        simOptions.tIN  = yINp(contStateIndices.time); 
        simOptions.tMAX = tOUT + 5; 

        % check for the simulation quit conditions
        contPoint_y = yINp(contStateIndices.y) - p(systParamIndices.l_0) * cos(p(systParamIndices.angAtt));
        contPoint_x = yINp(contStateIndices.x) + p(systParamIndices.l_0) * sin(p(systParamIndices.angAtt));
        if (contPoint_y < 0+terrain_prof(contPoint_x))
            disp('********* Simularion QUIT: Running Failure *******');
            disp('******* HINT: try another set of parameters ******');
            break;
        end

    end

simRES = recOUTPUT.retrieve(); 
plot(simRES.continuousStates(3,:),'k','LineWidth', 2)
    
title('Effect of leg retraction')
legend('with leg retraction','without leg retraction')
ylabel('CoM vertical trajectory')
xlabel('time steps')
end

