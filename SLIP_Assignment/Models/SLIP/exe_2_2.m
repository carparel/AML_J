function exe_2_2(angAtt_selected, k_low, k_up, n_k, dx_low, dx_up, n_dx)
% exe_2_2(angAtt_selected, k_low, k_up, n_k, dx_low, dx_up, n_dx)
% Inputs:
%   angAtt_selected: the angle of attack of your solution (deg)
%   k_low: the lower boundary for k
%   k_up: the upper boundary for k
%   n_k: sample points for K (at least 20 for a good graph!)
%   dx_low: the lower boundary for Vx_0
%   dx_up: the upper boundary for Vx_0
%   n_dx: sample points for Vx_0 (at least 10 for a good graph!)

% EXE-2 Examine the running stability

% *************************************************************************
% This tutorial is based on open-source Gait-Creation Framework published
% by David Remy et. al., 2011. 
% 'A MATLAB Framework For Gait Creation', 2011, C. David Remy (1), Keith
%  Buffinton (2), and Roland Siegwart (1),  International Conference on
%  Intelligent Robots and Systems, September 25-30, San Francisco, USA
% For more information please see
% http://www.asl.ethz.ch/people/cremy/personal/GaitCreation
%
% Analysis and Moldeling of Locomotion Course
% SLIP: Spring Loaded Inverted Pendulum Model
% Assignment-1, Exercise-2-2
% Biorobotics laboratoty, EPFL
% Salman Faraji (salman.faraji@epfl.ch)
% Spring 2014
% *************************************************************************

% The goal in this exercise it to examine the running stability through 
% systematic change of a selection of parameters
initSimulation;

global slope
global p
slope = 0.0; % the tangent of the ground

% Initialize the simulation parameters
p   = SystParamDefinition;


% k-dx map
disp('========================================================================');
disp('This function computes k-dx0 map stability plot for the given system');
fprintf('\n');

% creating the map  
K_sp = linspace(k_low,  k_up,  n_k);
dx   = linspace(dx_low, dx_up, n_dx);

all_nodes  = n_k * n_dx;
all_steps  = zeros(n_k, n_dx);
all_Ksps   = zeros(n_k, n_dx);
all_dx     = zeros(n_k, n_dx);

sim_num = 0;
tic
for i = 1 : n_k

    for j = 1 : n_dx

        % Re-Initialize the simulation parameters
        yIN = ContStateDefinition;
        zIN = DiscStateDefinition;
        p   = SystParamDefinition;

        % select a good value for angle of attack
        p(systParamIndices.angAtt) = (90-angAtt_selected)*pi/180;
        p(systParamIndices.initAngAtt)= (90-angAtt_selected)*pi/180;
        p(systParamIndices.wr) =0.05;
        p(systParamIndices.k)      = K_sp(i);
        yIN(contStateIndices.dx)   = dx(j);

        % A graphical output
        % Note that it must be called whenever p changes (such as angle of attack)
        %graphOUTPUT = SLIP_Model_Graphics_2(p); 

        % Define options for the simulation:
        simOptions.tIN  = 0;  % The simulation will start at t = 0
        simOptions.tMAX = 20;  % The simulation will abort when t reaches 5. 

        % number of steps we want the leg to run and the number of successful steps
        % before falling over
        nr_steps = 20;
        nr_stable_steps = 0;

        % run the simulation for the given number of steps
        for ii = 1:nr_steps
            yIN(contStateIndices.time)=0;
            % run the hybrid dynamics of this system for one step
            %[yOUT, zOUT, tOUT] = HybridDynamics(yIN, zIN, p, graphOUTPUT, simOptions);
            [p, yOUT, zOUT, tOUT] = HybridDynamics(yIN, zIN, p, simOptions);

            % update the initial state and go to the next step
            yIN = yOUT; 
            zIN = zOUT;

            % count the number of successful steps
            nr_stable_steps = nr_stable_steps + 1;

            % check for the simulation quit conditions
            contPoint_y = yIN(contStateIndices.y) - p(systParamIndices.l_0) * cos(p(systParamIndices.angAtt));
            if (contPoint_y < 0)
                %disp('                    ********* Simularion Done:  *******');
                %disp('           ******* HINT: try another set of parameters ******');
                break;
            end

        end % end of simulation loop

        sim_num = sim_num + 1;
        fprintf('\n')    
        disp(['========== Simulation No. : ',num2str(sim_num), ' out of total: ' ,num2str(all_nodes), ' ============']);          
        disp([' ===> k: [',num2str(K_sp(i)), '] V_x: [', num2str(dx(j)), '] ==> n_successful_steps: [',num2str(nr_stable_steps), ']'])         
        disp('===================================================================')

        all_Ksps(i,j)  = K_sp(i); % to dimentialize: *m_0*g/l_0/1000
        all_dx(i,j)    = dx(j);   % to dimentialize: *l_0*sqrt(l_0/g)
        all_steps(i,j) = nr_stable_steps;

    end % end of angAtt loop

end % end of K_sp loop
toc

% plotting
pcolor(all_Ksps, all_dx, all_steps)

xlim([K_sp(1) K_sp(end)]);

xlabel('Spring Stiffness (k)','FontWeight','bold','FontSize',18,...
    'FontName','Times New Roman');
ylabel('Forward Speed (V_x) ','FontWeight','bold','FontSize',18,...
    'FontName','Times New Roman')
colorbar('FontWeight','bold','FontSize',18,...
    'FontName','Times New Roman')
% shg
shading flat
colormap(flipud(gray))
caxis([0 20]);
    
disp('======================================================================================');
